// Installed arduino libraries:
//   Adafruit_SSD1306
//   Adafruit_GFX_Library
//   Adafruit_Sensor
//   OneWire by Paul Stoffregen etc.
//   DallasTemperature (DS18B20)

// Connections:
//   SSD1306 I2C display (pins: GND, VDD, SCX, SDA):
//     SCX pin --- arduino SSL pin (A5 on Arduino Nano)
//     SDA pin --- arduino SDA pin (A4 on Arduino Nano)
//   DS18B20+ temperature sensor (pin1=pin3=gnd in parasite mode, pin2---R4.7Ohm---vcc)
//     data pin --- arduino pin 2 (D2)
//   HC0905F 9mm 5v Electro-Magnetic Sound Generator (beeper):
//     - pin --- arduino GND pin
//     + pin --- arduino pin 9


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
#define SCREEN_WIDTH 128//64//64//128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64//32//64 //32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1//4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);




#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS_1 2
OneWire oneWire_in(ONE_WIRE_BUS_1);
DallasTemperature sensors(&oneWire_in);




#define PIEZO_BEEPER_PIN 9 // Can be 3, 5, 6, 9, 10, 11 on most arduinos
#define PIEZO_BEEPER_T 40
#define PIEZO_BEEPER_PWM_T 20 // (but PWM freq ~= 490Hz always)
void beep_init() {
  pinMode(PIEZO_BEEPER_PIN, OUTPUT);   // Not needed (for analogWrite)
}

void beep3() {
  beep(PIEZO_BEEPER_T);
  beep(PIEZO_BEEPER_T);
  beep(PIEZO_BEEPER_T);
} 

void beep(unsigned char delayms){
  analogWrite(PIEZO_BEEPER_PIN, PIEZO_BEEPER_PWM_T);
  delay(delayms);
  analogWrite(PIEZO_BEEPER_PIN, 0); // 0 - OFF
  delay(delayms);
}




void setup() {
  beep_init();
  beep3();

  // Initialize device.
  delay(500);
  sensors.begin();
  sensors.setResolution(12);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    // Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
}


int16_t flag=0;

void loop() {
  // Delay between measurements 1s
  sensors.requestTemperatures(); // 750 ms delay here (when 12 bit resolution)
  delay(1000-750); // 1s total

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  // Get temperature and print its value.
  uint8_t device_n = sensors.getDeviceCount();
  display.print(device_n); display.println(" devices found.");
  float avg = 0.0f;
  for (uint8_t device_i = 0; device_i < device_n; device_i++) {
    display.print("Device "); display.print(device_i); display.print(": ");
    float t = sensors.getTempCByIndex(device_i);
    avg += t;
    display.print(t);
    display.println(F("\xF7""C"));//°C
  }
  display.print("Avg: ");
  display.print(avg / (float)device_n);
  display.println(F("\xF7""C"));//°C
  flag = flag ^ 1;
  if (flag) display.println(".");

  display.display();
}
