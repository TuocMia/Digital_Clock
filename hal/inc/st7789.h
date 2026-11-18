/**
 * @file    st7789.h
 * @brief   ST7789 240x240 SPI TFT LCD driver.
 */

#ifndef ST7789_H
#define ST7789_H

#include "stm32f10x_conf.h"
#include "hal_gpio.h"
#include "hal_spi.h"
#include "stdlib.h"

/**
  * @brief ST7789 LCD controller runtime parameters.
  */
typedef struct
{
    u16 width;   /**< Active display width in pixels */
    u16 height;  /**< Active display height in pixels */
    u16 id;      /**< Controller ID */
    u8  dir;     /**< Current scan direction (0..3) */
    u16 wramcmd; /**< Memory write command (0x2C) */
    u16 setxcmd; /**< Column address set command (0x2A) */
    u16 setycmd; /**< Row address set command (0x2B) */
    u8  xoffset; /**< Hardware X offset */
    u8  yoffset; /**< Hardware Y offset */
} _lcd_dev;

extern _lcd_dev lcddev; /**< Global LCD runtime descriptor */

/* User configuration */
#define USE_HORIZONTAL  0   /**< Display orientation index (0..3) */

/* LCD dimensions */
#define LCD_W 240           /**< Panel width in pixels */
#define LCD_H 240           /**< Panel height in pixels */

/* LCD control pins */
#define LCD_GPIO_PORT   GPIOB       /**< GPIO port for LCD control pins */
#define LCD_LED_PIN     GPIO_Pin_9  /**< Backlight control pin (PB9) */
#define LCD_RS_PIN      GPIO_Pin_10 /**< Data/Command select pin (PB10) */
#define LCD_RST_PIN     GPIO_Pin_12 /**< Hardware reset pin (PB12) */

/* Drawing colors */
extern u16 POINT_COLOR; /**< Active foreground RGB565 color */
extern u16 BACK_COLOR;  /**< Active background RGB565 color */

/* Colors */
#define WHITE       0xFFFF  /**< RGB565 White */
#define BLACK       0x0000  /**< RGB565 Black */
#define BLUE        0x001F  /**< RGB565 Blue */
#define BRED        0xF81F  /**< RGB565 Bright Red / Magenta */
#define GRED        0xFFE0  /**< RGB565 Green-Red / Yellow */
#define GBLUE       0x07FF  /**< RGB565 Green-Blue / Cyan */
#define RED         0xF800  /**< RGB565 Red */
#define MAGENTA     0xF81F  /**< RGB565 Magenta */
#define GREEN       0x07E0  /**< RGB565 Green */
#define CYAN        0x7FFF  /**< RGB565 Light Cyan */
#define YELLOW      0xFFE0  /**< RGB565 Yellow */
#define BROWN       0xBC40  /**< RGB565 Brown */
#define BRRED       0xFC07  /**< RGB565 Brownish Red */
#define GRAY        0x8430  /**< RGB565 Gray */
#define GRAY0       0xEF7D  /**< RGB565 Light Gray 0 */
#define GRAY1       0x8410  /**< RGB565 Medium Gray 1 */
#define GRAY2       0x4208  /**< RGB565 Dark Gray 2 */

/**
  * @brief  Initialize SPI2, reset ST7789, configure RGB565 240x240, and turn on backlight.
  */
void LCD_Init(void);

/**
  * @brief  Turn the LCD panel display ON (command 0x29).
  */
void LCD_DisplayOn(void);

/**
  * @brief  Turn the LCD panel display OFF (command 0x28).
  */
void LCD_DisplayOff(void);

/**
  * @brief  Fill the entire 240x240 screen with a single RGB565 color.
  * @param  Color: 16-bit RGB565 fill color.
  */
void LCD_Clear(u16 Color);

/**
  * @brief  Set the active drawing cursor position.
  * @param  Xpos: X coordinate (0..239).
  * @param  Ypos: Y coordinate (0..239).
  */
void LCD_SetCursor(u16 Xpos, u16 Ypos);

/**
  * @brief  Draw a single pixel at (x, y) using POINT_COLOR.
  * @param  x: X coordinate.
  * @param  y: Y coordinate.
  */
void LCD_DrawPoint(u16 x, u16 y);

/**
  * @brief  Set inclusive RAM drawing window and prepare for pixel writes.
  * @param  xStar: Start X coordinate.
  * @param  yStar: Start Y coordinate.
  * @param  xEnd: End X coordinate.
  * @param  yEnd: End Y coordinate.
  */
void LCD_SetWindows(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd);

/**
  * @brief  Write a register and 16-bit value to the LCD controller.
  * @param  LCD_Reg: Register command byte.
  * @param  LCD_RegValue: 16-bit data value.
  */
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);

/**
  * @brief  Write an 8-bit data byte to the LCD.
  * @param  data: Data byte to send.
  */
void LCD_WR_DATA(u8 data);

/**
  * @brief  Send RAM write command (0x2C) to prepare pixel streaming.
  */
void LCD_WriteRAM_Prepare(void);

/**
  * @brief  Write one 16-bit RGB565 pixel to LCD RAM.
  * @param  Data: 16-bit RGB565 pixel color.
  */
void Lcd_WriteData_16Bit(u16 Data);

/**
  * @brief  Set display scan direction / orientation.
  * @param  direction: Orientation index (0..3).
  */
void LCD_direction(u8 direction);

#endif