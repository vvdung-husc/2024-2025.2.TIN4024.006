#include <Arduino.h>
#include <TM1637Display.h>

// Định nghĩa chân kết nối
#define BUTTON_PIN 23
#define LED_BLUE 21
#define LED_GREEN 25
#define LED_YELLOW 26
#define LED_RED 27
#define LDR_PIN 13
#define CLK 18
#define DIO 19

// Khởi tạo đối tượng hiển thị TM1637
TM1637Display display(CLK, DIO);

// Biến toàn cục
int buttonState = 0;
int lastButtonState = HIGH;
bool displayOn = true;
int ldrValue = 0;
int countdown = 5; // Bắt đầu đếm từ 10 (cho đèn xanh)
unsigned long previousMillis = 0;
const long interval = 1000; // Thời gian đếm lùi (1 giây)
unsigned long blinkMillis = 0;
const long blinkInterval = 500; // Tốc độ nhấp nháy đèn vàng
const long debounceDelay = 50; // Thời gian chống rung nút
unsigned long lastDebounceTime = 0;
bool yellowBlinkState = false; // Trạng thái nhấp nháy của đèn vàng
bool lightStateChanged = true; // Theo dõi thay đổi trạng thái đèn

enum TrafficLightState { GREEN, YELLOW, RED }; // Trạng thái đèn giao thông
TrafficLightState lightState = GREEN;         // Đèn bắt đầu từ xanh

void setup() {
    // Cài đặt chế độ cho các chân
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LDR_PIN, INPUT);

    // Cấu hình hiển thị
    display.setBrightness(0x0F); // Độ sáng tối đa
    display.showNumberDec(countdown);

    // Khởi động Serial để debug
    Serial.begin(115200);
    Serial.println("Hệ thống đèn giao thông khởi động...");
}

void loop() {
    unsigned long currentMillis = millis();

    // Đọc giá trị cảm biến ánh sáng (LDR) và chuyển đổi sang lux
    int ldrRaw = analogRead(LDR_PIN);
    float ldrLux = map(ldrRaw, 0, 4095, 0, 1000); // Giả định 0-4095 tương ứng 0-1000 lux

    // Kiểm tra nút nhấn để bật/tắt hiển thị
    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == LOW && lastButtonState == HIGH && (currentMillis - lastDebounceTime) > debounceDelay) {
        displayOn = !displayOn;
        if (!displayOn) {
            display.clear();
        }
        lastDebounceTime = currentMillis;
    }
    lastButtonState = buttonState;

    // Nếu trời tối hoàn toàn (lux < 10), bật chế độ ban đêm
    if (ldrLux < 10) {
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_BLUE, HIGH); // Bật đèn xanh dương (chế độ ban đêm)

        if (displayOn) display.clear(); // Tắt màn hình hiển thị
        Serial.println("Chế độ ban đêm: Bật đèn xanh dương (BLUE)");
        return; // Dừng loop để không chạy logic đèn giao thông
    } else {
        digitalWrite(LED_BLUE, LOW); // Tắt đèn xanh dương khi trời sáng hơn
    }

    // Nếu trời tối nhẹ (10 ≤ lux < 50), bật đèn vàng nhấp nháy
    if (ldrLux < 50) {
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_BLUE, HIGH); // Bật đèn xanh dương sáng liên tục
  
      if (currentMillis - blinkMillis >= blinkInterval) {
          blinkMillis = currentMillis;
          yellowBlinkState = !yellowBlinkState;
          digitalWrite(LED_YELLOW, yellowBlinkState);
      }
  
      if (displayOn) display.clear(); // Tắt màn hình hiển thị
      Serial.println("Chế độ cảnh báo: Đèn vàng nhấp nháy, đèn xanh dương bật");
      return; // Dừng loop để không chạy logic đèn giao thông
  }

    // Nếu trời sáng (lux >= 50), chạy đèn giao thông bình thường
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        countdown--;

        if (countdown < 0) {
            switch (lightState) {
                case GREEN:
                    lightState = YELLOW;
                    countdown = 3;
                    lightStateChanged = true;
                    break;
                case YELLOW:
                    lightState = RED;
                    countdown = 5;
                    lightStateChanged = true;
                    break;
                case RED:
                    lightState = GREEN;
                    countdown = 5;
                    lightStateChanged = true;
                    break;
            }
        }

        if (displayOn) {
            display.showNumberDec(countdown);
        }
    }

    // Điều khiển đèn giao thông và hiển thị trạng thái
    if (lightStateChanged) {
        switch (lightState) {
            case GREEN:
                digitalWrite(LED_GREEN, HIGH);
                digitalWrite(LED_YELLOW, LOW);
                digitalWrite(LED_RED, LOW);
                Serial.println("Tín hiệu: XANH (GREEN)");
                break;
            case YELLOW:
                digitalWrite(LED_GREEN, LOW);
                digitalWrite(LED_YELLOW, HIGH);
                digitalWrite(LED_RED, LOW);
                Serial.println("Tín hiệu: VÀNG (YELLOW)");
                break;
            case RED:
                digitalWrite(LED_GREEN, LOW);
                digitalWrite(LED_YELLOW, LOW);
                digitalWrite(LED_RED, HIGH);
                Serial.println("Tín hiệu: ĐỎ (RED)");
                break;
        }
        lightStateChanged = false; // Đánh dấu đã cập nhật trạng thái đèn
    }

    // In giá trị ánh sáng và tín hiệu đèn liên tục
    Serial.print("LDR Lux: ");
    Serial.print(ldrLux);
    Serial.print(" | Tín hiệu: ");
    switch (lightState) {
        case GREEN:
            Serial.println("XANH (GREEN)");
            break;
        case YELLOW:
            Serial.println("VÀNG (YELLOW)");
            break;
        case RED:
            Serial.println("ĐỎ (RED)");
            break;
    }
}
