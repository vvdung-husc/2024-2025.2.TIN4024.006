#include <Arduino.h>
#include "utils.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <U8g2lib.h>

#define BLYNK_TEMPLATE_ID "TMPL6c_gqr655"
#define BLYNK_TEMPLATE_NAME "ESP8266 Project"
#define BLYNK_AUTH_TOKEN "H2zzhUiXIihOz-6LOFb865Y15ZHGp3Cs"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu cho việc chạy mô phỏng
char ssid[] = "Wokwi-GUEST";  //Tên mạng WiFi
char pass[] = "";             //Mật khẩu mạng WiFi

#define gPIN 15
#define yPIN 2
#define rPIN 5

#define dhtPIN 16     // Digital pin connected to the DHT sensor
#define dhtTYPE DHT11 // DHT 22 (AM2302)

#define OLED_SDA 13
#define OLED_SCL 12

// Khởi tạo OLED SH1106
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

DHT dht(D0, dhtTYPE);


bool WelcomeDisplayTimeout(uint msSleep = 5000){
  static ulong lastTimer = 0;
  static bool bDone = false;
  if (bDone) return true;
  if (!IsReady(lastTimer, msSleep)) return false;
  bDone = true;    
  return bDone;
}


void setup() {
  Serial.begin(115200);
  pinMode(gPIN, OUTPUT);
  pinMode(yPIN, OUTPUT);
  pinMode(rPIN, OUTPUT);
  
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, LOW);

  // Start the WiFi connection
  Serial.print("Connecting to ");Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid, pass); //Kết nối đến mạng WiFi

  Serial.println();
  Serial.println("WiFi connected");

  dht.begin();

  Wire.begin(OLED_SDA, OLED_SCL);  // SDA, SCL

  oled.begin();
  oled.clearBuffer();
  
  oled.setFont(u8g2_font_unifont_t_vietnamese1);
  oled.drawUTF8(0, 14, "Trường ĐHKH");  
  oled.drawUTF8(0, 28, "Khoa CNTT");
  oled.drawUTF8(0, 42, "Lập trình IoT");  

  oled.sendBuffer();
  // Đồng bộ trạng thái ban đầu từ Blynk
  Blynk.syncVirtual(V0);
}

void ThreeLedBlink(){
  static ulong lastTimer = 0;
  static int currentLed = 0;  
  static const int ledPin[3] = {gPIN, yPIN, rPIN};

  if (!IsReady(lastTimer, 1000)) return;
  int prevLed = (currentLed + 2) % 3;
  digitalWrite(ledPin[prevLed], LOW);  
  digitalWrite(ledPin[currentLed], HIGH);  
  currentLed = (currentLed + 1) % 3;
}

float fHumidity = 0.0;
float fTemperature = 0.0;

BLYNK_WRITE(V0) {  // Hàm xử lý nút bật/tắt đèn vàng
  int value = param.asInt(); // Lấy giá trị từ Blynk (0 hoặc 1)
  digitalWrite(yPIN, value); // Điều khiển đèn vàng
}

void updateTime() {
  static ulong lastTimer = 0;
  if (!IsReady(lastTimer, 1000)) return; // Cập nhật mỗi 1 giây
  
  unsigned long uptime = millis() / 1000; // Lấy thời gian chạy tính bằng giây
  int hours = uptime / 3600;
  int minutes = (uptime % 3600) / 60;
  int seconds = uptime % 60;
  
  String currentTime = String(hours) + ":" + 
                      (minutes < 10 ? "0" : "") + String(minutes) + ":" + 
                      (seconds < 10 ? "0" : "") + String(seconds);
  
  Blynk.virtualWrite(V1, currentTime);
}

void updateDHT(){
  static ulong lastTimer = 0;  
  if (!IsReady(lastTimer, 2000)) return;

  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  bool bDraw = false;

  if (fTemperature != t){
    bDraw = true;
    fTemperature = t;
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C"); 
    Blynk.virtualWrite(V2, t);                   
  }

  if (fHumidity != h){
    bDraw = true;
    fHumidity = h;
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");  
    Blynk.virtualWrite(V3, t); 
  }
  if (bDraw){
    oled.clearBuffer();
    oled.setFont(u8g2_font_unifont_t_vietnamese2);

    String s = StringFormat("Nhiet do: %.2f °C", t);
    oled.drawUTF8(0, 14, s.c_str());  
    
    s = StringFormat("Do am: %.2f %%", h);
    oled.drawUTF8(0, 42, s.c_str());      

    oled.sendBuffer();
  } 
  
}

void DrawCounter(){  
  static uint counter = 0; // Biến đếm
  static ulong lastTimer = 0;  
  if (!IsReady(lastTimer, 2000)) return;

  // Bắt đầu vẽ màn hình
  oled.clearBuffer();  
  oled.setFont(u8g2_font_logisoso32_tf); // Chọn font lớn để hiển thị số
  oled.setCursor(30, 40); // Đặt vị trí chữ
  oled.print(counter); // Hiển thị số đếm
  oled.sendBuffer(); // Gửi dữ liệu lên màn hình

  counter++; // Tăng giá trị đếm

}

void loop() {
  Blynk.run();
  if (!WelcomeDisplayTimeout()) return;
  ThreeLedBlink();
  updateDHT();
  updateTime(); // Thêm hàm cập nhật thời gian
}