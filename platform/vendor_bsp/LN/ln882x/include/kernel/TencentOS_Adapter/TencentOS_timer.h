/**
 * @file   TencentOS_timer.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_TENCENT_OS_TIMER_H_
#define _KERNEL_OS_TENCENT_OS_TIMER_H_

#include "./TencentOS_Adapter/TencentOS_common.h"
#include "tos_k.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    OS_TIMER_ONCE       = 0, /* one shot timer */
    OS_TIMER_PERIODIC   = 1  /* periodic timer */
} OS_TimerType;

typedef k_timer_callback_t OS_TimerCallback_t;
typedef k_timer_t        * OS_TimerHandle;

typedef struct OS_Timer {
    k_timer_t * handle;
} OS_Timer_t;


OS_Status OS_TimerCreate(OS_Timer_t *timer, OS_TimerType type,OS_TimerCallback_t cb, void *arg, OS_Time_t periodMS);
OS_Status OS_TimerDelete(OS_Timer_t *timer);
OS_Status OS_TimerStart(OS_Timer_t *timer);
OS_Status OS_TimerChangeDelay( OS_Timer_t *timer, OS_Time_t periodMS);
OS_Status OS_TimerChangePeriod(OS_Timer_t *timer, OS_Time_t periodMS);
OS_Status OS_TimerStop(OS_Timer_t *timer);


#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_TENCENT_OS_TIMER_H_ */
