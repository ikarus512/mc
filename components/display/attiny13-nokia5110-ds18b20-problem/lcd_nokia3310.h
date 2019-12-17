/* lcd_nokia3310.h */
#ifndef __LCD_NOKIA3310_H_201409271233__
#define __LCD_NOKIA3310_H_201409271233__

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "mydelay.h"
#include "myio.h"

// Tested on ATtiny13A, using 3.6v accumulator

//LCD Philips PCD8544 (Nokia 3310): http://playground.arduino.cc/Code/PCD8544
//DS18B20:  http://sheepdogguides.com/arduino/ar3ne1tt.htm
//          http://playground.arduino.cc/Learning/OneWire

//#ifndef LCDNOKIA3310_W_PORT
    #define LCDNOKIA3310_W_PORT     PORTB
    #define LCDNOKIA3310_R_PORT     PINB
    #define LCDNOKIA3310_M_PORT     DDRB
//#endif
#ifndef LCDNOKIA3310_PIN_SCE
    #define LCDNOKIA3310_PIN_RESET  7//5//7
    #define LCDNOKIA3310_PIN_SCE    6//4//6
    #define LCDNOKIA3310_PIN_DC     5//3//5
    #define LCDNOKIA3310_PIN_SDIN   4//2//4
    #define LCDNOKIA3310_PIN_SCLK   3//1//3
    #define LCDNOKIA3310_PIN_VCC    2 //undefine if pin connects directly to battery
    //#define LCDNOKIA3310_PIN_LIGHT  1 //undefine if the pin will not be used by LCD (can be used by something else)
    #define LCDNOKIA3310_PIN_GND    0 //undefine if pin connects directly to battery
#endif




#define LCDNOKIA3310_COMMAND    0
#define LCDNOKIA3310_DATA       1

#define LCDNOKIA3310_MAX_X      84
#define LCDNOKIA3310_MAX_Y      6 //6 string lines on screen (6*8=48 pixels)




//#define DMAX 7
//#define DMAX 9
#define DMAX 0xF
static const uint8_t PROGMEM ASCII[][5] = { // Array located in program flash
{0x3e, 0x51, 0x49, 0x45, 0x3e}, // 30 0
{0x00, 0x42, 0x7f, 0x40, 0x00}, // 31 1
{0x42, 0x61, 0x51, 0x49, 0x46}, // 32 2
{0x21, 0x41, 0x45, 0x4b, 0x31}, // 33 3
{0x18, 0x14, 0x12, 0x7f, 0x10}, // 34 4
{0x27, 0x45, 0x45, 0x45, 0x39}, // 35 5
{0x3c, 0x4a, 0x49, 0x49, 0x30}, // 36 6
{0x01, 0x71, 0x09, 0x05, 0x03}, // 37 7
{0x36, 0x49, 0x49, 0x49, 0x36}, // 38 8
{0x06, 0x49, 0x49, 0x29, 0x1e}, // 39 9
{0x7e, 0x11, 0x11, 0x11, 0x7e}, // 41 A
{0x7f, 0x49, 0x49, 0x49, 0x36}, // 42 B
{0x3e, 0x41, 0x41, 0x41, 0x22}, // 43 C
{0x7f, 0x41, 0x41, 0x22, 0x1c}, // 44 D
{0x7f, 0x49, 0x49, 0x49, 0x41}, // 45 E
{0x7f, 0x09, 0x09, 0x09, 0x01}, // 46 F
#define SPACE 16
{0x00, 0x00, 0x00, 0x00, 0x00}, // 16 space
};



void LcdWrite(uint8_t dc, uint8_t data) {
    digitalWriteP(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_SCE, MYIO_LOW);
    digitalWriteP(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_DC, dc);
    SHIFTOUT_MSBFIRST(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_SDIN, LCDNOKIA3310_PIN_SCLK, data);
    digitalWriteP(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_SCE, MYIO_HIGH);
}

