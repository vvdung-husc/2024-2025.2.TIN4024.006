#include <Arduino.h>

int ledPin = 5;
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

}



void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(ledPin, HIGH);
  Serial.print("LED -> ON");
  delay(1000);
  digitalWrite(ledPin,LOW);
  Serial.print("LED -> OFF");
  delay(1000);

}

