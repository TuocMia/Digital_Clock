<<<<<<< Updated upstream
//////////////////////////////////////////////////////////////////////////////////	 
// QDtech-TFT LCD driver for STM32
// ST7789 240x240 SPI LCD Module
// Version: V1.0
// Copyright(C) 2018-2028 All rights reserved
/****************************************************************************************************
//========================================= Power Wiring =========================================//
//     LCD Module             STM32 MCU
//       VCC         ->          3.3V         // Power
//       GND         ->          GND          // Ground
//======================================= LCD Data Wiring ========================================//
// Default data bus type: 4-wire SPI
//     LCD Module             STM32 MCU    
//       SDA         ->          PB15         // LCD SPI MOSI (Data write)
//====================================== LCD Control Wiring ======================================//
//     LCD Module             STM32 MCU 
//       BLK         ->          PB9          // LCD Backlight control (or connect to 3.3V)
//       SCK         ->          PB13         // LCD SPI SCK (Clock)
//       DC          ->          PB10         // LCD Data/Command control (RS)
//       RES         ->          PB12         // LCD Reset signal
**************************************************************************************************/	
 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	
#include "st7789.h"
#include "stdlib.h"
#include "hal_spi.h"

// LCD parameter management
_lcd_dev lcddev;

// Pen color, background color
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u8 data)
{ 
//   LCD_CS_CLR;     
	 LCD_RS_CLR;	  
   SPI_WriteByte(SPI2,data);
//   LCD_CS_SET;	
=======
#include "st7789.h"

/* LCD device parameters */
_lcd_dev lcddev;

/* Drawing colors */
u16 POINT_COLOR = BLACK;
u16 BACK_COLOR  = WHITE;

/*****************************************************************************
 * @name       :void LCD_Delay(volatile u32 count)
 * @function   :Simple delay function for LCD initialization
 * @parameters :count: Delay loop count
 * @retvalue   :None
 ******************************************************************************/
static void LCD_Delay(volatile u32 count)
{
    while (count--)
    {
        __NOP();
    }
}

/*****************************************************************************
 * @name       :void LCD_GPIOInit(void)
 * @function   :Initialize LCD control GPIO pins using HAL GPIO
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
static void LCD_GPIOInit(void)
{
    /* Configure LCD control pins as push-pull outputs */
    hal_gpio_init(LCD_GPIO_PORT,
                  LCD_LED_PIN |
                  LCD_RS_PIN |
                  LCD_RST_PIN,
                  HAL_GPIO_OUT_PP);

    /* Set initial pin states */
    hal_gpio_set(LCD_GPIO_PORT,
                 LCD_LED_PIN |
                 LCD_RS_PIN |
                 LCD_RST_PIN);
}

/*****************************************************************************
 * @name       :void LCD_Reset(void)
 * @function   :Reset ST7789 LCD
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
static void LCD_Reset(void)
{
    /* Reset LCD */
    hal_gpio_clear(LCD_GPIO_PORT, LCD_RST_PIN);
    LCD_Delay(100000);

    /* Release reset */
    hal_gpio_set(LCD_GPIO_PORT, LCD_RST_PIN);
    LCD_Delay(100000);
}

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @function   :Write one command to LCD
 * @parameters :data: Command byte
 * @retvalue   :None
 ******************************************************************************/
