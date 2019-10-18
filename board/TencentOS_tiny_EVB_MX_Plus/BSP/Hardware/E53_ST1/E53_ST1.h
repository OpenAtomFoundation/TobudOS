#ifndef __E53_ST1_H__
#define __E53_ST1_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* 控制设备IO口定义 ------------------------------------------------------------*/

#define ST1_Beep_Pin GPIO_PIN_8
#define ST1_Beep_GPIO_Port GPIOB
#define ST1_Beep_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ST1_LED_Pin GPIO_PIN_0
#define ST1_LED_GPIO_Port GPIOA
#define ST1_LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define ST1_GPS_POW_Pin GPIO_PIN_9
#define ST1_GPS_POW_GPIO_Port GPIOC
#define ST1_GPS_POW_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()

/***************************************************************
* 名		称: GasStatus_ENUM
* 说    明：枚举状态结构体
***************************************************************/
typedef enum
{
	ST1_OFF = 0,
	ST1_ON
} E53ST1_Status_ENUM;

/***************************************************\
*GPS NMEA-0183协议重要参数结构体定义
*卫星信息
\***************************************************/
__packed typedef struct
{
	uint32_t latitude_bd;					//纬度   分扩大100000倍，实际要除以100000
	uint8_t nshemi_bd;						//北纬/南纬,N:北纬;S:南纬	
	uint32_t longitude_bd;			  //经度 分扩大100000倍,实际要除以100000
	uint8_t ewhemi_bd;						//东经/西经,E:东经;W:西经
}gps_msg;

/* E53_ST1传感器数据类型定义 ------------------------------------------------------------*/
typedef struct
{
		float    Longitude;				//经度
		float    Latitude;        //纬度
} E53_ST1_Data_TypeDef;

extern E53_ST1_Data_TypeDef E53_ST1_Data;

typedef struct
{
		char    Longitude[9];				//经度
		char    Latitude[8];        //纬度
} E53_ST1_Send_TypeDef;

extern E53_ST1_Send_TypeDef E53_ST1_Send;

extern TIM_HandleTypeDef htim16;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void Init_E53_ST1(void);
void E53_ST1_Read_Data(void);
void E53_ST1_LED_StatusSet(E53ST1_Status_ENUM status);
void E53_ST1_Beep(E53ST1_Status_ENUM status);

#endif





