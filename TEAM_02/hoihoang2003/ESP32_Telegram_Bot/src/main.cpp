#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Cấu hình WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Cấu hình Telegram Bot
#define BOT_TOKEN "7882389525:AAHSlKS59-U7bb2y0Ea4FWQlwW2tqtvuhOw"
#define CHAT_ID "-4728373238"

// Cấu hình chân cảm biến PIR
const int PIR_SENSOR = 27;  // Chân OUT của cảm biến PIR kết nối với GPIO27

// Cấu hình thời gian
unsigned long lastTimeBotRan;
const unsigned long BOT_MTBS = 1000; // Thời gian chờ giữa các lần kiểm tra tin nhắn (1 giây)

// Khởi tạo đối tượng WiFiClientSecure
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// Biến trạng thái cảm biến
int pirState = LOW;      // Trạng thái hiện tại của cảm biến
int lastPirState = LOW;  // Trạng thái trước đó của cảm biến
bool notificationSent = false;
unsigned long lastMotionTime = 0;
const unsigned long MOTION_TIMEOUT = 60000; // Thời gian chờ giữa các thông báo (1 phút)

// Khai báo prototype của hàm handleNewMessages
void handleNewMessages(int numNewMessages);

void setup() {
  // Khởi tạo Serial Monitor
  Serial.begin(115200);
  
  // Cấu hình chân PIR là INPUT
  pinMode(PIR_SENSOR, INPUT);
  
  // Kết nối WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Thêm chứng chỉ root cho HTTPS
  
  Serial.print("Đang kết nối WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("Đã kết nối WiFi, địa chỉ IP: ");
  Serial.println(WiFi.localIP());
  
  // Gửi thông báo khởi động
  bot.sendMessage(CHAT_ID, "Hệ thống cảm biến chuyển động đã khởi động!", "");
}

void loop() {
  // Đọc trạng thái cảm biến PIR
  pirState = digitalRead(PIR_SENSOR);
  
  // Kiểm tra nếu có chuyển động (từ LOW sang HIGH)
  if (pirState == HIGH && lastPirState == LOW) {
    Serial.println("Phát hiện chuyển động!");
    
    // Kiểm tra nếu đã đủ thời gian từ thông báo cuối
    unsigned long currentTime = millis();
    if (!notificationSent || (currentTime - lastMotionTime > MOTION_TIMEOUT)) {
      // Gửi thông báo qua Telegram
      String message = "⚠️ CẢNH BÁO: Phát hiện chuyển động!";     
      bot.sendMessage(CHAT_ID, message, "");
      
      notificationSent = true;
      lastMotionTime = currentTime;
      
      Serial.println("Đã gửi thông báo qua Telegram");
    } 
  } 
  else if (pirState == LOW && lastPirState == HIGH) {
    Serial.println("Chuyển động kết thúc");
  }
  
  // Cập nhật trạng thái cảm biến
  lastPirState = pirState;
  
  // Kiểm tra các tin nhắn đến từ Telegram
  if (millis() > lastTimeBotRan + BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    
    lastTimeBotRan = millis();
  }
  
  delay(100); // Tạm nghỉ để tránh đọc quá nhanh
}

// Xử lý tin nhắn đến từ Telegram
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    
    if (text == "/start") {
      String welcome = "Chào mừng, " + from_name + ".\n";
      welcome += "Sử dụng các lệnh sau để điều khiển hệ thống:\n\n";
      welcome += "/status - Kiểm tra trạng thái hiện tại\n";
      welcome += "/reset - Khởi động lại hệ thống\n";
      
      bot.sendMessage(chat_id, welcome, "");
    }
    
    if (text == "/status") {
      String status = "Trạng thái hệ thống:\n";
      status += "- Cảm biến PIR: " + String(pirState == HIGH ? "Đang phát hiện chuyển động" : "Không có chuyển động");
      status += "\n- Thời gian hoạt động: " + String(millis() / 1000) + " giây";
      
      bot.sendMessage(chat_id, status, "");
    }
    
    if (text == "/reset") {
      bot.sendMessage(chat_id, "Hệ thống sẽ khởi động lại trong 3 giây...", "");
      delay(3000);
      ESP.restart();
    }
  }
}