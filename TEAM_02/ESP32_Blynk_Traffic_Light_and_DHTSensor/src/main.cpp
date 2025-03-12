#include <Arduino.h>
#include <TM1637Display.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>
#define BLYNK_TEMPLATE_ID "TMPL6lD14z4JL"
#define BLYNK_TEMPLATE_NAME "ESP 32 Led controll"
#define BLYNK_AUTH_TOKEN "SsfKH68GnPaMpwFo7SuSHWogtbX80vw3"
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Định nghĩa chân kết nối
const int redPin = 27;    // Chân đèn đỏ
const int yellowPin = 26; // Chân đèn vàng
const int greenPin = 25;  // Chân đèn xanh lá
const int bluePin = 21;   // Chân đèn xanh dương (LED chỉ thị)
const int buttonPin = 23; // Chân nút nhấn
const int CLK = 18;       // Chân CLK của màn hình LED
const int DIO = 19;       // Chân DIO của màn hình LED
const int ldrPin = 33;    // Chân cảm biến ánh sáng LDR
#define DHTPIN 16
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
// Hằng số tính toán cho LDR
const float GAMMA = 0.7;
const float RL10 = 50;

// Biến trạng thái
bool nightMode = false;              // Chế độ ban đêm
unsigned long lastButtonPress = 0;   // Thời điểm cuối nhấn nút
unsigned long previousMillis = 0;    // Thời điểm trước đó cho đếm ngược
unsigned long lastLdrReadMillis = 0; // Thời điểm cuối đọc cảm biến LDR
long interval = 1000;                // Khoảng thời gian cập nhật đếm ngược (1 giây)
int countdown = 0;                   // Thời gian đếm ngược
int state = 0;                       // Trạng thái đèn giao thông (0: xanh, 1: vàng, 2: đỏ)
bool displayState = true;            // Trạng thái hiển thị màn hình (bật/tắt)
BlynkTimer timer;

// Khởi tạo đối tượng màn hình
TM1637Display display(CLK, DIO);

// Khai báo hàm
void khoiTaoChanPin();
void khoiTaoHienThi();
void khoiTaoDenGiaoThong();
void thayDoiDen();
void kiemTraNutNhan();
float tinhLux(int giaTriADC);
void docCamBienAnh();
void capNhatDenGiaoThong();
void sendSensor();
void updateUptime();
void capNhatTrangThaiHienThi(bool trangThai);

// Hàm xử lý khi có sự kiện từ Blynk (V1)
BLYNK_WRITE(V1)
{
    bool newState = param.asInt();
    // Gọi hàm chung để cập nhật trạng thái
    capNhatTrangThaiHienThi(newState);
}

// Hàm dùng chung để cập nhật trạng thái hiển thị từ cả Blynk và nút nhấn
void capNhatTrangThaiHienThi(bool trangThai)
{
    displayState = trangThai;

    // Cập nhật LED chỉ thị - Bật đèn khi màn hình tắt
    digitalWrite(bluePin, !displayState ? HIGH : LOW);

    // Cập nhật hiển thị màn hình
    if (!displayState || nightMode)
    {
        display.clear(); // Tắt màn hình nếu hiển thị tắt hoặc ở chế độ ban đêm
    }
    else
    {
        display.showNumberDec(countdown, true); // Hiển thị lại số đếm
    }

    // Đồng bộ trạng thái lên Blynk
    Blynk.virtualWrite(V1, displayState);

    // In trạng thái ra Serial
    Serial.print("Trạng thái hiển thị: ");
    Serial.println(displayState ? "BẬT" : "TẮT");
}

void setup()
{
    khoiTaoChanPin();
    khoiTaoHienThi();
    khoiTaoDenGiaoThong();

    WiFi.begin(ssid, pass);
    Serial.print("Đang kết nối WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Đã kết nối, địa chỉ IP: ");
    Serial.println(WiFi.localIP());

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    Serial.println("Kết nối Blynk thành công");

    // Đồng bộ trạng thái ban đầu lên Blynk
    Blynk.virtualWrite(V1, displayState);

    // Cài đặt timer
    timer.setInterval(2000L, sendSensor);   // Cập nhật nhiệt độ & độ ẩm mỗi 2 giây
    timer.setInterval(1000L, updateUptime); // Cập nhật thời gian hoạt động mỗi giây
}

// Khởi tạo cấu hình các chân pin
void khoiTaoChanPin()
{
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ldrPin, INPUT);
}

// Khởi tạo màn hình và cổng Serial
void khoiTaoHienThi()
{
    Serial.begin(9600);
    display.setBrightness(7); // Đặt độ sáng màn hình LED
}

// Khởi tạo trạng thái ban đầu đèn giao thông
void khoiTaoDenGiaoThong()
{
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, HIGH); // Bắt đầu với đèn xanh
    Serial.println("Đèn xanh (5 giây)");
    countdown = 5;
    state = 0;
}

// Thay đổi trạng thái đèn giao thông
void thayDoiDen()
{
    if (nightMode)
        return; // Nếu đang ở chế độ ban đêm thì không đổi đèn

    // Tắt tất cả đèn trước khi chuyển trạng thái
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);

    if (state == 0)
    { // Chuyển sang đèn vàng
        digitalWrite(yellowPin, HIGH);
        Serial.println("Đèn vàng (2 giây)");
        countdown = 2;
        state = 1;
    }
    else if (state == 1)
    { // Chuyển sang đèn đỏ
        digitalWrite(redPin, HIGH);
        Serial.println("Đèn đỏ (5 giây)");
        countdown = 5;
        state = 2;
    }
    else
    { // Chuyển sang đèn xanh
        digitalWrite(greenPin, HIGH);
        Serial.println("Đèn xanh (5 giây)");
        countdown = 5;
        state = 0;
    }
}

