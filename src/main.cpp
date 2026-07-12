#include <Arduino.h>
#include <Wire.h>
#include <AP33772S.h>
#include <Adafruit_INA260.h>
#include <Adafruit_MAX1704X.h>
#include <EEPROM.h>
#include <LittleFS.h>

AP33772S usbpd;             // defaults to Wire (I2C0)
Adafruit_INA260 ina260;     // output-bus monitor @ 0x40
Adafruit_MAX17048 maxlipo;  // battery fuel gauge @ 0x36
static bool battOk = false; // fuel gauge present

const int PPS_TARGET_MV = 9000; // request 9 V
const int PPS_LIMIT_MA = 2000;  // 2 A limit
int ppsIdx = -1;

#define PIN_CHG_STATE 6 // IO6: charger STAT read-back
#define PIN_CTL 7       // IO7: STAT node bias control
#define PIN_PD_INT 25   // IO25: AP33772S INT (active-HIGH, push-pull)

// ---- HMI control state (panel -> RP) ----
volatile uint16_t reqMV = PPS_TARGET_MV; // reg 0x0020 (next milestone)
volatile uint16_t limMA = PPS_LIMIT_MA;  // reg 0x0021 (next milestone)
volatile bool outputOn = false;          // reg 0x0022 - default OFF for safety
volatile bool g_ocpLatched = false;      // OCP tripped -> latched off until user re-arms
static uint16_t g_arcTgtMV = 0;          // latest measured mV, fed to the arc easer
volatile int pendingSel = -1;            // reg 0x0023 list position, applied in loop()

// ---- persisted settings (flash via EEPROM emulation) ----
#define SET_MAGIC 0xCB05
struct Settings
{
  uint16_t magic;
  uint8_t bootLastUsed; // reg 0x0031: 0=Off at boot, 1=restore Last used
  uint8_t autoArm;      // reg 0x0032: 0/1 auto turn output on after apply
  uint8_t lastOutputOn; // remembered for "Last used" boot restore
  int16_t lastSel;      // remembered profile position for "Last used"
  uint16_t lastMV;      // reqMV at last apply (PPS/AVS voltage restore)
  uint16_t lastMA;      // limMA at last apply
  uint8_t bright;       // reg 0x0030: panel brightness % (10..100)
  uint32_t lifeCWh;     // lifetime energy odometer, centi-Wh (0x003A/0x003B)
  uint8_t theme;        // reg 0x0039: 0=dark, 1=light
};
Settings g_set;
static bool g_bootRestore = false; // pending "Last used" rail restore at boot
static int g_bootRestoreOut = -1;  // output state to force after that restore
static int g_activeSel = -1;       // active list position (-1 none) -> reg 0x0017 highlight
static bool g_brightDirty = false;
static uint32_t g_brightT = 0;
static uint64_t g_sessE_uWh = 0, g_sessQ_uAh = 0, g_lifeE_uWh = 0; // µWh / µAh accumulators
static uint32_t g_sessMs = 0, g_eLastUs = 0, g_lifeSaveT = 0;
#define LIFE_FILE "/life.bin"
static uint64_t g_lifeSaved_uWh = 0; // last odometer value written to flash
static uint32_t g_lifeForceT = 0;    // periodic force-commit timer
static bool g_eRunning = false;

#define HMI Serial2 // UART1: IO8=TX, IO9=RX -> TR660

static void writeReg(uint16_t addr, uint16_t val)
{
  uint8_t f[8] = {0x5A, 0xA5, 0x05, 0x82,
                  (uint8_t)(addr >> 8), (uint8_t)addr,
                  (uint8_t)(val >> 8), (uint8_t)val};
  HMI.write(f, 8);
}

static void writeRegs(uint16_t addr, const uint16_t *vals, uint8_t n)
{
  uint8_t f[64];
  f[0] = 0x5A;
  f[1] = 0xA5;
  f[2] = (uint8_t)(n * 2 + 3);
  f[3] = 0x82;
  f[4] = (uint8_t)(addr >> 8);
  f[5] = (uint8_t)addr;
  for (uint8_t i = 0; i < n; i++)
  {
    f[6 + i * 2] = (uint8_t)(vals[i] >> 8);
    f[7 + i * 2] = (uint8_t)vals[i];
  }
  HMI.write(f, 6 + n * 2);
}

