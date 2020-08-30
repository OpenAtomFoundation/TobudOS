/********************************************************
 *
 * @file    ds18b20.c
 * @brief   ds18b20驱动
 * @author  Mculover666
 * @date    2020/08/29
 * @note    此驱动中us级延时使用DWT
 *
********************************************************/

#include "ds18b20.h"
#include <stdio.h>

/**
  * @brief    发送复位信号
  * @param    none
  * @retval   none
*/
static void DS18B20_Send_Reset_Single(void)
{
	DS18B20_OutPut_Mode();
    
    /* 拉低总线 480 - 960 us*/
	DS18B20_Out(0);
	DS18B20_Delay_us(750);
    
    /* 释放总线 15 - 60 us */
	DS18B20_Out(1);
	DS18B20_Delay_us(15);
}

/**
  * @brief    检测DS18B20存在脉冲
  * @param    none
  * @retval   0 DS18B20设备正常
  * @retval   1  DS18B20设备响应复位信号失败
  * @retval   2  DS18B20设备释放总线失败
*/
static uint8_t DS18B20_Check_Ready_Single(void)
{
	uint8_t cnt = 0;
    
	/* 1.检测存在脉冲 */
	DS18B20_InPut_Mode();
    
    //等待DS18B20 拉低总线 （60~240 us 响应复位信号）
	while (DS18B20_In() && cnt < 240) {
		DS18B20_Delay_us(1);
		cnt++;
	}
    
	if (cnt > 240) {
        return 1;
    }
    
	/* 2.检测DS18B20是否释放总线 */	
	cnt = 0;
	DS18B20_InPut_Mode();
    
    //判断DS18B20是否释放总线（60~240 us 响应复位信号之后会释放总线）
	while ((!DS18B20_In()) && cnt<240) {
		DS18B20_Delay_us(1);
		cnt++;
	}
    
	if (cnt > 240) {
        return 2;
    } else {
        return 0;
    }
}

/**
  * @brief    检测DS18B20设备是否正常
  * @param    none
  * @retval   0 DS18B20设备正常
  * @retval   1  DS18B20设备响应复位信号失败
  * @retval   2  DS18B20设备释放总线失败
*/
static uint8_t DS18B20_Check_Device(void)
{
    /*1.主机发送复位信号*/
	DS18B20_Send_Reset_Single();
    
    /*2.检测存在脉冲*/
	return DS18B20_Check_Ready_Single();
}
/**
  * @brief    DS18B20初始化
  * @param    none
  * @retval   none
*/
void DS18B20_Init(void)
{
	/* 1.DS18B20控制引脚初始化 */
    //在main函数中已经初始化，不需要再次重复。

	/*2.检测DS18B20设备是否正常*/
	switch (DS18B20_Check_Device()) {
		case 0:	
            printf("DS18B20_Init OK!\n");
            break;
		case 1:
            printf("DS18B20设备响应复位信号失败！\n");
            break;
		case 2:
            printf("DS18B20设备释放总线失败！\n");
            break;
	}
}

/**
  * @brief    向DS18B20写一个字节
  * @param    cmd 要写入的字节
  * @retval   none
*/
static uint8_t DS18B20_Write_Byte(uint8_t cmd)
{
	uint8_t i = 0;
    
    /* 1. 设置总线为输出模式 */
	DS18B20_OutPut_Mode();
    
    /* 2. 发送数据，低位在前 */
	for (i = 0; i < 8; i++) {
		DS18B20_Out(0);
		DS18B20_Delay_us(2);  
		DS18B20_Out(cmd & 0x01);
		DS18B20_Delay_us(60);
		DS18B20_Out(1);
		cmd >>= 1;
		DS18B20_Delay_us(2);
	}
    
	return 0;
}
 
/**
  * @brief    从DS18B20读一个字节
  * @param    none
  * @retval   读取到的一个字节数据
*/
uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i = 0;
    uint8_t data = 0;
    
    /* 读取数据 */
	for (i  =0; i < 8; i++)	{
		DS18B20_OutPut_Mode();
		DS18B20_Out(0);  
		DS18B20_Delay_us(2);
		DS18B20_Out(1);
        
		DS18B20_InPut_Mode();
		DS18B20_Delay_us(10);
		data >>= 1 ;
		if (DS18B20_In()) {
            data |= 0x80;
        }
        
		DS18B20_Delay_us(60);
		DS18B20_Out(1);
	}
    
	return data;
}

/**
  * @brief    从DS18B20读取一次数据
  * @param    none
  * @retval   读取到的温度数据
  * @note     适用于总线上只有一个DS18B20的情况
*/
uint16_t DS18B20_Read_Temperature(void)
{
    uint16_t temp = 0;
    uint8_t  temp_H, temp_L;

    DS18B20_Check_Device();
    
    DS18B20_Write_Byte(0xCC); //跳过ROM序列检测
    DS18B20_Write_Byte(0x44); //启动一次温度转换
    
    //等待温度转换完成
    while (DS18B20_Read_Byte() != 0xFF);
    
    DS18B20_Check_Device();   //必须，不能省略
    
    DS18B20_Write_Byte(0xCC); //跳过ROM序列检测
    DS18B20_Write_Byte(0xBE); //读取温度
    
    temp_L = DS18B20_Read_Byte(); //读取的温度低位数据
    temp_H = DS18B20_Read_Byte(); //读取的温度高位数据
    temp   = temp_L | (temp_H << 8);   //合成温度

    return temp;
    
}
