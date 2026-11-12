#include "hal_spi.h"

/*****************************************************************************
<<<<<<< Updated upstream
 * @name       :u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
 * @date       :2018-08-09 
 * @function   :Write a byte of data using STM32's hardware SPI
 * @parameters :SPIx: SPI type,x for 1,2,3
                Byte:Data to be written
 * @retvalue   :Data received by the bus
******************************************************************************/
u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
{
	while((SPIx->SR&SPI_I2S_FLAG_TXE)==RESET);		// Wait for TX buffer empty	  
	SPIx->DR=Byte;	 								// Send byte   
	while((SPIx->SR&SPI_I2S_FLAG_RXNE)==RESET);		// Wait for RX buffer not empty  
	return SPIx->DR;          	     				// Return received data			
} 

/*****************************************************************************
 * @name       :void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
 * @date       :2018-08-09 
 * @function   :Set hardware SPI Speed
 * @parameters :SPIx: SPI type,x for 1,2,3
                SpeedSet:0-high speed
                         1-low speed
 * @retvalue   :None
******************************************************************************/
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
{
	SPIx->CR1&=0XFFC7;
	if(SpeedSet==1)// High speed
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_2;// Fsck=Fpclk/2	
	}
	else// Low speed
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_32; // Fsck=Fpclk/32
	}
	SPIx->CR1|=1<<6; // Enable SPI peripheral
} 

/*****************************************************************************
 * @name       :void SPI2_Init(void)	
 * @date       :2018-08-09 
 * @function   :Initialize the STM32 hardware SPI2
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void SPI2_Init(void)	
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	 
	// Configure SPI2 GPIO pins
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	
	// Configure SPI2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 ,ENABLE);
	   
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	// Enable SPI2
	SPI_Cmd(SPI2, ENABLE);   
}
=======
 * @name       :u8 SPI_WriteByte(SPI_TypeDef* SPIx, u8 Byte)
 * @date       :2018-08-09
 * @function   :Write and receive one byte using STM32 hardware SPI
 * @parameters :SPIx: SPI peripheral (SPI1, SPI2, SPI3)
 *              Byte: Data to be transmitted
 * @retvalue   :Data received from the SPI bus
 ******************************************************************************/
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

/*****************************************************************************
 * @name       :void SPI_SetSpeed(SPI_TypeDef* SPIx, u8 SpeedSet)
 * @date       :2018-08-09
 * @function   :Set hardware SPI communication speed
 * @parameters :SPIx: SPI peripheral (SPI1, SPI2, SPI3)
 *              SpeedSet: 0 - low speed
 *                        1 - high speed
 * @retvalue   :None
 ******************************************************************************/
void SPI_SetSpeed(SPI_TypeDef *SPIx, u8 SpeedSet)
{
    /* Disable SPI before changing speed */
    SPI_Cmd(SPIx, DISABLE);

    /* Set SPI baud rate prescaler */
    if (SpeedSet == 1) {
        SPI_BaudRatePrescalerConfig(SPIx, SPI_BaudRatePrescaler_2);
    } else {
        SPI_BaudRatePrescalerConfig(SPIx, SPI_BaudRatePrescaler_32);
    }

    /* Enable SPI peripheral */
    SPI_Cmd(SPIx, ENABLE);
}

/*****************************************************************************
 * @name       :void SPI2_Init(void)
 * @date       :2018-08-09
 * @function   :Initialize the STM32 hardware SPI2
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
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
>>>>>>> Stashed changes
