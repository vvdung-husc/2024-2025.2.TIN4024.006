#include <Arduino.h>
#include <TM1637Display.h>

// Định nghĩa các chân cho đèn LED
const int RED_LED = 4;    // D2 trên NodeMCU
const int YELLOW_LED = 2; // D4 trên NodeMCU
const int GREEN_LED = 15; // D8 trên NodeMCU

// Định nghĩa chân cho TM1637 (sửa theo sơ đồ)
const int CLK = 16;  // D0 trên NodeMCU (theo sơ đồ)
const int DIO = 0;   // D3 trên NodeMCU (theo sơ đồ)

// Thời gian cho từng giai đoạn (đơn vị: mili giây)
const int RED_TIME = 5000;    // 5 giây
const int GREEN_TIME = 7000;  // 7 giây
const int YELLOW_TIME = 2000; // 2 giây

// Khởi tạo đối tượng TM1637
TM1637Display display(CLK, DIO);

// Khai báo hàm countdown trước khi dùng
void countdown(int seconds);

void setup() {
  // Cấu hình các chân LED là OUTPUT
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  
  // Tắt tất cả đèn khi khởi động
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  
  // Khởi tạo Serial để debug
  Serial.begin(115200);
  Serial.println("Traffic Light with TM1637 Started");
  
  // Khởi tạo TM1637
  display.setBrightness(0x0f); // Độ sáng tối đa (0x00 - 0x0f)
  display.clear();
}

void loop() {
  // Đèn đỏ sáng với đếm ngược
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  Serial.println("Red Light ON");
  countdown(RED_TIME / 1000); // Chuyển sang giây
  
  // Đèn xanh sáng với đếm ngược
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  Serial.println("Green Light ON");
  countdown(GREEN_TIME / 1000);
  
  // Đèn vàng sáng với đếm ngược
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  Serial.println("Yellow Light ON");
  countdown(YELLOW_TIME / 1000);
}

// Định nghĩa hàm countdown
void countdown(int seconds) {
  for (int i = seconds; i >= 0; i--) {
    display.showNumberDec(i, false); // Hiển thị số, không dẫn 0
    delay(1000); // Đợi 1 giây
  }
}