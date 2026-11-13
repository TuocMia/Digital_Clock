#ifndef HAL_DS1302_H
#define HAL_DS1302_H

#include <stdint.h>
#include "stm32f10x_conf.h"

/* GPIOA mapping: PA0 is reserved for the button and PA3 for the ADC pot. */
#define HAL_DS1302_CE_PORT   GPIOA
#define HAL_DS1302_CE_PIN    GPIO_Pin_4
#define HAL_DS1302_SCLK_PORT GPIOA
#define HAL_DS1302_SCLK_PIN  GPIO_Pin_5
#define HAL_DS1302_IO_PORT   GPIOA
#define HAL_DS1302_IO_PIN    GPIO_Pin_7

typedef struct {
    uint8_t seconds; /* 0..59 */
    uint8_t minutes; /* 0..59 */
    uint8_t hours;   /* 0..23 */
    uint8_t date;    /* 1..31 */
    uint8_t month;   /* 1..12 */
    uint8_t weekday; /* 1..7 */
    uint8_t year;    /* 0..99, representing 2000..2099 */
} hal_ds1302_datetime_t;

typedef enum {
    HAL_DS1302_OK = 0,
    HAL_DS1302_INVALID_ARGUMENT,
    HAL_DS1302_INVALID_DATA,
    HAL_DS1302_CLOCK_STOPPED
} hal_ds1302_status_t;

/* Configure CE, SCLK, and bidirectional IO pins through hal_gpio. */
void hal_ds1302_init(void);

/* Read and validate the current date/time registers. */
hal_ds1302_status_t hal_ds1302_read_time(hal_ds1302_datetime_t *datetime);

/* Validate and write date/time in 24-hour format. */
hal_ds1302_status_t hal_ds1302_write_time(const hal_ds1302_datetime_t *datetime);

/* Read one byte from DS1302 31-byte battery-backed RAM (addr: 0..30). */
hal_ds1302_status_t hal_ds1302_read_ram(uint8_t addr, uint8_t *value);

/* Write one byte to DS1302 31-byte battery-backed RAM (addr: 0..30). */
hal_ds1302_status_t hal_ds1302_write_ram(uint8_t addr, uint8_t value);

#endif
