#include <Arduino.h>

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-esp32-motion-detection-arduino/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define DEVICE_NAME "Bé Na"  // Đổi tên "ESP32_2", "ESP32_3", ...


// Kết nối WIFI
const char* ssid = "Wokwi-GUEST";  // Tên Wi-Fi (thường sẽ là Wokwi-GUEST nếu sử dụng bảng mạch ảo)
const char* password = "";         // Mật khẩu Wi-Fi

// Initialize Telegram BOT
#define BOTtoken "7619429343:AAFP4EzOeqa04U6v9XRFeBQBylQ4DnY5Nec"  // Mã Token của Telegram Bot (lấy từ BotFather)
#define GROUP_ID "7582904058" // ID của nhóm Telegram (thường là số âm)

WiFiClientSecure client; //Tạo một kết nối HTTPS an toàn.
UniversalTelegramBot bot(BOTtoken, client); // Đối tượng giúp giao tiếp với Telegram bot.

const int motionSensor = 27;  // Chân kết nối cảm biến
bool motionDetected = false;

// 🛠️ Hàm format chuỗi (Giúp tạo tin nhắn đẹp)
String StringFormat(const char* fmt, ...) {
  va_list vaArgs;
  va_start(vaArgs, fmt);
  va_list vaArgsCopy;
  va_copy(vaArgsCopy, vaArgs);
  const int iLen = vsnprintf(NULL, 0, fmt, vaArgsCopy);
  va_end(vaArgsCopy);
  int iSize = iLen + 1;
  char* buff = (char*)malloc(iSize);
  vsnprintf(buff, iSize, fmt, vaArgs);
  va_end(vaArgs);
  String s = buff;
  free(buff);
  return String(s);
}

// 📌 Hàm xử lý khi phát hiện chuyển động
void IRAM_ATTR detectsMovement() {
  motionDetected = true;
}

void setup() {
  Serial.begin(115200);

  // 🚀 Cấu hình cảm biến chuyển động PIR
  pinMode(motionSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  // 🌐 Kết nối WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Thêm chứng chỉ root cho api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nWiFi connected");
  
  // ✉️ Gửi tin nhắn khởi động ESP32 lên Telegram
  String startup_msg = StringFormat("[%s] IoT Developer started up", DEVICE_NAME);
  bot.sendMessage(GROUP_ID, startup_msg.c_str());
}

void loop() {
  static uint count_ = 0;

  if (motionDetected) {
    ++count_;
    Serial.print(count_);
    Serial.println(". MOTION DETECTED => Sending to Telegram");

    // 📨 Gửi tin nhắn chứa tên thiết bị
    String msg = StringFormat("[%s] %u => Motion detected!", DEVICE_NAME, count_);
    bot.sendMessage(GROUP_ID, msg.c_str());

    Serial.print(count_);
    Serial.println(". Sent successfully to Telegram");

    motionDetected = false;
  }
}