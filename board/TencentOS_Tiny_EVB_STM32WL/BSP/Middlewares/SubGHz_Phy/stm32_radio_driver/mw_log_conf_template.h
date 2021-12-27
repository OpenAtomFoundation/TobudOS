/**
 ******************************************************************************
 * @file    mw_log_conf.h
 * @author  MCD Application Team
 * @brief   Interface layer CM4 System to MBMUX (Mailbox Multiplexer)
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
#ifndef __MW_LOG_CONF_H__
#define __MW_LOG_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "trace.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define MW_LOG_ENABLED 
/* Exported macro ------------------------------------------------------------*/

#ifdef MW_LOG_ENABLED
#define PPRINTF(...)  do{ } while( 0!= TraceSend(TIME_STAMP_OFF, 0, __VA_ARGS__) ) //Polling Mode
#define TPRINTF(...)   do{ {TraceSend(TIME_STAMP_ON, 0, __VA_ARGS__);} }while(0)//with timestamp
#define PRINTF(...)   do{ {TraceSend(TIME_STAMP_OFF, 0, __VA_ARGS__);} }while(0)
#define MW_LOG(VL, TS, ...)   do{ {TraceSend(TS, VL, __VA_ARGS__);} }while(0)

#else
#define PPRINTF(...)
#define TPRINTF(...)
#define PRINTF(...)
#define MW_LOG(VL, TS, ...) 
#endif


/* Exported functions ------------------------------------------------------- */



#ifdef __cplusplus
}
#endif

#endif /*__MW_LOG_CONF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
