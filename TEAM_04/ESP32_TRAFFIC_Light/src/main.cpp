#include <Arduino.h>
#include <TM1637Display.h>

#define rLED  5     // Chân đèn đỏ
#define yLED  17    // Chân đèn vàng//
#define gLED  16    // Chân đèn xanh 
#define CLK   15    // Chân CLK của 4-Digit Display
#define DIO   2     // Chân DIO của 4-Digit Display
#define ldrPIN  13  // Chân cảm biến ánh sáng

uint gTIME = 6000;  //thời gian chờ đèn xanh  5s
uint yTIME = 3000;  //thời gian chờ đèn vàng  2s
uint rTIME = 4000;  //thời gian chờ đèn đỏ    3s

ulong currentMiliseconds = 0; //Miliseconds 
ulong ledTimeStart = 0;       //thời gian bắt đầu chạy LED
ulong counterTime = 0;        //thời gian bắt đầu đếm ngược
ulong nextTimeTotal = 0;      //biến tổng - chỉ để hiển thị

int currentLED = 0;           //đèn LED hiện tại đang sáng
int tmCounter = rTIME / 1000; //biến đếm ngược trên bảng
int darkThreshold = 500;     //ngưỡng ánh sáng < 1000 => DARK

TM1637Display display(CLK, DIO);  //biến kiểu TM1637Display điều khiển bảng đếm ngượcngược

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light_TM1637();
void YellowLED_Blink();
bool isDark();

void setup() {
  
  Serial.begin(115200);
  pinMode(gLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(rLED, OUTPUT);

  pinMode(ldrPIN,INPUT);

  tmCounter = (gTIME / 1000) - 1;
  display.setBrightness(7);
  
  digitalWrite(gLED, HIGH);
  digitalWrite(yLED, LOW);
  digitalWrite(rLED, LOW);
  display.showNumberDec(tmCounter--, true, 2, 2);
  
  currentLED = gLED;
  nextTimeTotal += gTIME;    
  Serial.println("== START ==>");  
  Serial.print("1. GREEN    => YELLOW  "); Serial.print((nextTimeTotal/1000)%60);Serial.println(" (ms)"); 
}


bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

void NonBlocking_Traffic_Light_TM1637(){  
  bool bShowCounter = false;
  switch (currentLED) {
    case gLED: // Đèn xanh: 5s
      if (IsReady(ledTimeStart, gTIME)) {
        digitalWrite(gLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = yLED;
        nextTimeTotal += yTIME;
        tmCounter = (yTIME / 1000) - 1 ; 
        bShowCounter = true;  
        counterTime = currentMiliseconds;        
        Serial.print("2. YELLOW  => RED "); Serial.print((nextTimeTotal/1000)%60);Serial.println(" (ms)");       
      }
      break;
    case yLED: // Đèn vàng: 2s
      if (IsReady(ledTimeStart,yTIME)) {        
        digitalWrite(yLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        nextTimeTotal += rTIME;
        tmCounter = (rTIME / 1000) - 1; 
        bShowCounter = true;   
        counterTime = currentMiliseconds;    
        Serial.print("3. RED => GREEN   "); Serial.print((nextTimeTotal/1000)%60);Serial.println(" (ms)");      
      }
      break;

    case rLED: // Đèn đỏ: 3 giây
      if (IsReady(ledTimeStart,rTIME)) {        
        digitalWrite(rLED, LOW);
        digitalWrite(gLED, HIGH);
        currentLED = gLED;
        nextTimeTotal += gTIME;
        tmCounter = (gTIME / 1000) - 1; 
        bShowCounter = true;       
        counterTime = currentMiliseconds;        
        Serial.print("1. GREEN    => YELLOW  "); Serial.print((nextTimeTotal/1000)%60);Serial.println(" (ms)");       
      }
      break;
  }
  if (!bShowCounter) bShowCounter = IsReady(counterTime, 1000);
  if (bShowCounter) {
    display.showNumberDec(tmCounter--, true, 2, 2);
  }
}

bool isDark(){
  static ulong darkTimeStart = 0; //lưu thời gian của việc đọc cảm biến
  static uint16_t lastValue = 0;  //lưu giá trị gần nhất của cảm biến
  static bool bDark = false;      //true: value > darkThreshold

  if (!IsReady(darkTimeStart, 50)) return bDark;//50ms đọc cảm biến 1 lần
  uint16_t value = analogRead(ldrPIN);          //đọc cảm biến theo chế đố tương tự
  if (value == lastValue) return bDark;         //vẫn bằng giá trị củ

  if (value < darkThreshold){
    if (!bDark){
      digitalWrite(currentLED, LOW);
      Serial.print("DARK  value: ");Serial.println(value);
    }   
    bDark = true;   
  }
  else {
    if (bDark){
      digitalWrite(currentLED, LOW);
      Serial.print("LIGHT value: ");Serial.println(value);
    }
    bDark = false;
  }
  
  lastValue = value;  
  return bDark;
}

void YellowLED_Blink(){
  static ulong yLedStart = 0;
  static bool isON = false;

  if (!IsReady(yLedStart,1000)) return;
  if (!isON) digitalWrite(yLED, HIGH);
  else digitalWrite(yLED, LOW);
  isON = !isON;

}

void loop() {  
  // put your main code here, to run repeatedly:
  currentMiliseconds = millis();
  if (isDark()) YellowLED_Blink();        //Nếu trời tối => Nhấp nháy đèn vàng
  else NonBlocking_Traffic_Light_TM1637();//Hiển thị đèn giao thôngthông
  
}
