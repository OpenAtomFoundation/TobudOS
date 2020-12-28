/******************************************************************************
 * @file    timer.h
 * @author  MCD Application Team
 * @brief   wrapper to timer server
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
#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32_timer.h"
#include <tos_k.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define TIMER_MAX_VAL 0xFFFFFFFFU

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define TimerTime_t k_tick_t

#define TimerEvent_t k_timer_t

#define TimerInit(HANDLE, CB) do {\
                                 tos_timer_create( HANDLE, TIMER_MAX_VAL, 0, CB, NULL, TOS_OPT_TIMER_ONESHOT);\
                                 } while(0)

#define TimerSetValue(HANDLE, TIMEOUT) do{ \
                                       tos_timer_delay_change(HANDLE, TIMEOUT);\
                                       } while(0)

#define TimerStart(HANDLE)   do {\
                                tos_timer_start(HANDLE);\
                                } while(0)

#define TimerStop(HANDLE)   do {\
                               tos_timer_stop(HANDLE);\
                               } while(0)

#define TimerGetCurrentTime  TIMER_GetCurrentTime

#define TimerGetElapsedTime TIMER_GetElapsedTime
							   
uint8_t TIMER_GetCurrentTime(void);

uint8_t TIMER_GetElapsedTime(TimerTime_t past);

/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
