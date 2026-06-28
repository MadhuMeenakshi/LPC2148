#include <LPC213x.h>
#include <stdio.h>

#define RS  (1<<16)
#define EN  (1<<17)

#define D4  (1<<18)
#define D5  (1<<19)
#define D6  (1<<20)
#define D7  (1<<21)

void delay_ms(unsigned int ms)
{
    unsigned int i,j;

    for(i=0;i<ms;i++)
        for(j=0;j<6000;j++);
}

void lcd_pulse(void)
{
    IO0SET = EN;
    delay_ms(2);

    IO0CLR = EN;
    delay_ms(2);
}

void lcd_send4(unsigned char data)
{
    IO0CLR = D4|D5|D6|D7;

    if(data&0x01) IO0SET=D4;
    if(data&0x02) IO0SET=D5;
    if(data&0x04) IO0SET=D6;
    if(data&0x08) IO0SET=D7;

    lcd_pulse();
}

void lcd_cmd(unsigned char cmd)
{
    IO0CLR=RS;

    lcd_send4(cmd>>4);
    lcd_send4(cmd);

    delay_ms(2);
}

void lcd_data(unsigned char data)
{
    IO0SET=RS;

    lcd_send4(data>>4);
    lcd_send4(data);

    delay_ms(2);
}

void lcd_string(char *s)
{
    while(*s)
        lcd_data(*s++);
}

void lcd_init(void)
{
    IO0DIR |= RS|EN|D4|D5|D6|D7;

    delay_ms(20);

    lcd_send4(0x03);
    delay_ms(5);

    lcd_send4(0x03);
    delay_ms(5);

    lcd_send4(0x03);
    delay_ms(5);

    lcd_send4(0x02);

    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);

    delay_ms(5);
}

/*-------------------------------------------------
              ADC FUNCTIONS
--------------------------------------------------*/

void adc_init(void)
{
    /* Configure P0.28 as AD0.1 */

    PINSEL1 &= ~(3 << 24);
    PINSEL1 |=  (1 << 24);

    AD0CR = (1 << 1)      |   // Select AD0.1
            (4 << 8)      |   // ADC Clock Divider
            (1 << 21);        // Power ON ADC
}


unsigned int adc_read(void)
{
    unsigned int value;

    AD0CR &= ~(7 << 24);      // Clear START bits
    AD0CR |=  (1 << 24);      // Start Conversion

    while(!(AD0DR1 & (1UL << 31)));   // Wait till DONE

    value = (AD0DR1 >> 6) & 0x3FF;

    return value;
}

/*-------------------------------------------------
                PWM FUNCTIONS
--------------------------------------------------*/

void pwm_init(void)
{
    /* Configure P0.0 as PWM1 */

    PINSEL0 &= ~(3 << 0);
    PINSEL0 |=  (2 << 0);

    PWMPR = 59;          // Prescaler (depends on PCLK)

    PWMMR0 = 1000;       // PWM Period
    PWMMR1 = 0;          // Initial Duty = 0%

    PWMMCR = (1 << 1);   // Reset on MR0

    PWMLER = (1 << 0) | (1 << 1);

    PWMPCR = (1 << 9);   // Enable PWM1 output

    PWMTCR = (1 << 1);   // Reset Counter
    PWMTCR = (1 << 0) | (1 << 3); // Counter + PWM Enable
}