#include <Arduino.h>
#include <TM1637Display.h>
#include <dht.h>

//NGO NGUYEN DUC QUY
#define BLYNK_TEMPLATE_ID "TMPL69AI-gkDM"
#define BLYNK_TEMPLATE_NAME "ESP32 LED TM1637 2"
#define BLYNK_AUTH_TOKEN "b8rpMHpj9nG_SeeXqgX9q0jx0sg_B_10"
//LE HUU NHAT
#define BLYNK_TEMPLATE_ID "TMPL6VOd5sqsE"
#define BLYNK_TEMPLATE_NAME "ESP32 LED TM1637"
#define BLYNK_AUTH_TOKEN "Z24-FlFOg2_-WIKTamwxA2uDvk7UWmz6"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu cho việc chạy mô phỏng
char ssid[] = "Wokwi-GUEST";  //Tên mạng WiFi
char pass[] = "";             //Mật khẩu mạng WiFi
// Pin - Các đèn LEDLED
#define rLED 27
#define yLED 26 
#define gLED 25

// Pin - TM1637TM1637
#define CLK 18  //Chân kết nối CLK của TM1637
#define DIO 19  //Chân kết nối DIO của TM1637

#define DHTTYPE DHT22 
#define DHT22_PIN 16
DHT dht(DHT22_PIN, DHTTYPE);

#define btnBLED  23 //Chân kết nối nút bấm
#define pinBLED  21 //Chân kết nối đèn xxanh

//Pin - Cảm biến quang điện trở
#define ldrPIN  33

//1000 ms = 1 seconds
uint rTIME = 5000;  //thời gian chờ đèn đỏ    5 giây
uint yTIME = 3000;  //thời gian chờ đèn vàng  3 giây
uint gTIME = 10000; //thời gian chờ đèn xanh 10 giây

ulong currentMiliseconds = 0; // miliseconds hiện tại
ulong ledTimeStart = 0;       // thời gian bắt đầu chạy LED
ulong nextTimeTotal = 0;      // biến tổng - chỉ để hiển thị
int currentLED = 0;           // đèn LED hiện tại đang sáng
int tmCounter = rTIME / 1000; // biến đếm ngược trên bảng
ulong counterTime = 0;        // thời gian bắt đầu đếm ngược
bool blueButtonON = true;
int value =0;
bool useBlynkValue = false;
bool tm1637ON = true;

int darkThreshold = 500; // ngưỡng ánh sáng < 1000 => DARK

TM1637Display display(CLK, DIO); // biến kiểu TM1637Display điều khiển bảng đếm ngượcngược

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light_TM1637();
bool isDark();
void YellowLED_Blink();
void DoamBlynk();
void NhietdoBlynk();
void updateBlueButton();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);

  pinMode(ldrPIN, INPUT);
  pinMode(pinBLED, OUTPUT);

  tmCounter = (rTIME / 1000) - 1;
  display.setBrightness(7);

  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  display.showNumberDec(tmCounter--, true, 2, 2);

  currentLED = rLED;
  nextTimeTotal += rTIME;
  Serial.println("== START ==>");
  Serial.print("1. RED    => GREEN  ");
  Serial.print((nextTimeTotal / 1000) % 60);
  Serial.println(" (ms)");

    // Start the WiFi connection
    Serial.print("Connecting to ");Serial.println(ssid);
    Blynk.begin(BLYNK_AUTH_TOKEN,ssid, pass); //Kết nối đến mạng WiFi

    Serial.println();
    Serial.println("WiFi connected");

    
    digitalWrite(pinBLED, blueButtonON? HIGH : LOW);  
    Blynk.virtualWrite(V1, blueButtonON); //Đồng bộ trạng thái trạng thái của đèn với Blynk
    
    Serial.println("== START ==>");
}

void loop()
{
    Blynk.run();
  // put your main code here, to run repeatedly:
  currentMiliseconds = millis();
  DoamBlynk();
  NhietdoBlynk();
  updateBlueButton();
  if (isDark())
    YellowLED_Blink(); // Nếu trời tối => Nhấp nháy đèn vàng
  else
    NonBlocking_Traffic_Light_TM1637(); // Hiển thị đèn giao thôngthông
}

bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond)
    return false;
  ulTimer = currentMiliseconds;
  return true;
}

void DoamBlynk(){
    float h = dht.readHumidity();
    Blynk.virtualWrite(V3, h);  
  }
  
void NhietdoBlynk(){
    float t = dht.readTemperature();
    Blynk.virtualWrite(V2, t); 
    
  }

  void updateBlueButton() {
    static ulong lastTime = 0;
    static int lastValue = HIGH;
    static ulong debounceTime = 0;

    if (!IsReady(lastTime, 200)) return;
    int v = digitalRead(btnBLED);
    if (v == lastValue) return; 
    lastValue = v;
    if (v == LOW) return;

    // Chống nhấn đúp liên tục
    if (!IsReady(debounceTime, 150)) return;

    // Đảo trạng thái nút
    blueButtonON = !blueButtonON;
    Serial.print("Trạng thái LED: ");
    Serial.println(blueButtonON ? "ON" : "OFF");

    // Bật/tắt đèn LED
    digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);
    Blynk.virtualWrite(V1, blueButtonON);

    // Điều khiển TM1637
    tm1637ON = blueButtonON;
    if (!tm1637ON) display.clear(); 
}


