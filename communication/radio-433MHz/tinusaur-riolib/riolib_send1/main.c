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

#include "riolib/riolib.h"
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
	riolib_txinit();

	// ---- Testing ----
	// Send some text out
	// Testing chars: '\xFF', '\xAA', '\x55'('U'), '\xCC'(''), '\x5F'('_'), '\x5A'('Z'), '\x5C('\\'), '\x33('3'), ...
	char text[] =
	    "\xFF\xFF\x55\x33" \
	    "1234\\HELLO#_";
	char text_x = '0';
	for (;;) { // Infinite main loop
		for (uint8_t i = 0; i < 10; i++) {
			if (text_x > '9') text_x = '0';
			text[sizeof(text) - 2] = text_x++;
			riolib_sendt(text, sizeof(text) - 1);	// use "size-1" to skip the '\0' terminating the string.
			_delay_ms(1);	// MUST wait a little before sending the data our. TODO: choose random delay.
		}
		_delay_ms(20);	// MUST wait a little before sending the data our. TODO: choose random delay.
	}

	return 0; // Return the mandatory for the "main" function int value. It is "0" for success.
}

// ============================================================================
