#include <Arduino.h>

const int ledPin = 5; // Chân GPIO điều khiển LED (GPIO2 là mặc định trên ESP32)

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200); // Khởi động Serial Monitor để kiểm tra
}

void loop() {
  digitalWrite(ledPin, HIGH); // Bật đèn LED
  Serial.println("LED ON");
  delay(1000); // Chờ 1 giây

  digitalWrite(ledPin, LOW);  // Tắt đèn LED
  Serial.println("LED OFF");
  delay(1000); // Chờ 1 giây
}

