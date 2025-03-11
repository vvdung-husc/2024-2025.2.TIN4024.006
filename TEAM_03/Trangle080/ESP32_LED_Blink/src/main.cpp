#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);


 
int ledPin = 5; // Chân GPIO 2 (thường nối sẵn LED trên board)

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH); // Bật LED
  Serial.println("LED -> ON");
  delay(1000);                // Đợi 1 giây
  digitalWrite(ledPin, LOW);  // Tắt LED
  Serial.println("LED -> OFF");
  delay(1000);                // Đợi 1 giây
}
// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}