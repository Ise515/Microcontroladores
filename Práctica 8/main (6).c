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

void ADC_Init(){
    ANSEL = 0x03;
    ANSELH = 0;
    ADCON0 = 0x01;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(unsigned char channel){
    __delay_us(5);
    ADCON0 &= 0x83; //ADCON0 = ADCON0 & 0x83
    ADCON0 |= channel << 2; //Sustituir los bits del canal por el seleccionado
    __delay_ms(2);
    GO_nDONE = 1;
    while(GO_nDONE);
    return((ADRESH<<8)+ADRESL);
}

void main(void){
    char buffer1[10];
    char buffer2[10];
    
    ADC_Init();
    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);
    
    while(1){
        LCD_Clear();
        LCD_Set_Cursor(0,0);
        LCD_putrs("Voltaje 1: ");
        LCD_Set_Cursor(0,11);
        unsigned int adc_result1 = ADC_Read(0);
        unsigned int volt1 = (adc_result1 * 50000) / 1023;
        unsigned int part_ent1 = volt1/10000; //Parte entera
        unsigned int part_dec1 = volt1 % 10000; //Parte decimal
        sprintf(buffer1, "%u.%u", part_ent1, part_dec1);
        LCD_putrs(buffer1);
        
        LCD_Set_Cursor(1,0);
        LCD_putrs("Voltaje 2: ");
        LCD_Set_Cursor(1,11);
        unsigned int adc_result2 = ADC_Read(1);
        unsigned int volt2 = (adc_result2 * 50000) / 1023;
        unsigned int part_ent2 = volt2 / 10000; //Parte entera
        unsigned int part_dec2 = volt2 % 10000; //Parte decimal
        sprintf(buffer2, "%u.%u", part_ent2, part_dec2);
        LCD_putrs(buffer2);
        
        __delay_ms(100);
    }
}