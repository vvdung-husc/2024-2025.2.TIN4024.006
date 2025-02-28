#include <Arduino.h>
#include <TM1637Display.h>

// Pin đèn LED
// Đèn giao thông
#define rLED 27 // Đèn đỏ
#define yLED 26 // Đèn vàng
#define gLED 25 // Đèn xanh

// Đèn trời tối
#define blueLED 21

// Pin - TM1637
#define CLK 18
#define DIO 19

// Nút nhấn để bật/tắt hiển thị số
#define BUTTON_PIN 23

// Cảm biến quang (LDR) để nhận biết trời tối
#define LDR_PIN 13

// Khai báo thời gian của mỗi tín hiệu đèn (ms)
#define rTIME 5000
#define yTIME 2000
#define gTIME 5000

TM1637Display display(CLK, DIO);

// Biến thời gian và trạng thái
ulong currentMiliseconds = 0; // Thời gian hiện tại
ulong ledTimeStart = 0;       // Thời điểm đèn đổi màu
int currentLED = rLED;        // Đèn hiện tại đang sáng
bool isDisplayOn = true;      // Trạng thái hiển thị số
int counter = rTIME / 1000;   // Thời gian hiển thị đếm ngược (s)
ulong lastButtonPress = 0;    // Lưu thời gian nhấn nút gần nhất

void toggleDisplay();                               // Bật/tắt màn hình số
bool IsReady(ulong &ulTimer, uint32_t milisecond); // Kiểm tra xem đã hết thời gian đèn chưa
void NonBlocking_Traffic_Light();                  // Điều khiển đèn giao thông không dùng delay
void Testing_Display();                            // Hiển thị thời gian đếm ngược trên TM1637
bool isNightMode = false;                          // Lưu trạng thái hiện tại (mặc định là ban ngày)
void yLED_Blink();

void setup()
{
    Serial.begin(115200);
    pinMode(rLED, OUTPUT);
    pinMode(yLED, OUTPUT);
    pinMode(gLED, OUTPUT);
    pinMode(blueLED, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);

    display.setBrightness(7);
    digitalWrite(yLED, LOW);
    digitalWrite(gLED, LOW);
    digitalWrite(rLED, HIGH);
    currentLED = rLED;

    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), toggleDisplay, FALLING);
    Serial.println("<== START ==>");
    Serial.print("RED light \t : ");
    Serial.print(counter);
    Serial.println("s");
}

void toggleDisplay()
{
    ulong currentTime = millis();
    if (currentTime - lastButtonPress > 300) // Chống nhiễu (debounce)
    {
        isDisplayOn = !isDisplayOn;
        lastButtonPress = currentTime;
    }
}

void loop()
{
    int lightLevel = analogRead(LDR_PIN);
    bool newMode = (lightLevel < 1000); // Xác định chế độ hiện tại <1000 LUX đêm else Ngày

    // Kiểm tra nếu chế độ thay đổi
    if (newMode != isNightMode)
    {
        isNightMode = newMode; // Cập nhật trạng thái mới
        if (isNightMode)
        {
            Serial.println("--------------------------------------------------------");
            Serial.println("🌙 NIGHT MODE 🌙");
        }
        else
        {
            Serial.println("--------------------------------------------------------");
            Serial.println("☀️ DAY MODE ☀️");
            Serial.println("--------------------------------------------------------");
        }
    }

    if (isNightMode)
    {
        digitalWrite(blueLED, HIGH);
        digitalWrite(rLED, LOW);
        digitalWrite(gLED, LOW);
        display.clear(); // Tắt màn hình hiển thị
        yLED_Blink();
    }
    else
    {
        digitalWrite(blueLED, LOW);
        digitalWrite(yLED, LOW);

        currentMiliseconds = millis();
        NonBlocking_Traffic_Light();
        Testing_Display();
    }
}

void Testing_Display()
{
    if (isDisplayOn)
    {
        display.showNumberDec(counter, true, 3, 1);
    }
    else
    {
        display.clear();
    }
    if (counter > 0)
    {
        counter--;
    }
    delay(1000);
}

bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
    if (currentMiliseconds - ulTimer < milisecond)
        return false;
    ulTimer = currentMiliseconds;
    return true;
}

void NonBlocking_Traffic_Light()
{
    switch (currentLED)
    {
    case rLED: // Đèn đỏ: 5 giây
        if (IsReady(ledTimeStart, rTIME))
        {
            digitalWrite(rLED, LOW);
            digitalWrite(gLED, HIGH);
            currentLED = gLED;
            counter = gTIME / 1000;

            // Luôn hiển thị trên Serial Monitor
            Serial.println("Green light\t: 5s");
        }
        break;

    case gLED: // Đèn xanh: 5 giây
        if (IsReady(ledTimeStart, gTIME))
        {
            digitalWrite(gLED, LOW);
            digitalWrite(yLED, HIGH);
            currentLED = yLED;
            counter = yTIME / 1000;

            Serial.println("Yellow light\t: 2s");
        }
        break;

    case yLED: // Đèn vàng: 2 giây
        if (IsReady(ledTimeStart, yTIME))
        {
            digitalWrite(yLED, LOW);
            digitalWrite(rLED, HIGH);
            currentLED = rLED;
            counter = rTIME / 1000;

            Serial.println("Red light\t: 5s");
        }
        break;
    }
}


void yLED_Blink()
{
    static ulong lastBlinkTime = 0; // Lưu thời gian lần nhấp nháy trước
    static bool isOn = false;       // Trạng thái đèn vàng

    if (millis() - lastBlinkTime >= 100) // Mỗi 100ms đổi trạng thái đèn vàng
    {
        isOn = !isOn;
        digitalWrite(yLED, isOn ? HIGH : LOW);
        lastBlinkTime = millis(); // Cập nhật thời gian nhấp nháy
    }
}
