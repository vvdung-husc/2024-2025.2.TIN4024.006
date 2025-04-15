// Hoàng Hữu Hội
// #define BLYNK_TEMPLATE_ID "TMPL6_v5ZkGLx"
// #define BLYNK_TEMPLATE_NAME "BaiTapNhom3"
// #define BLYNK_AUTH_TOKEN "aZaQRvmSEcYj7OQ_MLVMfpJw2CVrXFXD"
//Lê Đình Trung
// #define BLYNK_TEMPLATE_ID "TMPL6aAxLlxjB"
// #define BLYNK_TEMPLATE_NAME "ESP8266BlynkTelegrambot"
// #define BLYNK_AUTH_TOKEN "aCXRFPvWlwdESAlkBvOHBwJ7-x3LN1sT"
// Đặng Trọng Trí Tuệ
// #define BLYNK_TEMPLATE_ID "TMPL6wt_mGzLq"
// #define BLYNK_TEMPLATE_NAME "BaiTapNhom3"
// #define BLYNK_AUTH_TOKEN "xNgTnMqAzz4-VYipg4ZZ3eB-PHcvnmEg"
// Lê Đức Duy Anh
// #define BLYNK_TEMPLATE_ID "TMPL6T44Uqo5P"
// #define BLYNK_TEMPLATE_NAME "BaiTapNhom3"
// #define BLYNK_AUTH_TOKEN "-E1eTMRpGxeqTDp4jy4wkALmI7hagTMF"
// Nguyễn Hoàng Anh Tú
#define BLYNK_TEMPLATE_ID "TMPL66Vvyofjj"
#define BLYNK_TEMPLATE_NAME "BaiTapNhom3"
#define BLYNK_AUTH_TOKEN "nTKk5RlwVtfhP55pu19jLGSC-y-bi0Rg"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Thông tin WiFi
#define WIFI_SSID "CNTT-MMT"
#define WIFI_PASS "13572468"


// Cấu hình Telegram Bot
// #define BOTtoken "7882389525:AAHSlKS59-U7bb2y0Ea4FWQlwW2tqtvuhOw"
#define CHAT_ID "-4728373238"
//Lê Đình Trung
// #define BOTtoken "7757452277:AAEg_Zevqwn-fzKRqxpESJwAYmXkJUuU8UQ"
// #define CHAT_ID "-4673960554"
// Đặng Trọng Trí Tuệ
// #define BOTtoken "7860410039:AAEfVfs5V3GmFYfbNFeUkQF7H4EuMCxKM6g"
// #define GROUP_ID "-4753535169"
// Lê Đức Duy Anh
// #define BOTtoken "7953104144:AAES14DfJQDNXIIf3WOckh2b64tN41GMnFM"
// #define GROUP_ID "-1002672539893"
// Nguyễn Hoàng Anh Tú
#define BOTtoken "7746876071:AAFRmWn2MtZmcUZmYe5ZxodREJmBkkLqBys"
// Định nghĩa chân kết nối
#define gPIN 15 // Đèn LED Xanh
#define yPIN 2  // Đèn LED Vàng
#define rPIN 5  // Đèn LED Đỏ

// Khai báo nguyên mẫu hàm
void trafficLightSequence();
void blinkYellowLight();
void turnOffTrafficLights();
void checkHealthWarnings(float temp, float humidity);
void checkTelegramMessages();
void updateOLEDDisplay();
String checkTemperatureThreshold(float temp);
String checkHumidityThreshold(float humidity);

// Khai báo đối tượng
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/12, /* data=*/13, /* reset=*/U8X8_PIN_NONE);

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOTtoken, secured_client);

// Biến điều khiển
bool trafficLightEnabled = true;
bool yellowBlinkMode = false;
unsigned long lastTelegramCheck = 0;
unsigned long lastHealthCheck = 0;
unsigned long lastOLEDUpdate = 0;
unsigned long startTime = 0;
unsigned long lastYellowToggle = 0;

// Biến lưu trữ nhiệt độ và độ ẩm - sinh ngẫu nhiên
float currentTemperature = 0;
float currentHumidity = 0;

// Hàm sinh số ngẫu nhiên
float generateRandomFloat(float min, float max)
{
  return min + (max - min) * (float)random(10001) / 10000.0;
}

String checkTemperatureThreshold(float temp)
{
  if (temp < 10)
    return "Nguy cơ suy giảm miễn dịch";
  if (temp >= 10 && temp < 15)
    return "Tăng nguy cơ bệnh hô hấp";
  if (temp >= 25 && temp <= 30)
    return "Nhiệt độ lý tưởng";
  if (temp > 35 && temp <= 40)
    return "Nguy cơ sốc nhiệt";
  if (temp > 40)
    return "CỰC KỲ NGUY HIỂM";
  return "Bình thường";
}

String checkHumidityThreshold(float humidity)
{
  if (humidity < 30)
    return "Da khô, kích ứng";
  if (humidity >= 40 && humidity <= 60)
    return "Mức lý tưởng";
  if (humidity > 70 && humidity <= 80)
    return "Tăng nguy cơ nấm mốc";
  if (humidity > 80)
    return "Khó thở, nguy cơ sốc nhiệt";
  return "Bình thường";
}

