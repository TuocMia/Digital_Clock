/**
 * @file    hal_gpio.h
 * @brief   GPIO Hardware Abstraction Layer for STM32F10x.
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include "stm32f10x_conf.h"

/**
 * @brief GPIO pin configuration modes.
 */
typedef enum {
    HAL_GPIO_OUT_PP,   /**< Push-pull output */
    HAL_GPIO_IN_PU,    /**< Input with internal pull-up */
    HAL_GPIO_IN_FLOAT, /**< Floating input */
    HAL_GPIO_AF_PP,    /**< Alternate function push-pull */
    HAL_GPIO_AIN       /**< Analog input */
} hal_gpio_mode_t;

/**
 * @brief  Enable port clock and configure a GPIO pin.
 * @param  port GPIO port pointer (GPIOA..GPIOE).
 * @param  pin  Pin mask (GPIO_Pin_x).
 * @param  mode Pin configuration mode.
 */
void hal_gpio_init(GPIO_TypeDef *port, uint16_t pin, hal_gpio_mode_t mode);

/**
 * @brief  Drive a GPIO output pin high.
 * @param  port GPIO port pointer.
 * @param  pin  Pin mask (GPIO_Pin_x).
 */
void hal_gpio_set(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief  Drive a GPIO output pin low.
 * @param  port GPIO port pointer.
 * @param  pin  Pin mask (GPIO_Pin_x).
 */
void hal_gpio_clear(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief  Read the input logic level of a GPIO pin.
 * @param  port GPIO port pointer.
 * @param  pin  Pin mask (GPIO_Pin_x).
 * @return 1 if pin is high, 0 if pin is low.
 */
uint8_t hal_gpio_read(GPIO_TypeDef *port, uint16_t pin);

#endif
