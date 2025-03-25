#include <Arduino.h>

#define LED_XANH 15
#define LED_VANG 2
#define LED_DO 4

void setup() {
    pinMode(LED_XANH, OUTPUT);
    pinMode(LED_VANG, OUTPUT);
    pinMode(LED_DO, OUTPUT);
}

void loop() {
    digitalWrite(LED_XANH, HIGH);
    digitalWrite(LED_VANG, LOW);
    digitalWrite(LED_DO, LOW);
    delay(1000); // Giữ đèn xanh sáng 1 giây

    digitalWrite(LED_XANH, LOW);
    digitalWrite(LED_VANG, HIGH);
    digitalWrite(LED_DO, LOW);
    delay(1000); // Giữ đèn vàng sáng 1 giây

    digitalWrite(LED_XANH, LOW);
    digitalWrite(LED_VANG, LOW);
    digitalWrite(LED_DO, HIGH);
    delay(1000); // Giữ đèn đỏ sáng 1 giây
}
