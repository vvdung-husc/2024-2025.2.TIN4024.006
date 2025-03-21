// Blynk Template Information
#define BLYNK_TEMPLATE_ID "TMPL6RQGDLOQe" 
#define BLYNK_TEMPLATE_NAME "ESP8266" 
#define BLYNK_AUTH_TOKEN "HkHpsRJ8SZ-wbZcBynAWmAqATjJOvHQV" 

#define BLYNK_PRINT Serial 
#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h> 
#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 

// Blynk Credentials
char auth[] = "HkHpsRJ8SZ-wbZcBynAWmAqATjJOvHQV"; 
char ssid[] = "Thanh Nhan"; 
char pass[] = "66669999"; 

// OLED setup
#define SCREEN_WIDTH 128 // Chiều rộng màn hình OLED
#define SCREEN_HEIGHT 64 // Chiều cao màn hình OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Virtual Pins (Chân ảo của Blynk)
#define UPTIME_VPIN V0 // Gửi thời gian hoạt động lên Blynk
#define TEMP_VPIN V1 // Gửi nhiệt độ lên Blynk
#define HUMI_VPIN V2 // Gửi độ ẩm lên Blynk
#define SWITCH_VPIN V3 // Nhận trạng thái nút bấm từ Blynk

// LED Pins
#define RED_LED D5 // LED đỏ
#define YELLOW_LED D6 // LED vàng
#define GREEN_LED D7 // LED xanh

bool blinkMode = false; // Biến trạng thái cho chế độ nhấp nháy

void setup() {
    Serial.begin(115200); // Khởi động Serial
    Blynk.begin(auth, ssid, pass); // Kết nối Blynk
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Kiểm tra màn hình OLED
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.clearDisplay(); // Xóa màn hình OLED
}

void loop() {
    Blynk.run(); // Chạy Blynk
    float temperature = random(-400, 800) / 10.0; // Tạo giá trị nhiệt độ ngẫu nhiên (-40 đến 80 độ C)
    float humidity = random(0, 1000) / 10.0; // Tạo giá trị độ ẩm ngẫu nhiên (0 - 100%)
    long uptime = millis() / 1000; // Lấy thời gian hoạt động của thiết bị
    
    // Gửi dữ liệu lên Blynk
    Blynk.virtualWrite(UPTIME_VPIN, uptime);
    Blynk.virtualWrite(TEMP_VPIN, temperature);
    Blynk.virtualWrite(HUMI_VPIN, humidity);
    
    // Hiển thị lên màn hình OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");
    display.setCursor(0, 30);
    display.print("Humi: ");
    display.print(humidity);
    display.println(" %");
    display.display();

    delay(1000);
}

// Nhận dữ liệu từ Blynk để điều khiển chế độ nhấp nháy LED
BLYNK_WRITE(SWITCH_VPIN) {
    blinkMode = param.asInt(); // Đọc giá trị từ ứng dụng Blynk
}

// Hàm điều khiển hệ thống đèn giao thông
void trafficLightControl() {
    if (blinkMode) { // Nếu chế độ nhấp nháy được bật
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(YELLOW_LED, millis() % 1000 < 500 ? HIGH : LOW); // Nhấp nháy đèn vàng
    } else { // Nếu chế độ giao thông bình thường
        digitalWrite(RED_LED, HIGH);
        delay(3000);
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        delay(5000);
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(YELLOW_LED, HIGH);
        delay(2000);
        digitalWrite(YELLOW_LED, LOW);
    }
}