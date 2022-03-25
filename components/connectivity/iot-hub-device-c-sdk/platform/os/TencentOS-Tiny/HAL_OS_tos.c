/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
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
 * @file HAL_OS_linux.c
 * @brief Linux os api
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-31
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-31 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-08 <td>1.1     <td>fancyxu   <td>fix code standard of IotReturnCode
 * </table>
 */

#include "tos_k.h"


#include "qcloud_iot_platform.h"

/**
 * @brief Mutex create.
 *
 * @return pointer to mutex
 */
void *HAL_MutexCreate(void)
{
#ifdef MULTITHREAD_ENABLED
    k_err_t ret;
    
    k_mutex_t *mutex = (k_mutex_t *)HAL_Malloc(sizeof(k_mutex_t));
    if (!mutex) {
        return NULL;
    }
    
    ret = tos_mutex_create(mutex);
    if (ret != K_ERR_NONE) {
        HAL_Free(mutex);
        return NULL;
    }

    return mutex;
#else
    return (void *)0xFFFFFFFF;
#endif
}

/**
 * @brief Mutex destroy.
 *
 * @param[in,out] mutex pointer to mutex
 */
void HAL_MutexDestroy(void *mutex)
{
    if (!mutex) {
        return;
    }
#ifdef MULTITHREAD_ENABLED
    k_err_t err = tos_mutex_destroy((k_mutex_t *)mutex);
    if (err != K_ERR_NONE) {
    }

    HAL_Free(mutex);
#else
    return;
#endif
}

/**
 * @brief Mutex lock.
 *
 * @param[in,out] mutex pointer to mutex
 */
void HAL_MutexLock(void *mutex)
{
    if (!mutex) {
        return;
    }
#ifdef MULTITHREAD_ENABLED
    k_err_t err = tos_mutex_pend((k_mutex_t *)mutex);
    if (err != K_ERR_NONE && err != K_ERR_MUTEX_NESTING) {
        return;
    }
#else
    return;
#endif
}

/**
 * @brief Mutex try lock.
 *
 * @param[in,out] mutex pointer to mutex
 * @return 0 for success
 */
int HAL_MutexTryLock(void *mutex)
{
    if (!mutex) {
        return -1;
    }
#ifdef MULTITHREAD_ENABLED
     k_err_t err = tos_mutex_pend_timed((k_mutex_t *)mutex, 0);
    if (err != K_ERR_NONE && err != K_ERR_MUTEX_NESTING) {
        return -1;
    }
    return 0;
#else
    return 0;
#endif
}

/**
 * @brief Mutex unlock.
 *
 * @param[in,out] mutex pointer to mutex
 */
void HAL_MutexUnlock(void *mutex)
{
    if (!mutex) {
        return;
    }
#ifdef MULTITHREAD_ENABLED
    k_err_t err = tos_mutex_post((k_mutex_t *)mutex);
    if (err != K_ERR_NONE && err != K_ERR_MUTEX_NESTING) {
        return;
    }
#else
    return;
#endif
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
    if (ptr)
        tos_mmheap_free(ptr);
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
 * @brief Sleep for ms
 *
 * @param[in] ms ms to sleep
 */
void HAL_SleepMs(uint32_t ms)
{
    tos_sleep_ms(ms);
}

#ifdef MULTITHREAD_ENABLED

/**
 * @brief platform-dependent thread create function
 *
 * @param[in,out] params params to create thread @see ThreadParams
 * @return @see IotReturnCode
 */
int HAL_ThreadCreate(ThreadParams *params)
{
    if (params == NULL)
        return QCLOUD_ERR_INVAL;
    
    k_err_t err;
    
    params->thread_id = (k_task_t *)HAL_Malloc(sizeof(k_task_t *));
    if (params->thread_id == NULL) {
        return QCLOUD_ERR_FAILURE;
    }
    
    err = tos_task_create((k_task_t *)&params->thread_id, params->thread_name, params->thread_func, (void *)params->user_arg, 
                            params->priority, (k_stack_t *)params->stack_base, (size_t)params->stack_size, 0);
    if (err != K_ERR_NONE) {
        return QCLOUD_ERR_FAILURE;
    }

    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief platform-dependent thread destroy function.
 *
 */
void HAL_ThreadDestroy(void *thread_id) {
    
    tos_task_destroy((k_task_t *)thread_id);
    
    HAL_Free(thread_id);
}

/**
 * @brief platform-dependent semaphore create function.
 *
 * @return pointer to semaphore
 */
void *HAL_SemaphoreCreate(void)
{
    k_sem_t *sem = (k_sem_t *)HAL_Malloc(sizeof(k_sem_t));
    if (!sem) {
        return NULL;
    }

    if (tos_sem_create(sem, 0)) {
        HAL_Free(sem);
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
    tos_sem_destroy((k_sem_t *)sem);
    HAL_Free(sem);
}

/**
 * @brief platform-dependent semaphore post function.
 *
 * @param[in] sem pointer to semaphore
 */
void HAL_SemaphorePost(void *sem)
{
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
    k_err_t err;

    err = tos_sem_pend(sem, timeout_ms);
    
    return err == K_ERR_NONE ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

/**
 * @brief Mail buffer in tos.
 *
 */
typedef struct {
    long int type;
    uint8_t  data[2048];
} MailBuffer;

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
    k_err_t err;
    k_mail_q_t *mail_q;
    
    mail_q = HAL_Malloc(sizeof(k_mail_q_t));
    if (!mail_q) {
        return NULL;
    }

    err = tos_mail_q_create(mail_q, pool, mail_count, mail_size);
    if (err != K_ERR_NONE) {
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
int HAL_MailQueueSend(void *mail_q, void *buf, size_t size)
{
    k_err_t err;

    err = tos_mail_q_post(mail_q, buf, size);
    
    return err == K_ERR_NONE ? 0 : -1;
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
int HAL_MailQueueRecv(void *mail_q, void *buf, size_t *size, int timeout_ms)
{
    k_err_t err;

    err = tos_mail_q_pend((k_mail_q_t *)mail_q, buf, size, timeout_ms);

    return err == K_ERR_NONE ? 0 : -1;
}

#endif
