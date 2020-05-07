/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "qcloud_iot_export_error.h"
#include "qcloud_iot_import.h"

void *HAL_MutexCreate(void)
{
#ifdef MULTITHREAD_ENABLED
    HANDLE mutex = CreateMutex(NULL, FALSE, NULL);

    if (mutex == NULL) {
        HAL_Printf("%s: create mutex failed\n", __FUNCTION__);
    }

    return (void *)mutex;
#else
    return (void *)0xFFFFFFFF;
#endif
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    CloseHandle((HANDLE)mutex);
#else
    return;
#endif
}

void HAL_MutexLock(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    WaitForSingleObject((HANDLE)mutex, INFINITE);
#else
    return;
#endif
}

int HAL_MutexTryLock(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    return WaitForSingleObject((HANDLE)mutex, 0) == WAIT_OBJECT_0 ? 0 : -1;
#else
    return 0;
#endif
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    ReleaseMutex((HANDLE)mutex);
#else
    return;
#endif
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return malloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
    if (ptr)
        free(ptr);
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
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

uint32_t HAL_GetTimeMs(void)
{
    return (uint32_t)GetTickCount64();
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    Sleep(ms);
}

#ifdef MULTITHREAD_ENABLED

// platform-dependant thread routine/entry function
static void _HAL_thread_func_wrapper_(void *ptr)
{
    ThreadParams *params = (ThreadParams *)ptr;

    params->thread_func(params->user_arg);

    _endthread();
}

// platform-dependant thread create function
int HAL_ThreadCreate(ThreadParams *params)
{
    if (params == NULL)
        return QCLOUD_ERR_INVAL;

    uintptr_t ret = _beginthread(_HAL_thread_func_wrapper_, params->stack_size, (void *)params);
    if (ret == -1L) {
        HAL_Printf("%s: _beginthread failed: %d\n", __FUNCTION__, errno);
        return QCLOUD_ERR_FAILURE;
    }

    return QCLOUD_RET_SUCCESS;
}

#endif

#ifdef AT_TCP_ENABLED

void *HAL_SemaphoreCreate(void)
{
    return NULL;
}

void HAL_SemaphoreDestroy(void *sem)
{
    return;
}

void HAL_SemaphorePost(void *sem)
{
    return;
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    return QCLOUD_RET_SUCCESS;
}

#endif
