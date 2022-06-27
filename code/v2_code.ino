#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTCZero.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_CCS811.h"
#include "wiring_private.h"
#define PCF8563address 0x51
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
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
const byte MHZ19_CMD_READ_CO2[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
const byte MHZ19_CMD_CALIBRATE_ZERO[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
int ppm = 0;
float cco2;
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
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println("co2Monitor");
  display.setTextSize(2);
  display.println("by Aabhas");
  display.display();
  delay(5000);
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
  delay(1000);
  File dataFile = SD.open("logs.txt", FILE_WRITE);
  readPCF8563();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  //display.print("co2:");
  ppm = readCO2(0);
  ccs.readData();
  cco2 = ccs.geteCO2();
  if ((ppm > 0) && (ppm < 5001))
  {
    display.print(ppm);
    display.setTextSize(2);
    display.println(" PPM");
  }

  else
  {
    display.print(((int)(cco2)));
    display.setTextSize(2);
    display.println(" PPM");
  }
  display.println();
  display.print("Time:");
  display.print(hour, DEC);
  display.print(":");
  if (minute < 10)
  {
    display.print("0");
  }
  display.print(minute, DEC);
  display.display();
  Serial.print(cco2);
  Serial.print("\t");
  Serial.print(ppm);
  Serial.print("\t");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/20");
  Serial.print(year, DEC);
  Serial.print("\t");
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
}
byte bcdToDec(byte value)
{
  return ((value / 16) * 10 + value % 16);
}
byte decToBcd(byte value) {
  return (value / 10 * 16 + value % 10);
}
void readPCF8563()
{
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(PCF8563address, 7);
  second     = bcdToDec(Wire.read() & B01111111);
  minute     = bcdToDec(Wire.read() & B01111111);
  hour       = bcdToDec(Wire.read() & B00111111);
  dayOfMonth = bcdToDec(Wire.read() & B00111111);
  dayOfWeek  = bcdToDec(Wire.read() & B00000111);
  month      = bcdToDec(Wire.read() & B00011111);
  year       = bcdToDec(Wire.read());
}
int readCO2( int calibrate )
{
  char response[9]; // for answer
  if ( calibrate == 1)
  {
    Serial2.write(MHZ19_CMD_CALIBRATE_ZERO, 9);
    Serial.println("Calibrating!");
  }
  else
  {
    Serial2.write(MHZ19_CMD_READ_CO2, 9);
  }
  Serial2.readBytes(response, 9);
  if (response[0] != 0xFF)
  {
    return 0;
  }
  if (response[1] != 0x86)
  {
    Serial.println("MH redetected");
    Serial2.readBytes(response, 9);
    return cco2;
  }
  int responseHigh = (int) response[2];
  int responseLow = (int) response[3];
  int ppm = (256 * responseHigh) + responseLow;
  return ppm;
}
