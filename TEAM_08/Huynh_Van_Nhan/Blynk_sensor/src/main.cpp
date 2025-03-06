#define BLYNK_TEMPLATE_ID "TMPL6PqjG-L40"
#define BLYNK_TEMPLATE_NAME "Nhan"
#define BLYNK_AUTH_TOKEN "IOg3DFoDvjBPSbxdbu3OOYGlKXGeGv52"

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""

// Thư viện cần thiết
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TM1637Display.h>

#define DHTPIN 16       // Chân kết nối DHT22
#define DHTTYPE DHT22   // Loại cảm biến DHT22
#define LED_PIN 21      // Chân điều khiển LED
#define BUTTON_PIN 23   // Chân nút nhấn
#define CLK 18         // Chân CLK của TM1637
#define DIO 19         // Chân DIO của TM1637

DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);
BlynkTimer timer;

bool ledState = false;
unsigned long lastButtonPress = 0;
float temperature = 0;  // Biến lưu nhiệt độ
float humidity = 0;     // Biến lưu độ ẩm
bool useBlynkData = false; // Kiểm tra nếu Blynk gửi dữ liệu

// Kết nối WiFi
void connectWiFi() {
    Serial.print("Đang kết nối WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" Đã kết nối!");
}

// Gửi dữ liệu cảm biến lên Blynk
int counter = 0;  // Bắt đầu từ 0

void sendSensorData() {
    if (!useBlynkData) {
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
    }

    Serial.print("Nhiệt độ: "); Serial.println(temperature);
    Serial.print("Độ ẩm: "); Serial.println(humidity);

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Lỗi đọc cảm biến!");
        return;
    }

    // Gửi dữ liệu cảm biến hoặc từ Blynk lên ứng dụng
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V3, counter);
    
    display.showNumberDec(counter); // Hiển thị thời gian hoạt động
    
    counter++;  // Tăng thời gian hoạt động
}

// Nhận nhiệt độ từ Blynk
BLYNK_WRITE(V4) {
    temperature = param.asFloat();
    useBlynkData = true;
    Serial.print("Cập nhật nhiệt độ từ Blynk: ");
    Serial.println(temperature);
}

// Nhận độ ẩm từ Blynk
BLYNK_WRITE(V5) {
    humidity = param.asFloat();
    useBlynkData = true;
    Serial.print("Cập nhật độ ẩm từ Blynk: ");
    Serial.println(humidity);
}

// Kiểm tra nút nhấn và cập nhật LED
void checkButton() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (millis() - lastButtonPress > 300) {  // Chống rung
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
            Blynk.virtualWrite(V2, ledState);  // Cập nhật trạng thái LED lên Blynk
            Serial.println(ledState ? "LED ON" : "LED OFF");
            lastButtonPress = millis();
        }
    }
}

// Điều khiển LED từ Blynk App
BLYNK_WRITE(V2) {
    ledState = param.asInt();
    digitalWrite(LED_PIN, ledState);
}

// Cấu hình hệ thống
void setup() {
    Serial.begin(115200);
    connectWiFi();
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // RESET MÀN HÌNH DIGIT KHI KHỞI ĐỘNG
    display.clear();   // Xóa tất cả số trên màn hình
    counter = 0;       // Reset bộ đếm về 0
    Blynk.virtualWrite(V3, counter);  // Cập nhật lại trên Blynk

    dht.begin();
    display.setBrightness(7); // Độ sáng tối đa cho màn hình

    // Định kỳ gửi dữ liệu mỗi 2 giây
    timer.setInterval(2000L, sendSensorData);
}

void loop() {
    Blynk.run();
    timer.run();
    checkButton();
}
