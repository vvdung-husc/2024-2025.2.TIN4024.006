#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#define BUTTON_PIN 21  // Chân nối nút nhấn
#define LED_PIN 21     // LED dùng chung chân với nút (theo sơ đồ bạn gửi)
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "b19a27b914cd43a48595da8b74d75d9d.s1.eu.hivemq.cloud"; // Hoặc IP local của broker

WiFiClient espClient;
PubSubClient client(espClient);
const int relayPin = 2;  // chân điều khiển thiết bị


void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Sử dụng điện trở kéo lên nội bộ
  pinMode(LED_PIN, OUTPUT);          // LED gắn cùng chân, cần bật/tắt qua digitalWrite
  digitalWrite(LED_PIN, LOW);        // Tắt LED lúc bắt đầu
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {          // Nút nhấn được nhấn (vì dùng INPUT_PULLUP)
    digitalWrite(LED_PIN, HIGH);     // Bật LED
  } else {
    digitalWrite(LED_PIN, LOW);      // Tắt LED
  }

  delay(50); // Tránh nhiễu nút nhấn (debounce đơn giản)
}
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String command;
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }

  if (command == "ON") {
    digitalWrite(relayPin, HIGH);
  } else if (command == "OFF") {
    digitalWrite(relayPin, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe("esp32/led");
    } else {
      delay(5000);
    }
  }
}

// void setup() {
//   pinMode(relayPin, OUTPUT);
//   digitalWrite(relayPin, LOW);
  
//   Serial.begin(115200);
//   setup_wifi();
//   client.setServer(mqtt_server, 1883);
//   client.setCallback(callback);
// }

// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();
// }
