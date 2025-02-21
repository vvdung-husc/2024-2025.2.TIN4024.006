#include <Arduino.h>
#include <TM1637Display.h>

// Khai báo chân điều khiển LED giao thông
#define ledPinred 5
#define ledPingreen 4
#define ledPinyellow 16

// Thời gian sáng của mỗi đèn (ms)
const uint32_t RED_TIME = 10000;
const uint32_t YELLOW_TIME = 3000;
const uint32_t GREEN_TIME = 5000;

uint32_t previousTime = 0;
int currentState = 0;
int countdown = 0;  // Biến đếm lùi

// Khai báo TM1637
#define CLK 25  // Chân CLK của TM1637
#define DIO 26  // Chân DIO của TM1637
TM1637Display display(CLK, DIO);

#define buttonPin 14
bool isDisplayOn = true;
bool lastButtonState = LOW;
unsigned long lastDebouceTime = 0; //Hiển thị thời gian nhấn trước
const unsigned long debouceDelay = 50; // Hiển thị thời gian trễ để nhấn nút

bool isReady(uint32_t &ulTimer, uint32_t milisecond) {
  uint32_t t = millis();
  if (t - ulTimer < milisecond) return false;
  
  ulTimer = t;
  return true;
}



// Hàm hiển thị số giây đếm lùi lên TM1637
void updateDisplay(int seconds) {
  if(isDisplayOn){
    display.showNumberDec(seconds, true);
  }else{
    display.clear();
  }
}

void checkButtonPress() {
  bool buttonState = digitalRead(buttonPin);
  
  if (buttonState == LOW && lastButtonState == HIGH) {
    if (millis() - lastDebouceTime > debouceDelay) {
      isDisplayOn = !isDisplayOn;  // Đảo trạng thái hiển thị
      updateDisplay(countdown);    // Cập nhật hiển thị theo trạng thái mới
      Serial.println(isDisplayOn ? "Màn hình bật" : "Màn hình tắt");
      lastButtonState = millis();
    }
  }
  lastButtonState = buttonState;
}

void Use_Non_Blocking() {
  static uint32_t countdownTimer = millis();

  // Giảm giá trị đếm lùi mỗi giây
  if (millis() - countdownTimer >= 1000) {
    countdownTimer = millis();
    if (countdown > 0) {
      countdown--;
      updateDisplay(countdown);
    }
  }

  switch (currentState) {
    case 0: // Đèn đỏ
      if (isReady(previousTime, RED_TIME)) {
        digitalWrite(ledPinred, LOW);
        digitalWrite(ledPinyellow, HIGH);
        currentState = 1;
        countdown = YELLOW_TIME / 1000; // Cập nhật đếm lùi cho đèn vàng
        Serial.println("Đèn vàng sáng");
      }
      break;

    case 1: // Đèn vàng
      if (isReady(previousTime, YELLOW_TIME)) {
        digitalWrite(ledPinyellow, LOW);
        digitalWrite(ledPingreen, HIGH);
        currentState = 2;
        countdown = GREEN_TIME / 1000; // Cập nhật đếm lùi cho đèn xanh
        Serial.println("Đèn xanh sáng");
      }
      break;

    case 2: // Đèn xanh
      if (isReady(previousTime, GREEN_TIME)) {
        digitalWrite(ledPingreen, LOW);
        digitalWrite(ledPinred, HIGH);
        currentState = 0;
        countdown = RED_TIME / 1000; // Cập nhật đếm lùi cho đèn đỏ
        Serial.println("Đèn đỏ sáng");
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Cấu hình chân LED
  pinMode(ledPinred, OUTPUT);
  pinMode(ledPingreen, OUTPUT);
  pinMode(ledPinyellow, OUTPUT);
  // pinMode(buttonPin, INPUT_PULLUP);

  // Khởi động đèn đỏ trước
  digitalWrite(ledPinred, HIGH);
  digitalWrite(ledPingreen, LOW);
  digitalWrite(ledPinyellow, LOW);

  // Khởi tạo TM1637
  display.setBrightness(7);  // Độ sáng (0-7)
  countdown = RED_TIME / 1000; // Đếm lùi bắt đầu từ đèn đỏ
  updateDisplay(countdown);
}

void loop() { 
  checkButtonPress();
  Use_Non_Blocking();
}
