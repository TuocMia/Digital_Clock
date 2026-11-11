#ifndef BSP_PINS_H
#define BSP_PINS_H

#include "stm32f10x.h"

/* LCD ST7789 (SPI2, module 7-pin khong dung chan CS vi CS da noi GND tren board) */
#define LCD_SPI_PORT   GPIOB
#define LCD_SCK_PIN    GPIO_Pin_13
#define LCD_MOSI_PIN   GPIO_Pin_15
#define LCD_RS_PORT    GPIOB
#define LCD_RS_PIN     GPIO_Pin_10
#define LCD_RST_PORT   GPIOB
#define LCD_RST_PIN    GPIO_Pin_12

/* DS1302 (bit-bang 3 day, cap nguon 3V3) */
#define DS_PORT        GPIOA
#define DS_CE_PIN      GPIO_Pin_1
#define DS_CLK_PIN     GPIO_Pin_2
#define DS_IO_PIN      GPIO_Pin_4

/* Nut bam (noi GND, pull-up noi, nhan = 0) */
#define BTN_PORT       GPIOA
#define BTN_PIN        GPIO_Pin_0

/* Bien tro (ADC1 kenh 3 - PA3 nhu so do de cuong) */
#define POT_PORT       GPIOA
#define POT_PIN        GPIO_Pin_3
#define POT_ADC_CH     3

/* Buzzer chu dong (muc cao = keu) */
#define BUZ_PORT       GPIOA
#define BUZ_PIN        GPIO_Pin_8

/* LED tren Blue Pill (muc thap = sang) */
#define LED_PORT       GPIOC
#define LED_PIN        GPIO_Pin_13

#endif
