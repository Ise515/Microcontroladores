#include <stdio.h>
#include <stdlib.h>
#include <xc.h>         // Biblioteca principal del compilador XC8
#include <stdbool.h>
#include "lcd.h"
#include "Keypad.h"

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

char tecla;
char operador = 0;

unsigned int num1 = 0;
unsigned int num2 = 0;

unsigned long resultado;

char buffer[16];

void main(void){
    ANSEL = 0x00;
    ANSELH = 0x00;

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};

    LCD_Init(lcd);
    InitKeypad();

    LCD_Clear();

    LCD_Set_Cursor(0,0);
    LCD_putrs("Calculadora");

    __delay_ms(1500);

    LCD_Clear();

    while(1){
        tecla = switch_press_scan();

        //Tecla para borrar
        if(tecla == '*'){
            num1 = 0;
            num2 = 0;
            operador = 0;

            LCD_Clear();
            continue;
        }

        if(tecla >= '0' && tecla <= '9'){
            if(operador == 0)
            {
                num1 = (num1 * 10) + (tecla - '0');
            }
            else
            {
                num2 = (num2 * 10) + (tecla - '0');
            }

            LCD_putc(tecla);
        }
        
        //Tecla suma
        else if(tecla == 'A'){
            operador = '+';
            LCD_putc('+');
        }

        //Tecla Resta
        else if(tecla == 'B'){
            operador = '-';
            LCD_putc('-');
        }

        //Tecla multiplicacion
        else if(tecla == 'C'){
            operador = '*';
            LCD_putc('*');
        }

        //Tecla division
        else if(tecla == 'D'){
            operador = '/';
            LCD_putc('/');
        }

        
        //Calculos
        else if(tecla == '#')
        {
            LCD_Clear();

            LCD_Set_Cursor(0,0);
            LCD_putrs("Resultado:");

            LCD_Set_Cursor(1,0);

            switch(operador)
            {
                case '+':
                    resultado = num1 + num2;

                    sprintf(buffer,"%lu",resultado);
                    LCD_putrs(buffer);
                    break;

                case '-':
                    resultado = num1 - num2;

                    sprintf(buffer,"%lu",resultado);
                    LCD_putrs(buffer);
                    break;

                case '*':
                    resultado = num1 * num2;

                    sprintf(buffer,"%lu",resultado);
                    LCD_putrs(buffer);
                    break;

                case '/':
                {
                    unsigned int entero;
                    unsigned int decimal;

                    if(num2 != 0)
                    {
                        entero = num1 / num2;

                        decimal =
                        ((unsigned long)(num1 % num2) * 100)
                        / num2;

                        sprintf(buffer,"%u.%02u",
                                entero,
                                decimal);

                        LCD_putrs(buffer);
                    }
                    else
                    {
                        LCD_putrs("Error");
                    }

                    break;
                }

                default:
                    LCD_putrs("Operacion?");
                    break;
            }

            __delay_ms(3000);

            num1 = 0;
            num2 = 0;
            operador = 0;
            resultado = 0;

            LCD_Clear();
        }
    }
}