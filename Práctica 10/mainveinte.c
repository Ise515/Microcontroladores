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

void Timer1_Init(void){
    T1CON = 0b00110001;
    TMR1H = 0xCF;
    TMR1L = 0x2C;
    
    TMR1IF = 0;
    TMR1IE = 1;
    
    PEIE = 1;
    GIE = 1;
}

void __interrupt() ISR(void){
    if(TMR1IF){
        contador++;
        if(contador >= 20){
            tiempo++;
            contador = 0;
        }
        TMR1H = 0xCF;
        TMR1L = 0x2C;
        
        TMR1IF = 0;
    }
}

void main(void){
    Timer1_Init();
    
    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);
    
    LCD_Clear();
    LCD_Set_Cursor(0,0);
    LCD_putrs("Tiempo: ");
    LCD_Set_Cursor(1,0);
    LCD_putrs("00:00");
    
    while(1){
        LCD_Set_Cursor(1,0);
        sprintf(exec, "%02u:%02u", tiempo/60, tiempo%60);
        LCD_putrs(exec);
    }
}

