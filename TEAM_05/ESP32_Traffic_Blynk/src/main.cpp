#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6t-OvCkox"
#define BLYNK_TEMPLATE_NAME "ESP32 Traffic Blynk"
#define BLYNK_AUTH_TOKEN "Htp2pF3wBuKNSg17EwNLUDdx7mPoqLKw"
// Phải để trước khai báo sử dụng thư viện Blynk

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu cho việc chạy mô phỏng
char ssid[] = "Wokwi-GUEST"; // Tên mạng WiFi
char pass[] = "";            // Mật khẩu mạng WiFi

// Pin - Đèn giao thông
#define rLED 5
#define yLED 17
#define gLED 16

// Pin - TM1637
#define CLK 15
#define DIO 2

// Pin - Cảm biến quang
#define ldrPIN 32

// Pin - Nút nhấn và đèn xanh dương
#define btn1 23
#define bLED 21 // Đèn xanh dương

#define DHTPIN 27         // Chân kết nối DHT22
#define DHTTYPE DHT22     // Loại cảm biến
DHT dht(DHTPIN, DHTTYPE); // Khởi tạo đối tượng DHT

// Biến đo nhiệt độ và độ ẩm
float temperature = 0;
float humidity = 0;

// Thời gian chờ đèn
uint rTIME = 5000;
uint yTIME = 3000;
uint gTIME = 10000;

ulong currentMiliseconds = 0;
ulong ledTimeStart = 0;
ulong nextTimeTotal = 0;
int currentLED = 0;
int tmCounter = rTIME / 1000;
ulong counterTime = 0;
bool blueButtonON = true; // Trạng thái của nút bấm ON -> đèn Xanh sáng và hiển thị LED TM1637

int darkThreshold = 1000; // Ngưỡng độ sáng để bật/tắt đèn vàng
TM1637Display display(CLK, DIO);

bool displayOn = true; // Trạng thái màn hình mặc định: BẬT
bool lastButtonState = HIGH;

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light_TM1637();
bool isDark();
void YellowLED_Blink();
void updateBlueButton();
void uptimeBlynk();
void readDHT();
void setup()
{
  Serial.begin(115200);

  // Khởi tạo các chân đầu ra
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(ldrPIN, INPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(bLED, OUTPUT);

  tmCounter = (rTIME / 1000) - 1;
  display.setBrightness(0x0f);

  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  // Start the WiFi connection
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // Kết nối đến mạng WiFi

  Serial.println();
  Serial.println("WiFi connected");

  digitalWrite(bLED, blueButtonON ? HIGH : LOW);
  Blynk.virtualWrite(V1, blueButtonON); // Đồng bộ trạng thái trạng thái của đèn với Blynk

  Serial.println("== START ==>");

  display.showNumberDec(tmCounter--, true, 2, 2);
  currentLED = rLED;
  nextTimeTotal += rTIME;
  Serial.println("== START ==>");
  Serial.print("1. RED    => GREEN  ");
  Serial.print((nextTimeTotal / 1000) % 60);
  Serial.println(" (ms)");
}

void loop()
{
  currentMiliseconds = millis();
  updateBlueButton();
  readDHT();
  uptimeBlynk();
  if (isDark())
  {
    YellowLED_Blink(); // Nếu trời tối => Nhấp nháy đèn vàng
    display.clear();
  }
  else
    NonBlocking_Traffic_Light_TM1637(); // Hiển thị đèn giao thôngthông
}
bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond)
    return false;
  ulTimer = currentMiliseconds;
  return true;
}
void NonBlocking_Traffic_Light_TM1637()
{
  bool bShowCounter = false;

  switch (currentLED)
  {
  case rLED:
    if (IsReady(ledTimeStart, rTIME))
    {
      digitalWrite(rLED, LOW);
      digitalWrite(gLED, HIGH);
      currentLED = gLED;
      tmCounter = (gTIME / 1000) - 1;
      bShowCounter = true;
      counterTime = currentMiliseconds;
    }
    break;
  case gLED:
    if (IsReady(ledTimeStart, gTIME))
    {
      digitalWrite(gLED, LOW);
      digitalWrite(yLED, HIGH);
      currentLED = yLED;
      tmCounter = (yTIME / 1000) - 1;
      bShowCounter = true;
      counterTime = currentMiliseconds;
    }
    break;
  case yLED:
    if (IsReady(ledTimeStart, yTIME))
    {
      digitalWrite(yLED, LOW);
      digitalWrite(rLED, HIGH);
      currentLED = rLED;
      tmCounter = (rTIME / 1000) - 1;
      bShowCounter = true;
      counterTime = currentMiliseconds;
    }
    break;
  }

  if (!bShowCounter)
    bShowCounter = IsReady(counterTime, 1000);

  if (bShowCounter && displayOn)
  {
    display.showNumberDec(tmCounter--, true, 2, 2);
    // Blynk.virtualWrite(V0, tmCounter);
  }
}

