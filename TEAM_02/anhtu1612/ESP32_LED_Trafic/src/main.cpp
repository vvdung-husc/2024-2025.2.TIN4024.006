#include <Arduino.h>
#include <TM1637Display.h>
int redPin = 5;
int yellowPin = 17;
int greenPin = 16;

const int CLK = 23;
const int DIO = 22;

TM1637Display display(CLK, DIO);

unsigned long previousMillis = 0;
long interval = 1000;

int countdown = 0;
int state = 0;
void setup(){
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  Serial.begin(9600);
  display.setBrightness(7);
  
  digitalWrite(greenPin, HIGH);
  Serial.println("Đèn xanh (5 giây)");
  countdown = 5;
  state = 0;
}
void changeLight(){
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin, LOW);

  if(state == 0){
    digitalWrite(yellowPin, HIGH);
    Serial.println("Đèn vàng (2 giây)");
    countdown = 2;
    state = 1;
  } else if(state == 1) {
    digitalWrite(redPin, HIGH);
    Serial.println("Đèn đỏ (5 giây)");
    countdown = 2;
    state = 2;
  }else{
    digitalWrite(greenPin, HIGH);
    Serial.println("Đèn xanh (5 giây)");
    countdown = 5;
    state = 0;
  }
}
void loop(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;

    if(countdown < 0){
      changeLight();
    }

    Serial.print("Còn ");
    Serial.print(countdown);
    Serial.println(" giây");

    display.showNumberDec(countdown, true);
    countdown--;

  }
}