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

void ADC_Init(void){
    ANSEL = 0x01; 
    ANSELH = 0x00;

    TRISA0 = 1; //Entrada

    ADCON0 = 0x01;     // ADC ON canal 0
    ADCON1 = 0x80;     // Justificado a la derecha
}

unsigned int ADC_Read(void){
    __delay_us(20);  
    GO_nDONE = 1;      // Se inicia la conversion
    while(GO_nDONE);   // esperar resultado
    return ((ADRESH << 8) + ADRESL);
}
//PWM CCP1
void PWM_Init(void){
    TRISC2 = 0;            // RC2 salida CCP1
    PR2 = 124;             
    CCP1CON = 0b00001100;  // CCP1 modo PWM
    T2CON = 0b00000101;    // Timer2 ON, prescaler 1:4
}

void PWM_SetDuty(unsigned int duty){
    if(duty > 500)
        duty = 500;

    CCPR1L = duty >> 2;

    CCP1CONbits.DC1B1 = (duty & 0x02) >> 1;
    CCP1CONbits.DC1B0 = duty & 0x01;
}


void main(void){
    unsigned int adc;
    unsigned int velocidad;

    ADC_Init();
    PWM_Init();

    // RB0 y RB1 (botones para los giros)
    TRISB0 = 1;
    TRISB1 = 1;

    // RD0 y RD1 salidas (IN1 e IN2)
    TRISD0 = 0;
    TRISD1 = 0;

    // Activar pull-ups internos
    OPTION_REGbits.nRBPU = 0;

    PORTB = 0x00;

    while(1){
   
        adc = ADC_Read();
        velocidad = (unsigned long)adc * 500 / 1023;
        PWM_SetDuty(velocidad);

        // Giro horario
        if(RB0 == 0){
            RD0 = 1;   // IN1
            RD1 = 0;   // IN2
        }

        // Giro antihorario
        else if(RB1 == 0){
            RD0 = 0;   // IN1
            RD1 = 1;   // IN2
        }
        __delay_ms(10);
    }
}
