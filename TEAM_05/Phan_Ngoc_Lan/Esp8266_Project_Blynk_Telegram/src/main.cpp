#define BLYNK_TEMPLATE_ID "TMPL6t-OvCkox"
#define BLYNK_TEMPLATE_NAME "esp8266"
#define BLYNK_AUTH_TOKEN "FQdeeuC-JHJXCrvaczzf6V_qvH7IINKj"

#define BOTtoken "7850825358:AAFlNv4fV_JDX_dwKeFanHGv5QnZ5WOXOGY"
#define GROUP_ID "-4671481974"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// WiFi Credentials
const char *ssid = "CNTT-MMT";
const char *password = "13572468";

// Define Pins
#define LED_GREEN 15
#define LED_YELLOW 2
#define LED_RED 5
#define DHT_PIN 16
#define DHT_TYPE DHT11
#define OLED_SDA 13
#define OLED_SCL 12

// Initialize Objects
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
DHT dht(DHT_PIN, DHT_TYPE);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
BlynkTimer timer;

// Global Variables
float temperature = 0.0;
float humidity = 0.0;
bool yellowBlinkMode = false;
bool trafficEnabled = true;
unsigned long runTime = 0;

//  Hiển thị trên OLED
void updateOLED()
{
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese2);
  oled.drawUTF8(0, 14, ("Nhiet do: " + String(temperature, 1) + "°C").c_str());
  oled.drawUTF8(0, 28, ("Do am: " + String(humidity, 1) + "%").c_str());
  oled.drawUTF8(0, 42, ("Thoi gian: " + String(runTime) + "s").c_str());
  oled.sendBuffer();
}

// Điều khiển đèn giao thông
void TrafficLightControl()
{
  static unsigned long lastTimer = 0;
  static int state = 0;
  static const unsigned long durations[] = {2000, 3000, 1000};
  static const int ledPins[] = {LED_RED, LED_GREEN, LED_YELLOW};

  if (!trafficEnabled)
  {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);
    return;
  }

  if (yellowBlinkMode)
  {
    if (millis() - lastTimer > 500)
    {
      lastTimer = millis();
      digitalWrite(LED_YELLOW, !digitalRead(LED_YELLOW));
    }
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    return;
  }

  if (millis() - lastTimer > durations[state])
  {
    lastTimer = millis();
    digitalWrite(ledPins[state], LOW);
    state = (state + 1) % 3;
    digitalWrite(ledPins[state], HIGH);
  }
}

//  Cập nhật nhiệt độ & độ ẩm, Sinh dữ liệu nhiệt độ & độ ẩm ngẫu nhiên
void updateSensorData()
{
  static unsigned long lastTimer = 0;
  if (millis() - lastTimer < 2000)
    return;
  lastTimer = millis();

  temperature = random(-400, 800) / 10.0;
  humidity = random(0, 1000) / 10.0;
}

//  Gửi dữ liệu lên Blynk
void sendBlynkData()
{
  Blynk.virtualWrite(V0, runTime);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
}

//  Gửi dữ liệu lên Telegram
void sendTelegramAlert()
{
  static unsigned long lastTimer = 0;
  if (millis() - lastTimer < 60000)
    return;
  lastTimer = millis();

  String message = "";
  if (temperature < 10)
    message += "Nguy cơ hạ thân nhiệt!\n";
  else if (temperature > 35)
    message += "Nguy cơ sốc nhiệt!\n";
  if (humidity < 30)
    message += "Độ ẩm thấp, nguy cơ bệnh hô hấp!\n";
  else if (humidity > 70)
    message += "Độ ẩm cao, nguy cơ nấm mốc!\n";

  if (!message.isEmpty())
  {
    message = "Cảnh báo:\n" + message + "Nhiệt độ: " + String(temperature) + "°C\n" + "Độ ẩm: " + String(humidity) + "%";
    bot.sendMessage(GROUP_ID, message, "");
  }
}

void handleTelegramCommands()
{
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < numNewMessages; i++)
  {
    String text = bot.messages[i].text;
    if (text == "/traffic_off")
    {
      trafficEnabled = false;
      yellowBlinkMode = false;
      bot.sendMessage(GROUP_ID, "Đèn giao thông đã tắt", "");
    }
    else if (text == "/traffic_on")
    {
      trafficEnabled = true;
      bot.sendMessage(GROUP_ID, "Đèn giao thông đã bật", "");
    }
  }
}

BLYNK_WRITE(V3)
{
  yellowBlinkMode = param.asInt();
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, HIGH);

  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese1);
  oled.drawUTF8(0, 14, "Khoi dong...");
  oled.sendBuffer();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  client.setInsecure();

  timer.setInterval(2000L, sendBlynkData);
}

void loop()
{
  Blynk.run();
  timer.run();
  TrafficLightControl();
  updateSensorData();
  sendTelegramAlert();
  handleTelegramCommands();
  runTime++;
  updateOLED();
  delay(1000);
}
