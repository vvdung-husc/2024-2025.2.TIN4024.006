#include <Arduino.h>
#include <TM1637Display.h>

// Khai báo chân kết nối đèn giao thông
const int LedDo = 27;
const int LedVang = 26;
const int LedXanh = 25;
const int LedDuong = 21;
const int button = 23;
const int LDR = 13;
const int CLK = 18;
const int DIO = 19;
TM1637Display display(CLK, DIO);

const int countRedInit = 5;
const int countYellowInit = 2;
const int countGreenInit = 7;

int countRed = countRedInit;
int countYellow = countYellowInit;
int countGreen = countGreenInit;

ulong ledStart = 0;
int LedState = 0;
bool isPaused = false;
bool buttonPressed = false;

ulong blinkStart = 0;
bool yellowBlinkState = false;
bool lastLdrState = false;

// Kiểm tra thời gian không chặn
bool IsReady(ulong& ulTimer, uint32_t millisecond) {
    ulong t = millis();
    if (t - ulTimer < millisecond) return false;
    ulTimer = t;
    return true;
}

void UpdateDisplay(int number) {
    static int lastNumber = -1;
    if (lastNumber != number) {
        display.showNumberDec(number);
        lastNumber = number;
    }
}

void Use_Non_Blocking() {
    unsigned long currentMillis = millis();
    int ldrValue = analogRead(LDR);
    bool buttonState = digitalRead(button) == LOW;

    // Kiểm tra nút nhấn (Debounce)
    if (buttonState && !buttonPressed) {
        isPaused = !isPaused;
        buttonPressed = true;
        Serial.println(isPaused ? "TẮT BỘ ĐẾM" : "BẬT BỘ ĐẾM");
    } else if (!buttonState) {
        buttonPressed = false;
    }

    // Cập nhật đèn đường theo trạng thái bộ đếm
    digitalWrite(LedDuong, isPaused ? HIGH : LOW);

    // Kiểm tra ánh sáng môi trường
    bool isDark = (ldrValue < 50);
    if (isDark != lastLdrState) {
        lastLdrState = isDark;
        Serial.println(isDark ? "Trời tối - Đèn vàng nhấp nháy" : "Trời sáng - Hoạt động bình thường");
    }

    if (isDark) {
        digitalWrite(LedDo, LOW);
        digitalWrite(LedXanh, LOW);
        if (IsReady(blinkStart, 500)) {
            yellowBlinkState = !yellowBlinkState;
            digitalWrite(LedVang, yellowBlinkState);
        }
        return;
    }

    // Điều khiển hệ thống đèn giao thông
    switch (LedState) {
        case 0: // Đèn Xanh
            digitalWrite(LedDo, LOW);
            digitalWrite(LedVang, LOW);
            digitalWrite(LedXanh, HIGH);
            if (!isPaused) UpdateDisplay(countGreen);
            if (IsReady(ledStart, 1000)) { 
                if (!isPaused && --countGreen < 0) {
                    LedState = 1;
                    countGreen = countGreenInit;
                }
            }
            break;
        
        case 1: // Đèn Vàng
            digitalWrite(LedDo, LOW);
            digitalWrite(LedVang, HIGH);
            digitalWrite(LedXanh, LOW);
            if (!isPaused) UpdateDisplay(countYellow);
            if (IsReady(ledStart, 1000)) { 
                if (!isPaused && --countYellow < 0) {
                    LedState = 2;
                    countYellow = countYellowInit;
                }
            }
            break;

        case 2: // Đèn Đỏ
            digitalWrite(LedDo, HIGH);
            digitalWrite(LedVang, LOW);
            digitalWrite(LedXanh, LOW);
            if (!isPaused) UpdateDisplay(countRed);
            if (IsReady(ledStart, 1000)) { 
                if (!isPaused && --countRed < 0) {
                    LedState = 0;
                    countRed = countRedInit;
                }
            }
            break;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LedDo, OUTPUT);
    pinMode(LedVang, OUTPUT);
    pinMode(LedXanh, OUTPUT);
    pinMode(LedDuong, OUTPUT);
    pinMode(button, INPUT_PULLUP);
    pinMode(LDR, INPUT);

    digitalWrite(LedDo, LOW);
    digitalWrite(LedVang, LOW);
    digitalWrite(LedXanh, LOW);
    digitalWrite(LedDuong, LOW);

    display.setBrightness(0x0f);
}

void loop() {
    Use_Non_Blocking();
}
