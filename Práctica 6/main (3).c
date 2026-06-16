#include <xc.h>
#include <stdbool.h>
#include "lcd.h"

//=============================================================================
// CONFIGURACIÓN
//=============================================================================
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

//=============================================================================
// VARIABLES GLOBALES
//=============================================================================
volatile unsigned char mensaje = 0;
volatile unsigned char actualizarLCD = 1;

// CARACTERES PERSONALIZADOS

const char flechaDerecha[8] = {
    0b00100,
    0b00010,
    0b11111,
    0b11111,
    0b11111,
    0b00010,
    0b00100,
    0b00000
};


const char corazon[8] = {
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000
};

void LCD_CreateChar(unsigned char pos, const char *data){
    pos &= 0x07;
    LCD_Cmd(0x40 + (pos * 8));
    for(unsigned char i = 0; i < 8; i++){
        LCD_putc(data[i]);
    }
    LCD_Cmd(0x80);
}

void __interrupt() ISR(void){
    if(INTF){
        mensaje ^= 1;         
        actualizarLCD = 1;    

        INTF = 0;            
    }
}


void main(void){
    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};

    ANSEL = 0;
    ANSELH = 0;

    TRISC = 0x00;     
    TRISB = 0xFF;      

    PORTC = 0;
    PORTB = 0;

    OPTION_REGbits.nRBPU = 0;


    INTEDG = 0;        
    INTF = 0;
    INTE = 1;
    GIE = 1;

    LCD_Init(lcd);


    LCD_CreateChar(0, flechaDerecha);
    LCD_CreateChar(1, corazon);

    while(1){
        if(actualizarLCD){
            LCD_Clear();
            if(mensaje == 0){
                LCD_Set_Cursor(0,0);
                LCD_putrs("     ALOHA  ");

                LCD_Set_Cursor(1,0);
                LCD_putc(0);           
                LCD_putrs("    WORLD ");
            }
            else{
                LCD_Set_Cursor(0,0);
                LCD_putrs("    Hakuna");

                LCD_Set_Cursor(1,0);
                LCD_putrs("    Matata ");
                LCD_putc(1);           // Corazón
            }

            actualizarLCD = 0;
        }
    }
}