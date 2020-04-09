/**
 * @file   freertos_errno.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_FREERTOS_OS_ERRNO_H_
#define _KERNEL_OS_FREERTOS_OS_ERRNO_H_

#include "./FreeRTOS_Adapter/freertos_common.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Thread safe errno handling for FreeRTOS
 */

#if (configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0)

#define OS_ERRNO_LOCATION_IDX		0

/**
 * @brief Get error number of the current thread
 * @return Error number of the current thread
 */
__STATIC_INLINE int OS_GetErrno(void)
{
	return (int)pvTaskGetThreadLocalStoragePointer(NULL, OS_ERRNO_LOCATION_IDX);
}

/**
 * @brief Set error number of the current thread
 * @param[in] Error number to be set
 * @return None
 */
__STATIC_INLINE void OS_SetErrno(int err)
{
	vTaskSetThreadLocalStoragePointer(NULL, OS_ERRNO_LOCATION_IDX, (void *)err);
}

#endif /* configNUM_THREAD_LOCAL_STORAGE_POINTERS */

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_FREERTOS_OS_ERRNO_H_ */