static uint32_t lastSig = 1; /* not 0: forces a clear on first unplugged cycle */

// list position -> real PDO (charger-agnostic; rebuilt every sendProfileList)
struct Slot
{
  uint8_t pdoIndex, type;
  uint16_t vmin, vmax, imax;
};
static Slot g_slots[13];
static uint8_t g_slotN = 0;
static int activePdoIdx = -1;         // applied PDO (1-based), -1 = none
static uint8_t activeType = 0;        // 0 FIX, 1 PPS, 2 AVS, 3 EPR
static bool g_prevSource = false;     // source-present edge detect
static volatile bool g_pdInt = false; // AP33772S INT pending (set in ISR)
static void pdIntISR() { g_pdInt = true; }

// Arm AP33772S hardware OCP at the given trip current (50 mA/LSB). Re-armed on every
// apply: limMA changes, and PD renegotiation can reset chip config.
static void armOCP(uint16_t ocpMA)
{
  usbpd.setOCPTHR(ocpMA);  // trip threshold, 50 mA/LSB
  usbpd.setConfig(OCP_EN); // enable OCP -> chip opens VOUT on trip
  usbpd.setMask(OCP_MSK);  // route OCP to INT so firmware can latch
}
static volatile bool g_outAttach = false; // re-assert output after a (re)attach


// Read AP33772S 2-byte STATUS (clear-on-read): [0]=nego events, [1]=protect events.
// Called from INT and polled, so OCP is caught even if it never asserts INT.
static void serviceStatus()
{
  Wire.beginTransmission(0x52);
  Wire.write(0x01);
  uint8_t st = 0; // STATUS is 1 byte (bits mirror MASK: OCP = bit5)
  if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x52, 1) == 1)
    st = Wire.read();
  if (st) Serial.printf("PDSTAT=%02X\n", st); // TEMP: capture idle vs during OC
  if (st & 0x07) // (re)negotiated
  {
    g_outAttach = true;
    lastSig = 0xFFFFFFFF;
  }
  if (st & 0x20) // OCP (STATUS bit5): chip only flags -> cut VOUT + latch off
  {
    outputOn = false;
    g_ocpLatched = true;
    usbpd.setOutput(0);
    writeReg(0x0016, 0);
    writeReg(0x001F, 1);
  }
}

