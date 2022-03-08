/* 
 * File:   ADCconfiguration.h
 * Author: david
 *
 * Created on 28 de febrero de 2022, 05:02 PM
 */

#ifndef ADCCONFIGURATION_H
#define	ADCCONFIGURATION_H

//-----------------------------DEFINICIONES-------------------------------------
//Canales analógicos
#define _AN0 0
#define _AN1 1
#define _AN2 2
#define _AN3 3
#define _AN4 4
#define _AN5 5
#define _AN6 6
#define _AN7 7
#define _AN8 8
#define _AN9 9
#define _AN10 10
#define _AN11 11
#define _AN12 12
#define _AN13 13

//Configuración de ADC
#define _rightAlign 1
#define _leftAlign 0
#define _VssRef 0
#define _VddRef 0
#define _VrefPin 1
#define _OnADC 1
#define _OffADC 0
#define _Fosc2 0b00
#define _Fosc8 0b01
#define _Fosc32 0b10
#define _Frc 0b11
//------------------------------------------------------------------------------

//-----------------------PROTOTIPOS DE FUNCIONES--------------------------------
void ADCenableInterrupt(void);
void ADCenableAnalogPort(uint8_t puerto);
void ADCchannelSelect(uint8_t canal);
void ADCstartMeasure(void);
void setupADC(uint8_t Fosc, uint8_t VrefP, uint8_t VrefN, uint8_t channel,
                uint8_t align);
//------------------------------------------------------------------------------

#endif	/* ADCCONFIGURATION_H */

