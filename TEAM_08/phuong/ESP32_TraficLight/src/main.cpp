#include <Arduino.h>
#include <TM1637Display.h>

#define LED_RED 27
#define LED_YELLOW 26
#define LED_GREEN 25
#define BTN_PIN 23
#define LDR_PIN 34  // Chân đọc cảm biến ánh sáng (ESP32 dùng ADC34)

#define TM1637_CLK 18
#define TM1637_DIO 19

TM1637Display display(TM1637_CLK, TM1637_DIO);
bool showTime = true;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

const int lightThreshold = 500; // Ngưỡng để xác định trời tối (0-4095 trên ESP32)

void setup() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(BTN_PIN, INPUT_PULLUP);
    display.setBrightness(7);
}

void handleButton();
void countDownNonBlocking(int);
void blinkYellowLight();

void loop() {
    handleButton(); // Xử lý nút nhấn
    int lightLevel = analogRead(LDR_PIN); // Đọc cảm biến ánh sáng

    if (lightLevel < lightThreshold) {
        blinkYellowLight(); // Trời tối → chỉ bật đèn vàng nhấp nháy
    } else {
        digitalWrite(LED_RED, HIGH);
        countDownNonBlocking(5);
        digitalWrite(LED_RED, LOW);

        digitalWrite(LED_GREEN, HIGH);
        countDownNonBlocking(5);
        digitalWrite(LED_GREEN, LOW);

        digitalWrite(LED_YELLOW, HIGH);
        countDownNonBlocking(2);
        digitalWrite(LED_YELLOW, LOW);
    }
}

// Xử lý nút nhấn bật/tắt màn hình
void handleButton() {
    bool buttonState = digitalRead(BTN_PIN);
    if (buttonState != lastButtonState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (buttonState == LOW && lastButtonState == HIGH) {
            showTime = !showTime;
        }
    }
    lastButtonState = buttonState;
}

// Đếm ngược không chặn vòng lặp
void countDownNonBlocking(int seconds) {
    unsigned long startTime = millis();
    int remainingTime = seconds;
    while (remainingTime >= 0) {
        if (millis() - startTime >= 1000) {
            startTime = millis();
            if (showTime) {
                display.showNumberDec(remainingTime, true);
            } else {
                display.clear();
            }
            remainingTime--;
        }
        handleButton();
    }
}

// Chế độ đèn vàng nhấp nháy khi trời tối
void blinkYellowLight() {
    while (true) {
        digitalWrite(LED_YELLOW, HIGH);
        delay(500);
        digitalWrite(LED_YELLOW, LOW);
        delay(500);

        handleButton();
        int lightLevel = analogRead(LDR_PIN);
        if (lightLevel >= lightThreshold) {
            return; // Khi trời sáng lại, thoát chế độ nhấp nháy
        }
    }
}