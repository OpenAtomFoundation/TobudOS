/**
 * @file   TencentOS_mutex.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_TENCENT_OS_MUTEX_H_
#define _KERNEL_OS_TENCENT_OS_MUTEX_H_

#include "./TencentOS_Adapter/TencentOS_common.h"
#include "./TencentOS_Adapter/TencentOS_time.h"
#include "./TencentOS_Adapter/TencentOS_thread.h"
#include "tos_k.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef k_mutex_t * OS_MutexHandle;

typedef struct OS_Mutex {
	k_mutex_t * handle;
} OS_Mutex_t;


OS_Status OS_MutexCreate(OS_Mutex_t *mutex);
OS_Status OS_MutexDelete(OS_Mutex_t *mutex);
OS_Status OS_MutexLock(OS_Mutex_t *mutex, OS_Time_t waitMS);
OS_Status OS_MutexUnlock(OS_Mutex_t *mutex);

static __always_inline OS_Status OS_RecursiveMutexCreate(OS_Mutex_t *mutex){
    return OS_MutexCreate(mutex);
};
static __always_inline OS_Status OS_RecursiveMutexDelete(OS_Mutex_t *mutex){
	return OS_MutexDelete(mutex);
};

static __always_inline OS_Status OS_RecursiveMutexLock(OS_Mutex_t *mutex, OS_Time_t waitMS){
    return OS_MutexLock(mutex,waitMS);
};

static __always_inline OS_Status OS_RecursiveMutexUnlock(OS_Mutex_t *mutex){
    return OS_MutexUnlock(mutex);
};

OS_ThreadHandle  OS_MutexGetOwner(OS_Mutex_t *mutex);

static __always_inline int OS_MutexIsValid(OS_Mutex_t *mutex)
{
	return ((mutex->handle != NULL) && (mutex->handle->knl_obj.type == KNL_OBJ_TYPE_MUTEX));
}

static __always_inline void OS_MutexSetInvalid(OS_Mutex_t *mutex)
{
	mutex->handle->knl_obj.type == KNL_OBJ_TYPE_NONE;
}



#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_TENCENT_OS_MUTEX_H_ */
