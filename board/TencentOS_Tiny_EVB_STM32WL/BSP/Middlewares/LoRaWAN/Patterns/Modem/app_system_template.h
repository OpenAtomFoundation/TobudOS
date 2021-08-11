/**
  ******************************************************************************
  * @file   app_system_template.h
  * @author MCD Application Team
  * @brief  function prototypes for app_system.c file
  * @note   this is a template: the app_conf.h shall be defined in the Project/...
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "trace.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define VERBOSE_LEVEL   VLEVEL_L

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define APP_PPRINTF(...)  do{ } while( 0!= TraceSend(0, 0, __VA_ARGS__) ) //Polling Mode
#define APP_TPRINTF(...)   do{ {TraceSend(1, 0, __VA_ARGS__);} }while(0); //with timestamp
#define APP_PRINTF(...)   do{ {TraceSend(0, 0, __VA_ARGS__);} }while(0);


#if defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 1)
#define APP_LOG(TS,VL,...)   do{ {TraceSend(TS, VL, __VA_ARGS__);} }while(0);
#elif defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 0) /* APP_LOG disabled */
#define APP_LOG(TS,VL,...)
#else
#error "APP_LOG_ENABLED not defined or out of range <0,1>"
#endif

/* Defines -------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif /* __APP_SYSTEM_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
