/*
 * File:   PIC1P2main.c
 * Author: david
 *
 * Created on 28 de febrero de 2022, 03:58 PM
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
#include "../../../Librerias/SerialPIC.X/USARTconfiguration.h"
#include "../../../Librerias/conf16f887.X/OSCconfiguration.h"
#include "../../../Librerias/LCDPIC.X/LCDcontrol.h"

//----------------------PROTOTIPOS DE FUNCIONES---------------------------------
void configuracion(void);
void separar_numero(int numero, uint8_t *centenas, uint8_t *decenas, uint8_t *unidades);
//------------------------------------------------------------------------------

//-------------------------VARIABLES GLOBALES-----------------------------------
struct variables{
    uint8_t valor;
    uint8_t centenas;
    uint8_t decenas;
    uint8_t unidades;
}temp;

uint8_t semaforo;
//------------------------------------------------------------------------------

//---------------------------INTERRUPCION---------------------------------------
void __interrupt()isr(){
    if(PIR1bits.RCIF){
        //Se lee el valor en la recepción de USART
        //semaforo = RCREG;
        //Se apaga la bandera
        PIR1bits.RCIF = 0;
        //Se imprime el valor en la terminar virtual
        UARTprintCharacter(semaforo);
    }
}
//------------------------------------------------------------------------------

//-------------------------------MAIN-------------------------------------------
void main(void) {
    configuracion();
    while(1){
        semaforo++;
        if(semaforo>3){
            semaforo = 1;
        }
        //I2C_MW();
        //Se inicia la comunicacion
        I2C_SCon();
        //Se envía la dirección
        I2C_MT(0x40);
        //Se carga el valor a mandar
        I2C_MT(semaforo);
        //Se detiene la comunicacion
        I2C_STCon();
        //Se hace un delay
        __delay_ms(50);
        
        //Se inicia la comunicacion
        I2C_SCon();
        //Se envia la dirección del LM75
        I2C_sendAddress(0b1001111,1);
        //Se activa el modo de lectura en maestro
        I2C_MR(&temp.valor, 1);
        //Se detiene la comunicación
        I2C_STCon();
        __delay_ms(50);
        
        //Desglosamos el valor de temperatura en decenas, centenas y unidades
        separar_numero(temp.valor,&temp.centenas,&temp.decenas,&temp.unidades);
        //Imprimimos valores en el LCD
        char tempTxt[] = {temp.centenas+48,temp.decenas+48,temp.unidades+48,'\0'};
        //Seteamos el puntero en la segunda linea
        LCD_clearDisplay();
        LCD_setDDRAM(0);
        LCDprintText("Temp: Semaforo:");
        LCD_setDDRAM(0b1000000);
        LCDprintText(tempTxt);
        LCDprintText("°C ");
        
        switch(semaforo){
            case 1:
                LCDprintText("Rojo");
                break;
            case 2:
                LCDprintText("Amarillo");
                break;
            case 3:
                LCDprintText("Verde");
                break;
            default:
                LCDprintText("ERROR");
                break;
        }
        __delay_ms(400);
        //Se transmite el valor de emperatura al USART
        /*UARTprintText("Temperatura: ");
        UARTprintText(tempTxt);
        UARTprintCharacter(13);*/
    }
}
//------------------------------------------------------------------------------

//-----------------------------FUNCIONES----------------------------------------
void configuracion(void){
    //--------------------CONFIGURACION DE I/O----------------------------------
    //Se configuran los I/O como digitales
    ANSELH = 0;
    ANSEL = 0;
    //Se configuran PORTA, PORTB, PORTC, y PORTD
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    //Se limpian los puertos
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    //--------------------------------------------------------------------------
    
    //----------------CONFIGURACION DEL OSCILADOR-------------------------------
    setupOsc(_4MHz,_intOsc);
    //--------------------------------------------------------------------------
    
    //------------------CONFIGURACION DEL USART---------------------------------
    //Se configura el USART a 115200 de BaudRate
    initUSARTransmitReceive(1,1,8);
    //--------------------------------------------------------------------------
    
    //------------------CONFIGURACION DEL 12C-----------------------------------
    initI2C_MASTER(100000);
    //--------------------------------------------------------------------------
    
    //---------------CONFIGURACION DE INTERRUPCIONES----------------------------
    //Se habilitan interrupciones globales y perifericas
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    //Se habilitan las interrupciones pertinentes
    USART_enableRInt();
    //SSPenableInterrupt();
    //--------------------------------------------------------------------------
    
    //------------------INICIALIZACION DEL LCD----------------------------------
    initLCD();
    //--------------------------------------------------------------------------
    
    //-----------VALORES INICIALES DE VARIABLES GLOBALES------------------------
    //Se escribe el encabezado del LCD
    LCDprintText("Temp: Semaforo:");
    //Valor inicial de temperatura y semaforo
    semaforo = 0;
    temp.valor = 0;
    //--------------------------------------------------------------------------
    return;
}

void separar_numero(int numero, uint8_t *centenas, uint8_t *decenas, uint8_t *unidades){
    int numaux,numaux2;
    numaux = numero%100;
    if(numero == numaux){
        *centenas = 0;
    }else{
        *centenas = (numero - numaux)/100;
    }
    numaux2 = numaux%10;
    if(numaux == numaux2){
        *decenas = 0;
    }else{
        *decenas = (numaux - numaux2)/10;
    }
    *unidades = numaux2;
}
//------------------------------------------------------------------------------