// Read source PDOs over I2C and push the real list to the HMI
static void sendProfileList()
{
  uint8_t raw[26];
  Wire.beginTransmission(0x52); // AP33772S
  Wire.write(0x20);             // CMD_SRCPDO
  uint16_t zero = 0, rdyz = 1;
  if (Wire.endTransmission(false) != 0)
  { // no source / bus error -> clear list
    static uint8_t clrTries = 0;
    if (lastSig != 0)
    {
      clrTries = 0;
    }
    writeRegs(0x0100, &zero, 1);
    writeRegs(0x0101, &rdyz, 1);
    lastSig = 0;
    g_slotN = 0;
    g_prevSource = false;
    return;
  }
  if (Wire.requestFrom(0x52, 26) < 26)
  {
    static uint8_t clrTries = 0;
    if (lastSig != 0)
    {
      clrTries = 0;
    }
    writeRegs(0x0100, &zero, 1);
    writeRegs(0x0101, &rdyz, 1);
    lastSig = 0;
    g_prevSource = false;
    return;
  }
  for (uint8_t i = 0; i < 26; i++)
    raw[i] = Wire.read();

  uint16_t rows[13][4];
  uint16_t n = 0;

  for (uint8_t idx = 0; idx < 13; idx++)
  {
    uint8_t b0 = raw[idx * 2], b1 = raw[idx * 2 + 1];
    if (b0 == 0 && b1 == 0)
      continue; // empty slot

    uint16_t pdo = b0 | (b1 << 8);
    uint16_t vField = pdo & 0xFF;         // voltage_max field
    uint8_t cField = (pdo >> 10) & 0xF;   // current_max field
    uint8_t typeBit = (pdo >> 14) & 1;    // 0=fixed, 1=PPS/AVS
    uint8_t vminField = (pdo >> 8) & 0x3; // voltage_min field (1=floor 3.3V/15V, 2=floor up to 5V/20V)
    bool isEPR = (idx >= 7);              // PDO 8..13 are EPR

    uint16_t type, vmin, vmax;
    if (!isEPR)
    { // SPR PDO 1..7 (100 mV units)
      if (typeBit == 0)
      {
        type = 0;
        vmin = vmax = vField * 100;
      } // FIX
      else
      {
        type = 1;
        vmin = (vminField == 2) ? 5000 : 3300; // field 2: real min is >3.3V (up to 5V)
        vmax = vField * 100;
      } // PPS
    }
    else
    { // EPR PDO 8..13 (200 mV units)
      if (typeBit == 0)
      {
        type = 3;
        vmin = vmax = vField * 200;
      } // EPR fixed
      else
      {
        type = 2;
        vmin = (vminField == 2) ? 20000 : 15000; // field 2: real min is >15V (up to 20V)
        vmax = vField * 200;
      } // AVS
    }
    uint16_t imax = (cField == 15)  ? 5000
                    : (cField == 0) ? 1240
                                    : (uint16_t)(1000 + cField * 250);

    rows[n][0] = type;
    rows[n][1] = vmin;
    rows[n][2] = vmax;
    rows[n][3] = imax;
    g_slots[n] = {(uint8_t)(idx + 1), (uint8_t)type, vmin, vmax, imax}; // idx+1 = real 1-based PDO
    n++;
  }
  g_slotN = n;
  if (n > 0 && !g_prevSource)
    g_outAttach = true; // source just (re)appeared -> re-assert output state
  g_prevSource = (n > 0);
  // signature of the list; skip resend (and HMI re-render) if unchanged
  uint32_t sig = n * 2654435761u;
  for (uint16_t i = 0; i < n; i++)
    for (uint8_t k = 0; k < 4; k++)
      sig = sig * 31u + rows[i][k];
  if (sig == lastSig)
    return;
  lastSig = sig;

  writeRegs(0x0100, &n, 1);
  for (uint16_t i = 0; i < n; i++)
  {
    writeRegs(0x0110 + i * 4, rows[i], 4);
    delay(5);
  }
  uint16_t rdy = 1;
  writeRegs(0x0101, &rdy, 1);
}

static void saveSettings()
{
  g_set.magic = SET_MAGIC;
  EEPROM.put(0, g_set);
  EEPROM.commit();
}

static void loadSettings()
{
  EEPROM.begin(256);
  EEPROM.get(0, g_set);
  if (g_set.magic != SET_MAGIC)
  { // first boot or invalid -> safe defaults
    g_set.magic = SET_MAGIC;
    g_set.bootLastUsed = 0; // Off at boot (safety)
    g_set.autoArm = 1;      // keep today's behavior: apply auto-arms
    g_set.lastOutputOn = 0;
    g_set.lastSel = -1;
    g_set.lastMV = PPS_TARGET_MV;
    g_set.lastMA = PPS_LIMIT_MA;
    g_set.bright = 100;
    g_set.lifeCWh = 0;
    g_set.theme = 0;
    saveSettings();
  }
  bool fsOk = LittleFS.begin();
  Serial.printf("LittleFS mount: %s\n", fsOk ? "OK" : "FAIL");
  {
    File f = LittleFS.open(LIFE_FILE, "r");
    Serial.printf("life.bin: %s size=%u\n", f ? "found" : "absent", f ? (unsigned)f.size() : 0);
    if (f && f.size() == sizeof(g_lifeE_uWh))
      f.read((uint8_t *)&g_lifeE_uWh, sizeof(g_lifeE_uWh)); // authoritative
    else
      g_lifeE_uWh = (uint64_t)g_set.lifeCWh * 10000ULL; // migrate from EEPROM once
    if (f)
      f.close();
    g_lifeSaved_uWh = g_lifeE_uWh;
  }
  Serial.printf("Settings loaded: magic=%04X boot=%u autoArm=%u\n",
                g_set.magic, g_set.bootLastUsed, g_set.autoArm);
}

