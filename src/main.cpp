#include <Arduino.h>
#include <Wire.h>
#include <AP33772S.h>
#include <Adafruit_INA260.h>

AP33772S usbpd;         // defaults to Wire (I2C0)
Adafruit_INA260 ina260; // output-bus monitor @ 0x40

const int PPS_TARGET_MV = 9000; // request 9 V
const int PPS_LIMIT_MA = 2000;  // 2 A limit
int ppsIdx = -1;

#define HMI Serial2 // UART1: IO8=TX, IO9=RX -> TR660

static void writeReg(uint16_t addr, uint16_t val)
{
  uint8_t f[8] = {0x5A, 0xA5, 0x05, 0x82,
                  (uint8_t)(addr >> 8), (uint8_t)addr,
                  (uint8_t)(val >> 8), (uint8_t)val};
  HMI.write(f, 8);
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

  HMI.begin(115200);   // screen link

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
  if (ppsIdx > 0)
  {
    usbpd.setPPSPDO(ppsIdx, PPS_TARGET_MV, PPS_LIMIT_MA); // re-request keeps PPS alive
    usbpd.setOutput(1);                                   // close load switch
  }

  delay(300); // let the rail settle before measuring

  uint16_t mV = (uint16_t)ina260.readBusVoltage();   // mV
  uint16_t mA = (uint16_t)ina260.readCurrent();      // mA
  uint16_t dW = (uint16_t)(ina260.readPower()/100);  // mW -> 0.1W

  writeReg(0x0010, mV);   // -> voltage label
  writeReg(0x0011, mA);   // -> current label
  writeReg(0x0012, dW);   // -> power label

  Serial.printf("Bus: %u mV  %u mA  %u dW\n", mV, mA, dW);

  delay(300); // total cycle <750ms keeps the PPS request fresh
}