// Tested on ATtiny85, ATtyny45, ATtiny25

// // Low battery test:
// while (1) {
//   v = getVoltageMeasurement100();
//   if (v < 2.5 * 100) { // If 3.3v battery shows less than 2.5v
//     alarm("Low battery!!!");
//   }
//   ...
// }

#define XSTR(x) STR(x)
#define STR(x) #x

// MUX [3:0] bits:
#if   defined(VOLTMETER_PIN) && (VOLTMETER_PIN == PB5)
    #define MUX_BITS 0b0000 // ADC0 (PB5)
#elif defined(VOLTMETER_PIN) && (VOLTMETER_PIN == PB2)
    #define MUX_BITS 0b0001 // ADC1 (PB2)
#elif defined(VOLTMETER_PIN) && (VOLTMETER_PIN == PB4)
    #define MUX_BITS 0b0010 // ADC2 (PB4)
#elif defined(VOLTMETER_PIN) && (VOLTMETER_PIN == PB3)
    #define MUX_BITS 0b0011 // ADC3 (PB3)
#else
    #define MUX_BITS 0b1100 // Vbg
#endif

void voltmeterPinInit() {
    #if   defined(VOLTMETER_PIN)
	    #pragma message "voltmeter pin=PB" XSTR(VOLTMETER_PIN)
	    #pragma message "MUX bit 3: " XSTR(MUX3)
	    #pragma message "MUX bit 2: " XSTR(MUX2)
	    #pragma message "MUX bit 1: " XSTR(MUX1)
	    #pragma message "MUX bit 0: " XSTR(MUX0)
        // DDRB &= ~(1 << VOLTMETER_PIN); //set port as input
    #endif
}

uint16_t getVoltageMeasurement100() {

    float Vcc_value = 0 /* measured Vcc value */;

    ADMUX =
            (0 << ADLAR) | // Right ajusted result

            #define ONE_OVER_1024 0.0009765625

            #define USE_VCC_AS_REF
            #define V_REF_VAL 5.3
            (0 << REFS2) | // REFS bit 2: Vcc used as Reference (disconnected from PB0 AREF)
            (0 << REFS1) | // REFS bit 1
            (0 << REFS0) | // REFS bit 0

            // #define USE_1V1_AS_REF
            // #define V_REF_VAL 1.1
            // (0 << REFS2) | // REFS bit 2: Internal 1.1V Voltage Reference.
            // (1 << REFS1) | // REFS bit 1
            // (0 << REFS0) | // REFS bit 0

            // #define USE_2V6_AS_REF
            // #define V_REF_VAL 2.6
            // (1 << REFS2) | // REFS bit 2: Internal 2.6V Voltage Reference.
            // (1 << REFS1) | // REFS bit 1
            // (0 << REFS0) | // REFS bit 0


            // (1 << MUX3)  | // MUX bit 3: use Vbg for input (no pin)
            // (1 << MUX2)  | // MUX bit 2
            // (0 << MUX1)  | // MUX bit 1
            // (0 << MUX0);   // MUX bit 0
            MUX_BITS;

    ADCSRA =
            (0 << ADEN)  |     // Disable ADC 

            (0 << ADPS2) |     // ADPS bit 2: prescaler=8 (cpu 1MHz div 8 fits [50-200kHz])
            (1 << ADPS1) |     // ADPS bit 1
            (1 << ADPS0) |     // ADPS bit 0

            0;

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
        #if   defined(USE_VCC_AS_REF)
            #pragma message "----- USE_VCC_AS_REF"
            // Vcc_value = (0x400 * V_REF_VAL) / (ADC_RES_L + ADC_RES_H * 0x100); /* calculate the Vcc value */
            Vcc_value = (ADC_RES_L + ADC_RES_H * 0x100) * V_REF_VAL * ONE_OVER_1024;
        #elif defined(USE_1V1_AS_REF)
            #pragma message "----- USE_1V1_AS_REF"
            // Vcc_value = (0x400 * 1.0) / V_REF_VAL * (ADC_RES_L + ADC_RES_H * 0x100); /* calculate the Vcc value */
            Vcc_value = (ADC_RES_L + ADC_RES_H * 0x100) / 100.0;
        #elif defined(USE_2V6_AS_REF)
            #pragma message "----- USE_2V6_AS_REF"
            // Vcc_value = (0x400 * 1.0) / V_REF_VAL * (ADC_RES_L + ADC_RES_H * 0x100); /* calculate the Vcc value */
            Vcc_value = (ADC_RES_L + ADC_RES_H * 0x100) * V_REF_VAL * ONE_OVER_1024;
        #else
            Vcc_value = 0.0;
        #endif
    }

    ADCSRA &= ~(1 << ADEN); //disables the ADC

    return (Vcc_value * 100.0); // Return as int, multiplied by 100
}
