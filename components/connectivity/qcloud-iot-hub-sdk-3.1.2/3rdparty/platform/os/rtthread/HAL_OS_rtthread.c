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
#include <stdarg.h>
#include <string.h>
#include <rtthread.h>
#include "rtconfig.h"
#include "qcloud_iot_import.h"
#include "qcloud_iot_export.h"


#define HAL_OS_LOG_MAXLEN   1024
static uint16_t g_mutex_count = 0;



void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;
	char log_buf[HAL_OS_LOG_MAXLEN];
    
    va_start(args, fmt);
    rt_vsnprintf(log_buf, HAL_OS_LOG_MAXLEN, fmt, args);
    va_end(args);
    printf("%s", log_buf);
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


void *HAL_MutexCreate(void)
{
	rt_mutex_t mutex;
	char mutexName[RT_NAME_MAX];

	memset(mutexName, 0, RT_NAME_MAX);
	HAL_Snprintf(mutexName, RT_NAME_MAX, "tmutex_%d", g_mutex_count++);	
	mutex = rt_mutex_create(mutexName, RT_IPC_FLAG_FIFO);
    if (NULL == mutex) {
		HAL_Printf("create mutex failed");

    }

    return mutex;


}

void HAL_MutexDestroy(_IN_ void *mutex)
{
	int err_num;

	err_num = rt_mutex_delete((rt_mutex_t)mutex);

    if (0 != err_num) 
	{
        HAL_Printf("destroy mutex failed");
    }
}

void HAL_MutexLock(_IN_ void *mutex)
{
	int err_num;
	
	err_num = rt_mutex_take((rt_mutex_t)mutex, RT_WAITING_FOREVER);

    if (0 != err_num)
	{
        HAL_Printf("lock mutex failed");
    }
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
    int err_num;

	err_num = rt_mutex_release((rt_mutex_t)mutex);
	 
    if (0 != err_num)
	{
        HAL_Printf("unlock mutex failed");
    }

}

int HAL_MutexTryLock(_IN_ void *mutex)
{
	int err_num;
	
	err_num = rt_mutex_take((rt_mutex_t)mutex, RT_WAITING_NO);

    if (0 != err_num)
	{
        HAL_Printf("trylock mutex failed");
    }
    return err_num;
}


void *HAL_Malloc(_IN_ uint32_t size)
{
   return rt_malloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
    rt_free(ptr);
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    (void)rt_thread_delay(rt_tick_from_millisecond(ms));
}


