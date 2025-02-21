#include <Arduino.h>
#include <TM1637Display.h>

#define RED_LED 5
#define YELLOW_LED 16
#define GREEN_LED 4

const int CLK = 23;
const int DIO = 22;

// Định nghĩa thời gian cho mỗi đèn (tính bằng giây)
const int GREEN_TIME = 5;  // Đèn xanh 5 giây
const int YELLOW_TIME = 3;  // Đèn vàng 3 giây
const int RED_TIME = 5;    // Đèn đỏ 5 giây

ulong previousMillis = 0;
int state = 0;  // 0: đỏ, 1: xanh, 2: vàng
int countdown = 0;
bool needChange = false;  // Biến đánh dấu cần chuyển đèn
TM1637Display display(CLK, DIO);

void setup() {
  Serial.begin(115200);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  display.setBrightness(7);
  
  // Bắt đầu với đèn đỏ
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  
  countdown = RED_TIME - 1;  // Bắt đầu từ 4 thay vì 5
  display.showNumberDec(countdown, true);
}

void loop() {
  ulong currentMillis = millis();

  // Xử lý đếm ngược
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    
    Serial.printf("Đèn %s: Còn %d giây", 
      state == 0 ? "ĐỎ" : (state == 1 ? "XANH" : "VÀNG"),
      countdown);
    display.showNumberDec(countdown, true);
    
    if (countdown == 0) {
      needChange = true;  // Đánh dấu cần chuyển đèn ở chu kỳ tiếp theo
    }
    
    countdown--;  // Giảm bộ đếm
    
    // Xử lý chuyển trạng thái sau khi hiển thị 0
    if (needChange) {
      needChange = false;  // Reset biến đánh dấu
      
      switch (state) {
        case 0:  // Đỏ -> Xanh
          digitalWrite(RED_LED, LOW);
          digitalWrite(GREEN_LED, HIGH);
          state = 1;
          countdown = GREEN_TIME - 1;  // Bắt đầu từ số 4
          break;
          
        case 1:  // Xanh -> Vàng
          digitalWrite(GREEN_LED, LOW);
          digitalWrite(YELLOW_LED, HIGH);
          state = 2;
          countdown = YELLOW_TIME - 1;  // Bắt đầu từ số 2
          break;
          
        case 2:  // Vàng -> Đỏ
          digitalWrite(YELLOW_LED, LOW);
          digitalWrite(RED_LED, HIGH);
          state = 0;
          countdown = RED_TIME - 1;  // Bắt đầu từ số 4
          break;
      }
    }
  }
}