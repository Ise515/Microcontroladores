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

#define _XTAL_FREQ 8000000      // Frecuencia del oscilador (para __delay_ms y __delay_us)

//DEFINIR EL PATRON DE LAS FILAS
unsigned char mensaje[] = {

    // D
    0b00000000,
    0b11111111,
    0b10000001,
    0b10000001,
    0b10000001,
    0b01000010,
    0b00111100,
    0b00000000,

    // Espacio
    0b00000000,

    // E
    0b00000000,
    0b11111111,
    0b10011001,
    0b10011001,
    0b10011001,
    0b10011001,
    0b10000001,
    0b00000000,

    // Espacio
    0b00000000,

    // M
    0b11111111,
    0b11111110,
    0b00000100,
    0b00001000,
    0b00001000,
    0b00000100,
    0b11111110,
    0b11111111,

    // Espacio
    0b00000000,

    // I
    0b00000000,
    0b10000001,
    0b10000001,
    0b11111111,
    0b11111111,
    0b10000001,
    0b10000001,
    0b00000000,

    // Espacios finales
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
};

void main(void)
{
    TRISB = 0;
    TRISD = 0;

    PORTB = 0;
    PORTD = 0;

    int columnas = sizeof(mensaje);
    char offset;

    while(1)
    {
        for(offset = 0; offset < columnas - 8; offset++)
        {
            for(int tiempo = 0; tiempo < 10; tiempo++)
            {
                for(char i = 0; i < 8; i++)
                {
                    PORTB = 1 << i;
                    PORTD = ~mensaje[offset + i];
                    __delay_ms(2);
                }
            }
        }
    }
}