void checkHealthWarnings(float temp, float humidity)
{
  String tempWarning = checkTemperatureThreshold(temp);
  String humidityWarning = checkHumidityThreshold(humidity);

  bool healthRisk =
      (tempWarning != "Bình thường" && tempWarning != "Nhiệt độ lý tưởng") ||
      (humidityWarning != "Bình thường" && humidityWarning != "Mức lý tưởng");

  if (healthRisk)
  {
    String warningMessage = "Cảnh báo sức khỏe:\n";
    warningMessage += "Nhiệt độ " + String(temp) + "°C - " + tempWarning + "\n";
    warningMessage += "Độ ẩm " + String(humidity) + "% - " + humidityWarning;

    bot.sendMessage(CHAT_ID, warningMessage, "");
  }
}

void blinkYellowLight()
{
  unsigned long currentTime = millis();
  if (currentTime - lastYellowToggle >= 500)
  { // Nhấp nháy 2 lần/giây
    digitalWrite(yPIN, !digitalRead(yPIN));
    lastYellowToggle = currentTime;
  }
}

void updateOLEDDisplay()
{
  // Tính thời gian hoạt động
  unsigned long currentTime = millis();
  unsigned long uptime = (currentTime - startTime) / 1000;

  int hours = uptime / 3600;
  int minutes = (uptime % 3600) / 60;
  int seconds = uptime % 60;

  // Xóa bộ đệm
  u8g2.clearBuffer();

  // Thời gian hoạt động
  char timeStr[30];
  snprintf(timeStr, sizeof(timeStr), "Thoi gian: %uh %02um %02us", hours, minutes, seconds);
  u8g2.setCursor(2, 10);
  u8g2.print(timeStr);

  // Hiển thị nhiệt độ
  char tempStr[30];
  snprintf(tempStr, sizeof(tempStr), "Nhiet do: %.1f C", currentTemperature);
  u8g2.setCursor(2, 25);
  u8g2.print(tempStr);

  // Hiển thị độ ẩm
  char humStr[30];
  snprintf(humStr, sizeof(humStr), "Do am: %.1f %%", currentHumidity);
  u8g2.setCursor(2, 40);
  u8g2.print(humStr);

  // Gửi dữ liệu đến màn hình OLED
  u8g2.sendBuffer();
}

void trafficLightSequence()
{
  // Đèn Xanh
  digitalWrite(gPIN, HIGH);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, LOW);
  delay(3000);

  // Đèn Vàng
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, HIGH);
  digitalWrite(rPIN, LOW);
  delay(1000);

  // Đèn Đỏ
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, HIGH);
  delay(3000);
}

void turnOffTrafficLights()
{
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, LOW);
}

void checkTelegramMessages()
{
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    // Tắt đèn giao thông
    if (text == "/traffic_off")
    {
      trafficLightEnabled = false;
      bot.sendMessage(chat_id, "Đã tắt đèn giao thông.", "");
    }

    // Bật đèn giao thông
    if (text == "/traffic_on")
    {
      trafficLightEnabled = true;
      bot.sendMessage(chat_id, "Đã bật đèn giao thông.", "");
    }
  }
}

void setup()
{
  Serial.begin(115200);

  // Khởi tạo bộ sinh số ngẫu nhiên
  randomSeed(analogRead(0));

  // Cài đặt chân LED
  pinMode(gPIN, OUTPUT);
  pinMode(yPIN, OUTPUT);
  pinMode(rPIN, OUTPUT);

  // Khởi tạo màn hình OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // Kết nối WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Khởi tạo Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

  // Cấu hình kết nối an toàn cho Telegram
  secured_client.setInsecure();

  // Lưu thời gian bắt đầu
  startTime = millis();
}

void loop()
{
  Blynk.run();

  // Cập nhật OLED và dữ liệu sau mỗi 5 giây
  unsigned long currentTime = millis();
  if (currentTime - lastOLEDUpdate >= 5000)
  {
    // Sinh ngẫu nhiên nhiệt độ và độ ẩm
    currentTemperature = generateRandomFloat(-40.0, 80.0);
    currentHumidity = generateRandomFloat(0.0, 100.0);

    // Cập nhật dữ liệu lên Blynk
    Blynk.virtualWrite(V1, currentTemperature); // Nhiệt độ
    Blynk.virtualWrite(V2, currentHumidity);    // Độ ẩm

    // Cập nhật hiển thị OLED
    updateOLEDDisplay();

    lastOLEDUpdate = currentTime;
  }

  // Tính thời gian hoạt động liên tục
  unsigned long uptime = (currentTime - startTime) / 1000;
  Blynk.virtualWrite(V0, uptime); // Thời gian hoạt động

  // Chế độ điều khiển đèn giao thông
  if (trafficLightEnabled)
  {
    if (yellowBlinkMode)
    {
      blinkYellowLight();
    }
    else
    {
      trafficLightSequence();
    }
  }
  else
  {
    turnOffTrafficLights();
  }

  // Kiểm tra cảnh báo sức khỏe (5 phút một lần)
  if (currentTime - lastHealthCheck >= 300000)
  {
    checkHealthWarnings(currentTemperature, currentHumidity);
    lastHealthCheck = currentTime;
  }

  // Kiểm tra tin nhắn Telegram
  if (currentTime - lastTelegramCheck >= 1000)
  {
    checkTelegramMessages();
    lastTelegramCheck = currentTime;
  }
}

// Hàm xử lý switch điều khiển đèn giao thông trên Blynk
BLYNK_WRITE(V3)
{
  int switchValue = param.asInt();
  if (switchValue == 1)
  {
    yellowBlinkMode = true;
    trafficLightEnabled = true;
  }
  else
  {
    yellowBlinkMode = false;
    trafficLightEnabled = true;
  }
}
