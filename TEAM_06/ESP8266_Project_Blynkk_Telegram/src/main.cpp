
//Trần Hải Đông
#define BLYNK_TEMPLATE_ID "TMPL6hl4V_1Lr"
#define BLYNK_TEMPLATE_NAME "esp8266 blynk telegram"
#define BLYNK_AUTH_TOKEN "Ex622jIU2X7ixiJfYpOtA6tvmYhYijv4"

//Nguyễn Ngọc Tiến Đạt
// #define BLYNK_TEMPLATE_ID "TMPL6IUsklK2w"
// #define BLYNK_TEMPLATE_NAME "ESP8266 Project Blynk Telegram"
// #define BLYNK_AUTH_TOKEN "2PxzaHtrIk7sCPT8FV4fanSC8CofObeR"


// //Nguyễn Văn Tuấn Hải
// #define BLYNK_TEMPLATE_ID "TMPL66BocAkvr"
// #define BLYNK_TEMPLATE_NAME "ESP8266 Blynk Tele"
// #define BLYNK_AUTH_TOKEN "KWjPv-LRxnvl-hvgFp3iyF4y3cp--eC-"

//Huỳnh Ngọc Vĩnh Khang
//#define BLYNK_TEMPLATE_ID "TMPL6J7ZiGUe4"
//#define BLYNK_TEMPLATE_NAME "ESP8266 TELEGRAM"
//#define BLYNK_AUTH_TOKEN "l6fU1dx7EFGOTpMw2I25mhpC0dyxWmkx"

//Dương Duy Quốc
// #define BLYNK_TEMPLATE_ID "TMPL6CAb1i-FP"
// #define BLYNK_TEMPLATE_NAME "Quoc"
// #define BLYNK_AUTH_TOKEN "zAqkB_WAIkQhDwwl322ZBsh04YgWlXv"

//Nguyễn Đức Huy
// #define BLYNK_TEMPLATE_ID "TMPL6r3KCHcQS"
// #define BLYNK_TEMPLATE_NAME "ESP8266 TELEGRAM"
// #define BLYNK_AUTH_TOKEN "h1H-2UILxsZ7oHUbu2fW2I47T-x_aHtv"

#include <Arduino.h>   
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// 🟢 Chân LED (đỏ, vàng, xanh)
#define gPIN 15
#define yPIN 2
#define rPIN 5

// 📟 Chân I2C OLED
#define OLED_SDA 13
#define OLED_SCL 12

// 🖥 Khởi tạo OLED SH1106
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// ⏳ Biến đếm thời gian
unsigned long runTime = 0;

// 🌡 Biến nhiệt độ & độ ẩm
float fTemperature = 0.0;
float fHumidity = 0.0;

// 🔁 Trạng thái đèn vàng nhấp nháy
bool yellowBlinkMode = false;

// 🔌 Kết nối Blynk
BlynkTimer timer;

// 📶 Bắt sự kiện Switch từ Blynk (V1: bật/tắt nhấp nháy đèn vàng)
BLYNK_WRITE(V1) {
  yellowBlinkMode = param.asInt(); // 1 = bật, 0 = tắt
}

//Trần Hải Đông
#define BOTtoken "7905069023:AAEL-dgmljUYwG3Hihu73O7bNPoUtfnMPC4"
#define USER_ID "7196545948" 

//Nguyễn Ngọc Tiến Đạt
// #define BOTtoken "7996806265:AAHeTN2z6k0muLRw4X99sSDFv579rlnBcoU"
// #define GROUP_ID "-4604565399"

//Nguyễn Văn Tuấn Hải
// #define BOTtoken "7845979937:AAFVcEGgh9baF0sUOusSQXxO-21x6F-OfCE"
// #define USER_ID "7413444616"

//Huỳnh Ngọc Vĩnh Khang
// #define BOTtoken "7761669773:AAEfawBH-nA9ONjIG1oTC5sIQ3e4wEuddsA"
// #define USER_ID "8000470021"

//Dương Duy Quốc
// #define BOTtoken "8108680571:AAFHtHHaKovNDdEDxjO15XM-FkC8oLX9kQ0" 
// #define USER_ID "1338793809" 

// Group ID
#define GROUP_ID "-1002594207741"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Trạng thái đèn giao thông
bool trafficLightsOn = true;

// 📟 Hiển thị màn hình chào mừng
bool WelcomeDisplayTimeout(unsigned int msSleep = 3000) {
  static unsigned long lastTimer = 0;
  static bool bDone = false;
  if (bDone) return true;
  if (millis() - lastTimer < msSleep) return false;
  bDone = true;
  return bDone;
}

// 📟 Hiển thị trên OLED
void updateOLED() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese2);

  oled.drawUTF8(0, 14, ("Nhiet do: " + String(fTemperature, 1) + "°C").c_str());
  oled.drawUTF8(0, 28, ("Do am: " + String(fHumidity, 1) + "%").c_str());
  oled.drawUTF8(0, 42, ("Thoi gian: " + String(runTime) + "s").c_str());

  oled.sendBuffer();
}

