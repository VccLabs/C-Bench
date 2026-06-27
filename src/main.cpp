#include <Arduino.h>
#include <Wire.h>
#include <AP33772S.h>
#include <Adafruit_INA260.h>
#include <EEPROM.h>

AP33772S usbpd;         // defaults to Wire (I2C0)
Adafruit_INA260 ina260; // output-bus monitor @ 0x40

const int PPS_TARGET_MV = 9000; // request 9 V
const int PPS_LIMIT_MA = 2000;  // 2 A limit
int ppsIdx = -1;

// ---- HMI control state (panel -> RP) ----
volatile uint16_t reqMV = PPS_TARGET_MV; // reg 0x0020 (next milestone)
volatile uint16_t limMA = PPS_LIMIT_MA;  // reg 0x0021 (next milestone)
volatile bool outputOn = false;          // reg 0x0022 - default OFF for safety
volatile int pendingSel = -1;            // reg 0x0023 list position, applied in loop()

// ---- persisted settings (flash via EEPROM emulation) ----
#define SET_MAGIC 0xCB01
struct Settings
{
  uint16_t magic;
  uint8_t bootLastUsed; // reg 0x0031: 0=Off at boot, 1=restore Last used
  uint8_t autoArm;      // reg 0x0032: 0/1 auto turn output on after apply
  uint8_t lastOutputOn; // remembered for "Last used" boot restore (step 2)
  int16_t lastSel;      // remembered profile position for "Last used" (step 2)
};
Settings g_set;

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
static int activePdoIdx = -1;             // applied PDO (1-based), -1 = none
static uint8_t activeType = 0;            // 0 FIX, 1 PPS, 2 AVS, 3 EPR
static bool g_prevSource = false;         // source-present edge detect
static volatile bool g_outAttach = false; // re-assert output after a (re)attach

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
    saveSettings();
  }
  Serial.printf("Settings loaded: magic=%04X boot=%u autoArm=%u\n",
                g_set.magic, g_set.bootLastUsed, g_set.autoArm);
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
    break; // output enable
  case 0x0023:
    if (val < g_slotN)
      pendingSel = (int)val; // selected position, applied in loop()
    break;
  case 0x0024:
    lastSig = 0xFFFFFFFF; // panel opened view2 -> force a fresh list push now
    sendProfileList();
    break;
  case 0x0031:
    g_set.bootLastUsed = (val != 0); // boot output state: 0=Off, 1=Last used
    saveSettings();
    break;
  case 0x0032:
    g_set.autoArm = (val != 0); // auto-arm output after apply
    saveSettings();
    break;
  case 0x0033: // panel entered view4 -> push stored settings back for display
    writeReg(0x0031, g_set.bootLastUsed);
    writeReg(0x0032, g_set.autoArm);
    break;
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

  loadSettings(); // restore persisted settings from flash

  Wire.setSDA(20); // IO20
  Wire.setSCL(21); // IO21
  Wire.begin();

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

  ppsIdx = usbpd.getPPSIndex();
  Serial.print("PPS index: ");
  Serial.println(ppsIdx);

  delay(300);           // let the HMI come up
  lastSig = 0xFFFFFFFF; // force the first render regardless of prior RAM state
  sendProfileList();    // initial render (clears list if no source)
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
      if (g_set.autoArm) // auto-arm setting (reg 0x0032)
      {
        usbpd.setOutput(1);
        outputOn = true;
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
  if ((int)outputOn != lastOut)
  {
    lastOut = outputOn;
    usbpd.setOutput(outputOn ? 1 : 0);
  }

  // Re-push settings to the panel for the first few seconds (panel boots slower than RP)
  static uint32_t tSet = 0;
  static uint8_t setPushes = 0;
  if (setPushes < 6 && now - tSet >= 1000)
  {
    tSet = now;
    writeReg(0x0031, g_set.bootLastUsed);
    writeReg(0x0032, g_set.autoArm);
    setPushes++;
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
    uint16_t mV = (uint16_t)ina260.readBusVoltage();
    uint16_t mA = (uint16_t)ina260.readCurrent();
    uint16_t dW = (uint16_t)(ina260.readPower() / 100);
    writeReg(0x0010, mV);
    writeReg(0x0011, mA);
    writeReg(0x0012, dW);
  }
  // Fast source-attach watch: kill VOUT ASAP after a contract appears
  static uint32_t tAtt = 0;
  if (now - tAtt >= 150)
  {
    tAtt = now;
    Wire.beginTransmission(0x52);
    bool present = (Wire.endTransmission() == 0);
    if (present && !g_prevSource)
    {
      g_outAttach = true;   // re-assert output (default OFF) immediately
      lastSig = 0xFFFFFFFF; // and refresh the PDO list now
    }
  }
}