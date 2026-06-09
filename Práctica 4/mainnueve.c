#include <xc.h>         // Biblioteca principal del compilador XC8

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

unsigned char tabla[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

unsigned char contador = 0;

void mostrarNumero(unsigned char numero){
    unsigned char decenas;
    unsigned char unidades;

    decenas = numero / 10;
    unidades = numero % 10;

    PORTC = tabla[decenas];   // Decenas
    PORTD = tabla[unidades];  // Unidades
}
void main(void){
    
    ANSEL = 0;
    ANSELH = 0;

    OPTION_REG = OPTION_REG & 0b01111111;

    TRISB = 0xFF;   // Botones
    TRISC = 0x00;   // Decenas
    TRISD = 0x00;   // Unidades

    PORTB = 0;
    PORTC = 0;
    PORTD = 0;

    mostrarNumero(contador);

    while(1){
        //Boton +1
        if(!PORTBbits.RB0){
            __delay_ms(20);
            if(!PORTBbits.RB0){
                contador++;
                if(contador > 99)
                    contador = 0;

                mostrarNumero(contador);

                while(!PORTBbits.RB0);
            }
        }

        //Boton -1
        if(!PORTBbits.RB1){
            __delay_ms(20);
            if(!PORTBbits.RB1){
                if(contador == 0)
                    contador = 99;
                else
                    contador--;

                mostrarNumero(contador);

                while(!PORTBbits.RB1);
            }
        }

        //Reset
        if(!PORTBbits.RB2){
            __delay_ms(20);
            if(!PORTBbits.RB2){
                contador = 0;
                mostrarNumero(contador);

                while(!PORTBbits.RB2);
            }
        }
    }
}