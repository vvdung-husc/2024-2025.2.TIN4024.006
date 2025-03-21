#include <Arduino.h>

int gPin = 15;  // Đèn Xanh
int yPin = 4;   // Đèn Vàng
int rPin = 2;   // Đèn Đỏ

void setup() {
  Serial.begin(9600);
  
  pinMode(gPin, OUTPUT);
  pinMode(yPin, OUTPUT);
  pinMode(rPin, OUTPUT);
}

void loop() {
  // Bật đèn Xanh, tắt đèn khác
  digitalWrite(gPin, HIGH);
  digitalWrite(yPin, LOW);
  digitalWrite(rPin, LOW);
  Serial.println("LED ON: Green");
  delay(1000);

  // Bật đèn Vàng, tắt đèn khác
  digitalWrite(gPin, LOW);
  digitalWrite(yPin, HIGH);
  digitalWrite(rPin, LOW);
  Serial.println("LED ON: Yellow");
  delay(1000);

  // Bật đèn Đỏ, tắt đèn khác
  digitalWrite(gPin, LOW);
  digitalWrite(yPin, LOW);
  digitalWrite(rPin, HIGH);
  Serial.println("LED ON: Red");
  delay(1000);
}
// #include <Arduino.h>

// int gPin = 15;
// int yPin = 4;
// int rPin = 2;

// int currenPin = 0;
// int listPIN[] = {yPin,gPin, rPin };
// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);

//   pinMode(gPin,OUTPUT);
//   pinMode(yPin,OUTPUT);
//   pinMode(rPin,OUTPUT);

//   Serial.println("ESP8266 đã sẵn sàng!");
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//  digitalWrite(listPIN[currenPin], HIGH);
// /*  digitalWrite(gPin, HIGH);
//  digitalWrite(yPin, HIGH);
//  digitalWrite(rPin, HIGH); */
//  Serial.println("LED ON");
//  delay(2000); // thời gian bật
//  digitalWrite(listPIN[currenPin], LOW);
//  Serial.println("LED OFF");
//  delay(100);  // thời gian tắt
// if(++currenPin > 4) currenPin = 0;
// }