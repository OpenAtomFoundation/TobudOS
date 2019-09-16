#ifndef __ONEWIRE_DHT11_H__
#define        __ONEWIRE_DHT11_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "mcu_init.h"
/* 类型定义 ------------------------------------------------------------------*/
/************************ DHT11 数据类型定义******************************/
typedef struct
{
    uint8_t  humi_high8bit;		//原始数据：湿度高8位
    uint8_t  humi_low8bit;	 	//原始数据：湿度低8位
    uint8_t  temp_high8bit;	 	//原始数据：温度高8位
    uint8_t  temp_low8bit;	 	//原始数据：温度高8位
    uint8_t  check_sum;	 	    //校验和
    float    humidity;        //实际湿度
    float    temperature;     //实际温度  
} DHT11_Data_TypeDef;


/* 宏定义 -------------------------------------------------------------------*/
/***********************   DHT11 连接引脚定义  **************************/
#define DHT11_Dout_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define DHT11_Dout_PORT                           GPIOA
#define DHT11_Dout_PIN                            GPIO_PIN_6

/***********************   DHT11 函数宏定义  ****************************/
#define DHT11_Dout_LOW()                          HAL_GPIO_WritePin(DHT11_Dout_PORT,DHT11_Dout_PIN,GPIO_PIN_RESET)
#define DHT11_Dout_HIGH()                         HAL_GPIO_WritePin(DHT11_Dout_PORT,DHT11_Dout_PIN,GPIO_PIN_SET)
#define DHT11_Data_IN()	                          HAL_GPIO_ReadPin(DHT11_Dout_PORT,DHT11_Dout_PIN)

/***************************************************************
* 函数名称: DHT11_Msg_TypeDef
* 说    明: DHT11字符串数据的结构体
* 参    数: 无
* 返 回 值: 无
***************************************************************/
typedef struct
{
	char temp[4];
	char hum[4];	
}DHT11_Msg_TypeDef;

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void DHT11_Init( void );
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef * DHT11_Data);

#endif /* __ONEWIRE_DHT11_H__ */

