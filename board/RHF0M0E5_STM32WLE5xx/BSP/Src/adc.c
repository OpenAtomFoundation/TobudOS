/**
  ******************************************************************************
  * @file    adc.c
  * @author  MCD Application Team
  * @brief   configuration of the ADC instances
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "adc.h"
#include "dma.h"

#ifdef HAL_ADC_MODULE_ENABLED
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/


/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */

void MX_ADC1_Init(void)
{
  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */



  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution            = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode          = ADC_SCAN_DISABLE;              /* Sequencer set to fully configurable: only the rank 1 is enabled (no scan sequence on several ranks) */
  hadc1.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  hadc1.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
  hadc1.Init.NbrOfConversion       = 1;                             /* Parameter discarded because sequencer is disabled. Parameter relevancy depending on setting of parameter "ScanConvMode" */
  hadc1.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
  hadc1.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because trig of conversion by software start (no external event) */
  hadc1.Init.DMAContinuousRequests = DISABLE;                       /* ADC with DMA transfer: continuous requests to DMA disabled (default state) since DMA is not used in this example. */
  hadc1.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.SamplingTimeCommon1   = LL_ADC_SAMPLINGTIME_160CYCLES_5;
  hadc1.Init.OversamplingMode      = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */
}


/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable clock of GPIO associated to the peripheral channels */
  /* Enable clock of ADCx peripheral (core clock) */
  __HAL_RCC_ADC_CLK_ENABLE();

  /* Note: In case of usage of asynchronous clock for ADC, with ADC setting   */
  /*       "AdcHandle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIVx",            */
  /*       the clock source has to be enabled at RCC top level using function */
  /*       "HAL_RCCEx_PeriphCLKConfig()" or macro "__HAL_RCC_ADC_CONFIG()"    */
  /*       (refer to comments in driver file header).                         */

  /*##-4- Configure the NVIC #################################################*/
  /* NVIC configuration for ADC interrupt */
  /* Priority: high-priority */

}

/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC1)
  {
    /* USER CODE BEGIN ADC1_MspDeInit 0 */

    /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PC3     ------> ADC1_IN4
    */
    /* USER CODE BEGIN ADC1_MspDeInit 1 */

    /* USER CODE END ADC1_MspDeInit 1 */
  }

}
#endif
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
