/*
  * @file    hal_gpio.c
  * @brief   GPIO Hardware Abstraction Layer implementation for STM32F10x.
  */

#include "hal_gpio.h"

/**
 * @brief  Enable APB2 peripheral clock for the specified GPIO port.
 * @param  port GPIO port pointer (GPIOA..GPIOE).
 */
static void hal_gpio_enable_clock(GPIO_TypeDef *port)
{
    if (port == GPIOA) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }
    else if (port == GPIOB) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    }
    else if (port == GPIOC) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    }
    else if (port == GPIOD) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    }
    else if (port == GPIOE) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    }
}

/**
 * @brief  Enable port clock and configure a GPIO pin at 50 MHz.
 * @param  port GPIO port pointer (GPIOA..GPIOE).
 * @param  pin  GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15).
 * @param  mode Pin operating mode (hal_gpio_mode_t).
 */
void hal_gpio_init(GPIO_TypeDef *port, uint16_t pin, hal_gpio_mode_t mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable clock for the selected GPIO port */
    hal_gpio_enable_clock(port);

    /* Configure GPIO pin parameters */
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* Select GPIO operating mode */
    switch (mode) {
        case HAL_GPIO_OUT_PP:
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
            break;

        case HAL_GPIO_IN_PU:
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            break;

        case HAL_GPIO_IN_FLOAT:
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            break;

        case HAL_GPIO_AF_PP:
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
            break;

        case HAL_GPIO_AIN:
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
            break;

        default:
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            break;
    }

    /* Initialize GPIO using Standard Peripheral Library */
    GPIO_Init(port, &GPIO_InitStructure);
}

/**
 * @brief  Set a GPIO output pin to HIGH level.
 * @param  port GPIO port pointer.
 * @param  pin  GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15).
 */
void hal_gpio_set(GPIO_TypeDef *port, uint16_t pin)
{
    /* Set selected GPIO pin */
    GPIO_SetBits(port, pin);
}

/**
 * @brief  Clear a GPIO output pin to LOW level.
 * @param  port GPIO port pointer.
 * @param  pin  GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15).
 */
void hal_gpio_clear(GPIO_TypeDef *port, uint16_t pin)
{
    /* Reset selected GPIO pin */
    GPIO_ResetBits(port, pin);
}

/**
 * @brief  Read the input logic level of a GPIO pin.
 * @param  port GPIO port pointer.
 * @param  pin  GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15).
 * @retval 1 if HIGH, 0 if LOW.
 */
uint8_t hal_gpio_read(GPIO_TypeDef *port, uint16_t pin)
{
    /* Read input data bit and return normalized value */
    return (GPIO_ReadInputDataBit(port, pin) == Bit_SET) ? 1U : 0U;
}