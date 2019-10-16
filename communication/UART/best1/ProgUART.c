/*****************************************************
This program was produced by the
CodeWizardAVR V2.03.9 Standard

Project : ProgUART
Version : 
Date    : 23.06.2010
Author  : www.GetChip.net
Company : Home
Comments: 

Chip type               : ATtiny13
AVR Core Clock frequency: 9,600000 MHz
Memory model            : Tiny
External RAM size       : 0
Data Stack size         : 16

TxD - Ножка 2 - передатчик 
RxD - Ножка 3 - приемник 
*****************************************************/

#include 

#define f_clk                      9600000     // Частота задающего генератора        
#define N                           8          // Пределитель Timer0
#define Baund_Rate  9600
// Частота работы UART (если прием-передача с ошибками -
// поменяйте на значение больше/меньше на 5%)              
#define Bit_Width (f_clk/(N*Baund_Rate)-1)     // Значение для OCR0A
#define Half_Bit_Width (Bit_Width/2)           // Значение для вычисления OCR0B
#define RxD PinB4                              // Ножка приемник
#define TxD PortB.3                            // Ножка передатчик

// Declare your global variables here
register unsigned char Rx_Count @11;
register unsigned char Rx_Byte @12;
register unsigned char Tx_Count @13;
register unsigned char Tx_Byte @14;



// Pin change interrupt service routine
interrupt [PCINT0] void pin_change_isr(void)
{
// Пришел Старт-бит, инициируем прием кадра

     #asm

        push    r16
        in      r16,0x3F
        push    r16
        ldi     r16,0x00
        out     0x3B,r16
        in      r16,0x32
        subi    r16,0xC2   ;дополнение до 1 Half_Bit_Width
        cpi     r16,0x7C   ;Bit_Width
        brcs    LI000A
 
        subi    r16,0x7C   ;Bit_Width
LI000A:
        out     0x29,r16
        ldi     r16,0x00
        mov     r11,r16
        pop     r16
        out     0x3F,r16
        pop     r16

      #endasm 

}

// Timer 0 output compare A interrupt service routine
interrupt [TIM0_COMPA] void timer0_compa_isr(void)
{
// Побитно передаем байт
      #asm

        push    r16
        push    r17
        in      r16,0x3F
        push    r16
        ldi     r17,0x0A
        cp      r13,r17
        brcc    LA0013
 
        ldi     r17,0x00
        cp      r13,r17
        brne    LA000C
 
        cbi     0x18,0x3
        rjmp    LA0012
 
LA000C:
        bst     r14,0x0
        in      r16,0x18
        bld     r16,0x3
        out     0x18,r16
        sec
        ror     r14
LA0012:
        inc     r13
LA0013:
        pop     r16
        out     0x3F,r16
        pop     r17
        pop     r16
    
    #endasm

}

// Timer 0 output compare B interrupt service routine
interrupt [TIM0_COMPB] void timer0_compb_isr(void)
{
// Обработка принятых бит по линии RxD
      #asm
        push    r16
        push    r17
        in      r16,0x3F
        push    r16
        ldi     r17,0x0A
        cp      r11,r17
        brcc    LB001E
 
        ldi     r17,0x00
        cp      r11,r17
        brne    LB000C
 
        sbic    0x16,0x4
        rjmp    LB0016
 
LB000C:
        ldi     r16,0x08
        cp      r16,r11
        brcs    LB0014
 
        lsr     r12
        in      r16,0x16
        bst     r16,0x4
        bld     r12,0x7
        rjmp    LB001D
 
LB0014:
 
        sbic    0x16,0x4
        rjmp    LB001A
 
LB0016:
        ldi     r16,0x9E
        mov     r12,r16
        ldi     r16,0x09
        mov     r11,r16
LB001A:
        ldi     r16,0x20
        out     0x3A,r16
        out     0x3B,r16
LB001D:
        inc     r11
LB001E:
        pop     r16
        out     0x3F,r16
        pop     r17
        pop     r16
      
    #endasm
}


void putbyte(unsigned char B)
{
// Передача байта по программному UART
// по линии TxD
while (Tx_Count < 10);        // ожидаем когда закончится  передача предыдущего байта
Tx_Byte = B;                  // передаваемый байт
Tx_Count = 0;                // инициируем передачу байта по UART
}

unsigned char getbyte(void)
{
// Прием байта по программному UART
// по линии RxD
if (Rx_Count == 11) return (157);  // Если байт уже раз прочитали
// то получаем 157 (за ненадобностью можно убрать)
while (Rx_Count < 10);     // ожидаем когда закончится прием байта
Rx_Count = 11;              // запрещаем "холостой" прием 
return (Rx_Byte);           // принятый байт
}




void main(void)
{
// Declare your local variables here                                       
unsigned char R_byte;

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port B initialization
// Func5=In Func4=In Func3=Out Func2=In Func1=In Func0=In 
// State5=T State4=P State3=1 State2=T State1=T State0=T 
// PORTB=0x18;
// DDRB=0x08;
DDRB.3=1;
PORTB.3=1;
PORTB.4=1;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 1200,000 kHz
// Mode: CTC top=OCR0A
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x02;
TCCR0B=0x02;
TCNT0=0x00;

OCR0A=Bit_Width;
OCR0B=0x00;

// External Interrupt(s) initialization
// INT0: Off
// Interrupt on any change on pins PCINT0-5: On
GIMSK=0x20;
MCUCR=0x00;
PCMSK=0x10;
GIFR=0x20;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x0C;

// начальные установки
Tx_Count=11;   // нет передачи
Rx_Count=11;   // нет приема

// Global enable interrupts
#asm("sei")

while (1)
      {
      R_byte=getbyte();     // получаем байт из UART
      if ((R_byte!=157) & (R_byte!=158))     // если не ошибка и не повтор - обрабатываем
        {// переводим все буквы нижнего регистра в верхний
        if ((R_byte > 0x60) & (R_byte < 0x7B))
        putbyte(R_byte-32);
        else putbyte(R_byte);
        };
      };
}
