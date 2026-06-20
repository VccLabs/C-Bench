#include <Arduino.h>
#define HMI       Serial2
#define HMI_BAUD  115200

static void writeReg(uint16_t addr, uint16_t val) {
  uint8_t f[8] = { 0x5A, 0xA5, 0x05, 0x82,
                   (uint8_t)(addr >> 8), (uint8_t)addr,
                   (uint8_t)(val >> 8),  (uint8_t)val };
  HMI.write(f, 8);
}

void setup() {
  HMI.begin(HMI_BAUD);
}

void loop() {
  writeReg(0x0001, 0);   // -> "hi"
  delay(1000);
  writeReg(0x0001, 1);   // -> "hello"
  delay(1000);
}