static void activeProfileInfo(uint16_t *type, uint16_t *mV)
{
  if (g_activeSel < 0 || g_activeSel >= g_slotN)
  {
    *type = 0;
    *mV = 0;
    return;
  }
  Slot &s = g_slots[g_activeSel];
  *type = (uint16_t)(s.type + 1);                      // 1=Fixed,2=PPS,3=AVS,4=EPR
  *mV = (s.type == 1 || s.type == 2) ? reqMV : s.vmin; // PPS/AVS: requested; else PDO voltage
}

// Battery presence — deterministic, charger-STAT only. The full-cell vs no-cell
// termination plateau is physically indistinguishable on +BATT, so we don't try:
// "complete" (00) and High-Z (01) BOTH map to NO BATTERY. Only sustained charging
// (11) — held for minutes by a real cell, never by the ~1.4us no-cell cap-recharge
// blips — counts as present. A cell reverts to "no battery" once it tops off.
#define CHG_DEBOUNCE 2 // consecutive charging reads before latching present

static uint8_t g_chg_run = 0; // consecutive chg==1 samples

static bool batteryPresent(uint8_t chg)
{
  if (chg == 1)
  {
    if (g_chg_run < 0xFF)
      g_chg_run++;
  }
  else
    g_chg_run = 0;
  return g_chg_run >= CHG_DEBOUNCE;
}

// SoC from resting Li-ion voltage (MAX17048 ModelGauge unusable: CELL on +BATT).
// Piecewise-linear over a typical single-cell discharge curve.
static uint8_t socFromVoltage(uint16_t mv)
{
  const uint16_t v[] = {3300, 3500, 3600, 3700, 3750, 3800, 3900, 4000, 4100, 4200};
  const uint8_t p[] = {0, 5, 15, 40, 55, 65, 80, 90, 97, 100};
  if (mv <= v[0])
    return 0;
  if (mv >= v[9])
    return 100;
  for (uint8_t i = 1; i < 10; i++)
    if (mv < v[i])
      return p[i - 1] + (uint32_t)(mv - v[i - 1]) * (p[i] - p[i - 1]) / (v[i] - v[i - 1]);
  return 100;
}

// Read CHG_STATE at both CTL polarities -> 0=no battery, 1=charging, 2=complete
static uint8_t readChargeState()
{
  digitalWrite(PIN_CTL, LOW);
  delayMicroseconds(50);
  uint8_t lo = digitalRead(PIN_CHG_STATE);
  digitalWrite(PIN_CTL, HIGH);
  delayMicroseconds(50);
  uint8_t hi = digitalRead(PIN_CHG_STATE);
  Serial.printf("CTL=LOW read=%u  CTL=HIGH read=%u\n", lo, hi);
  if (lo == 1 && hi == 1)
    return 1; // 11 = charging
  if (lo == 0 && hi == 0)
    return 2; // 00 = complete
  return 0;   // 01 = High-Z (no battery / done)
}

static void pushSession()
{
  uint32_t mWh = (uint32_t)(g_sessE_uWh / 1000ULL); // µWh -> mWh (0.001 Wh)
  uint32_t mAh = (uint32_t)(g_sessQ_uAh / 1000ULL); // µAh -> mAh (0.001 Ah)
  uint32_t s = g_sessMs / 1000UL;
  writeReg(0x0013, (uint16_t)(mWh & 0xFFFF)); // energy mWh, low 16
  writeReg(0x0014, (uint16_t)(mWh >> 16));    // energy mWh, high 16
  writeReg(0x0015, mAh > 65535 ? 65535 : (uint16_t)mAh);
  writeReg(0x0018, s > 65535 ? 65535 : (uint16_t)s);
}

static void lifeWriteFile() // LittleFS wear-levels across sectors; power-loss safe
{
  File f = LittleFS.open(LIFE_FILE, "w");
  if (!f)
  {
    Serial.println("lifeWriteFile: open FAIL");
    return;
  }
  size_t n = f.write((const uint8_t *)&g_lifeE_uWh, sizeof(g_lifeE_uWh));
  f.close();
  Serial.printf("lifeWriteFile: wrote %u B, %llu uWh\n", (unsigned)n, g_lifeE_uWh);
  g_lifeSaved_uWh = g_lifeE_uWh;
}

