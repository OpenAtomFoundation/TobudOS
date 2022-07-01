/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2022 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file HAL_OS_tencentos_tiny.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-01-24
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-01-24 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "qcloud_iot_platform.h"
#include "tos_k.h"

/**
 * @brief Sleep for ms
 *
 * @param[in] ms ms to sleep
 */
void HAL_SleepMs(uint32_t ms)
{
    (void)tos_sleep_hmsm(0, 0, 0, ms);
}

/**
 * @brief Printf with format.
 *
 * @param[in] fmt format
 */
void HAL_Printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

/**
 * @brief Snprintf with format.
 *
 * @param[out] str buffer to save
 * @param[in] len buffer len
 * @param[in] fmt format
 * @return length of formatted string, >0 for success.
 */
int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

/**
 * @brief Malloc from heap.
 *
 * @param[in] size size to malloc
 * @return pointer to buffer, NULL for failed.
 */
void *HAL_Malloc(size_t size)
{
    return tos_mmheap_alloc(size);
}

/**
 * @brief Free buffer malloced by HAL_Malloc.
 *
 * @param[in] ptr
 */
void HAL_Free(void *ptr)
{
    tos_mmheap_free(ptr);
}

/**
 * @brief Mutex create.
 *
 * @return pointer to mutex
 */
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

/**
 * @brief Mutex destroy.
 *
 * @param[in,out] mutex pointer to mutex
 */
void HAL_MutexDestroy(void *mutex)
{
    k_err_t rc;

    if (!mutex) {
        return;
    }

    if (K_ERR_NONE != (rc = tos_mutex_destroy((k_mutex_t *)mutex))) {
        tos_kprintf("osal_mutex_destroy err, err:%d\n\r", rc);
    } else {
        HAL_Free((void *)mutex);
    }
}

/**
 * @brief Mutex lock.
 *
 * @param[in,out] mutex pointer to mutex
 */
void HAL_MutexLock(void *mutex)
{
    k_err_t rc;

    if (!mutex) {
        return;
    }

    if (K_ERR_NONE != (rc = tos_mutex_pend((k_mutex_t *)mutex))) {
        if (K_ERR_MUTEX_NESTING != rc && K_ERR_PEND_NOWAIT != rc) {
            k_task_t *current_task = tos_task_curr_task_get();
            tos_kprintf("%s osal_mutex_lock err, err:%d\n\r", current_task->name, rc);
        }
    }
}

/**
 * @brief Mutex try lock.
 *
 * @param[in,out] mutex pointer to mutex
 * @return 0 for success
 */
int HAL_MutexTryLock(void *mutex)
{
    k_err_t rc;
    if (!mutex) {
        return -1;
    }
    if (K_ERR_NONE != (rc = tos_mutex_pend_timed((k_mutex_t *)mutex, 0))) {
        if (K_ERR_MUTEX_NESTING != rc && K_ERR_PEND_NOWAIT != rc) {
            tos_kprintf("osal_mutex_lock err, err:%d\n\r", rc);
            return (int)rc;
        }
    }

    return 0;
}

/**
 * @brief Mutex unlock.
 *
 * @param[in,out] mutex pointer to mutex
 */
void HAL_MutexUnlock(void *mutex)
{
    k_err_t rc;
    if (!mutex) {
        return;
    }
    if (K_ERR_NONE != (rc = tos_mutex_post((k_mutex_t *)mutex))) {
        if (K_ERR_MUTEX_NESTING != rc && K_ERR_PEND_TIMEOUT != rc) {
            tos_kprintf("osal_mutex_unlock err, err:%d\n\r", rc);
        }
    }
}

/**
 * @brief platform-dependent thread create function
 *
 * @param[in,out] params params to create thread @see ThreadParams
 * @return @see IotReturnCode
 */
