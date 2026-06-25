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

//PWM hardware
char volt[12];
char pct[12];

//PWM software
volatile unsigned char duty2 = 0;
volatile unsigned char pwm_cnt = 0;

//Hardware
void PWM_Init(void){
    PORTC = 0;
    PORTD = 0;

    TRISCbits.TRISC1 = 0;   
    TRISDbits.TRISD0 = 0;  

    PR2 = 124;

    T2CON = 0b00000111;   // 1:16

    CCP2CON = 0b00001100;
    CCPR2L = 0;

    TMR2IF = 0;
    TMR2IE = 1;

    PEIE = 1;
    GIE = 1;
}

void PWM_SetDuty(unsigned int duty){
    CCPR2L = duty >> 2;
    CCP2CON &= 0b11001111;
    CCP2CON |= ((duty & 0x03) << 4);
}

void ADC_Init(void){
    ANSEL = 0x03;       
    ANSELH = 0x00;

    ADCON0 = 0x01;      
    ADCON1 = 0x80;     
}

unsigned int ADC_Read(unsigned char channel){
    ADCON0 &= 0b11000011;
    ADCON0 |= (channel << 2);
    __delay_us(20);
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((unsigned int)ADRESH << 8) | ADRESL;
}

unsigned int ADC_ReadAverage(unsigned char channel){
    unsigned long suma = 0;
    for(char i = 0; i < 8; i++){
        suma += ADC_Read(channel);
        __delay_us(100);
    }

    return (unsigned int)(suma / 8);
}

void main(void){
    unsigned int adc1;
    unsigned int adc2;
    unsigned int duty1;
    unsigned long pct1;
    unsigned long pct2;

    ADC_Init();
    PWM_Init();

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);

    LCD_Clear();

    LCD_Set_Cursor(0,0);
    LCD_putrs("L1:");

    LCD_Set_Cursor(1,0);
    LCD_putrs("L2:");

    while(1){
        // Potenciometro 1 (hardware)
        adc1 = ADC_ReadAverage(0);

        // Potenciometro 2 (software)
        adc2 = ADC_ReadAverage(1);

        duty1 = ((unsigned long)adc1 * 499) / 1023;

        PWM_SetDuty(duty1);

        duty2 = (unsigned char)(((unsigned long)adc2 * 100) / 1023);

        pct1 = ((unsigned long)adc1 * 100) / 1023;
        pct2 = ((unsigned long)adc2 * 100) / 1023;

        LCD_Set_Cursor(0,3);
        sprintf(volt,"%3lu%%   ",pct1);
        LCD_putrs(volt);

        LCD_Set_Cursor(1,3);
        sprintf(pct,"%3lu%%   ",pct2);
        LCD_putrs(pct);

        __delay_ms(50);
    }
}

void __interrupt() ISR(void){
    if(TMR2IF){
        pwm_cnt++;
        if(pwm_cnt >= 100){
            pwm_cnt = 0;
        }

        if(pwm_cnt < duty2){
            PORTDbits.RD0 = 1;
        }
        else{
            PORTDbits.RD0 = 0;
        }
        TMR2IF = 0;
    }
}
