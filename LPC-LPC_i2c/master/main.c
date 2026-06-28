#include <LPC214x.h>
#include <stdio.h>

#define SLAVE_ADDR 0x50

char buffer[32];

void delay_ms(unsigned int ms)
{
    unsigned int i,j;

    for(i=0;i<ms;i++)
        for(j=0;j<6000;j++);
}

/*---------------- UART0 ----------------*/

void UART0_Init(void)
{
    PINSEL0 |= 0x00000005;      // P0.0=TXD0 P0.1=RXD0

    U0LCR = 0x83;               // 8-bit,1 stop,No parity
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

/*---------------- I2C MASTER ----------------*/

void I2C_Master_Init(void)
{
    /* P0.2 = SCL0
       P0.3 = SDA0
    */

    PINSEL0 |= (1<<4) | (1<<6);

    I2C0SCLH = 75;
    I2C0SCLL = 75;

    I2C0CONCLR = 0x6C;
    I2C0CONSET = 0x40;      // Enable I2C
}

void I2C_Start(void)
{
    I2C0CONSET = 0x20;      // STA

    while(!(I2C0CONSET & 0x08));

    I2C0CONCLR = 0x20;
}

void I2C_Stop(void)
{
    I2C0CONSET = 0x10;      // STO

    I2C0CONCLR = 0x08;
}

void I2C_Write(unsigned char data)
{
    I2C0DAT = data;

    I2C0CONCLR = 0x08;

    while(!(I2C0CONSET & 0x08));
}

void I2C_SendByte(unsigned char slaveAddr,
                  unsigned char data)
{
    I2C_Start();

    I2C_Write(slaveAddr << 1);      // Write Address

    I2C_Write(data);

    I2C_Stop();
}

/*---------------- MAIN ----------------*/

int main(void)
{
    unsigned char tx = 'A';

    UART0_Init();
    I2C_Master_Init();

    UART0_String("\r\n");
    UART0_String("===== I2C MASTER =====\r\n");

    while(1)
    {
        I2C_SendByte(SLAVE_ADDR, tx);

        UART0_String("Sent : ");
        UART0_TxChar(tx);
        UART0_String("\r\n");

        tx++;

        if(tx > 'Z')
            tx = 'A';

        delay_ms(1000);
    }
}