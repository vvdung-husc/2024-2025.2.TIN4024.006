#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>

// Trần Tiến Lợi
#define BLYNK_TEMPLATE_ID "TMPL63UdyxQPo"
#define BLYNK_TEMPLATE_NAME "TrafficBlynk"
#define BLYNK_AUTH_TOKEN "QJeHnTbQcSkdcOcbEnXoniJxXSHvmHOn"
//

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";  
char pass[] = "";             

//Pin - Các đèn LEDLED
#define rLED  27
#define yLED  26
#define gLED  25
#define btnBLED  23 //Chân kết nối nút bấm
#define pinBLED  21 //Chân kết nối đèn xanh
//Pin - TM1637TM1637
#define CLK   15
#define DIO   2

#define DHTPIN 16         // Chân DATA của cảm biến DHT22
#define DHTTYPE DHT22    // Loại cảm biến

//Pin - Cảm biến quang điện trởtrở
#define ldrPIN  32

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
bool blueButtonON = true; 

int darkThreshold = 1000;     //ngưỡng ánh sáng < 1000 => DARK

DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);  //biến kiểu TM1637Display điều khiển bảng đếm ngượcngược

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light_TM1637();
bool isDark();
void YellowLED_Blink();


void sendSensor() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Kiểm tra nếu dữ liệu không hợp lệ
  if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Lỗi: Không đọc được dữ liệu từ cảm biến DHT22!");
      return; 
  }

  // Hiển thị lên Serial Monitor
  Serial.print("Nhiệt độ: ");
  Serial.print(temperature);
  Serial.print(" °C  |  Độ ẩm: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V2, temperature);  // Nhiệt độ gửi lên V2
  Blynk.virtualWrite(V3, humidity);     // Độ ẩm gửi lên V3
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(pinBLED, OUTPUT);
  pinMode(btnBLED, INPUT_PULLUP);

  pinMode(ldrPIN,INPUT);
  dht.begin(); // Khởi động DHT22
  tmCounter = (rTIME / 1000) - 1;
  display.setBrightness(7);
  
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  display.showNumberDec(tmCounter--, true, 2, 2);
  
  currentLED = rLED;
  nextTimeTotal += rTIME;    
  Serial.print("Connecting to ");Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid, pass); //Kết nối đến mạng WiFi

  Serial.println();
  Serial.println("WiFi connected");

  
  digitalWrite(pinBLED, blueButtonON? HIGH : LOW);  
  Blynk.virtualWrite(V1, blueButtonON); //Đồng bộ trạng thái trạng thái của đèn với Blynk
  Serial.println("== START ==>");  
  Serial.print("1. RED    => GREEN  "); Serial.print((nextTimeTotal/1000)%60);Serial.println(" (ms)"); 
}
void uptimeBlynk(){
  static ulong lastTime = 0;
  if (!IsReady(lastTime, 1000)) return; //Kiểm tra và cập nhật lastTime sau mỗi 1 giây
  ulong value = lastTime / 1000;
  Blynk.virtualWrite(V0, value);  //Gửi giá trị lên chân ảo V0 trên ứng dụng Blynk.
  if (blueButtonON){
    display.showNumberDec(value);
  }
}
void updateBlueButton(){
  static ulong lastTime = 0;
  static int lastValue = HIGH;
  if (!IsReady(lastTime, 50)) return; //Hạn chế bấm nút quá nhanh - 50ms mỗi lần bấm
  int v = digitalRead(btnBLED);
  if (v == lastValue) return;
  lastValue = v;
  if (v == LOW) return;

  if (!blueButtonON){
    Serial.println("Blue Light ON");
    digitalWrite(pinBLED, HIGH);
    blueButtonON = true;
    Blynk.virtualWrite(V1, blueButtonON);//Gửi giá trị lên chân ảo V1 trên ứng dụng Blynk.
  }
  else {
    Serial.println("Blue Light OFF");
    digitalWrite(pinBLED, LOW);    
    blueButtonON = false;
    Blynk.virtualWrite(V1, blueButtonON);//Gửi giá trị lên chân ảo V1 trên ứng dụng Blynk.
    display.clear();
  }    
}
void loop() {  
  
  Blynk.run(); 
  currentMiliseconds = millis();
  
  uint16_t lightValue = analogRead(ldrPIN);        
  Blynk.virtualWrite(V4, lightValue);

  if (isDark()) {  
    // Nếu trời tối, tắt tất cả đèn khác và bật đèn vàng nhấp nháy
    digitalWrite(rLED, LOW);
    digitalWrite(gLED, LOW);
    YellowLED_Blink();
  } 
  else {  
    // Nếu trời sáng, chạy hệ thống đèn giao thông bình thường
    NonBlocking_Traffic_Light_TM1637();
  }
  
  currentMiliseconds = millis();
  uptimeBlynk();
  updateBlueButton();
  
  // Đọc dữ liệu cảm biến DHT22 mỗi 2 giây
  static ulong lastDHTRead = 0;
  if (currentMiliseconds - lastDHTRead > 2000) { 
    lastDHTRead = currentMiliseconds;
    sendSensor();
  }
  
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

bool isDark() {
  static ulong darkTimeStart = 0;
  static uint16_t lastValue = 0;
  static bool bDark = false;
  
  uint16_t value = analogRead(ldrPIN);  
  if (!IsReady(darkTimeStart, 50)) return bDark;
  if (value == lastValue) return bDark;

  if (value < darkThreshold) {
    if (!bDark) {
      Serial.print("DARK mode activated, value: "); Serial.println(value);
      digitalWrite(yLED, HIGH);  // Chuyển sang đèn vàng
    }
    bDark = true;   
  } 
  else {
    if (bDark) {
      Serial.print("LIGHT mode activated, value: "); Serial.println(value);
      digitalWrite(yLED, LOW);
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
BLYNK_WRITE(V1) { //virtual_pin định nghĩa trong ứng dụng Blynk
  // Xử lý dữ liệu nhận được từ ứng dụng Blynk
  blueButtonON = param.asInt();  // Lấy giá trị từ ứng dụng Blynk
  if (blueButtonON){
    Serial.println("Blynk -> Blue Light ON");
    digitalWrite(pinBLED, HIGH);
    
  }
  else {
    Serial.println("Blynk -> Blue Light OFF");
    digitalWrite(pinBLED, LOW);   
    display.clear(); 
  }
}
