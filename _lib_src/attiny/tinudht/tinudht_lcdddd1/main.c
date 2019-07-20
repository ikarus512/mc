/*
 * TinuDHT - Tinusaur DHT11 Library
 *
 * @created: 2014-07-08
 * @author: Neven Boyanov
 *
 * Source code available at: https://bitbucket.org/tinusaur/tinudht
 *
 */

// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <avr/io.h>

#define F_CPU 1000000UL
#include <util/delay.h>

#include "lcdddd.h"
#include "lcddddf.h"

//	#define LCD_PIN_RESET PB0	// RST,	Pin 1 on LCD - RESET
//	#define LCD_PIN_SCE   PB1	// CE,	Pin 2 on LCD - Chip Enable
//	#define LCD_PIN_DC    PB2	// DC,	Pin 3 on LCD - Data/Command mode select
//	#define LCD_PIN_SDIN  PB3	// DIN,	Pin 4 on LCD - Serial Data In line
//	#define LCD_PIN_SCLK  PB4	// CLK,	Pin 5 on LCD - Serial Clock line
//	// ----------------------	// Vcc,	Pin 6 on LCD
//	// ----------------------	// Lit,	Pin 7 on LCD
//	// ----------------------	// GND,	Pin 8 on LCD

// NOTE/IMPORTANT: The RESET wire of te LCD module should be connected directly
//                 to the RESET pin of the MCU - pin 1 on the chip.

#include "../tinudht/tinudht.h"

#define TINUDHT_PIN PB0

int main(void) {

	lcdddd_init(); // Initialize the LCD
	lcdddd_clear();

	_delay_ms(100);	// Delay for DHT11 to stabilize (REQUIRED on power on)

	while (1) {

		lcdddd_gotoxy(0, 0);
		lcdddd_print_string("TinuDHT Test");

		// --------------------------------------------------------------------

		// Retrieve the data from the DHT11

		TinuDHT tinudht;
		tinudht_read(&tinudht, TINUDHT_PIN);
		// TODO: Check the result returned from the functions for errors.

		// --------------------------------------------------------------------

		// Output the results to the LCD screen

		lcdddd_gotoxy(0, 4);
		lcdddd_print_string("H:");
		lcdddd_print_dec_padded(tinudht.humidity);
		lcdddd_print_string(" %");

		lcdddd_gotoxy(0, 5);
		lcdddd_print_string("T:");
		lcdddd_print_dec_padded(tinudht.temperature);
		lcdddd_print_string(" C");

		// --------------------------------------------------------------------

		_delay_ms(1000);
		// IMPORTANT: Do not query the DHT11 more often than 1 time per second.
	}

	return 0;
}
