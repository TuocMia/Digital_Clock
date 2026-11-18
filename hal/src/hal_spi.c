/*
  * @file    hal_spi.c
  * @brief   SPI2 Hardware Abstraction Layer implementation for ST7789 LCD.
  */

#include "hal_spi.h"

/*
  * @brief  Write and receive one byte using STM32 hardware SPI
  * @param  SPIx: SPI peripheral (SPI1, SPI2, SPI3)
  * @param  Byte: Data to be transmitted
  * @retval Data received from the SPI bus
  */
u8 SPI_WriteByte(SPI_TypeDef *SPIx, u8 Byte)
{
    /* Wait for TX buffer empty */
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte */
    SPI_I2S_SendData(SPIx, Byte);

    /* Wait for RX buffer not empty */
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return received data */
    return (u8)SPI_I2S_ReceiveData(SPIx);
}

/*
  * @brief  Set hardware SPI communication speed
  * @param  SPIx: SPI peripheral (SPI1, SPI2, SPI3)
  * @param  SpeedSet: 0 - low speed
  *         1 - high speed
  * @retval None
  */
void SPI_SetSpeed(SPI_TypeDef *SPIx, u8 SpeedSet)
{
    /* Disable SPI before changing speed */
    SPI_Cmd(SPIx, DISABLE);

    /* Set SPI baud rate prescaler */
    SPIx->CR1 = (uint16_t)((SPIx->CR1 & (uint16_t)~SPI_CR1_BR) |
                ((SpeedSet == 1U) ? SPI_BaudRatePrescaler_2 : SPI_BaudRatePrescaler_32));

    /* Enable SPI peripheral */
    SPI_Cmd(SPIx, ENABLE);
}

/*
  * @brief  Initialize the STM32 hardware SPI2
  * @retval None
  */
void SPI2_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;

    /* Configure SPI2 GPIO pins using HAL GPIO */
    hal_gpio_init(GPIOB, GPIO_Pin_13,
                  HAL_GPIO_AF_PP);      /* SCK */

    hal_gpio_init(GPIOB, GPIO_Pin_15,
                  HAL_GPIO_AF_PP);      /* MOSI */

    hal_gpio_init(GPIOB, GPIO_Pin_14,
                  HAL_GPIO_IN_PU);      /* MISO */

    /* Enable SPI2 peripheral clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* Configure SPI2 parameters */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    /* Initialize SPI2 using SPL */
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Enable SPI2 peripheral */
    SPI_Cmd(SPI2, ENABLE);
}