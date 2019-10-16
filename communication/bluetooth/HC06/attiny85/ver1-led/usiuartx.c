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

// ============================================================================

#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>

// NOTE: Used only during development.
// #include "owowod/owowod.h"
// #include "owowod/debugging.h"

#include "usiuartx.h"

// ============================================================================

// REF: AVR307 Half Duplex UART Using the USI Module on tinyAVR and megaAVR devices

// ----------------------------------------------------------------------------

//********** USI UART Defines **********//

//#define SYSTEM_CLOCK	  14745600
//#define SYSTEM_CLOCK	  11059200
//#define SYSTEM_CLOCK	   8000000
//#define SYSTEM_CLOCK	   7372800
//#define SYSTEM_CLOCK	   3686400
//#define SYSTEM_CLOCK	   2000000
//#define SYSTEM_CLOCK	   1843200
#define SYSTEM_CLOCK	 1000000

//#define BAUDRATE 115200
//#define BAUDRATE	57600
//#define BAUDRATE	28800
//#define BAUDRATE	19200
//#define BAUDRATE	14400
#define BAUDRATE   9600

#define TIMER_PRESCALER	 1
//#define TIMER_PRESCALER  8

// Transmit and Receive buffer size
// Must be power of 2, i.e. 2, 4, 8, etc.
#define USIUARTX_RX_BUFFER_SIZE		   32
#define USIUARTX_TX_BUFFER_SIZE		   32

// ----------------------------------------------------------------------------

//********** USI UART Defines **********//

#define DATA_BITS				  8
#define START_BIT				  1
#define STOP_BIT				  1
#define HALF_FRAME				  5

#define USI_COUNTER_MAX_COUNT	  16
#define USI_COUNTER_SEED_TRANSMIT (USI_COUNTER_MAX_COUNT - HALF_FRAME)
#define INTERRUPT_STARTUP_DELAY	  (0x11 / TIMER_PRESCALER)
// TIMER0_SEED_COMPENSATION: timer0 seed compensation (for interrupt latency).
// Ex.: ESP8266 + ATtiny85/Tinusaur = 7,8 (or 2)
#define TIMER0_SEED_COMPENSATION  2
#define TIMER0_SEED				  (256 - ( (SYSTEM_CLOCK / BAUDRATE) / TIMER_PRESCALER )) + TIMER0_SEED_COMPENSATION

#if ( (( (SYSTEM_CLOCK / BAUDRATE) / TIMER_PRESCALER ) * 3/2) > (256 - INTERRUPT_STARTUP_DELAY) )
#define INITIAL_TIMER0_SEED		  ( 256 - (( (SYSTEM_CLOCK / BAUDRATE) / TIMER_PRESCALER ) * 1/2) )
#define USI_COUNTER_SEED_RECEIVE  ( USI_COUNTER_MAX_COUNT - (START_BIT + DATA_BITS) )
#else
#define INITIAL_TIMER0_SEED		  ( 256 - (( (SYSTEM_CLOCK / BAUDRATE) / TIMER_PRESCALER ) * 3/2) )
#define USI_COUNTER_SEED_RECEIVE  (USI_COUNTER_MAX_COUNT - DATA_BITS)
#endif

#define USIUARTX_RX_BUFFER_MASK ( USIUARTX_RX_BUFFER_SIZE - 1 )
#if ( USIUARTX_RX_BUFFER_SIZE & USIUARTX_RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
#endif

#define USIUARTX_TX_BUFFER_MASK ( USIUARTX_TX_BUFFER_SIZE - 1 )
#if ( USIUARTX_TX_BUFFER_SIZE & USIUARTX_TX_BUFFER_MASK )
#error TX buffer size is not a power of 2
#endif

/* General defines */
#define TRUE					  1
#define FALSE					  0

// ----------------------------------------------------------------------------

//********** Static Variables **********//

register uint8_t usiuartx_tx_data asm("r15");				// Tells the compiler to use Register 15 instead of SRAM

static uint8_t usiuartx_rx_buffer[USIUARTX_RX_BUFFER_SIZE];	// UART transmit buffer. Size is definable in the header file.
static volatile uint8_t usiuartx_rx_head;	// Index pointing at the beginning (the head) of the transmit buffer.
static volatile uint8_t usiuartx_rx_tail;	// Index pointing at the end (the tail) of the transmit buffer.
static uint8_t usiuartx_tx_buffer[USIUARTX_TX_BUFFER_SIZE];	// UART receive buffer. Size is definable in the header file.
static volatile uint8_t usiuartx_tx_head;	// Index pointing at the beginning (the head) of the receive buffer.
static volatile uint8_t usiuartx_tx_tail;	// Index pointing at the end (the tail) of the receive buffer.

