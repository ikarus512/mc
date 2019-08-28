/* myds18b20.h */
#ifndef __MYDS18B20_H_201409271233__
#define __MYDS18B20_H_201409271233__

#include "mydelay.h"
#include "myio.h"

#define DS18B20_W_PORT      PORTB
#define DS18B20_R_PORT      PINB
#define DS18B20_M_PORT      DDRB
#ifdef MYDS_PIN
#define DS18B20_TEMP_PIN    MYDS_PIN
#else
#define DS18B20_TEMP_PIN    1
#endif


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


typedef struct {
  uint8_t t_int;
  uint8_t t_frac;
} temp_t;
//uint16_t
//uint8_t
temp_t
TempGet(){
  OneWireReset();       // Step 1. Initialization
  OneWireOutByte(0xcc); // Step 2. ROM command: "SKIP ROM".
  OneWireOutByte(0x44); // Step 3. Function command: "Convert T".
    ////Parsite mode: pull up
    //pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN,MYIO_PIN_MODE_OUTPUT);
    //digitalWriteP(DS18B20_W_PORT,DS18B20_TEMP_PIN, MYIO_LOW);
delayMilliseconds(2*750+10); //wait for temperature conversion
//  pinModeP(DS18B20_M_PORT,DS18B20_TEMP_PIN,MYIO_PIN_MODE_INPUT); while(OneWireInByte(DS18B20_TEMP_PIN)) Wait(0);

  OneWireReset();       // Step 1. Initialization
  OneWireOutByte(0xcc); // Step 2. ROM command: "SKIP ROM".
  OneWireOutByte(0xbe); // Step 3. Function command: "Read scratchpad".

#define _VER 4
#if _VER==4
  uint8_t LowByte,HighByte;
  LowByte  = OneWireInByte(DS18B20_TEMP_PIN); //Get LowByte
  HighByte = OneWireInByte(DS18B20_TEMP_PIN); //Get HighByte
  uint16_t t = LowByte|(((uint16_t)HighByte)<<8);
  uint16_t t_int = t>>4; // integer part * 100
  //uint16_t t_frac = (t & (uint16_t)0x000f) * 625 / 100; // fractional part * 10000 / 100
  uint16_t t_frac = ((t>>1) & (uint16_t)0x000f) * 125 / 10; // fractional part * 1000 / 10
  temp_t res;
  res.t_int = t_int;
  res.t_frac = t_frac;
  return res;
#elif _VER==0
  uint8_t LowByte,HighByte;
  LowByte  = OneWireInByte(DS18B20_TEMP_PIN); //Get LowByte
  HighByte = OneWireInByte(DS18B20_TEMP_PIN); //Get HighByte
  uint16_t t = LowByte|(((uint16_t)HighByte)<<8);
  uint16_t t_int = (t>>4)*100; // integer part * 100
  uint16_t t_frac = (t & (uint16_t)0x000f) * 625 / 100; // fractional part * 10000 / 100
  return t_int + t_frac;
#elif _VER==1
  uint8_t LowByte,HighByte;
  LowByte  = OneWireInByte(DS18B20_TEMP_PIN); //Get LowByte
  HighByte = OneWireInByte(DS18B20_TEMP_PIN); //Get HighByte
  return LowByte|(((uint16_t)HighByte)<<8);
#elif _VER==2
  uint8_t LowByte,HighByte;
  LowByte  = OneWireInByte(DS18B20_TEMP_PIN)>>4; //Get LowByte
  HighByte = OneWireInByte(DS18B20_TEMP_PIN)<<4; //Get HighByte
  return LowByte|HighByte;
#else //if _VER==3
  uint16_t
  t = OneWireInByte(DS18B20_TEMP_PIN);
  t |= (uint16_t)OneWireInByte(DS18B20_TEMP_PIN) << 8;
  return ((t >> 4) * 100 + ((t << 12) / 6553) * 10); // Return temperature * 100
#endif
}

#endif /* __MYDS18B20_H_201409271233__ */
