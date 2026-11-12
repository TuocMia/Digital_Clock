#include "hal_gpio.h"

/*****************************************************************************
<<<<<<< Updated upstream
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
=======
 * @name       :static void hal_gpio_enable_clock(GPIO_TypeDef *port)
 * @function   :Enable clock for the specified GPIO port
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
 * @retvalue   :None
 ******************************************************************************/
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

/*****************************************************************************
 * @name       :void hal_gpio_init(GPIO_TypeDef *port, uint16_t pin,
 *                                  hal_gpio_mode_t mode)
 * @function   :Configure GPIO pin mode and enable corresponding port clock
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 *              mode: Pin mode (hal_gpio_mode_t)
 * @retvalue   :None
 ******************************************************************************/
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
>>>>>>> Stashed changes
}

/*****************************************************************************
 * @name       :void hal_gpio_set(GPIO_TypeDef *port, uint16_t pin)
 * @function   :Set GPIO pin to HIGH level
<<<<<<< Updated upstream
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :None
******************************************************************************/
void hal_gpio_set(GPIO_TypeDef *port, uint16_t pin)
{
    port->BSRR = pin;
=======
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :None
 ******************************************************************************/
void hal_gpio_set(GPIO_TypeDef *port, uint16_t pin)
{
    /* Set selected GPIO pin */
    GPIO_SetBits(port, pin);
>>>>>>> Stashed changes
}

/*****************************************************************************
 * @name       :void hal_gpio_clear(GPIO_TypeDef *port, uint16_t pin)
 * @function   :Clear GPIO pin to LOW level
<<<<<<< Updated upstream
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :None
******************************************************************************/
void hal_gpio_clear(GPIO_TypeDef *port, uint16_t pin)
{
    port->BRR = pin;
=======
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :None
 ******************************************************************************/
void hal_gpio_clear(GPIO_TypeDef *port, uint16_t pin)
{
    /* Reset selected GPIO pin */
    GPIO_ResetBits(port, pin);
>>>>>>> Stashed changes
}

/*****************************************************************************
 * @name       :uint8_t hal_gpio_read(GPIO_TypeDef *port, uint16_t pin)
 * @function   :Read logic level of a GPIO pin
<<<<<<< Updated upstream
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :1 if HIGH, 0 if LOW
******************************************************************************/
uint8_t hal_gpio_read(GPIO_TypeDef *port, uint16_t pin)
{
    return (port->IDR & pin) ? 1U : 0U;
}
=======
 * @parameters :port: GPIO port (GPIOA, GPIOB, GPIOC, ...)
 *              pin: GPIO pin mask (GPIO_Pin_0 .. GPIO_Pin_15)
 * @retvalue   :1 if HIGH, 0 if LOW
 ******************************************************************************/
uint8_t hal_gpio_read(GPIO_TypeDef *port, uint16_t pin)
{
    /* Read input data bit and return normalized value */
    return (GPIO_ReadInputDataBit(port, pin) == Bit_SET) ? 1U : 0U;
}
>>>>>>> Stashed changes
