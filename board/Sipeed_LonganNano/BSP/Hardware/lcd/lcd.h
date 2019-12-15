#ifndef __LCD_H
#define __LCD_H		

#include "stdlib.h"	
#include "gd32vf103_gpio.h"

typedef enum {
    LCD_DISPMODE_HORIZONTAL,
    LCD_DISPMODE_HORIZONTAL_MIRROR,
    LCD_DISPMODE_VERTICAL,
    LCD_DISPMODE_VERTICAL_MIRROR
} lcd_display_mode_t;

#define USE_HARDWARE_SPI        1
//#define USE_HARDWARE_DMA_SPI    1
//#define USE_SOFTWARE_SPI        1


#if USE_HARDWARE_SPI
#define LCD_SCLK_Clr() 
#define LCD_SCLK_Set() 
#define LCD_SDIN_Clr()
#define LCD_SDIN_Set()
#define LCD_CS_Clr()    gpio_bit_reset( GPIOB, GPIO_PIN_2)     //CS PB2
#define LCD_CS_Set()    gpio_bit_set(   GPIOB, GPIO_PIN_2)
#endif


#if USE_HARDWARE_DMA_SPI 
#define LCD_SCLK_Clr() 
#define LCD_SCLK_Set() 
#define LCD_SDIN_Clr()
#define LCD_SDIN_Set()
#define LCD_CS_Clr()
#define LCD_CS_Set()
#endif


#if USE_SOFTWARE_SPI
#define LCD_SCLK_Clr()  gpio_bit_reset( GPIOA,  GPIO_PIN_5)    //CLK PA5
#define LCD_SCLK_Set()  gpio_bit_set(   GPIOA,  GPIO_PIN_5)
#define LCD_SDIN_Clr()  gpio_bit_reset( GPIOA,  GPIO_PIN_7)    //DIN PA7
#define LCD_SDIN_Set()  gpio_bit_set(   GPIOA,  GPIO_PIN_7)
#define LCD_CS_Clr()    gpio_bit_reset( GPIOB,  GPIO_PIN_2)     //CS PB2
#define LCD_CS_Set()    gpio_bit_set(   GPIOB,  GPIO_PIN_2)
#endif

#define LCD_RST_Clr()   gpio_bit_reset( GPIOB,  GPIO_PIN_1)     //RES PB1
#define LCD_RST_Set()   gpio_bit_set(   GPIOB,  GPIO_PIN_1)
#define LCD_DC_Clr()    gpio_bit_reset( GPIOB,  GPIO_PIN_0)      //DC PB0
#define LCD_DC_Set()    gpio_bit_set(   GPIOB,  GPIO_PIN_0)


void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);
void LCD_WR_REG(uint8_t dat);

void LCD_Init(void);
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2,uint16_t y2);
void LCD_SetDisplayMode(lcd_display_mode_t m);

void LCD_Clear(uint16_t Color);
void LCD_ShowChinese(uint16_t x,uint16_t y,uint8_t index,uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
void LCD_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void LCD_DrawCircle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t ch,uint16_t color);
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t color);
void LCD_ShowNum(uint16_t x,uint16_t y,uint16_t num,uint16_t color);

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430
#define DARKBLUE      	 0x01CF
#define LIGHTBLUE      	 0x7D7C
#define GRAYBLUE       	 0x5458
#define LIGHTGREEN     	 0x841F
#define LGRAY 			 0xC618
#define LGRAYBLUE        0xA651
#define LBBLUE           0x2B12

#endif  
