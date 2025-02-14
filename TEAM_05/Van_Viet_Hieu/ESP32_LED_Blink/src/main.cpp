#include <Arduino.h>

#define RED_LED 5
#define YELLOW_LED 17
#define GREEN_LED 16

ulong prevMillis = 0;
int state = 0;
const uint32_t durations[] = {5000, 2000, 5000}; // Thời gian từng trạng thái

void setup() {
  Serial.begin(115200);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(RED_LED, HIGH); // Bắt đầu từ đèn đỏ
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
}

void TrafficLight_Non_Blocking() {
  ulong currentMillis = millis();
  
  if (currentMillis - prevMillis >= durations[state]) {
    prevMillis = currentMillis; // Cập nhật thời gian chuyển trạng thái

    switch (state) {
      case 0: // Chuyển từ ĐỎ sang VÀNG
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, HIGH);
        state = 1;
        break;
      case 1: // Chuyển từ VÀNG sang XANH
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        state = 2;
        break;
      case 2: // Chuyển từ XANH sang ĐỎ
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        state = 0;
        break;
    }
  }
}

void loop() {
  TrafficLight_Non_Blocking();
  Serial.print("Current State: ");
  Serial.println(state);
}
