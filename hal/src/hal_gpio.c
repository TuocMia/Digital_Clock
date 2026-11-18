/*
  * @file    hal_gpio.c
  * @brief   GPIO Hardware Abstraction Layer implementation for STM32F10x.
  */

#include "hal_gpio.h"

/*
  * @brief  Enable clock for the specified GPIO port
  * @param  port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
  * @retval None
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

/*
  * @brief  Configure GPIO pin mode and enable corresponding port clock
  * @param  port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
  * @param  pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
  * @param  mode: Pin mode (hal_gpio_mode_t)
  * @retval None
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

/*
  * @brief  Set GPIO pin to HIGH level
  * @param  port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
  * @param  pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
  * @retval None
  */
void hal_gpio_set(GPIO_TypeDef *port, uint16_t pin)
{
    /* Set selected GPIO pin */
    GPIO_SetBits(port, pin);
}

/*
  * @brief  Clear GPIO pin to LOW level
  * @param  port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
  * @param  pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
  * @retval None
  */
void hal_gpio_clear(GPIO_TypeDef *port, uint16_t pin)
{
    /* Reset selected GPIO pin */
    GPIO_ResetBits(port, pin);
}

/*
  * @brief  Read logic level of a GPIO pin
  * @param  port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
  * @param  pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
  * @retval 1 if HIGH, 0 if LOW
  */
uint8_t hal_gpio_read(GPIO_TypeDef *port, uint16_t pin)
{
    /* Read input data bit and return normalized value */
    return (GPIO_ReadInputDataBit(port, pin) == Bit_SET) ? 1U : 0U;
}