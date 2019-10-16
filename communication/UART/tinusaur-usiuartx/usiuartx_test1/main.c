/**
 * USIUARTX - Tinusaur USIUARTX library for USI as UART in half-duplex mode
 *
 * @author Neven Boyanov
 *
 * This is part of the Tinusaur/USIUARTX project.
 *
 * Copyright (c) 2018 Neven Boyanov, The Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Retain in your source code the link http://tinusaur.org to the Tinusaur project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/usiuartx
 *
 */

// ============================================================================

// #define F_CPU 1000000UL
// NOTE: The F_CPU (CPU frequency) should not be defined in the source code.
//       It should be defined in either (1) Makefile; or (2) in the IDE.

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usiuartx/usiuartx.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                 ATtiny85
//               +----------+   (-)--GND--
//       (RST)---+ PB5  Vcc +---(+)--VCC--
// ---[OWOWOD]---+ PB3  PB2 +---USI-USCK--
// --------------+ PB4  PB1 +---USI-DO->>-
// --------(-)---+ GND  PB0 +---USI-DI-<<-
//               +----------+
//              Tinusaur Board
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ----------------------------------------------------------------------------

char *testing_text = "THE_QUICK_BROWN_FOX_JUMPS_OVER_THE_LAZY_DOG.";
//                       12345678901234567890123456789012345678901234567890
//                       0--------1---------2---------3---------4---------5
//                      "\xFF\xFF\xAA\xAA\x55\x55\xCC\xCC\x33\x33\\HELLO#_";

// ----------------------------------------------------------------------------

int main(void) {

	// ---- Initialization ----
	usiuartx_init();
	sei();	// Enable global interrupts

	// Testing for transmitting multiple characters
	for (;;) {
		char *text = testing_text;
		while (*text) {
			usiuartx_tx_byte(*text++);	// Transmit a character.
			_delay_ms(1);
		}
		usiuartx_tx_string("\r\n");	// Transmit a CR/LF for new line.
		_delay_ms(20);
	}

	// Return the mandatory for the "main" function int value. It is "0" for success.
	return 0;
}

// ============================================================================
