#include "st7789.h"

/* LCD device parameters */
_lcd_dev lcddev;

/* Drawing colors */
u16 POINT_COLOR = BLACK;
u16 BACK_COLOR  = WHITE;

/*****************************************************************************
 * @name       :void LCD_Delay(volatile u32 count)
 * @function   :Simple delay function for LCD initialization
 * @parameters :count: Delay loop count
 * @retvalue   :None
 ******************************************************************************/
static void LCD_Delay(volatile u32 count)
{
    while (count--)
    {
        __NOP();
    }
}

/*****************************************************************************
 * @name       :void LCD_GPIOInit(void)
 * @function   :Initialize LCD control GPIO pins using HAL GPIO
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
static void LCD_GPIOInit(void)
{
    /* Configure LCD control pins as push-pull outputs */
    hal_gpio_init(LCD_GPIO_PORT,
                  LCD_LED_PIN |
                  LCD_RS_PIN |
                  LCD_RST_PIN,
                  HAL_GPIO_OUT_PP);

    /* Set initial pin states */
    hal_gpio_set(LCD_GPIO_PORT,
                 LCD_LED_PIN |
                 LCD_RS_PIN |
                 LCD_RST_PIN);
}

/*****************************************************************************
 * @name       :void LCD_Reset(void)
 * @function   :Reset ST7789 LCD
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
static void LCD_Reset(void)
{
    /* Reset LCD */
    hal_gpio_clear(LCD_GPIO_PORT, LCD_RST_PIN);
    LCD_Delay(100000);

    /* Release reset */
    hal_gpio_set(LCD_GPIO_PORT, LCD_RST_PIN);
    LCD_Delay(100000);
}

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @function   :Write one command to LCD
 * @parameters :data: Command byte
 * @retvalue   :None
 ******************************************************************************/
void LCD_WR_REG(u8 data)
{
    /* Select command mode */
    hal_gpio_clear(LCD_GPIO_PORT, LCD_RS_PIN);

    /* Send command through HAL SPI */
    SPI_WriteByte(SPI2, data);
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
 * @function   :Write one data byte to LCD
 * @parameters :data: Data byte
 * @retvalue   :None
 ******************************************************************************/
void LCD_WR_DATA(u8 data)
{
    /* Select data mode */
    hal_gpio_set(LCD_GPIO_PORT, LCD_RS_PIN);

    /* Send data through HAL SPI */
    SPI_WriteByte(SPI2, data);
}

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @function   :Write 16-bit data to LCD
 * @parameters :Data: 16-bit data
 * @retvalue   :None
 ******************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{
    /* Select data mode */
    hal_gpio_set(LCD_GPIO_PORT, LCD_RS_PIN);

    /* Send high byte */
    SPI_WriteByte(SPI2, Data >> 8);

    /* Send low byte */
    SPI_WriteByte(SPI2, Data);
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @function   :Prepare LCD for RAM write operation
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(0x2C);
}

/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,
 *                                  u16 xEnd, u16 yEnd)
 * @function   :Set LCD drawing window
 * @parameters :xStar: Start X coordinate
 *              yStar: Start Y coordinate
 *              xEnd : End X coordinate
 *              yEnd : End Y coordinate
 * @retvalue   :None
 ******************************************************************************/
void LCD_SetWindows(u16 xStar, u16 yStar,
                    u16 xEnd, u16 yEnd)
{
    /* Set column address */
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(xStar & 0xFF);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(xEnd & 0xFF);

    /* Set row address */
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(yStar & 0xFF);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(yEnd & 0xFF);

    /* Start memory write */
    LCD_WriteRAM_Prepare();
}

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @function   :Set LCD drawing cursor position
 * @parameters :Xpos: X coordinate
 *              Ypos: Y coordinate
 * @retvalue   :None
 ******************************************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    LCD_SetWindows(Xpos, Ypos, Xpos, Ypos);
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x, u16 y)
 * @function   :Draw one pixel using POINT_COLOR
 * @parameters :x: X coordinate
 *              y: Y coordinate
 * @retvalue   :None
 ******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);
    Lcd_WriteData_16Bit(POINT_COLOR);
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @function   :Fill the LCD screen with a specified color
 * @parameters :Color: Fill color
 * @retvalue   :None
 ******************************************************************************/
void LCD_Clear(u16 Color)
{
    u32 i;
    u32 pixel_count;

    /* Set drawing window to full screen */
    LCD_SetWindows(0, 0,
                   lcddev.width - 1,
                   lcddev.height - 1);

    /* Select data mode */
    hal_gpio_set(LCD_GPIO_PORT, LCD_RS_PIN);

    /* Calculate total number of pixels */
    pixel_count = (u32)lcddev.width * lcddev.height;

    /* Fill screen */
    for (i = 0; i < pixel_count; i++)
    {
        SPI_WriteByte(SPI2, Color >> 8);
        SPI_WriteByte(SPI2, Color & 0xFF);
    }
}

/*****************************************************************************
 * @name       :void LCD_DisplayOn(void)
 * @function   :Turn on LCD display
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0x29);
}

/*****************************************************************************
 * @name       :void LCD_DisplayOff(void)
 * @function   :Turn off LCD display
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0x28);
}

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @function   :Set LCD display direction
 * @parameters :direction: Display direction
 * @retvalue   :None
 ******************************************************************************/
void LCD_direction(u8 direction)
{
    lcddev.dir = direction;

    if (direction == 0)
    {
        lcddev.width = LCD_W;
        lcddev.height = LCD_H;

        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x00);
    }
    else if (direction == 1)
    {
        lcddev.width = LCD_H;
        lcddev.height = LCD_W;

        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x60);
    }
}

/*****************************************************************************
 * @name       :void LCD_Init(void)
 * @function   :Initialize ST7789 LCD
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_Init(void)
{
    /* Initialize LCD control GPIO */
    LCD_GPIOInit();

    /* Initialize SPI2 through HAL */
    SPI2_Init();

    /* Reset LCD */
    LCD_Reset();

    /* Software reset */
    LCD_WR_REG(0x01);
    LCD_Delay(200000);

    /* Sleep out */
    LCD_WR_REG(0x11);
    LCD_Delay(200000);

    /* Color mode: RGB565 */
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);

    /* Memory data access control */
    LCD_WR_REG(0x36);

#if USE_HORIZONTAL == 0
    LCD_WR_DATA(0x00);
#elif USE_HORIZONTAL == 1
    LCD_WR_DATA(0x60);
#elif USE_HORIZONTAL == 2
    LCD_WR_DATA(0xC0);
#elif USE_HORIZONTAL == 3
    LCD_WR_DATA(0xA0);
#endif

    /* Display inversion on */
    LCD_WR_REG(0x21);

    /* Normal display mode */
    LCD_WR_REG(0x13);

    /* Display on */
    LCD_WR_REG(0x29);

    /* Configure LCD parameters */
    lcddev.width = LCD_W;
    lcddev.height = LCD_H;
    lcddev.dir = USE_HORIZONTAL;
    lcddev.wramcmd = 0x2C;
    lcddev.setxcmd = 0x2A;
    lcddev.setycmd = 0x2B;

    /* Turn on LCD backlight */
    hal_gpio_set(LCD_GPIO_PORT, LCD_LED_PIN);

    /* Clear screen */
    LCD_Clear(BLACK);
}