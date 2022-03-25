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

#include <errno.h>
#include <memory.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>

#include "qcloud_iot_platform.h"

/**
 * @brief Mutex create.
 *
 * @return pointer to mutex
 */
void *HAL_MutexCreate(void)
{
#ifdef MULTITHREAD_ENABLED
    int err_num;

    /**
     * @brief
     *
     * @ref https://manpages.debian.org/jessie/glibc-doc/pthread_mutex_lock.3.en.html
     *
     * If the mutex is already locked by the calling thread, the behavior of pthread_mutex_lock depends on the
     * kind of the mutex. If the mutex is of the ``fast'' kind, the calling thread is suspended until the mutex is
     * unlocked, thus effectively causing the calling thread to deadlock. If the mutex is of the ``error checking''
     * kind, pthread_mutex_lock returns immediately with the error code EDEADLK. If the mutex is of the ``recursive''
     * kind, pthread_mutex_lock succeeds and returns immediately, recording the number of times the calling thread has
     * locked the mutex. An equal number of pthread_mutex_unlock operations must be performed before the mutex returns
     * to the unlocked state.
     *
     */
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);

    pthread_mutex_t *mutex = (pthread_mutex_t *)HAL_Malloc(sizeof(pthread_mutex_t));
    if (!mutex) {
        return NULL;
    }

    err_num = pthread_mutex_init(mutex, &attr);

    pthread_mutexattr_destroy(&attr);

    if (err_num) {
        HAL_Printf("%s: create mutex failed\n", __FUNCTION__);
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
    int err_num = pthread_mutex_destroy((pthread_mutex_t *)mutex);
    if (err_num) {
        HAL_Printf("%s: destroy mutex failed\n", __FUNCTION__);
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
    int err_num = pthread_mutex_lock((pthread_mutex_t *)mutex);
    if (err_num) {
        HAL_Printf("%s: lock mutex failed\n", __FUNCTION__);
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
    return pthread_mutex_trylock((pthread_mutex_t *)mutex);
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
    int err_num;
    if (0 != (err_num = pthread_mutex_unlock((pthread_mutex_t *)mutex))) {
        HAL_Printf("%s: unlock mutex failed\n", __FUNCTION__);
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
    return malloc(size);
}

/**
 * @brief Free buffer malloced by HAL_Malloc.
 *
 * @param[in] ptr
 */
void HAL_Free(void *ptr)
{
    if (ptr)
        free(ptr);
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
    usleep(1000 * ms);
}

#ifdef MULTITHREAD_ENABLED

/**
 * @brief platform-dependent thread routine/entry function
 *
 * @param[in,out] ptr
 * @return NULL
 */
static void *_HAL_thread_func_wrapper_(void *ptr)
{
    ThreadParams *params = (ThreadParams *)ptr;

    params->thread_func(params->user_arg);

    pthread_detach(pthread_self());
    pthread_exit(0);
    return NULL;
}

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

    int rc = pthread_create((pthread_t *)&params->thread_id, NULL, _HAL_thread_func_wrapper_, (void *)params);
    if (rc) {
        HAL_Printf("%s: pthread_create failed: %d\n", __FUNCTION__, rc);
        return QCLOUD_ERR_FAILURE;
    }

    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief platform-dependent thread destroy function.
 *
 */
void HAL_ThreadDestroy(void *thread_id) {}

/**
 * @brief platform-dependent semaphore create function.
 *
 * @return pointer to semaphore
 */
void *HAL_SemaphoreCreate(void)
{
    sem_t *sem = (sem_t *)malloc(sizeof(sem_t));
    if (!sem) {
        return NULL;
    }

    if (sem_init(sem, 0, 0)) {
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
    sem_destroy((sem_t *)sem);
    free(sem);
}

/**
 * @brief platform-dependent semaphore post function.
 *
 * @param[in] sem pointer to semaphore
 */
void HAL_SemaphorePost(void *sem)
{
    sem_post((sem_t *)sem);
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
#define PLATFORM_WAIT_INFINITE (~0)

    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        sem_wait(sem);
        return QCLOUD_RET_SUCCESS;
    } else {
        struct timespec ts;
        int             s;
        /* Restart if interrupted by handler */
        do {
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                return QCLOUD_ERR_FAILURE;
            }

            s = 0;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_nsec -= 1000000000;
                s = 1;
            }

            ts.tv_sec += timeout_ms / 1000 + s;

        } while (((s = sem_timedwait(sem, &ts)) != 0) && errno == EINTR);

        return s ? QCLOUD_ERR_FAILURE : QCLOUD_RET_SUCCESS;
    }
#undef PLATFORM_WAIT_INFINITE
}

/**
 * @brief Mail queue handle in linux.
 *
 */
typedef struct {
    int    msg_id;
    size_t msg_size;
} MailQueueHandle;

/**
 * @brief Mail buffer in linux.
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
    static key_t sg_mail_key = 1234;

    MailQueueHandle *handle = HAL_Malloc(sizeof(MailQueueHandle));
    if (!handle) {
        return NULL;
    }

    handle->msg_id = msgget(sg_mail_key++, 0666 | IPC_CREAT);
    if (handle->msg_id == -1) {
        HAL_Free(handle);
        return NULL;
    }

    handle->msg_size = mail_size;
    return handle;
}

/**
 * @brief platform-dependent mail queue deinit function.
 *
 * @param[in] mail_q pointer to mail queue
 */
void HAL_MailQueueDeinit(void *mail_q)
{
    MailQueueHandle *handle = (MailQueueHandle *)mail_q;
    msgctl(handle->msg_id, IPC_RMID, 0);
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
    MailQueueHandle *handle = (MailQueueHandle *)mail_q;
    MailBuffer       data;
    memset(&data, 0, sizeof(MailBuffer));
    data.type = 1;
    memcpy(data.data, buf, size);
    return msgsnd(handle->msg_id, &data, size, 0);
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
    MailQueueHandle *handle = (MailQueueHandle *)mail_q;
    MailBuffer       data;
    memset(&data, 0, sizeof(MailBuffer));
    data.type = 1;
    *size     = handle->msg_size;
    int rc    = msgrcv(handle->msg_id, &data, handle->msg_size, 0, 0);
    memcpy(buf, data.data, handle->msg_size);
    return rc < 0;
}

#endif
