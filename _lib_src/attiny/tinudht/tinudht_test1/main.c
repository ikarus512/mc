/**
 * TinuDHT - Library for the DHT11 temperature/humidity sensors
 *
 * @author Neven Boyanov
 *
 * This is part of the Tinusaur/TinuDHT project.
 *
 * Copyright (c) 2018 Neven Boyanov, The Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Retain in your source code the link http://tinusaur.org to the Tinusaur project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/tinudht
 *
 */

// ============================================================================

// NOTE: About F_CPU - it should be set in either (1) Makefile; or (2) in the IDE.

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "tinudht/tinudht.h"

#include "owowod/owowod.h"
#include "owowod/debugging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                 ATtiny85
//               +----------+   (-)--GND--
//       (RST)---+ PB5  Vcc +---(+)--VCC--
// ---[OWOWOD]---+ PB3  PB2 +-------------
// --------------+ PB4  PB1 +--DHT11DATA--
// --------(-)---+ GND  PB0 +-------------
//               +----------+
//              Tinusaur Board
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ----------------------------------------------------------------------------

#define TINUDHT_PIN PB1

// ----------------------------------------------------------------------------

int main(void) {

	// ---- Initialization ----
	DEBUGGING_INIT();
	DEBUGGING_STRINGLN("HELLO[TinuDHT]");	// Not really needed :)

	_delay_ms(100);	// Delay for DHT11 to stabilize (REQUIRED on power on)

	// ---- Main Loop ----
	uint8_t num = 0;
	for (;;) {
		DEBUGGING_NUMDECUP(num++);
		DEBUGGING_STRING("> ");	// Print a counter
		// Retrieve the data from the DHT11
		TinuDHT tinudht;
		uint8_t tinudht_result = tinudht_read(&tinudht, TINUDHT_PIN);
		// IMPORTANT: Do not query the DHT11 more often than 1 time per second.
		// Output the result
		if (tinudht_result == TINUDHT_OK) {
			DEBUGGING_VAR("H(%)", tinudht.humidity);
			DEBUGGING_VAR("T(C\xB0)", tinudht.temperature);	// ASCII 0x0B = "°"
		} else {
			DEBUGGING_ERROR(tinudht_result, "ERR");
		}
		DEBUGGING_CRLF();
		_delay_ms(2000);
	}

	return 0; // Return the mandatory result value. It is "0" for success.
}

// ============================================================================
