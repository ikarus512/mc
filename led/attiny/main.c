/**
 * The Tinusaur Project
 *
 * Tutorial 001: Blinking LED
 *
 * file: main.c
 * created: 2014-01-04
 *
 **/

#include <avr/io.h>
#include <util/delay.h>
#include "cpufreq.h"

// ====================================
//                ATtiny
//               25/45/85
//              +--------+
//            --+ o  Vcc +------------
//  LED - PB3 --+        +--
//            --+        +--
//  ------------+ GND    +--
//              +--------+
// ====================================

// Define the I/O port to be used for the LED.
// This a number between 0 and 7 that tells which bit to use.
#define LED_PORT PB4

int main(void) {


/*
// ---- CPU Frequency Setup ----
#if F_CPU == 1000000UL
#pragma message "F_CPU=1MHZ"
        CLKPR_SET(CLKPR_1MHZ);
#elif F_CPU == 8000000UL
#pragma message "F_CPU=8MHZ"
        CLKPR_SET(CLKPR_8MHZ);
#else
#pragma message "F_CPU=????"
#error "CPU frequency should be 1 MHz for DHT usage! (1 or 8 for ssd1306)"
#endif
*/




    // Set the LED port number as output.
    // The DDRB is the data direction for port B.
    // This ...
    //  - shifts the "1" on left to the desired position and ...
    //  - does bitwise "OR" with the value in the port register.
    DDRB |= (1 << LED_PORT);

    // Start infinite loop.
    // (this is how most programs work)
    while (1) {

        // Set the LED bit to "1" - LED will be "on".
        PORTB |= (1 << LED_PORT);

        // Wait a little.
        // The delay function simply does N-number of "empty" loops.
        _delay_ms(200);

        // Set the LED bit to "0" - LED will be "off".
        PORTB &= ~(1 << LED_PORT);

        // Wait a little.
        _delay_ms(800);

        // Do it again ...
    }

    // Return the mandatory for the "main" function value.
    return (0);
}
