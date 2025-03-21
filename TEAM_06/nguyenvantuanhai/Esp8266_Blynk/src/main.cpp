// Thông tin Blynk
#define BLYNK_TEMPLATE_ID "TMPL6_0wllQ2P"
#define BLYNK_TEMPLATE_NAME "ESP8266 Blynk"
#define BLYNK_AUTH_TOKEN "8_dbofoXgl9tGvigYjj_oDOM6EuZOJfC"

#include <Arduino.h>
#include "utils.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>

// Khai báo chân điều khiển
#define DEN_XANH 15
#define DEN_VANG 2
#define DEN_DO 5

#define CAM_BIEN_DHT 16 // Chân kết nối cảm biến DHT11
#define DHT_LOAI DHT11
#define OLED_SDA 13
#define OLED_SCL 12

// Khởi tạo màn hình OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
DHT dht(CAM_BIEN_DHT, DHT_LOAI);

// Thông tin WiFi
char wifi_ssid[] = "CNTT-MMT";
char wifi_pass[] = "13572468";

bool trangThaiNutNhan = false;
float doAm = 0.0;
float nhietDo = 0.0;

bool HienThi(uint thoiGianCho = 5000) {
    static ulong thoiGianTruoc = 0;
    static bool daHienThi = false;
    if (daHienThi) return true;
    if (!IsReady(thoiGianTruoc, thoiGianCho)) return false;
    daHienThi = true;
    return daHienThi;
}

void setup() {
    Serial.begin(115200);
    pinMode(DEN_XANH, OUTPUT);
    pinMode(DEN_VANG, OUTPUT);
    pinMode(DEN_DO, OUTPUT);
    
    digitalWrite(DEN_XANH, LOW);
    digitalWrite(DEN_VANG, LOW);
    digitalWrite(DEN_DO, LOW);

    dht.begin();
    Wire.begin(OLED_SDA, OLED_SCL);
    Blynk.begin(BLYNK_AUTH_TOKEN, wifi_ssid, wifi_pass);

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ Kết nối WiFi thất bại!");
    } else {
        Serial.println("✅ Đã kết nối WiFi!");
    }
    Blynk.virtualWrite(V3, trangThaiNutNhan);
    oled.begin();
    oled.clearBuffer();
    oled.sendBuffer();
    randomSeed(analogRead(0));
}

void NhayDenBaMau() {
    static ulong thoiGianTruoc = 0;
    static int denHienTai = 0;
    static const int danhSachDen[3] = { DEN_XANH, DEN_VANG, DEN_DO };
    if (!IsReady(thoiGianTruoc, 1000)) return;
    int denTruoc = (denHienTai + 2) % 3;
    digitalWrite(danhSachDen[denTruoc], LOW);
    digitalWrite(danhSachDen[denHienTai], HIGH);
    denHienTai = (denHienTai + 1) % 3;
}

void CapNhatManHinh() {
    static ulong thoiGianTruoc = 0;
    if (!IsReady(thoiGianTruoc, 1000)) return;

    ulong thoiGianChay = millis() / 1000;
    int phut = (thoiGianChay % 3600) / 60;
    int giay = thoiGianChay % 60;

    float h = random(0, 10001) / 100.0;
    float t = random(-4000, 8001) / 100.0;

    if (isnan(h) || isnan(t)) {
        Serial.println("Lỗi đọc cảm biến DHT!");
        return;
    }

    nhietDo = t;
    doAm = h;

    oled.clearBuffer();
    oled.setFont(u8g2_font_unifont_t_symbols);
    oled.drawUTF8(0, 16, StringFormat("Nhiet do: %.1f C", t).c_str());
    oled.drawUTF8(0, 32, StringFormat("Do am: %.1f %%", h).c_str());
    oled.drawUTF8(0, 48, StringFormat("Thoi gian: %02d:%02d", phut, giay).c_str());
    oled.sendBuffer();

    Blynk.virtualWrite(V1, t);
    Blynk.virtualWrite(V2, h);
    Blynk.virtualWrite(V0, thoiGianChay);
}

void NhayDenVang() {
    static bool trangThaiVang = false;
    static unsigned long thoiGianTruoc = 0;
    if (IsReady(thoiGianTruoc, 500)) {
        trangThaiVang = !trangThaiVang;
        digitalWrite(DEN_VANG, trangThaiVang);
    }
    digitalWrite(DEN_XANH, LOW);
    digitalWrite(DEN_DO, LOW);
}

void CapNhatThoiGianBlynk() {
    static ulong thoiGianTruoc = 0;
    if (!IsReady(thoiGianTruoc, 1000)) return;
    ulong giaTri = millis() / 1000;
    Blynk.virtualWrite(V0, giaTri);
}

BLYNK_WRITE(V3) {
    trangThaiNutNhan = param.asInt();
}

void loop() {
    if (!HienThi()) return;
    NhayDenBaMau();
    CapNhatManHinh();
    CapNhatThoiGianBlynk();
    if (trangThaiNutNhan) {
        NhayDenVang();
    }
}