// Kiểm tra nút nhấn với chống dội
void kiemTraNutNhan()
{
    if (digitalRead(buttonPin) == LOW)
    { // Khi nút được nhấn
        unsigned long currentMillis = millis();
        if (currentMillis - lastButtonPress > 200)
        { // Chống dội phím
            lastButtonPress = currentMillis;
            // Đảo trạng thái hiển thị màn hình
            capNhatTrangThaiHienThi(!displayState);
        }
    }
}

// Tính giá trị Lux từ giá trị ADC cảm biến LDR
float tinhLux(int giaTriADC)
{
    if (giaTriADC == 0)
        return 0; // Tránh lỗi chia cho 0

    // Chuyển đổi giá trị ADC thành điện áp (ESP32 có ADC 12-bit: 0 - 4095)
    float voltage = giaTriADC / 4095.0 * 5.0;

    // Tính toán giá trị điện trở của LDR
    float resistance = 2000 * voltage / (1 - voltage / 5);

    // Tính toán lux dựa trên công thức tham khảo
    float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

    return lux;
}

// Đọc và xử lý giá trị từ cảm biến ánh sáng
void docCamBienAnh()
{
    unsigned long currentMillis = millis();

    // Đọc giá trị cảm biến 2 giây một lần
    if (currentMillis - lastLdrReadMillis >= 2000)
    {
        lastLdrReadMillis = currentMillis;

        int ldrValue = analogRead(ldrPin); // Đọc giá trị từ LDR
        float lux = tinhLux(ldrValue);     // Chuyển đổi sang lux

        // In giá trị lux ra Serial để kiểm tra
        Serial.print("Lux: ");
        Serial.println(lux);

        // Xử lý chế độ ban đêm
        if (lux < 50)
        { // Trời tối → Chỉ đèn vàng sáng, tắt màn hình
            if (!nightMode)
            { // Nếu mới chuyển sang chế độ ban đêm
                digitalWrite(redPin, LOW);
                digitalWrite(greenPin, LOW);
                digitalWrite(yellowPin, HIGH);
                display.clear(); // Tắt màn hình số
                Serial.println("Chuyển sang chế độ ban đêm - Chỉ đèn vàng sáng");
                nightMode = true; // Bật chế độ ban đêm
            }
        }
        else
        {
            if (nightMode)
            { // Nếu chuyển từ chế độ tối sang sáng, reset lại đèn xanh
                digitalWrite(yellowPin, LOW);
                digitalWrite(greenPin, HIGH);
                Serial.println("Chuyển sang chế độ ban ngày - Đèn xanh sáng (5 giây)");
                countdown = 5;
                state = 0;
                nightMode = false; // Tắt chế độ ban đêm

                // Hiển thị lại màn hình nếu màn hình đang ở trạng thái bật
                if (displayState)
                {
                    display.showNumberDec(countdown, true);
                }
            }
        }
    }
}

// Cập nhật trạng thái đèn giao thông
void capNhatDenGiaoThong()
{
    // Nếu ở chế độ ban đêm, luôn duy trì đèn vàng sáng
    if (nightMode)
    {
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(yellowPin, HIGH);
        return;
    }

    unsigned long currentMillis = millis();

    // Cập nhật đếm ngược mỗi giây
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;

        if (countdown < 0)
        {
            thayDoiDen(); // Đổi đèn khi hết thời gian đếm ngược
        }

        Serial.print("Còn ");
        Serial.print(countdown);
        Serial.println(" giây");

        // Cập nhật hiển thị nếu màn hình đang ở trạng thái bật và không ở chế độ ban đêm
        if (displayState && !nightMode)
        {
            display.showNumberDec(countdown, true);
        }

        countdown--; // Giảm bộ đếm
    }
}

// Gửi dữ liệu từ cảm biến DHT22 lên Blynk
void sendSensor()
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t))
    {
        Serial.println("Lỗi đọc cảm biến DHT22!");
        return;
    }

    Serial.print("Nhiệt độ: ");
    Serial.print(t);
    Serial.println("°C");
    Serial.print("Độ ẩm: ");
    Serial.print(h);
    Serial.println("%");

    // Gửi dữ liệu lên Blynk (luôn gửi bất kể trạng thái hiển thị)
    Blynk.virtualWrite(V3, t);
    Blynk.virtualWrite(V2, h);
}

// Cập nhật thời gian hoạt động lên Blynk
void updateUptime()
{
    int uptime = millis() / 1000;

    // Gửi dữ liệu lên Blynk
    Blynk.virtualWrite(V0, uptime);
}

void loop()
{
    Blynk.run();           // Xử lý kết nối Blynk
    timer.run();           // Chạy các timer
    kiemTraNutNhan();      // Kiểm tra nút nhấn
    docCamBienAnh();       // Đọc và xử lý cảm biến ánh sáng
    capNhatDenGiaoThong(); // Cập nhật trạng thái đèn giao thông
}