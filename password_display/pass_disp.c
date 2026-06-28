#include <LPC214x.h>
#include <string.h>

#define RS (1<<0)
#define EN (1<<1)

char password[]="1234";
char entered[5];

void delay_ms(unsigned int ms)
{
    unsigned int i,j;

    for(i=0;i<ms;i++)
    {
        for(j=0;j<6000;j++);
    }
}

void lcd_enable()
{
    IO0SET=EN;
    delay_ms(2);
    IO0CLR=EN;
    delay_ms(2);
}

void lcd_cmd(unsigned char cmd)
{
    IO0CLR=RS;

    IO0CLR=0x000000F0;
    IO0SET=(cmd&0xF0);

    lcd_enable();

    IO0CLR=0x000000F0;
    IO0SET=((cmd<<4)&0xF0);

    lcd_enable();

    delay_ms(2);
}

void lcd_data(unsigned char data)
{
    IO0SET=RS;

    IO0CLR=0x000000F0;
    IO0SET=(data&0xF0);

    lcd_enable();

    IO0CLR=0x000000F0;
    IO0SET=((data<<4)&0xF0);

    lcd_enable();

    delay_ms(2);
}

void lcd_string(char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}

void lcd_init()
{
    IODIR0 |= 0x000000F3;

    delay_ms(20);

    lcd_cmd(0x02);

    lcd_cmd(0x28);

    lcd_cmd(0x0C);

    lcd_cmd(0x06);

    lcd_cmd(0x01);

    delay_ms(5);
}

char keypad_scan()
{
    char key = 0;

    IO1DIR |= 0x000F0000;          // P1.16-P1.19 as OUTPUT (Rows)

    IO1DIR &= ~(0x00700000);       // P1.20-P1.22 as INPUT (Columns)

    IO1SET = 0x000F0000;           // Make all rows HIGH

    //--------------- Row A ----------------

    IO1CLR = (1<<16);
    IO1SET = (1<<17)|(1<<18)|(1<<19);

    if(!(IO1PIN & (1<<20)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<20)));
        return '1';
    }

    if(!(IO1PIN & (1<<21)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<21)));
        return '2';
    }

    if(!(IO1PIN & (1<<22)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<22)));
        return '3';
    }

    //---------------- Row B ----------------

    IO1CLR = (1<<17);
    IO1SET = (1<<16)|(1<<18)|(1<<19);

    if(!(IO1PIN & (1<<20)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<20)));
        return '4';
    }

    if(!(IO1PIN & (1<<21)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<21)));
        return '5';
    }

    if(!(IO1PIN & (1<<22)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<22)));
        return '6';
    }

    //---------------- Row C ----------------

    IO1CLR = (1<<18);
    IO1SET = (1<<16)|(1<<17)|(1<<19);

    if(!(IO1PIN & (1<<20)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<20)));
        return '7';
    }

    if(!(IO1PIN & (1<<21)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<21)));
        return '8';
    }

    if(!(IO1PIN & (1<<22)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<22)));
        return '9';
    }

    //---------------- Row D ----------------

    IO1CLR = (1<<19);
    IO1SET = (1<<16)|(1<<17)|(1<<18);

    if(!(IO1PIN & (1<<20)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<20)));
        return '*';
    }

    if(!(IO1PIN & (1<<21)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<21)));
        return '0';
    }

    if(!(IO1PIN & (1<<22)))
    {
        delay_ms(20);
        while(!(IO1PIN&(1<<22)));
        return '#';
    }

    return key;
}

int main()
{
    char key;
    unsigned char i=0;

    /* LED Output */
    IO1DIR |= (1<<31);
    IO1CLR = (1<<31);          // LED OFF

    lcd_init();

    while(1)
    {
        lcd_cmd(0x01);
        lcd_cmd(0x80);
        lcd_string("ENTER PASSWORD");

        lcd_cmd(0xC0);

        i=0;

        while(1)
        {
            key = keypad_scan();

            if(key==0)
                continue;

            /* Clear */
            if(key=='*')
            {
                i=0;

                lcd_cmd(0xC0);
                lcd_string("                ");
                lcd_cmd(0xC0);

                continue;
            }

            /* Verify */
            if(key=='#')
            {
                entered[i]='\0';

                lcd_cmd(0x01);

                if(strcmp(entered,password)==0)
                {
                    lcd_cmd(0x80);
                    lcd_string("ACCESS GRANTED");

                    IO1SET = (1<<31);

                    delay_ms(3000);
                }
                else
                {
                    lcd_cmd(0x80);
                    lcd_string("WRONG PASSWORD");

                    IO1CLR = (1<<31);

                    delay_ms(3000);
                }

                break;
            }

            /* Store only first 4 digits */
            if(i<4)
            {
                entered[i++] = key;

                lcd_data('*');
            }
        }
    }
}