static volatile union USI_UART_status {		// Status byte holding flags.
	uint8_t status;
	struct {
		uint8_t ongoing_Transmission_From_Buffer: 1;
		uint8_t ongoing_Transmission_Of_Package: 1;
		uint8_t ongoing_Reception_Of_Package: 1;
		uint8_t reception_Buffer_Overflow: 1;
		uint8_t flag4: 1;
		uint8_t flag5: 1;
		uint8_t flag6: 1;
		uint8_t flag7: 1;
	};
} USI_UART_status = { 0 };

static char *usiuartx_crlf = "\r\n";

// ----------------------------------------------------------------------------

uint8_t usiuartx_bit_reverse(uint8_t);
void usiuartx_rx_init(void);
void usiuartx_tx_init(void);

// ----------------------------------------------------------------------------

//********** USI_UART functions **********//

// Reverses the order of bits in a byte.
// I.e. MSB is swapped with LSB, etc.
uint8_t usiuartx_bit_reverse( uint8_t x ) {
	x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
	x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
	x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
	return x;
}

// Initialize.
void usiuartx_init(void) {
	// Reset the buffers - that is to clear the indexes.
	usiuartx_rx_tail = 0;
	usiuartx_rx_head = 0;
	usiuartx_tx_tail = 0;
	usiuartx_tx_head = 0;

	// Initialization for USI_UART receiver
	usiuartx_rx_init();
}

// ----------------------------------------------------------------------------

// Initialize USI for UART transmission.
void usiuartx_tx_init(void) {
	cli();	// __disable_interrupt();
	TCNT0  = 0x00;	// Counter0 set to 0
	TCCR0B = (1 << PSR0) | (0 << CS02) | (0 << CS01) | (1 << CS00);		 // Reset the pre-scaler and start Timer0.
	TIFR   = (1 << TOV0);										 // Clear Timer0 OVF interrupt flag.
	TIMSK |= (1 << TOIE0);									 // Enable Timer0 OVF interrupt.

	USICR  = (0 << USISIE) | (1 << USIOIE) |						 // Enable USI Counter OVF interrupt.
	         (0 << USIWM1) | (1 << USIWM0) |						 // Select Three Wire mode.
	         (0 << USICS1) | (1 << USICS0) | (0 << USICLK) |			 // Select Timer0 OVER as USI Clock source.
	         (0 << USITC);

	USIDR  = 0xFF;											  // Make sure MSB is '1' before enabling USI_DO.
	USISR  = 0xF0 |											  // Clear all USI interrupt flags.
	         0x0F;											  // Pre-load the USI counter to generate interrupt at first USI clock.
	DDRB  |= (1 << PB1);										 // Configure USI_DO as output.

	USI_UART_status.ongoing_Transmission_From_Buffer = TRUE;

	sei();	// __enable_interrupt();
}

#define USIUARTX_TX_ISEMPTY() (usiuartx_tx_head == usiuartx_tx_tail)
#define USIUARTX_TX_HASDATA() (usiuartx_tx_head != usiuartx_tx_tail)

// Check if there is data in the transmit buffer.
int usiuartx_tx_has_data(void) {
	return (USIUARTX_TX_HASDATA());	// Return 0 (FALSE) if the transmit buffer is empty.
	// return (usiuartx_tx_head != usiuartx_tx_tail);
}

// Puts data in the transmission buffer, after reversing the bits in the byte.
// Initiates the transmission routines if not already started.
void usiuartx_tx_byte(uint8_t data) {
	// DEBUGGING_CHAR(data);
	uint8_t tmp_tx_head;
	tmp_tx_head = ( usiuartx_tx_head + 1 ) & USIUARTX_TX_BUFFER_MASK;	// Calculate buffer index.
	// DEBUGGING_VARU("tmp_tx_head", tmp_tx_head);
	while ( tmp_tx_head == usiuartx_tx_tail );	// Wait for free space in the buffer.
	usiuartx_tx_buffer[tmp_tx_head] = usiuartx_bit_reverse(data);	// Reverse the order of the bits in the data byte and store data in buffer.
	usiuartx_tx_head = tmp_tx_head;	// Store new index.
	if ( !USI_UART_status.ongoing_Transmission_From_Buffer ) {	// Start transmission from buffer (if not already started).
		while ( USI_UART_status.ongoing_Reception_Of_Package );	// Wait for USI to finish reading incoming data.
		usiuartx_tx_init();
	}
}

void usiuartx_tx_string(const char *text) {
	while (*text) {
		usiuartx_tx_byte(*text++);
	}
}

