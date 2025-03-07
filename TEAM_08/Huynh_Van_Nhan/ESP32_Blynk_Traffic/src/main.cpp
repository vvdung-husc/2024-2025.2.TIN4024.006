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

#define DHTPIN 16
#define DHTTYPE DHT22

#define GREEN_LED 25    
#define YELLOW_LED 26   
#define RED_LED 27      
#define BLUE_LED 21     

#define LDR_PIN 34

#define BUTTON_PIN 23  
#define CLK 18        
#define DIO 19        

DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);
BlynkTimer timer;

bool ledState = false; // Trạng thái của Blue LED (true = bật, false = tắt)

bool displayState = false; // Trạng thái của màn hình (true = bật, false = tắt)

unsigned long lastButtonPress = 0;
float temperature = 0;  
float humidity = 0;     
bool useBlynkData = false; 

int counter = 0;  
int lightState = 0;  // 0 = Xanh, 1 = Vàng, 2 = Đỏ
unsigned long lightTimer = 0;
int countdown = 5;  // Giá trị đếm ngược ban đầu
bool useBlynkLightData = false;  // Xác định có dùng dữ liệu từ Blynk không

int darkThreshold = 1000;  // Ngưỡng độ sáng mặc định
bool lowLightMode = false; // Chế độ ánh sáng thấp


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
void sendSensorData() {
    if (!useBlynkData) {
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        
        
    }

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Lỗi đọc cảm biến!");
        return;
    }

    
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V3, counter);
    
    counter++;  
}

// Nhận giá trị ngưỡng ánh sáng từ Blynk
BLYNK_WRITE(V6) {
    darkThreshold = param.asInt();
    useBlynkLightData = true;  // Đánh dấu rằng dữ liệu từ Blynk đang được sử dụng
    Serial.print("Cập nhật ngưỡng ánh sáng từ Blynk: ");
    Serial.println(darkThreshold);
}

// Nhận nhiệt độ từ Blynk
BLYNK_WRITE(V4) {
    temperature = param.asFloat();
    useBlynkData = true;
    // In giá trị nhiệt độ và độ ẩm ra Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    
}

// Nhận độ ẩm từ Blynk
BLYNK_WRITE(V5) {
    humidity = param.asFloat();
    useBlynkData = true;

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
}

// Kiểm tra nút nhấn và cập nhật trạng thái màn hình
void checkButton() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (millis() - lastButtonPress > 300) {  
            ledState = !ledState;
            digitalWrite(BLUE_LED, ledState);
            Blynk.virtualWrite(V2, ledState);
            
            displayState = !displayState; 

            if (!displayState) {
                display.clear();
                Serial.println("Off");  // In ra "Off" khi displayState là false
            } else {
                display.showNumberDec(countdown);
                Serial.println("On");   // In ra "On" khi displayState là true
            }

            lastButtonPress = millis();
        }
    }
}

// Điều khiển LED từ Blynk
BLYNK_WRITE(V2) {
    ledState = param.asInt();
    digitalWrite(BLUE_LED, ledState);

    displayState = ledState;

    if (!displayState) {
        display.clear(); 
        Serial.println("Off");  // In ra "Off" khi displayState là false
    } else {
        display.showNumberDec(countdown);
        Serial.println("On");   // In ra "On" khi displayState là true
    }
}

// Quản lý ánh sáng thấp

void checkLightLevel() {
    int lightLevel = analogRead(LDR_PIN);
    if (useBlynkLightData) {
        // Ưu tiên sử dụng ngưỡng từ Blynk để quyết định lowLightMode
        lowLightMode = (darkThreshold < 1000);
    } else {
        // Nếu chưa có dữ liệu từ Blynk, tự động lấy từ cảm biến
        darkThreshold = lightLevel;
        lowLightMode = (darkThreshold < 1000);
    }
}
// Quản lý đèn giao thông và đếm ngược
void updateTrafficLights() {
    checkLightLevel();

    if (lowLightMode) {
        // Chế độ ánh sáng yếu: Đèn vàng nhấp nháy, màn hình hiển thị "0000"
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, LOW);
        
        for (int i = 0; i < 5; i++) {  
            digitalWrite(YELLOW_LED, HIGH);
            delay(500);
            digitalWrite(YELLOW_LED, LOW);
            delay(500);
        }

        if (displayState) {
            display.showNumberDec(0000);
        }
    } else {
        // Chế độ giao thông bình thường
        if (millis() - lightTimer >= 1000) {
            lightTimer = millis();

            if (countdown > 0) {
                countdown--; 
            } else {
                if (lightState == 0) {  
                    lightState = 1;
                    countdown = 2;
                } else if (lightState == 1) {  
                    lightState = 2;
                    countdown = 3;
                } else {  
                    lightState = 0;
                    countdown = 5;
                }
            }
        }
        // Điều khiển LED theo trạng thái
        digitalWrite(GREEN_LED, lightState == 0);
        digitalWrite(YELLOW_LED, lightState == 1);
        digitalWrite(RED_LED, lightState == 2);

        // Cập nhật màn hình LED nếu đang bật
        if (displayState) {
            display.showNumberDec(countdown);
        }
    }
}

// Cấu hình hệ thống
void setup() {
    Serial.begin(115200);
    connectWiFi();
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

    pinMode(GREEN_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);

    display.clear();   
    Blynk.virtualWrite(V3, counter);  
    Blynk.virtualWrite(V6, darkThreshold);  

    dht.begin();
    display.setBrightness(7); 
    
    timer.setInterval(2000L, sendSensorData);
}

void loop() {
    Blynk.run();
    timer.run();
    checkButton();
    updateTrafficLights();
}
