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

#define LED PORTCbits.RC0

unsigned char patron[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

volatile unsigned char interrupcion_activa = 0;

void blink_led(void){
    for(int i = 0; i < 4; i++){
        LED = 1;
        __delay_ms(500);
        LED = 0;
        __delay_ms(500);
    }
}

void main(void){
    ANSEL  = 0;
    ANSELH = 0;

    OPTION_REG &= 0b01111111;  

    TRISC = 0;
    TRISD = 0;
    TRISBbits.TRISB0 = 1;  

    PORTC = 0;
    PORTD = 0;

    unsigned char count = 0;

    INTEDG = 0;   
    INTF   = 0;   
    INTE   = 1;   
    GIE    = 1;   

    while(1){
        PORTD = patron[count];
        __delay_ms(500);

        if(interrupcion_activa){
            interrupcion_activa = 0;
            blink_led();         
        }
        count = (count + 1) % 10;
    }
}

void __interrupt() ISR(void){
    if(INTF){
        INTF = 0;
        interrupcion_activa = 1;
    }
}
