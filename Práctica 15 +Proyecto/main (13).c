#include <xc.h>

//==================================================
// CONFIG
//==================================================

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

//==================================================
// VARIABLES GLOBALES
//==================================================

volatile unsigned char frame[8];
volatile unsigned char currentRow = 0;
volatile unsigned int tick_ms = 0;
volatile unsigned char ballActive;
volatile unsigned char rxState = 0;
volatile unsigned char rxBallX;
volatile signed char  rxVX;

// Pelota
signed char ballX = 4;
signed char ballY = 4;

signed char vx = 1;
signed char vy = -1;

// Raqueta
unsigned char paddleX = 2;
// Control de reinicio
unsigned char gameOver = 0;
unsigned char blinkCount = 0;
unsigned char ballVisible = 1;
unsigned int blinkTimer = 0;

void ADC_Init(void);
unsigned int ADC_Read(unsigned char channel);

void Timer0_Init(void);
void ClearFrame(void);
void DrawObjects(void);
void UpdatePaddle(void);
void UpdateBall(void);
void ResetBall(void);
void UART_Init(void);
void UART_Write(unsigned char dato);

//==================================================
// INTERRUPCION TIMER0
//==================================================

void __interrupt() ISR(void)
{
    if(T0IF)
    {
        T0IF = 0;

        TMR0 = 194;

        // Apagar todo
        PORTB = 0x00;
        PORTD = 0xFF;

        // Activar fila
        PORTB = (1 << currentRow);

        // Mostrar columnas
        PORTD = ~(frame[currentRow]);

        currentRow++;

        if(currentRow >= 8)
        {
            currentRow = 0;
        }

        tick_ms++;
    }
    
    if(RCIF)
    {
        unsigned char dato;

        dato = RCREG;

        switch(rxState)
        {
            case 0:

                if(dato == 0xAA)
                {
                    rxState = 1;
                }

                break;

            case 1:

                rxBallX = dato;
                rxState = 2;

                break;

            case 2:

                rxVX = (signed char)dato;

                ballX = rxBallX;
                ballY = 0;

                vx = rxVX;
                vy = 1;

                ballActive = 1;

                rxState = 0;

                break;
        }

        if(OERR)
        {
            CREN = 0;
            CREN = 1;
        }
    }
}

//==================================================
// ADC
//==================================================

void ADC_Init(void)
{
    ANSEL = 0x01;
    ANSELH = 0x00;

    ADCON0 = 0x01;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(unsigned char channel)
{
    ADCON0 &= 0b11000101;
    ADCON0 |= (channel << 3);

    __delay_us(20);

    GO_nDONE = 1;

    while(GO_nDONE);

    return ((ADRESH << 8) | ADRESL);
}

//==================================================
// TIMER0
//==================================================

void Timer0_Init(void)
{
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;

    // Prescaler 1:32

    OPTION_REGbits.PS2 = 1;
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS0 = 0;

    TMR0 = 194;

    T0IF = 0;
    T0IE = 1;

    GIE = 1;
    PEIE = 1;
}

//==================================================
// LIMPIAR FRAME
//==================================================

void ClearFrame(void)
{
    unsigned char i;

    for(i = 0; i < 8; i++)
    {
        frame[i] = 0x00;
    }
}

//==================================================
// DIBUJAR OBJETOS
//==================================================

void DrawObjects(void)
{
    // Seguridad

    if(ballX < 0) ballX = 0;
    if(ballX > 7) ballX = 7;

    if(ballY < 0) ballY = 0;
    if(ballY > 7) ballY = 7;

    // Pelota

    if(ballVisible && ballActive)
    {
        frame[ballY] |= (0x01 << ballX);
    }

    // Raqueta (3 LEDs)

    frame[7] |= (0x01 << paddleX);
    frame[7] |= (0x01 << (paddleX + 1));
    frame[7] |= (0x01 << (paddleX + 2));
}

//==================================================
// JOYSTICK
//==================================================

void UpdatePaddle(void)
{
    unsigned int adc;

    adc = ADC_Read(0);

    if(adc < 50)
    {
        paddleX = 0;
    }
    else if(adc > 970)
    {
        paddleX = 5;
    }
    else
    {
        paddleX = (adc * 6UL) / 1024;
    }
}

//==================================================
// REINICIO DE PELOTA
//==================================================

void ResetBall(void)
{
    ballX = 4;
    ballY = 4;

    vx = 1;
    vy = -1;

    gameOver = 1;

    blinkCount = 0;
    ballVisible = 1;

    blinkTimer = tick_ms;

    ClearFrame();
    DrawObjects();
}

//==================================================
// MOVIMIENTO DE PELOTA
//==================================================

void UpdateBall(void){
    // Detectar raqueta antes de mover
    if(!ballActive)
    {
        return;
    }

    if(vy > 0 && ballY == 6)
    {
        if(ballX >= paddleX &&
           ballX <= (paddleX + 2))
        {
            vy = -1;

            if(ballX == paddleX)
            {
                vx = -1;
            }
            else if(ballX == (paddleX + 2))
            {
                vx = 1;
            }
        }
        else
        {
            ResetBall();
            return;
        }
    }

    ballX += vx;
    ballY += vy;

    // Pared izquierda
    if(ballX < 0)
    {
        ballX = 0;
        vx = 1;
    }

    // Pared derecha
    if(ballX > 7)
    {
        ballX = 7;
        vx = -1;
    }

    // Techo
    if(ballY < 0)
    {
        UART_Write(0xAA);
        UART_Write(ballX);
        UART_Write((unsigned char)vx);

        ballActive = 0;

        return;
    }
}
//===================================================
// COMUNICACION UART
//===================================================
void UART_Init(void)
{
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;

    SPBRG = 12;      // 9600 @ 8MHz

    BRGH = 1;
    SYNC = 0;
    SPEN = 1;

    TXEN = 1;
    CREN = 1;

    RCIF = 0;
    RCIE = 1;
}

void UART_Write(unsigned char dato)
{
    while(!TXIF);

    TXREG = dato;
}

//==================================================
// MAIN
//==================================================

void main(void)
{
    // Configuración de puertos

    TRISA = 0xFF;
    TRISB = 0x00;
    TRISD = 0x00;

    PORTA = 0x00;
    PORTB = 0x00;
    PORTD = 0xFF;

    ADC_Init();
    Timer0_Init();
    UART_Init();
    ballActive = 0; 

    ClearFrame();
    DrawObjects();

    unsigned int lastGameUpdate = 0;

    while(1)
    {
        //=========================================
        // PARPADEO AL PERDER
        //=========================================

        if(gameOver)
        {
            if((tick_ms - blinkTimer) >= 300)
            {
                blinkTimer = tick_ms;

                ballVisible = !ballVisible;

                blinkCount++;

                ClearFrame();
                DrawObjects();
            }

            if(blinkCount >= 6)
            {
                gameOver = 0;
                ballVisible = 1;

                ClearFrame();
                DrawObjects();
            }

            continue;
        }

        //=========================================
        // JUEGO NORMAL
        //=========================================

        if((tick_ms - lastGameUpdate) >= 120)
        {
            lastGameUpdate = tick_ms;

            UpdatePaddle();
            UpdateBall();

            ClearFrame();
            DrawObjects();
        }
    }
}

