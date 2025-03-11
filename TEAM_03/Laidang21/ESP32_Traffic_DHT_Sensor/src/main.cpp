#include <Arduino.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>

#include <Arduino.h>
#include <TM1637Display.h>
#include <DHTesp.h>  // Thay thế thư viện Adafruit bằng DHTesp

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6i1PVifIH"
#define BLYNK_TEMPLATE_NAME "Sensor"
#define BLYNK_AUTH_TOKEN "vDlShrkiSaIRhQymoL5H9jFwTHDZkfov"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu
char ssid[] = "Wokwi-GUEST";  
char pass[] = "";             

#define DHTPIN 16         // Chân DATA của DHT22
#define btnBLED  23       // Chân nút bấm
#define pinBLED  21       // Chân đèn LED xanh

#define CLK 18  // Chân kết nối CLK của TM1637
#define DIO 19  // Chân kết nối DIO của TM1637

// Biến toàn cục
ulong currentMiliseconds = 0; 
bool blueButtonON = true;  

DHTesp dht;  // Dùng thư viện DHTesp
TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void updateBlueButton();
void uptimeBlynk();

int errorCount = 0;  // Biến đếm số lần lỗi đọc cảm biến

void sendSensor() {
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  // Kiểm tra nếu dữ liệu không hợp lệ
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Lỗi: Không đọc được dữ liệu từ cảm biến DHT22!");
    Serial.print("Chi tiết lỗi: ");
    Serial.println(dht.getStatusString()); // In lỗi chi tiết
    errorCount++;
    
    if (errorCount >= 5) {  // Nếu lỗi liên tục 5 lần, reset cảm biến
      Serial.println("Reset cảm biến DHT22...");
      pinMode(DHTPIN, OUTPUT);
      digitalWrite(DHTPIN, LOW);
      delay(1000);
      pinMode(DHTPIN, INPUT);
      dht.setup(DHTPIN, DHTesp::DHT22);
      errorCount = 0;
    }
    return;
  }

  errorCount = 0; // Reset biến lỗi nếu đọc thành công

  // Hiển thị lên Serial Monitor
  Serial.print("Nhiệt độ: ");
  Serial.print(temperature);
  Serial.print(" °C  |  Độ ẩm: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, humidity);
}

void setup() {
  Serial.begin(115200);
  pinMode(pinBLED, OUTPUT);
  pinMode(btnBLED, INPUT_PULLUP);
  
  display.setBrightness(0x0f);
  dht.setup(DHTPIN, DHTesp::DHT22);  // Khởi động cảm biến DHT22

  Serial.print("Connecting to "); Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("WiFi connected");

  digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);  
  Blynk.virtualWrite(V1, blueButtonON); 

  Serial.println("== START ==");
}

void loop() {  
  Blynk.run();  

  currentMiliseconds = millis();
  uptimeBlynk();
  updateBlueButton();

  static ulong lastDHTRead = 0;
  if (currentMiliseconds - lastDHTRead > 2000) { // Đọc mỗi 5 giây
    lastDHTRead = currentMiliseconds;
    sendSensor();
  }
}

// Kiểm tra thời gian để hạn chế thao tác quá nhanh
bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

// Xử lý nút bấm đèn xanh
void updateBlueButton() {
  static ulong lastTime = 0;
  static int lastValue = HIGH;
  if (!IsReady(lastTime, 50)) return;
  
  int v = digitalRead(btnBLED);
  if (v == lastValue) return;
  lastValue = v;
  if (v == LOW) return;

  if (!blueButtonON) {
    Serial.println("Blue Light ON");
    digitalWrite(pinBLED, HIGH);
    blueButtonON = true;
    Blynk.virtualWrite(V1, blueButtonON);
  } else {
    Serial.println("Blue Light OFF");
    digitalWrite(pinBLED, LOW);    
    blueButtonON = false;
    Blynk.virtualWrite(V1, blueButtonON);
    display.clear();
  }    
}

// Cập nhật thời gian hoạt động lên Blynk
void uptimeBlynk() {
  static ulong lastTime = 0;
  if (!IsReady(lastTime, 1000)) return;
  
  ulong value = lastTime / 1000;
  Blynk.virtualWrite(V0, value);  

  if (blueButtonON) {
    display.showNumberDec(value);
  }
}

// Xử lý dữ liệu từ Blynk
BLYNK_WRITE(V1) { 
  blueButtonON = param.asInt();  
  if (blueButtonON) {
    Serial.println("Blynk -> Blue Light ON");
    digitalWrite(pinBLED, HIGH);
  } else {
    Serial.println("Blynk -> Blue Light OFF");
    digitalWrite(pinBLED, LOW);   
    display.clear(); 
  }
}
