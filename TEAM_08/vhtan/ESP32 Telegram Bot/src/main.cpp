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

// Kết nối WIFI
const char* ssid = "Wokwi-GUEST";  // Tên Wi-Fi (thường sẽ là Wokwi-GUEST nếu sử dụng bảng mạch ảo)
const char* password = "";         // Mật khẩu Wi-Fi

// Initialize Telegram BOT
#define BOTtoken "7619429343:AAGqmzURxunRvxQdZU2Cjdl3rwXcAZtnLKQ"  // Mã Token của Telegram Bot (lấy từ BotFather)
#define GROUP_ID "-1002574788554" // ID của nhóm Telegram (thường là số âm)

WiFiClientSecure client; //Tạo một kết nối HTTPS an toàn.
UniversalTelegramBot bot(BOTtoken, client); // Đối tượng giúp giao tiếp với Telegram bot.

const int motionSensor = 27; // Chân GPIO của ESP32 để đọc dữ liệu từ cảm biến PIR (chân 27).
bool motionDetected = false; //Biến để đánh dấu khi có chuyển động: Khi cảm biến phát hiện chuyển động, biến này sẽ đổi thành true.

//Định dạng chuỗi %s,%d,...
String StringFormat(const char* fmt, ...){
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

// Hàm xử lý khi phát hiện chuyển động
void IRAM_ATTR detectsMovement() {
  //Serial.println("MOTION DETECTED!!!");
  motionDetected = true;
}

void setup() {
  Serial.begin(115200);

  // Cảm biến PIR ở chế độ INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  
  bot.sendMessage(GROUP_ID, "IoT Developer started up");
}


void loop() {
  static uint count_ = 0;

  if(motionDetected){
    ++count_;
    Serial.print(count_);Serial.println(". MOTION DETECTED => Waiting to send to Telegram");    
    String msg = StringFormat("%u => Motion detected!",count_);
    bot.sendMessage(GROUP_ID, msg.c_str());
    Serial.print(count_);Serial.println(". Sent successfully to Telegram: Motion Detected");
    motionDetected = false;
  }
}