#include <Arduino.h>

int ledPinred = 5;
int ledPingreen = 4;
int ledPinyellow = 16;

// Thời gian sáng của mỗi đèn
const uint32_t RED_TIME = 10000;
const uint32_t YELLOW_TIME = 3000;
const uint32_t GREEN_TIME = 5000;

// bool IsLed_On = false;
// ulong led_start_red = 0;
uint32_t previoustime = 0;
int currentState = 0;

// put function declarations here:
int myFunction(int, int);

void setup() { // đoạn code khởi tạo
  // put your setup code here, to run once:
  Serial.begin(115200);

  // int result = myFunction(2, 3);
  // Serial.println("IOT welcome");
  pinMode(ledPinred, OUTPUT);
  pinMode(ledPingreen, OUTPUT);
  pinMode(ledPinyellow, OUTPUT);

  digitalWrite(ledPinred, HIGH);
  digitalWrite(ledPingreen, LOW);
  digitalWrite(ledPinyellow, LOW);
}

void Use_Blocking(){
  digitalWrite(ledPinred, HIGH); // Bật
  Serial.println("LED -> ON");
  delay(1000);                // Chờ 1 giây có cơ chế block
  digitalWrite(ledPinred, LOW);  // tắt
  Serial.println("LED -> OFF");
  delay(1000);
}

bool iSReady(uint32_t& ulTimer, uint32_t milisecond){
  uint32_t t = millis();
  if(t - ulTimer < milisecond) return false;
  ulTimer = t;
  return true;
}

void Use_Non_Blocking(){
  
  switch (currentState)
  {
  case 0:
    if(iSReady(previoustime, RED_TIME)){
      digitalWrite(ledPinred, LOW);
      digitalWrite(ledPinyellow, HIGH);
      currentState = 1;
      Serial.println("Đèn vàng sáng");
    }
    break;
    case 1: // Đèn vàng
    if (iSReady(previoustime, YELLOW_TIME)) {
      digitalWrite(ledPinyellow, LOW);
      digitalWrite(ledPingreen, HIGH);
      currentState = 2;
      Serial.println("Đèn xanh sáng");
    }
    break;
  
  case 2: // Đèn xanh
    if (iSReady(previoustime, GREEN_TIME)) {
      digitalWrite(ledPingreen, LOW);
      digitalWrite(ledPinred, HIGH);
      currentState = 0;
      Serial.println("Đèn đỏ sáng");
    }
    break;
  }
}

void loop() { 
  Use_Non_Blocking();

  ulong t = millis();
  Serial.print("Timer : ");
  Serial.println(t);
}
