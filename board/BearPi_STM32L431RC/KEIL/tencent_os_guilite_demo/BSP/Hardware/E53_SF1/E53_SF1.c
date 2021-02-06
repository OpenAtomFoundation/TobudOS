#include "E53_SF1.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "main.h"

ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim16;

/***************************************************************
* 函数名称: MX_TIM16_Init
* 说    明: 初始化定时器16
* 参    数: 无
* 返 回 值: 无
***************************************************************/
/* TIM16 init function */
void MX_TIM16_Init(void)
{
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 79;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 999;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim16);

}
/***************************************************************
* 函数名称: HAL_TIM_Base_MspInit
* 说    明: 使能定时器16时钟,设置时钟源
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

	if(tim_baseHandle->Instance==TIM16)
  {
  /* USER CODE BEGIN TIM16_MspInit 0 */

  /* USER CODE END TIM16_MspInit 0 */
    /* TIM16 clock enable */
    __HAL_RCC_TIM16_CLK_ENABLE();
  /* USER CODE BEGIN TIM16_MspInit 1 */

  /* USER CODE END TIM16_MspInit 1 */
  }
}

/***************************************************************
* 函数名称: HAL_TIM_MspPostInit
* 说    明: 使能定时器16硬件初始化
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(timHandle->Instance==TIM16)
  {
  /* USER CODE BEGIN TIM16_MspPostInit 0 */

  /* USER CODE END TIM16_MspPostInit 0 */
  
    /**TIM16 GPIO Configuration    
    PB8     ------> TIM16_CH1 
    */
    GPIO_InitStruct.Pin = SF1_Beep_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_TIM16;
    HAL_GPIO_Init(SF1_Beep_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM16_MspPostInit 1 */

  /* USER CODE END TIM16_MspPostInit 1 */
  }

}

/***************************************************************
* 函数名称: MX_ADC1_Init
* 说    明: 初始化ADC1电压采集通道
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/***************************************************************
* 函数名称: HAL_ADC_MspInit
* 说    明: 使能ADC时钟,设置时钟源
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();
  
    /**ADC1 GPIO Configuration    
    PC2     ------> ADC1_IN3 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

/***************************************************************
* 函数名称: Init_Beep
* 说    明: 初始化E53_SF1的蜂鸣器
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_Beep(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  SF1_Beep_GPIO_CLK_ENABLE();

	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SF1_Beep_GPIO_Port, SF1_Beep_Pin, GPIO_PIN_RESET);
	
	 /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SF1_Beep_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SF1_Beep_GPIO_Port, &GPIO_InitStruct);
}

/***************************************************************
* 函数名称: Init_LED
* 说    明: 初始化E53_SF1的LED灯
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_LED(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  SF1_LED_GPIO_CLK_ENABLE();

	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SF1_LED_GPIO_Port, SF1_LED_Pin, GPIO_PIN_SET);
	
	 /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SF1_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SF1_LED_GPIO_Port, &GPIO_InitStruct);
}
/***************************************************************
* 函数名称: Init_E53_SF1
* 说    明: 初始化E53_SF1扩展板
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_E53_SF1(void)
{
	MX_ADC1_Init();
	Init_Beep();
	Init_LED();
	MX_TIM16_Init();	
}
/***************************************************************
* 函数名称: E53_SF1_Read_Data
* 说    明: 获取烟雾传感器的数据
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void E53_SF1_Read_Data(void)
{
	HAL_ADC_Start(&hadc1);	
	HAL_ADC_PollForConversion(&hadc1, 50);
	E53_SF1_Data.Smoke_Value = HAL_ADC_GetValue(&hadc1);
}
/***************************************************************
* 函数名称: E53SF1_LED_StatusSet
* 说    明: E53SF1开发板上的LED灯的亮灭控制
* 参    数: status,LED灯的状态
*									非1,关灯
*									1,开灯
* 返 回 值: 无
***************************************************************/
void E53SF1_LED_StatusSet(E53SF1_Status_ENUM status)
{
	HAL_GPIO_WritePin(SF1_LED_GPIO_Port, SF1_LED_Pin, status != ON ? GPIO_PIN_RESET : GPIO_PIN_SET );	
}

/***************************************************************
* 函数名称: E53SF1_BEEP
* 说    明: E53SF1蜂鸣器报警与否
* 参    数: status,LED_ENUM枚举的数据
*									LED_OFF,关灯
*									LED_ON,开灯
* 返 回 值: 无
***************************************************************/
void E53SF1_Beep(E53SF1_Status_ENUM status)
{
	if(status == ON)
		HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
	if(status == OFF)
		HAL_TIM_PWM_Stop(&htim16,TIM_CHANNEL_1);
}


