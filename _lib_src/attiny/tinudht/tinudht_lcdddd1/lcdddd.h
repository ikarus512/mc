/*
 * lcdddd.h
 * LCDDDD - LCD Direct Drawing Driver for PCD8544 based displays
 *
 *  Created on: Mar 22, 2013
 *      Author: neven
 *
 * Source code available at: https://bitbucket.org/boyanov/avr/src/default/lcdddd/
 * Other projects and libraries: https://bitbucket.org/boyanov/avr
 *
 */

#ifndef LCDDDD_H_
#define LCDDDD_H_

// ---------------------------------------------------------------------------

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "lcddddf.h"

// ---------------------------------------------------------------------------
#ifndef LCD_PIN_RESET
#define LCD_PIN_RESET PB0	// RST,	Pin 1 on LCD
#endif
#ifndef LCD_PIN_SCE
#define LCD_PIN_SCE   PB1	// CE,	Pin 2 on LCD
#endif
#ifndef LCD_PIN_DC
#define LCD_PIN_DC    PB2	// DC,	Pin 3 on LCD
#endif
#ifndef LCD_PIN_SDIN
#define LCD_PIN_SDIN  PB3	// DIN,	Pin 4 on LCD
#endif
#ifndef LCD_PIN_SCLK
#define LCD_PIN_SCLK  PB4	// CLK,	Pin 5 on LCD
#endif
// ----------------------	// Vcc,	Pin 6 on LCD
// ----------------------	// Lit,	Pin 7 on LCD
// ----------------------	// GND,	Pin 8 on LCD

// Type of what is sent to the controller - data or commands.
#define LCDDDD_XFERTYPE_COMM	0
#define LCDDDD_XFERTYPE_DATA	1

// Display size is 84 by 48 pixels
#define LCDDDD_MAX_X     84
#define LCDDDD_MAX_Y     48

// ---------------------------------------------------------------------------

/**
 * Send a byte (8 bits) to the controller.
 *
 * This takes a byte (8 bits) and sends it to the LCD
 */
void lcdddd_write_byte(uint8_t byte) {
	// Send the byte
	// shiftOut(LCD_PIN_SDIN, LCD_PIN_SCLK, MSBFIRST, byte);
	// Ref: http://arduino.cc/en/Reference/shiftOut
	PORTB &= ~(1 << LCD_PIN_SCE);	// Clear the bit, CE=enable
	for (uint8_t i = 0; i < 8; i++) {
		if (byte & 0x80) {
			PORTB |= (1 << LCD_PIN_SDIN);	// Set the bit, LCD_PIN_SDIN
		} else {
			PORTB &= ~(1 << LCD_PIN_SDIN);	// Clear the bit, LCD_PIN_SDIN
		}
		PORTB &= ~(1 << LCD_PIN_SCLK);	// Clear the bit, LCD_PIN_SCLK
		byte = byte << 1;
		// _delay_ms(10);	// Do delay, if necessary
		PORTB |= (1 << LCD_PIN_SCLK);	// Set the bit, LCD_PIN_SCLK
	}
	PORTB |= (1 << LCD_PIN_SCE);	// Set the bit, CE=disable
}

// Send command byte (8 bits) to the controller.
void lcdddd_write_command(uint8_t command) {
		PORTB &= ~(1 << LCD_PIN_DC); // Clear the bit
		lcdddd_write_byte(command);
}

// Send data byte (8 bits) to the controller.
void lcdddd_write_data(uint8_t data) {
		PORTB |= (1 << LCD_PIN_DC); // Set the bit
		lcdddd_write_byte(data);
}

#define PCD8544_CMDH1_TCx		0b00000100	// set Temperature Coefficient (TCx)
#define PCD8544_CMDH1_TCx_MASK	0b00000011	// range 0..3

#define PCD8544_CMDH1_BSx		0b00010000	// set Bias System (BSx)
#define PCD8544_CMDH1_BSx_MASK	0b00000111	// range 0..7

#define PCD8544_CMDH1_Vop		0b10000000	// write Vop to register
#define PCD8544_CMDH1_Vop_MASK	0b01111111	// range 0x00..0x7f

/**
 * Initializes output and LCD controller (PCD8544)
 * There are two memory banks in the controller: data and commands.
 */
