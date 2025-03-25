#include <Arduino.h>
#define LED_PIN 2  // Chân D2 trên ESP8266

void setup() {
    pinMode(LED_PIN, OUTPUT);  // Thiết lập chân D2 là OUTPUT
}

void loop() {
    digitalWrite(LED_PIN, HIGH);  // Bật đèn LED
    delay(1000);                 // Chờ 1 giây
    digitalWrite(LED_PIN, LOW);   // Tắt đèn LED
    delay(1000);                 // Chờ 1 giây
}
