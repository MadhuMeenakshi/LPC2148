#include <LPC214x.h>

void delay(void)
{
    unsigned int i,j;
    for(i=0;i<200;i++)
        for(j=0;j<6000;j++);
}

void UART0_Init(void)
{
    PINSEL0 |= 0x00000005;      // P0.0=TXD0, P0.1=RXD0

    U0LCR = 0x83;               // 8-bit, 1 stop, no parity, DLAB=1
    U0DLL = 0xC3;               // 9600 baud @12MHz
    U0DLM = 0x00;
    U0LCR = 0x03;               // DLAB=0
}

void UART0_SendChar(char ch)
{
    while(!(U0LSR & (1<<5)));
    U0THR = ch;
}

int main()
{
    char count='0';

    UART0_Init();

    while(1)
    {
        UART0_SendChar(count);

        count++;

        if(count>'9')
            count='0';

        delay();
    }
}