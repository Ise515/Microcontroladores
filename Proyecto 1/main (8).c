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

signed char posicionx = 0;
unsigned char posiciony = 0;
unsigned int ADCX;
unsigned int ADCY;
unsigned char tiempo = 0;
signed char x_ant = 0; 
unsigned char y_ant = 0;

volatile unsigned char caracter = 0;
//Caracteres
const char fantasma[8]= {0b01110, 0b11111, 0b10101, 0b10101, 0b11111, 0b11111, 0b11111, 0b01010};
const char corazon[8] = {0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000, 0b00000};

void LCD_CreateChar(unsigned char pos, unsigned char *data){
    LCD_Cmd(0x40 + (pos*8));
    for (unsigned char i = 0; i < 8; i++){
        LCD_putc(data[i]);
    }
    LCD_Cmd(0x80);
}
void ADC_Init(void){
    ANSEL = 0x03;
    ANSELH = 0x00;
    ADCON0 = 0x01;
    ADCON1 = 0x81;
}
unsigned int ADC_Read (unsigned char channel){
    __delay_us(5);
    ADCON0 &= 0x83;
    ADCON0 |= channel << 2;
    __delay_ms(2);
    GO_nDONE = 1;
    while(GO_nDONE);
    return((ADRESH<<8) + ADRESL);
}

void __interrupt()ISR(void){
    if(INTF){
        __delay_ms(20);
        if(RB0==0){
            caracter++;
            if (caracter>1)
                caracter = 0;
        }
        INTF = 0;
    }
}
void main (void){
    ADC_Init();
    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);
    LCD_CreateChar(0,fantasma);
    LCD_CreateChar(1,corazon);
    
    
    TRISA = 0xFF;
    TRISB = 0x01;
    
    PORTA = 0;
    PORTB = 0;
    
    OPTION_REG = OPTION_REG & 0b01111111;
    
    INTEDG = 0;
    INTF = 0;
    INTE = 1;
    GIE = 1;
    
    while(1){
        x_ant = posicionx;
        y_ant = posiciony;
        
        ADCX = ADC_Read(0);
        ADCY = ADC_Read(1);
        
        //Eje x
        //Izquierda
        if (ADCX < 256){
            posicionx--;
            tiempo = 1;
        }
        else if((ADCX > 255) && (ADCX < 500)){
            posicionx--;
            tiempo = 0;
        }
        else if((ADCX > 524) && (ADCX < 768)){
            posicionx++;
            tiempo=0;
        }
        else if(ADCX > 767){
            posicionx++;
            tiempo=1;
        }
        
        ///Eje y
        if(ADCY < 500){
            posiciony = 0;
        }
        else if(ADCY > 524){
            posiciony = 1;
        }
        
        //Teletransporte
        if(posicionx > 15){
            posicionx = 0;
        }
        if(posicionx < 0){
            posicionx = 15;   
        }
        
        // Borrar personaje anterior
        LCD_Set_Cursor(y_ant,x_ant);
        LCD_putc(' ');

        // Dibujar personaje nuevo
        LCD_Set_Cursor(posiciony,posicionx);
        
        if(caracter == 0){
            LCD_putc(0);
        }
        else{
            LCD_putc(1);
        }
        if(tiempo){
        __delay_ms(250);
    }
    else{
        __delay_ms(500);
    }
        
        
    }
        
}