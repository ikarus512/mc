// #define F_CPU 1000000UL
// NOTE: The F_CPU (CPU frequency) should not be defined in the source code.
//       It should be defined in either (1) Makefile; or (2) in the IDE. 
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "cpufreq.h"
//#include "mydelay.h"
//#include "myio.h"
//#include <stdint.h>
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>
#include "usiuartx.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                 ATtiny85
//               +----------+
//       (RST)---+ PB5  Vcc +---(+)--VCC--
//               + PB3  PB2 +---USI USCK
//            ---+ PB4  PB1 +---USI DO ---- [RX]HC06
// --------(-)---+ GND  PB0 +---USI DI ---- [TX]HC06
//               +----------+
//
// ----------------------------------------------------------------------------

#define LED_PORT2 PB3
#define LED_PORT PB2 //PB4

int main(void) {

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

    _delay_ms(1000); // Delay 1000 for DHT, 40 for ssd1306_init
    // ---- Initialization ----
    usiuartx_init();
    sei(); // Enable global interrupts



    // Set the LED port number as output.
    // The DDRB is the data direction for port B.
    // This ...
    //  - shifts the "1" on left to the desired position and ...
    //  - does bitwise "OR" with the value in the port register.
    DDRB |= (1 << LED_PORT);
    DDRB |= (1 << LED_PORT2);


    // ---- Main Loop ----
    for (;;) {

        if (usiuartx_rx_has_data()) { // Check if there is data in the Rx buffer
            _delay_ms(20); // Wait a while for all the data to come
            while (usiuartx_rx_has_data()) { // Check if there is data in the internal buffer
                uint8_t b = usiuartx_rx_byte(); // Receive a byte.

                switch (b) {
                    case '1':
                        // Set the LED bit to "1" - LED will be "on".
                        PORTB |= (1 << LED_PORT);
                        break;
                    case '0':
                        // Set the LED bit to "0" - LED will be "off".
                        PORTB &= ~(1 << LED_PORT);
                        break;
                }
                usiuartx_tx_byte(b); // Transmit a character.
            }
        }

        PORTB ^= (1 << LED_PORT2);
        _delay_ms(1000);
    }

    return 0; // Return the mandatory result value. It is "0" for success.
}
