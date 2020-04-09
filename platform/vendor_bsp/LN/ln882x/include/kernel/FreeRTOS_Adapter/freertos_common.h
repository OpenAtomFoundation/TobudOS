/**
 * @file   freertos_common.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_FREERTOS_OS_COMMON_H_
#define _KERNEL_OS_FREERTOS_OS_COMMON_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "ln88xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Thread priority definition
 */
typedef enum  {
    OS_PRIORITY_IDLE            = 0,
    OS_PRIORITY_LOW             = 1,
    OS_PRIORITY_BELOW_NORMAL    = 2,
    OS_PRIORITY_NORMAL          = 3,
    OS_PRIORITY_ABOVE_NORMAL    = 4,
    OS_PRIORITY_HIGH            = 5,
    OS_PRIORITY_REAL_TIME       = 6
} OS_Priority;

/**
 * @brief OS status definition
 */
typedef enum {
    OS_OK           = 0,    /* success */
    OS_FAIL         = -1,   /* general failure */
    OS_E_NOMEM      = -2,   /* out of memory */
    OS_E_PARAM      = -3,   /* invalid parameter */
    OS_E_TIMEOUT    = -4,   /* operation timeout */
    OS_E_ISR        = -5,   /* not allowed in ISR context */
} OS_Status;

/** @brief Type definition of OS time */
typedef uint32_t OS_Time_t;

#define OS_WAIT_FOREVER         0xffffffffU /* Wait forever timeout value */
#define OS_SEMAPHORE_MAX_COUNT  0xffffffffU /* Maximum count value for semaphore */
#define OS_INVALID_HANDLE       NULL        /* OS invalid handle */

/* check if in ISR context or not */
__STATIC_INLINE int OS_IsISRContext(void)
{
	return __get_IPSR();
}

/* memory */
#include <stdlib.h>
#define OS_Malloc(l)         pvPortMalloc(l)
#define OS_Free(p)           vPortFree(p)

#define OS_CPSR_ALLOC()
#define OS_ENTER_CRITICAL()                       taskENTER_CRITICAL()
#define OS_ENTER_CRITICAL_FROM_ISR()              taskENTER_CRITICAL_FROM_ISR()
#define OS_EXIT_CRITICAL()                        taskEXIT_CRITICAL()
#define OS_EXIT_CRITICAL_FROM_ISR(x)              taskEXIT_CRITICAL_FROM_ISR(x)
#define OS_DISABLE_INTERRUPTS()                   taskDISABLE_INTERRUPTS()
#define OS_ENABLE_INTERRUPTS()                    taskENABLE_INTERRUPTS()


void OS_DefineHeapRegions(void);

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_FREERTOS_OS_COMMON_H_ */
