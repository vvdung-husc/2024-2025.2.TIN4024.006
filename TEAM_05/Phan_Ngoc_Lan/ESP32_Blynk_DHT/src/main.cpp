#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6SS8FacQo"
#define BLYNK_TEMPLATE_NAME "ESP32 Blynk DHT"
#define BLYNK_AUTH_TOKEN "j2qUlcMj1NpwNcOFizYxKyaHFdr6wk-S"
// Phải để trước khai báo sử dụng thư viện Blynk

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu cho việc chạy mô phỏng
char ssid[] = "Wokwi-GUEST"; // Tên mạng WiFi
char pass[] = "";            // Mật khẩu mạng WiFi

#define btnBLED 23 // Chân kết nối nút bấm
#define pinBLED 21 // Chân kết nối đèn xxanh

#define CLK 18 // Chân kết nối CLK của TM1637
#define DIO 19 // Chân kết nối DIO của TM1637

#define DHTPIN 16         // Chân kết nối DHT22
#define DHTTYPE DHT22     // Loại cảm biến
DHT dht(DHTPIN, DHTTYPE); // Khởi tạo đối tượng DHT

// Biến đo nhiệt độ và độ ẩm
float temperature = 0;
float humidity = 0;
// Biến toàn cục
ulong currentMiliseconds = 0; // Thời gian hiện tại - miliseconds
bool blueButtonON = true;     // Trạng thái của nút bấm ON -> đèn Xanh sáng và hiển thị LED TM1637

// Khởi tạo mà hình TM1637
TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void updateBlueButton();
void uptimeBlynk();
void readDHT();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pinBLED, OUTPUT);
  pinMode(btnBLED, INPUT_PULLUP);

  display.setBrightness(0x0f);

  // Start the WiFi connection
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // Kết nối đến mạng WiFi

  Serial.println();
  Serial.println("WiFi connected");

  digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);
  Blynk.virtualWrite(V1, blueButtonON); // Đồng bộ trạng thái trạng thái của đèn với Blynk

  Serial.println("== START ==>");
}

void loop()
{
  Blynk.run(); // Chạy Blynk để cập nhật trạng thái từ Blynk Cloud

  currentMiliseconds = millis();
  uptimeBlynk();
  updateBlueButton();
  readDHT();
}

// put function definitions here:
bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond)
    return false;
  ulTimer = currentMiliseconds;
  return true;
}
void updateBlueButton()
{
  static ulong lastTime = 0;
  static int lastValue = HIGH;
  if (!IsReady(lastTime, 50))
    return; // Hạn chế bấm nút quá nhanh - 50ms mỗi lần bấm
  int v = digitalRead(btnBLED);
  if (v == lastValue)
    return;
  lastValue = v;
  if (v == LOW)
    return;

  if (!blueButtonON)
  {
    Serial.println("Blue Light ON");
    digitalWrite(pinBLED, HIGH);
    blueButtonON = true;
    Blynk.virtualWrite(V1, blueButtonON); // Gửi giá trị lên chân ảo V1 trên ứng dụng Blynk.
  }
  else
  {
    Serial.println("Blue Light OFF");
    digitalWrite(pinBLED, LOW);
    blueButtonON = false;
    Blynk.virtualWrite(V1, blueButtonON); // Gửi giá trị lên chân ảo V1 trên ứng dụng Blynk.
    display.clear();
  }
}

void uptimeBlynk()
{
  static ulong lastTime = 0;
  if (!IsReady(lastTime, 1000))
    return; // Kiểm tra và cập nhật lastTime sau mỗi 1 giây
  ulong value = lastTime / 1000;
  Blynk.virtualWrite(V0, value); // Gửi giá trị lên chân ảo V0 trên ứng dụng Blynk.
  if (blueButtonON)
  {
    display.showNumberDec(value);
  }
}

// được gọi mỗi khi có dữ liệu mới được gửi từ ứng dụng Blynk đến thiết bị.
BLYNK_WRITE(V1)
{ // virtual_pin định nghĩa trong ứng dụng Blynk
  // Xử lý dữ liệu nhận được từ ứng dụng Blynk
  blueButtonON = param.asInt(); // Lấy giá trị từ ứng dụng Blynk
  if (blueButtonON)
  {
    Serial.println("Blynk -> Blue Light ON");
    digitalWrite(pinBLED, HIGH);
  }
  else
  {
    Serial.println("Blynk -> Blue Light OFF");
    digitalWrite(pinBLED, LOW);
    display.clear();
  }
}
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