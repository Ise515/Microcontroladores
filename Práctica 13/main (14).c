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
    ANSELH = 0x00;
    TRISA0 = 1;
    ADCON0 = 0x01;
    ADCON1 = 0x80;
}
unsigned int ADC_Read(){
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH<<8) + ADRESL);
}

void PWM_Init(){
    TRISC2 = 0;            // RC2 como salida PWM
    PR2 = 124; 
    CCP1CON = 0b00001100;  // CCP1 modo PWM
    T2CON = 0b00000101;    // Timer2 ON, prescaler 4
}

void PWM_SetDuty(unsigned int duty){
    if(duty > 1023)
        duty = 1023;
    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B0 = duty & 0x01;
    CCP1CONbits.DC1B1 = (duty & 0x02)>>1;
}

void main(){
    unsigned int lecturaADC;
    ADC_Init();     // Inicializar ADC
    PWM_Init();     // Inicializar PWM

    while(1){
        lecturaADC = ADC_Read();
        PWM_SetDuty(lecturaADC);
        __delay_ms(10);
    }
}