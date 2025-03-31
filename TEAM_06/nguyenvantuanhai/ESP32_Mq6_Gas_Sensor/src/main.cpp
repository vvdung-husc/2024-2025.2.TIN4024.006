#include <Arduino.h>
#include "utils.h"

#define GAS_SENSOR_PIN 34  // Chân SIG của Potentiometer nối với GPIO34
#define BUZZER_PIN 25      // Chân Buzzer
#define LED_GREEN_PIN 18   // LED Xanh (Báo bình thường)
#define LED_RED_PIN 19     // LED Đỏ (Cảnh báo)

#define GAS_THRESHOLD 2.5  // Ngưỡng cảnh báo (điện áp)

unsigned long ulTimer = 0;

void setup() {
  // Khởi tạo các chân
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  
  // Khởi tạo Serial Monitor để theo dõi giá trị cảm biến
  Serial.begin(115200);
}

void loop() {
  // Kiểm tra xem có đủ thời gian không
  if (IsReady(ulTimer, 1000)) {
    int gasValue = analogRead(GAS_SENSOR_PIN);
    float voltage = gasValue * (3.3 / 4095.0);  // Chuyển đổi giá trị đọc từ ADC sang điện áp

    // In ra giá trị điện áp của cảm biến
    Serial.print("Gas Voltage: ");
    Serial.println(voltage);

    // Kiểm tra nếu vượt quá ngưỡng, kích hoạt còi báo động và LED đỏ
    if (voltage > GAS_THRESHOLD) {
      digitalWrite(BUZZER_PIN, HIGH);  // Bật còi báo động
      digitalWrite(LED_GREEN_PIN, LOW);  // Tắt LED xanh
      digitalWrite(LED_RED_PIN, HIGH);  // Bật LED đỏ
      Serial.println("⚠️ Cảnh báo: Phát hiện rò rỉ khí gas!");
    } else {
      digitalWrite(BUZZER_PIN, LOW);   // Tắt còi
      digitalWrite(LED_GREEN_PIN, HIGH);  // Bật LED xanh
      digitalWrite(LED_RED_PIN, LOW);  // Tắt LED đỏ
      Serial.println("💚 An toàn: Không phát hiện rò rỉ khí gas.");
    }
  }
}
