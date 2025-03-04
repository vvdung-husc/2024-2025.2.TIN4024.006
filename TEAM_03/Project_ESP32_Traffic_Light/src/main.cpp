
#include <Arduino.h>
#include <TM1637Display.h>

//Pin - Các đèn LEDLED
#define rLED  27
#define yLED  26
#define gLED  25

#define bLED  21
const int buttonPin = 23;
int oldValue = HIGH;
bool screenOn = true;

//Pin - TM1637TM1637
#define CLK   18
#define DIO   19

//Pin - Cảm biến quang điện trởtrở
#define ldrPIN  13

//1000 ms = 1 seconds
uint rTIME = 5000;  //thời gian chờ đèn đỏ    5 giây
uint yTIME = 3000;  //thời gian chờ đèn vàng  3 giây
uint gTIME = 10000; //thời gian chờ đèn xanh 10 giây

ulong currentMiliseconds = 0; //miliseconds hiện tại
ulong ledTimeStart = 0;       //thời gian bắt đầu chạy LED
ulong nextTimeTotal = 0;      //biến tổng - chỉ để hiển thị
int currentLED = 0;           //đèn LED hiện tại đang sáng
int tmCounter = rTIME / 1000; //biến đếm ngược trên bảng
ulong counterTime = 0;        //thời gian bắt đầu đếm ngược

int darkThreshold = 1000;     //ngưỡng ánh sáng < 1000 => DARK

TM1637Display display(CLK, DIO);  //biến kiểu TM1637Display điều khiển bảng đếm ngượcngược

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light_TM1637();
bool isDark();
void YellowLED_Blink();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(bLED, OUTPUT);

  pinMode(ldrPIN,INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  tmCounter = (rTIME / 1000) - 1;
  display.setBrightness(7);
  
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  digitalWrite(bLED, HIGH);
  display.showNumberDec(tmCounter--, true, 2, 2);
  
  currentLED = rLED;
  nextTimeTotal += rTIME;    
  Serial.println("== START ==>");  
  Serial.print("1. RED    => GREEN  "); Serial.print((nextTimeTotal/1000)%60);Serial.println(" (ms)"); 
}
void btnPress() {
  int newValue = digitalRead(buttonPin);
  if (newValue != oldValue) {
    if (newValue == LOW) { 
      screenOn = !screenOn;

      if (screenOn) {
        display.setBrightness(7);
        display.showNumberDec(tmCounter, true, 2, 2); 
        digitalWrite(bLED, HIGH); 
      } else {
        display.clear(); 
        digitalWrite(bLED, LOW); 
      }
    }
    oldValue = newValue; 
  }
}
void loop() {  
  currentMiliseconds = millis();
  btnPress(); 
  if (isDark()) YellowLED_Blink();        //Nếu trời tối => Nhấp nháy đèn vàng
  else NonBlocking_Traffic_Light_TM1637();//Hiển thị đèn giao thôngthông
  
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
    case rLED: // Đèn đỏ: 5 giây
      if (IsReady(ledTimeStart, rTIME)) {
        digitalWrite(rLED, LOW);
        digitalWrite(gLED, HIGH);
        currentLED = gLED;
        nextTimeTotal += gTIME;
        tmCounter = (gTIME / 1000) - 1 ; 
        bShowCounter = true;  
        counterTime = currentMiliseconds;        
        Serial.print("2. GREEN  => YELLOW "); Serial.print((nextTimeTotal/1000)%60);Serial.println(" (ms)");       
      }
      break;
    case gLED: // Đèn xanh: 7 giây
      if (IsReady(ledTimeStart,gTIME)) {        
        digitalWrite(gLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = yLED;
        nextTimeTotal += yTIME;
        tmCounter = (yTIME / 1000) - 1; 
        bShowCounter = true;   
        counterTime = currentMiliseconds;    
        Serial.print("3. YELLOW => RED    "); Serial.print((nextTimeTotal/1000)%60);Serial.println(" (ms)");      
      }
      break;

    case yLED: // Đèn vàng: 2 giây
      if (IsReady(ledTimeStart,yTIME)) {        
        digitalWrite(yLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        nextTimeTotal += rTIME;
        tmCounter = (rTIME / 1000) - 1; 
        bShowCounter = true;       
        counterTime = currentMiliseconds;        
        Serial.print("1. RED    => GREEN  "); Serial.print((nextTimeTotal/1000)%60);Serial.println(" (ms)");       
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

  if (!IsReady(darkTimeStart, 50)) return bDark;//50ms đọc cảm biến 1 lầnlần
  uint16_t value = analogRead(ldrPIN);          //đọc cảm biến theo chế đố tương tựtự
  if (value == lastValue) return bDark;         //vẫn bằng giá trị củcủ

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
