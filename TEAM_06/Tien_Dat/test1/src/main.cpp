#include <Arduino.h>
#include <TM1637Display.h>


// định nghĩa các chân điều khiển đèn led 
#define LED_RED_PIN 22 
#define LED_GREEN_PIN 23
#define LED_YELLOW_PIN 18

// định nghĩa chân CLK và chân DIO của module tm1637

#define CLK   12   // Chân mới cho TM1637 CLK
#define DIO   14   // Chân mới cho TM1637 DIO


// thiết lập thời gian cho mỗi loại đèn (mili giây)

#define R_TIME 9000
#define G_TIME 7000
#define Y_TIME 3000

// khai báo các biến quản lý trạng thái

ulong ledTimeStart = 0; // lưu thời điểm bắt đầu sáng của đèn hiện tại

int currentLED = LED_RED_PIN; // biến lưu đèn hiện tại đang sáng( bắt đầu từ đèn đỏ)

int countdown = R_TIME / 1000; // biến đếm ngược thời gian 

// khai bao đối tượng để điều khiển module hiển thị cụ thể là đối tượng display để giao tiếp với module tm1637

TM1637Display display(CLK,DIO);

void updateDisplay(int x){
  display.showNumberDec(x, true, 2, 2);
}


void setup(){
  Serial.begin(115200); // Mở cổng serial

  // thiết lập các đèn led là output
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  display.setBrightness(7);
  // điều khiển trạng thái ban đầu của các đèn led 

  digitalWrite(LED_RED_PIN, HIGH);
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_YELLOW_PIN,LOW);

  currentLED = LED_RED_PIN;
  ledTimeStart = millis(); // ghi lại thời điểm bắt đầu sáng của đèn 

  countdown = R_TIME / 1000;

  updateDisplay(countdown); // hiển thị thời gian đếm ngược

}

void loop(){
  ulong currentmillis = millis(); // lấy thời gian hiện tại của esp32

  if(currentmillis - ledTimeStart >= 1000){
    ledTimeStart = currentmillis;
    countdown--;
    updateDisplay(countdown);

    if(countdown == 0){
      switch(currentLED) {
        case LED_RED_PIN:
          digitalWrite(LED_RED_PIN,LOW);
          digitalWrite(LED_GREEN_PIN,HIGH);
          currentLED = LED_GREEN_PIN;
          countdown = G_TIME / 1000;
          break;
        case LED_GREEN_PIN:
          digitalWrite(LED_GREEN_PIN,LOW);
          digitalWrite(LED_YELLOW_PIN,HIGH);
          currentLED = LED_YELLOW_PIN;
          countdown = Y_TIME / 1000;
          break;
        case LED_YELLOW_PIN:
        digitalWrite(LED_YELLOW_PIN, LOW);
        digitalWrite(LED_RED_PIN,HIGH);
        currentLED = LED_RED_PIN;
        countdown = R_TIME / 1000;
        break;
      }
      updateDisplay(countdown);
    }
  }
}


