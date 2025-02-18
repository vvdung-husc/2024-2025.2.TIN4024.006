#include <Arduino.h>

int redPin = 5; // Chân GPIO 2 (thường nối sẵn LED trên board)
int yellowPin = 22;
int greenPin = 4;

// Thời gian (ms) mỗi đèn sáng
const uint32_t redDuration = 5000;    // 5 giây
const uint32_t yellowDuration = 2000; // 2 giây
const uint32_t greenDuration = 5000;  // 5 giây

// Biến theo dõi trạng thái đèn và thời gian
enum LightState { RED, YELLOW, GREEN }; // Trạng thái của đèn
LightState currentState = RED;         // Trạng thái hiện tại (bắt đầu với đèn đỏ)
ulong lastChangeTime = 0;              // Thời điểm đèn chuyển trạng thái

// Hàm kiểm tra xem đã đủ thời gian để chuyển trạng thái chưa
bool IsReady(ulong& ulTimer, uint32_t duration) {
  ulong t = millis();
  if (t - ulTimer < duration) return false; // Nếu chưa đủ thời gian, trả về false
  ulTimer = t;                              // Cập nhật lại thời gian
  return true;
}

void setup() {
  Serial.begin(115200);

  // Cài đặt các chân LED làm đầu ra
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  // Bật đèn đỏ ban đầu
  digitalWrite(redPin, HIGH);
}

void loop() {
  // Kiểm tra thời gian đã đủ để chuyển trạng thái chưa
  switch (currentState) {
    case RED:
      if (IsReady(lastChangeTime, redDuration)) {
        // Chuyển sang đèn xanh
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, HIGH);
        currentState = GREEN;
        Serial.println("Đèn xanh -> ON");
      }
      break;

    case GREEN:
      if (IsReady(lastChangeTime, greenDuration)) {
        // Chuyển sang đèn vàng
        digitalWrite(greenPin, LOW);
        digitalWrite(yellowPin, HIGH);
        currentState = YELLOW;
        Serial.println("Đèn vàng -> ON");
      }
      break;

    case YELLOW:
      if (IsReady(lastChangeTime, yellowDuration)) {
        // Chuyển sang đèn đỏ
        digitalWrite(yellowPin, LOW);
        digitalWrite(redPin, HIGH);
        currentState = RED;
        Serial.println("Đèn đỏ -> ON");
      }
      break;
  }

  // Debug thời gian
  ulong t = millis();
  Serial.print("Timer: ");
  Serial.println(t);
}