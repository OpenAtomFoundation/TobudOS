#ifndef __LCD_H
#define __LCD_H
#include "stm32l4xx_hal.h"

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK Pandora STM32L475 IOT开发板
 *	LCD TFT驱动代码
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2018/10/27
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	初始版本
 *	******************************************************************************/

extern uint16_t POINT_COLOR; //默认画笔颜色
extern uint16_t BACK_COLOR;  //默认背景颜色

//LCD的宽和高定义
#define LCD_Width 240
#define LCD_Height 240

//画笔颜色
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40 //棕色
#define BRRED 0XFC07 //棕红色
#define GRAY 0X8430  //灰色
//GUI颜色

#define DARKBLUE 0X01CF  //深蓝色
#define LIGHTBLUE 0X7D7C //浅蓝色
#define GRAYBLUE 0X5458  //灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE 0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12	//浅棕蓝色(选择条目的反色)

/*
	LCD_PWR:	LCD_PWR_Pin, 	PB7
	LCD_RST:	LCD_RESET_Pin,  PB6
	LCD_DC:		LCD_WR_Pin, 	PB4
	LCD_CS:		LCD_CS_Pin, 	PD7
*/
// #define LCD_PWR(n) (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET))
// #define LCD_RST(n) (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET))
// #define LCD_DC(n)  (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET))
// #define LCD_CS(n)  (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET))

#define LCD_PWR(n)	HAL_GPIO_WritePin(LCD_PWR_GPIO_Port, 	LCD_PWR_Pin, 	(n?GPIO_PIN_SET:GPIO_PIN_RESET))
#define LCD_RST(n)	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, (n?GPIO_PIN_SET:GPIO_PIN_RESET))
#define LCD_DC(n) 	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, 	LCD_WR_Pin, 	(n?GPIO_PIN_SET:GPIO_PIN_RESET))
#define LCD_CS(n) 	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, 	LCD_CS_Pin, 	(n?GPIO_PIN_SET:GPIO_PIN_RESET))

void LCD_Init(void);																				 //初始化
void LCD_DisplayOn(void);																			 //开显示
void LCD_DisplayOff(void);																			 //关显示
void LCD_Write_HalfWord(const uint16_t da);															 //写半个字节数据到LCD
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);							 //设置数据显示区域
void LCD_Clear(uint16_t color);																		 //清屏
void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);   //填充单色
void LCD_Draw_Point(uint16_t x, uint16_t y);														 //画点
void LCD_Draw_ColorPoint(uint16_t x, uint16_t y, uint16_t color);									 //画带颜色点
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);								 //画线
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);							 //画矩形
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r);											 //画圆
void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint8_t size);									 //显示一个字符
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size);					 //显示一个数字
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);	//显示数字
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p); //显示字符串
void LCD_Show_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p);		 //显示图片
void Display_ALIENTEK_LOGO(uint16_t x, uint16_t y);													 //显示ALIENTEK LOGO

#endif
