/**
 * @file   TencentOS_time.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_TENCENT_OS_TIME_H_
#define _KERNEL_OS_TENCENT_OS_TIME_H_

#include "./TencentOS_Adapter/TencentOS_common.h"
#include "tos_k.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef k_tick_t TickType_t;

/* Parameters used to convert the time values */
#define OS_MSEC_PER_SEC     1000U       /* milliseconds per second */
#define OS_USEC_PER_MSEC    1000U       /* microseconds per millisecond */
#define OS_USEC_PER_SEC     1000000U    /* microseconds per second */

/* system clock's frequency, OS ticks per second */
#define OS_HZ               TOS_CFG_CPU_TICK_PER_SECOND

/* microseconds per OS tick (1000000 / OS_HZ) */
#define OS_TICK             (OS_USEC_PER_SEC / OS_HZ)

/** @brief Get the number of ticks since OS start */
/* Due to portTICK_TYPE_IS_ATOMIC is 1, calling xTaskGetTickCount() in ISR is
 * safe also.
 */
#define OS_GetTicks()       ((TickType_t)tos_systick_get())

/** @brief Get the number of seconds since OS start */
#define OS_GetTime()        (OS_GetTicks() / OS_HZ)


#define OS_SecsToTicks(sec)     ((uint32_t)(sec) * OS_HZ)
#define OS_MSecsToTicks(msec)   ((uint32_t)(msec) * (OS_USEC_PER_MSEC / OS_TICK))

#define OS_TicksToMSecs(t)      ((TickType_t)((t) / (OS_USEC_PER_MSEC / OS_TICK)))
#define OS_TicksToSecs(t)       ((TickType_t)((t) / (OS_USEC_PER_SEC  / OS_TICK)))


#define OS_MsDelay(msec)         tos_task_delay((TickType_t)OS_MSecsToTicks(msec))
#define OS_Delay(sec)            OS_MsDelay((sec) * OS_MSEC_PER_SEC)
#define OS_SecDelay(sec)         OS_Delay(sec)


#define OS_Rand32()  ((uint32_t)(((*((volatile uint32_t *)0xE000E018)) & 0xffffff) | (OS_GetTicks() << 24)))

static __always_inline TickType_t OS_CalcWaitTicks(OS_Time_t msec)
{
	TickType_t tick;
	if (msec == OS_WAIT_FOREVER) 
	{
		tick = TOS_TIME_FOREVER;
	} 
	else if (msec != 0) 
	{
		tick = OS_MSecsToTicks(msec);
		if (tick == 0){
			tick = 1;
		}			
	}
	else 
	{
		tick = TOS_TIME_NOWAIT;
	}
	return tick;
}


#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_TENCENT_OS_TIME_H_ */
