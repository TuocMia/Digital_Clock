/**
 * @file    bsp_pins.h
 * @brief   GPIO pin mapping for STM32F103C8T6 Blue Pill peripherals.
 */

#ifndef BSP_PINS_H
#define BSP_PINS_H

#include "stm32f10x.h"

/* LCD ST7789 (SPI2, module 7-pin khong dung chan CS vi CS da noi GND tren board) */
#define LCD_SPI_PORT   GPIOB        /**< GPIO port for LCD SPI2 pins */
#define LCD_SCK_PIN    GPIO_Pin_13  /**< SPI2 SCK pin (PB13) */
#define LCD_MOSI_PIN   GPIO_Pin_15  /**< SPI2 MOSI pin (PB15) */
#define LCD_RS_PORT    GPIOB        /**< GPIO port for LCD RS/BL pins */
#define LCD_LED_PIN    GPIO_Pin_9   /**< LCD backlight control pin (PB9) */
#define LCD_RS_PIN     GPIO_Pin_10  /**< LCD Data/Command select pin (PB10) */
#define LCD_RST_PORT   GPIOB        /**< GPIO port for LCD reset pin */
#define LCD_RST_PIN    GPIO_Pin_12  /**< LCD reset pin (PB12) */

/* DS1302 (bit-bang 3 day, khop voi hal_ds1302.h) */
#define DS_PORT        GPIOA        /**< GPIO port for DS1302 3-wire bus */
#define DS_CE_PIN      GPIO_Pin_4   /**< DS1302 Chip Enable pin (PA4) */
#define DS_CLK_PIN     GPIO_Pin_5   /**< DS1302 Serial Clock pin (PA5) */
#define DS_IO_PIN      GPIO_Pin_7   /**< DS1302 Bidirectional Data pin (PA7) */

/* Nut bam (noi GND, pull-up noi, nhan = 0) */
#define BTN_PORT       GPIOA        /**< GPIO port for user push button */
#define BTN_PIN        GPIO_Pin_0   /**< User push button pin (PA0, active-low) */

/* Bien tro (ADC1 kenh 3 - PA3, khop voi hal_adc.c) */
#define POT_PORT       GPIOA        /**< GPIO port for potentiometer ADC input */
#define POT_PIN        GPIO_Pin_3   /**< Potentiometer analog pin (PA3) */
#define POT_ADC_CH     3            /**< ADC1 channel index for potentiometer */

/* Buzzer chu dong (muc cao = keu) */
#define BUZ_PORT       GPIOA        /**< GPIO port for active buzzer */
#define BUZ_PIN        GPIO_Pin_8   /**< Active buzzer pin (PA8, active-high) */

/* LED tren Blue Pill (muc thap = sang) */
#define LED_PORT       GPIOC        /**< GPIO port for onboard Blue Pill LED */
#define LED_PIN        GPIO_Pin_13  /**< Onboard LED pin (PC13, active-low) */

#endif
