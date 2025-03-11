#include <Arduino.h>
#include <TM1637Display.h>

// Khai báo chân đèn giao thông
int redPin = 27;
int yellowPin = 26;
int greenPin = 25;
int bluePin = 21; // Đèn xanh dương

const int buttonPin = 23; // Nút nhấn
bool isPaused = false;    // Biến kiểm soát dừng hiển thị
unsigned long lastButtonPress = 0;
const float GAMMA = 0.7;
const float RL10 = 50;

const int CLK = 18;
const int DIO = 19;
const int ldrPin = 13; // Cảm biến ánh sáng
TM1637Display display(CLK, DIO);

unsigned long previousMillis = 0;
long interval = 1000;
int countdown = 0;
int state = 0;
bool nightMode = false; // Chế độ ban đêm

const int NUM_SAMPLES = 5;
float getLuxFiltered()

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
                display.clear(); // Tắt màn hình số
            } else {
                digitalWrite(bluePin, LOW);
                display.showNumberDec(countdown, true); // Hiển thị lại số
            }

            Serial.print("Trạng thái hiển thị: ");
            Serial.println(isPaused ? "TẮT MÀN HÌNH" : "BẬT MÀN HÌNH");
        }
    }
}

// Hàm tính Lux từ cảm biến LDR
float calculateLux(int adcValue) {
    if (adcValue == 0) return 0; // Tránh lỗi chia cho 0

    // Chuyển đổi giá trị ADC thành điện áp (ESP32 có ADC 12-bit: 0 - 4095)
    float voltage = adcValue / 4095.0 * 3.3;
    
    // Tính toán giá trị điện trở của LDR
    float resistance = 2000 * voltage / (1 - voltage / 5);
    
    // Tính toán lux dựa trên công thức tham khảo
    float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

    return lux;
}

void loop() {
    checkButtonPress(); // Kiểm tra nút nhấn non-blocking

    int ldrValue = analogRead(ldrPin); // Đọc giá trị từ LDR
    float lux = calculateLux(ldrValue); // Chuyển đổi sang lux

    // 🟡 In giá trị LDR & Lux ra Serial để kiểm tra
    // Serial.print("LDR ADC: "); Serial.print(ldrValue);
    Serial.print("Lux: "); Serial.println(lux);

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