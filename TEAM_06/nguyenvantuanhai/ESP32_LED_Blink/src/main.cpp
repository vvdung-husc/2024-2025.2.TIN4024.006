#include <Arduino.h>
#include <TM1637Display.h>

// Pin kết nối LED
#define rLED  5
#define yLED  17
#define gLED  16

// Pin kết nối module 7 đoạn TM1637
#define CLK   23
#define DIO   22

// Thời gian delay(miliseconds)
#define rTIME  5000   // Đỏ: 5 giây
#define yTIME  4000   // Vàng: 4 giây
#define gTIME  10000   // Xanh: 10 giây

// Biến thời gian
ulong ledTimeStart = 0; // Thời điểm bật đèn hiện tại
ulong lastUpdate = 0;    // Lần cuối cập nhật bộ đếm
int currentLED = rLED;  // Bắt đầu từ đèn đỏ
int timeLeft = rTIME / 1000; // Thời gian còn lại

TM1637Display display(CLK, DIO);

// Hàm kiểm tra thời gian có đủ điều kiện để chuyển đèn hay không
bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (millis() - ulTimer < milisecond) return false;
  ulTimer = millis();  // Cập nhật lại thời gian khi chuyển màu đèn
  return true;
}

// Cập nhật thời gian đếm ngược
void UpdateCountdown() {
  if (millis() - lastUpdate >= 1000) { // Giảm thời gian đếm mỗi giây
    lastUpdate = millis();
    timeLeft--; 
    if (timeLeft < 0) timeLeft = 0; // Đảm bảo không bị giá trị âm
    display.showNumberDec(timeLeft, true, 2, 2); // Hiển thị trên màn hình
  }
}

// Điều khiển đèn giao thông không chặn
void NonBlocking_Traffic_Light() {
  UpdateCountdown(); // Cập nhật số giây còn lại lên màn hình

  switch (currentLED) {
    case rLED: // Đèn đỏ -> Đèn xanh
      if (IsReady(ledTimeStart, rTIME)) {
        digitalWrite(rLED, LOW);
        digitalWrite(gLED, HIGH);
        currentLED = gLED;
        timeLeft = gTIME / 1000; // Đếm 
        Serial.println("2. GREEN\t => Chuyển sang đèn xanh");
      }
      break;

    case gLED: // Đèn xanh -> Đèn vàng
      if (IsReady(ledTimeStart, gTIME)) {
        digitalWrite(gLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = yLED;
        timeLeft = yTIME / 1000;
        Serial.println("3. YELLOW\t => Chuyển sang đèn vàng");
      }
      break;

    case yLED: // Đèn vàng -> Đèn đỏ
      if (IsReady(ledTimeStart, yTIME)) {
        digitalWrite(yLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        timeLeft = rTIME / 1000;
        Serial.println("1. RED\t\t => Chuyển sang đèn đỏ");
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);

  display.setBrightness(7);

  // Khởi động với đèn đỏ
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);

  ledTimeStart = millis();  // Bắt đầu đếm thời gian từ hiện tại
  timeLeft = rTIME / 1000;   // Đếm

  Serial.println("== START ==> Đèn giao thông hoạt động!");
}

void loop() {
  NonBlocking_Traffic_Light();
}
