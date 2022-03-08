/*
 * File:   PICP2main.c
 * Author: david
 *
 * Created on 7 de marzo de 2022, 02:15 AM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include "../../../Librerias/SerialPIC.X/I2Cconfiguration.h"
#include "../../../Librerias/conf16f887.X/OSCconfiguration.h"
#include "../../../Librerias/LCDPIC.X/LCDcontrol.h"

//----------------------PROTOTIPOS DE FUNCIONES---------------------------------
void configuracion(void);
//------------------------------------------------------------------------------

//-------------------------VARIABLES GLOBALES-----------------------------------
uint8_t buffer, semaforo;
//------------------------------------------------------------------------------

//---------------------------INTERRUPCION---------------------------------------
void __interrupt()isr(){
    if(PIR1bits.SSPIF){
        //Se mantiene SCL en LOW mientras se prepara la accion
        SSPCONbits.CKP = 0;
        //----------------------RECEPCION---------------------------------------
        
        if((!SSPSTATbits.D_nA)&&(!SSPSTATbits.R_nW)){
            while(!SSPSTATbits.BF);
            //Se limpia el buffer con una lectura
            buffer = SSPBUF;
            //Se limpia el SSPIF
            //PIR1bits.SSPIF = 0;
            //Se libera el SCL
            SSPCONbits.CKP = 1;
        }
        
        if((SSPSTATbits.D_nA)&&(!SSPSTATbits.R_nW)){
            //Se espera a que la transmisión haya terminado
            while(!SSPSTATbits.BF);
            //Se pasa el valor a PORTA
            semaforo = SSPBUF;
            //Se desactiva la bandera
            //PIR1bits.SSPIF = 0;
            //Se libera el SCL
            SSPCONbits.CKP = 1;
        }
        //----------------------------------------------------------------------
        
        //---------------------TRANSMISION--------------------------------------
        //----------------------------------------------------------------------
        
        //------------------------ERROR-----------------------------------------
        //Se entra cuando hay colision u overflow
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            //Se apagan las 'banderas'
            SSPSTATbits.BF=0;
            SSPCONbits.SSPOV = 0;
            SSPCONbits.WCOL = 0;
            
            //Se libera el SCL
            SSPCONbits.CKP = 1;
        }
        //----------------------------------------------------------------------
        PIR1bits.SSPIF = 0;
    }
}
//------------------------------------------------------------------------------

//-------------------------------MAIN-------------------------------------------
void main(void) {
    configuracion();
    while(1){
        switch(semaforo){
            case 1:
                PORTA=0b0001;
                break;
            case 2:
                PORTA=0b0010;
                break;
            case 3:
                PORTA=0b1000;
                break;
            default:
                PORTA=0b1011;
                break;
        }
    }
}
//------------------------------------------------------------------------------

//-----------------------------FUNCIONES----------------------------------------
void configuracion(void){
    //--------------------CONFIGURACION DE I/O----------------------------------
    //Se configuran los I/O como digitales
    ANSELH = 0;
    ANSEL = 0;
    //Se configuran PORTA y PORTC como salida
    TRISC = 0;
    TRISA = 0;
    //Se limpian los puertos
    PORTA = 0;
    PORTC = 0;
    //--------------------------------------------------------------------------
    
    //----------------CONFIGURACION DEL OSCILADOR-------------------------------
    setupOsc(_4MHz,_intOsc);
    //--------------------------------------------------------------------------
    
    //------------------CONFIGURACION DEL 12C-----------------------------------
    initI2C_SLAVE(0x40);
    //--------------------------------------------------------------------------
    
    //---------------CONFIGURACION DE INTERRUPCIONES----------------------------
    //Se habilitan interrupciones globales y perifericas
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    //Se habilitan las interrupciones pertinentes
    SSPenableInterrupt();
    //--------------------------------------------------------------------------
    
    //-----------VALORES INICIALES DE VARIABLES GLOBALES------------------------
    //--------------------------------------------------------------------------
    return;
}
//------------------------------------------------------------------------------