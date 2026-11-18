/**
  ******************************************************************************
  * @file    hal_ds1302.h
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   DS1302 RTC driver (bit-banged 3-wire + 31-byte battery-backed RAM).
  ******************************************************************************
  */

#ifndef HAL_DS1302_H
#define HAL_DS1302_H

#include <stdint.h>
#include "stm32f10x_conf.h"

/* GPIOA mapping: PA0 is reserved for the button and PA3 for the ADC pot. */
#define HAL_DS1302_CE_PORT   GPIOA       /**< GPIO port for DS1302 CE line */
#define HAL_DS1302_CE_PIN    GPIO_Pin_4  /**< DS1302 Chip Enable pin (PA4) */
#define HAL_DS1302_SCLK_PORT GPIOA       /**< GPIO port for DS1302 SCLK line */
#define HAL_DS1302_SCLK_PIN  GPIO_Pin_5  /**< DS1302 Serial Clock pin (PA5) */
#define HAL_DS1302_IO_PORT   GPIOA       /**< GPIO port for DS1302 IO line */
#define HAL_DS1302_IO_PIN    GPIO_Pin_7  /**< DS1302 Bidirectional Data pin (PA7) */

/**
  * @brief Date and time structure in binary (24-hour) format.
  */
typedef struct {
    uint8_t seconds; /**< 0..59 */
    uint8_t minutes; /**< 0..59 */
    uint8_t hours;   /**< 0..23 */
    uint8_t date;    /**< 1..31 */
    uint8_t month;   /**< 1..12 */
    uint8_t weekday; /**< 1..7 */
    uint8_t year;    /**< 0..99, representing 2000..2099 */
} hal_ds1302_datetime_t;

/**
  * @brief DS1302 driver status codes.
  */
typedef enum {
    HAL_DS1302_OK = 0,           /**< Operation succeeded */
    HAL_DS1302_INVALID_ARGUMENT, /**< Invalid pointer or out-of-range field */
    HAL_DS1302_INVALID_DATA,     /**< Corrupted BCD or out-of-range RTC registers */
    HAL_DS1302_CLOCK_STOPPED     /**< Clock Halt (CH) bit is set */
} hal_ds1302_status_t;

/**
  * @brief  Configure CE, SCLK, and bidirectional IO pins through hal_gpio.
  */
void hal_ds1302_init(void);

/**
  * @brief  Read and validate the current date/time registers.
  * @param  datetime: Pointer to output date/time structure.
  * @retval HAL_DS1302_OK, HAL_DS1302_CLOCK_STOPPED, or error status.
  */
hal_ds1302_status_t hal_ds1302_read_time(hal_ds1302_datetime_t *datetime);

/**
  * @brief  Validate and write date/time in 24-hour format.
  * @param  datetime: Pointer to input date/time structure.
  * @retval HAL_DS1302_OK or HAL_DS1302_INVALID_ARGUMENT.
  */
hal_ds1302_status_t hal_ds1302_write_time(const hal_ds1302_datetime_t *datetime);

/**
  * @brief  Read one byte from DS1302 31-byte battery-backed RAM.
  * @param  addr: RAM byte index (0..30).
  * @param  value: Pointer to store the read byte.
  * @retval HAL_DS1302_OK or HAL_DS1302_INVALID_ARGUMENT.
  */
hal_ds1302_status_t hal_ds1302_read_ram(uint8_t addr, uint8_t *value);

/**
  * @brief  Write one byte to DS1302 31-byte battery-backed RAM.
  * @param  addr: RAM byte index (0..30).
  * @param  value: Byte value to write.
  * @retval HAL_DS1302_OK or HAL_DS1302_INVALID_ARGUMENT.
  */
hal_ds1302_status_t hal_ds1302_write_ram(uint8_t addr, uint8_t value);

#endif
