/*---------------- MAIN ----------------*/

int main(void)
{
    unsigned char rx;

    UART0_Init();

    I2C_Slave_Init();

    UART0_String("\r\n");
    UART0_String("===== I2C SLAVE =====\r\n");

    while(1)
    {
        rx = I2C_Slave_Read();

        UART0_String("Received : ");

        UART0_TxChar(rx);

        UART0_String("\r\n");

        delay_ms(100);
    }
}