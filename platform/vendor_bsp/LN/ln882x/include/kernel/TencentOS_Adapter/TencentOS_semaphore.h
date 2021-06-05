/**
 * @file   TencentOS_semaphore.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_TENCENT_OS_SEMAPHORE_H_
#define _KERNEL_OS_TENCENT_OS_SEMAPHORE_H_

#include "./TencentOS_Adapter/TencentOS_common.h"
#include "tos_k.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef k_sem_t * OS_SemaphoreHandle;

typedef struct OS_Semaphore {
    k_sem_t * handle;
} OS_Semaphore_t;

OS_Status OS_SemaphoreCreate(OS_Semaphore_t *sem, uint32_t initCount, uint32_t maxCount);
OS_Status OS_SemaphoreCreateBinary(OS_Semaphore_t *sem);
OS_Status OS_SemaphoreDelete(OS_Semaphore_t *sem);
OS_Status OS_SemaphoreWait(OS_Semaphore_t *sem, OS_Time_t waitMS);
OS_Status OS_SemaphoreRelease(OS_Semaphore_t *sem);
uint16_t  OS_SemaphoreGetCount(OS_Semaphore_t *sem);

static __always_inline int OS_SemaphoreIsValid(OS_Semaphore_t *sem)
{
	return ((sem->handle != NULL) && (sem->handle->knl_obj.type == KNL_OBJ_TYPE_SEMAPHORE));
}

static __always_inline void OS_SemaphoreSetInvalid(OS_Semaphore_t *sem)
{
	sem->handle->knl_obj.type = KNL_OBJ_TYPE_NONE;
}


#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_TENCENT_OS_SEMAPHORE_H_ */
