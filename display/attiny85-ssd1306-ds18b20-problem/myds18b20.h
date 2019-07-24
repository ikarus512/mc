/* myds18b20.h */
#ifndef __MYDS18B20_H_201409271233__
#define __MYDS18B20_H_201409271233__

#include "mydelay.h"
#include "myio.h"

#define DS18B20_W_PORT      PORTB
#define DS18B20_R_PORT      PINB
#define DS18B20_M_PORT      DDRB
#define DS18B20_TEMP_PIN    4



void OneWireReset() {
    //During the initialization sequence the bus master transmits (Tx) the reset pulse
    // by pulling the 1-Wire bus low  for a  minimum of 480us:
    pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN,MYIO_PIN_MODE_OUTPUT);
    digitalWriteP(DS18B20_W_PORT,DS18B20_TEMP_PIN, MYIO_LOW);
    delayMicroseconds(500);

    pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN, MYIO_PIN_MODE_INPUT); // Release bus.
    //When the bus is released, the 5k? pullup resistor pulls the 1-Wire bus high.
    //When the DS18B20 detects this rising edge, it waits 15us to 60us
    //and then transmits a presence pulse by pulling the 1-Wire bus low 
    //for 60us to 240us.
    delayMicroseconds(500); //We do not detect DS18B20 presence, but only wait >=60+240us.
}

void OneWireOutByte(uint8_t d) { // output uint8_t d (least sig bit first).
   uint8_t n;

   for(n=8; n!=0; n--)
   {
      if ((d & 0x01) == 1)  // test least sig bit
      {
         pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN, MYIO_PIN_MODE_OUTPUT);
         digitalWriteP(DS18B20_W_PORT,DS18B20_TEMP_PIN, MYIO_LOW);
         delayMicroseconds(5);
         pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN, MYIO_PIN_MODE_INPUT);
         delayMicroseconds(60);
      }
      else
      {
         pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN, MYIO_PIN_MODE_OUTPUT);
         digitalWriteP(DS18B20_W_PORT,DS18B20_TEMP_PIN, MYIO_LOW);
         delayMicroseconds(60);
         pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN, MYIO_PIN_MODE_INPUT);
      }

      d=d>>1; // now the next bit is in the least sig bit position.
   }
}
uint8_t OneWireInByte() // read uint8_t, least sig uint8_t first
{
    uint8_t d, n, b;
    d=0;

    for (n=0; n<8; n++)
    {
        pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN, MYIO_PIN_MODE_OUTPUT);
        digitalWriteP(DS18B20_W_PORT,DS18B20_TEMP_PIN, MYIO_LOW);
        delayMicroseconds(5);
        pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN, MYIO_PIN_MODE_INPUT);
        delayMicroseconds(5);
        b = digitalReadP(DS18B20_R_PORT,DS18B20_TEMP_PIN);
        delayMicroseconds(50);
        d = (d >> 1) | (b<<7); // shift d to right and insert b in most sig bit position
    }
    return(d);
}

void TempInit() {
    pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN,MYIO_PIN_MODE_INPUT);
    digitalWriteP(DS18B20_W_PORT,DS18B20_TEMP_PIN,MYIO_LOW);
}

uint16_t TempGet(){
  uint8_t LowByte,HighByte;

  OneWireReset();       // Step 1. Initialization
  OneWireOutByte(0xcc); // Step 2. ROM command: "SKIP ROM".
  OneWireOutByte(0x44); // Step 3. Function command: "Convert T".
    ////Parsite mode: pull up
    //pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN,MYIO_PIN_MODE_OUTPUT);
    //digitalWriteP(DS18B20_W_PORT,DS18B20_TEMP_PIN, MYIO_LOW);
delayMilliseconds(750+10); //wait for temperature conversion
//  pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN,MYIO_PIN_MODE_INPUT); while(OneWireInByte(DS18B20_TEMP_PIN)) Wait(0);

  OneWireReset();       // Step 1. Initialization
  OneWireOutByte(0xcc); // Step 2. ROM command: "SKIP ROM".
  OneWireOutByte(0xbe); // Step 3. Function command: "Read scratchpad".
  LowByte  = OneWireInByte(DS18B20_TEMP_PIN); //Get LowByte
  HighByte = OneWireInByte(DS18B20_TEMP_PIN); //Get HighByte

  return LowByte|(((uint16_t)HighByte)<<8);
}

#endif /* __MYDS18B20_H_201409271233__ */
