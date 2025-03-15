#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>

// Định nghĩa các chân theo sơ đồ kết nối
#define RED_PIN 27    // Đèn đỏ (với điện trở 220 ohm)
#define YELLOW_PIN 26 // Đèn vàng (với điện trở 330 ohm)
#define GREEN_PIN 25  // Đèn xanh lá
#define BLUE_PIN 21   // Đèn xanh dương
#define BUTTON_PIN 23 // Nút nhấn
#define LDR_PIN 32    // Cảm biến ánh sáng
#define CLK_PIN 18    // Chân CLK của TM1637
#define DIO_PIN 19    // Chân DIO của TM1637
#define DHT_PIN 16    // Chân DHT22

// Nguyễn Hoàng Anh Tú
//  #define BLYNK_TEMPLATE_ID "TMPL6BZfffxJA"
//  #define BLYNK_TEMPLATE_NAME "BAITAPNHOM"
//  #define BLYNK_AUTH_TOKEN "qbKOyl_8WYeS6SZ0T1sn2zUKx1Z66KSx"

// Hoàng Hữu Hội
//  #define BLYNK_TEMPLATE_ID "TMPL6Pqb5LTxn"
//  #define BLYNK_TEMPLATE_NAME "BaiTapNhom"
//  #define BLYNK_AUTH_TOKEN "Zw0n552_0-26dD-BEiCzzJpQIS4y6DfS"

// Lê Đình Trung
//  #define BLYNK_TEMPLATE_ID "TMPL6g_ylSMD2"
//  #define BLYNK_TEMPLATE_NAME "ESP32T2"
//  #define BLYNK_AUTH_TOKEN "w_TGq_MGsbS0PuAFALp4UsTRipPdF-ee"

// Lê Đức Duy Anh
//  #define BLYNK_TEMPLATE_ID "TMPL6V7RZZSM7"
//  #define BLYNK_TEMPLATE_NAME "ESP32 BAITAPNHOM"
//  #define BLYNK_AUTH_TOKEN "9hmYK4FgSUgn_ApmyYaZ_nwjZXrvm2Oc"

// Đặng Trọng Trí Tuệ
// #define BLYNK_TEMPLATE_ID "TMPL6BvjrT3MS"
// #define BLYNK_TEMPLATE_NAME "blynk traffic"
// #define BLYNK_AUTH_TOKEN "sz6CGXBNgPinjCOS3b-xL6oigWRcJoiy"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Cấu hình DHT
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// Các hằng số khác
#define GAMMA 0.7
#define RL10 50
#define DEBOUNCE_TIME 200
#define NIGHT_LUX_THRESHOLD 50

// Biến toàn cục
bool isPaused = false;
unsigned long lastButtonPress = 0;
TM1637Display display(CLK_PIN, DIO_PIN);
unsigned long previousMillis = 0;
unsigned long dhtPreviousMillis = 0;
unsigned long startMillis = 0; // Thời gian bắt đầu hệ thống
long interval = 1000;
long dhtInterval = 2000; // Đọc DHT mỗi 2 giây
int countdown = 0;
int state = 0;
bool nightMode = false;
float temperature = 0;
float humidity = 0;
bool systemEnabled = true; // Trạng thái hệ thống (bật/tắt từ Blynk)
int luxThreshold = NIGHT_LUX_THRESHOLD;

void setup()
{
    // Cấu hình các chân
    pinMode(RED_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);

    // Khởi tạo Serial và các thiết bị
    Serial.begin(9600);
    display.setBrightness(7);
    dht.begin();

    // Kết nối WiFi và Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    startMillis = millis(); // Lưu thời gian bắt đầu hệ thống
    Blynk.virtualWrite(V1, 1);
    // Bắt đầu với đèn xanh
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);
    Serial.println("Đèn xanh (5 giây)");
    countdown = 5;
    state = 0;
}

void changeLight()
{
    if (nightMode || !systemEnabled)
        return; // Không đổi đèn nếu ở chế độ ban đêm hoặc hệ thống tắt

    digitalWrite(RED_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);

    if (state == 0)
    { // Đèn vàng
        digitalWrite(YELLOW_PIN, HIGH);
        Serial.println("Đèn vàng (2 giây)");
        countdown = 2;
        state = 1;
    }
    else if (state == 1)
    { // Đèn đỏ
        digitalWrite(RED_PIN, HIGH);
        Serial.println("Đèn đỏ (5 giây)");
        countdown = 5;
        state = 2;
    }
    else
    { // Đèn xanh
        digitalWrite(GREEN_PIN, HIGH);
        Serial.println("Đèn xanh (5 giây)");
        countdown = 5;
        state = 0;
    }
}

