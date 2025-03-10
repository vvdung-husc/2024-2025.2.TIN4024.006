#include <Arduino.h>
#include <TM1637Display.h>
#include <dht.h>

//LE HUU NHAT
#define BLYNK_TEMPLATE_ID "TMPL6VOd5sqsE"
#define BLYNK_TEMPLATE_NAME "ESP32 LED TM1637"
#define BLYNK_AUTH_TOKEN "-cGkO50zH-imn-5AGyb3gUYL_bWPNMB0"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu cho việc chạy mô phỏng
char ssid[] = "Wokwi-GUEST";  //Tên mạng WiFi
char pass[] = "";             //Mật khẩu mạng WiFi



#define ledRed 27
#define ledYellow 26
#define ledGreen 25

#define DHTTYPE DHT22 
#define DHT22_PIN 16
DHT dht(DHT22_PIN, DHTTYPE);

#define CLK 18  //Chân kết nối CLK của TM1637
#define DIO 19  //Chân kết nối DIO của TM1637
TM1637Display display(CLK, DIO);



const uint32_t RED_TIME = 5000;
const uint32_t YELLOW_TIME = 3000;
const uint32_t GREEN_TIME = 10000;


int currentState = 0;
int countdown = 0;
uint32_t preTime = 0;

const float GAMMA = 0.7;
const float RL10 = 50;

#define btnBLED  23 //Chân kết nối nút bấm
#define pinBLED  21 //Chân kết nối đèn xxanh


#define ldrPin 13 // Chân analog của LDR
bool isDisplayOn = true;
bool isLedOn = false;
bool lastButtonState = LOW;
unsigned long lastDebouceTime = 0;
const unsigned long debouceDelay = 50;

void DoamBlynk(){
  float h = dht.readHumidity();
  Blynk.virtualWrite(V3, h);  
}

void NhietdoBlynk(){
  float t = dht.readTemperature();
  Blynk.virtualWrite(V2, t); 
  
}

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
    bool buttonState = digitalRead(btnBLED);

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
        digitalWrite(pinBLED, LOW);
    } else {
        digitalWrite(pinBLED, HIGH);
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
    Serial.print("Lux: ");
    Serial.println(ldrValue);
    
    if (ldrValue < 500) {
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
    pinMode(pinBLED, OUTPUT);
    pinMode(ldrPin, INPUT);

    digitalWrite(ledRed, LOW);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(pinBLED, LOW);

    Serial.begin(115200);
    display.setBrightness(0x0f);
    countdown = GREEN_TIME / 1000;
    UpdateDisplay(countdown);
      // Start the WiFi connection
    Serial.print("Connecting to ");Serial.println(ssid);
    Blynk.begin(BLYNK_AUTH_TOKEN,ssid, pass); //Kết nối đến mạng WiFi

    Serial.println();
    Serial.println("WiFi connected");

    
    digitalWrite(pinBLED, isDisplayOn? HIGH : LOW);  
    Blynk.virtualWrite(V1, isDisplayOn); //Đồng bộ trạng thái trạng thái của đèn với Blynk
    
    Serial.println("== START ==>");
}

void loop() {
    Blynk.run();
    checkButtonPress();
    checkLed();
    Non_block();
    DoamBlynk();
    NhietdoBlynk();
}