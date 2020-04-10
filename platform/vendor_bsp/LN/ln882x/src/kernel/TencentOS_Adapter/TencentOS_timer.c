/**
 * @file   adapter_timer.c
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#include "./TencentOS_Adapter/TencentOS_timer.h"
#include "./TencentOS_Adapter/TencentOS_time.h"
#include "./TencentOS_Adapter/TencentOS_debug.h"

int32_t soft_timer_count = 0;

OS_Status OS_TimerCreate(OS_Timer_t *timer, OS_TimerType type,OS_TimerCallback_t cb, void *arg, uint32_t periodMS)
{
    k_err_t err;
    
    if(NULL == (timer->handle = OS_Malloc(sizeof(k_timer_t)))){
        OS_ERR("err:os timer handle malloc failed.\r\n");
        return OS_FAIL;
    }

    if(type == OS_TIMER_ONCE){
        err = tos_timer_create(timer->handle, OS_MSecsToTicks(periodMS), (k_tick_t)0u, cb, arg, TOS_OPT_TIMER_ONESHOT);
    } else {
        err = tos_timer_create(timer->handle, (k_tick_t)0u, OS_MSecsToTicks(periodMS), cb, arg, TOS_OPT_TIMER_PERIODIC);
    }
    
    if (err == K_ERR_NONE){
        soft_timer_count++;
        return OS_OK;
	}else{
        OS_Free(timer->handle);        
        timer->handle = NULL;
        
		OS_ERR("err:%d,OS_TimerCreate filed.\r\n", err);        
		return OS_FAIL;        
    }
}

OS_Status OS_TimerDelete(OS_Timer_t *timer)
{
    k_err_t err = tos_timer_destroy(timer->handle);   
    if (err == K_ERR_NONE) {
        OS_Free(timer->handle); 
        timer->handle = NULL;
        soft_timer_count--;

        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_TimerDelete filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_TimerStart(OS_Timer_t *timer)
{
    k_err_t err = tos_timer_start(timer->handle);
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_TimerStart filed.\r\n", err);
        return OS_FAIL;
    }
}


OS_Status OS_TimerChangeDelay(OS_Timer_t *timer, uint32_t periodMS)
{
    tos_timer_stop(timer->handle);
    
    k_err_t err = tos_timer_delay_change(timer->handle, OS_MSecsToTicks(periodMS));
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_TimerChangeDelay filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_TimerChangePeriod(OS_Timer_t *timer, uint32_t periodMS)
{
    tos_timer_stop(timer->handle);

    k_err_t err = tos_timer_period_change(timer->handle, OS_MSecsToTicks(periodMS));
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_ERR("err:%d,OS_TimerChangePeriod filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_TimerStop(OS_Timer_t *timer)
{    
    //TODO:check timer handle
    if((timer == NULL) &&(timer->handle == NULL))
    {
        OS_ERR("err:OS_TimerStop handle invalid.\r\n");
        return OS_FAIL;
    }

    tos_timer_stop(timer->handle);
    return OS_OK;
}


