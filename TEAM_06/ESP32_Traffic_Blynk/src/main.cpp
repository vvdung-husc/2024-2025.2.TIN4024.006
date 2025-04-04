/* Nguyễn Văn Tuấn Hải */
#define BLYNK_TEMPLATE_ID "TMPL6nKf8MdXm"
#define BLYNK_TEMPLATE_NAME "Esp32 Traffic Blynk"
#define BLYNK_AUTH_TOKEN "BNZyTNA92WBaNZyI9n1QyMGJcRb1ePa9"

// Huỳnh Ngọc Vĩnh Khang
//#define BLYNK_TEMPLATE_ID "TMPL6O0lyhqIF"
//#define BLYNK_TEMPLATE_NAME "ESP32 LED TM1637" 
//#define BLYNK_AUTH_TOKEN "l7wJ7KNMTtXJwGAX68QaqmcPJMWBO09C"

//Trần Hải Đông
// #define BLYNK_TEMPLATE_ID "TMPL6CrjN4tCi"
// #define BLYNK_TEMPLATE_NAME "ESP32 Blynkk Trafic Project"
// #define BLYNK_AUTH_TOKEN "ebn0HBX3ebcrl2pp93LxiUyGev2x0q8g"

//Dương Duy Quốc
// #define BLYNK_TEMPLATE_ID "TMPL6CAb1i-FP"
// #define BLYNK_TEMPLATE_NAME "Quoc"
// #define BLYNK_AUTH_TOKEN "zAqkB_WAIkQhDwwl322ZBsh04YgWlXv"

#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// WiFi thông tin
char tenWifi[] = "Wokwi-GUEST";
char matKhau[] = "";

// Chân kết nối phần cứng
#define nutNhay  23  // Nút nhấn bật/tắt đèn xanh
#define denXanhDuong  21  // Đèn LED xanh dương
#define CLK 18       // Chân CLK của TM1637
#define DIO 19       // Chân DIO của TM1637
#define camBienNhiet 16    // Cảm biến nhiệt độ DHT22
#define loaiDHT DHT22      // Kiểu cảm biến DHT22
#define camBienAnhSang 33  // Cảm biến ánh sáng LDR
#define denDo 27           // Đèn đỏ
#define denVang 26         // Đèn vàng
#define denXanh 25         // Đèn xanh

// Khởi tạo đối tượng
TM1637Display manHinhLED(CLK, DIO);
DHT dht(camBienNhiet, loaiDHT);

ulong thoiGianHienTai = 0;
bool trangThaiDenXanh = true;
bool laBanDem = false;

bool kiemTraThoiGian(ulong &boDem, uint32_t miligiay);
void xuLyNutNhan();
void hienThiSoGiay();
void docNhietDo();
void docCamBienAnhSang();
void dieuKhienDenGiaoThong();

void setup() {
  Serial.begin(115200);
  pinMode(denXanhDuong, OUTPUT);
  pinMode(nutNhay, INPUT_PULLUP);
  pinMode(camBienAnhSang, INPUT);
  pinMode(denDo, OUTPUT);
  pinMode(denVang, OUTPUT);
  pinMode(denXanh, OUTPUT);
  
  manHinhLED.setBrightness(0x0f);
  dht.begin();

  Serial.print("Đang kết nối tới "); Serial.println(tenWifi);
  Blynk.begin(BLYNK_AUTH_TOKEN, tenWifi, matKhau);
  Serial.println("WiFi đã kết nối");

  digitalWrite(denXanhDuong, trangThaiDenXanh ? HIGH : LOW);
  Blynk.virtualWrite(V1, trangThaiDenXanh);

  Serial.println("== BẮT ĐẦU ==>");
}

void loop() {  
  Blynk.run();
  thoiGianHienTai = millis();

  hienThiSoGiay();
  xuLyNutNhan();
  docNhietDo();
  docCamBienAnhSang();
  dieuKhienDenGiaoThong();
}

// Kiểm tra khoảng thời gian
bool kiemTraThoiGian(ulong &boDem, uint32_t miligiay) {
  if (thoiGianHienTai - boDem < miligiay) return false;
  boDem = thoiGianHienTai;
  return true;
}

