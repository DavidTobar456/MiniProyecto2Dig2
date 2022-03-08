/*
 * File:   ADCconfiguration.c
 * Author: david
 *
 * Created on 28 de febrero de 2022, 04:24 PM
 */

#include <xc.h>
#include <stdint.h>
#include "ADCconfiguration.h"
#define _XTAL_FREQ 8000000

void ADCenableInterrupt(void){
    PIE1bits.ADIE=1;
    PIR1bits.ADIF=0;
    return;
}

void ADCenableAnalogPort(uint8_t puerto){
    switch(puerto){
        case 0:
            TRISAbits.TRISA0=1;
            ANSELbits.ANS0=1;
            break;
        case 1:
            TRISAbits.TRISA1=1;
            ANSELbits.ANS1=1;
            break;
        case 2:
            TRISAbits.TRISA2=1;
            ANSELbits.ANS2=1;
            break;
        case 3:
            TRISAbits.TRISA3=1;
            ANSELbits.ANS3=1;
            break;
        case 4:
            TRISAbits.TRISA5=1;
            ANSELbits.ANS4=1;
            break;
        case 5:
            TRISEbits.TRISE0=1;
            ANSELbits.ANS5=1;
            break;
        case 6:
            TRISEbits.TRISE1=1;
            ANSELbits.ANS6=1;
            break;
        case 7:
            TRISEbits.TRISE2=1;
            ANSELbits.ANS7=1;
            break;
        case 8:
            TRISBbits.TRISB2=1;
            ANSELHbits.ANS8=1;
            break;
        case 9:
            TRISBbits.TRISB3=1;
            ANSELHbits.ANS9=1;
            break;
        case 10:
            TRISBbits.TRISB1=1;
            ANSELHbits.ANS10=1;
            break;
        case 11:
            TRISBbits.TRISB4=1;
            ANSELHbits.ANS11=1;
            break;
        case 12:
            TRISBbits.TRISB0=1;
            ANSELHbits.ANS12=1;
            break;
        case 13:
            TRISBbits.TRISB5=1;
            ANSELHbits.ANS13=1;
            break;
        default:
            break;
    }
    return;
}

void ADCchannelSelect(uint8_t canal){
    ADCON0bits.CHS=canal;
    return;
}

void ADCstartMeasure(void){
    ADCON0bits.GO=1;
    return;
}

void setupADC(uint8_t Fosc, uint8_t VrefP, uint8_t VrefN, uint8_t channel,
                uint8_t align){
    ADCON0bits.ADCS=Fosc;
    ADCON1bits.VCFG0=VrefP;
    ADCON1bits.VCFG1=VrefN;
    ADCON0bits.CHS=channel;
    __delay_us(50);
    ADCON1bits.ADFM=align;
    ADCON0bits.ADON=1;
    PIR1bits.ADIF=0;
    return;
}
