void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
}

void loop() {
  Serial2.write(0x02);

  if (Serial2.available()) {
    Serial.print("Got a message: ");
      Serial.println(Serial2.read());
  }

  delay(1000);
}
