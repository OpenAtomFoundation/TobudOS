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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "qcloud_iot_import.h"
#include "stm32l4xx_hal.h"

	
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

void HAL_SleepMs(_IN_ uint32_t ms)
{
   (void)HAL_Delay(ms);
}

void HAL_DelayMs(_IN_ uint32_t ms)
{
   (void)HAL_Delay(ms);
}

void HAL_Free(void *ptr)
{
    free(ptr);
}

void *HAL_Malloc(uint32_t size)
{
    return malloc(size);
}

void *HAL_MutexCreate(void)
{
    return (void *)1;
}

void HAL_MutexDestroy(void *mutex)
{
    return;
}

void HAL_MutexLock(void *mutex)
{
    return;
}

void HAL_MutexUnlock(void *mutex)
{
    return;
}

