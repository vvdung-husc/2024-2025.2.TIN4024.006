#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

#define BOT_TOKEN "7788820897:AAH80UXAHY5rQHtyvd_yHGrHmQeH6sERO7I"
#define CHAT_ID "7850672731"
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// GPIO
#define IR_IN_PIN 14   // Button 1
#define IR_OUT_PIN 27  // Button 2
#define LED_PIN 15

// Counter
int peopleCount = 0;
int enterCount = 0;
int exitCount = 0;

bool waitIR_OUT = false;
bool waitIR_IN = false;

unsigned long lastInTime = 0;
unsigned long lastOutTime = 0;
const unsigned long debounceDelay = 300;

void sendToTelegram(String msg) {
  bot.sendMessage(CHAT_ID, msg, "");
}

void updateDisplay(String reason) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("Vao: %d | Ra: %d", enterCount, exitCount);
  display.setTextSize(2);
  display.setCursor(0, 30);
  display.print("Trong phong:");
  display.setCursor(0, 50);
  display.print(peopleCount);
  display.display();

  String message = "📢 " + reason + "\n👥 Trong phong: " + String(peopleCount) +
                   "\n🚶‍♂️ Vao: " + String(enterCount) + " | 🚪 Ra: " + String(exitCount);
  sendToTelegram(message);
}

void blinkLED() {
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(IR_IN_PIN, INPUT_PULLUP);
  pinMode(IR_OUT_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (1);
  }

  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Khoi dong...");
  display.display();

  WiFi.begin(ssid, password);
  client.setInsecure();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  sendToTelegram("✅ ESP32 da khoi dong.");
  updateDisplay("Khoi dong");
}

void loop() {
  bool IR_IN = digitalRead(IR_IN_PIN) == LOW;
  bool IR_OUT = digitalRead(IR_OUT_PIN) == LOW;
  unsigned long now = millis();

  // Người vào: nhấn nút IR_IN rồi IR_OUT
  if (!waitIR_OUT && IR_IN) {
    waitIR_OUT = true;
    lastInTime = now;
    Serial.println(">>> Phat hien IR_IN, doi IR_OUT...");
  }

  if (waitIR_OUT && IR_OUT && now - lastInTime > debounceDelay) {
    enterCount++;
    peopleCount++;
    updateDisplay("Phat hien nguoi vao");
    blinkLED();
    waitIR_OUT = false;
    delay(300);
  }

  // Người ra: nhấn nút IR_OUT rồi IR_IN
  if (!waitIR_IN && IR_OUT) {
    waitIR_IN = true;
    lastOutTime = now;
    Serial.println("<<< Phat hien IR_OUT, doi IR_IN...");
  }

  if (waitIR_IN && IR_IN && now - lastOutTime > debounceDelay) {
    if (peopleCount > 0) {
      exitCount++;
      peopleCount--;
      updateDisplay("Phat hien nguoi ra");
      blinkLED();
    }
    waitIR_IN = false;
    delay(300);
  }

  // Reset nếu người bấm sai thứ tự
  if (waitIR_OUT && now - lastInTime > 3000) waitIR_OUT = false;
  if (waitIR_IN && now - lastOutTime > 3000) waitIR_IN = false;
}
