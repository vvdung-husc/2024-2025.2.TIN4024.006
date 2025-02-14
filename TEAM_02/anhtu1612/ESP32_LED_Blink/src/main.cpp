#include <Arduino.h>

int ledPin = 5;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); // Cấu hình chân LED là đầu ra
}

void loop() {
  digitalWrite(ledPin, HIGH); // Bật đèn LED
  Serial.print("LED -> ON");
  delay(1000); // Đợi 1 giây
  Serial.print(" LED -> OFF");
  digitalWrite(ledPin, LOW); // Tắt đèn LED
  delay(1000); // Đợi 1 giây
}