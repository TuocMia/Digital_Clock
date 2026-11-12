#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>

/* ADC1 channel 3 is connected to PA3 on STM32F103C8T6. */
typedef enum {
    HAL_ADC_OK = 0,
    HAL_ADC_INVALID_ARGUMENT,
    HAL_ADC_TIMEOUT
} hal_adc_status_t;

/* Initialize ADC1 and configure PA3 for analog input. */
hal_adc_status_t hal_adc_init(void);

/* Start one conversion and store its 12-bit result in value. */
hal_adc_status_t hal_adc_read(uint16_t *value);

#endif
