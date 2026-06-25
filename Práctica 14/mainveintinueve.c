#include <stdio.h>
#include <stdlib.h>
#include <xc.h>         // Biblioteca principal del compilador XC8
#include <stdbool.h>

//=============================================================================
// CONFIGURACION DE BITS DE CONFIGURACION (FUSES)
//=============================================================================

// Seleccion de oscilador (usar XT si estas usando un cristal de 8 MHz)
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

//=============================================================================
// DEFINICIONES
//=============================================================================
#define _XTAL_FREQ 8000000

#define SERVO_0    62    // 0°   (0.5ms)
#define SERVO_180  250   // 180° (2.5ms)

void ADC_Init(void){
    ANSEL  = 0x01;
    ANSELH = 0x00;
    ADCON0 = 0x81;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(void){
    __delay_us(10);
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH << 8) + ADRESL);
}

void PWM_Init(void){
    TRISC2  = 0;
    PR2     = 249;
    CCP1CON = 0x0C;
    CCPR1L  = SERVO_0;
    T2CON   = 0b00000111;
}

void PWM_SetDuty(unsigned int duty){
    CCPR1L  = (unsigned char)(duty);
    CCP1CON = (CCP1CON & 0xCF);
}

void main(void){
    ADC_Init();
    PWM_Init();

    while(1){
        // Leer poti (0-1023)
        unsigned int adc = ADC_Read();

        // Mapear ADC (0-1023) a duty (62-255)
        unsigned int duty = SERVO_0 + (unsigned int)(((unsigned long)adc * (SERVO_180 - SERVO_0)) / 1023);

        PWM_SetDuty(duty);

        __delay_ms(10);
    }
}