void LCD_WR_REG(u8 data)
{
    /* Select command mode */
    hal_gpio_clear(LCD_GPIO_PORT, LCD_RS_PIN);

    /* Send command through HAL SPI */
    SPI_WriteByte(SPI2, data);
>>>>>>> Stashed changes
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
<<<<<<< Updated upstream
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u8 data)
{
//   LCD_CS_CLR;
	 LCD_RS_SET;
   SPI_WriteByte(SPI2,data);
//   LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void Lcd_WriteData_16Bit(u16 Data)
{	
//   LCD_CS_CLR;
   LCD_RS_SET;  
   SPI_WriteByte(SPI2,Data>>8);
	 SPI_WriteByte(SPI2,Data);
//   LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y); // Set cursor position 
	Lcd_WriteData_16Bit(POINT_COLOR); 
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(u16 Color)
{
  unsigned int i,m;  
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
//	LCD_CS_CLR;
	LCD_RS_SET;
	for(i=0;i<lcddev.height;i++)
	{
    for(m=0;m<lcddev.width;m++)
    {	
			SPI_WriteByte(SPI2,Color>>8);
			SPI_WriteByte(SPI2,Color);
		}
	}
//	 LCD_CS_SET;
} 

/*****************************************************************************
 * @name       :void LCD_GPIOInit(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen GPIO
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_GPIOInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	      
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);	// Enable GPIOB clock
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_12;//| GPIO_Pin_11; //GPIOB9,10,11,12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // Output Push-Pull
	GPIO_Init(GPIOB, &GPIO_InitStructure);             // Initialize GPIO
	GPIO_SetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_12);	
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
	LCD_RST_CLR;
	LCD_RST_SET;
}

/*****************************************************************************
 * @name       :void LCD_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{  
	SPI2_Init();     // Initialize SPI2
	LCD_GPIOInit();  // Initialize LCD GPIO
 	LCD_RESET();     // Reset LCD
//************* ST7789 initialization **********//	
	LCD_WR_REG(0x36); 
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0x3A); 
	LCD_WR_DATA(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x33);

	LCD_WR_REG(0xB7); 
	LCD_WR_DATA(0x35);  

	LCD_WR_REG(0xBB);
	LCD_WR_DATA(0x19);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA(0x12);   

	LCD_WR_REG(0xC4);
	LCD_WR_DATA(0x20);  

	LCD_WR_REG(0xC6); 
	LCD_WR_DATA(0x0F);    

	LCD_WR_REG(0xD0); 
	LCD_WR_DATA(0xA4);
	LCD_WR_DATA(0xA1);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0xD0);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x2B);
	LCD_WR_DATA(0x3F);
	LCD_WR_DATA(0x54);
	LCD_WR_DATA(0x4C);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x0B);
	LCD_WR_DATA(0x1F);
	LCD_WR_DATA(0x23);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA(0xD0);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x3F);
	LCD_WR_DATA(0x44);
	LCD_WR_DATA(0x51);
	LCD_WR_DATA(0x2F);
	LCD_WR_DATA(0x1F);
	LCD_WR_DATA(0x1F);
	LCD_WR_DATA(0x20);
	LCD_WR_DATA(0x23);

	LCD_WR_REG(0x21); 

	LCD_WR_REG(0x11); 

	LCD_WR_REG(0x29); 	
	LCD_direction(USE_HORIZONTAL); // Set LCD display direction
	LCD_LED=1;                     // Turn on backlight	 
	LCD_Clear(WHITE);              // Clear screen white
}
 
