/**
 * SSD1306xLED - Library for the SSD1306 based OLED/PLED 128x64 displays
 *
 * @author Neven Boyanov
 *
 * This is part of the Tinusaur/SSD1306xLED project.
 *
 * Copyright (c) 2018 Neven Boyanov, The Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Retain in your source code the link http://tinusaur.org to the Tinusaur project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */

// ============================================================================

// NOTE: About F_CPU - it should be set in either (1) Makefile; or (2) in the IDE.

#include <stdint.h>
#include <string.h>

#include <avr/io.h>
#include <util/delay.h>

#include "tinyavrlib/cpufreq.h"

#include "ssd1306xled/ssd1306xled.h"
#include "ssd1306xled/ssd1306xledtx.h"

#define PIEZO_BIT PB3
// #define PIEZO_BIT2 PB4
#include "beep.h"

#define BUTTON_PIN PB4
#include "button.h"

// #include "ssd1306xled/font6x8.h"
// #define SYMBOL_WIDTH 6
// #define SYMBOL_TABLE ssd1306xled_font6x8data
// #define DRAW_STR(x, y, str) { ssd1306_setpos(SYMBOL_WIDTH * x, y); ssd1306tx_string(str); }

#include "ssd1306xled/font8x16.h"
#define SYMBOL_WIDTH 8
#define SYMBOL_TABLE ssd1306xled_font8x16data
#define DRAW_STR(x, y, str) ssd1306tx_stringxy(SYMBOL_TABLE, SYMBOL_WIDTH * x, y, str);

#include "ssd1306xled/ssd1306xled.c"
#include "ssd1306xled/ssd1306xledtx.c"
#include "tinyavrlib/num2str.c"

void ssd1306tx_string16x32xy(const uint8_t *fron_src, uint8_t x, uint8_t y, const char s[]);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                 ATtiny85
//               +----------+   (-)--GND--
//       (RST)---+ PB5  Vcc +---(+)--VCC--
// ---[OWOWOD]---+ PB3  PB2 +---[TWI/SCL]-
// --------------+ PB4  PB1 +-------------
// --------(-)---+ GND  PB0 +---[TWI/SDA]-
//               +----------+
//              Tinusaur Board
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// NOTE: If you want to reassign the SCL and SDA pins and the I2C address
// do that in the library source code and recompile it so it will take affect.


// ----------------------------------------------------------------------------






#define TESTING_DELAY 200

void mainfunc() {
	for (uint8_t time = 0; time < 10; time ++) {

		uint8_t sec = (time <   60) ? time : (time - 60);
		char* delimiter = (time & 0x01) ? ":" : " ";
		char*   min = (time < 1*60) ? "00" :
						(time < 2*60) ? "01" :
						(time < 3*60) ? "02" :
						(time < 4*60) ? "04" :
						(time < 5*60) ? "05" : "--";

		DRAW_STR(0, 0, min);
		DRAW_STR(2, 0, delimiter);
		if (sec > 9) {
			char ssec[16]; usint2decascii(sec, ssec); char* ssec1 = ssec + strlen(ssec) - 2;
			DRAW_STR(3, 0, ssec1);
		} else {
			DRAW_STR(3, 0, "0");
			char ssec[16]; usint2decascii(sec, ssec); char* ssec1 = ssec + strlen(ssec) - 1;
			DRAW_STR(4, 0, ssec1);
		}

		_delay_ms(1000 - TESTING_DELAY);
	}
	beep3();
}

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

	_delay_ms(1000); // Delay 1000 for DHT, 40 for ssd1306_init
	ssd1306_init();
	ssd1306tx_init(SYMBOL_TABLE, ' ');
	ssd1306_clear(); // Clear the screen.
	beep_init();
	button_init(&mainfunc);


	// ---- Main Loop ----
	for (;;) {
		// do nothing
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
