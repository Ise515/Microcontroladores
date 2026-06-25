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

#define SERVO_0    62    // 0°   
#define SERVO_180  250   // 180° 


void PWM_Init(void){
    TRISC2 = 0;      // CCP1 como salida
    PR2 = 249;       // Periodo PWM
    CCP1CON = 0x0C; 
    CCPR1L = SERVO_0;
    T2CON = 0b00000111; // Timer2 ON, prescaler 16
    __delay_ms(100);
}

void PWM_SetDuty(unsigned int duty){
    CCPR1L = duty;
    CCP1CON &= 0xCF;
}

void main(void){
    unsigned int duty;

    PWM_Init();

    while(1){
        // Movimiento de 0° a 180°
        for(duty = SERVO_0; duty <= SERVO_180; duty++){
            PWM_SetDuty(duty);
            __delay_ms(10);
        }

        // Movimiento de 180° a 0°
        for(duty = SERVO_180; duty > SERVO_0; duty--){
            PWM_SetDuty(duty);
            __delay_ms(10);
        }
    }
}
