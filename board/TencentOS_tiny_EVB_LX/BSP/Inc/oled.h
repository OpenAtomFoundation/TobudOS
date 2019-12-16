#ifndef __OLED_H
#define __OLED_H
#include "gd32vf103.h"
#include "stdlib.h"
#define OLED_MODE 0
#define OLED_SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF
#define X_WIDTH 	128
#define Y_WIDTH 	64



#define OLED_SCLK_Clr() gpio_bit_write(GPIOB,GPIO_PIN_10, RESET);//SCL
#define OLED_SCLK_Set() gpio_bit_write(GPIOB,GPIO_PIN_10, SET);

#define OLED_SDIN_Clr() gpio_bit_write(GPIOB,GPIO_PIN_11, RESET);//SDA
#define OLED_SDIN_Set() gpio_bit_write(GPIOB,GPIO_PIN_11, SET);

#define OLED_CMD  0
#define OLED_DATA 1



void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p,uint8_t Char_Size);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);



void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(unsigned char x, unsigned char y);
#endif
