/* myio.h */
#ifndef __MYIO_H_201409271233__
#define __MYIO_H_201409271233__

#include "mydelay.h"

//#define _BV(bit) (1<<(bit))

#define _W_PORT_B     PORTB
#define _R_PORT_B     PINB
#define _M_PORT_B     DDRB

//#define _W_PORT_D     PORTD
//#define _R_PORT_D     PIND
//#define _M_PORT_D     DDRD


#define MYIO_HIGH       0x1
#define MYIO_LOW        0x0

#define MYIO_PIN_MODE_INPUT         0x0
#define MYIO_PIN_MODE_OUTPUT        0x1
#define MYIO_PIN_MODE_INPUT_PULLUP  0x2

#define digitalWriteP(wport,pin,databit)   ((!(databit))?(wport&=~_BV(pin)):(wport|=_BV(pin)))
#define digitalReadP(rport,pin)            (!!(rport&_BV(pin)))
#define pinModeP(mport,pin,mode)           ((!(mode))?(mport&=~_BV(pin)):(mport|=_BV(pin)))
#define SHIFTOUT_MSBFIRST(wport,dataPin,clockPin,val) { /*shiftOut*/ \
    uint8_t i;                                          \
    for (i=0x80; i!=0; i=i>>1)  {                       \
        digitalWriteP(wport,dataPin, val&i);                   \
        digitalWriteP(wport,clockPin, MYIO_HIGH); /*delayMicroseconds(5);*/ \
        digitalWriteP(wport,clockPin, MYIO_LOW);  /*delayMicroseconds(5);*/ \
    }                                                   \
}

#endif /* __MYIO_H_201409271233__ */
