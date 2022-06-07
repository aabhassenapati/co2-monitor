/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_CCS811.h"
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_CCS811 ccs;
#include <RTCZero.h>
RTCZero rtc;

const int chipSelect = 4;

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#define ANALOGPIN A0 
/* Comment out above, uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);
*/


void setup() {
  Serial.begin(9600);
  rtc.begin(); 
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println("CCS811 test");

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
    
  }
while(!ccs.available());
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
   if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
   }
  display.display();

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white


  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...
// Draw triangles (filled)

   // Draw characters of the default font

    // Draw 'stylized' characters

 
  // Invert and restore display, pausing in-between
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);
 pinMode(ANALOGPIN, INPUT_PULLUP);                         // Pullup A0
ccs.begin();
  }

void loop() {
File dataFile = SD.open("CO2_log.txt", FILE_WRITE);
if (dataFile) {
  
      dataFile.print("CO2@C: ");
       ccs.readData();
     dataFile.println(ccs.geteCO2());
         float adjustedADC = analogRead(A0);   
                    float Co2 = ((6.4995*adjustedADC) - 590.53); // format; y=mx+c
        dataFile.print("CO2@M: ");      
       dataFile.println(Co2);
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
  display.clearDisplay();
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextSize(1);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);

       display.print("CO2@C: ");
       ccs.readData();
      display.println(ccs.geteCO2());
       float adjustedADC = analogRead(A0);   
       
             float Co2 = ((6.4995*adjustedADC) - 590.53); // format; y=mx+c
        display.print("CO2@M: ");      
        display.println(Co2);
        display.print("TIME: ");  
         display.print(rtc.getDay());
 display.print("/");
  display.print(rtc.getMonth());
  display.print("/");
  display.print(rtc.getYear());
  display.print(" ");

  // ...and time
  display.print(rtc.getHours());
  display.print(":");
  display.print(rtc.getMinutes());
  display.print(":");
  display.print(rtc.getSeconds());
delay(800);
   display.display();
} 
