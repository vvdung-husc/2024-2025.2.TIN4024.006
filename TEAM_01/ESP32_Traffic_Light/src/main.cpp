#include <Arduino.h>
#include <TM1637Display.h>

#define PIN_RED 27
#define PIN_YELLOW 26
#define PIN_GREEN 25
#define PIN_BLUE 21
#define PIN_BUTTON 23
#define PIN_LDR 13
#define CLK 18
#define DIO 19

TM1637Display display(CLK, DIO);
bool isRunning = false;

unsigned long previousMillis = 0;
const long redTime = 5000;
const long yellowTime = 2000;
const long greenTime = 7000;
const long blinkInterval = 500;

int currentState = 0;
unsigned long stateStartTime = 0;
bool yellowBlinkState = false;

void setup() {
    pinMode(PIN_RED, OUTPUT);
    pinMode(PIN_YELLOW, OUTPUT);
    pinMode(PIN_GREEN, OUTPUT);
    pinMode(PIN_BLUE, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LDR, INPUT);
    display.setBrightness(0x0F);
    display.showNumberDec(0);
    stateStartTime = millis();
}

void loop() {
    unsigned long currentMillis = millis();
    int ldrValue = analogRead(PIN_LDR);
    bool buttonState = digitalRead(PIN_BUTTON) == LOW;

    Serial.println(ldrValue);
    digitalWrite(PIN_BLUE, buttonState ? HIGH : LOW);

    if (ldrValue < 50) { // Trời tối, chỉ đèn vàng nhấp nháy
        digitalWrite(PIN_RED, LOW);
        digitalWrite(PIN_GREEN, LOW);
        if (currentMillis - previousMillis >= blinkInterval) {
            previousMillis = currentMillis;
            yellowBlinkState = !yellowBlinkState;
            digitalWrite(PIN_YELLOW, yellowBlinkState ? HIGH : LOW);
        }
    } else { // Chạy hệ thống đèn giao thông bình thường
        switch (currentState) {
            case 0: // Đèn đỏ
                digitalWrite(PIN_RED, HIGH);
                digitalWrite(PIN_YELLOW, LOW);
                digitalWrite(PIN_GREEN, LOW);
                display.showNumberDec((redTime - (currentMillis - stateStartTime)) / 1000);
                if (currentMillis - stateStartTime >= redTime) {
                    currentState = 1;
                    stateStartTime = currentMillis;
                }
                break;
            case 1: // Đèn xanh
                digitalWrite(PIN_RED, LOW);
                digitalWrite(PIN_YELLOW, LOW);
                digitalWrite(PIN_GREEN, HIGH);
                display.showNumberDec((greenTime - (currentMillis - stateStartTime)) / 1000);
                if (currentMillis - stateStartTime >= greenTime) {
                    currentState = 2;
                    stateStartTime = currentMillis;
                }
                break;
            case 2: // Đèn vàng
                digitalWrite(PIN_RED, LOW);
                digitalWrite(PIN_YELLOW, HIGH);
                digitalWrite(PIN_GREEN, LOW);
                display.showNumberDec((yellowTime - (currentMillis - stateStartTime)) / 1000);
                if (currentMillis - stateStartTime >= yellowTime) {
                    currentState = 0;
                    stateStartTime = currentMillis;
                }
                break;
        }
    }
}
