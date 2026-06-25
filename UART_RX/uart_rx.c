#include <LPC214x.h>

unsigned char seg[10]=
{
    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F
};

void UART0_Init(void)
{
    PINSEL0 |= 0x00000005;

    U0LCR = 0x83;
    U0DLL = 0xC3;
    U0DLM = 0x00;
    U0LCR = 0x03;
}

char UART0_GetChar(void)
{
    while(!(U0LSR & 0x01));
    return U0RBR;
}

int main()
{
    char ch;

    IODIR0 |= 0x007F0000;       // P0.16-P0.22 output

    UART0_Init();

    while(1)
    {
        ch = UART0_GetChar();

        if(ch>='0' && ch<='9')
        {
            IO0CLR = 0x007F0000;

            IO0SET = ((unsigned int)seg[ch-'0']) << 16;
        }
    }
}