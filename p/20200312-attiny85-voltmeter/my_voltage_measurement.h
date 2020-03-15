// Tested on ATtiny85, ATtyny45, ATtiny25

// // Low battery test:
// while (1) {
//   v = getVoltageMeasurement100();
//   if (v < 2.5 * 100) { // If 3.3v battery shows less than 2.5v
//     alarm("Low battery!!!");
//   }
//   ...
// }

// MUX [3:0] bits:
#if   defined(VOLTMETER_PIN) && (VOLTMETER_PIN == PB5)
    #define MUX30BITS 0b0000 // ADC0 (PB5)
#elif defined(VOLTMETER_PIN) && (VOLTMETER_PIN == PB5)
    #define MUX30BITS 0b0001 // ADC1 (PB2)
#elif defined(VOLTMETER_PIN) && (VOLTMETER_PIN == PB5)
    #define MUX30BITS 0b0010 // ADC2 (PB4)
#elif defined(VOLTMETER_PIN) && (VOLTMETER_PIN == PB5)
    #define MUX30BITS 0b0011 // ADC3 (PB3)
#else
    #define MUX30BITS 0b1100 // Vbg
#endif

void voltmeterPinInit() {
    #if   defined(VOLTMETER_PIN)
	    #pragma message "Init voltmeter pin to VOLTMETER_PIN"
        DDRB &= ~(1 << VOLTMETER_PIN); //set port as input
    #endif
}

uint16_t getVoltageMeasurement100() {

    float Vcc_value = 0 /* measured Vcc value */;

    ADMUX =
            (0 << ADLAR) | // Right ajusted result
            (0 << REFS2) | // REFS bit 2: Vcc used as Reference (disconnected from PB0 AREF)
            (0 << REFS1) | // REFS bit 1
            (0 << REFS0) | // REFS bit 0

            // (1 << MUX3)  | // MUX bit 3: use Vbg for input (no pin)
            // (1 << MUX2)  | // MUX bit 2
            // (0 << MUX1)  | // MUX bit 1
            // (0 << MUX0);   // MUX bit 0
            MUX30BITS;

    ADCSRA =
            (0 << ADEN)  |     // Disable ADC 
            0;//(0 << ADPS2) |     // set prescaler to 64, bit 2 
            //(1 << ADPS1) |     // set prescaler to 64, bit 1 
            //(1 << ADPS0);      // set prescaler to 64, bit 0  
              //ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //set division factor-8 for 125kHz ADC clock

    #if   defined(VOLTMETER_PIN)
        _delay_ms(10); // Delay for settling Vbg
    #else
        _delay_ms(10); // Delay for settling Vbg
    #endif

    ADCSRA |= (1 << ADEN); //enables the ADC

    for (int y=0; y<3; y++) { // Several measurements (drop several first ones, return the last one)
        ADCSRA |= (1 << ADSC); // start ADC measurement
        while (ADCSRA & (1 << ADSC) ); // wait till conversion complete

        uint16_t ADC_RES_L = 0;
        uint16_t ADC_RES_H = 0;
        ADC_RES_L = ADCL;
        ADC_RES_H = ADCH;
        Vcc_value = ( 0x400 * 1.1 ) / (ADC_RES_L + ADC_RES_H * 0x100); /* calculate the Vcc value */
    }

    ADCSRA &= ~(1 << ADEN); //disables the ADC

    return (Vcc_value * 100.0); // Return as int, multiplied by 100
}
