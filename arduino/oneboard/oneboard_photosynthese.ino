#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

#include <Adafruit_SGP30.h>
#include <Adafruit_SHT31.h>
#include <BH1750.h>

const char* WIFI_SSID = "PHOTOSYNTH_LAB";
const char* WIFI_PASS = "plante123";

IPAddress esp32IP(192,168,4,1);
const int UDP_PORT = 4210;
WiFiUDP udp;

Adafruit_SGP30 sgp;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
BH1750 lightMeter;

unsigned long lastSend = 0;
const unsigned long SEND_MS = 1000; // 1 seconde

void setup() {
  Serial.begin(115200);

  // I2C ESP8266 (souvent D2 SDA, D1 SCL)
  Wire.begin(D2, D1);

  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to AP");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. IP=");
  Serial.println(WiFi.localIP());

  // Capteurs
  if (!sgp.begin()) {
    Serial.println("SGP30 not found");
  } else {
    Serial.println("SGP30 OK");
  }

  if (!sht31.begin(0x44)) {
    Serial.println("SHT31 not found");
  } else {
    Serial.println("SHT31 OK");
  }

  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 not found");
  } else {
    Serial.println("BH1750 OK");
  }

  udp.begin(UDP_PORT);
}

void loop() {
  if (millis() - lastSend >= SEND_MS) {
    lastSend = millis();

    // Lecture SHT31
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();

    // Lecture BH1750
    float luxF = lightMeter.readLightLevel();
    int lux = (luxF < 0) ? -1 : (int)luxF;

    // Lecture SGP30 (eco2)
    // Important: SGP30 nécessite des lectures régulières.
    // On fait un IAQmeasure() toutes les secondes.
    int co2 = -1;
    if (sgp.IAQmeasure()) {
      co2 = (int)sgp.eCO2;
    }

    String msg = "CO2=" + String(co2) +
                 ";LUX=" + String(lux) +
                 ";T=" + String(t, 1) +
                 ";H=" + String(h, 1);

    Serial.println(msg);

    udp.beginPacket(esp32IP, UDP_PORT);
    udp.write(msg.c_str());
    udp.endPacket();
  }
}
