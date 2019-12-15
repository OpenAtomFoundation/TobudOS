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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

//#define PLATFORM_HAS_CMSIS

#ifdef PLATFORM_HAS_CMSIS
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"
#endif

//TODO platform dependant
void HAL_SleepMs(_IN_ uint32_t ms)
{
  TickType_t ticks = ms / portTICK_PERIOD_MS;

  vTaskDelay(ticks ? ticks : 1);          /* Minimum delay = 1 tick */

  return ;
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
	return pvPortMalloc( size);
}

void HAL_Free(_IN_ void *ptr)
{
	vPortFree(ptr);
}



void *HAL_MutexCreate(void)
{
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    if (NULL == mutex) {
        HAL_Printf("%s: xSemaphoreCreateMutex failed\n", __FUNCTION__);
        return NULL;
    }

    return mutex;
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
    if (xSemaphoreTake(mutex, 0) != pdTRUE) {
        HAL_Printf("%s: xSemaphoreTake failed\n", __FUNCTION__);
    }

    vSemaphoreDelete(mutex);
}

void HAL_MutexLock(_IN_ void *mutex)
{
    if (!mutex) {
        HAL_Printf("%s: invalid mutex\n", __FUNCTION__);
        return ;
    }
    
    if (xSemaphoreTake(mutex, portMAX_DELAY) != pdTRUE) {
        HAL_Printf("%s: xSemaphoreTake failed\n", __FUNCTION__);
        return ;
    }
}

int HAL_MutexTryLock(_IN_ void *mutex)
{
    if (!mutex) {
        HAL_Printf("%s: invalid mutex\n", __FUNCTION__);
        return -1;
    }
    
    if (xSemaphoreTake(mutex, 0) != pdTRUE) {
        HAL_Printf("%s: xSemaphoreTake failed\n", __FUNCTION__);
        return -1;
    }
    
    return 0;
}


void HAL_MutexUnlock(_IN_ void *mutex)
{
    if (!mutex) {
        HAL_Printf("%s: invalid mutex\n", __FUNCTION__);
        return ;
    }

    if (xSemaphoreGive(mutex) != pdTRUE) {
        HAL_Printf("%s: xSemaphoreGive failed\n", __FUNCTION__);
        return ;
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
