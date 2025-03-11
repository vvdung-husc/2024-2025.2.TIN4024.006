#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>

/* Fill in information from Blynk Device Info here */
// #define BLYNK_TEMPLATE_ID "TMPL6nbCvap-d"
// #define BLYNK_TEMPLATE_NAME "ESP32 blink trafic DHT senor"
// #define BLYNK_AUTH_TOKEN "PrTa8hHSCix_9LUGZmtwoXA--43GftEg"

//Trần Hải Đông
#define BLYNK_TEMPLATE_ID "TMPL6CrjN4tCi"
#define BLYNK_TEMPLATE_NAME "ESP32 Blynkk Trafic Project"
#define BLYNK_AUTH_TOKEN "s87JscYfMFpl9SSk6QHRNTUyZawlW5Bc"

// Thư viện Blynk và kết nối WiFi
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu cho việc chạy mô phỏng
char ssid[] = "Wokwi-GUEST";  // Tên mạng WiFi
char pass[] = "";             // Mật khẩu mạng WiFi

#define btnBLED  23 // Chân kết nối nút bấm
#define pinBLED  21 // Chân kết nối đèn xanh

#define CLK 18  // Chân kết nối CLK của TM1637
#define DIO 19  // Chân kết nối DIO của TM1637

// Cấu hình cảm biến DHT22
#define DHTPIN 16     // Chân nối DHT22 (sử dụng chân số 16)
#define DHTTYPE DHT22 // Loại cảm biến DHT22
DHT dht(DHTPIN, DHTTYPE);

// Biến toàn cục
unsigned long currentMiliseconds = 0; // Thời gian hiện tại - miliseconds 
bool blueButtonON = true;             // Trạng thái của nút bấm (ON: đèn xanh sáng, hiển thị trên TM1637)

// Khởi tạo màn hình TM1637
TM1637Display display(CLK, DIO);

// Prototype của các hàm
bool IsReady(unsigned long &ulTimer, uint32_t milisecond);
void updateBlueButton();
void uptimeBlynk();
void updateDHTSensor();

void setup() {
  Serial.begin(115200);
  pinMode(pinBLED, OUTPUT);
  pinMode(btnBLED, INPUT_PULLUP);
  
  display.setBrightness(0x0f);
  
  // Kết nối WiFi và Blynk Cloud
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println();
  Serial.println("WiFi connected");
  
  // Đồng bộ trạng thái LED với Blynk
  digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);  
  Blynk.virtualWrite(V1, blueButtonON);
  
  Serial.println("== START ==");
  
  // Khởi tạo cảm biến DHT22
  dht.begin();
}

void loop() {  
  Blynk.run();  // Cập nhật trạng thái từ Blynk Cloud

  currentMiliseconds = millis();
  uptimeBlynk();
  updateBlueButton();
  updateDHTSensor();
}

// Hàm kiểm tra khoảng cách thời gian giữa các lần thực hiện
bool IsReady(unsigned long &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

// Hàm cập nhật trạng thái nút bấm và đèn xanh
void updateBlueButton(){
  static unsigned long lastTime = 0;
  static int lastValue = HIGH;
  if (!IsReady(lastTime, 50)) return;
  int v = digitalRead(btnBLED);
  if (v == lastValue) return;
  lastValue = v;
  if (v == LOW) return;

  if (!blueButtonON){
    Serial.println("Blue Light ON");
    digitalWrite(pinBLED, HIGH);
    blueButtonON = true;
    Blynk.virtualWrite(V1, blueButtonON);
  }
  else {
    Serial.println("Blue Light OFF");
    digitalWrite(pinBLED, LOW);    
    blueButtonON = false;
    Blynk.virtualWrite(V1, blueButtonON);
    display.clear();
  }    
}

// Hàm cập nhật thời gian uptime và hiển thị trên màn hình TM1637 nếu đèn xanh bật
void uptimeBlynk(){
  static unsigned long lastTime = 0;
  if (!IsReady(lastTime, 1000)) return;
  unsigned long seconds = currentMiliseconds / 1000;
  Blynk.virtualWrite(V0, seconds);
  if (blueButtonON){
    display.showNumberDec(seconds);
  }
}

// Hàm đọc dữ liệu cảm biến DHT22 và gửi lên Blynk
void updateDHTSensor(){
  static unsigned long lastDHTTime = 0;
  if (currentMiliseconds - lastDHTTime < 2000) return; // Cập nhật mỗi 2 giây
  lastDHTTime = currentMiliseconds;
  
  // Đọc giá trị nhiệt độ và độ ẩm
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Kiểm tra nếu đọc không thành công
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // In ra Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
  
  // Gửi giá trị nhiệt độ và độ ẩm lên ứng dụng Blynk
  // Ở đây: V2 -> Nhiệt độ, V3 -> Độ ẩm
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, humidity);
}

// Hàm callback khi có dữ liệu mới từ ứng dụng Blynk cho LED
BLYNK_WRITE(V1) { 
  blueButtonON = param.asInt();
  if (blueButtonON){
    Serial.println("Blynk -> Blue Light ON");
    digitalWrite(pinBLED, HIGH);
  }
  else {
    Serial.println("Blynk -> Blue Light OFF");
    digitalWrite(pinBLED, LOW);   
    display.clear(); 
  }
}