void lcdddd_init(void) {
	// Configure control pins
	// Ref: http://arduino.cc/en/Reference/pinMode
	DDRB |= (1 << LCD_PIN_RESET);	// Set pin as output, pinMode(LCD_PIN_RESET, OUTPUT);
	DDRB |= (1 << LCD_PIN_SCE);	// Set pin as output, pinMode(LCD_PIN_SCE, OUTPUT);
	DDRB |= (1 << LCD_PIN_DC);	// Set pin as output, pinMode(LCD_PIN_DC, OUTPUT);
	DDRB |= (1 << LCD_PIN_SDIN);	// Set pin as output, pinMode(LCD_PIN_SDIN, OUTPUT);
	DDRB |= (1 << LCD_PIN_SCLK);	// Set pin as output, pinMode(LCD_PIN_SCLK, OUTPUT);

	// Reset the LCD to a known state
	PORTB &= ~(1 << LCD_PIN_RESET);	// Clear the bit, digitalWrite(LCD_PIN_RESET, LOW);
	_delay_ms(100);
	PORTB |= (1 << LCD_PIN_RESET);	// Set the bit, digitalWrite(LCD_PIN_RESET, HIGH);

	lcdddd_write_command(0x21); //Tell LCD that extended commands follow
	lcdddd_write_command(PCD8544_CMDH1_Vop | (PCD8544_CMDH1_Vop_MASK & 0x30)); // Set LCD Vop (Contrast): 0x00..0x7f: Try 0x31 (good @ 3.3V) or 0x3F if your display is too dark
	lcdddd_write_command(PCD8544_CMDH1_TCx | (PCD8544_CMDH1_TCx_MASK & 0x03)); // Set Temp coefficient - 0x00..0x03: Try 0x02 or 0x03
	lcdddd_write_command(PCD8544_CMDH1_BSx | (PCD8544_CMDH1_BSx_MASK & 0x04)); // Set bias mode - 0..7: Try 0x03 or 0x04

	lcdddd_write_command(0x20); //We must send 0x20 before modifying the display control mode
	lcdddd_write_command(0x0C); //Set display control, normal mode. 0x0D for inverse
}

/**
 * Changes current position
 * The position is kept within the LCD controller and could be changed with this function.
 */
void lcdddd_gotoxy(uint8_t x, uint8_t y) {
	lcdddd_write_command(0x80 | x); // X - 0..84
	lcdddd_write_command(0x40 | y); // Y - 0..5	(48 / 8)
}

/**
 * Draw vertical short line.
 * Short line is between 0 and 8 pixels in height.
 */
void lcdddd_vsline(uint8_t h) {
	/*
	uint8_t t[] = {
			0b00000000,	// 0
			0b10000000,	// 1
			0b11000000,	// 2
			0b11100000,	// 3
			0b11110000,	// 4
			0b11111000,	// 5
			0b11111100,	// 6
			0b11111110,	// 7
		//	0b11111111,	// 8
	};
		b = t[h];
	*/
	uint8_t b = 0xff;
	if (h < 8) {
		b = b << (8 - h);
	}
	lcdddd_write_data(b);
}

// TODO: Implement.
void lcdddd_vline(uint8_t x, uint8_t y, uint8_t h);

// TODO: Implement.
void lcdddd_vbar(uint8_t x, uint8_t y, uint8_t h, uint8_t w);

/*
 * Prints one character at the current position.
 * The data for the character dots is taken from the font's array.
 * Each character is 8 bit high. Width depends on font and implementation, usually 5 bits or less.
 * Each character is right-padded with a blank column for better readability.
 */
void lcdddd_print_char(char chr) {
    chr -= 0x20;    // Normalize, convert to offset in the character font.
                    // 0x20 is the ASCII character for Space (' ')
                    // The font table starts with this character
    int offset = (chr << 2) + chr;  // equivalent of chr * 5
	for (uint8_t i = 0; i < 5; i++) {
		lcdddd_write_data(pgm_read_byte(&lcdddd_font5x8[offset + i]));   // TODO: Try: (uint8_t)(offset + i)
	}
	lcdddd_write_data(0x00); // Padding, blank column on the right
}

/*
 * Prints string of characters starting at the current position.
 */
void lcdddd_print_string(char *str) {
	while (*str)
		lcdddd_print_char(*str++);
}

#define USINT2DECASCII_MAX_DIGITS 5

