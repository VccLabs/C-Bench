#include <Arduino.h>

#define HMI_SERIAL  Serial2          // UART1: GP8=TX, GP9=RX
#define HMI_BAUD    115200

String rxLine;
enum RpState { ST_IDLE, ST_WAIT_HELLO } state = ST_IDLE;
uint32_t t0 = 0;

void setup() {
  Serial.begin(115200);              // USB debug
  HMI_SERIAL.setTX(8);
  HMI_SERIAL.setRX(9);
  HMI_SERIAL.begin(HMI_BAUD);
}

void loop() {
  while (HMI_SERIAL.available()) {
    char c = HMI_SERIAL.read();
    if (c == '\n' || c == '\r') {
      rxLine.trim();
      if (rxLine == "hi") {
        HMI_SERIAL.print("Hi from RP\n");
        Serial.println("got 'hi' -> sent 'Hi from RP'");
        state = ST_WAIT_HELLO; t0 = millis();
      }
      rxLine = "";
    } else {
      rxLine += c;
      if (rxLine.length() > 32) rxLine = "";   // guard
    }
  }

  if (state == ST_WAIT_HELLO && millis() - t0 >= 1000) {
    HMI_SERIAL.print("hello\n");
    Serial.println("sent 'hello'");
    state = ST_IDLE;
  }
}