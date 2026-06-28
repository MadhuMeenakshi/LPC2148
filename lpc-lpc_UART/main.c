#include <LPC214x.h>

void UART0_Init(void)
{
    PINSEL0 |= 0x00000005;

    U0LCR = 0x83;
    U0DLL = 97;
    U0DLM = 0;
    U0LCR = 0x03;
}

char UART0_RxChar(void)
{
    while(!(U0LSR & 0x01));
    return U0RBR;
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
    char ch;

    UART0_Init();

    UART0_String("UART SLAVE\r\n");

    while(1)
    {
        ch = UART0_RxChar();

        UART0_String("Received : ");
        UART0_TxChar(ch);
        UART0_String("\r\n");
    }
}