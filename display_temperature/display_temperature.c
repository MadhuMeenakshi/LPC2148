#include <LPC214x.h>
#include <stdio.h>

#define RS (1<<16)
#define EN (1<<17)

void delay_ms(unsigned int ms)
{
    unsigned int i,j;

    for(i=0;i<ms;i++)
    {
        for(j=0;j<6000;j++);
    }
}

void lcd_cmd(unsigned char cmd)
{
    IOCLR0 = RS;

    IOCLR0 = 0x003C0000;
    IOSET0 = ((cmd & 0xF0) << 14);

    IOSET0 = EN;
    delay_ms(2);
    IOCLR0 = EN;

    IOCLR0 = 0x003C0000;
    IOSET0 = ((cmd & 0x0F) << 18);

    IOSET0 = EN;
    delay_ms(2);
    IOCLR0 = EN;

    delay_ms(2);
}

void lcd_data(unsigned char data)
{
    IOSET0 = RS;

    IOCLR0 = 0x003C0000;
    IOSET0 = ((data & 0xF0) << 14);

    IOSET0 = EN;
    delay_ms(2);
    IOCLR0 = EN;

    IOCLR0 = 0x003C0000;
    IOSET0 = ((data & 0x0F) << 18);

    IOSET0 = EN;
    delay_ms(2);
    IOCLR0 = EN;

    delay_ms(2);
}

void lcd_string(char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}

void lcd_init(void)
{
    IODIR0 |= 0x003F0000;

    delay_ms(20);

    lcd_cmd(0x02);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);

    delay_ms(5);
}

void adc_init(void)
{
    PINSEL1 &= ~(3<<24);
    PINSEL1 |=  (1<<24);

    AD0CR =
    (1<<1) |
    (4<<8) |
    (1<<21);
}

unsigned int adc_read(void)
{
    unsigned int value;

    AD0CR &= ~(7<<24);
    AD0CR |=  (1<<24);

    while(!(AD0GDR & (1UL<<31)));

    value = (AD0GDR >> 6) & 0x3FF;

    return value;
}

int main(void)
{
    unsigned int adc;
    float voltage;
    float temperature;

    char buffer[16];

    lcd_init();
    adc_init();

    while(1)
    {
        adc = adc_read();

        voltage = ((float)adc * 3.3f) / 1023.0f;

        temperature = voltage * 100.0f;

        lcd_cmd(0x80);
        lcd_string("Temp:");

        sprintf(buffer,"%2.1f C",temperature);

        lcd_cmd(0xC0);
        lcd_string("                ");

        lcd_cmd(0xC0);
        lcd_string(buffer);

        delay_ms(500);
    }
}