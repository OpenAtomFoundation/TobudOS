/**
 * @file   freertos_mutex.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_FREERTOS_OS_MUTEX_H_
#define _KERNEL_OS_FREERTOS_OS_MUTEX_H_

#include "./FreeRTOS_Adapter/freertos_common.h"
#include "./FreeRTOS_Adapter/freertos_time.h"
#include "./FreeRTOS_Adapter/freertos_thread.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mutex object definition
 */
typedef struct OS_Mutex {
	SemaphoreHandle_t	handle;
} OS_Mutex_t;

OS_Status OS_MutexCreate(OS_Mutex_t *mutex);
OS_Status OS_MutexDelete(OS_Mutex_t *mutex);
OS_Status OS_MutexLock(OS_Mutex_t *mutex, OS_Time_t waitMS);
OS_Status OS_MutexUnlock(OS_Mutex_t *mutex);

OS_Status OS_RecursiveMutexCreate(OS_Mutex_t *mutex);
OS_Status OS_RecursiveMutexLock(OS_Mutex_t *mutex, OS_Time_t waitMS);
OS_Status OS_RecursiveMutexUnlock(OS_Mutex_t *mutex);

/**
 * @brief Delete the recursive mutex object
 * @param[in] mutex Pointer to the recursive mutex object
 * @retval OS_Status, OS_OK on success
 */
__STATIC_INLINE OS_Status OS_RecursiveMutexDelete(OS_Mutex_t *mutex)
{
	return OS_MutexDelete(mutex);
}

/**
 * @brief Check whether the mutex object is valid or not
 * @param[in] mutex Pointer to the mutex object
 * @return 1 on valid, 0 on invalid
 */
__STATIC_INLINE int OS_MutexIsValid(OS_Mutex_t *mutex)
{
	return (mutex->handle != OS_INVALID_HANDLE);
}

/**
 * @brief Set the mutex object to invalid state
 * @param[in] mutex Pointer to the mutex object
 * @return None
 */
__STATIC_INLINE void OS_MutexSetInvalid(OS_Mutex_t *mutex)
{
	mutex->handle = OS_INVALID_HANDLE;
}

/**
 * @brief Get the mutex object's owner
 * @note A mutex object's owner is a thread that locks the mutex
 * @param[in] mutex Pointer to the mutex object
 * @return The handle of the thread that locks the mutex object.
 *         NULL when the mutex is not locked by any thread.
 */
__STATIC_INLINE OS_ThreadHandle_t OS_MutexGetOwner(OS_Mutex_t *mutex)
{
	return (OS_ThreadHandle_t)xSemaphoreGetMutexHolder(mutex->handle);
}

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_FREERTOS_OS_MUTEX_H_ */