// Integrate measured power/current over real dt; accumulate only while output is on.
static void energyAccumulate(uint32_t now_ms, uint32_t mW, uint16_t mA, bool good)
{
  uint32_t us = micros();
  if (outputOn && good)
  {
    if (!g_eRunning)
    {
      g_eLastUs = us;
      g_eRunning = true;
    } // start interval cleanly
    uint32_t dt = us - g_eLastUs; // unsigned: wrap-safe
    if (dt > 2000000UL)
      dt = 0;                                     // >2s gap: stall/anomaly -> no phantom energy
    uint64_t dE = (uint64_t)mW * dt / 3600000ULL; // µWh
    g_sessE_uWh += dE;
    g_lifeE_uWh += dE;
    g_sessQ_uAh += (uint64_t)mA * dt / 3600000ULL; // µAh
    g_sessMs += dt / 1000;
    g_eLastUs = us;
  }
  else
  {
    g_eRunning = false; // freeze when off
  }
  pushSession();
  if (now_ms - g_lifeForceT >= 20000UL) // commit every 20 s if value changed
  {
    g_lifeForceT = now_ms;
    if (g_lifeE_uWh != g_lifeSaved_uWh)
      lifeWriteFile();
  }
}

// Apply one decoded control register from the panel
static void applyControl(uint16_t addr, uint16_t val)
{
  switch (addr)
  {
  case 0x0020:
    reqMV = val;
    break; // requested voltage (mV)
  case 0x0021:
    limMA = (val > 4999) ? 4999 : val; // 5000 maps to current code 16, which the sink rejects
    break;                             // current limit (mA)
  case 0x0022:
    outputOn = (val != 0);
    if (val) // manual re-arm clears the OCP latch + dismisses popups
    {
      g_ocpLatched = false;
      writeReg(0x001F, 0);
    }
    break; // output enable
  case 0x0023:
    if (val < g_slotN)
      pendingSel = (int)val; // selected position, applied in loop()
    break;
  case 0x0024:
    lastSig = 0xFFFFFFFF;                                                  // panel opened view2 -> force a fresh list push now
    writeReg(0x0017, (g_activeSel >= 0) ? (uint16_t)g_activeSel : 0xFFFF); // active rail -> highlight
    sendProfileList();
    break;
  case 0x0025: // session trip reset from panel (view1 ↺)
    g_sessE_uWh = 0;
    g_sessQ_uAh = 0;
    g_sessMs = 0;
    pushSession();
    break;
  case 0x0031:
    g_set.bootLastUsed = (val != 0); // boot output state: 0=Off, 1=Last used
    saveSettings();
    break;
  case 0x0032:
    g_set.autoArm = (val != 0); // auto-arm output after apply
    saveSettings();
    break;
  case 0x0033: // panel entered view4 / HMI ready -> push stored settings back for display
    writeReg(0x0031, g_set.bootLastUsed);
    writeReg(0x0032, g_set.autoArm);
    writeReg(0x0039, g_set.theme);
    writeReg(0x0016, outputOn ? 1 : 0); /* real output state -> view1 toggle reflects arm on boot */
    {                                   /* re-push odometer so view4's entry-reset labels repaint */
      uint32_t wh = (uint32_t)(g_lifeE_uWh / 1000000ULL);
      if (wh > 9999999UL)
        wh = 9999999UL;
      writeReg(0x003A, (uint16_t)(wh >> 16));
      writeReg(0x003B, (uint16_t)(wh & 0xFFFF));
    }
    break;
  case 0x0039: // theme from panel (0=dark,1=light)
    if (g_set.theme != (uint8_t)(val ? 1 : 0))
    {
      g_set.theme = val ? 1 : 0;
      saveSettings();
    }
    break;
  case 0x0030:
  { // brightness % (10..100) from panel; persist debounced
    uint16_t b = (val < 10) ? 10 : (val > 100 ? 100 : val);
    if (b != g_set.bright)
    {
      g_set.bright = (uint8_t)b;
      g_brightDirty = true;
      g_brightT = millis();
    }
    break;
  }
  }
}

