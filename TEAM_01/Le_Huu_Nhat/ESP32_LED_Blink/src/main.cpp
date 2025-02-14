#include <Arduino.h>

int Ledpin = 5;

void setup() {
  Serial.begin(115200);
  pinMode(Ledpin,OUTPUT);
  
}

void loop() {
digitalWrite(Ledpin, HIGH);
Serial.println("LED -> ON");
delay(1000);
digitalWrite(Ledpin, LOW);
Serial.println("LED -> OFF");
delay(1000);
}