bool isDark()
{
  static ulong darkTimeStart = 0;
  static uint16_t lastValue = 0;
  static bool bDark = false;

  if (!IsReady(darkTimeStart, 50))
    return bDark;
  uint16_t value = analogRead(ldrPIN);
  if (value == lastValue)
    return bDark;

  if (value < darkThreshold)
  {
    if (!bDark)
    {
      digitalWrite(currentLED, LOW);
      Serial.print("DARK  value: ");
      Serial.println(value);
    }
    bDark = true;
  }
  else
  {
    if (bDark)
    {
      digitalWrite(currentLED, LOW);
      Serial.print("LIGHT value: ");
      Serial.println(value);
    }
    bDark = false;
  }

  lastValue = value;
  return bDark;
}

void YellowLED_Blink()
{
  static ulong yLedStart = 0;
  static bool isON = false;

  if (!IsReady(yLedStart, 1000))
    return;
  if (!isON)
    digitalWrite(yLED, HIGH);
  else
    digitalWrite(yLED, LOW);
  isON = !isON;
}

void updateBlueButton()
{
  static ulong lastTime = 0;
  static int lastValue = HIGH;
  if (!IsReady(lastTime, 50))
    return; // Hạn chế bấm nút quá nhanh - 50ms mỗi lần bấm
  int v = digitalRead(btn1);
  if (v == lastValue)
    return;
  lastValue = v;
  if (v == LOW)
    return;

  if (!blueButtonON)
  {
    Serial.println("Blue Light ON");
    digitalWrite(bLED, HIGH);
    blueButtonON = true;
    Blynk.virtualWrite(V1, blueButtonON); // Gửi giá trị lên chân ảo V1 trên ứng dụng Blynk.
  }
  else
  {
    Serial.println("Blue Light OFF");
    digitalWrite(bLED, LOW);
    blueButtonON = false;
    Blynk.virtualWrite(V1, blueButtonON); // Gửi giá trị lên chân ảo V1 trên ứng dụng Blynk.
    display.clear();
  }
}

void uptimeBlynk()
{
  static ulong lastTime = 0;
  if (!IsReady(lastTime, 000))
    return; // Kiểm tra và cập nhật lastTime sau mỗi 1 giây
  ulong value = lastTime / 1000;
  Blynk.virtualWrite(V0, value); // Gửi giá trị lên chân ảo V0 trên ứng dụng Blynk.
  // if (blueButtonON)
  // {
  //   display.showNumberDec(value);
  // }
}

// được gọi mỗi khi có dữ liệu mới được gửi từ ứng dụng Blynk đến thiết bị.
BLYNK_WRITE(V1)
{ // virtual_pin định nghĩa trong ứng dụng Blynk
  // Xử lý dữ liệu nhận được từ ứng dụng Blynk
  blueButtonON = param.asInt(); // Lấy giá trị từ ứng dụng Blynk
  if (blueButtonON)
  {
    Serial.println("Blynk -> Blue Light ON");
    digitalWrite(bLED, HIGH);
  }
  else
  {
    Serial.println("Blynk -> Blue Light OFF");
    digitalWrite(bLED, LOW);
    display.clear();
  }
}

// Đọc nhiệt độ và độ ẩm
void readDHT()
{
  static ulong lastReadTime = 0;
  if (!IsReady(lastReadTime, 2000))
    return; // Đọc mỗi 2 giây

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Đọc DHT22 thất bại!");
    return;
  }

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, humidity);

  Serial.print("Nhiệt độ: ");
  Serial.print(temperature);
  Serial.print("°C\tĐộ ẩm: ");
  Serial.print(humidity);
  Serial.println("%");
}
// Hàm nhận giá trị ngưỡng ánh sáng từ Blynk
BLYNK_WRITE(V4)
{

  int newDarkThreshold = param.asInt();
  if (newDarkThreshold > 0)
  {
    darkThreshold = newDarkThreshold;
    Serial.print("Ngưỡng ánh sáng cập nhật từ Blynk:");
    Serial.println(darkThreshold);
  }
}
