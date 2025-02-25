#include <Arduino.h>
#include <TM1637Display.h>


#define led 21
#define ledRed 27
#define ledYellow 26
#define ledGreen 25

#define CLK 18
#define DIO 19
TM1637Display display(CLK, DIO);

const uint32_t RED_TIME = 5000;
const uint32_t YELLOW_TIME = 3000;
const uint32_t GREEN_TIME = 10000;

int currentState = 0;
int countdown = 0;
uint32_t preTime = 0;

const float GAMMA = 0.7;
const float RL10 = 50;

#define buttonPin 23
#define ldrPin 13 // Chân analog của LDR
bool isDisplayOn = true;
bool isLedOn = false;
bool lastButtonState = LOW;
unsigned long lastDebouceTime = 0;
const unsigned long debouceDelay = 50;

bool isReady(uint32_t &ulTimer, uint32_t milisecond) {
    uint32_t t = millis();
    if (t - ulTimer < milisecond) return false;

    ulTimer = t;
    return true;
}

void UpdateDisplay(int seconds) {
    if (isDisplayOn) {
        display.showNumberDec(seconds, true);
    } else {
        display.clear();
    }
}

void checkButtonPress() {
    bool buttonState = digitalRead(buttonPin);

    if (buttonState == LOW && lastButtonState == HIGH) {
        if (millis() - lastDebouceTime > debouceDelay) {
            isDisplayOn = !isDisplayOn;  // Đảo trạng thái hiển thị
            isLedOn = !isLedOn;
            UpdateDisplay(countdown);    // Cập nhật hiển thị theo trạng thái mới
            Serial.println(isDisplayOn ? "Màn hình bật" : "Màn hình tắt");

            lastButtonState = millis();
        }
    }

    lastButtonState = buttonState;
}

void checkLed() {
    if (isLedOn) {
        digitalWrite(led, LOW);
    } else {
        digitalWrite(led, HIGH);
    }
}

void blinkYellowLed() {
    static uint32_t lastBlinkTime = 0;
    static bool ledState = LOW;

    if (millis() - lastBlinkTime >= 500) { // Nhấp nháy mỗi 500ms
        lastBlinkTime = millis();
        ledState = !ledState;
        digitalWrite(ledYellow, ledState);
    }
}

void Non_block() {
    static uint32_t countdownTimer = millis();

    // Giảm giá trị đếm lùi mỗi giây
    if (millis() - countdownTimer >= 1000) {
        countdownTimer = millis();
        if (countdown > 0) {
            countdown--;
            UpdateDisplay(countdown);
        }
    }

    int ldrValue = analogRead(ldrPin);
    float analogValue = 250250 / ldrValue;
    float voltage = analogValue / 1024. * 5;
    float resistance = 2000 * voltage / (1 - voltage / 5);
    float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
    
    Serial.print("Lux: ");
    Serial.println(lux);
    
    if (lux < 50) {
        // Nếu ánh sáng yếu, đèn vàng nhấp nháy, tắt các đèn khác
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, LOW);
        blinkYellowLed();
        return;
    }

    switch (currentState) {
        case 0:
            if (isReady(preTime, GREEN_TIME)) {
                digitalWrite(ledGreen, LOW);
                digitalWrite(ledYellow, HIGH);
                currentState = 1;
                countdown = YELLOW_TIME / 1000;
                Serial.println("Đèn vàng sáng!");
            }
            break;
        case 1:
            if (isReady(preTime, YELLOW_TIME)) {
                digitalWrite(ledYellow, LOW);
                digitalWrite(ledRed, HIGH);
                currentState = 2;
                countdown = RED_TIME / 1000;
                Serial.println("Đèn đỏ sáng!");
            }
            break;
        case 2:
            if (isReady(preTime, RED_TIME)) {
                digitalWrite(ledRed, LOW);
                digitalWrite(ledGreen, HIGH);
                currentState = 0;
                countdown = GREEN_TIME / 1000;
                Serial.println("Đèn xanh sáng!");
            }
            break;
    }
}

void setup() {
    pinMode(ledRed, OUTPUT);
    pinMode(ledYellow, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(led, OUTPUT);
    pinMode(ldrPin, INPUT);

    digitalWrite(ledRed, LOW);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(led, LOW);

    Serial.begin(115200);
    display.setBrightness(7);
    countdown = GREEN_TIME / 1000;
    UpdateDisplay(countdown);
}

void loop() {
    checkButtonPress();
    checkLed();
    Non_block();
}