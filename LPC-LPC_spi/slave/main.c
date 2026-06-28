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
    PINSEL0 |= 0x00000005;      // TXD0 RXD0

    U0LCR = 0x83;
    U0DLL = 97;                 //9600 baud
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

/*---------------- SPI SLAVE ----------------*/

void SPI_Slave_Init(void)
{
    /* P0.4=SCK0
       P0.5=MISO0
       P0.6=MOSI0
       P0.7=SSEL0
    */

    PINSEL0 |= (1<<8);
    PINSEL0 |= (1<<10);
    PINSEL0 |= (1<<12);
    PINSEL0 |= (1<<14);

    /* Slave Mode */
    S0SPCR = (1<<3);
}

unsigned char SPI_Slave_Receive(void)
{
    while(!(S0SPSR & (1<<7)));

    return S0SPDR;
}

/*---------------- MAIN ----------------*/

int main(void)
{
    unsigned char rx;
    char msg[30];

    UART0_Init();
    SPI_Slave_Init();

    UART0_String("\r\n");
    UART0_String("===== SPI SLAVE =====\r\n");

    while(1)
    {
        rx = SPI_Slave_Receive();

        sprintf(msg,"Received : %c\r\n",rx);

        UART0_String(msg);

        delay_ms(50);
    }
}