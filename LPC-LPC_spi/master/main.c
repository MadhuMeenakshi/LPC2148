#include <LPC214x.h>
#include <stdio.h>

void delay_ms(unsigned int ms)
{
    unsigned int i,j;

    for(i=0;i<ms;i++)
        for(j=0;j<6000;j++);
}

/*---------------- UART0 ----------------*/

void UART0_Init(void)
{
    PINSEL0 |= 0x00000005;      // P0.0=TXD0, P0.1=RXD0

    U0LCR = 0x83;               // 8-bit, 1 stop, DLAB=1
    U0DLL = 97;                 // 9600 baud @15MHz
    U0DLM = 0;
    U0LCR = 0x03;
}

void UART0_TxChar(char ch)
{
    while(!(U0LSR & 0x20));

    U0THR = ch;
}

void UART0_String(char *str)
{
    while(*str)
        UART0_TxChar(*str++);
}

/*---------------- SPI MASTER ----------------*/

void SPI_Master_Init(void)
{
    /* P0.4=SCK0
       P0.5=MISO0
       P0.6=MOSI0
       P0.7=SSEL0
    */

    PINSEL0 |= (1<<8);      // SCK0
    PINSEL0 |= (1<<10);     // MISO0
    PINSEL0 |= (1<<12);     // MOSI0
    PINSEL0 |= (1<<14);     // SSEL0

    S0SPCCR = 8;            // SPI Clock Divider

    S0SPCR =
        (1<<5) |            // Master Mode
        (1<<3);             // CPHA
}

unsigned char SPI_Master_Transfer(unsigned char data)
{
    S0SPDR = data;

    while(!(S0SPSR & (1<<7)));

    return S0SPDR;
}

/*---------------- MAIN ----------------*/

int main(void)
{
    unsigned char tx = 'A';
    char msg[30];

    UART0_Init();
    SPI_Master_Init();

    UART0_String("\r\n");
    UART0_String("===== SPI MASTER =====\r\n");

    while(1)
    {
        SPI_Master_Transfer(tx);

        sprintf(msg,"Sent : %c\r\n",tx);
        UART0_String(msg);

        tx++;

        if(tx > 'Z')
            tx = 'A';

        delay_ms(1000);
    }
}