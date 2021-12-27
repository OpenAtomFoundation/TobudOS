/**
  ******************************************************************************
  * @file   app_system.h
  * @author MCD Application Team
  * @brief  function prototypes for app_system.c file
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
#ifndef __APP_SYSTEM_H__
#define __APP_SYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "app_conf.h"

#include "mw_log_conf.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

#if defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 1)
	#define APP_LOG    LOG_PRINT 
	#define APP_PPRINTF   printf   
	#define APP_TPRINTF   printf 
	#define APP_PRINTF    printf 
#elif defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 0) 
	#define APP_LOG(...)
	#define APP_PPRINTF(...)       
	#define APP_TPRINTF(...)     
	#define APP_PRINTF(...)      
#else
	#error "APP_LOG_ENABLED not defined or out of range <0,1>"
#endif

/* Defines -------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief initialises the system (dbg pins, trace, systiemr, LPM, ...)
  * @param none
  * @retval  none
  */
void SystemApp_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* __APP_SYSTEM_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