//được gọi mỗi khi có dữ liệu mới được gửi từ ứng dụng Blynk đến thiết bị.
BLYNK_WRITE(V1) {
    bool newState = param.asInt();  // Lấy trạng thái từ Blynk
    if (newState != blueButtonON) { // Chỉ cập nhật nếu có thay đổi
        blueButtonON = newState;
        Serial.println(blueButtonON ? "Blynk -> Blue Light ON" : "Blynk -> Blue Light OFF");

        // Cập nhật trạng thái LED thực tế
        digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);

        // Đồng bộ với TM1637
        tm1637ON = blueButtonON;
        if (!tm1637ON) display.clear();
    }
}


void NonBlocking_Traffic_Light_TM1637()
{
  if (!tm1637ON) return;

  bool bShowCounter = false;
  switch (currentLED)
  {
  case rLED: // Đèn đỏ: 5 giây
    if (IsReady(ledTimeStart, rTIME))
    {
      digitalWrite(rLED, LOW);
      digitalWrite(gLED, HIGH);
      currentLED = gLED;
      nextTimeTotal += gTIME;
      tmCounter = (gTIME / 1000) - 1;
      bShowCounter = true;
      counterTime = currentMiliseconds;
      Serial.print("2. GREEN  => YELLOW ");
      Serial.print((nextTimeTotal / 1000) % 60);
      Serial.println(" (ms)");
    }
    break;
  case gLED: // Đèn xanh: 7 giây
    if (IsReady(ledTimeStart, gTIME))
    {
      digitalWrite(gLED, LOW);
      digitalWrite(yLED, HIGH);
      currentLED = yLED;
      nextTimeTotal += yTIME;
      tmCounter = (yTIME / 1000) - 1;
      bShowCounter = true;
      counterTime = currentMiliseconds;
      Serial.print("3. YELLOW => RED    ");
      Serial.print((nextTimeTotal / 1000) % 60);
      Serial.println(" (ms)");
    }
    break;

  case yLED: // Đèn vàng: 2 giây
    if (IsReady(ledTimeStart, yTIME))
    {
      digitalWrite(yLED, LOW);
      digitalWrite(rLED, HIGH);
      currentLED = rLED;
      nextTimeTotal += rTIME;
      tmCounter = (rTIME / 1000) - 1;
      bShowCounter = true;
      counterTime = currentMiliseconds;
      Serial.print("1. RED    => GREEN  ");
      Serial.print((nextTimeTotal / 1000) % 60);
      Serial.println(" (ms)");
    }
    break;
  }
  if (!bShowCounter)
    bShowCounter = IsReady(counterTime, 1000);
  if (bShowCounter)
  {
    Blynk.virtualWrite(V0, tmCounter);
    display.showNumberDec(tmCounter--, true, 2, 2);
  }
}

bool isDark()
{
  static ulong darkTimeStart = 0; // lưu thời gian của việc đọc cảm biến
  static uint16_t lastValue = 0;  // lưu giá trị gần nhất của cảm biến
  static bool bDark = false;      // true: value > darkThreshold

  if (!IsReady(darkTimeStart, 50))
    return bDark;                      // 50ms đọc cảm biến 1 lầnlần
    
    if (!useBlynkValue) // Nếu không dùng giá trị từ Blynk, đọc cảm biến
    value = analogRead(ldrPIN); // đọc cảm biến theo chế đố tương tựtự
    
    
    Blynk.virtualWrite(V4, value);

  if (value == lastValue)
    return bDark; // vẫn bằng giá trị củcủ

  if (value < darkThreshold)
  {
    if (!bDark)
    {
      digitalWrite(currentLED, LOW);
      Serial.print("DARK  value: ");
      Serial.println(value);
    }
    bDark = true;
  }
  else
  {
    if (bDark)
    {
      digitalWrite(currentLED, LOW);
      Serial.print("LIGHT value: ");
      Serial.println(value);
    }
    bDark = false;
  }

  lastValue = value;
  return bDark;
}

void YellowLED_Blink()
{
  static ulong yLedStart = 0;
  static bool isON = false;

  if (!IsReady(yLedStart, 1000))
    return;
  if (!isON)
    digitalWrite(yLED, HIGH);
  else
    digitalWrite(yLED, LOW);
  isON = !isON;
}

//được gọi mỗi khi có dữ liệu mới được gửi từ ứng dụng Blynk đến thiết bị.
BLYNK_WRITE(V4) { //virtual_pin định nghĩa trong ứng dụng Blynk
    // Xử lý dữ liệu nhận được từ ứng dụng Blynk
    int dark = param.asInt();
    if (dark > 0) // Nếu giá trị từ Blynk hợp lệ
    {
        value = dark;
        useBlynkValue = true; // Chuyển sang sử dụng giá trị từ Blynk
        Serial.print("New dark value from Blynk: ");
        Serial.println(value);
    }
    else
    {
        useBlynkValue = false; // Quay lại dùng cảm biến
    }
}