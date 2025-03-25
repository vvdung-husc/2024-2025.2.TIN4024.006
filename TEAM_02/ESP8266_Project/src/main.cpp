// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL6vCrMNkel"
#define BLYNK_TEMPLATE_NAME "ESP8266 Project"
#define BLYNK_AUTH_TOKEN "CsgSkLzpxCu1uhHhnHlwJu1JkGXGkSSF"
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define gPIN 15
#define yPIN 2
#define rPIN 5

#define OLED_SDA 13
#define OLED_SCL 12


char ssid[] = "CNTT-MMT";
char pass[] = "13572468";

// Khởi tạo OLED SH1106
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Global variables for Blynk
float temperature = 0.0;
float humidity = 0.0;
unsigned long uptime = 0;

bool IsReady(unsigned long& lastTimer, unsigned long interval) {
  unsigned long currentTime = millis();
  if (currentTime - lastTimer >= interval) {
    lastTimer = currentTime;
    return true;
  }
  return false;
}

bool WelcomeDisplayTimeout(uint msSleep = 5000){
  static unsigned long lastTimer = 0;
  static bool bDone = false;
  if (bDone) return true;
  if (!IsReady(lastTimer, msSleep)) return false;
  bDone = true;    
  return bDone;
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V1) {
  int pinValue = param.asInt(); // Đọc giá trị từ nút switch V1
  digitalWrite(yPIN, pinValue); // Bật/tắt đèn vàng
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0)); // Khởi tạo seed cho random
  
  pinMode(gPIN, OUTPUT);
  pinMode(yPIN, OUTPUT);
  pinMode(rPIN, OUTPUT);
  
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, LOW);

  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin();
  oled.clearBuffer();
  
  // Kết nối Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  oled.setFont(u8g2_font_unifont_t_vietnamese1);
  oled.drawUTF8(0, 14, "Trường ĐHKH");  
  oled.drawUTF8(0, 28, "Khoa CNTT");
  oled.drawUTF8(0, 42, "Lập trình IoT");  
  oled.sendBuffer();
}

void ThreeLedBlink(){
  static unsigned long lastTimer = 0;
  static int currentLed = 0;  
  static const int ledPin[3] = {gPIN, yPIN, rPIN};
  static const int durations[3] = {7000, 2000, 5000};

  if (!IsReady(lastTimer, durations[currentLed])) return;

  int prevLed = (currentLed + 2) % 3;
  digitalWrite(ledPin[prevLed], LOW);  
  digitalWrite(ledPin[currentLed], HIGH);  
  currentLed = (currentLed + 1) % 3;
}

// Hàm tạo nhiệt độ ngẫu nhiên từ -40.0 đến 80.0
float generateRandomTemperature() {
  return random(-400, 801) / 10.0; // -40.0 đến 80.0
}

// Hàm tạo độ ẩm ngẫu nhiên từ 0.0 đến 100.0
float generateRandomHumidity() {
  return random(0, 1001) / 10.0; // 0.0 đến 100.0
}

// Hàm hiển thị thời gian hoạt động
void displayUptime() {
  uptime = millis() / 1000; // Chuyển đổi millis() thành giây
  String uptimeStr = "Thoi gian: " + String(uptime) + "s";
  
  oled.drawUTF8(0, 48, uptimeStr.c_str());
}

void updateRandomDHT(){
  static unsigned long lastTimer = 0;  
  if (!IsReady(lastTimer, 1000)) return; // Cập nhật mỗi 1 giây

  // Tạo giá trị ngẫu nhiên
  temperature = generateRandomTemperature();
  humidity = generateRandomHumidity();

  // In ra Serial để kiểm tra
  Serial.print("Random Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("Random Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V0, uptime);      // Gửi thời gian hoạt động lên V0
  Blynk.virtualWrite(V2, temperature);  // Gửi nhiệt độ lên V2
  Blynk.virtualWrite(V3, humidity);     // Gửi độ ẩm lên V3

  // Hiển thị lên OLED
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese1);
  String tempStr = "Nhiet do: " + String(temperature, 1) + " C";
  String humStr = "Do am: " + String(humidity, 1) + " %";
  
  oled.drawUTF8(0, 16, tempStr.c_str());  
  oled.drawUTF8(0, 32, humStr.c_str());  
  displayUptime();
  oled.sendBuffer();
}

void loop() {
  Blynk.run(); // Chạy Blynk
  
  if (!WelcomeDisplayTimeout()) return;
  ThreeLedBlink();
  updateRandomDHT();
}