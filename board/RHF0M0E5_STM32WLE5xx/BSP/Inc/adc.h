/**
  ******************************************************************************
  * @file   adc.h
  * @author MCD Application Team
  * @brief  configuration of the ADC instances
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
#ifdef HAL_ADC_MODULE_ENABLED
extern ADC_HandleTypeDef hadc1;
#endif
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* Exported macros -----------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void MX_ADC1_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