// Xử lý nút nhấn
void xuLyNutNhan() {
  static ulong lanCuoi = 0;
  static int giaTriCuoi = HIGH;
  if (!kiemTraThoiGian(lanCuoi, 50)) return;
  
  int giaTriHienTai = digitalRead(nutNhay);
  if (giaTriHienTai == giaTriCuoi) return;
  giaTriCuoi = giaTriHienTai;
  if (giaTriHienTai == LOW) return;

  trangThaiDenXanh = !trangThaiDenXanh;
  Serial.println(trangThaiDenXanh ? "🔵 Bật đèn xanh dương" : "⚫ Tắt đèn xanh dương");
  digitalWrite(denXanhDuong, trangThaiDenXanh ? HIGH : LOW);
  Blynk.virtualWrite(V1, trangThaiDenXanh);

  if (!trangThaiDenXanh) {
    Serial.println("⚠️ Tắt màn hình LED");
    manHinhLED.clear();
  }
}

// Hiển thị số giây trên màn hình LED
void hienThiSoGiay() {
  static ulong lanCuoi = 0;
  if (!kiemTraThoiGian(lanCuoi, 1000)) return;

  ulong soGiay = lanCuoi / 1000;
  Blynk.virtualWrite(V0, soGiay);

  if (trangThaiDenXanh) {
    manHinhLED.showNumberDec(soGiay);
  }
}

// Đọc nhiệt độ & độ ẩm từ DHT22
void docNhietDo() {
  static ulong lanCuoi = 0;
  if (!kiemTraThoiGian(lanCuoi, 2000)) return;

  float nhietDo = dht.readTemperature();
  float doAm = dht.readHumidity();

  if (isnan(nhietDo) || isnan(doAm)) {
    Serial.println("⚠️ Lỗi! Không đọc được dữ liệu từ DHT22");
    return;
  }

  Serial.print("🌡️ Nhiệt độ: "); Serial.print(nhietDo); Serial.print("°C  ");
  Serial.print("💧 Độ ẩm: "); Serial.print(doAm); Serial.println("%");

  Blynk.virtualWrite(V2, nhietDo);
  Blynk.virtualWrite(V3, doAm);
}

// Đọc cảm biến ánh sáng và cập nhật trạng thái ngày/đêm
void docCamBienAnhSang() {
  static ulong lanCuoi = 0;
  if (!kiemTraThoiGian(lanCuoi, 1000)) return;

  int giaTriAnhSang = analogRead(camBienAnhSang);
  Serial.print("💡 Cảm biến ánh sáng: "); Serial.println(giaTriAnhSang);
  Blynk.virtualWrite(V7, giaTriAnhSang);

  laBanDem = giaTriAnhSang < 1000;
}

// Điều khiển đèn giao thông
// Điều khiển đèn giao thông
void dieuKhienDenGiaoThong() {
  static ulong lanCuoi = 0;
  static int trangThaiDen = 0;  // 0: Xanh, 1: Vàng, 2: Đỏ

  if (laBanDem) {
    if (kiemTraThoiGian(lanCuoi, 500)) {
      digitalWrite(denDo, LOW);
      digitalWrite(denXanh, LOW);
      digitalWrite(denVang, !digitalRead(denVang));
      Serial.println("🌙 Trời tối - Nhấp nháy đèn vàng");

      Blynk.virtualWrite(V6, 0);
      Blynk.virtualWrite(V5, digitalRead(denVang));
      Blynk.virtualWrite(V4, 0);
    }
  } else {
    if (kiemTraThoiGian(lanCuoi, 3000)) {
      if (trangThaiDen == 0) trangThaiDen = 1;  // Xanh -> Vàng
      else if (trangThaiDen == 1) trangThaiDen = 2; // Vàng -> Đỏ
      else trangThaiDen = 0;  // Đỏ -> Xanh

      digitalWrite(denDo, trangThaiDen == 2);
      digitalWrite(denVang, trangThaiDen == 1);
      digitalWrite(denXanh, trangThaiDen == 0);

      Serial.print("☀️ Trời sáng - Đèn giao thông: ");
      if (trangThaiDen == 2) Serial.println("🔴 Đèn đỏ");
      else if (trangThaiDen == 1) Serial.println("🟡 Đèn vàng");
      else Serial.println("🟢 Đèn xanh");

      Blynk.virtualWrite(V6, trangThaiDen == 2);
      Blynk.virtualWrite(V5, trangThaiDen == 1);
      Blynk.virtualWrite(V4, trangThaiDen == 0);
    }
  }
}

// Nhận dữ liệu từ Blynk
BLYNK_WRITE(V1) {
  trangThaiDenXanh = param.asInt();
  Serial.println(trangThaiDenXanh ? "🟢 Bật đèn" : "🔴 Tắt đèn");

  digitalWrite(denXanhDuong, trangThaiDenXanh ? HIGH : LOW);
  if (!trangThaiDenXanh) {
    Serial.println("⚠️ Tắt màn hình LED");
    manHinhLED.clear();
  }
}
