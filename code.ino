#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_CCS811.h"
#include <RTCZero.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI,
OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
Adafruit_CCS811 ccs;
RTCZero rtc;
const int chipSelect = 4;
void setup() {
  Serial.begin(9600);
  rtc.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  Serial.println("CCS811 test");
  if (!ccs.begin()) {
    Serial.println("Failed to start sensor! Please check your wiring.");
    while (1);
  }
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  display.display();
  // Clear the buffer
  display.clearDisplay();
  pinMode(A0, INPUT_PULLUP);                         // Pullup A0
}
void loop() {
  File dataFile = SD.open("CO2_log.txt", FILE_WRITE);
  display.clearDisplay();
  display.setCursor(0, 0);            // Start at top-left corner
  display.setTextSize(1);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print("CO2@C: ");
  ccs.readData();
  float cco2 = ccs.geteCO2();
  display.println(cco2);
  float mco2 = analogRead(A0);
  mco2 = ((6.4995 * mco2) - 590.53); // format; y=mx+c
  display.print("CO2@M: ");
  display.println(mco2);
  display.print("TIME: ");
  display.print(rtc.getDay());
  display.print("/");
  display.print(rtc.getMonth());
  display.print("/");
  display.print(rtc.getYear());
  display.print(" ");
  display.print(rtc.getHours());
  display.print(":");
  display.print(rtc.getMinutes());
  display.print(":");
  display.print(rtc.getSeconds());
  display.display();
  if (dataFile) {
    dataFile.print("CO2@C: ");
    dataFile.println(cco2);
    dataFile.print("CO2@M: ");
    dataFile.println(mco2);
    dataFile.print("TIME: ");
    dataFile.print(rtc.getDay());
    dataFile.print("/");
    dataFile.print(rtc.getMonth());
    dataFile.print("/");
    dataFile.print(rtc.getYear());
    dataFile.print(" ");
    dataFile.print(rtc.getHours());
    dataFile.print(":");
    dataFile.print(rtc.getMinutes());
    dataFile.print(":");
    dataFile.print(rtc.getSeconds());
    dataFile.println("-----------");
    dataFile.close();
  }
}
