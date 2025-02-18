#include <Arduino.h>

#define GREEN_LED 16  // Chân kết nối đèn xanh
#define YELLOW_LED 17 // Chân kết nối đèn vàng
#define RED_LED 5     // Chân kết nối đèn đỏ

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  Serial.begin(115200); // Khởi động Serial Monitor
}

void loop() {
  // Đèn xanh sáng trong 10 giây
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  for (int i = 10; i > 0; i--) {
    Serial.print("Đèn xanh: ");
    Serial.print(i);
    Serial.println(" giây");
    delay(1000);
  }

  // Đèn vàng sáng trong 3 giây
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  for (int i = 3; i > 0; i--) {
    Serial.print("Đèn vàng: ");
    Serial.print(i);
    Serial.println(" giây");
    delay(1000);
  }

  // Đèn đỏ sáng trong 7 giây
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  for (int i = 7; i > 0; i--) {
    Serial.print("Đèn đỏ: ");
    Serial.print(i);
    Serial.println(" giây");
    delay(1000);
  }
}
