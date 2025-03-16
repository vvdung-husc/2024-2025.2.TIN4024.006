#include <Arduino.h>
#include <TM1637Display.h>

// Pin đèn giao thông
#define rLED  5
#define yLED  16
#define gLED  15

// Pin điều khiển TM1637
#define CLK   23
#define DIO   22

// Định nghĩa thời gian cho mỗi đèn
#define rTIME  5   // 5 giây cho đèn đỏ
#define yTIME  2   // 2 giây cho đèn vàng
#define gTIME  7   // 7 giây cho đèn xanh

ulong ledTimeStart = 0; // Lưu thời gian bắt đầu của LED hiện tại
int currentLED = rLED;  // Đèn hiện tại (bắt đầu với đèn đỏ)
int countdown = rTIME;  // Biến đếm ngược

TM1637Display display(CLK, DIO); // Khởi tạo màn hình LED

void setup() {
  Serial.begin(115200);
  
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  
  display.setBrightness(7); // Đặt độ sáng tối đa

  // Bật đèn đỏ đầu tiên
  digitalWrite(rLED, HIGH);
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  ledTimeStart = millis();
  
  Serial.println("== START ==> RED");
  display.showNumberDec(countdown, true, 2, 2); // Hiển thị thời gian ban đầu
}
void changeLight() {
  // Tắt đèn hiện tại
  digitalWrite(currentLED, LOW);

  // Chuyển sang đèn tiếp theo
  if (currentLED == rLED) {
    currentLED = gLED;
    countdown = gTIME;
    Serial.println("GREEN ON");
  } 
  else if (currentLED == gLED) {
    currentLED = yLED;
    countdown = yTIME;
    Serial.println("YELLOW ON");
  } 
  else if (currentLED == yLED) {
    currentLED = rLED;
    countdown = rTIME;
    Serial.println("RED ON");
  }

  // Bật đèn mới
  digitalWrite(currentLED, HIGH);
  display.showNumberDec(countdown, true, 2, 2); // Hiển thị thời gian mới

  ledTimeStart = millis(); // Cập nhật lại thời gian bắt đầu
}
void loop() {
  ulong currentMillis = millis(); // Lấy thời gian hiện tại

  // Kiểm tra nếu đã qua 1 giây -> Cập nhật đếm ngược
  if (currentMillis - ledTimeStart >= 1000) {
    ledTimeStart = currentMillis; // Cập nhật thời gian mới
    countdown--; // Giảm thời gian đếm ngược

    // Hiển thị số đếm trên màn hình TM1637
    display.showNumberDec(countdown, true, 2, 2);
    Serial.print("Countdown: "); Serial.println(countdown);

    // Nếu đếm ngược về 0, chuyển sang đèn tiếp theo
    if (countdown == 0) {
      changeLight();
    }
  }
}
