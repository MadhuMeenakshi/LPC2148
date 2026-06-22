#include <LPC214x.h>

unsigned char digit[10] =
{
    0x3F,0x06,0x5B,0x4F,0x66,
    0x6D,0x7D,0x07,0x7F,0x6F
};

void delay_ms(unsigned int ms)
{
    unsigned int i,j;
    for(i=0;i<ms;i++)
        for(j=0;j<6000;j++);
}

void display(unsigned int num)
{
    IOCLR0 = 0x7F;
    IOSET0 = digit[num];
}

int main()
{
    unsigned int count = 0;

    IODIR0 |= 0x7F;      // 7-segment outputs
    IODIR0 &= ~(1<<14);  // button input

    display(count);

    while(1)
    {
        if(!(IOPIN0 & (1<<14)))
        {
            delay_ms(20);

            if(!(IOPIN0 & (1<<14)))
            {
                count++;

                if(count > 9)
                    count = 0;

                display(count);

                while(!(IOPIN0 & (1<<14)));
            }
        }
    }
}