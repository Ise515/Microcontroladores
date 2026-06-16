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

unsigned char patron[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

volatile unsigned char direccion = 1; // 1=sube, 0=baja
volatile unsigned char cambio = 0;

//interrupcion
void __interrupt() ISR(void){
    if(INTCONbits.INTF){
        cambio = 1;
        INTCONbits.INTF = 0;
    }
}

void main(void){
    
    ANSEL = 0;
    ANSELH = 0;

    TRISC = 0x00;
    TRISD = 0x00;

    TRISBbits.TRISB0 = 1;

    PORTC = 0;
    PORTD = 0;

    OPTION_REGbits.nRBPU = 0;   
    WPUBbits.WPUB0 = 1;         

    //hacia atras
    OPTION_REGbits.INTEDG = 0;   

    INTCONbits.INTF = 0;
    INTCONbits.INTE = 1;
    INTCONbits.GIE = 1;

    unsigned int num = 0;

    while(1){
        int uni = num % 10;
        int dec = (num / 10) % 10;
        int cen = (num / 100) % 10;
        int mil = (num / 1000) % 10;

        for(int i=0;i<40;i++){
            PORTC = 0b00001110;
            PORTD = patron[uni];
            __delay_ms(2);

            PORTC = 0b00001101;
            PORTD = patron[dec];
            __delay_ms(2);

            PORTC = 0b00001011;
            PORTD = patron[cen];
            __delay_ms(2);

            PORTC = 0b00000111;
            PORTD = patron[mil];
            __delay_ms(2);
        }

        if(cambio){
            direccion ^= 1;
            cambio = 0;
        }

        if(direccion){
            num++;

            if(num > 9999)
                num = 0;
        }
        else{
            if(num == 0)
                num = 9999;
            else
                num--;
        }
    }
}