//==============================================================================
//	File: main.c
//	Description: voltmeter
//==============================================================================
//
// 	PROGRAMMING
//
// 	Using Arduino Nano as ISP.
//   Arduino Nano - ATtiny85 - ATtinyBoard:	// ATtiny85 (target):
//   pin Vcc      - Vcc		 -	vcc			//          +----------+
//   pin GND	  - GND		 -	gnd			//   RESET--+ PB5  Vcc +--VCC
//   pin D2 	  - RESET	 -	pin5		//        --+ PB3  PB2 +--SCK
//   pin D3 	  - SCK		 -	pin2		//        --+ PB4  PB1 +--MISO
//   pin D4 	  - MISO	 -	pin1		//     GND--+ GND  PB0 +--MOSI
//   pin D5 	  - MOSI	 -	pin0		//          +----------+
//
//==============================================================================
//
//	PINOUT
//
#define MYPIN0	PB0
#define MYPIN1	PB1
#define MYPIN2	PB2
//
// =============================================================================

// NOTE: About F_CPU - it should be set in either (1) Makefile; or (2) in the IDE.

#include <stdint.h>
#include <string.h>

#include <avr/io.h>
#include <util/delay.h>

#include "tinyavrlib/cpufreq.h"

//==========================================


int main(void) {

	// ---- CPU Frequency Setup ----
	#if F_CPU == 1000000UL
		#pragma message "F_CPU=" XSTR(F_CPU) "HZ"
		CLKPR_SET(CLKPR_1MHZ);
	#elif F_CPU == 8000000UL
		#pragma message "F_CPU=" XSTR(F_CPU) "HZ"
		CLKPR_SET(CLKPR_8MHZ);
	#else
		#pragma message "F_CPU=????"
		#error "CPU frequency should be 1 MHz for DHT usage! (1 or 8 for ssd1306)"
	#endif

// #define SSD1306_VDD //PB2
// #define SSD1306_GND //PB3
// DDRB |= (1 << PORT); //set port as output
// PORTB |= (1 << PORT); // set to 1
// PORTB &= ~(1 << PORT); // clear to 0

#define PIN_AS_OUTPUT(PIN)	DDRB |= (1 << PIN)
#define PIN_SET(PIN)	 	PORTB |= (1 << PIN)
#define PIN_CLEAR(PIN)  	PORTB &= ~(1 << PIN)
//#define PORT_INPUT_PULLUP(PIN) 		DDRB &= ~(1 << PIN); PORTB |= (1 << PIN)

	_delay_ms(100); // Delay 1000 for DHT, 40 for ssd1306_init
	PIN_CLEAR(MYPIN0);
	PIN_CLEAR(MYPIN1);
	PIN_CLEAR(MYPIN2);
	PIN_AS_OUTPUT(MYPIN0);
	PIN_AS_OUTPUT(MYPIN1);
	PIN_AS_OUTPUT(MYPIN2);

	// ---- Main Loop ----
	for (uint8_t i=0; ; i++) {
		//#define GO(PIN)   PIN_SET(PIN); _delay_ms(200); PIN_CLEAR(PIN); _delay_ms(1000);
                #define GO(PIN)   PIN_SET(PIN); _delay_ms(1000); PIN_CLEAR(PIN);
		GO(MYPIN0);
		GO(MYPIN1);
		GO(MYPIN2);
	}

	return 0;
}