// 🚥 Điều khiển đèn giao thông
void TrafficLightControl() {
  if (!trafficLightsOn) {
    digitalWrite(rPIN, LOW);
    digitalWrite(yPIN, LOW);
    digitalWrite(gPIN, LOW);
    return;
  }

  static unsigned long lastTimer = 0;
  static int state = 0;
  static const unsigned long durations[] = {2000, 3000, 1000}; // Thời gian cho từng màu (ms)
  static const int ledPins[] = {rPIN, gPIN, yPIN};

  if (yellowBlinkMode) {
    if (millis() - lastTimer > 500) {
      lastTimer = millis();
      digitalWrite(yPIN, !digitalRead(yPIN));
    }
    digitalWrite(rPIN, LOW);
    digitalWrite(gPIN, LOW);
    return;
  }

  if (millis() - lastTimer > durations[state]) {
    lastTimer = millis();
    digitalWrite(ledPins[state], LOW);
    state = (state + 1) % 3;
    digitalWrite(ledPins[state], HIGH);
  }
}

// 📟 Gửi dữ liệu lên Blynk
void sendToBlynk() {
  Blynk.virtualWrite(V0, runTime);      // Thời gian chạy
  Blynk.virtualWrite(V2, fTemperature); // Nhiệt độ
  Blynk.virtualWrite(V3, fHumidity);    // Độ ẩm
}

// 🔢 Sinh dữ liệu nhiệt độ & độ ẩm ngẫu nhiên
float randomTemperature() {
  return random(-400, 800) / 10.0;
}

float randomHumidity() {
  return random(0, 1000) / 10.0;
}

// 🌡 Cập nhật nhiệt độ & độ ẩm
void updateSensorData() {
  static unsigned long lastTimer = 0;
  if (millis() - lastTimer < 2000) return;
  lastTimer = millis();

  fTemperature = randomTemperature();
  fHumidity = randomHumidity();
  Serial.print("Nhiet do: ");
  Serial.print(fTemperature);
  Serial.println("°C");

  Serial.print("Do am: ");
  Serial.print(fHumidity);
  Serial.println("%");
}

// 🔢 Hiển thị thời gian chạy
void updateRunTime() {
  static unsigned long lastTimer = 0;
  if (millis() - lastTimer < 1000) return;
  lastTimer = millis();

  runTime++;
}

// Telegram: Gửi cảnh báo nếu nhiệt độ hoặc độ ẩm nguy hiểm
void checkAndSendAlert() {
  if (fTemperature < 10 || fTemperature > 35 || fHumidity < 30 || fHumidity > 80) {
    String message = "Cảnh báo: Điều kiện môi trường có thể gây nguy hại cho sức khỏe!\n";
    message += "Nhiệt độ: " + String(fTemperature, 1) + "°C\n";
    message += "Độ ẩm: " + String(fHumidity, 1) + "%\n";
    if (fTemperature < 10) {
      message += "Nguy cơ hạ thân nhiệt, tê cóng.\n";
    } else if (fTemperature > 35) {
      message += "Nguy cơ sốc nhiệt, say nắng.\n";
    }
    if (fHumidity < 30) {
      message += "Da khô, kích ứng mắt, tăng nguy cơ bệnh hô hấp.\n";
    } else if (fHumidity > 80) {
      message += "Oi bức, khó thở, tăng nguy cơ sốc nhiệt.\n";
    }
    bot.sendMessage(GROUP_ID, message, "");
  }
}

// Telegram: Xử lý tin nhắn từ Telegram
void handleTelegramMessages() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != GROUP_ID) continue;
    String text = bot.messages[i].text;
    if (text == "/traffic_off") {
      trafficLightsOn = false;
      bot.sendMessage(GROUP_ID, "Đèn giao thông đã tắt.", "");
    } else if (text == "/traffic_on") {
      trafficLightsOn = true;
      bot.sendMessage(GROUP_ID, "Đèn giao thông đã bật.", "");
    }
  }
}

// 🏁 SETUP
void setup() {
  Serial.begin(115200);

  // 🛠 Cấu hình LED
  pinMode(gPIN, OUTPUT);
  pinMode(yPIN, OUTPUT);
  pinMode(rPIN, OUTPUT);
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, HIGH); // Bắt đầu với đèn đỏ

  // 📟 Cấu hình OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese1);
  oled.drawUTF8(0, 14, "Khoi dong...");
  oled.sendBuffer();

  // 🌍 Kết nối WiFi & Blynk
  const char* ssid = "CNTT-MMT";       // Thay bằng SSID thực tế
  const char* password = "13572468";   // Thay bằng mật khẩu thực tế

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Cấu hình Telegram
  client.setInsecure(); // Bỏ qua kiểm tra chứng chỉ SSL

  // ⏳ Hẹn giờ
  timer.setInterval(2000L, sendToBlynk);          // Gửi dữ liệu lên Blynk mỗi 2 giây
  timer.setInterval(20000L, checkAndSendAlert);   // Gửi cảnh báo Telegram mỗi 20 giây
  timer.setInterval(5000L, handleTelegramMessages); // Kiểm tra tin nhắn Telegram mỗi 5 giây
}

// 🔁 LOOP
void loop() {
  Blynk.run();
  timer.run();

  if (!WelcomeDisplayTimeout()) return;

  TrafficLightControl();
  updateSensorData();
  updateRunTime();
  updateOLED();
}