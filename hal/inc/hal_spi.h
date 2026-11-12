#ifndef HAL_SPI_H
#define HAL_SPI_H

#include "stm32f10x_conf.h"
#include "hal_gpio.h"


u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte);
void SPI2_Init(void);
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet);

#endif