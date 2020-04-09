/**
 * @file   freertos_semaphore.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_FREERTOS_OS_SEMAPHORE_H_
#define _KERNEL_OS_FREERTOS_OS_SEMAPHORE_H_

#include "./FreeRTOS_Adapter/freertos_common.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Semaphore object definition
 */
typedef struct OS_Semaphore {
    SemaphoreHandle_t   handle;
} OS_Semaphore_t;

OS_Status OS_SemaphoreCreate(OS_Semaphore_t *sem, uint32_t initCount, uint32_t maxCount);
OS_Status OS_SemaphoreCreateBinary(OS_Semaphore_t *sem);
OS_Status OS_SemaphoreDelete(OS_Semaphore_t *sem);
OS_Status OS_SemaphoreWait(OS_Semaphore_t *sem, OS_Time_t waitMS);
OS_Status OS_SemaphoreRelease(OS_Semaphore_t *sem);
UBaseType_t OS_SemaphoreGetCount(OS_Semaphore_t *sem);

/**
 * @brief Check whether the semaphore object is valid or not
 * @param[in] sem Pointer to the semaphore object
 * @return 1 on valid, 0 on invalid
 */
__STATIC_INLINE int OS_SemaphoreIsValid(OS_Semaphore_t *sem)
{
	return (sem->handle != OS_INVALID_HANDLE);
}

/**
 * @brief Set the semaphore object to invalid state
 * @param[in] sem Pointer to the semaphore object
 * @return None
 */
__STATIC_INLINE void OS_SemaphoreSetInvalid(OS_Semaphore_t *sem)
{
	sem->handle = OS_INVALID_HANDLE;
}

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_FREERTOS_OS_SEMAPHORE_H_ */
