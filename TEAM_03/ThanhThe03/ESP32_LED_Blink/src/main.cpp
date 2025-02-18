#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

#define RED 2
#define YELLOW 3
#define GREEN 4

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
}

void loop() {
  digitalWrite(GREEN, HIGH);  // Đèn xanh bật
  delay(5000);                // Giữ trong 5 giây
  digitalWrite(GREEN, LOW);   // Tắt đèn xanh

  digitalWrite(YELLOW, HIGH); // Đèn vàng bật
  delay(2000);                // Giữ trong 2 giây
  digitalWrite(YELLOW, LOW);  // Tắt đèn vàng

  digitalWrite(RED, HIGH);    // Đèn đỏ bật
  delay(5000);                // Giữ trong 5 giây
  digitalWrite(RED, LOW);     // Tắt đèn đỏ
}


// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}