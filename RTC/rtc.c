#include <LPC214x.h>
#include <stdio.h>

#define FOSC 12000000
#define CCLK (FOSC * 5)
#define PCLK (CCLK / 4)

#define LCD_DATA   IO1PIN
#define LCD_DIR    IO1DIR

#define RS (1<<16)
#define RW (1<<17)
#define EN (1<<18)

#define D0 (1<<19)
#define D1 (1<<20)
#define D2 (1<<21)
#define D3 (1<<22)
#define D4 (1<<23)
#define D5 (1<<24)
#define D6 (1<<25)
#define D7 (1<<26)

char lcd_buffer[20];

void delay(unsigned int ms);

void LCD_Command(unsigned char cmd);
void LCD_Data(unsigned char data);
void LCD_Init(void);
void LCD_String(char *str);
void LCD_Clear(void);
void LCD_SetCursor(unsigned char row,unsigned char col);

void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(unsigned char data);
unsigned char I2C_Read_ACK(void);
unsigned char I2C_Read_NACK(void);

void DS1307_SetTime(unsigned char hh,
                    unsigned char mm,
                    unsigned char ss);

void DS1307_GetTime(unsigned char *hh,
                    unsigned char *mm,
                    unsigned char *ss);

unsigned char DecToBCD(unsigned char val);
unsigned char BCDToDec(unsigned char val);

void delay(unsigned int ms)
{
    unsigned int i,j;

    for(i=0;i<ms;i++)
    {
        for(j=0;j<6000;j++);
    }
}

void LCD_Command(unsigned char cmd)
{
    LCD_DATA &= ~(RS | RW | EN);

    LCD_DATA &= ~(D0|D1|D2|D3|D4|D5|D6|D7);

    if(cmd & 0x01) LCD_DATA |= D0;
    if(cmd & 0x02) LCD_DATA |= D1;
    if(cmd & 0x04) LCD_DATA |= D2;
    if(cmd & 0x08) LCD_DATA |= D3;
    if(cmd & 0x10) LCD_DATA |= D4;
    if(cmd & 0x20) LCD_DATA |= D5;
    if(cmd & 0x40) LCD_DATA |= D6;
    if(cmd & 0x80) LCD_DATA |= D7;

    LCD_DATA &= ~RS;
    LCD_DATA &= ~RW;

    LCD_DATA |= EN;
    delay(2);
    LCD_DATA &= ~EN;

    delay(2);
}

void LCD_Data(unsigned char data)
{
    LCD_DATA &= ~(D0|D1|D2|D3|D4|D5|D6|D7);

    if(data & 0x01) LCD_DATA |= D0;
    if(data & 0x02) LCD_DATA |= D1;
    if(data & 0x04) LCD_DATA |= D2;
    if(data & 0x08) LCD_DATA |= D3;
    if(data & 0x10) LCD_DATA |= D4;
    if(data & 0x20) LCD_DATA |= D5;
    if(data & 0x40) LCD_DATA |= D6;
    if(data & 0x80) LCD_DATA |= D7;

    LCD_DATA |= RS;
    LCD_DATA &= ~RW;

    LCD_DATA |= EN;
    delay(2);
    LCD_DATA &= ~EN;

    delay(2);
}

void LCD_Clear(void)
{
    LCD_Command(0x01);
    delay(5);
}

void LCD_SetCursor(unsigned char row,unsigned char col)
{
    unsigned char address;

    if(row==0)
        address=0x80+col;
    else
        address=0xC0+col;

    LCD_Command(address);
}

void LCD_String(char *str)
{
    while(*str)
    {
        LCD_Data(*str++);
    }
}

void LCD_Init(void)
{
    LCD_DIR |= RS|RW|EN|
               D0|D1|D2|D3|
               D4|D5|D6|D7;

    delay(20);

    LCD_Command(0x38);      // 8-bit mode
    LCD_Command(0x0C);      // Display ON
    LCD_Command(0x06);      // Entry mode
    LCD_Command(0x01);      // Clear display

    delay(5);
}

unsigned char DecToBCD(unsigned char val)
{
    return ((val/10)<<4) | (val%10);
}

unsigned char BCDToDec(unsigned char val)
{
    return (((val>>4)*10) + (val & 0x0F));
}



void I2C_Init(void)
{
    PINSEL0 |= 0x00000050;      // P0.2=SCL0 , P0.3=SDA0

    I2C0SCLH = 75;
    I2C0SCLL = 75;

    I2C0CONCLR = 0x6C;
    I2C0CONSET = 0x40;
}

void I2C_Start(void)
{
    I2C0CONSET = 0x20;

    while(!(I2C0CONSET & 0x08));

    I2C0CONCLR = 0x28;
}

void I2C_Stop(void)
{
    I2C0CONSET = 0x10;

    I2C0CONCLR = 0x08;

    while(I2C0CONSET & 0x10);
}

void I2C_Write(unsigned char data)
{
    I2C0DAT = data;

    I2C0CONCLR = 0x08;

    while(!(I2C0CONSET & 0x08));
}

unsigned char I2C_Read_ACK(void)
{
    unsigned char data;

    I2C0CONSET = 0x04;

    I2C0CONCLR = 0x08;

    while(!(I2C0CONSET & 0x08));

    data = I2C0DAT;

    return data;
}

unsigned char I2C_Read_NACK(void)
{
    unsigned char data;

    I2C0CONCLR = 0x0C;

    while(!(I2C0CONSET & 0x08));

    data = I2C0DAT;

    return data;
}



#define DS1307_WRITE 0xD0
#define DS1307_READ  0xD1

void DS1307_SetTime(unsigned char hh,
                    unsigned char mm,
                    unsigned char ss)
{
    I2C_Start();

    I2C_Write(DS1307_WRITE);

    I2C_Write(0x00);              // Seconds Register

    I2C_Write(DecToBCD(ss));
    I2C_Write(DecToBCD(mm));
    I2C_Write(DecToBCD(hh));

    I2C_Stop();
}

void DS1307_GetTime(unsigned char *hh,
                    unsigned char *mm,
                    unsigned char *ss)
{
    I2C_Start();

    I2C_Write(DS1307_WRITE);

    I2C_Write(0x00);

    I2C_Start();

    I2C_Write(DS1307_READ);

    *ss = BCDToDec(I2C_Read_ACK() & 0x7F);

    *mm = BCDToDec(I2C_Read_ACK());

    *hh = BCDToDec(I2C_Read_NACK());

    I2C_Stop();
}

int main(void)
{
    unsigned char hour;
    unsigned char minute;
    unsigned char second;

    LCD_Init();

    I2C_Init();

    LCD_Clear();
    LCD_SetCursor(0,0);
    LCD_String("RTC CLOCK");

    delay(1000);

    /* Uncomment this only the first time to set the RTC.
       Afterwards comment it again, otherwise every reset
       will set the time back to 12:00:00.

    DS1307_SetTime(12,0,0);

    */

    while(1)
    {
        DS1307_GetTime(&hour,&minute,&second);

        LCD_Clear();

        LCD_SetCursor(0,0);
        LCD_String("TIME :");

        sprintf(lcd_buffer,"%02d:%02d:%02d",
                hour,
                minute,
                second);

        LCD_SetCursor(1,0);
        LCD_String(lcd_buffer);

        delay(500);
    }
}