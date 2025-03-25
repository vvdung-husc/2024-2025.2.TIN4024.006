#include <Arduino.h>

// Khai báo chân LED
#define LED_RED 4
#define LED_YELLOW 2
#define LED_GREEN 15

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
}

void loop() {
  // Bật đèn đỏ, tắt hai đèn còn lại
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
  delay(1000);

  // Bật đèn vàng, tắt hai đèn còn lại
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_GREEN, LOW);
  delay(1000);

  // Bật đèn xanh, tắt hai đèn còn lại
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, HIGH);
  delay(1000); 
}
