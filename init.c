/**
 * @package Caldaia
 * @desc init.c
 * @author WizLab.it
 * @version 20220924.002
 * @processor: PIC16F1825
 */

#include "init.h"


/*==============================================================================
 * System init
 *============================================================================*/
void init(void) {
    /*
    * Init Oscillator: 32MHz
    *  - SPLLEN[7]: 1 (4x PLL enabled)
    *  - IRCF[6-3]: 1110 (8MHz/32MHz)
    *  - SCS[1-0]: 00 (Clock by FOSC [FOSC = INTOSC])
    */
    OSCCON = 0b11110000;
    OSCTUNE = 0x00;

    /*
    * Options Register
    * Timer0 initialized here
    *  - WPUEN[7]: 1 (Weak pull ups disabled)
    *  - INTEDG[6]: 1 (External INT PIN interrupt on rising edge)
    *  - TMR0CS[5]: 0 (Timer0 source clock from internal instruction cycle clock [FOSC/4])
    *  - TMR0SE[4]: 1 (Timer0 increment on high-to-low transaction on INT PIN)
    *  - PSA[3]: 0 (Prescaler assigned to Timer0)
    *  - PS[2-0]: 111 (Prescaler set to 256)
    */
    OPTION_REG = 0b11010111;

    /*
    * PORT A
    *  - RA[7-6]: 00 (Not implemented)
    *  - RA[5]: 0 (Output, led network link)
    *  - RA[4-0]: 1 (Input, Unused)
    */
    TRISA = 0b00011111;
    ANSELA = 0b00000000;
    LATA = 0;

    /*
    * PORT C
    *  - RC[7-6]: 00 (Not implemented)
    *  - RC[5]: 1 (Input, EUSART RX)
    *  - RC[4]: 1 (Input, EUSART TX)
    *  - RC[3]: 1 (Input, Unused)
    *  - RC[2]: 0 (Output, led ring)
    *  - RC[1]: 0 (Output, led caldaia)
    *  - RC[0]: 1 (Input, Unused)
    */
    TRISC = 0b00111001;
    ANSELC = 0b00000000;
    LATC = 0;

    /*
    * Interrupts
    * TMR0IE: 1 (Timer0 interrupt enabled)
    * SSP1 (I2C): 0 (SSP1 interrupt disabled)
    * PEIE: 1 (Peripheral interrupts enabled)
    * GIE: 1 (Global interrupts enabled)
    */
    TMR0IE = 1;
    SSP1IE = 0;
    PEIE = 1;
    GIE = 1;
}


/*==============================================================================
 * Sleep/delay function based on timer
 *============================================================================*/
void sleepMS(uint32_t ms) {
    uint32_t t = MILLISECONDS + ms;
    while(t > MILLISECONDS);
}


/*==============================================================================
 * Interrupt Service Routine
 *============================================================================*/
void __interrupt() isr(void) {
    //Handle Timer0 Interrupt (1 second ticker and MILLISECONDS handler)
    if(TMR0IE && TMR0IF) {
        MILLISECONDS += 8;
        __CLOCK_SCALER_1_SEC++;
        if(__CLOCK_SCALER_1_SEC == _CLOCK_TICKS_PER_SECOND) {
            __CLOCK_SCALER_1_SEC = 0;
            Flags.timer1sec = 1;
        }
        TMR0IF = 0;
        TMR0 = 10;
        return;
    }

    //EUSART RX Interrupt
    if(RCIE && RCIF) {
        EUSART_Interrupt();
    }
}