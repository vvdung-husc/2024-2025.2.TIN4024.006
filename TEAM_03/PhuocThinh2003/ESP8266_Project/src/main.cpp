#include <Arduino.h>

// Chân GPIO của đèn LED tích hợp trên ESP8266 thường là GPIO2
#define LED1 2  // GPIO2 - thường là D4
#define LED2 15  // GPIO4 - thường là D2
#define LED3 4  // GPIO5 - thường là D1

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
  digitalWrite(LED1, LOW);
  delay(1000);
  digitalWrite(LED1, HIGH);

  // Bật LED2
  digitalWrite(LED2, LOW);
  delay(1000);
  digitalWrite(LED2, HIGH);

  // Bật LED3
  // digitalWrite(LED3, LOW);
  // delay(1000);
  // digitalWrite(LED3, HIGH);
}
