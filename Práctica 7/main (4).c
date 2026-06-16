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
    ANSEL = 0x01;
    ADCON0 = 0x81;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(){
    __delay_us(5);
    GO_nDONE = 1;
    while(GO_nDONE);
    return((ADRESH<<8)+ADRESL);
}

void main(void){
    ADC_Init();
    
    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);
    
    char buffer[10];
    
    while(1){
        LCD_Clear();
        LCD_Set_Cursor(0,0);
        LCD_putrs("Voltaje: ");
        LCD_Set_Cursor(0,9);
        
        unsigned int adc_result = ADC_Read();
        
        //No optimizado
//        float volt = adc_result * 5.0 / 1023;
//        sprintf(buffer, "%.1f", volt);
//        LCD_putrs(buffer);
        
        unsigned int volt = (adc_result*50000)/1023;
        unsigned int part_int = volt / 10000;
        unsigned int part_dec = volt % 10000;
        sprintf(buffer,"%u.%u", part_int, part_dec);
        LCD_putrs(buffer);
        
        __delay_ms(200);
        
    }
}