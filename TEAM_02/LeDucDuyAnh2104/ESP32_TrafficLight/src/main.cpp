#include <Arduino.h>
#include <TM1637Display.h>

int redPin = 5;
int yellowPin = 17;
int greenPin = 16;
int bluePin = 21; // Đèn xanh dương

const int buttonPin = 12; // Nút nhấn
bool isPaused = false;    // Biến kiểm soát dừng hiển thị
unsigned long lastButtonPress = 0; // Lưu thời gian nhấn nút cuối cùng

const int CLK = 23;
const int DIO = 22;
const int ldrPin = 13;
TM1637Display display(CLK, DIO);

unsigned long previousMillis = 0;
long interval = 1000;
int countdown = 0;
int state = 0;

void setup() {
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);   // Cấu hình đèn xanh dương
    pinMode(buttonPin, INPUT_PULLUP); // Tránh nhiễu

    Serial.begin(9600);
    display.setBrightness(7);

    digitalWrite(greenPin, HIGH);
    Serial.println("Đèn xanh (5 giây)");
    countdown = 5;
    state = 0;
}

void changeLight() {
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);

    if (state == 0) {
        digitalWrite(yellowPin, HIGH);
        Serial.println("Đèn vàng (2 giây)");
        countdown = 2;
        state = 1;
    } else if (state == 1) {
        digitalWrite(redPin, HIGH);
        Serial.println("Đèn đỏ (5 giây)");
        countdown = 5;
        state = 2;
    } else {
        digitalWrite(greenPin, HIGH);
        Serial.println("Đèn xanh (5 giây)");
        countdown = 5;
        state = 0;
    }
}

// Kiểm tra nút nhấn với debounce bằng millis()
void checkButtonPress() {
    if (digitalRead(buttonPin) == LOW) { // Khi nút được nhấn
        unsigned long currentMillis = millis();
        if (currentMillis - lastButtonPress > 200) { // Chống dội phím bằng millis()
            isPaused = !isPaused; // Đảo trạng thái dừng/mở hiển thị
            lastButtonPress = currentMillis; // Cập nhật thời gian nhấn cuối cùng
            
            if (isPaused) {
                digitalWrite(bluePin, HIGH); // Bật đèn xanh dương khi màn hình dừng
            } else {
                digitalWrite(bluePin, LOW);  // Tắt đèn xanh dương khi màn hình chạy
            }

            Serial.print("Trạng thái hiển thị: ");
            Serial.println(isPaused ? "DỪNG" : "CHẠY");
        }
    }
}

void loop() {
    checkButtonPress(); // Kiểm tra nút nhấn non-blocking

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (countdown < 0) {
            changeLight();
        }

        Serial.print("Còn ");
        Serial.print(countdown);
        Serial.println(" giây");

        if (!isPaused) {
            display.showNumberDec(countdown, true);
        }

        countdown--;
    }
}
