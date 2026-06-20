#include <Arduino.h>
#include <Wire.h>
#include <AP33772S.h>
#include <Adafruit_INA260.h>

AP33772S usbpd;            // defaults to Wire (I2C0)
Adafruit_INA260 ina260;   // output-bus monitor @ 0x40

const int   PPS_TARGET_MV = 9000;   // request 9 V
const int   PPS_LIMIT_MA  = 2000;   // 2 A limit
int         ppsIdx        = -1;

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

  if (!ina260.begin(0x40)) {
    Serial.println("INA260 not found!");
    while (1) delay(100);
  }

  ppsIdx = usbpd.getPPSIndex();
  Serial.print("PPS index: "); Serial.println(ppsIdx);
}

void loop() {
  if (ppsIdx > 0) {
    usbpd.setPPSPDO(ppsIdx, PPS_TARGET_MV, PPS_LIMIT_MA);  // re-request keeps PPS alive
    usbpd.setOutput(1);                                    // close load switch
  }

  delay(300);  // let the rail settle before measuring

  Serial.print("Bus: ");
  Serial.print(ina260.readBusVoltage() / 1000.0, 3); Serial.print(" V  ");
  Serial.print(ina260.readCurrent()    / 1000.0, 3); Serial.print(" A  ");
  Serial.print(ina260.readPower()      / 1000.0, 3); Serial.println(" W");

  delay(300);  // total cycle <750ms keeps the PPS request fresh
}