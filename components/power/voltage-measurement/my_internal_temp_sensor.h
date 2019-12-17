// Tested on ATtiny85, ATtyny45, ATtiny25

uint16_t getInternalTemperature100() { // multiplied by 100

    float Vcc_value = 0 /* measured Vcc value */;
LED_STOP();
    ADMUX =
            (0 << ADLAR) | // Right ajusted result
            (0 << REFS2) | // REFS bit 2: internal 1.1v reference
            (1 << REFS1) | // REFS bit 1
            (0 << REFS0) | // REFS bit 0
            (1 << MUX3)  | // MUX bit 3: temperature sensor (ADC4 pin use???)
            (1 << MUX2)  | // MUX bit 2
            (1 << MUX1)  | // MUX bit 1
            (1 << MUX0);   // MUX bit 0

    ADCSRA =
            (0 << ADEN)  |     // Disable ADC 
            0;//(0 << ADPS2) |     // set prescaler to 64, bit 2 
            //(1 << ADPS1) |     // set prescaler to 64, bit 1 
            //(1 << ADPS0);      // set prescaler to 64, bit 0  
              //ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //set division factor-8 for 125kHz ADC clock

    _delay_ms(100); // Delay for settling Vbg

    ADCSRA |= (1 << ADEN); //enables the ADC

    #define M 200
    for (int y=0; y<M; y++) { // Several measurements (drop several first ones, return the last one)
        ADCSRA |= (1 << ADSC); // start ADC measurement
        while (ADCSRA & (1 << ADSC) ); // wait till single conversion complete
        //while (ADCSRA & (1 << ADIF) ); // wait till all conversion complete

        uint16_t ADC_RES_L = 0;
        uint16_t ADC_RES_H = 0;
        ADC_RES_L = ADCL;
        ADC_RES_H = ADCH;
        //Vcc_value = ( 0x400 * 1.1 ) / (ADC_RES_L + ADC_RES_H * 0x100); /* calculate the Vcc value */
        Vcc_value += ADC_RES_L + (ADC_RES_H & 3) * 0x100;
    }

    ADCSRA &= ~(1 << ADEN); //disables the ADC

// -40'C = 230
//  25'C = 300
//  85'C = 370

LED_INIT();
    return ((Vcc_value/M - 300.0)/70.0*60.0 + 25.0)*100.0;
    //return Vcc_value;
}
