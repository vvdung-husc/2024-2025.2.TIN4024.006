#include <WiFi.h>
#include <HTTPClient.h>
#include <Base64.h>  


//Thông tin Wi-Fi 
const char* ssid = "Wokwi-GUEST";
const char* password = "";

//Thông tin Telegram Bot
String BOT_TOKEN = "8027625067:AAHdREtzh1lGcXPLv2VNkIr8-Lo5ffGE3pI";  //Bot Token
String CHAT_ID = "-1002439452600";      //Chat ID

//Thông tin Gmail (SMTP Server)
const char* smtpServer = "smtp.gmail.com";
const int smtpPort = 443; 
const char* emailSender = "nhanhuynh1203@gmail.com";   // Email gửi
const char* emailPassword = "Banh10112003.";      // Mật khẩu ứng dụng
const char* emailReceiver = "banhbodoi1203@gmail.com"; // Email nhận

#define PIR_PIN 14  

void sendTelegramMessage(String message) {
  String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + message;
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();
  http.end();
}

void sendEmailAlert() {
  WiFiClientSecure client;
  client.setInsecure(); 

  if (!client.connect(smtpServer, smtpPort)) {
    Serial.println("❌ Kết nối SMTP thất bại!");
    return;
  }

  // Gửi lệnh SMTP
  client.println("EHLO smtp.gmail.com");
  client.println("AUTH LOGIN");
  client.println(base64::encode(emailSender));
  client.println(base64::encode(emailPassword));
  client.println("MAIL FROM:<" + String(emailSender) + ">");
  client.println("RCPT TO:<" + String(emailReceiver) + ">");
  client.println("DATA");
  client.println("Subject: 🚨 Cảnh báo chuyển động!");
  client.println("Phát hiện có người di chuyển!");
  client.println(".");
  client.println("QUIT");

  Serial.println("✅ Email đã gửi!");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);

  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("🔄 Đang kết nối Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\n✅ Đã kết nối Wi-Fi!");
}

void loop() {
  int motion = digitalRead(PIR_PIN);
  if (motion == HIGH) {
    Serial.println("🚨 Phát hiện chuyển động! Gửi cảnh báo...");
    sendTelegramMessage("🚨 Cảnh báo: Phát hiện chuyển động!");
    sendEmailAlert();
    delay(10000);
  }
  delay(500);
}
