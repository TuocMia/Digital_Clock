#include "stm32f10x.h"
#include "st7789.h"

static void delay(volatile uint32_t count)
{
    while (count--)
    {
        __NOP();
    }
}

int main(void)
{
    /* Initialize ST7789 LCD */
    LCD_Init();

    /* Test RED */
    LCD_Clear(RED);
    delay(5000000);

    /* Test GREEN */
    LCD_Clear(GREEN);
    delay(5000000);

    /* Test BLUE */
    LCD_Clear(BLUE);
    delay(5000000);

    /* Test WHITE */
    LCD_Clear(WHITE);
    delay(5000000);

    while (1)
    {
    }
}