uint8_t lcdddd_usint2decascii(uint16_t num, char* buffer)
{
    const unsigned short powers[] = { 10000u, 1000u, 100u, 10u, 1u }; // The "const unsigned short" combination gives shortest code.
    char digit; // "digit" is stored in a char array, so it should be of type char.
    uint8_t digits = USINT2DECASCII_MAX_DIGITS - 1;
    for (uint8_t pos = 0; pos < 5; pos++) // "pos" is index in array, so should be of type int.
    {
        digit = 0;
        while (num >= powers[pos])
        {
            digit++;
            num -= powers[pos];
        }

        // ---- CHOOSE 1, 2 or 3 ----

        // ADD/REMOVE (1) Fixed width, zero padded result.
//        buffer[pos] = digit + '0';

        // ADD/REMOVE (2) Fixed width, zero padded result, digits offset.
//        buffer[pos] = digit + '0';
        // Note: Determines the offset of the first significant digit.
//        if (digits == -1 && digit != 0) digits = pos;
        // Note: Could be used for variable width, not padded, left aligned result.

        // ADD/REMOVE (3) Fixed width, space (or anything else) padded result, digits offset.
        // Note: Determines the offset of the first significant digit.
        // Note: Could be used for variable width, not padded, left aligned result.
        if (digits == USINT2DECASCII_MAX_DIGITS - 1)
        {
            if (digit == 0)
            {
                if (pos < USINT2DECASCII_MAX_DIGITS - 1)    // Check position, so single "0" will be handled properly.
                    digit = -16;    // Use: "-16" for space (' '), "-3" for dash/minus ('-'), "0" for zero ('0'), etc. ...
            }
            else
            {
                digits = pos;
            }
        }
        buffer[pos] = digit + '0';  // Convert to ASCII
    }
    return digits;
}

void lcdddd_print_dec(uint16_t dec) {
    char buffer[USINT2DECASCII_MAX_DIGITS + 1];
    buffer[USINT2DECASCII_MAX_DIGITS] = '\0';   // Terminate the string.
    uint8_t digits = lcdddd_usint2decascii(dec, buffer);
	lcdddd_print_string(buffer + digits);
}

void lcdddd_print_dec_padded(uint16_t dec) {
    char buffer[USINT2DECASCII_MAX_DIGITS + 1];
    buffer[USINT2DECASCII_MAX_DIGITS] = '\0';   // Terminate the string.
    lcdddd_usint2decascii(dec, buffer);
	lcdddd_print_string(buffer);
}

/*
 * Fills the screen with a byte.
 */
void lcdddd_fill_byte(uint8_t byte) {
	lcdddd_gotoxy(0, 0);
	for (uint16_t i = 0; i < (LCDDDD_MAX_X * LCDDDD_MAX_Y / 8); i++)
		lcdddd_write_data(byte);
	// lcdddd_gotoxy(0, 0);	// Not necessary, after full-cycle the position is back to the upper left corner.
}

/*
 * Clears the screen.
 * Fills out with blanks.
 */
void lcdddd_clear(void) {
	lcdddd_fill_byte(0x00);
}

// ---------------------------------------------------------------------------
//
// Unused functions
//
///*
// * Fills the screen with 2 sequential bytes.
// */
//void lcdddd_fill_byte2(uint8_t byte1, uint8_t byte2) {
//	lcdddd_gotoxy(0, 0);
//	for (int i = 0; i < (LCDDDD_MAX_X * LCDDDD_MAX_Y / 16); i++) {
//		lcdddd_write_data(byte1);
//		lcdddd_write_data(byte2);
//	}
//}
//
///**
// * Fills the screen with data taken from the supplied array.
// */
//void lcdddd_fill_data(uint8_t * data) {
//	lcdddd_gotoxy(0, 0);
//	for (int i = 0; i < (LCDDDD_MAX_X * LCDDDD_MAX_Y / 8); i++) {
//		lcdddd_write_data(pgm_read_byte(&data[i]));
//	}
//}
//
///**
// * Fills the screen with a preset pattern
// * This function is mostly for testing.
// */
//void lcdddd_fill_pattern(void) {
//	static const uint8_t a = 0x11;
//	uint8_t x = a;
//	lcdddd_gotoxy(0, 0);
//	for (int i = 0; i < (LCDDDD_MAX_X * LCDDDD_MAX_Y / 8); i++) {
//		lcdddd_write_data(x);
//		x = x << 1;
//		if (x == 0) x = a;
//	}
//}
//
//void lcdddd_tests(void)
//{
//	lcdddd_clear();
//
//	lcdddd_fill_byte2(0x55, 0xAA);
//	_delay_ms(1000);
//
//	lcdddd_fill_byte2(0x33, 0xCC);
//	_delay_ms(1000);
//
//	lcdddd_fill_byte2(0x77, 0x88);
//	_delay_ms(1000);
//
//	lcdddd_fill_pattern();
//	_delay_ms(1000);
//
//}
//
// ---------------------------------------------------------------------------

#endif /* LCDDDD_H_ */
