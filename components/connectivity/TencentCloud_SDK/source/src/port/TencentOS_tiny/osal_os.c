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

#include "tos_k.h"
#include "qcloud.h"

__QCLOUD_OSAL__ void osal_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

__QCLOUD_OSAL__ int osal_snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

__QCLOUD_OSAL__ int osal_vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

__QCLOUD_OSAL__ void *osal_mutex_create(void)
{
    k_mutex_t *mutex;

    mutex = (k_mutex_t *)osal_malloc(sizeof(k_mutex_t));
    if (!mutex) {
        return K_NULL;
    }

    tos_mutex_create(mutex);

    return (void *)mutex;
}

__QCLOUD_OSAL__ void osal_mutex_destroy(void *mutex)
{
	k_err_t ret;

    if (K_ERR_NONE != (ret = tos_mutex_destroy((k_mutex_t *)mutex))) {
        osal_printf("osal_mutex_destroy err, err:%d\n\r", ret);
    } else {
        osal_free((void *)mutex);
    }
}

__QCLOUD_OSAL__ void osal_mutex_lock(void *mutex)
{
    k_err_t ret;

    if (K_ERR_NONE != (ret = tos_mutex_pend((k_mutex_t *)mutex))) {
        osal_printf("osal_mutex_lock err, err:%d\n\r", ret);
    }
}

__QCLOUD_OSAL__ int osal_mutex_trylock(void *mutex)
{
	k_err_t ret;

	if (K_ERR_NONE != (ret = tos_mutex_pend_timed((k_mutex_t *)mutex, 0))) {
		osal_printf("osal_mutex_lock err, err:%d\n\r", ret);
		return (int)ret;
	}

	return 0;
}

__QCLOUD_OSAL__ void osal_mutex_unlock(void *mutex)
{
	k_err_t ret;

	if (K_ERR_NONE != (ret = tos_mutex_post((k_mutex_t *)mutex))) {
		osal_printf("osal_mutex_unlock err, err:%d\n\r", ret);
	}
}

__QCLOUD_OSAL__ void *osal_malloc(uint32_t size)
{
	return  tos_mmheap_alloc(size);
}

__QCLOUD_OSAL__ void osal_free(void *ptr)
{
    tos_mmheap_free(ptr);
}

__QCLOUD_OSAL__ void osal_sleep_ms(uint32_t ms)
{
   (void)tos_sleep_hmsm(0,0,0, ms);
}