void usiuartx_tx_ln(void) {
	usiuartx_tx_string(usiuartx_crlf);
}

void usiuartx_tx_stringln(const char *text) {
	usiuartx_tx_string(text);
	usiuartx_tx_ln();
}

// ----------------------------------------------------------------------------

// Initialize USI for UART reception.
// Note that this function only enables pinchange interrupt on the USI Data Input pin.
// The USI is configured to read data within the pinchange interrupt.
void usiuartx_rx_init(void) {
	PORTB |=   (1 << PB1) | (1 << PB0);							// Enable pull up on USI DO, DI pins. (USCK/PB2 and PB3 not needed)
	DDRB  &= ~((1 << PB1) | (1 << PB0));							// Set USI DI, DO pins as inputs. (USCK/PB2 and PB3 not needed)
	USICR  =  0;											// Disable USI.
	GIFR   =  (1 << PCIF);									// Clear pin change interrupt flag.
	GIMSK |=  (1 << PCIE);									// Enable pin change interrupt for PCINT[5:0]
	PCMSK |=  (1 << PCINT0);									// Pin Change Mask Register, enable for PCINT0/PB0
}

#define USIUARTX_RX_ISEMPTY() (usiuartx_rx_head == usiuartx_rx_tail)
#define USIUARTX_RX_HASDATA() (usiuartx_rx_head != usiuartx_rx_tail)

// Check if there is data in the receive buffer.
int usiuartx_rx_has_data(void) {
	// return (usiuartx_rx_head != usiuartx_rx_tail);
	return (USIUARTX_RX_HASDATA());	// Return 0 (FALSE) if the receive buffer is empty.
}

// Returns a byte from the receive buffer. Waits if buffer is empty.
uint8_t usiuartx_rx_byte(void) {
	uint8_t tmp_rx_tail;
	while (USIUARTX_RX_ISEMPTY());	// Wait for incoming data
	tmp_rx_tail = ( usiuartx_rx_tail + 1 ) & USIUARTX_RX_BUFFER_MASK;	// Calculate buffer index
	usiuartx_rx_tail = tmp_rx_tail;	// Store new index
	return usiuartx_bit_reverse(usiuartx_rx_buffer[tmp_rx_tail]);	// Reverse the order of the bits in the data byte before it returns data from the buffer.
}

// ----------------------------------------------------------------------------

// ********** Interrupt Handlers ********** //

// The pin change interrupt is used to detect USI_UART reception.
// It is here the USI is configured to sample the UART signal.
// #pragma vector=IO_PINS_vect
// __interrupt void IO_Pin_Change_ISR(void)
ISR(SIG_PIN_CHANGE) {
	if (!( PINB & (1 << PB0) )) {								 // If the USI DI pin is low, then it is likely that it
		// was this pin that generated the pin change interrupt.
		TCNT0  = INTERRUPT_STARTUP_DELAY + INITIAL_TIMER0_SEED;	  // Plant TIMER0 seed to match baudrate (including interrupt start up time.).
		TCCR0B	= (1 << PSR0) | (0 << CS02) | (0 << CS01) | (1 << CS00);		 // Reset the pre-scaler and start Timer0.
		TIFR   = (1 << TOV0);										 // Clear Timer0 OVF interrupt flag.
		TIMSK |= (1 << TOIE0);									 // Enable Timer0 OVF interrupt.

		USICR  = (0 << USISIE) | (1 << USIOIE) |						 // Enable USI Counter OVF interrupt.
		         (0 << USIWM1) | (1 << USIWM0) |						 // Select Three Wire mode.
		         (0 << USICS1) | (1 << USICS0) | (0 << USICLK) |			 // Select Timer0 OVER as USI Clock source.
		         (0 << USITC);
		// Note that enabling the USI will also disable the pin change interrupt.
		USISR  = 0xF0 |											  // Clear all USI interrupt flags.
		         USI_COUNTER_SEED_RECEIVE;						  // Preload the USI counter to generate interrupt.

		GIMSK &=  ~(1 << PCIE);									 // Disable pin change interrupt for PCINT[5:0]

		USI_UART_status.ongoing_Reception_Of_Package = TRUE;
	}
}

