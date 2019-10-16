/**
 * USIUARTX - Tinusaur USIUARTX library for USI as UART in half-duplex mode
 *
 * @created 2015-03-23
 * @author Neven Boyanov
 *
 * This is part of the Tinusaur/USIUARTX project.
 *
 * Copyright (c) 2017 Neven Boyanov, The Tinusaur Team. All Rights Reserved.
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

int main(void) {

	// ---- Initialization ----
    usiuartx_init();
    sei();	// Enable global interrupts
	
	usiuartx_tx_string("hello[usiuartx]\r\n");	// Transmit a string. (This may wait for free space in the buffer)
	
	// Testing for transmitting and receiving characters
	// NOTE: This is half-duplex so it cannot receive and transmit at the same time.
	for (;;) {		
		if (usiuartx_rx_has_data()) {	// Check if there is data in the Rx buffer
			_delay_ms(20);	// Wait a while for all the data to come
			while (usiuartx_rx_has_data()) {	// Check if there is data in the internal buffer
				uint8_t b = usiuartx_rx_byte();	// Receive a byte.
				usiuartx_tx_byte(b);	// Transmit a character.
			}
		}
	}

	// Return the mandatory for the "main" function int value. It is "0" for success.
	return 0;
}

// ============================================================================
