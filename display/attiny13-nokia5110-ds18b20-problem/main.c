/*
// ATtiny13A/2313A speed is (4+(20-4)/(4.5-1.8)*(Volts-1.8)) MHz
//   1.2v:    444 444 Hz, 1 cycle per 2250.000000000 ns
//   2.4v:  7 555 556 Hz, 1 cycle per  132.352941176 ns
//   3.6v: 14 666 667 Hz, 1 cycle per   68.181818182 ns
//   4.8v: 21 777 778 Hz, 1 cycle per   45.918367347 ns
*/
#define F_CPU 14666667
#include <avr/io.h>
//#include <util/delay.h>
#include "mydelay.h"
#include "myio.h"
#include "myds18b20.h"
#include "lcd_nokia3310.h"


#define STRLEN 4
static uint8_t s[STRLEN]={0,0,0,SPACE};
static uint8_t t[STRLEN]={0,0,0,SPACE};

void myCounterIncrement() {
    s[2]=(s[2]+1)&0x0F;
    if(s[2]==0) {
        s[1]=(s[1]+1)&0x0F;
        if(s[1]==0) s[0]=(s[0]+1)&0x0F;
    }
}

void myGetTemp() {
    uint16_t temp;
    temp=TempGet();
    t[3]=temp&0xf; temp=temp>>4;
    t[2]=temp&0xf; temp=temp>>4;
    t[1]=temp&0xf; temp=temp>>4;
    t[0]=temp&0xf;
}

int main() {
    LcdInitialise();
    TempInit();
    LcdClear();
    while(1) {
        LcdGotoXY(0,0);
        LcdStringN(s,STRLEN);//time
        LcdStringN(t,STRLEN);//temperature
        //LcdCharacter(SPACE);
        myCounterIncrement();
        myGetTemp();
        delayMilliseconds(100);
    }
    return 0;
}
