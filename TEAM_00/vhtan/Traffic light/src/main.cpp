#include <Arduino.h>

#define LED_RED 5      // Chân điều khiển đèn LED đỏ
#define LED_YELLOW 0   // Chân điều khiển đèn LED vàng
#define LED_GREEN 16   // Chân điều khiển đèn LED xanh

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
}

void loop() {
  // Bật đèn xanh trong 5 giây
  digitalWrite(LED_GREEN, HIGH);
  delay(5000);
  digitalWrite(LED_GREEN, LOW);

  // Bật đèn vàng trong 2 giây
  digitalWrite(LED_YELLOW, HIGH);
  delay(2000);
  digitalWrite(LED_YELLOW, LOW);

  // Bật đèn đỏ trong 5 giây
  digitalWrite(LED_RED, HIGH);
  delay(5000);
  digitalWrite(LED_RED, LOW);
}
