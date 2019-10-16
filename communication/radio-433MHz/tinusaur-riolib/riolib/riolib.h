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

#ifndef RIOLIB_H
#define RIOLIB_H

// ----------------------------------------------------------------------------

#include <stdint.h>
#include <avr/io.h>

// ----------------------------------------------------------------------------

#define RIOLIB_TXDATA_PORT		PB3 // Port for output data
#define RIOLIB_TXPOWR_PORT		PB4 // Port for power on/off

#define RIOLIB_TX_CLOCKLEN	25	// Clock length
#define RIOLIB_TX_CLOCKPF	0	// Clock Power-Factor (0-7)

// ----------------------------------------------------------------------------

#define RIOLIB_RXDATA_PORT		PB3 // Port for input data
#define RIOLIB_RXPOWR_PORT		PB4 // Port for power on/off

#define RIOLIB_RX_CLOCKLEN	25	// Clock length
#define RIOLIB_RX_CLOCKPF	0	// Clock Power-Factor (0-7)

#define RIOLIB_RX_TIMEOUT	10000
#define RIOLIB_RXBUF_MAX	127	// Absolute maximum for the buffer size.

#define RIOLIB_RECVERR_1		-1
#define RIOLIB_RECVERR_2		-2
#define RIOLIB_RECVERR_3		-3
#define RIOLIB_RECVERR_4		-4
#define RIOLIB_RECVERR_5		-5
#define RIOLIB_RECVERR_6		-6
#define RIOLIB_RECVERR_7		-7
#define RIOLIB_RECVERR_8		-8
#define RIOLIB_RECVERR_CHECKSUM	-9

// ----------------------------------------------------------------------------

#define RIOLIB_TXPOWR_ON()	PORTB |= (1 << RIOLIB_TXPOWR_PORT)
#define RIOLIB_TXPOWR_OFF()	PORTB &= ~(1 << RIOLIB_TXPOWR_PORT)

#define RIOLIB_RXPOWR_ON()	PORTB |= (1 << RIOLIB_RXPOWR_PORT)
#define RIOLIB_RXPOWR_OFF()	PORTB &= ~(1 << RIOLIB_RXPOWR_PORT)

// ----------------------------------------------------------------------------

void riolib_txinit(void);
void riolib_send(uint8_t *buffer, uint8_t len);
inline void riolib_sendt(char *text, uint8_t len) {
	riolib_send((uint8_t*)text, len);
};

// ----------------------------------------------------------------------------

void riolib_rxinit(void);
int8_t riolib_recv(uint8_t *buffer, uint8_t len);

// ============================================================================

//
// Length: 6 bytes, or 48 bits - PP-A-DD-C
// PPPPPPPP-PPPPPPPP-TTAAAAAA-DDDDDDDD-DDDDDDDD-CCCCCCCC
// <---preamble----> <-addr-> <-----data------> <-chks->
// Packet type: TT=01 - command (00=reserved, 01=command, 10=sensor, 11=reserved)
// AAAAAA=address, 6-bit (64 values)

#define RIOLIBP_PREAMBLE		0xAAFF		// Preamble. IMPORTANT: This is bytes {0xFF,0xAA}
#define RIOLIBP_PKTYPE_RES00	0b00000000	// Packet type - reserved
#define RIOLIBP_PKTYPE_COMMAND	0b01000000	// Packet type - command
#define RIOLIBP_PKTYPE_SENSOR	0b10000000	// Packet type - sensor
#define RIOLIBP_PKTYPE_RES11	0b11000000	// Packet type - packet
#define RIOLIBP_ADDRMASK		0b00111111	// The lower 6 bits.

// ----------------------------------------------------------------------------

typedef union {
	uint8_t bytes[6];
	struct {
		union {
			uint16_t preamble;
			struct {
				uint8_t preamble1;
				uint8_t preamble2;
			};
		};
		union {
			uint8_t addrdata[3];
			struct {
				uint8_t address;
				union {
					uint16_t data;
					struct {
						uint8_t data1;
						uint8_t data2;
					};
				};
			};
		};
		uint8_t checksum;
	};
} riolibp_packet;

// ----------------------------------------------------------------------------

uint8_t riolibp_mod13(uint8_t);
uint8_t riolibp_adler8(uint8_t *buffer, uint8_t len);

// ----------------------------------------------------------------------------

void riolibp_prep(riolibp_packet *packet, uint8_t address, uint8_t pktype);
void riolibp_send(riolibp_packet *packet, uint8_t data1, uint8_t data2);
int8_t riolibp_recv(riolibp_packet *packet);

// ============================================================================

#endif
