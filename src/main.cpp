#include <Arduino.h>

#define HMI_SERIAL Serial2 // UART1: GP8=TX, GP9=RX
#define HMI_BAUD 115200

// ---- HMI register protocol (matches Giraffe grf_hw_uart.c) ----
static const uint8_t HEAD_H = 0x5A, HEAD_L = 0xA5;

uint16_t g_vsetMv = 0, g_ilimMa = 0;
bool g_outEn = false;

static void hmiWriteReg(uint16_t addr, uint16_t val)
{
  uint8_t f[8] = {HEAD_H, HEAD_L, 0x05, 0x82,
                  (uint8_t)(addr >> 8), (uint8_t)addr,
                  (uint8_t)(val >> 8), (uint8_t)val};
  HMI_SERIAL.write(f, sizeof(f));
}

static void onReg(uint16_t addr, uint16_t val)
{
  switch (addr)
  {
  case 0x20:
    g_vsetMv = val;
    break;
  case 0x21:
    g_ilimMa = val;
    break;
  case 0x22:
    g_outEn = val;
    break;
  case 0x30:
    Serial.println("HMI button0 pressed");
    break;
    // 0x23 profile idx, 0x24 apply -> act here
  }
  Serial.printf("HMI reg %04X = %u\n", addr, val);
}

void setup()
{
  Serial.begin(115200); // USB debug
  HMI_SERIAL.setTX(8);
  HMI_SERIAL.setRX(9);
  HMI_SERIAL.begin(HMI_BAUD);
}

void loop()
{
  static uint8_t buf[64];
  static uint8_t blen = 0;

  while (HMI_SERIAL.available())
  {
    uint8_t c = HMI_SERIAL.read();
    if (blen == 0 && c != HEAD_H)
      continue;
    if (blen == 1 && c != HEAD_L)
    {
      blen = 0;
      continue;
    }
    buf[blen++] = c;
    if (blen >= 3 && blen == (uint8_t)(3 + buf[2]))
    { // full frame
      if (buf[3] == 0x82)
      { // write-reg
        uint16_t addr = (buf[4] << 8) | buf[5];
        uint8_t n = (buf[2] - 3) / 2;
        for (uint8_t i = 0; i < n; i++)
          onReg(addr + i, (buf[6 + i * 2] << 8) | buf[7 + i * 2]);
      }
      blen = 0;
    }
    if (blen >= sizeof(buf))
      blen = 0;
  }
  // 1 Hz toggle for loopback test
  static uint32_t tTog = 0;
  static uint16_t tog = 0;
  if (millis() - tTog >= 1000)
  {
    tTog = millis();
    tog ^= 1;
    hmiWriteReg(0x10, tog);
    Serial.printf("TX 0x10 = %u\n", tog);
  }
}