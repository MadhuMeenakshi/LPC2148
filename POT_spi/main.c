#include <LPC214x.h>
#include <stdio.h>

#define CS_LOW()   IOCLR0 = (1<<7)
#define CS_HIGH()  IOSET0 = (1<<7)

void delay_ms(unsigned int ms)
{
    unsigned int i,j;

    for(i=0;i<ms;i++)
        for(j=0;j<6000;j++);
}

/*---------------- UART ----------------*/

void UART0_Init(void)
{
    PINSEL0 |= 0x00000005;      // P0.0=TXD0, P0.1=RXD0

    U0LCR = 0x83;               // 8-bit,1 stop,DLAB=1
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
    {
        UART0_TxChar(*str++);
    }
}

/*---------------- SPI ----------------*/

void SPI0_Init(void)
{
    PINSEL0 |= (1<<8);      // P0.4 SCK0
    PINSEL0 |= (1<<10);     // P0.5 MISO0
    PINSEL0 |= (1<<12);     // P0.6 MOSI0
    PINSEL0 |= (1<<14);     // P0.7 SSEL0

    IODIR0 |= (1<<7);

    CS_HIGH();

    S0SPCCR = 8;

    S0SPCR =
        (1<<5) |      // Master
        (1<<3);       // CPHA
}

/*---------------- SPI Transfer ----------------*/

unsigned char SPI0_Transfer(unsigned char data)
{
    S0SPDR = data;

    while(!(S0SPSR & (1<<7)));      // Wait until transfer complete

    return S0SPDR;
}

/*---------------- MCP3204 ADC Read ----------------*/

unsigned int MCP3204_Read(unsigned char channel)
{
    unsigned char high, low;
    unsigned int value;

    channel &= 0x03;                // CH0-CH3 only

    CS_LOW();

    SPI0_Transfer(0x06 | (channel >> 2));

    high = SPI0_Transfer(channel << 6);

    low = SPI0_Transfer(0x00);

    CS_HIGH();

    value = ((high & 0x0F) << 8) | low;

    return value;
}

/*---------------- Main Program ----------------*/

int main(void)
{
    unsigned int adc_value;
    char buffer[30];

    UART0_Init();
    SPI0_Init();

    UART0_String("\r\n");
    UART0_String("=================================\r\n");
    UART0_String(" LPC2148 SPI - MCP3204 ADC Demo\r\n");
    UART0_String("=================================\r\n\r\n");

    while(1)
    {
        adc_value = MCP3204_Read(0);

        sprintf(buffer, "ADC Value = %4u\r\n", adc_value);

        UART0_String(buffer);

        delay_ms(500);
    }
}