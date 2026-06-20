#include <Arduino.h>

void setup() {
  Serial.begin(115200);   // UART0: IO0=TX, IO1=RX -> probe UART
  Serial.println("\nC-Bench up, printing over probe UART");
}

void loop() {
  Serial.println("hello from RP2354A");
  delay(1000);
}