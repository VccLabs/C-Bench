#include <Arduino.h>

// HMI UART per README: RP TX=IO8, RP RX=IO9 -> UART1 = Serial2 (earlephilhower)
#define HMI        Serial2
#define HMI_BAUD   115200
#define TEST_ADDR  0x0001     // any register in 0x0000..0x07FF
#define TEST_VAL   0xABCD

static void txFrame(const uint8_t *f, size_t n) {
  HMI.write(f, n);
  Serial.print("TX ->");
  for (size_t i = 0; i < n; i++) Serial.printf(" %02X", f[i]);
  Serial.println();
}

// Write 1 reg: 5A A5 05 82 addrH addrL valH valL  (cmd 0x82, no HMI reply)
static void writeReg(uint16_t a, uint16_t v) {
  uint8_t f[8] = {0x5A,0xA5,0x05,0x82,
                  (uint8_t)(a>>8),(uint8_t)(a&0xFF),
                  (uint8_t)(v>>8),(uint8_t)(v&0xFF)};
  txFrame(f, sizeof(f));
}

// Read N regs: 5A A5 04 83 addrH addrL N  (cmd 0x83, HMI replies)
static void readReg(uint16_t a, uint8_t n) {
  uint8_t f[7] = {0x5A,0xA5,0x04,0x83,
                  (uint8_t)(a>>8),(uint8_t)(a&0xFF),n};
  txFrame(f, sizeof(f));
}

static void dumpRx(uint32_t ms) {
  uint32_t t = millis(); bool any = false;
  while (millis() - t < ms) {
    while (HMI.available()) {
      if (!any) { Serial.print("RX <-"); any = true; }
      Serial.printf(" %02X", (uint8_t)HMI.read());
    }
  }
  Serial.println(any ? "" : "RX <- (no response)");
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000) {}   // wait for USB-CDC
  Serial.println("\nC-Bench HMI UART test");
  HMI.setTX(8);
  HMI.setRX(9);
  HMI.begin(HMI_BAUD);
}

void loop() {
  Serial.println("\n--- cycle ---");

  Serial.println("[1] read before write:");
  readReg(TEST_ADDR, 1);
  dumpRx(300);

  Serial.println("[2] write reg:");
  writeReg(TEST_ADDR, TEST_VAL);   // no reply expected
  dumpRx(150);

  Serial.println("[3] read back:");
  readReg(TEST_ADDR, 1);
  dumpRx(300);

  delay(3000);
}