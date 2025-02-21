#include <Arduino.h>
#include <TM1637Display.h>

// Pin - Đèn giao thông
#define rLED  5
#define yLED  17
#define gLED  16

// Pin - TM1637
#define CLK   15
#define DIO   2

// Pin - Cảm biến quang
#define ldrPIN  13

// Pin - Nút nhấn và đèn xanh dương
#define btn1  23
#define bLED  21  // Đèn xanh dương

// Thời gian chờ đèn
uint rTIME = 5000;  
uint yTIME = 3000;  
uint gTIME = 10000; 

ulong currentMiliseconds = 0; 
ulong ledTimeStart = 0;       
ulong nextTimeTotal = 0;      
int currentLED = 0;           
int tmCounter = rTIME / 1000; 
ulong counterTime = 0;        

int darkThreshold = 1000;    

TM1637Display display(CLK, DIO);  

bool displayOn = true;   // Trạng thái màn hình mặc định: BẬT
bool lastButtonState = HIGH; 

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light_TM1637();
bool isDark();
void YellowLED_Blink();
void CheckButtonPress();

void setup() {
  Serial.begin(115200);

  // Khởi tạo các chân đầu ra
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(ldrPIN, INPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(bLED, OUTPUT);

  tmCounter = (rTIME / 1000) - 1;
  display.setBrightness(7);
  
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  digitalWrite(bLED, HIGH); // Ban đầu đèn xanh dương tắt
  display.showNumberDec(tmCounter--, true, 2, 2);
  
  currentLED = rLED;
  nextTimeTotal += rTIME;    
  Serial.println("== START ==>");  
}

void loop() {  
  currentMiliseconds = millis();
  CheckButtonPress(); // Kiểm tra nút nhấn bật/tắt màn hình

  if (isDark()) YellowLED_Blink();
  else NonBlocking_Traffic_Light_TM1637();
}

// 🎯 **Hàm xử lý nút nhấn để bật/tắt màn hình và đèn xanh dương**
void CheckButtonPress() {
  bool buttonState = digitalRead(btn1);

  if (buttonState == LOW && lastButtonState == HIGH) { // Khi nút được nhấn
    displayOn = !displayOn; // Đảo trạng thái màn hình

    if (displayOn) {
      display.setBrightness(7);  // Bật màn hình
      display.showNumberDec(tmCounter, true, 2, 2);
      digitalWrite(bLED, HIGH);  // Đèn bật
    } else {
      display.clear();  // Tắt màn hình
      digitalWrite(bLED, LOW); // Đèn tắt
    }

    
  }
  lastButtonState = buttonState; // Cập nhật trạng thái nút
}

// 🎯 **Hàm kiểm tra thời gian đã trôi qua**
bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

// 🎯 **Hàm điều khiển đèn giao thông**
void NonBlocking_Traffic_Light_TM1637(){  
  bool bShowCounter = false;

  switch (currentLED) {
    case rLED:
      if (IsReady(ledTimeStart, rTIME)) {
        digitalWrite(rLED, LOW);
        digitalWrite(gLED, HIGH);
        currentLED = gLED;
        nextTimeTotal += gTIME;
        tmCounter = (gTIME / 1000) - 1 ; 
        bShowCounter = true;  
        counterTime = currentMiliseconds;        
      }
      break;
    case gLED:
      if (IsReady(ledTimeStart,gTIME)) {        
        digitalWrite(gLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = yLED;
        nextTimeTotal += yTIME;
        tmCounter = (yTIME / 1000) - 1; 
        bShowCounter = true;   
        counterTime = currentMiliseconds;    
      }
      break;
    case yLED:
      if (IsReady(ledTimeStart,yTIME)) {        
        digitalWrite(yLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        nextTimeTotal += rTIME;
        tmCounter = (rTIME / 1000) - 1; 
        bShowCounter = true;       
        counterTime = currentMiliseconds;        
      }
      break;
  }

  if (!bShowCounter) bShowCounter = IsReady(counterTime, 1000);

  if (bShowCounter && displayOn) { 
    display.showNumberDec(tmCounter--, true, 2, 2);
  }
}

// 🎯 **Hàm kiểm tra trời tối**
bool isDark(){
  static ulong darkTimeStart = 0;
  static uint16_t lastValue = 0;
  static bool bDark = false;

  if (!IsReady(darkTimeStart, 50)) return bDark;
  uint16_t value = analogRead(ldrPIN);  
  if (value == lastValue) return bDark;

  if (value < darkThreshold){
    if (!bDark){
      digitalWrite(currentLED, LOW);
    }   
    bDark = true;   
  }
  else {
    if (bDark){
      digitalWrite(currentLED, LOW);
    }
    bDark = false;
  }
  
  lastValue = value;  
  return bDark;
}

// 🎯 **Hàm điều khiển đèn vàng nhấp nháy**
void YellowLED_Blink(){
  static ulong yLedStart = 0;
  static bool isON = false;

  if (!IsReady(yLedStart,1000)) return;
  if (!isON) digitalWrite(yLED, HIGH);
  else digitalWrite(yLED, LOW);
  isON = !isON;
}
