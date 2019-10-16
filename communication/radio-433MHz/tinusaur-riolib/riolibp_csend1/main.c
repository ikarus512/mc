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

	// uint8_t b[2] = { 1, 2 };
	// uint16_t w = (uint16_t) b;
	// uint16_t w2 = (uint16_t) (uint8_t[]){ 1, 2 };

	// ---- Testing ----
	riolibp_packet packet;
	uint8_t address = 0;
	for (;;) { // Infinite main loop
		uint16_t rand = ((rand << 6) + (rand << 1) + rand + 59);	// TEST/RANDOM: Pseudo-random number generator
		if (address > 9) address = 0;
		riolibp_prep(&packet, address++, RIOLIBP_PKTYPE_COMMAND);
		uint8_t data1 = (rand >> 2) & 0b0111;	// TEST/RANDOM: Data
		// uint8_t data1 = 0b0001;
		uint8_t data2 = (rand >> 5) & 0b0111;	// TEST/RANDOM: Data
		// uint8_t data2 = 0b0111;
		riolibp_send(&packet, data1, data2);
		_delay_ms(12);	// MUST wait a little before sending the data out. TODO: choose random delay.
	}

	return 0; // Return the mandatory for the "main" function int value. It is "0" for success.
}

// ============================================================================
