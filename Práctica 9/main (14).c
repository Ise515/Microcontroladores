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

volatile unsigned int tiempo = 0;
volatile unsigned int contador = 0;

char exec[6];
char voltaje_txt[8];

void Timer0_Init(){
    OPTION_REG = 0x07;
    TMR0 = 178;
    T0IE = 1;
    GIE = 1;
}

void ADC_Init(){
    ADCON0 = 0x41;      // Canal AN0, ADC encendido
    ADCON1 = 0x8E;      // AN0 analógico, Vref=Vdd
}

unsigned int ADC_Read(){
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH << 8) + ADRESL);
}

void __interrupt() ISR(void){
    if(T0IF){
        contador++;

        if(contador > 100){
            tiempo++;
            contador = 0;
        }

        TMR0 = 178;
        T0IF = 0;
    }
}

void main(void){

    float voltaje;
    unsigned int adc;

    Timer0_Init();
    ADC_Init();

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);

    LCD_Clear();

    while(1){

        // Lectura del potenciómetro
        adc = ADC_Read();
        voltaje = (adc * 5.0)/1023.0;

        // Voltaje arriba izquierda
        LCD_Set_Cursor(0,0);
        sprintf(voltaje_txt,"%.2fV",voltaje);
        LCD_putrs(voltaje_txt);

        // Tiempo abajo derecha
        LCD_Set_Cursor(1,11);
        sprintf(exec,"%02u:%02u",tiempo/60,tiempo%60);
        LCD_putrs(exec);

        __delay_ms(100);
    }
}