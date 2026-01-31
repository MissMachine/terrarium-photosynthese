#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* AP_SSID = "PHOTOSYNTH_LAB";
const char* AP_PASS = "plante123";   // tu peux changer
const int UDP_PORT = 4210;

WiFiUDP udp;
char packetBuffer[256];

// valeurs affichées
int co2ppm = -1;
int lux = -1;
float tempC = NAN;
float hum = NAN;

void parseKV(const String& msg) {
  // attend: "CO2=845;LUX=120;T=21.6;H=52.4"
  int idx = 0;
  while (idx < msg.length()) {
    int sep = msg.indexOf(';', idx);
    String part = (sep == -1) ? msg.substring(idx) : msg.substring(idx, sep);
    int eq = part.indexOf('=');
    if (eq != -1) {
      String k = part.substring(0, eq);
      String v = part.substring(eq + 1);
      k.trim(); v.trim();

      if (k == "CO2") co2ppm = v.toInt();
      else if (k == "LUX") lux = v.toInt();
      else if (k == "T") tempC = v.toFloat();
      else if (k == "H") hum = v.toFloat();
    }
    if (sep == -1) break;
    idx = sep + 1;
  }
}

void drawScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("CO2: ");
  if (co2ppm >= 0) { display.print(co2ppm); display.print(" ppm"); }
  else display.print("--");

  display.setCursor(0, 16);
  display.print("Lux: ");
  if (lux >= 0) { display.print(lux); display.print(" lx"); }
  else display.print("--");

  display.setCursor(0, 32);
  display.print("Temp: ");
  if (!isnan(tempC)) { display.print(tempC, 1); display.print(" C"); }
  else display.print("--");

  display.setCursor(0, 48);
  display.print("Hum: ");
  if (!isnan(hum)) { display.print(hum, 1); display.print(" %"); }
  else display.print("--");

  display.display();
}

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init fail");
    while (true) delay(1000);
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("ESP32 AP + OLED");
  display.println("Starting WiFi...");
  display.display();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(ip);

  udp.begin(UDP_PORT);
  Serial.print("UDP listen on ");
  Serial.println(UDP_PORT);

  delay(500);
  drawScreen();
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1);
    if (len > 0) packetBuffer[len] = '\0';

    String msg = String(packetBuffer);
    Serial.print("RX: ");
    Serial.println(msg);

    parseKV(msg);
    drawScreen();
  }
}
