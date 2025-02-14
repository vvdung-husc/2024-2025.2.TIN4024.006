#include <Arduino.h>

// put function declarations here:

int ledPin = 5;
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);


}

void loop() {

  digitalWrite(ledPin, HIGH);
  Serial.println("LED is on");
  delay(1000);
  digitalWrite(ledPin, LOW);
  Serial.println("LED is off");
  delay(1000);
}

