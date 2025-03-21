#define BLYNK_TEMPLATE_ID "TMPL6mNbHq6qS"
#define BLYNK_TEMPLATE_NAME "test2"
#define BLYNK_AUTH_TOKEN "A00yvSMSLQ-F57w6CIdS9ywDC9RHlTPg"

#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>



char ssid[] = "Wokwi-GUEST";  //Tên mạng WiFi
char pass[] = "";             //Mật khẩu mạng WiFi

#define DHTPIN 16
#define DHTTYPE DHT22
#define LED_PIN 21
#define BUTTON_PIN 23
#define CLK 18
#define DIO 19

DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);

ulong currentMiliseconds = 0;
bool blueButtonON = true;

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void updateBlueButton();
void uptimeBlynk();
void sendSensorData();

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    display.setBrightness(0x0f);
    dht.begin();
    
    Serial.print("Connecting to "); Serial.println(ssid);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    Serial.println("WiFi connected");
    
    digitalWrite(LED_PIN, blueButtonON ? HIGH : LOW);
    Blynk.virtualWrite(V1, blueButtonON);
    Serial.println("== START ==> ");
}

void loop() {
    Blynk.run();
    currentMiliseconds = millis();
    uptimeBlynk();
    updateBlueButton();
    sendSensorData();
}

bool IsReady(ulong &ulTimer, uint32_t milisecond) {
    if (currentMiliseconds - ulTimer < milisecond) return false;
    ulTimer = currentMiliseconds;
    return true;
}

void updateBlueButton() {
    static ulong lastTime = 0;
    static int lastValue = HIGH;
    if (!IsReady(lastTime, 50)) return;
    int v = digitalRead(BUTTON_PIN);
    if (v == lastValue) return;
    lastValue = v;
    if (v == LOW) return;
    
    if (!blueButtonON) {
        Serial.println("Blue Light ON");
        digitalWrite(LED_PIN, HIGH);
        blueButtonON = true;
        Blynk.virtualWrite(V1, blueButtonON);
    } else {
        Serial.println("Blue Light OFF");
        digitalWrite(LED_PIN, LOW);
        blueButtonON = false;
        Blynk.virtualWrite(V1, blueButtonON);
        display.clear();
    }
}

void uptimeBlynk() {
    static ulong lastTime = 0;
    if (!IsReady(lastTime, 1000)) return;
    ulong value = lastTime / 1000;
    Blynk.virtualWrite(V0, value);
    if (blueButtonON) {
        display.showNumberDec(value);
    }
}

void sendSensorData() {
    static ulong lastTime = 0;
    if (!IsReady(lastTime, 2000)) return;
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    Blynk.virtualWrite(V2, temp);
    Blynk.virtualWrite(V3, hum);
}

BLYNK_WRITE(V1) {
    blueButtonON = param.asInt();
    if (blueButtonON) {
        Serial.println("Blynk -> Blue Light ON");
        digitalWrite(LED_PIN, HIGH);
    } else {
        Serial.println("Blynk -> Blue Light OFF");
        digitalWrite(LED_PIN, LOW);
        display.clear();
    }
}