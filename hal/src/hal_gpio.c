#include "hal_gpio.h"

/*****************************************************************************
 * @name       :static uint8_t pin_index(uint16_t pin)
 * @function   :Get pin position index (0..15) from GPIO_Pin bitmask
 * @parameters :pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :Pin index (0..15)
******************************************************************************/
static uint8_t pin_index(uint16_t pin)
{
    uint8_t idx = 0;
    while (idx < 16) {
        if (pin & (1U << idx))
            return idx;
        idx++;
    }
    return 0;
}

/*****************************************************************************
 * @name       :void hal_gpio_init(GPIO_TypeDef *port, uint16_t pin, hal_gpio_mode_t mode)
 * @function   :Configure GPIO pin mode and enable corresponding port clock
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 *              mode: Pin mode (hal_gpio_mode_t)
 * @retvalue   :None
******************************************************************************/
void hal_gpio_init(GPIO_TypeDef *port, uint16_t pin, hal_gpio_mode_t mode)
{
    /* Enable GPIO port clock */
    if (port == GPIOA)      RCC->APB2ENR |= RCC_APB2Periph_GPIOA;
    else if (port == GPIOB) RCC->APB2ENR |= RCC_APB2Periph_GPIOB;
    else if (port == GPIOC) RCC->APB2ENR |= RCC_APB2Periph_GPIOC;

    /* Determine 4-bit configuration (CNF:MODE) */
    uint32_t cfg;
    switch (mode) {
        case HAL_GPIO_OUT_PP:
            cfg = 0x3; /* Output Push-Pull, 50MHz */
            break;
        case HAL_GPIO_IN_PU:
            cfg = 0x8; /* Input with Pull-Up / Pull-Down */
            break;
        case HAL_GPIO_IN_FLOAT:
            cfg = 0x4; /* Floating Input */
            break;
        case HAL_GPIO_AF_PP:
            cfg = 0xB; /* Alternate Function Push-Pull, 50MHz */
            break;
        case HAL_GPIO_AIN:
            cfg = 0x0; /* Analog Input */
            break;
        default:
            cfg = 0x4;
            break;
    }

    /* Configure CRL (pins 0-7) or CRH (pins 8-15) */
    uint8_t pos = pin_index(pin);
    if (pos < 8) {
        uint32_t shift = pos * 4U;
        port->CRL &= ~(0xFU << shift);
        port->CRL |=  (cfg  << shift);
    } else {
        uint32_t shift = (pos - 8U) * 4U;
        port->CRH &= ~(0xFU << shift);
        port->CRH |=  (cfg  << shift);
    }

    /* Enable pull-up via ODR if configured */
    if (mode == HAL_GPIO_IN_PU) {
        port->ODR |= pin;
    }
}

/*****************************************************************************
 * @name       :void hal_gpio_set(GPIO_TypeDef *port, uint16_t pin)
 * @function   :Set GPIO pin to HIGH level
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :None
******************************************************************************/
void hal_gpio_set(GPIO_TypeDef *port, uint16_t pin)
{
    port->BSRR = pin;
}

/*****************************************************************************
 * @name       :void hal_gpio_clear(GPIO_TypeDef *port, uint16_t pin)
 * @function   :Clear GPIO pin to LOW level
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :None
******************************************************************************/
void hal_gpio_clear(GPIO_TypeDef *port, uint16_t pin)
{
    port->BRR = pin;
}

/*****************************************************************************
 * @name       :uint8_t hal_gpio_read(GPIO_TypeDef *port, uint16_t pin)
 * @function   :Read logic level of a GPIO pin
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :1 if HIGH, 0 if LOW
******************************************************************************/
uint8_t hal_gpio_read(GPIO_TypeDef *port, uint16_t pin)
{
    return (port->IDR & pin) ? 1U : 0U;
}
