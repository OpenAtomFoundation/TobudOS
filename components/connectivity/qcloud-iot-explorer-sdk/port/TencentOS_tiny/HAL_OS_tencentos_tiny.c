/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "qcloud_iot_import.h"
#include "tos.h"

//#define PLATFORM_HAS_CMSIS

#ifdef PLATFORM_HAS_CMSIS
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"
#endif

//TODO platform dependant
void HAL_SleepMs(_IN_ uint32_t ms)
{
  (void)tos_sleep_hmsm(0,0,0, ms);
}


void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}


void *HAL_Malloc(_IN_ uint32_t size)
{
	return tos_mmheap_alloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
	tos_mmheap_free(ptr);
}

void *HAL_MutexCreate(void)
{
    k_mutex_t *mutex;

    mutex = (k_mutex_t *)HAL_Malloc(sizeof(k_mutex_t));
    if (!mutex) {
        return K_NULL;
    }

    tos_mutex_create(mutex);

    return (void *)mutex;
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
	k_err_t ret;

    if (K_ERR_NONE != (ret = tos_mutex_destroy((k_mutex_t *)mutex))) {
        HAL_Printf("osal_mutex_destroy err, err:%d\n\r", ret);
    } else {
        HAL_Free((void *)mutex);
    }
}

void HAL_MutexLock(_IN_ void *mutex)
{
    k_err_t ret;

    if (K_ERR_NONE != (ret = tos_mutex_pend((k_mutex_t *)mutex))) {
        HAL_Printf("osal_mutex_lock err, err:%d\n\r", ret);
    }
}

int HAL_MutexTryLock(_IN_ void *mutex)
{
	k_err_t ret;

	if (K_ERR_NONE != (ret = tos_mutex_pend_timed((k_mutex_t *)mutex, 0))) {
		HAL_Printf("osal_mutex_lock err, err:%d\n\r", ret);
		return (int)ret;
	}

	return 0;
}


void HAL_MutexUnlock(_IN_ void *mutex)
{
	k_err_t ret;

	if (K_ERR_NONE != (ret = tos_mutex_post((k_mutex_t *)mutex))) {
		HAL_Printf("osal_mutex_unlock err, err:%d\n\r", ret);
	}
}

#if defined(PLATFORM_HAS_CMSIS) && defined(AT_TCP_ENABLED)

/*
* return void* threadId
*/
void * HAL_ThreadCreate(uint16_t stack_size, int priority, char * taskname,void *(*fn)(void*), void* arg)
{
	osThreadId thread_t = (osThreadId)HAL_Malloc(sizeof(osThreadId));

	osThreadDef(taskname, (os_pthread)fn, (osPriority)priority, 0, stack_size);
	thread_t = osThreadCreate(osThread(taskname), arg);
	if(NULL == thread_t){
		HAL_Printf("create thread fail\n\r");
	}

	return (void *)thread_t;
}

int HAL_ThreadDestroy(void* threadId)
{
	return osThreadTerminate(threadId);
}

void *HAL_SemaphoreCreate(void)
{
	return (void *)osSemaphoreCreate(NULL, 1);
}

void HAL_SemaphoreDestroy(void *sem)
{
	osStatus ret;

	ret = osSemaphoreDelete ((osSemaphoreId)sem);
	if(osOK != ret)
	{
		HAL_Printf("HAL_SemaphoreDestroy err, err:%d\n\r",ret);
	}
}

void HAL_SemaphorePost(void *sem)
{
	osStatus ret;

	ret = osSemaphoreRelease ((osSemaphoreId) sem);

	if(osOK != ret)
	{
		HAL_Printf("HAL_SemaphorePost err, err:%d\n\r",ret);
	}
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
	return osSemaphoreWait ((osSemaphoreId)sem, timeout_ms);

}
#endif
