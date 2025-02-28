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

// Nút nhấn để tạm dừng.
#define BUTTON_PIN 23
// Cảm biến quang (LDR) để nhận biết trời tối.
#define LDR_PIN 13

// Khai báo thời gian của mỗi tín hiệu đèn (ms)
#define rTIME 5000
#define yTIME 2000
#define gTIME 5000

TM1637Display display(CLK, DIO);
// biến thời gian và trạng thái
ulong currentMiliseconds = 0; // Thời gian hiện tại
ulong ledTimeStart = 0;       // Thời điểm đèn đổi màu
int currentLED = rLED;        // Đèn hiện tại đang sáng
bool isPaused = false;        // Trạng thái dừng
int counter = rTIME / 1000;   // Thời gian hiển thị đếm ngược (s)

void togglePause();                                // Tạm dừng hoặc tiếp tục hệ thống
bool IsReady(ulong &ulTimer, uint32_t milisecond); // Kiểm tra xem đã hết thời gian đèn chưa
void NonBlocking_Traffic_Light();                  // Điều khiển đèn giao thông không dùng delay
void Testing_Display();                            // Hiển thị thời gian đếm ngược trên  TM1637
bool isNightMode = false; // Lưu trạng thái hiện tại (mặc định là ban ngày)
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

    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), togglePause, FALLING);
    Serial.println("<== START ==>");
    Serial.print("RED light \t : ");
    Serial.print(counter);
    Serial.println("s");
}

void togglePause()
{
    isPaused = !isPaused;
    if (isPaused)
    {
        // Tắt tất cả đèn giao thông khi tạm dừng
        digitalWrite(rLED, LOW);
        digitalWrite(yLED, LOW);
        digitalWrite(gLED, LOW);
    }
    else
    {
        // Khi tiếp tục, bật lại đèn theo trạng thái hiện tại
        switch (currentLED)
        {
        case rLED:
            digitalWrite(rLED, HIGH);
            break;
        case yLED:
            digitalWrite(yLED, HIGH);
            break;
        case gLED:
            digitalWrite(gLED, HIGH);
            break;
        }
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

        if (!isPaused)
        {
            currentMiliseconds = millis();
            NonBlocking_Traffic_Light();
            Testing_Display();
        }
    }
}



void Testing_Display()
{
    display.showNumberDec(counter, true, 3, 1);
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
            Serial.print("GREEN light\t : ");
            Serial.print(counter);
            Serial.println("s");
        }
        break;

    case gLED: // Đèn xanh: 5 giây
        if (IsReady(ledTimeStart, gTIME))
        {
            digitalWrite(gLED, LOW);
            digitalWrite(yLED, HIGH);
            currentLED = yLED;
            counter = yTIME / 1000;
            Serial.print("YELLOW light\t : ");
            Serial.print(counter);
            Serial.println("s");
        }
        break;

    case yLED: // Đèn vàng: 2 giây
        if (IsReady(ledTimeStart, yTIME))
        {
            digitalWrite(yLED, LOW);
            digitalWrite(rLED, HIGH);
            currentLED = rLED;
            counter = rTIME / 1000;
            Serial.print("RED light\t : ");
            Serial.print(counter);
            Serial.println("s");
        }
        break;
    }
}
void yLED_Blink()
{
    static ulong lastBlinkTime = 0;  // Lưu thời gian lần nhấp nháy trước
    static bool isOn = false;        // Trạng thái đèn vàng

    if (millis() - lastBlinkTime >= 100)  // Mỗi 100ms đổi trạng thái đèn vàng
    {
        isOn = !isOn;
        digitalWrite(yLED, isOn ? HIGH : LOW);
        lastBlinkTime = millis();  // Cập nhật thời gian nhấp nháy
    }
}
