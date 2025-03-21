#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels

#define OLED_SDA 4   // I2C SDA pin for ESP8266 (GPIO4)
#define OLED_SCL 5   // I2C SCL pin for ESP8266 (GPIO5)

#define DHTPIN 16    // Digital pin connected to the DHT sensor (GPIO16)
#define DHTTYPE DHT22 // DHT 22 (AM2302)

#define LED_BLUE 15
#define LED_YELLOW 2
#define LED_RED 4
#define LED_CAM 5

// Create an OLED display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHT dht(DHTPIN, DHTTYPE);

String strTemp;
bool ledState = false;
int8_t ledNumber = LED_BLUE;

void setup()
{
  Serial.begin(115200); // Start serial communication at 115200 baud rate

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_CAM, OUTPUT);

  digitalWrite(LED_BLUE, LOW);   // OFF
  digitalWrite(LED_YELLOW, LOW); // OFF
  digitalWrite(LED_RED, LOW);    // OFF
  digitalWrite(LED_CAM, LOW);    // OFF

  dht.begin();

  // Initialize I2C communication with ESP8266
  Wire.begin(OLED_SDA, OLED_SCL);

  // Initialize OLED display with I2C address 0x3C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("Failed to start SSD1306 OLED"));
    while (1)
      ;
  }

  delay(2000);         // Wait two seconds for initialization
  oled.clearDisplay(); // Clear display

  oled.setTextSize(2);               // Set text size
  oled.setTextColor(WHITE);          // Set text color
  oled.setCursor(0, 2);              // Set position to display (x,y)
  oled.println("   IOT\n Welcome!"); // Set text
  oled.display();                    // Display on OLED
}

void loop()
{
  delay(1000);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
  }

  oled.clearDisplay();

  strTemp = String("Temperature: ");
  bool bCamON = true;
  if (t < 0.0)
  {
    strTemp += "Too COOL";
    ledNumber = LED_BLUE;
    bCamON = false;
  }
  else if (t < 40.0)
  {
    strTemp += "Normal";
    ledNumber = LED_YELLOW;
  }
  else
  {
    strTemp += "Too HOT";
    ledNumber = LED_RED;
  }

  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print(strTemp.c_str());
  oled.setTextSize(2);
  oled.setCursor(0, 10);
  oled.print(t);
  oled.print(" ");
  oled.setTextSize(1);
  oled.cp437(true);
  oled.write(167); //*C 
  oled.setTextSize(2);
  oled.print("C");

  oled.setTextSize(1);
  oled.setCursor(0, 35);
  oled.print("Humidity: ");
  oled.setTextSize(2);
  oled.setCursor(0, 45);
  oled.print(h);
  oled.print(" %");

  oled.display();

  if (bCamON)
    digitalWrite(LED_CAM, HIGH);
  digitalWrite(ledNumber, HIGH);
  delay(500);
  digitalWrite(ledNumber, LOW);
  if (!bCamON)
    digitalWrite(LED_CAM, LOW);
}
