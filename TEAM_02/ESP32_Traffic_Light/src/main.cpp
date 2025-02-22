#include <Arduino.h>
#include <TM1637Display.h>

// Khai báo chân đèn giao thông
int redPin = 5;
int yellowPin = 17;
int greenPin = 16;
int bluePin = 21; // Đèn xanh dương

const int buttonPin = 12; // Nút nhấn
bool isPaused = false;    // Biến kiểm soát dừng hiển thị
unsigned long lastButtonPress = 0;

const int CLK = 23;
const int DIO = 22;
const int ldrPin = 13; // Cảm biến ánh sáng
TM1637Display display(CLK, DIO);

unsigned long previousMillis = 0;
long interval = 1000;
int countdown = 0;
int state = 0;
bool nightMode = false; // Chế độ ban đêm

void setup() {
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT); // Đèn xanh dương

    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ldrPin, INPUT);

    Serial.begin(9600);
    display.setBrightness(7);

    digitalWrite(greenPin, HIGH);
    Serial.println("Đèn xanh (5 giây)");
    countdown = 5;
    state = 0;
}

void changeLight() {
    if (nightMode) return; // Nếu trời tối thì không đổi đèn

    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);

    if (state == 0) { // Đèn vàng
        digitalWrite(yellowPin, HIGH);
        Serial.println("Đèn vàng (2 giây)");
        countdown = 2;
        state = 1;
    } else if (state == 1) { // Đèn đỏ
        digitalWrite(redPin, HIGH);
        Serial.println("Đèn đỏ (5 giây)");
        countdown = 5;
        state = 2;
    } else { // Đèn xanh
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
        if (currentMillis - lastButtonPress > 200) { // Chống dội phím
            isPaused = !isPaused; // Đảo trạng thái
            lastButtonPress = currentMillis; 

            if (isPaused) {
                digitalWrite(bluePin, HIGH);
            } else {
                digitalWrite(bluePin, LOW);
            }

            Serial.print("Trạng thái hiển thị: ");
            Serial.println(isPaused ? "DỪNG" : "CHẠY");
        }
    }
}

// Hàm tính Lux từ cảm biến LDR
float calculateLux(int adcValue) {
    if (adcValue == 0) return 0; // Tránh chia cho 0

    float resistance = (4095.0 / adcValue - 1) * 10000; // 10kΩ là điện trở cố định
    float lux = 500 / (resistance / 1000); // Công thức gần đúng

    return lux;
}

void loop() {
    checkButtonPress(); // Kiểm tra nút nhấn non-blocking

    int ldrValue = analogRead(ldrPin); // Đọc giá trị từ LDR
    float lux = calculateLux(ldrValue); // Chuyển đổi sang lux

    // 🟡 In giá trị LDR & Lux ra Serial để kiểm tra
    Serial.print("LDR ADC: "); Serial.print(ldrValue);
    Serial.print(" → Lux: "); Serial.println(lux);

    if (lux < 50) { // Trời tối → Chỉ đèn vàng sáng, tắt màn hình
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(yellowPin, HIGH);
        display.clear(); // Tắt màn hình số
        nightMode = true; // Bật chế độ ban đêm
        return;
    } else { 
        if (nightMode) { // Nếu chuyển từ chế độ tối sang sáng, reset lại đèn xanh
            digitalWrite(yellowPin, LOW);
            digitalWrite(greenPin, HIGH);
            Serial.println("Chuyển sang chế độ ban ngày - Đèn xanh sáng (5 giây)");
            countdown = 5;
            state = 0;
        }
        nightMode = false; // Tắt chế độ ban đêm
    }

    // Nếu trời sáng → Chạy bình thường
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (!nightMode && countdown < 0) {
            changeLight();
        }

        Serial.print("Còn ");
        Serial.print(countdown);
        Serial.println(" giây");

        if (!isPaused && !nightMode) {
            display.showNumberDec(countdown, true);
        }

        countdown--;
    }
}
