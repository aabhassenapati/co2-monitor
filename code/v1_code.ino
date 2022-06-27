#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTCZero.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_CCS811.h"
#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function
#define PCF8563address 0x51
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  A2
#define OLED_CLK   A1
#define OLED_DC    A4
#define OLED_CS    A5
#define OLED_RESET A3
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI,
                         OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
Adafruit_CCS811 ccs;
RTCZero rtc;
const int chipSelect = 4;
const byte MHZ19_CMD_READ_CO2[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; 
const byte MHZ19_CMD_CALIBRATE_ZERO[9] = {0xFF,0x01,0x87,0x00,0x00,0x00,0x00,0x00,0x78}; 
Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
int ppm = 0;
int ppmLast = 0;
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}
void setup() {
  Serial.begin(9600);
    Serial2.begin(9600);
     pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);
    pinMode(A0, OUTPUT);
  digitalWrite( A0, HIGH );
  readCO2(1);
  delay(1000);
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
  File dataFile = SD.open("logs.txt", FILE_WRITE);
 readPCF8563();
  display.clearDisplay();
  display.setCursor(0, 0);            // Start at top-left corner
  display.setTextSize(1);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print("CO2@C: ");
  ccs.readData();
  float cco2 = ccs.geteCO2();
  display.println(cco2);
  ppm = readCO2(0);
   // format; y=mx+c
  display.print("CO2@M: ");
  display.println(ppm);
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
  Serial.print(ppm);
    Serial.print("\t");
// // Serial.print("DATE: ");
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
    dataFile.print(cco2);
    dataFile.print(",");
    dataFile.print(ppm);
    dataFile.print(",");
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
    dataFile.print(second, DEC);
    dataFile.print(" ");
    dataFile.print(dayOfMonth, DEC);
    dataFile.print("/");
    dataFile.print(month, DEC);
    dataFile.print("/20");
    dataFile.println(year, DEC);
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
int readCO2( int calibrate )
{
  // command to ask for data
  char response[9]; // for answer

  if( calibrate == 1)
  {
    Serial2.write(MHZ19_CMD_CALIBRATE_ZERO, 9); //request PPM CO2 calibration
    Serial.println("Calibrating!");
  }
  else
  {
    Serial2.write(MHZ19_CMD_READ_CO2, 9); //request PPM CO2
  }
  //Serial2.write(MHZ19_CMD_READ_CO2, 9);
  Serial2.readBytes(response, 9);
  if (response[0] != 0xFF)
  {
    //Serial.println("Wrong starting byte from co2 sensor!");
    return 0;
  }

  if (response[1] != 0x86)
  {
    Serial.println("MH redetected");
     Serial2.readBytes(response, 9);
    Serial2.write(MHZ19_CMD_READ_CO2, 9);
     }

  // Return value:
  // FF=startbyte, 86=command, 02=high byte, 60=low byte, 00=4xnull bytes, 79=checksum
  // byte cmd[9] = {0xFF, 0x86, 0x02, 0x60, 0x47, 0x00, 0x00, 0x00, 0xD1};
  // Gas concentration: (high byte*256)+low byte

  int responseHigh = (int) response[2];
  int responseLow = (int) response[3];
  int ppm = (256 * responseHigh) + responseLow;
  return ppm;
}