// Kiểm tra nút nhấn với debounce bằng millis()
void checkButtonPress()
{
    if (digitalRead(BUTTON_PIN) == LOW)
    { // Khi nút được nhấn
        unsigned long currentMillis = millis();
        if (currentMillis - lastButtonPress > DEBOUNCE_TIME)
        {                         // Chống dội phím
            isPaused = !isPaused; // Đảo trạng thái
            lastButtonPress = currentMillis;

            if (!isPaused)
            { // Khi bật BLUE_PIN và màn hình LED
                digitalWrite(BLUE_PIN, HIGH);
                display.showNumberDec(countdown, true); // Hiển thị số
                Blynk.virtualWrite(V1, 1);              // Gửi tín hiệu lên V1 (Bật)
                Serial.println("BẬT - V1 = 1");
            }
            else
            { // Khi tắt BLUE_PIN và màn hình LED
                digitalWrite(BLUE_PIN, LOW);
                display.clear();           // Tắt màn hình số
                Blynk.virtualWrite(V1, 0); // Gửi tín hiệu lên V1 (Tắt)
                Serial.println("TẮT - V1 = 0");
            }
        }
    }
}
BLYNK_WRITE(V1)
{
    int pinValue = param.asInt(); // Đọc trạng thái từ Switch trên Blynk

    if (pinValue == 1)
    { // Khi bật (ON)
        isPaused = false;
        digitalWrite(BLUE_PIN, HIGH);           // Bật đèn BLUE
        display.showNumberDec(countdown, true); // Hiển thị số
        Serial.println("Bật từ Blynk - V1 = 1");
    }
    else
    { // Khi tắt (OFF)
        isPaused = true;
        digitalWrite(BLUE_PIN, LOW); // Tắt đèn BLUE
        display.clear();             // Tắt màn hình LED
        Serial.println("Tắt từ Blynk - V1 = 0");
    }
}
BLYNK_WRITE(V4)
{
    luxThreshold = param.asInt(); // Cập nhật giá trị từ Blynk
    Serial.print("Ngưỡng Lux cập nhật: ");
    Serial.println(luxThreshold);
}

// Hàm đọc dữ liệu từ DHT22 và gửi lên Blynk
void readDHT()
{
    unsigned long currentMillis = millis();
    if (currentMillis - dhtPreviousMillis >= dhtInterval)
    {
        dhtPreviousMillis = currentMillis;

        humidity = dht.readHumidity();
        temperature = dht.readTemperature();

        if (!isnan(humidity) && !isnan(temperature))
        {
            Serial.print("Nhiệt độ: ");
            Serial.print(temperature);
            Serial.print("°C | Độ ẩm: ");
            Serial.print(humidity);
            Serial.println("%");

            // Gửi nhiệt độ và độ ẩm lên Blynk
            Blynk.virtualWrite(V2, temperature); // Nhiệt độ lên V2
            Blynk.virtualWrite(V3, humidity);    // Độ ẩm lên V3
        }
        else
        {
            Serial.println("Lỗi đọc dữ liệu từ DHT22!");
        }
    }
}

// Hàm tính Lux từ cảm biến LDR
float calculateLux(int adcValue)
{
    if (adcValue == 0)
        return 0; // Tránh lỗi chia cho 0

    // Chuyển đổi giá trị ADC thành điện áp (ESP32 có ADC 12-bit: 0 - 4095)
    float voltage = adcValue / 4095.0 * 5.0;

    // Tính toán giá trị điện trở của LDR
    float resistance = 2000 * voltage / (1 - voltage / 5);

    // Tính toán lux dựa trên công thức tham khảo
    float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

    return lux;
}

// Hàm gửi thời gian hoạt động lên Blynk (V0)
void updateUptime()
{
    unsigned long currentMillis = millis();
    unsigned long uptimeSeconds = (currentMillis - startMillis) / 1000; // Tính thời gian hoạt động (giây)
    Blynk.virtualWrite(V0, uptimeSeconds);                              // Gửi lên V0
}

// Hàm xử lý lệnh bật/tắt hệ thống từ Blynk (V1)

void loop()
{
    Blynk.run(); // Chạy Blynk để duy trì kết nối và nhận lệnh

    // Nếu hệ thống bị tắt từ Blynk, dừng toàn bộ chương trình
    if (!systemEnabled)
    {
        return; // Thoát khỏi loop(), không thực hiện bất kỳ tác vụ nào khác
    }

    // Nếu hệ thống được bật, tiếp tục thực hiện các tác vụ bình thường
    checkButtonPress(); // Kiểm tra nút nhấn non-blocking
    readDHT();          // Đọc dữ liệu từ DHT22 và gửi lên Blynk
    updateUptime();     // Gửi thời gian hoạt động lên Blynk

    // Xử lý chế độ ban đêm
    if (luxThreshold < NIGHT_LUX_THRESHOLD)
    {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(GREEN_PIN, LOW);
        digitalWrite(YELLOW_PIN, HIGH);
        digitalWrite(BLUE_PIN, LOW);
        display.clear();

        if (!nightMode)
        {
            Serial.println("Chuyển sang chế độ ban đêm - Chỉ đèn vàng sáng");
            nightMode = true;
        }
    }
    else
    {
        if (nightMode)
        {
            digitalWrite(YELLOW_PIN, LOW);
            if (systemEnabled)
            {
                digitalWrite(GREEN_PIN, HIGH);
                digitalWrite(BLUE_PIN, HIGH);
                Serial.println("Chuyển sang chế độ ban ngày - Đèn xanh sáng (5 giây)");
                countdown = 5;
                state = 0;
            }
            nightMode = false;
        }

        // Chỉ cập nhật đèn và đếm ngược khi ở chế độ ban ngày và hệ thống được bật
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;

            if (!nightMode && countdown < 0 && systemEnabled)
            {
                changeLight();
            }

            Serial.print("Còn ");
            Serial.print(countdown);
            Serial.println(" giây");

            if (!isPaused && !nightMode && systemEnabled)
            {
                display.showNumberDec(countdown, true);
            }

            countdown--;
        }
    }
}