// Decode one validated frame. f -> CMD byte; len = frame LEN field.
static void handleFrame(const uint8_t *f, uint8_t len)
{
  uint8_t cmd = f[0];
  uint16_t addr = ((uint16_t)f[1] << 8) | f[2];
  const uint8_t *d;
  uint8_t nregs;
  if (cmd == 0x83)
  { // grf_reg_com_send(): [reglen][data...]
    nregs = f[3];
    d = &f[4];
  }
  else if (cmd == 0x82)
  { // raw write: [data...]
    nregs = (len - 3) / 2;
    d = &f[3];
  }
  else
  {
    return;
  }
  for (uint8_t i = 0; i < nregs; i++)
    applyControl(addr + i, ((uint16_t)d[i * 2] << 8) | d[i * 2 + 1]);
}

// Drain the HMI UART and parse 5A A5 frames (call every loop)
static void pollHMI()
{
  static uint8_t buf[64];
  static uint8_t idx = 0;
  static uint16_t need = 0;
  while (HMI.available())
  {
    uint8_t b = HMI.read();
    if (idx == 0)
    {
      if (b == 0x5A)
        buf[idx++] = b;
    }
    else if (idx == 1)
    {
      if (b == 0xA5)
        buf[idx++] = b;
      else
        idx = (b == 0x5A) ? 1 : 0;
    }
    else if (idx == 2)
    {
      buf[idx++] = b;
      need = 3 + b;
      if (need > sizeof(buf))
        idx = need = 0;
    }
    else
    {
      buf[idx++] = b;
      if (idx >= need)
      {
        handleFrame(&buf[3], buf[2]);
        idx = need = 0;
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 4000)
  {
  } // wait for USB-CDC
  Serial.println("hi");
  loadSettings(); // restore persisted settings from flash
  Serial.println("hi2");
  Wire.setSDA(20); // IO20
  Wire.setSCL(21); // IO21
  Wire.begin();

  pinMode(PIN_CHG_STATE, INPUT);
  pinMode(PIN_CTL, OUTPUT);
  pinMode(PIN_PD_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_PD_INT), pdIntISR, RISING);

  HMI.begin(115200); // screen link

  delay(1000);   // let charger negotiation settle (lib recommends >500ms)
  usbpd.begin(); // reads source PDOs over I2C

  Serial.println("\nC-Bench: charger PDOs");
  usbpd.displayProfiles();

  if (!ina260.begin(0x40))
  {
    Serial.println("INA260 not found!");
    while (1)
      delay(100);
  }

  battOk = maxlipo.begin(&Wire); // fuel gauge optional; absent = no battery UI
  if (!battOk)
    Serial.println("MAX17048 not found (no battery gauge)");

  ppsIdx = usbpd.getPPSIndex();
  Serial.print("PPS index: ");
  Serial.println(ppsIdx);

  delay(300);           // let the HMI come up
  lastSig = 0xFFFFFFFF; // force the first render regardless of prior RAM state
  sendProfileList();    // initial render (clears list if no source)

  // boot output state = "Last used": arm restore after list is known (reg 0x0031)
  g_bootRestore = (g_set.bootLastUsed && g_set.lastSel >= 0 && g_set.lastSel < g_slotN);
}

void loop()
{
  pollHMI(); // parse incoming control frames every pass
  uint32_t now = millis();

  // Apply a profile selected on the panel (reg 0x0023; 0x0020/0x0021 already latched)
  if (pendingSel >= 0)
  {
    int sel = pendingSel;
    pendingSel = -1;
    if (sel < g_slotN)
    {
      Slot &s = g_slots[sel];
      switch (s.type)
      {
      case 1:
        usbpd.setPPSPDO(s.pdoIndex, reqMV, limMA);
        break; // PPS
      case 2:
        usbpd.setAVSPDO(s.pdoIndex, reqMV, limMA);
        break; // AVS
      default:
        usbpd.setFixPDO(s.pdoIndex, (s.imax > 4999) ? 4999 : s.imax);
        break; // FIX / EPR-fixed
      }
      activePdoIdx = s.pdoIndex;
      activeType = s.type;
      armOCP((s.type == 1 || s.type == 2) ? limMA
                                          : ((s.imax > 4999) ? 4999 : s.imax)); // PPS/AVS use limMA; FIX uses PDO imax
      g_activeSel = sel;                                                        // active position for the profile-list highlight (reg 0x0017)
      if (g_set.lastSel != sel || g_set.lastMV != reqMV || g_set.lastMA != limMA)
      {
        g_set.lastSel = sel; // remember rail + adjust for "Last used"
        g_set.lastMV = reqMV;
        g_set.lastMA = limMA;
        saveSettings();
      }
      if (g_set.autoArm) // auto-arm setting (reg 0x0032)
      {
        usbpd.setOutput(1);
        outputOn = true;
        g_ocpLatched = false;
        writeReg(0x001F, 0); // fresh apply clears any OCP latch + popups
      }
      if (g_bootRestoreOut >= 0) // boot "Last used": force saved output state
      {
        outputOn = (g_bootRestoreOut != 0);
        g_bootRestoreOut = -1;
      }
    }
  }

  // PD keep-alive: refresh the *applied* PPS/AVS rail (fixed PDOs don't need it)
  static uint32_t tPPS = 0;
  if ((activeType == 1 || activeType == 2) && now - tPPS >= 2000)
  {
    tPPS = now;
    if (activeType == 1)
      usbpd.setPPSPDO(activePdoIdx, reqMV, limMA);
    else
      usbpd.setAVSPDO(activePdoIdx, reqMV, limMA);
  }

  // Output switch: act when the HMI changes it, or re-assert after a source attach
  static int lastOut = -1;
  if (g_outAttach)
  {
    g_outAttach = false;
    usbpd.begin(); // refresh library PDO array so setFixPDO/setPPSPDO use the new source
    lastOut = -1;  // force re-apply of outputOn (default OFF) on the next check
  }
  // boot "Last used": once the initial attach is consumed, restore the rail
  if (g_bootRestore && !g_outAttach)
  {
    g_bootRestore = false;
    reqMV = g_set.lastMV;                  // restore PPS/AVS voltage...
    limMA = g_set.lastMA;                  // ...and current limit
    pendingSel = g_set.lastSel;            // applied next loop pass
    g_bootRestoreOut = g_set.lastOutputOn; // then forces saved output state
  }
  if ((int)outputOn != lastOut)
  {
    lastOut = outputOn;
    usbpd.setOutput(outputOn ? 1 : 0);
    writeReg(0x0016, outputOn ? 1 : 0); /* tell the panel immediately */
    if (!outputOn)
      lifeWriteFile();                           /* force-commit odometer at end of run */
    if (g_set.lastOutputOn != (uint8_t)outputOn) // remember for "Last used"
    {
      g_set.lastOutputOn = outputOn;
      saveSettings();
    }
  }

  // Re-push settings to the panel for the first few seconds (panel boots slower than RP)
  static uint32_t tSet = 0;
  static uint8_t setPushes = 0;
  if (setPushes < 12 && now - tSet >= 1000)
  {
    tSet = now;
    writeReg(0x0030, g_set.bright);
    writeReg(0x0039, g_set.theme);
    writeReg(0x0031, g_set.bootLastUsed);
    writeReg(0x0032, g_set.autoArm);
    setPushes++;
  }
  if (g_brightDirty && now - g_brightT >= 800)
  {
    g_brightDirty = false;
    saveSettings();
  }

  static uint32_t tProf = 0;
  uint32_t profPeriod = (now < 5000) ? 1000 : 2000; // faster while the HMI boots
  if (now - tProf >= profPeriod)
  {
    tProf = now;
    if (now < 5000)
      lastSig = 0xFFFFFFFF; // boot window: force re-push until the panel is listening
    sendProfileList();
  }

  // Telemetry: fast, smooth refresh
  static uint32_t tTel = 0;
  if (now - tTel >= 500) // 2 Hz
  {
    tTel = now;
    float fmV = ina260.readBusVoltage();
    float fmA = ina260.readCurrent();
    float fmW = ina260.readPower();
    if (fmA < 0)
      fmA = 0; // INA reads slightly negative near no-load
    if (fmW < 0)
      fmW = 0;
    bool good = (fmW <= 160000.0f && fmA <= 6000.0f); // plausible? supply max ~140W / 5A
    uint16_t mV = (uint16_t)fmV;
    uint16_t mA = (uint16_t)fmA;
    uint32_t mW = (uint32_t)fmW;
    writeReg(0x0010, mV);
    g_arcTgtMV = mV; /* feed the analog arc easer */
    writeReg(0x0011, mA);
    writeReg(0x0012, (uint16_t)(mW / 100));
    writeReg(0x0016, outputOn ? 1 : 0);  /* real output state for the view1 toggle */
    energyAccumulate(now, mW, mA, good); /* session + lifetime integration */
    uint16_t apType, apMV;
    activeProfileInfo(&apType, &apMV);
    writeReg(0x0019, apType); /* active profile type (0=none) */
    writeReg(0x001A, apMV);   /* active profile setpoint mV   */

    /* presence == sustained charging; full/none both -> "no battery" (by design) */
    uint8_t chg = readChargeState();
    bool present = battOk && batteryPresent(chg);
    uint16_t vcellmv = present ? (uint16_t)(maxlipo.cellVoltage() * 1000.0f) : 0;
    Serial.printf("BATT mv=%u chg=%u run=%u present=%u\n", vcellmv, chg, g_chg_run, present);
    writeReg(0x001E, present ? 1 : 0); /* 0=none/full, 1=charging (2=complete unused) */
    if (present)
    {
      writeReg(0x001C, vcellmv);                 /* cell mV */
      writeReg(0x001D, socFromVoltage(vcellmv)); /* SoC % */
    }
    else
    {
      writeReg(0x001C, 0);      /* -> panel "-.-" */
      writeReg(0x001D, 0xFFFF); /* -> panel "-.-" */
    }
  }

  // Analog arc easing: ramp the ring toward the measured voltage, pushed faster
  // than telemetry so it settles like a needle (reg 0x001B). Numeric V stays instant.
  static uint32_t tArc = 0;
  static float arcShown = 0.0f; // arc units = mV/100 (0..280)
  static uint16_t lastArc = 0xFFFF;
  if (now - tArc >= 40) // 25 Hz
  {
    tArc = now;
    float target = g_arcTgtMV / 100.0f;
    arcShown += (target - arcShown) * 0.22f; // approach rate: higher = snappier
    float d = target - arcShown;
    if (d < 0)
      d = -d;
    if (d < 0.5f)
      arcShown = target; // snap within 0.05 V
    uint16_t a = (uint16_t)(arcShown + 0.5f);
    if (a != lastArc)
    {
      lastArc = a;
      writeReg(0x001B, a);
    } // push only on change
  }

  // Lifetime energy -> HMI odometer (Wh, shown as XXXX.XXX kWh). Push on change @2Hz.
  static uint32_t tLife = 0, lastWh = 0xFFFFFFFF;
  if (now - tLife >= 500)
  {
    tLife = now;
    uint32_t wh = (uint32_t)(g_lifeE_uWh / 1000000ULL);
    if (wh > 9999999UL)
      wh = 9999999UL; // clamp to 7 digits (9999.999 kWh)
    if (wh != lastWh)
    {
      lastWh = wh;
      writeReg(0x003A, (uint16_t)(wh >> 16));
      writeReg(0x003B, (uint16_t)(wh & 0xFFFF));
    }
  }

  // AP33772S INT (active-HIGH): read STATUS 0x01 (auto-clears), decode events
  if (g_pdInt)
  {
    g_pdInt = false;
    serviceStatus();
  }
  // Poll STATUS for OCP (protection may not assert INT) + nego backstop
  static uint32_t tStat = 0;
  if (now - tStat >= 40)
  {
    tStat = now;
    serviceStatus();
  }

  // Fallback attach watch (INT backstop): slow poll for detach/missed edges
  static uint32_t tAtt = 0;
  if (now - tAtt >= 500)
  {
    tAtt = now;
    Wire.beginTransmission(0x52);
    bool present = (Wire.endTransmission() == 0);
    if (present && !g_prevSource)
    {
      g_outAttach = true;
      lastSig = 0xFFFFFFFF;
    }
  }
}