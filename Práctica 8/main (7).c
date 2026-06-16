#include <stdio.h>
#include <stdlib.h>
#include <xc.h>         // Biblioteca principal del compilador XC8
#include <stdbool.h>
#include "lcd.h"
//=============================================================================
// CONFIGURACION DE BITS DE CONFIGURACION (FUSES)
//=============================================================================

// Seleccion de oscilador (usar XT si estas usando un cristal de 8 MHz)
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (enabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (disabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection (disabled)
#pragma config WRT = OFF        // Flash Program Memory Write Enable (disabled)
#pragma config CP = OFF         // Flash Program Memory Code Protection (disabled)

//=============================================================================
// DEFINICIONES
//=============================================================================

#define _XTAL_FREQ 8000000      // Frecuencia del oscilador (para __delay_ms y __delay_us)


volatile unsigned char modo = 0;
unsigned char canal = 0;

void ADC_Init(){
    ANSEL = 0x03;
    ANSELH = 0;
    ADCON0 = 0x01;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(unsigned char channel){
    __delay_us(5);
    ADCON0 &= 0x83;
    ADCON0 |= channel << 2;
    __delay_ms(2);
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH<<8)+ADRESL);
}

void __interrupt() ISR(void){
    if(INTF){
        __delay_ms(20);
        if(!RB0){
            modo++;
            if(modo > 2)
                modo = 0;
        }
        INTF = 0;
    }
}

void main(void){
    char buffer[16];

    ANSEL = 0x03;
    ANSELH = 0x00;

    TRISA = 0xFF;
    TRISB = 0xFF;

    OPTION_REG = OPTION_REG & 0b01111111;

    ADC_Init();

    LCD lcd = {&PORTC,2,3,4,5,6,7};
    LCD_Init(lcd);

    GIE = 1;
    INTE = 1;
    INTEDG = 0;

    unsigned char estadoAnteriorRB1 = 1;

    while(1){

        if((RB1 == 0) && (estadoAnteriorRB1 == 1)){
            __delay_ms(20);
            if(RB1 == 0){
                canal ^= 1;
                while(RB1 == 0);
            }
        }

        estadoAnteriorRB1 = RB1;

        unsigned int adc = ADC_Read(canal);

        LCD_Clear();

        LCD_Set_Cursor(0,0);

        if(canal == 0)
            LCD_putrs("Voltaje 1");
        else
            LCD_putrs("Voltaje 2");

        LCD_Set_Cursor(1,0);


        //Voltaje
        if(modo == 0){
            unsigned long volt = ((unsigned long)adc * 5000)/1023;
            sprintf(buffer,
                    "%lu.%03lu V",
                    volt/1000,
                    volt%1000);

            LCD_putrs(buffer);
        }

        //Porcentaje

        else if(modo == 1){
            unsigned int porcentaje =
                    ((unsigned long)adc * 100)/1023;
            sprintf(buffer,"%u %%", porcentaje);
            LCD_putrs(buffer);
        }

        //ADC

        else{
            sprintf(buffer,"ADC=%u", adc);
            LCD_putrs(buffer);
        }
        __delay_ms(100);
    }
}

