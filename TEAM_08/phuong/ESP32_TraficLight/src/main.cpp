#include <Arduino.h>
#include <TM1637Display.h>

#define LED_RED 27
#define LED_YELLOW 26
#define LED_GREEN 25
#define BTN_PIN 23

#define TM1637_CLK 18
#define TM1637_DIO 19

TM1637Display display(TM1637_CLK, TM1637_DIO);
bool showTime = true;  // Trạng thái hiển thị đồng hồ
bool lastButtonState = HIGH; // Lưu trạng thái nút trước đó

void setup() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(BTN_PIN, INPUT_PULLUP);
    display.setBrightness(7); // Độ sáng tối đa
}

void countDown(int);
void loop() {
    // Kiểm tra nếu nút được nhấn để bật/tắt màn hình
    bool buttonState = digitalRead(BTN_PIN);
    if (buttonState == LOW && lastButtonState == HIGH) {
        showTime = !showTime;
        delay(200); // Chống dội phím
    }
    lastButtonState = buttonState;

    // Đèn đỏ sáng 5s
    digitalWrite(LED_RED, HIGH);
    countDown(5);
    digitalWrite(LED_RED, LOW);

    // Đèn xanh sáng 5s
    digitalWrite(LED_GREEN, HIGH);
    countDown(5);
    digitalWrite(LED_GREEN, LOW);

    // Đèn vàng sáng 2s
    digitalWrite(LED_YELLOW, HIGH);
    countDown(2);
    digitalWrite(LED_YELLOW, LOW);
}

// Hàm đếm ngược hiển thị trên TM1637
void countDown(int seconds) {
    for (int i = seconds; i >= 0; i--) {
        if (showTime) {
            display.showNumberDec(i, true);
        } else {
            display.clear();
        }
        delay(1000);
    }
}