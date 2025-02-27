#include <Arduino.h>
#include <TM1637Display.h>

// Pin kết nối 
#define rLED  27
#define yLED  26
#define gLED  25
#define blueLED 21
#define CLK   18
#define DIO   19
#define BUTTON_PIN 23
#define LDR_PIN 13

// Thời gian của mỗi đèn (ms)
#define rTIME  5000
#define yTIME  2000
#define gTIME  5000

TM1637Display display(CLK, DIO);
// biến thời gian và trạng thái
ulong currentMiliseconds = 0;
ulong ledTimeStart = 0;
int currentLED = rLED;
bool isPaused = false;
int counter = rTIME / 1000;

void togglePause();
bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light();
void Testing_Display();

void setup() {
    Serial.begin(115200);
    pinMode(rLED, OUTPUT);
    pinMode(yLED, OUTPUT);
    pinMode(gLED, OUTPUT);
    pinMode(blueLED, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);
    
    display.setBrightness(7);
    digitalWrite(yLED, LOW);
    digitalWrite(gLED, LOW);
    digitalWrite(rLED, HIGH);
    currentLED = rLED;
    
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), togglePause, FALLING);
    Serial.println("== START ==>");
    Serial.print("1. RED \t\t => Next "); 
    Serial.print(counter);
    Serial.println("s");
}

void loop() {
    int lightLevel = analogRead(LDR_PIN);
    if (lightLevel < 500) {  // Trời tối
        digitalWrite(blueLED, HIGH);
        digitalWrite(rLED, LOW);
        digitalWrite(yLED, LOW);
        digitalWrite(gLED, LOW);
    } else {
        if (!isPaused) {
            currentMiliseconds = millis();
            NonBlocking_Traffic_Light();
            Testing_Display();
        }
    }
}

void togglePause() {
    isPaused = !isPaused;
}

void Testing_Display() {
    display.showNumberDec(counter, true, 3, 1);
    if (counter > 0) {
        counter--;
    }
    delay(1000);
}

bool IsReady(ulong &ulTimer, uint32_t milisecond) {
    if (currentMiliseconds - ulTimer < milisecond) return false;
    ulTimer = currentMiliseconds;
    return true;
}

void NonBlocking_Traffic_Light() {
    switch (currentLED) {
        case rLED: // Đèn đỏ: 5 giây
            if (IsReady(ledTimeStart, rTIME)) {
                digitalWrite(rLED, LOW);
                digitalWrite(gLED, HIGH);
                currentLED = gLED;
                counter = gTIME / 1000;
                Serial.print("2. GREEN\t => Next "); 
                Serial.print(counter);
                Serial.println("s");
            }
            break;

        case gLED: // Đèn xanh: 5 giây
            if (IsReady(ledTimeStart, gTIME)) {
                digitalWrite(gLED, LOW);
                digitalWrite(yLED, HIGH);
                currentLED = yLED;
                counter = yTIME / 1000;
                Serial.print("3. YELLOW\t => Next "); 
                Serial.print(counter);
                Serial.println("s");
            }
            break;

        case yLED: // Đèn vàng: 2 giây
            if (IsReady(ledTimeStart, yTIME)) {
                digitalWrite(yLED, LOW);
                digitalWrite(rLED, HIGH);
                currentLED = rLED;
                counter = rTIME / 1000;
                Serial.print("1. RED \t\t => Next "); 
                Serial.print(counter);
                Serial.println("s");
            }
            break;
    }
}