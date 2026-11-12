#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include "stm32f10x_conf.h"

typedef enum {
    HAL_GPIO_OUT_PP,
    HAL_GPIO_IN_PU,
    HAL_GPIO_IN_FLOAT,
    HAL_GPIO_AF_PP,
    HAL_GPIO_AIN
} hal_gpio_mode_t;

void    hal_gpio_init(GPIO_TypeDef *port, uint16_t pin, hal_gpio_mode_t mode);
void    hal_gpio_set(GPIO_TypeDef *port, uint16_t pin);
void    hal_gpio_clear(GPIO_TypeDef *port, uint16_t pin);
uint8_t hal_gpio_read(GPIO_TypeDef *port, uint16_t pin);

#endif
