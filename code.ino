#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_CCS811.h"
#define PCF8563address 0x51
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
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
Adafruit_CCS811 ccs;
const int chipSelect = 4;
void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  Serial.println("CCS811 test");
  if (!ccs.begin()) {
    Serial.println("Failed to start sensor! Please check your wiring.");
  }
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  display.display();
  // Clear the buffer
  display.clearDisplay();
  pinMode(A0, INPUT_PULLUP);                         // Pullup A0
Serial.print("CO2@C: ");
  Serial.print("\t");
Serial.print("CO2@M: ");
  Serial.print("\t");
Serial.print("DATE: ");
  Serial.print("\t");
Serial.print("TIME: ");
  Serial.print("\t");
     Serial.println();
}
void loop() {
  File dataFile = SD.open("co2_logs.txt", FILE_WRITE);
  readPCF8563();
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
  display.print("DATE: ");
  display.print(dayOfMonth, DEC);
  display.print("/");
  display.print(month, DEC);
  display.print("/20");
  display.print(year, DEC);
  display.println();
  display.print("TIME: ");
  display.print(hour, DEC);
  display.print(":");
  if (minute < 10)
  {
    display.print("0");
  }
  display.print(minute, DEC);
  display.print(":");
  if (second < 10)
  {
    display.print("0");
  }
  display.println(second, DEC);
  display.display();
 // Serial.print("CO2@C: ");
  Serial.print(cco2);
    Serial.print("\t");
 // Serial.print("CO2@M: ");
  Serial.print(mco2);
    Serial.print("\t");
 // Serial.print("DATE: ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/20");
  Serial.print(year, DEC);
  Serial.print("\t");
 // Serial.print("TIME: ");
  //Serial.print(days[dayOfWeek]);
  // Serial.print(" ");
  Serial.print(hour, DEC);
  Serial.print(":");
  if (minute < 10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second < 10)
  {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print("\t");
   Serial.println();
  if (dataFile) {
    dataFile.print("CO2@C: ");
    dataFile.println(cco2);
    dataFile.print("CO2@M: ");
    dataFile.println(mco2);
    dataFile.print("DATE: ");
    dataFile.print(dayOfMonth, DEC);
    dataFile.print("/");
    dataFile.print(month, DEC);
    dataFile.print("/20");
    dataFile.print(year, DEC);
    dataFile.println();
    dataFile.print("TIME: ");
    dataFile.print(hour, DEC);
    dataFile.print(":");
    if (minute < 10)
    {
      dataFile.print("0");
    }
    dataFile.print(minute, DEC);
    dataFile.print(":");
    if (second < 10)
    {
      dataFile.print("0");
    }
    dataFile.println(second, DEC);
    dataFile.println("-----------");
    dataFile.close();
  }
  delay(1000);
}
byte bcdToDec(byte value)
{
  return ((value / 16) * 10 + value % 16);
}
byte decToBcd(byte value) {
  return (value / 10 * 16 + value % 10);
}
void readPCF8563()
// this gets the time and date from the PCF8563
{
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(PCF8563address, 7);
  second     = bcdToDec(Wire.read() & B01111111); // remove VL error bit
  minute     = bcdToDec(Wire.read() & B01111111); // remove unwanted bits from MSB
  hour       = bcdToDec(Wire.read() & B00111111);
  dayOfMonth = bcdToDec(Wire.read() & B00111111);
  dayOfWeek  = bcdToDec(Wire.read() & B00000111);
  month      = bcdToDec(Wire.read() & B00011111);  // remove century bit, 1999 is over
  year       = bcdToDec(Wire.read());
}
