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

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "riolib.h"
#include "tinyavrlib/debugpin.h"

// ----------------------------------------------------------------------------

#define	ASM_VOLATILE_NOP()		asm volatile ("nop\n\t" ::)
#define	ASM_VOLATILE_NOP_X2()	asm volatile ("nop\n\t" "nop\n\t" ::)

// ----------------------------------------------------------------------------

inline void riolib_clock_delay(void) {
	for (uint8_t i = RIOLIB_TX_CLOCKLEN; i != 0; i--) ASM_VOLATILE_NOP();
}

inline void riolib_clock_delay0(void) {
	for (uint8_t i = RIOLIB_TX_CLOCKLEN - RIOLIB_TX_CLOCKPF; i != 0; i--) ASM_VOLATILE_NOP();
}

inline void riolib_clock_delay1(void) {
	for (uint8_t i = RIOLIB_TX_CLOCKLEN + RIOLIB_TX_CLOCKPF; i != 0; i--) ASM_VOLATILE_NOP();
}

#define RIOLIB_ADLER8_MOD 13

// Super optimized mod 13
uint8_t riolibp_mod13(uint8_t n) {
	if (n >= (RIOLIB_ADLER8_MOD * RIOLIB_ADLER8_MOD)) n -= (RIOLIB_ADLER8_MOD * RIOLIB_ADLER8_MOD);
	while (n >= RIOLIB_ADLER8_MOD) n -= RIOLIB_ADLER8_MOD;
	return n;
}

// Adler-8 checksum
uint8_t riolibp_adler8(uint8_t *buffer, uint8_t len) {
	uint8_t a = 1;
	uint8_t b = 0;
	for (uint8_t i = 0; i < len; i++) {
		uint8_t c = buffer[i];
		a = riolibp_mod13(a + c);
		b = riolibp_mod13(b + a);
	}
	return (b << 4) + a;
}

// ----------------------------------------------------------------------------

void riolib_txinit(void) {
	DEBUGPIN_INIT();	// DebugPin init
	DDRB |= (1 << RIOLIB_TXDATA_PORT);		// Setup output data pin
	PORTB &= ~(1 << RIOLIB_TXDATA_PORT);	// LO, default
	// DDRB |= (1 << RIOLIB_TXPOWR_PORT);	// Setup the power pin.
	// RIOLIB_TXPOWR_ON();					// Set the power pin to LO, default.
}

void riolib_send(uint8_t *buffer, uint8_t len) {
	riolib_clock_delay();
	// Send the buffer out: loop through the buffer
	for (uint8_t index = 0; index < len; index++) {
		/*DEBUG*/ DEBUGPIN_TICK1();
		uint8_t b = buffer[index];
		// Send the start-condition out
		PORTB |= (1 << RIOLIB_TXDATA_PORT);		// Data:HI
		riolib_clock_delay1();
		PORTB &= ~(1 << RIOLIB_TXDATA_PORT);	// Data:LO
		riolib_clock_delay0();
		// Send 1 byte out: loop through the 8 bits
		for (uint8_t i = 8; i != 0; i--) {
			if (b & 0x01) { // Got "1"
				/*DEBUG*/ DEBUGPIN_HI();
				PORTB &= ~(1 << RIOLIB_TXDATA_PORT);	// Data:LO
				riolib_clock_delay0();
				PORTB |= (1 << RIOLIB_TXDATA_PORT);		// Data:HI
				riolib_clock_delay1();
			} else { // Got "0"
				/*DEBUG*/ DEBUGPIN_LO();
				PORTB |= (1 << RIOLIB_TXDATA_PORT);		// Data:HI
				riolib_clock_delay1();
				PORTB &= ~(1 << RIOLIB_TXDATA_PORT);	// Data:LO
				riolib_clock_delay0();
			}
			b = b >> 1;
		}
		PORTB &= ~(1 << RIOLIB_TXDATA_PORT);	// Data:LO - default
		/*DEBUG*/ DEBUGPIN_LO();	// LO, default
		// Send the stop-condition out
		riolib_clock_delay();
		riolib_clock_delay();	// TODO: Experiment with removing this line.
	}
}

// ----------------------------------------------------------------------------

void riolib_rxinit(void) {
	DEBUGPIN_INIT(); // DebugPin init
	DDRB &= ~(1 << RIOLIB_RXDATA_PORT);		// Setup input data pin.
	// PORTB |= (1 << RIOLIB_RXDATA_PORT);	// Do not use the internal pull-up, makes the receiver data pin unstable.
	// DDRB |= (1 << RIOLIB_RXPOWR_PORT);	// Setup the power pin.
	// RIOLIB_RXPOWR_OFF();					// Set the power pin to LO, default.
}