// The USI Counter Overflow interrupt is used for moving data between memory and the USI data register.
// The interrupt is used for both transmission and reception.
// #pragma vector=USI_OVF_vect
// __interrupt void USI_Counter_Overflow_ISR(void)
ISR(SIG_USI_OVERFLOW) {
	uint8_t tmp_tx_tail;
	uint8_t tmp_rx_head;

	// Check if we are running in Transmit mode.
	if( USI_UART_status.ongoing_Transmission_From_Buffer ) {
		// If ongoing transmission, then send second half of transmit data.
		if( USI_UART_status.ongoing_Transmission_Of_Package ) {
			USI_UART_status.ongoing_Transmission_Of_Package = FALSE;	// Clear on-going package transmission flag.

			USISR = 0xF0 | (USI_COUNTER_SEED_TRANSMIT);					// Load USI Counter seed and clear all USI flags.
			USIDR = (usiuartx_tx_data << 3) | 0x07;						// Reload the USIDR with the rest of the data and a stop-bit.
		}
		// Else start sending more data or leave transmit mode.
		else {
			// If there is data in the transmit buffer, then send first half of data.
			if ( usiuartx_tx_head != usiuartx_tx_tail ) {
				USI_UART_status.ongoing_Transmission_Of_Package = TRUE; // Set on-going package transmission flag.

				tmp_tx_tail = ( usiuartx_tx_tail + 1 ) & USIUARTX_TX_BUFFER_MASK;	 // Calculate buffer index.
				usiuartx_tx_tail = tmp_tx_tail;										 // Store new index.
				usiuartx_tx_data = usiuartx_tx_buffer[tmp_tx_tail];					 // Read out the data that is to be sent. Note that the data must be bit reversed before sent.
				// The bit reversing is moved to the application section to save time within the interrupt.
				USISR  = 0xF0 | (USI_COUNTER_SEED_TRANSMIT);			// Load USI Counter seed and clear all USI flags.
				USIDR  = (usiuartx_tx_data >> 2) | 0x80;				// Copy (initial high state,) start-bit and 6 LSB of original data (6 MSB
				//	of bit of bit reversed data).
			}
			// Else enter receive mode.
			else {
				USI_UART_status.ongoing_Transmission_From_Buffer = FALSE;

				TCCR0B	= (0 << CS02) | (0 << CS01) | (0 << CS00);				// Stop Timer0.
				PORTB |=   (1 << PB1) | (1 << PB0);							// Enable pull up on USI DO, DI pins. (USCK/PB2 and PB3 not needed)
				DDRB  &= ~((1 << PB1) | (1 << PB0));							// Set USI DI, DO pins as inputs. (USCK/PB2 and PB3 not needed)
				USICR  =  0;											// Disable USI.
				GIFR   =  (1 << PCIF);									// Clear pin change interrupt flag.
				GIMSK |=  (1 << PCIE);									// Enable pin change interrupt for PCINT[5:0]
				PCMSK |=  (1 << PCINT0);									// Pin Change Mask Register, enable for PCINT0/PB0
			}
		}
	}
	// Else running in receive mode.
	else {
		USI_UART_status.ongoing_Reception_Of_Package = FALSE;

		tmp_rx_head = ( usiuartx_rx_head + 1 ) & USIUARTX_RX_BUFFER_MASK;		 // Calculate buffer index.

		if ( tmp_rx_head == usiuartx_rx_tail ) {						// If buffer is full trash data and set buffer full flag.
			USI_UART_status.reception_Buffer_Overflow = TRUE;			// Store status to take actions elsewhere in the application code
		} else {														// If there is space in the buffer then store the data.
			usiuartx_rx_head = tmp_rx_head;								// Store new index.
			usiuartx_rx_buffer[tmp_rx_head] = USIDR;					// Store received data in buffer. Note that the data must be bit reversed before used.
		}																// The bit reversing is moved to the application section to save time within the interrupt.

		TCCR0B	= (0 << CS02) | (0 << CS01) | (0 << CS00);				// Stop Timer0.
		PORTB |=   (1 << PB1) | (1 << PB0);							// Enable pull up on USI DO, DI pins. (USCK/PB2 and PB3 not needed)
		DDRB  &= ~((1 << PB1) | (1 << PB0));							// Set USI DI, DO pins as inputs. (USCK/PB2 and PB3 not needed)
		USICR  =  0;											// Disable USI.
		GIFR   =  (1 << PCIF);									// Clear pin change interrupt flag.
		GIMSK |=  (1 << PCIE);									// Enable pin change interrupt for PCINT[5:0]
		PCMSK |=  (1 << PCINT0);									// Pin Change Mask Register, enable for PCINT0/PB0
	}

}

// Timer0 Overflow interrupt is used to trigger the sampling of signals on the USI ports.
// #pragma vector=TIMER0_OVF0_vect
// __interrupt void Timer0_OVF_ISR(void)
ISR(SIG_OVERFLOW0) {
	TCNT0 += TIMER0_SEED;					// Reload the timer,
	// current count is added for timing correction.
}

// ============================================================================
