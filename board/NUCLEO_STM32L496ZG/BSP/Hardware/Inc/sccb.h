#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"
#include "gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序参考自网友guanfu_wang代码。
//ALIENTEK STM32F103开发板 
//SCCB 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/16
//版本：V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////

//IO操作函数	 
#define SCCB_SDA_IN()  {GPIOB->MODER&=~(3<<(5*2));GPIOB->MODER|=0<<5*2;}	//PD7 输入
#define SCCB_SDA_OUT() {GPIOB->MODER&=~(3<<(5*2));GPIOB->MODER|=1<<5*2;} 	//PD7 输出
#define SCCB_ID   			0X60  			//OV2640的ID

//IO方向设置
#define SCCB_SDA_IN()  {GPIOB->MODER&=~(3<<(5*2));GPIOB->MODER|=0<<5*2;}	//PD7 输入
#define SCCB_SDA_OUT() {GPIOB->MODER&=~(3<<(5*2));GPIOB->MODER|=1<<5*2;} 	//PD7 输出

#define SCCB_ID   			0X60  			//OV2640的ID

void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
uint8_t SCCB_WR_Byte(uint8_t dat);
uint8_t SCCB_RD_Byte(void);
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);

void SCCB_SCL(uint8_t sccb_scl);
void SCCB_SDA(uint8_t sccb_sda);
uint8_t SCCB_READ_SDA(void);
#endif