int8_t riolib_recv(uint8_t *buffer, uint8_t len) {
	DEBUGPIN_TICK3();    // DEBUG: Begin of function
	int8_t index;
	uint16_t timeout;
	uint8_t period;
	// Wait for a long pause at LO - the default status of the line
	if (PINB & (1 << RIOLIB_RXDATA_PORT)) return RIOLIB_RECVERR_1; // ERROR: Line is HI, should be LO (the default status)
	for (timeout = 0; !(PINB & (1 << RIOLIB_RXDATA_PORT)) && timeout < RIOLIB_RX_TIMEOUT; timeout++); // Wait for a long LO period before going to HI
	if (timeout <= RIOLIB_RX_CLOCKLEN * 2) return RIOLIB_RECVERR_2; // ERROR: Timein, LO pulse is too short
	if (timeout >= RIOLIB_RX_TIMEOUT) return RIOLIB_RECVERR_3; // ERROR: Timeout, LO pulse reached timeout length
	DEBUGPIN_TICK1();	// DEBUG: Found begin of transmission.
	// NOTE: At this point the line is HI
	for (index = 0; index < len; index++) {
		// Detect start-condition.
		if (!(PINB & (1 << RIOLIB_RXDATA_PORT))) { // Line is HI, wait to go LO
			for (period = 0; !(PINB & (1 << RIOLIB_RXDATA_PORT)) && period < RIOLIB_RX_CLOCKLEN * 4; period++); // Wait for short period to go LO
			if (period >= RIOLIB_RX_CLOCKLEN * 4) break; // Transmitting stopped, quit collecting bits.
		}
		for (period = 0; PINB & (1 << RIOLIB_RXDATA_PORT) && period < RIOLIB_RX_CLOCKLEN; period++); // Wait for short LO, then goes HI
		if (period >= RIOLIB_RX_CLOCKLEN) return RIOLIB_RECVERR_5;	// ERROR: Pulse too long
		for (period = 0; !(PINB & (1 << RIOLIB_RXDATA_PORT)) && period < RIOLIB_RX_CLOCKLEN; period++); // Wait for short HI, then goes LO
		// if (period >= RIOLIB_RX_CLOCKLEN) return RIOLIB_RECVERR_6;	// ERROR: Pulse too short	**** NOTE: this is normal, i.e. not an error ****
		DEBUGPIN_TICK1();	// DEBUG: Found begin of a byte of data
		// Start receiving the bits
		uint8_t b = 0;
		for (uint8_t i = 8; i != 0; i--) {
			b = b >> 1;
			if (PINB & (1 << RIOLIB_RXDATA_PORT)) { // Got HI, will go LO ==> "0"
				for (period = 0; PINB & (1 << RIOLIB_RXDATA_PORT) && period < RIOLIB_RX_CLOCKLEN; period++); // Wait for short HI, then LO
				DEBUGPIN_LO(); // DEBUG
				for (period = 0; !(PINB & (1 << RIOLIB_RXDATA_PORT)) && period < RIOLIB_RX_CLOCKLEN; period++); // Wait for short LO, then HI
				b &= ~0x80;
			} else { // Got LO, will go HI ==> "1"
				for (period = 0; !(PINB & (1 << RIOLIB_RXDATA_PORT)) && period < RIOLIB_RX_CLOCKLEN; period++); // Wait for short LO, then HI
				DEBUGPIN_HI(); // DEBUG
				for (period = 0; PINB & (1 << RIOLIB_RXDATA_PORT) && period < RIOLIB_RX_CLOCKLEN; period++); // Wait for short HI, then LO
				b |= 0x80;
			}
		}
		buffer[index] = b;
		DEBUGPIN_LO(); // DEBUG, default LO
	}
	return index;	// SUCCESS
}

// ============================================================================

void riolibp_prep(riolibp_packet *packet, uint8_t address, uint8_t pktype) {
	// riolibp_packet packet = { { 0xFF, 0xAA } }; // This works but takes to much space for code ~ 14 more bytes.
	// packet.preamble = *(uint16_t *) (uint8_t[]){ 0xFF, 0xAA }; // This works but takes to much space for code ~ 14 more bytes.
	(*packet).preamble = RIOLIBP_PREAMBLE;
	(*packet).address = (address & RIOLIBP_ADDRMASK) | pktype;
}

void riolibp_send(riolibp_packet *packet, uint8_t data1, uint8_t data2) {
	(*packet).data1 = data1;
	(*packet).data2 = data2;
	uint8_t adler8 = riolibp_adler8((*packet).addrdata, 3);
	(*packet).checksum = adler8;
	riolib_send((*packet).bytes, sizeof(riolibp_packet));
}

// ----------------------------------------------------------------------------

#define RIOLIBP_RXBUF_MAX	6

// ----------------------------------------------------------------------------

int8_t riolibp_recv(riolibp_packet *packet) {
	int8_t received = riolib_recv((*packet).bytes, 6);
	uint8_t adler8 = riolibp_adler8((*packet).addrdata, 3);
	if ((*packet).checksum != adler8) return RIOLIB_RECVERR_CHECKSUM;
	(*packet).address &= RIOLIBP_ADDRMASK;
	return received;
}

// ============================================================================
