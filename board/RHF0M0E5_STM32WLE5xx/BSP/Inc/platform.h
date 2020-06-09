/**
  ******************************************************************************
  * @file   platform.h
  * @author MCD Application Team
  * @brief  configuration of general hw instances
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
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stm32wlxx.h"
#include "stm32wlxx_ll_gpio.h"
#include "main.h"
#include "usart.h"
#include "rtc.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/

/* #warning "DBG LCH: VREFINT to be defined from AN" */


/* Nothing for L4 but other platform might need it */
#define __HAL_RCC_DBGMCU_CLK_ENABLE()
#define __HAL_RCC_DBGMCU_CLK_DISABLE()

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
