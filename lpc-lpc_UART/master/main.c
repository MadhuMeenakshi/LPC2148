#include <LPC214x.h>

void delay_ms(unsigned int ms)
{
    unsigned int i,j;
    for(i=0;i<ms;i++)
        for(j=0;j<6000;j++);
}

void UART0_Init(void)
{
    PINSEL0 |= 0x00000005;

    U0LCR = 0x83;
    U0DLL = 97;
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

int main()
{
    char ch='A';

    UART0_Init();

    UART0_String("UART MASTER\r\n");

    while(1)
    {
        UART0_TxChar(ch);
        UART0_String("\r\n");

        ch++;

        if(ch>'Z')
            ch='A';

        delay_ms(1000);
    }
}