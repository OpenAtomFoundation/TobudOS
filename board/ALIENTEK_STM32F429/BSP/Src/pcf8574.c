#include "pcf8574.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//PCF8574驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化PCF8574
uint8_t PCF8574_Init(void)
{
    uint8_t temp=0;
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();           //使能GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_12;           //PB12
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //初始化
    IIC_Init();					            //IIC初始化 	
	//检查PCF8574是否在位
    IIC_Start();    	 	   
	IIC_Send_Byte(PCF8574_ADDR);            //写地址			   
	temp=IIC_Wait_Ack();		            //等待应答,通过判断是否有ACK应答,来判断PCF8574的状态
    IIC_Stop();					            //产生一个停止条件
    PCF8574_WriteOneByte(0XFF);	            //默认情况下所有IO输出高电平
	return temp;
}

//读取PCF8574的8位IO值
//返回值:读到的数据
uint8_t PCF8574_ReadOneByte(void)
{				  
	uint8_t temp=0;		  	    																 
    IIC_Start();    	 	   
	IIC_Send_Byte(PCF8574_ADDR|0X01);   //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();							//产生一个停止条件	    
	return temp;
}
//向PCF8574写入8位IO值  
//DataToWrite:要写入的数据
void PCF8574_WriteOneByte(uint8_t DataToWrite)
{				   	  	    																 
    IIC_Start();  
    IIC_Send_Byte(PCF8574_ADDR|0X00);   //发送器件地址0X40,写数据 	 
	IIC_Wait_Ack();	    										  		   
	IIC_Send_Byte(DataToWrite);    	 	//发送字节							   
	IIC_Wait_Ack();      
    IIC_Stop();							//产生一个停止条件 
	HAL_Delay(10);	 
}

//设置PCF8574某个IO的高低电平
//bit:要设置的IO编号,0~7
//sta:IO的状态;0或1
void PCF8574_WriteBit(uint8_t bit,uint8_t sta)
{
    uint8_t data;
    data=PCF8574_ReadOneByte(); //先读出原来的设置
    if(sta==0)data&=~(1<<bit);     
    else data|=1<<bit;
    PCF8574_WriteOneByte(data); //写入新的数据
}

//读取PCF8574的某个IO的值
//bit：要读取的IO编号,0~7
//返回值:此IO的值,0或1
uint8_t PCF8574_ReadBit(uint8_t bit)
{
    uint8_t data;
    data=PCF8574_ReadOneByte(); //先读取这个8位IO的值 
    if(data&(1<<bit))return 1;
    else return 0;   
}  
    
