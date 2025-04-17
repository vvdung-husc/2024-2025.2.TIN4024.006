#define BLYNK_TEMPLATE_ID "TMPL644gL5d-g"
#define BLYNK_TEMPLATE_NAME "MQTT BLYNK"
#define BLYNK_AUTH_TOKEN "gZeUNnNrPRLCosk_shI__oR1UtpHpuuz"

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>

#define MQ2_PIN 34 
#define LED_PIN 22   
#define BUZZER_PIN 18
#define DHT_PIN 21 
#define DHT_TYPE DHT22 
unsigned long lastAlertTime = 0; 
const unsigned long alertInterval = 2000;

const char* ssid = "Wokwi-GUEST";
const char* password = "";


const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/fire_alarm";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHT dht(DHT_PIN, DHT_TYPE);

void reconnect() {
    while (!mqttClient.connected()) {
        Serial.print("Đang kết nối MQTT...");
        if (mqttClient.connect("ESP32_Client")) {
            Serial.println("Thành công!");
        } else {
            Serial.print("Thất bại, mã lỗi: ");
            Serial.print(mqttClient.state());
            Serial.println(" Thử lại sau 5 giây...");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    dht.begin();

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi đã kết nối!");

    mqttClient.setServer(mqtt_server, mqtt_port);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

    delay(2000);
}

void loop() {
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();
    Blynk.run();

    int sensorValue = analogRead(MQ2_PIN);
    float ppm = (sensorValue / 4095.0) * 5000;
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    Serial.print("ADC Raw: ");
    Serial.print(sensorValue);
    Serial.print(" | Nồng độ khí: ");
    Serial.print(ppm);
    Serial.println(" ppm");
    Serial.print("Nhiệt độ: ");
    Serial.print(temperature);
    Serial.println("°C");
    Serial.print("Độ ẩm: ");
    Serial.print(humidity);
    Serial.println("%");

    Blynk.virtualWrite(V0, ppm);
    Blynk.virtualWrite(V2, temperature);
    Blynk.virtualWrite(V3, humidity);

    if (ppm > 2000 || temperature > 50) {
        Serial.println("CẢNH BÁO CHÁY!!! Đèn đỏ bật + Buzzer kêu!");
        digitalWrite(LED_PIN, HIGH);
        tone(BUZZER_PIN, 1000);
        Blynk.virtualWrite(V1, 1);

        if (millis() - lastAlertTime > alertInterval) {
            Serial.println("Gửi tin nhắn MQTT: CẢNH BÁO CHÁY!");
            mqttClient.publish(mqtt_topic, "CANH BAO CHAY! Nong do Khi, Nhiet Do vuot qua nguong an toan");
            lastAlertTime = millis();
        }
    } else {
        digitalWrite(LED_PIN, LOW);
        noTone(BUZZER_PIN);
        Blynk.virtualWrite(V1, 0); 
        mqttClient.publish(mqtt_topic, "Nong do Khi, Nhiet Do an toan");
    }

    delay(2000);
}