// Installed libraries:
//   Adafruit_SSD1306
//   Adafruit_GFX_Library
//   Adafruit_Sensor
//   DHT_sensor_library

// Connections:
//   pin 2  - DHD11 data pin (DHT11 pins: 1=vcc, 2=data, 4=gnd)
//   pin A5 - display SCX pin
//   pin A4 - display SDA pin

// Arduino I2C pins (SDA, SCL):
//   Arduino Uno (Nano, Pro Mini, using ATMega328/168): SDA = pin A4, SCL = pin A5.
//   Arduino Mega: SDA = pin 20, SCL = pin 21.



/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#ifndef ARDUINO
    #define ARDUINO 10900
#endif
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 2     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;



#define SCREEN_WIDTH 64//64//128 // OLED display width, in pixels
#define SCREEN_HEIGHT 8//32//64 //32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1//4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }



  // Initialize device.
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void w1(const __FlashStringHelper* s) {
  w(s,0,0);
}
void w3(const __FlashStringHelper* s, int f, const __FlashStringHelper* s2) {
  w(s,&f,s2);
}
void w(const __FlashStringHelper* s, int* f, const __FlashStringHelper* s2) {
//  display.println(F("Hello, world!"));
  display.print(s);
  if (f) display.print(*f);
  if (s2) display.print(s2);
//  display.println();
}

int16_t flag=0;
void loop() {
  // Delay between measurements.
  delay(delayMS);

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    w1(F("Error T!"));
  }
  else {
    w3(F(""), event.temperature, F("\xF7""C"));//° 100
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    w1(F("Error H!"));
  }
  else {
    w3(flag ? F(".") : F(" "), event.relative_humidity, F("%"));
  }

//  w3(F(""), ARDUINO, F(""));
  
  flag = flag ^ 1;
//  if (flag) w1(F("."));
  
  display.display();
//  delay(2000);
}