void LcdCharacter(uint8_t ch) {
    uint8_t i;
    LcdWrite(LCDNOKIA3310_DATA, 0x00);
    for(i=0;i<5;i++) LcdWrite(LCDNOKIA3310_DATA, pgm_read_byte(&ASCII[ch][i]) );
    LcdWrite(LCDNOKIA3310_DATA, 0x00);
}

void LcdStringN(uint8_t* str, uint8_t len) {
    uint8_t i;
    for(i=0;i<len;i++) LcdCharacter(str[i]);
}

#define LcdGotoXY(x,y) LcdWrite(LCDNOKIA3310_COMMAND, 0x40 | ((y)&0xF)); LcdWrite(LCDNOKIA3310_COMMAND, 0x80 | ((x)&0x7F))

void LcdClear(void) {
    uint16_t i;
    for(i=0;i<LCDNOKIA3310_MAX_X*LCDNOKIA3310_MAX_Y;i++) LcdWrite(LCDNOKIA3310_DATA, 0x00);
    LcdGotoXY(0,0);
}

#if defined(LCDNOKIA3310_PIN_LIGHT)
    #define LCDNOKIA3310_LIGHT_ON()  digitalWriteP(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_LIGHT,MYIO_LOW)
    #define LCDNOKIA3310_LIGHT_OFF() digitalWriteP(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_LIGHT,MYIO_HIGH)
#else
    #define LCDNOKIA3310_LIGHT_ON()
    #define LCDNOKIA3310_LIGHT_OFF()
#endif

void LcdInitialise(void) {
    LCDNOKIA3310_M_PORT|=_BV(LCDNOKIA3310_PIN_SCE)|_BV(LCDNOKIA3310_PIN_RESET)|_BV(LCDNOKIA3310_PIN_DC)|_BV(LCDNOKIA3310_PIN_SDIN)|_BV(LCDNOKIA3310_PIN_SCLK);

    #if defined(LCDNOKIA3310_PIN_LIGHT)
        pinModeP(LCDNOKIA3310_M_PORT,LCDNOKIA3310_PIN_LIGHT,MYIO_PIN_MODE_OUTPUT);
        LCDNOKIA3310_LIGHT_OFF();
    #endif
    #if defined(LCDNOKIA3310_PIN_VCC)
        pinModeP(LCDNOKIA3310_M_PORT,LCDNOKIA3310_PIN_VCC,MYIO_PIN_MODE_OUTPUT);
        digitalWriteP(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_VCC, MYIO_HIGH);
    #endif
    #if defined(LCDNOKIA3310_PIN_GND)
        pinModeP(LCDNOKIA3310_M_PORT,LCDNOKIA3310_PIN_GND,MYIO_PIN_MODE_OUTPUT);
        digitalWriteP(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_GND, MYIO_LOW);
    #endif

    digitalWriteP(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_RESET, MYIO_LOW);
    digitalWriteP(LCDNOKIA3310_W_PORT,LCDNOKIA3310_PIN_RESET, MYIO_HIGH);

    LcdWrite(LCDNOKIA3310_COMMAND,0x21); //(H=1) LCD Extended Commands:
    LcdWrite(LCDNOKIA3310_COMMAND,0x28); //  Set LCD Vop (Contrast).        //0xB1
    LcdWrite(LCDNOKIA3310_COMMAND,0x04); //  Set Temp coefficent.           //0x04
    LcdWrite(LCDNOKIA3310_COMMAND,0x04); //  LCD bias mode 1:48.            //0x14  1:48
    LcdWrite(LCDNOKIA3310_COMMAND,0x20); //(H=0) LCD basic instruction set:
    LcdWrite(LCDNOKIA3310_COMMAND,0x0C); //  LCD normal mode
}

#define LcdContrast(c)  //LcdWrite(LCDNOKIA3310_COMMAND,0x21); LcdWrite(LCDNOKIA3310_COMMAND,0x80|((c)&0x7F)); LcdWrite(LCDNOKIA3310_COMMAND,0x20);

#endif /* __LCD_NOKIA3310_H_201409271233__ */
