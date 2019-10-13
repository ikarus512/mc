//                 ATtiny85
//               +----------+
//       (RST)---+ PB5  Vcc +--- (+)
// ---[OWOWOD]---+ PB3  PB2 +--- TX (HC06)
// --------------+ PB4  PB1 +--- RX (HC06)
// --------(-)---+ GND  PB0 +--- led
//               +----------+
//
//
//
// ----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#define Led   0
volatile unsigned char buf;
//*********************//
ISR(USART_RXC_vect) 
{
    buf = UDR;   // сохраняем принятые данные
}

//********************//
int main(void)
{
    UBRRL = 103;   //9600
    UCSRB |= (1 << RXCIE)|(1<<TXEN)|(1<<RXEN);
    UCSRC |= (1 << URSEL)|(1 << UCSZ1)|(1 << UCSZ0);
//-------------
    DDRB |=(1<<Led ); // порт на выход
    asm("sei"); // разрешаем работу прерываний
    while(1)
    {
        switch (buf)
        {
            case 0x31:  // принята цифра 1
                           PORTB |=( 1<<Led ); // вкл.светодиод
                           UDR = 0x4F; // передаем обратно символ "0"
                           buf = 0;
                           break; 
             case 0x32: // принята цифра 2
                           PORTB &=~( 1<<Led ); // откл. светодиод
                           UDR = 0x46; // передаем обратно символ "F"
                           buf = 0;    
                           break;
             default:
                           break;
        }
    }
}
