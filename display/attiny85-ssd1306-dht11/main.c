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

#include "tinyavrlib/num2str.h"
#include "tinudht/tinudht.h"
 #define TINUDHT_PIN PB3

#include "tinyavrlib/num2str.c"
#include "ssd1306xled/ssd1306xled.c"
#include "ssd1306xled/ssd1306xledtx.c"
//#include "tinyavrlib/num2str.c"
#include "tinudht/tinudht.c"


#include "ds/ds18b20.h"
#include "ds/ds18b20.c"
#include "ds/onewire.h"
#include "ds/onewire.c"
#define DS18B20_PIN    4 //PB4

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

int main(void) {

// ---- CPU Frequency Setup ----
#if F_CPU == 1000000UL
#pragma message "F_CPU=1MHZ"
	CLKPR_SET(CLKPR_1MHZ);
//#elif F_CPU == 8000000UL
//#pragma message "F_CPU=8MHZ"
//	CLKPR_SET(CLKPR_8MHZ);
#else
#pragma message "F_CPU=????"
#error "CPU frequency should be 1 MHz for DHT usage! (1 or 8 for ssd1306)"
#endif

	_delay_ms(1000); // Delay 1000 for DHT, 40 for ssd1306_init
	ssd1306_init();
	ssd1306tx_init(ssd1306xled_font6x8data, ' ');
	ssd1306_clear(); // Clear the screen.

	/* setup */
	DS18B20_init(DS18B20_PIN);


	// ---- Main Loop ----
	for (;;) {
		ssd1306_setpos(0, 0); ssd1306tx_string("          "); // Clear line

		// Retrieve the data from the DHT11
		TinuDHT tinudht;
		uint8_t tinudht_result = tinudht_read(&tinudht, TINUDHT_PIN);
		// IMPORTANT: Do not query the DHT11 more often than 1 time per second.
		// Output the result
		if (tinudht_result == TINUDHT_OK) {
			ssd1306_setpos(6*0, 0); ssd1306tx_numdec((uint16_t)tinudht.temperature);
			ssd1306_setpos(6*2, 0); ssd1306tx_string("'C");
			ssd1306_setpos(6*6, 0); ssd1306tx_numdec((uint16_t)tinudht.humidity);
			ssd1306_setpos(6*8, 0); ssd1306tx_string("%");

			//ssd1306_setpos(6*10, 0); ssd1306tx_numdec(tinudht.temperature / 8);
			//ssd1306_setpos(6*12, 0); ssd1306tx_numdec(tinudht.temperature % 8);

			//_delay_ms(220);
			ssd1306_start_command();
			ssd1306_data_byte(0x26); //   Horizontal scroll
			ssd1306_data_byte(0x00); // A
			ssd1306_data_byte(0x01); // B start page
			ssd1306_data_byte(0x07); // C Time interval between each scroll step 2 frames
			ssd1306_data_byte(0x07); // D end page
			ssd1306_data_byte(0x00); // E
			ssd1306_data_byte(0xFF); // F
			ssd1306_data_byte(0x2F); //   Activate scroll
			//ssd1306_stop();
			_delay_ms(10);
			//ssd1306_start_command();
			ssd1306_data_byte(0x2E); // Deactivate scroll
			ssd1306_stop();
			//_delay_ms(220);

			uint8_t t = (56 - tinudht.temperature) / 8;
			uint8_t r = (56 - tinudht.temperature) % 8;
			ssd1306_setpos(0, 1); ssd1306_start_data(); ssd1306_data_byte(0); ssd1306_stop();
			ssd1306_setpos(0, 2); ssd1306_start_data(); ssd1306_data_byte(0); ssd1306_stop();
			ssd1306_setpos(0, 3); ssd1306_start_data(); ssd1306_data_byte(0); ssd1306_stop();
			ssd1306_setpos(0, t); ssd1306_start_data(); ssd1306_data_byte(1 << r); ssd1306_stop();

		} else {
			ssd1306_setpos(0, 0); ssd1306tx_numdec(256-tinudht_result); // - error code negated
uint16_t t = DS18B20_read();
ssd1306_setpos(4*6, 0); ssd1306tx_numdec(t); // - error code negated
		}
		_delay_ms(2000 - TESTING_DELAY);
	}

	return 0; // Return the mandatory result value. It is "0" for success.
}
//		uart_putt(t);