/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the beginning x coordinate of the LCD display window
 *              yStar:the beginning y coordinate of the LCD display window
 *              xEnd:the ending x coordinate of the LCD display window
 *              yEnd:the ending y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA((xStar+lcddev.xoffset)>>8);
	LCD_WR_DATA(xStar+lcddev.xoffset);		
	LCD_WR_DATA((xEnd+lcddev.xoffset)>>8);
	LCD_WR_DATA(xEnd+lcddev.xoffset);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA((yStar+lcddev.yoffset)>>8);
	LCD_WR_DATA(yStar+lcddev.yoffset);		
	LCD_WR_DATA((yEnd+lcddev.yoffset)>>8);
	LCD_WR_DATA(yEnd+lcddev.yoffset);

	LCD_WriteRAM_Prepare();	// Start writing to GRAM			
}   

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the x coordinate of the pixel
 *              Ypos:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
 *                        1-90 degree
 *                        2-180 degree
 *                        3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(u8 direction)
{ 
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	lcddev.wramcmd=0x2C;
	switch(direction){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			lcddev.xoffset=0;
			lcddev.yoffset=0;
			LCD_WriteReg(0x36,0);//BGR==1,MY==0,MX==0,MV==0
			break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			lcddev.xoffset=0;
			lcddev.yoffset=0;
			LCD_WriteReg(0x36,(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
			break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;
			lcddev.xoffset=0;
			lcddev.yoffset=80;			
			LCD_WriteReg(0x36,(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
			break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			lcddev.xoffset=80;
			lcddev.yoffset=0;
			LCD_WriteReg(0x36,(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
			break;	
		default:break;
	}		
}
=======
 * @function   :Write one data byte to LCD
 * @parameters :data: Data byte
 * @retvalue   :None
 ******************************************************************************/
void LCD_WR_DATA(u8 data)
{
    /* Select data mode */
    hal_gpio_set(LCD_GPIO_PORT, LCD_RS_PIN);

    /* Send data through HAL SPI */
    SPI_WriteByte(SPI2, data);
}

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @function   :Write 16-bit data to LCD
 * @parameters :Data: 16-bit data
 * @retvalue   :None
 ******************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{
    /* Select data mode */
    hal_gpio_set(LCD_GPIO_PORT, LCD_RS_PIN);

    /* Send high byte */
    SPI_WriteByte(SPI2, Data >> 8);

    /* Send low byte */
    SPI_WriteByte(SPI2, Data);
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @function   :Prepare LCD for RAM write operation
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(0x2C);
}

/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,
 *                                  u16 xEnd, u16 yEnd)
 * @function   :Set LCD drawing window
 * @parameters :xStar: Start X coordinate
 *              yStar: Start Y coordinate
 *              xEnd : End X coordinate
 *              yEnd : End Y coordinate
 * @retvalue   :None
 ******************************************************************************/
void LCD_SetWindows(u16 xStar, u16 yStar,
                    u16 xEnd, u16 yEnd)
{
    /* Set column address */
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(xStar & 0xFF);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(xEnd & 0xFF);

    /* Set row address */
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(yStar & 0xFF);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(yEnd & 0xFF);

    /* Start memory write */
    LCD_WriteRAM_Prepare();
}

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @function   :Set LCD drawing cursor position
 * @parameters :Xpos: X coordinate
 *              Ypos: Y coordinate
 * @retvalue   :None
 ******************************************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    LCD_SetWindows(Xpos, Ypos, Xpos, Ypos);
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x, u16 y)
 * @function   :Draw one pixel using POINT_COLOR
 * @parameters :x: X coordinate
 *              y: Y coordinate
 * @retvalue   :None
 ******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);
    Lcd_WriteData_16Bit(POINT_COLOR);
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @function   :Fill the LCD screen with a specified color
 * @parameters :Color: Fill color
 * @retvalue   :None
 ******************************************************************************/
void LCD_Clear(u16 Color)
{
    u32 i;
    u32 pixel_count;

    /* Set drawing window to full screen */
    LCD_SetWindows(0, 0,
                   lcddev.width - 1,
                   lcddev.height - 1);

    /* Select data mode */
    hal_gpio_set(LCD_GPIO_PORT, LCD_RS_PIN);

    /* Calculate total number of pixels */
    pixel_count = (u32)lcddev.width * lcddev.height;

    /* Fill screen */
    for (i = 0; i < pixel_count; i++)
    {
        SPI_WriteByte(SPI2, Color >> 8);
        SPI_WriteByte(SPI2, Color & 0xFF);
    }
}

/*****************************************************************************
 * @name       :void LCD_DisplayOn(void)
 * @function   :Turn on LCD display
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0x29);
}

/*****************************************************************************
 * @name       :void LCD_DisplayOff(void)
 * @function   :Turn off LCD display
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0x28);
}

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @function   :Set LCD display direction
 * @parameters :direction: Display direction
 * @retvalue   :None
 ******************************************************************************/
void LCD_direction(u8 direction)
{
    lcddev.dir = direction;

    if (direction == 0)
    {
        lcddev.width = LCD_W;
        lcddev.height = LCD_H;

        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x00);
    }
    else if (direction == 1)
    {
        lcddev.width = LCD_H;
        lcddev.height = LCD_W;

        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x60);
    }
}

/*****************************************************************************
 * @name       :void LCD_Init(void)
 * @function   :Initialize ST7789 LCD
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void LCD_Init(void)
{
    /* Initialize LCD control GPIO */
    LCD_GPIOInit();

    /* Initialize SPI2 through HAL */
    SPI2_Init();

    /* Reset LCD */
    LCD_Reset();

    /* Software reset */
    LCD_WR_REG(0x01);
    LCD_Delay(200000);

    /* Sleep out */
    LCD_WR_REG(0x11);
    LCD_Delay(200000);

    /* Color mode: RGB565 */
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);

    /* Memory data access control */
    LCD_WR_REG(0x36);

#if USE_HORIZONTAL == 0
    LCD_WR_DATA(0x00);
#elif USE_HORIZONTAL == 1
    LCD_WR_DATA(0x60);
#elif USE_HORIZONTAL == 2
    LCD_WR_DATA(0xC0);
#elif USE_HORIZONTAL == 3
    LCD_WR_DATA(0xA0);
#endif

    /* Display inversion on */
    LCD_WR_REG(0x21);

    /* Normal display mode */
    LCD_WR_REG(0x13);

    /* Display on */
    LCD_WR_REG(0x29);

    /* Configure LCD parameters */
    lcddev.width = LCD_W;
    lcddev.height = LCD_H;
    lcddev.dir = USE_HORIZONTAL;
    lcddev.wramcmd = 0x2C;
    lcddev.setxcmd = 0x2A;
    lcddev.setycmd = 0x2B;

    /* Turn on LCD backlight */
    hal_gpio_set(LCD_GPIO_PORT, LCD_LED_PIN);

    /* Clear screen */
    LCD_Clear(BLACK);
}
>>>>>>> Stashed changes
