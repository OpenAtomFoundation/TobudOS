/**
 * @file   TencentOS_common.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_TENCENT_OS_COMMON_H_
#define _KERNEL_OS_TENCENT_OS_COMMON_H_

#include <stdint.h>
//#include "compiler.h"
#include "tos_k.h"
#include "ln88xx.h"

#define __always_inline	__forceinline


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Thread priority definition
 */
typedef enum  {
    OS_PRIORITY_IDLE            = 6,
    OS_PRIORITY_LOW             = 5,
    OS_PRIORITY_BELOW_NORMAL    = 4,
    OS_PRIORITY_NORMAL          = 3,
    OS_PRIORITY_ABOVE_NORMAL    = 2,
    OS_PRIORITY_HIGH            = 1,
    OS_PRIORITY_REAL_TIME       = 0
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
#define OS_SEMAPHORE_MAX_COUNT  0xffffU /* Maximum count value for semaphore */

/* check if in ISR context or not */
static __always_inline int OS_IsISRContext(void)
{
	return __get_IPSR();
}



/* memory */
#include <stdlib.h>
void * OS_Malloc(size_t size);
void OS_Free(void *p);

//#define OS_Memcpy(d, s, l) 
//#define OS_Memset(d, c, l) 
//#define OS_Memcmp(a, b, l) 
//#define OS_Memmove(d, s, n)

void os_heap_mem_add_pool(void);

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_TENCENT_OS_COMMON_H_ */


