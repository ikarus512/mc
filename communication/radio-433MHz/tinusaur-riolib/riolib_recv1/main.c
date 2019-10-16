/**
 * RIOLib - Remote/Radio Input-Output Library for wired and wireless communication.
 *
 * @author Neven Boyanov
 *
 * This is part of the Tinusaur/RIOLib project.
 *
 * Copyright (c) 2018 Neven Boyanov, The Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Retain in your source code the link http://tinusaur.org to the Tinusaur project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/riolib
 *
 */

// ============================================================================

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "riolib/riolib.h"
#include "usiuartx/usiuartx.h"
#include "tinyavrlib/debugpin.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~ Shield RIOLib ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                Shield RIOLib
//                   +-------+
//       (RST)-->   /    Vcc +------(+)--VCC---
// ---[Data]--<->--/ PB3 PB2 +-->--[DebugPin]--
// ---------------+ PB4  PB1 +-----------------
// ----GND--(-)---+ GND  PB0 +-----------------
//                +----------+
//                  Tinusaur
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ----------------------------------------------------------------------------

int main(void) {
	// ---- Initialization ----
	DEBUGPIN_INIT();	// DebugPin init
	riolib_rxinit();
	usiuartx_init();
	sei();	// Enable global interrupts

	usiuartx_tx_string("HELLO[RIOLib]\r\n");

	// ---- Testing ----
	char receive_buffer[RIOLIB_RXBUF_MAX / 4];
	for (;;) { // Infinite main loop

		// Receive the text
		int8_t received = riolib_recv(receive_buffer, sizeof(receive_buffer) - 1);
		// if (received == RIOLIB_RECVERR_1) continue;
		// if (received == RIOLIB_RECVERR_2) continue;
		if (received >= 0) {
			// usiuartx_tx_byte('*');
			// usiuartx_tx_byte('0' + received);
			receive_buffer[received] = '\x0';	// use this for text content
			usiuartx_tx_string(receive_buffer);
			usiuartx_tx_string("\r\n");
			_delay_ms(10);	// Wait for debug text to go out through the UART
		} else {	// ERROR
			DEBUGPIN_TICK2();    // DEBUG: Error
			// usiuartx_tx_byte('E');
			// usiuartx_tx_byte('0' - received);	// DEBUG: Print the error code
			_delay_ms(1);	// Wait for debug text to go out through the UART
		}
		// if (received < 0) DEBUGGING_VAR("r", received);
		_delay_ms(1);
	}

	return 0; // Return the mandatory for the "main" function int value. It is "0" for success.
}

// ============================================================================
