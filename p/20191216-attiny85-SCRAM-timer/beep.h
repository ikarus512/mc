#include <avr/io.h>
#include <util/delay.h>

#ifdef PIEZO_BIT
	#define PIEZO_PORT PORTB 
	// #define PIEZO_BIT PB3  // pin 1 - piezo - gnd
	// #define PIEZO_BIT2 PB4 // pin 1 - piezo - pin 2
	#define PIEZO_DDR DDRB 

	// #define PIEZO_BEEPER_PIN 3 // Can be 3, 5, 6, 9, 10, 11 on most arduinos
	#define PIEZO_BEEPER_T_MS 150
	// #define PIEZO_BEEPER_PWM_T 20 // (but PWM freq ~= 490Hz always)
	void beep_init() {
		//   pinMode(PIEZO_BEEPER_PIN, OUTPUT);   // Not needed (for analogWrite)
		#ifdef PIEZO_BIT2
			PIEZO_DDR = _BV(PIEZO_BIT2); // set piezo pin as digital output
			PIEZO_PORT &= !_BV(PIEZO_BIT);
			PIEZO_PORT |= _BV(PIEZO_BIT2);
		#else
			PIEZO_DDR = _BV(PIEZO_BIT); // set piezo pin as digital output
		#endif
	}

	void beep() {
		for (uint8_t i = 0; i < 40; i ++) {
			#ifdef PIEZO_BIT2
				PIEZO_PORT ^= _BV(PIEZO_BIT) | _BV(PIEZO_BIT2);
			#else
				PIEZO_PORT ^= _BV(PIEZO_BIT);
			#endif
			_delay_us(2270); // 1/440Hz = 0.00227 seconds
		}
		_delay_ms(PIEZO_BEEPER_T_MS);

	//   analogWrite(PIEZO_BEEPER_PIN, PIEZO_BEEPER_PWM_T);
	//   delay(delayms);
	//   analogWrite(PIEZO_BEEPER_PIN, 0); // 0 - OFF
	//   delay(delayms);
	}

	void beep3() {
		beep();
		beep();
		beep();
	} 
#else
	void beep_init() {
	}

	void beep() {
	}

	void beep3() {
	} 
#endif
