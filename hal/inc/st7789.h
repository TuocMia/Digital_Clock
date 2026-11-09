#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h"

// LCD parameter structure
typedef struct  
{										    
	u16 width;			// LCD width
	u16 height;			// LCD height
	u16 id;				// LCD ID
	u8  dir;			// Display orientation: 0: portrait, 1: landscape	
	u16	wramcmd;		// Start write GRAM command
	u16  setxcmd;		// Set X coordinate command
	u16  setycmd;		// Set Y coordinate command	
	u8   xoffset;    
	u8	 yoffset;
}_lcd_dev; 	

// LCD parameters
extern _lcd_dev lcddev;

///////////////////////////////////// User Configuration ///////////////////////////////////	 
#define USE_HORIZONTAL  	 0 // LCD rotation: 0: 0 deg, 1: 90 deg, 2: 180 deg, 3: 270 deg

//////////////////////////////////////////////////////////////////////////////////	  
// LCD dimensions
#define LCD_W 240
#define LCD_H 240

// Drawing colors		   
extern u16  POINT_COLOR; // Default pen color    
extern u16  BACK_COLOR;  // Default background color

////////////////////////////////////////////////////////////////////
//----------------- LCD Port Definitions ---------------- 
#define GPIO_TYPE  GPIOB  // GPIO port type
#define LED      9        // Backlight control pin PB9
//#define LCD_CS   11       // Chip Select pin PB11
#define LCD_RS   10       // Register/data select pin PB10 
#define LCD_RST  12       // Reset pin PB12

// Backlight control (transistor or PWM)
#define	LCD_LED PBout(LED) // LCD backlight control PB9

// GPIO Set (High)
//#define	LCD_CS_SET  GPIO_TYPE->BSRR=1<<LCD_CS    // CS pin PB11
#define	LCD_RS_SET	GPIO_TYPE->BSRR=1<<LCD_RS    // RS pin PB10	  
#define	LCD_RST_SET	GPIO_TYPE->BSRR=1<<LCD_RST   // Reset pin PB12

// GPIO Reset (Low)							    
//#define	LCD_CS_CLR  GPIO_TYPE->BRR=1<<LCD_CS     // CS pin PB11
#define	LCD_RS_CLR	GPIO_TYPE->BRR=1<<LCD_RS     // RS pin PB10	 
#define	LCD_RST_CLR	GPIO_TYPE->BRR=1<<LCD_RST    // Reset pin PB12

// Colors
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 // Brown
#define BRRED 			0XFC07 // Brown-red
#define GRAY  			0X8430 // Gray
#define GRAY0       0xEF7D 
#define GRAY1       0x8410 // Gray1
#define GRAY2       0x4208 

// GUI Colors
#define DARKBLUE      	 0X01CF	// Dark blue
#define LIGHTBLUE      	 0X7D7C	// Light blue  
#define GRAYBLUE       	 0X5458 // Gray blue

// Panel colors 
#define LIGHTGREEN     	0X841F // Light green
#define LIGHTGRAY     0XEF5B // Light gray (Panel)
#define LGRAY 			 		0XC618 // Light gray (Panel), window background

#define LGRAYBLUE      	0XA651 // Light gray blue (middle layer)
#define LBBLUE          0X2B12 // Light brown blue (inverted selection)
	    															  
void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(u16 Color);	 
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);
u16  LCD_ReadPoint(u16 x,u16 y); 
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);

u16 LCD_RD_DATA(void);								    
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_DATA(u8 data);
u16 LCD_ReadReg(u8 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
u16 LCD_ReadRAM(void);		   
u16 LCD_BGR2RGB(u16 c);
void LCD_SetParam(void);
void Lcd_WriteData_16Bit(u16 Data);
void LCD_direction(u8 direction );
				  		 
#endif
