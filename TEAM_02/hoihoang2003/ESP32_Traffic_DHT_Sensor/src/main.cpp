#define BLYNK_TEMPLATE_ID "TMPL6Pqb5LTxn"
#define BLYNK_TEMPLATE_NAME "ThoiTiet"
#define BLYNK_AUTH_TOKEN "EJ09Np-So_h3eu_2VdX1qKgbt_vxJOia"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TM1637Display.h>

//Thông tin WiFi
const char ssid[] = "Wokwi-GUEST";
const char pass[] = "";

//Chân kết nối phần cứng
#define DHT_PIN 16
#define BUTTON_PIN 23
#define BLUE_LED_PIN 21
#define CLK_PIN 18
#define DIO_PIN 19

//Thời gian lấy mẫu
#define SENSOR_INTERVAL 2000  // 2 giây cập nhật dữ liệu cảm biến

//Khởi tạo đối tượng phần cứng
DHT dht(DHT_PIN, DHT22);
TM1637Display display(CLK_PIN, DIO_PIN);
BlynkTimer timer;

//Địa chỉ máy chủ Blynk (IP của blynk.cloud, có thể cần cập nhật nếu thay đổi)
IPAddress blynkServerIP(128, 199, 144, 129);
uint16_t blynkPort = 8080; // Dùng cổng 8080 thay vì 80

// Biến toàn cục
uint32_t startTime;
bool displayOn = true;
bool lastButtonState = HIGH;
bool lastLedState = LOW;
uint32_t lastDisplayUpdate = 0;

//Biến cảm biến
float humidity = 0;
float temperature = 0;

//Trạng thái kết nối
bool blynkConnected = false;

//Biến để thử kết nối lại khi mất WiFi/Blynk
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // 5 giây thử lại

//Hàm gửi dữ liệu cảm biến lên Blynk
void sendSensorData() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Mất kết nối WiFi, không thể gửi dữ liệu!");
        return;
    }

    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (!isnan(humidity) && !isnan(temperature)) {
        Serial.printf("Nhiệt độ: %.1f °C, Độ ẩm: %.1f%%"\n, temperature, humidity);

        if (Blynk.connected()) {
            Blynk.virtualWrite(V5, temperature);  // Gửi nhiệt độ lên Blynk
            Blynk.virtualWrite(V6, humidity);    // Gửi độ ẩm lên Blynk
            Blynk.virtualWrite(V0, (millis() - startTime) / 1000); // Gửi thời gian hoạt động
        } else {
            Serial.println("Blynk chưa kết nối, không thể gửi dữ liệu!");
        }
    } else {
        Serial.println("Lỗi đọc dữ liệu từ DHT22!");
    }
}

//Xử lý nút nhấn từ Blynk
BLYNK_WRITE(V1) {
    lastLedState = param.asInt();
    digitalWrite(BLUE_LED_PIN, lastLedState);
    displayOn = !lastLedState;
    if (!displayOn) {
        display.clear();
    }
}

//Khi Blynk kết nối lại
BLYNK_CONNECTED() {
    blynkConnected = true;
    Blynk.virtualWrite(V1, lastLedState);
    Serial.println("Đã kết nối Blynk thành công!");
}

//Kết nối WiFi không làm treo chương trình
void connectWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Đang kết nối lại WiFi...");
        WiFi.disconnect();
        WiFi.begin(ssid, pass);
    }
}

//Kết nối lại Blynk (dùng IP trực tiếp để nhanh hơn)
void connectBlynk() {
    unsigned long currentMillis = millis();
    
    if (!Blynk.connected() && (currentMillis - lastReconnectAttempt > reconnectInterval)) {
        lastReconnectAttempt = currentMillis;
        
        Serial.println("Đang kết nối lại Blynk...");
        
        if (Blynk.connect(1000)) {
            Serial.println("Đã kết nối lại Blynk thành công!");
            blynkConnected = true;
        } else {
            Serial.println("Kết nối Blynk thất bại, sẽ thử lại sau!");
            blynkConnected = false;
        }
    }
}

//Hiển thị thời gian hoạt động lên màn hình LED 7 đoạn
void updateDisplay() {
    if (displayOn && millis() - lastDisplayUpdate > 1000) {
        lastDisplayUpdate = millis();
        uint32_t uptime = (millis() - startTime) / 1000; // Thời gian hoạt động tính bằng giây
        display.showNumberDecEx(uptime, 0x80, true);
    }
}

void loop() {
    Blynk.run();   // Chạy Blynk
    timer.run();   // Chạy timer
    connectWiFi(); // Kiểm tra & kết nối lại WiFi nếu mất kết nối
    connectBlynk(); // Kiểm tra & kết nối lại Blynk nếu mất kết nối

    // Xử lý nút nhấn vật lý
    bool currentButtonState = digitalRead(BUTTON_PIN);
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        lastLedState = !lastLedState;
        digitalWrite(BLUE_LED_PIN, lastLedState);
        Blynk.virtualWrite(V1, lastLedState);
    }
    lastButtonState = currentButtonState;

    // Hiển thị thời gian hoạt động lên màn hình LED 7 đoạn
    updateDisplay();
}

void setup() {
    Serial.begin(115200);
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BLUE_LED_PIN, OUTPUT);
    digitalWrite(BLUE_LED_PIN, LOW);

    dht.begin();
    display.setBrightness(5);
    display.clear();

    startTime = millis();

    Serial.println("Đang kết nối WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    uint32_t startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nĐã kết nối WiFi!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        //Kết nối nhanh với Blynk qua IP
        Blynk.config(BLYNK_AUTH_TOKEN, blynkServerIP, blynkPort);

        if (Blynk.connect()) {
            Serial.println("Đã kết nối Blynk thành công!");
        } else {
            Serial.println("Lỗi kết nối Blynk!");
        }
    } else {
        Serial.println("\nKhông thể kết nối WiFi!");
    }

    //Thiết lập timer gửi dữ liệu cảm biến
    timer.setInterval(SENSOR_INTERVAL, sendSensorData);
    
    Serial.println("Hệ thống đã sẵn sàng!");
}
