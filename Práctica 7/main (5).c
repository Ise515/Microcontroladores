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
char buffer[16];

void ADC_Init(void){
    ANSEL = 0x01;      
    ANSELH = 0x00;     

    ADCON0 = 0x81;    
    ADCON1 = 0x80;     
}

unsigned int ADC_Read(void){
    __delay_us(5);

    GO_nDONE = 1;

    while(GO_nDONE);

    return ((ADRESH << 8) + ADRESL);
}

void main(void){

    ADC_Init();

    TRISBbits.TRISB0 = 1;

    OPTION_REG = OPTION_REG & 0b01111111;

    INTEDG = 0; 
    INTF = 0;
    INTE = 1;
    GIE = 1;

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);

    unsigned int adc_result;
    unsigned long volt;
    unsigned int porcentaje;

    while(1){

        adc_result = ADC_Read();
        LCD_Clear();
        switch(modo){

            case 0:

                LCD_Set_Cursor(0,0);
                LCD_putrs("Voltaje:");

                volt = ((unsigned long)adc_result * 5000UL) / 1023UL;

                sprintf(buffer,"%lu.%03lu V",
                        volt/1000,
                        volt%1000);

                LCD_Set_Cursor(1,0);
                LCD_putrs(buffer);

                break;

            case 1:

                LCD_Set_Cursor(0,0);
                LCD_putrs("Porcentaje:");

                porcentaje = ((unsigned long)adc_result * 100UL) / 1023UL;

                sprintf(buffer,"%u %%", porcentaje);

                LCD_Set_Cursor(1,0);
                LCD_putrs(buffer);

                break;

            case 2:

                LCD_Set_Cursor(0,0);
                LCD_putrs("Valor ADC:");

                sprintf(buffer,"%u", adc_result);

                LCD_Set_Cursor(1,0);
                LCD_putrs(buffer);

                break;
        }

        __delay_ms(200);
    }
}

void __interrupt() ISR(void){

    if(INTF){
        __delay_ms(50);
        modo++;

        if(modo > 2){
            modo = 0;
        }
        INTF = 0;
    }
}