int HAL_ThreadCreate(ThreadParams *params)
{
    params->thread_id = HAL_Malloc(sizeof(k_task_t));
    if (!params->thread_id) {
        return QCLOUD_ERR_MALLOC;
    }
    k_prio_t priority = TOS_CFG_TASK_PRIO_MAX - 2;
    switch (params->priority) {
        case THREAD_PRIORITY_HIGH:
            priority = 2;
            break;
        case THREAD_PRIORITY_MIDDLE:
            priority = TOS_CFG_TASK_PRIO_MAX / 2;
            break;
        case THREAD_PRIORITY_LOW:
            priority = TOS_CFG_TASK_PRIO_MAX - 2;
            break;
    }
    return tos_task_create((k_task_t *)params->thread_id, params->thread_name, (k_task_entry_t)params->thread_func,
                           params->user_arg, priority, params->stack_base, params->stack_size, 20);
}

/**
 * @brief platform-dependent thread destroy function.
 *
 */
void HAL_ThreadDestroy(void *threadId)
{
    // no use in sdk
    tos_task_destroy((k_task_t *)threadId);
    HAL_Free(threadId);
}

/**
 * @brief platform-dependent semaphore create function.
 *
 * @return pointer to semaphore
 */
void *HAL_SemaphoreCreate(void)
{
    k_sem_t *sem = (k_sem_t *)malloc(sizeof(k_sem_t));
    if (!sem) {
        return NULL;
    }

    if (0 != tos_sem_create(sem, 0)) {
        free(sem);
        return NULL;
    }

    return sem;
}

/**
 * @brief platform-dependent semaphore destory function.
 *
 * @param[in] sem pointer to semaphore
 */
void HAL_SemaphoreDestroy(void *sem)
{
    if (!sem) {
        return;
    }
    tos_sem_destroy((k_sem_t *)sem);
    free(sem);
}

/**
 * @brief platform-dependent semaphore post function.
 *
 * @param[in] sem pointer to semaphore
 */
void HAL_SemaphorePost(void *sem)
{
    if (!sem) {
        return;
    }
    tos_sem_post((k_sem_t *)sem);
}

/**
 * @brief platform-dependent semaphore wait function.
 *
 * @param[in] sem pointer to semaphore
 * @param[in] timeout_ms wait timeout
 * @return @see IotReturnCode
 */
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    if (!sem) {
        return -1;
    }
    return tos_sem_pend((k_sem_t *)sem, timeout_ms);
}

/**
 * @brief platform-dependent mail queue init function.
 *
 * @param[in] pool pool using in mail queue
 * @param[in] mail_size mail size
 * @param[in] mail_count mail count
 * @return pointer to mail queue
 */
void *HAL_MailQueueInit(void *pool, size_t mail_size, int mail_count)
{
    k_mail_q_t *mail_q = HAL_Malloc(sizeof(k_mail_q_t));
    if (!mail_q) {
        return NULL;
    }

    int rc = tos_mail_q_create(mail_q, pool, mail_count, mail_size);
    if (rc) {
        HAL_Free(mail_q);
        return NULL;
    }
    return mail_q;
}

/**
 * @brief platform-dependent mail queue deinit function.
 *
 * @param[in] mail_q pointer to mail queue
 */
void HAL_MailQueueDeinit(void *mail_q)
{
    if (!mail_q) {
        return;
    }
    tos_mail_q_destroy((k_mail_q_t *)mail_q);
    HAL_Free(mail_q);
    return;
}

/**
 * @brief platform-dependent mail queue send function.
 *
 * @param[in] mail_q pointer to mail queue
 * @param[in] buf data buf
 * @param[in] size data size
 * @return 0 for success
 */
int HAL_MailQueueSend(void *mail_q, const void *buf, size_t size)
{
    if (!mail_q) {
        return -1;
    }
    return tos_mail_q_post((k_mail_q_t *)mail_q, (void *)buf, size);
}

/**
 * @brief platform-dependent mail queue send function.
 *
 * @param[in] mail_q pointer to mail queue
 * @param[out] buf data buf
 * @param[in] size data size
 * @param[in] timeout_ms
 * @return 0 for success
 */
int HAL_MailQueueRecv(void *mail_q, void *buf, size_t *size, uint32_t timeout_ms)
{
    if (!mail_q) {
        return -1;
    }
    return tos_mail_q_pend((k_mail_q_t *)mail_q, buf, size, timeout_ms);
}
