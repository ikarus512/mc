/* mydelay.h */
#ifndef __MYDELAY_H_201409271233__
#define __MYDELAY_H_201409271233__

/*
void delayMicroseconds(uint16_t us){
    if(us == 0){return;}
    //__asm__ __volatile__("1: sbiw %0,1\n brne 1b\n" : "=w" (us) : "0" (us)); //4 cycles per iteration
    //{uint16_t a1=0; uint8_t a2=0; __asm__ __volatile__("1: sbiw %0,1\n   sbiw %1,1\n sbiw %1,1\n sbiw %1,1\n sbiw %1,1\n sbiw %1,1\n subi %2,1\n      brne 1b\n" : "=w"(us), "=w"(a1), "=w"(a2)  : "0"(us), "1"(a1), "2"(a2));} //15 cycles per iteration
    {uint8_t a2=0; __asm__ __volatile__("1: sbiw %0,1\n   subi %1,1\n    brne 1b\n" : "=w"(us),"=w"(a2) : "0"(us),"1"(a2));} //5 cycles per iteration
}
#define delayMilliseconds(ms) {uint16_t i; for(i=0;i<ms;i++)delayMicroseconds(1000);}
*/
#define delayMicroseconds(us) _delay_us(us)
#define delayMilliseconds(ms) _delay_ms(ms)

#endif /* __MYDELAY_H_201409271233__ */
