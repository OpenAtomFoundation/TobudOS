/**
 * @file   freertos_semaphore.c
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#include "./TencentOS_Adapter/TencentOS_semaphore.h"
#include "./TencentOS_Adapter/TencentOS_time.h"
#include "./TencentOS_Adapter/TencentOS_debug.h"


OS_Status OS_SemaphoreCreate(OS_Semaphore_t *sem, uint32_t initCount, uint32_t maxCount)
{
    //TODO: check handle is null
    
    if(NULL == (sem->handle = OS_Malloc(sizeof(k_sem_t)))){
        OS_ERR("err:os semaphore handle  malloc failed.\r\n");
        return OS_FAIL;
    }

    k_err_t err = tos_sem_create_max(sem->handle, (k_sem_cnt_t)initCount, (k_sem_cnt_t)maxCount);
    
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_Free(sem->handle);
        sem->handle = NULL; 
        OS_ERR("err:%d,OS_SemaphoreCreate filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_SemaphoreCreateBinary(OS_Semaphore_t *sem)
{
    //TODO: check handle is null
    
    if(NULL == (sem->handle = OS_Malloc(sizeof(k_sem_t)))){
        OS_ERR("err:os semaphore handle binary malloc failed.\r\n");
        return OS_FAIL;
    }
    
    k_err_t err = tos_sem_create_max(sem->handle, (k_sem_cnt_t)0u, (k_sem_cnt_t)1u);

    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_Free(sem->handle);
        sem->handle = NULL; 
        OS_ERR("err:%d,OS_SemaphoreCreateBinary filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_SemaphoreDelete(OS_Semaphore_t *sem)
{
    k_err_t err = tos_sem_destroy(sem->handle);
    
    if (err == K_ERR_NONE) {
        OS_Free(sem->handle);
        sem->handle = NULL; 
        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_SemaphoreDelete filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_SemaphoreWait(OS_Semaphore_t *sem, OS_Time_t waitMS)
{
    k_err_t err = tos_sem_pend(sem->handle, OS_CalcWaitTicks(waitMS));
    
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
//        OS_ERR("err:%d,OS_SemaphoreWait filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_SemaphoreRelease(OS_Semaphore_t *sem)
{
    k_err_t err = tos_sem_post(sem->handle);
    
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_SemaphoreRelease filed.\r\n", err);
        return OS_FAIL;
    }
}

uint16_t OS_SemaphoreGetCount(OS_Semaphore_t *sem)
{
    //TODO:handle check!
    return sem->handle->count;
}


