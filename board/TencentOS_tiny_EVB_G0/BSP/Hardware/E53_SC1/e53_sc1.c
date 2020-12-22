/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/
#include "e53_sc1.h"
#include "i2c.h"
#include "stm32g0xx_hal_tim.h"

/***************************************************************
* 函数名称: Init_BH1750
* 说    明: 写命令初始化BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
static void Init_BH1750(void)
{
	uint8_t t_Data = 0x01;
	HAL_I2C_Master_Transmit(&hi2c2,BH1750_Addr,&t_Data,1,0xff);
}

/***************************************************************
* 函数名称: Start_BH1750
* 说    明: 启动BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
static void Start_BH1750(void)
{
	uint8_t t_Data = 0x10;
	HAL_I2C_Master_Transmit(&hi2c2,BH1750_Addr,&t_Data,1,0xff); 
}

/***************************************************************
* 函数名称: Convert_BH1750
* 说    明: 数值转换
* 参    数: 无
* 返 回 值: 光强值
***************************************************************/
static float Convert_BH1750(void)
{
	float result_lx;
	uint8_t BUF[2];
	int result;
	Start_BH1750();
	HAL_Delay(180);
	HAL_I2C_Master_Receive(&hi2c2, BH1750_Addr+1,BUF,2,0xff); 
	result=BUF[0];
	result=(result<<8)+BUF[1];  //Synthetic Digital Illumination Intensity Data
	result_lx=(float)(result/1.2);
	return result_lx;
}

/***************************************************************
* 函数名称: e53_scl_read_data
* 说    明: 读取e53接口数据
* 参    数: 无
* 返 回 值: 无
***************************************************************/
float e53_scl_read_data(void)
{
	return Convert_BH1750();
}

// 硬件定时器参考代码

//定时器句柄 
TIM_HandleTypeDef TIM3_Handler;      

/***************************************************************
* 函数名称: TIM3_Init
* 说    明: 启动定时器，定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us. Ft=定时器工作频率,单位:Mhz
***************************************************************/
static void TIM3_Init(uint16_t arr,uint16_t psc)		//arr：自动重装值	psc：时钟预分频数
{  
	TIM3_Handler.Instance=TIM3;                             //通用定时器3
	TIM3_Handler.Init.Prescaler=psc;                        //分频系数
	TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;       //向上计数器
	TIM3_Handler.Init.Period=arr;                           //自动装载值
	TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1; //时钟分频因子
	HAL_TIM_Base_Init(&TIM3_Handler);						//时基初始化
	HAL_TIM_Base_Start_IT(&TIM3_Handler); 					//使能定时器3和定时器3更新中断：TIM_IT_UPDATE
}

/***************************************************************
* 函数名称: HAL_TIM_Base_MspInit
* 说    明: 中断初始化
***************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断
	}
}

/***************************************************************
* 函数名称: TIM3_IRQHandler
* 说    明: 中断处理函数
***************************************************************/
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);
}

/***************************************************************
* 函数名称: HAL_TIM_PeriodElapsedCallback
* 说    明: 定时器溢出回调
***************************************************************/
static uint8_t light_pwm = 99;
static uint32_t pwm_count = 0;
extern int power_switch_cache;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&TIM3_Handler))
	{
		if (power_switch_cache) {
			if(pwm_count < 4 * light_pwm)
			{
				LED_CLOSE;
			} else {
				LED_OPEN;
			}
			if(pwm_count >= 4 * 100)
			{
				pwm_count = 0;
			}
			pwm_count++;
		}
	}
}

void e53_sc1_set_ligth_level(int light_level)
{
	switch(light_level) {
		case LIGHT_LEVEL_LOW:
			light_pwm = 99;
			break;
		case LIGHT_LEVEL_MID:
			light_pwm = 95;
			break;
		case LIGHT_LEVEL_HIGH:
			light_pwm = 80;
			break;
		default:
			break;
	}
}

/***************************************************************
* 函数名称: e53_sc1_init
* 说    明: 初始化e53
* 参    数: 无
* 返 回 值: 无
***************************************************************/
int e53_sc1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : LED_Pin */
	GPIO_InitStruct.Pin = LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

	Init_BH1750();
	TIM3_Init(1, 719);
	e53_sc1_set_ligth_level(2);
	return 0;
}
