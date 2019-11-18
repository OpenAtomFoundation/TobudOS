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
#include <memory.h>

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "qcloud_iot_import.h"
#include "qcloud_iot_export.h"


void *HAL_MutexCreate(void)
{
    int err_num;
    pthread_mutex_t *mutex = (pthread_mutex_t *)HAL_Malloc(sizeof(pthread_mutex_t));
    if (NULL == mutex) {
        return NULL;
    }

    if (0 != (err_num = pthread_mutex_init(mutex, NULL))) {
		HAL_Printf("%s: create mutex failed\n", __FUNCTION__);
        HAL_Free(mutex);
        return NULL;
    }

    return mutex;
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_destroy((pthread_mutex_t *)mutex))) {
		HAL_Printf("%s: destroy mutex failed\n", __FUNCTION__);
    }

    HAL_Free(mutex);
}

void HAL_MutexLock(_IN_ void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_lock((pthread_mutex_t *)mutex))) {     
		HAL_Printf("%s: lock mutex failed\n", __FUNCTION__);
    }
}

int HAL_MutexTryLock(_IN_ void *mutex)
{
    return pthread_mutex_trylock((pthread_mutex_t *)mutex);
    //return 0;
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_unlock((pthread_mutex_t *)mutex))) {       
		HAL_Printf("%s: unlock mutex failed\n", __FUNCTION__);
    }
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return malloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
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

uint32_t HAL_GetTimeMs(void)
{	
    struct timeval time_val = {0};
    uint32_t time_ms;

    gettimeofday(&time_val, NULL);
    time_ms = time_val.tv_sec * 1000 + time_val.tv_usec / 1000;

    return time_ms;
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    usleep(1000 * ms);
}

#ifdef AT_TCP_ENABLED

void HAL_DelayMs(_IN_ uint32_t ms)
{
   usleep(1000 * ms);
}

void * HAL_ThreadCreate(uint16_t stack_size, int priority, char * taskname,void *(*fn)(void*), void* arg)
{
	pthread_t *thread_t = (pthread_t *)HAL_Malloc(sizeof(unsigned long int));
	return pthread_create(thread_t, NULL, fn, arg) ? NULL  : (void*)thread_t;
}

int HAL_ThreadDestroy(void* threadId)
{
	int ret;

	if(0 == pthread_cancel(*((pthread_t*)threadId))){
		ret = QCLOUD_RET_SUCCESS;
	}else{
		ret = QCLOUD_ERR_FAILURE;
	}

	HAL_Free(threadId);	
	return ret;
}

void *HAL_SemaphoreCreate(void)
{
	sem_t *sem = (sem_t *)malloc(sizeof(sem_t));
	if (NULL == sem) {
		return NULL;
	}

	if (0 != sem_init(sem, 0, 0)) {
		free(sem);
		return NULL;
	}

	return sem;
}

void HAL_SemaphoreDestroy(void *sem)
{
	sem_destroy((sem_t *)sem);
	free(sem);
}

void HAL_SemaphorePost(void *sem)
{
	sem_post((sem_t *)sem);
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
#define PLATFORM_WAIT_INFINITE (~0)

	if (PLATFORM_WAIT_INFINITE == timeout_ms) {
		sem_wait(sem);
		return QCLOUD_RET_SUCCESS;
	}
	else {
		struct timespec ts;
		int s;
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
#undef 	PLATFORM_WAIT_INFINITE
}

#endif
