#ifndef __E53_SF1_H__
#define __E53_SF1_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim16;

/* 控制设备IO口定义 ------------------------------------------------------------*/

#define SF1_Beep_Pin GPIO_PIN_8
#define SF1_Beep_GPIO_Port GPIOB
#define SF1_Beep_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SF1_LED_Pin GPIO_PIN_0
#define SF1_LED_GPIO_Port GPIOA
#define SF1_LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()



/* E53_SF1传感器数据类型定义 ------------------------------------------------------------*/
typedef struct
{
		int Smoke_Value;
} E53_SF1_Data_TypeDef;

extern E53_SF1_Data_TypeDef E53_SF1_Data;

/***************************************************************
* 名		称: GasStatus_ENUM
* 说    明：枚举状态结构体
***************************************************************/
typedef enum
{
	OFF = 0,
	ON
} E53SF1_Status_ENUM;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void Init_E53_SF1(void);
void E53_SF1_Read_Data(void);
void E53SF1_LED_StatusSet(E53SF1_Status_ENUM status);
void E53SF1_Beep(E53SF1_Status_ENUM status);

#endif





