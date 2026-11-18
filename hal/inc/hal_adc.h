/**
  ******************************************************************************
  * @file    hal_adc.h
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   ADC1 Hardware Abstraction Layer for potentiometer on PA3.
  ******************************************************************************
  */

#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>

/**
  * @brief ADC HAL status codes.
  */
typedef enum {
    HAL_ADC_OK = 0,           /**< Conversion or init succeeded */
    HAL_ADC_INVALID_ARGUMENT, /**< Null pointer argument */
    HAL_ADC_TIMEOUT           /**< Calibration or conversion timeout */
} hal_adc_status_t;

/**
  * @brief  Initialize ADC1 and configure PA3 (channel 3) for analog input.
  * @retval HAL_ADC_OK on success, HAL_ADC_TIMEOUT if calibration fails.
  */
hal_adc_status_t hal_adc_init(void);

/**
  * @brief  Start one conversion and return its 12-bit result (0..4095).
  * @param  value: Pointer to store the 12-bit conversion result.
  * @retval HAL_ADC_OK, HAL_ADC_INVALID_ARGUMENT, or HAL_ADC_TIMEOUT.
  */
hal_adc_status_t hal_adc_read(uint16_t *value);

#endif
