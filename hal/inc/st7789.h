#ifndef ST7789_H
#define ST7789_H

#include "stm32f10x_conf.h"
#include "hal_gpio.h"
#include "hal_spi.h"
#include "stdlib.h"

/* LCD parameter structure */
typedef struct
{
    u16 width;
    u16 height;
    u16 id;
    u8  dir;
    u16 wramcmd;
    u16 setxcmd;
    u16 setycmd;
    u8  xoffset;
    u8  yoffset;
} _lcd_dev;

extern _lcd_dev lcddev;

/* User configuration */
#define USE_HORIZONTAL  0

/* LCD dimensions */
#define LCD_W 240
#define LCD_H 240

/* LCD control pins */
#define LCD_GPIO_PORT   GPIOB
#define LCD_LED_PIN     GPIO_Pin_9
#define LCD_RS_PIN      GPIO_Pin_10
#define LCD_RST_PIN     GPIO_Pin_12

/* Drawing colors */
extern u16 POINT_COLOR;
extern u16 BACK_COLOR;

/* Colors */
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define BRED        0xF81F
#define GRED        0xFFE0
#define GBLUE       0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN       0xBC40
#define BRRED       0xFC07
#define GRAY        0x8430
#define GRAY0       0xEF7D
#define GRAY1       0x8410
#define GRAY2       0x4208

void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(u16 Color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x, u16 y);
void LCD_SetWindows(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd);

void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_DATA(u8 data);
void LCD_WriteRAM_Prepare(void);
void Lcd_WriteData_16Bit(u16 Data);
void LCD_direction(u8 direction);

#endif