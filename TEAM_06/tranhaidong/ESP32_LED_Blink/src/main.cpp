#include <Arduino.h>
#include <TimeLib.h>  // Thư viện hỗ trợ lấy giờ

// Khai báo các chân điều khiển các đèn LED
int ledRed = 17;
int ledYellow = 4;
int ledGreen = 5;

// Thời gian bật đèn (tính bằng milliseconds)
unsigned long greenDuration = 10000;  // 10 giây
unsigned long yellowDuration = 3000;  // 3 giây
unsigned long redDuration = 5000;     // 5 giây

// Biến lưu trữ thời gian cho đèn giao thông
unsigned long lastChangeTime = 0;
int currentState = 0;  // 0: xanh, 1: vàng, 2: đỏ
unsigned long blinkStartTime = 0;

// Hàm bật đèn
void turnOnLed(int ledPin) {
  digitalWrite(ledPin, HIGH);
}

// Hàm tắt đèn
void turnOffLed(int ledPin) {
  digitalWrite(ledPin, LOW);
}

bool IsReady(unsigned long& ulTimer, uint32_t milliseconds) {
  unsigned long currentTime = millis();
  if (currentTime - ulTimer < milliseconds) {
    return false;
  }
  ulTimer = currentTime;
  return true;
}

void setup() {
  Serial.begin(115200);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  // Khởi tạo thời gian hệ thống
  setTime(6, 0, 0, 1, 1, 2025);  // Ví dụ: set giờ là 6h sáng ngày 1 tháng 1 năm 2025
}

void loop() {
  // Lấy giờ hiện tại
  int currentHour = hour();

  // Nếu giờ trong khoảng từ 6h đến 22h
  if (currentHour >= 6 && currentHour < 22) {
    // Luân phiên đèn giao thông
    if (IsReady(lastChangeTime, currentState == 0 ? greenDuration : (currentState == 1 ? yellowDuration : redDuration))) {
      // Tắt tất cả đèn
      turnOffLed(ledRed);
      turnOffLed(ledYellow);
      turnOffLed(ledGreen);

      // Bật đèn theo trạng thái
      if (currentState == 0) {
        turnOnLed(ledGreen);  // Bật đèn xanh
      } else if (currentState == 1) {
        turnOnLed(ledYellow);  // Bật đèn vàng
      } else {
        turnOnLed(ledRed);  // Bật đèn đỏ
      }

      // Chuyển sang trạng thái tiếp theo
      currentState = (currentState + 1) % 3;
    }
  }
  // Nếu ngoài khoảng 22h, chỉ bật đèn vàng nhấp nháy
  else {
    if (IsReady(blinkStartTime, 500)) {  // Mỗi 500ms nhấp nháy
      // Tắt đèn vàng nếu đang bật
      if (digitalRead(ledYellow) == HIGH) {
        turnOffLed(ledYellow);
      } else {
        turnOnLed(ledYellow);  // Bật đèn vàng
      }
    }
  }
}
