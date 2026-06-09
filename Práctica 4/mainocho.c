#include <xc.h>         // Biblioteca principal del compilador XC8

//=============================================================================
// CONFIGURACI N DE BITS DE CONFIGURACI N (FUSES)
//=============================================================================

// Selecci n de oscilador (usar XT si est s usando un cristal de 4 MHz)
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
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

#define _XTAL_FREQ 8000000     // Frecuencia del oscilador (para __delay_ms y __delay_us)

unsigned char Estado1, Estado2, Estado3;

void main (void){
   ANSEL=0;
   ANSELH=0;
   
   OPTION_REG=OPTION_REG & 0b01111111;
   
   TRISB=0xFF;
   TRISD=0;
   
   while(1){
       PORTDbits.RD0=!PORTBbits.RB0;
       PORTDbits.RD1=!PORTBbits.RB1;
       PORTDbits.RD2=!PORTBbits.RB2;
   }
}

