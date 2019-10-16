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

#ifndef USIUARTX_H
#define USIUARTX_H

// ============================================================================

#include <stdint.h>
#include <avr/io.h>

// ============================================================================

void usiuartx_init(void);

int usiuartx_tx_has_data(void);
void usiuartx_tx_byte(uint8_t);
void usiuartx_tx_string(const char *);
void usiuartx_tx_ln(void);
void usiuartx_tx_stringln(const char *);

void usiuartx_rx_init(void);
int usiuartx_rx_has_data(void);
uint8_t usiuartx_rx_byte(void);

// ============================================================================

#endif
