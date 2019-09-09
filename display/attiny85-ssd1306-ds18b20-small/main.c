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
#include <avr/io.h>
#include <util/delay.h>

#include "tinyavrlib/cpufreq.h"

#include "ssd1306xled/ssd1306xled.h"
#include "ssd1306xled/ssd1306xledtx.h"
#include "ssd1306xled/font6x8.h"

//#include "tinyavrlib/num2str.h"
//#include "tinudht/tinudht.h"
// #define TINUDHT_PIN PB3

#include "tinyavrlib/num2str.c"
#include "ssd1306xled/ssd1306xled.c"
#include "ssd1306xled/ssd1306xledtx.c"


#define USE_MY_DS_SRC
#ifdef USE_MY_DS_SRC
  #include "mydelay.h"
  #include "myio.h"
  #define MYDS_PIN           1 // =PB1
  #include "myds18b20.h"
#else
  #include "ds/ds18b20.h"
  #include "ds/ds18b20.c"
  #include "ds/onewire.h"
  #include "ds/onewire.c"
  #define DS18B20_PIN     PB1
#endif

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                 ATtiny85
//               +----------+   (-)--GND--
//       (RST)---+.PB5  Vcc +---(+)--VCC--
// ---[OWOWOD]---+ PB3  PB2 +---[TWI/SCL]-
// --------------+ PB4  PB1 +-------------
// --------(-)---+ GND  PB0 +---[TWI/SDA]-
//               +----------+
//
// Display ssd1306:
//  SDA = ATtiny85 PB0
//  SCX = ATtiny85 PB2
//  VDD = ATtiny85 VCC
//  GND = ATtiny85 GND
//
// DS18B20:
//  pin1 = pin3 = GND
//  pin2 = ATtiny PB1
//  pin2 = via R 220 Ohm to VCC 3V (or via R 4.7 kOhm to VCC 5V)
// 
// ----------------------------------------------------------------------------


#define TESTING_DELAY 740

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
	ssd1306tx_init(ssd1306xled_font6x8data, ' ');
	ssd1306_clear(); // Clear the screen.

	/* setup */
        #ifdef USE_MY_DS_SRC
        TempInit();
        #else
	DS18B20_init(DS18B20_PIN);
	#endif

	//_delay_ms(1000); // Delay 1000 for DHT, 40 for ssd1306_init

	uint8_t f = 0;
	// ---- Main Loop ----
	for (;;) {
                f = !f;

//		ssd1306_setpos(0, 0); ssd1306tx_string("          "); // Clear line
#ifndef MY_ONLY_DIGITS
		ssd1306_setpos(0, 0);
		if (f) ssd1306tx_string(": ");
		else   ssd1306tx_string("  ");
#endif

		ssd1306_setpos(6*2, 0);
		#ifdef USE_MY_DS_SRC
		//ssd1306tx_numdec(TempGet());
		temp_t t = TempGet();
		// print integer part of t (0 <= t < 256)
		ssd1306tx_numdec(t.t_int);
		ssd1306_setpos(6*4, 0);
		ssd1306tx_string(".");
		// print fractional part of t (0 <= fr < 100, it consists of 2 digits)
		ssd1306_setpos(6*5, 0);
		if (t.t_frac < 10) { ssd1306tx_char('0'); ssd1306_setpos(6*6, 0); ssd1306tx_char('0' + t.t_frac); }
		else ssd1306tx_numdec(t.t_frac);
		#else
		uint16_t t = DS18B20_read(); ssd1306tx_numdec(t);
		#endif

		ssd1306_setpos(6*7, 0); ssd1306tx_string(" 'C");

		_delay_ms(5000 - TESTING_DELAY);
	}

	return 0; // Return the mandatory result value. It is "0" for success.
}
