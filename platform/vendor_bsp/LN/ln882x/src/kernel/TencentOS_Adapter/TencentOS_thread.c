/**
 * @file   freertos_thread.c
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */
 
#include "./TencentOS_Adapter/TencentOS_thread.h"
#include "./TencentOS_Adapter/TencentOS_debug.h"

OS_Status OS_ThreadCreateTimeslice(OS_Thread_t *thread, const char *name, OS_ThreadEntry_t entry, \
                           void *arg, OS_Priority priority, uint32_t stackSize, uint32_t timeslice)
{
    k_err_t err;

    //TODO: check handle is null.
    
    err = tos_task_create_dyn(&thread->handle,(char*)name,entry,arg,       \
                                            (k_prio_t)priority,       \
                                            stackSize,                \
                                            (k_timeslice_t)timeslice);

    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {        
        OS_ERR("err:%d,OS_ThreadCreate filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_ThreadDelete(OS_Thread_t *thread)
{
    k_err_t err;
    
    if(thread == NULL){
        err = tos_task_destroy(NULL);
    } else {
        err = tos_task_destroy(thread->handle);
    }
    
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_ThreadDelete filed.\r\n", err);
        return OS_FAIL;
    }
}



