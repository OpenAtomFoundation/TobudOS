#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "debug.h"

#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#define LCD_W 240
#define LCD_H 240



//-----------------LCD端口定义---------------- 

#define LCD_SCLK_Clr() GPIO_WriteBit(GPIOE,GPIO_Pin_1,0)//SCL=SCLK
#define LCD_SCLK_Set() GPIO_WriteBit(GPIOE,GPIO_Pin_1,1)

#define LCD_MOSI_Clr() GPIO_WriteBit(GPIOD,GPIO_Pin_1,0)//SDA=MOSI
#define LCD_MOSI_Set() GPIO_WriteBit(GPIOD,GPIO_Pin_1,1)

#define LCD_RES_Clr()  GPIO_WriteBit(GPIOD,GPIO_Pin_3,0)//RES
#define LCD_RES_Set()  GPIO_WriteBit(GPIOD,GPIO_Pin_3,1)

#define LCD_DC_Clr()   GPIO_WriteBit(GPIOE,GPIO_Pin_0,0)//DC
#define LCD_DC_Set()   GPIO_WriteBit(GPIOE,GPIO_Pin_0,1)
 		     
//#define LCD_CS_Clr()   GPIO_WriteBit(GPIOA,GPIO_Pin_4)//CS
//#define LCD_CS_Set()   GPIO_WriteBit(GPIOA,GPIO_Pin_4)

#define LCD_BLK_Clr()  GPIO_WriteBit(GPIOD,GPIO_Pin_0,1)//BLK
#define LCD_BLK_Set()  GPIO_WriteBit(GPIOD,GPIO_Pin_0,0)




void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif




