#include <Arduino.h>
#include <Wire.h>
#include <AP33772S.h>

AP33772S usbpd;   // defaults to Wire (I2C0)

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000) {}   // wait for USB-CDC

  Wire.setSDA(20);   // IO20
  Wire.setSCL(21);   // IO21
  Wire.begin();

  delay(1000);       // let charger negotiation settle (lib recommends >500ms)
  usbpd.begin();     // reads source PDOs over I2C

  Serial.println("\nC-Bench: charger PDOs");
  usbpd.displayProfiles();
}

void loop() {
  usbpd.begin();     // reads source PDOs over I2C
    Serial.println("\nC-Bench: charger PDOs");
  usbpd.displayProfiles();
  delay(5000);
}