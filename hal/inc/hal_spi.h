/**
 * @file    hal_spi.h
 * @brief   SPI2 Hardware Abstraction Layer for ST7789 LCD.
 */

#ifndef HAL_SPI_H
#define HAL_SPI_H

#include "stm32f10x_conf.h"
#include "hal_gpio.h"

/**
 * @brief  Transmit and receive one byte over SPI.
 * @param  SPIx Pointer to SPI peripheral (e.g. SPI2).
 * @param  Byte Byte to transmit.
 * @return Received byte.
 */
u8 SPI_WriteByte(SPI_TypeDef *SPIx, u8 Byte);

/**
 * @brief  Initialize SPI2 in master mode on PB13 (SCK), PB14 (MISO), PB15 (MOSI).
 */
void SPI2_Init(void);

/**
 * @brief  Adjust the SPI baud rate prescaler.
 * @param  SPIx     Pointer to SPI peripheral.
 * @param  SpeedSet 1 for high speed (div 2), 0 for low speed (div 32).
 */
void SPI_SetSpeed(SPI_TypeDef *SPIx, u8 SpeedSet);

#endif
