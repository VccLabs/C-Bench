#include <Arduino.h>
#include <Wire.h>
#include <AP33772S.h>
#include <Adafruit_INA260.h>

AP33772S usbpd;         // defaults to Wire (I2C0)
Adafruit_INA260 ina260; // output-bus monitor @ 0x40

const int PPS_TARGET_MV = 9000; // request 9 V
const int PPS_LIMIT_MA = 2000;  // 2 A limit
int ppsIdx = -1;

// ---- HMI control state (panel -> RP) ----
volatile uint16_t reqMV = PPS_TARGET_MV; // reg 0x0020 (next milestone)
volatile uint16_t limMA = PPS_LIMIT_MA;  // reg 0x0021 (next milestone)
volatile bool outputOn = false;          // reg 0x0022 - default OFF for safety

#define HMI Serial2 // UART1: IO8=TX, IO9=RX -> TR660

static void writeReg(uint16_t addr, uint16_t val)
{
  uint8_t f[8] = {0x5A, 0xA5, 0x05, 0x82,
                  (uint8_t)(addr >> 8), (uint8_t)addr,
                  (uint8_t)(val >> 8), (uint8_t)val};
  HMI.write(f, 8);
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
    limMA = val;
    break; // current limit (mA)
  case 0x0022:
    outputOn = (val != 0);
    break; // output enable
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
}

void loop()
{
  pollHMI(); // parse incoming control frames every pass
  uint32_t now = millis();

  // PD keep-alive: re-request PPS occasionally (PPS only needs a refresh ~every <10s)
  static uint32_t tPPS = 0;
  if (ppsIdx > 0 && now - tPPS >= 2000)
  {
    tPPS = now;
    usbpd.setPPSPDO(ppsIdx, PPS_TARGET_MV, PPS_LIMIT_MA);
  }

  // Output switch: act only when the HMI changes it
  static int lastOut = -1;
  if (ppsIdx > 0 && (int)outputOn != lastOut)
  {
    lastOut = outputOn;
    usbpd.setOutput(outputOn ? 1 : 0);
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
}