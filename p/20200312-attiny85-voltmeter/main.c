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
// Display pinout.   ATtiny85 -- SSD1306
#define SSD1306_SDA		PB0   //- [SDA] pin on the SSD1306 display board
#define SSD1306_SCL		PB1   //- [SCK] pin on the SSD1306 display board
#define SSD1306_VDD		PB2   //- [VDD] pin on the SSD1306 display board
#define SSD1306_GND		PB3   //- [GND] pin on the SSD1306 display board
//
// Voltage measurement pin (V input)
#define VOLTMETER_PIN	PB4   //- (ATtiny85 PB4/ADC2 pin)
//
// =============================================================================

// NOTE: About F_CPU - it should be set in either (1) Makefile; or (2) in the IDE.

#include <stdint.h>
#include <string.h>

#include <avr/io.h>
#include <util/delay.h>

#include "tinyavrlib/cpufreq.h"

//==========================================
// display
#include "ssd1306xled/ssd1306xled.h"
#include "ssd1306xled/ssd1306xledtx.h"

#include "ssd1306xled/font6x8.h"
#define SYMBOL_WIDTH 6
#define SYMBOL_TABLE ssd1306xled_font6x8data
#define DRAW_STR(x, y, str) { ssd1306_setpos(SYMBOL_WIDTH * x, y); ssd1306tx_string(str); }

// #include "ssd1306xled/font8x16.h"
// #define SYMBOL_WIDTH 8
// #define SYMBOL_TABLE ssd1306xled_font8x16data
// #define DRAW_STR(x, y, str) ssd1306tx_stringxy(SYMBOL_TABLE, SYMBOL_WIDTH * x, y, str);

#include "ssd1306xled/ssd1306xled.c"
#include "ssd1306xled/ssd1306xledtx.c"
#include "tinyavrlib/num2str.c"

void ssd1306tx_string16x32xy(const uint8_t *fron_src, uint8_t x, uint8_t y, const char s[]);

//==========================================

// Voltmeter
#include "my_voltage_measurement.h"

//==========================================



// #define TESTING_DELAY 200

int main(void) {

	// ---- CPU Frequency Setup ----
	#if F_CPU == 1000000UL
	#pragma message "F_CPU=1MHZ"
		CLKPR_SET(CLKPR_1MHZ);
	#elif F_CPU == 8000000UL
	#pragma message "F_CPU=8MHZ"
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
#define PORT_SET(PORT)	  DDRB |= (1 << PORT); PORTB |= (1 << PORT);
#define PORT_CLEAR(PORT)  DDRB |= (1 << PORT); PORTB &= ~(1 << PORT);
PORT_SET(SSD1306_VDD);
PORT_CLEAR(SSD1306_GND);

	voltmeterPinInit();

	_delay_ms(100); // Delay 1000 for DHT, 40 for ssd1306_init
	ssd1306_init();
	ssd1306tx_init(SYMBOL_TABLE, ' ');
	ssd1306_clear(); // Clear the screen.


	// ---- Main Loop ----
	for (uint8_t i=0; ; i++) {
		DRAW_STR(0, 0, "      ");
  		uint16_t v = getVoltageMeasurement100() * 100;
		char ssec[16]; usint2decascii(v, ssec); char* ssec1 = ssec;// + strlen(ssec) - 2;
		DRAW_STR(0, 0, ssec1);
		_delay_ms(100);

		if (i > 10) { i=0;
			ssd1306_init();
			ssd1306tx_init(SYMBOL_TABLE, ' ');
			ssd1306_clear(); // Clear the screen.
		}

	}

	return 0;
}

void ssd1306tx_string16x32xy(const uint8_t *fron_src, uint8_t x, uint8_t y, const char s[]) {
	uint16_t j, k = 0;
	while (s[k] != '\0') {
		j = s[k] * 16 - (32 * 16); // Convert ASCII code to font data index. NOTE: (x*16) already optimized to (x<<4).
		if (x > 120) {
			x = 0;    // Go to the next line.
			y++;
		}
		ssd1306_setpos(x, y);
		ssd1306_start_data();
		for (uint8_t i = 0; i < 8; i++) {
			ssd1306_data_byte(pgm_read_byte(&fron_src[j + i]));
		}
		ssd1306_stop();
		ssd1306_setpos(x, y + 1);
		ssd1306_start_data();
		for (uint8_t i = 0; i < 8; i++) {
			ssd1306_data_byte(pgm_read_byte(&fron_src[j + i + 8]));
		}
		ssd1306_stop();
		x += 8;
		k++;
	}
}
