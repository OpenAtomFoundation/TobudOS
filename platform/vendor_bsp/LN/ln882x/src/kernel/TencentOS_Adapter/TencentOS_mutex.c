/**
 * @file    freertos_mutex.c
 * @author  LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */
#include "./TencentOS_Adapter/TencentOS_mutex.h"
#include "./TencentOS_Adapter/TencentOS_time.h"
#include "./TencentOS_Adapter/TencentOS_debug.h"

/*-------------------------Recursive Mutex---------------------------------------*/
OS_Status OS_MutexCreate(OS_Mutex_t *mutex)
{
	//TODO: check handle is null.

    if(NULL == (mutex->handle = OS_Malloc(sizeof(k_mutex_t)))){
        OS_ERR("err:os mutex handle malloc failed.\r\n");
        return OS_FAIL;
    }

    k_err_t err = tos_mutex_create(mutex->handle);
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_Free(mutex->handle); 
        mutex->handle = NULL;
        OS_ERR("err:%d,OS_MutexCreate filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_MutexDelete(OS_Mutex_t *mutex)
{
    k_err_t err = tos_mutex_destroy(mutex->handle);
    if (err == K_ERR_NONE) {
        OS_Free(mutex->handle);        
        mutex->handle = NULL;
        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_MutexDelete.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_MutexLock(OS_Mutex_t *mutex, OS_Time_t waitMS)
{
    k_err_t err = tos_mutex_pend_timed(mutex->handle, OS_CalcWaitTicks(waitMS));
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_MutexLock filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_MutexUnlock(OS_Mutex_t *mutex)
{
    k_err_t err = tos_mutex_post(mutex->handle);
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_MutexUnlock filed.\r\n", err);
        return OS_FAIL;
    }
}


OS_ThreadHandle  OS_MutexGetOwner(OS_Mutex_t *mutex)
{
    //TODO: check handle is null
	return (OS_ThreadHandle)mutex->handle->owner;
}



