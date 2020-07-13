/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-25 23:56:34
 * @LastEditTime: 2020-06-17 18:50:26
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _SALOF_DEFCONFIG_H_
#define _SALOF_DEFCONFIG_H_

#include "salof_config.h"

#ifdef SALOF_USING_LOG

#define         SALOF_USING_RTT             1
#define         SALOF_USING_FREERTOS        2
#define         SALOF_USING_TENCENTOS       3
#define         SALOF_USING_LINUX           4

#define         SALOF_BASE_LEVEL          (0)
#define         SALOF_ERR_LEVEL           (SALOF_BASE_LEVEL + 1)
#define         SALOF_WARN_LEVEL          (SALOF_ERR_LEVEL + 1)
#define         SALOF_INFO_LEVEL          (SALOF_WARN_LEVEL + 1)
#define         SALOF_DEBUG_LEVEL         (SALOF_INFO_LEVEL + 1)

#ifndef SALOF_USING_SALOF
    #define         SALOF_USING_SALOF                   (1U)
#endif

#ifndef SALOF_USING_IDLE_HOOK
    #define         SALOF_USING_IDLE_HOOK               (0U)
#endif

#ifndef SALOF_LOG_COLOR
    #define         SALOF_LOG_COLOR                     (1U)
#endif

#ifndef SALOF_LOG_TS
    #define         SALOF_LOG_TS                        (1U)
#endif

#ifndef SALOF_LOG_TAR
    #define         SALOF_LOG_TAR                       (0U)
#endif

#ifndef SALOF_LOG_LEVEL
#define         SALOF_LOG_LEVEL                         SALOF_DEBUG_LEVEL   //SALOF_WARN_LEVEL SALOF_DEBUG_LEVEL
#endif

#if SALOF_USING_SALOF

#ifndef SALOF_BUFF_SIZE
    #define         SALOF_BUFF_SIZE             (512U)
#endif

#ifndef SALOF_FIFO_SIZE
    #define         SALOF_FIFO_SIZE             (1024*4U)
#endif

#ifndef SALOF_TASK_STACK_SIZE
    #define         SALOF_TASK_STACK_SIZE       (2048U)
#endif

#ifndef SALOF_TASK_TICK
    #define         SALOF_TASK_TICK             (20U)
#endif

#endif

#if !defined(SALOF_OS)
    #error "SALOF_OS isn't defined in 'salof_config.h'"
#endif

#if (SALOF_OS == SALOF_USING_FREERTOS)
    #include "FreeRTOS.h"
    #include "task.h"
    #include "semphr.h"
    #define salof_mutex     SemaphoreHandle_t
    #define salof_tcb       TaskHandle_t
    #define salof_sem       salof_mutex
#if SALOF_USING_IDLE_HOOK
    #define salof_handler   vApplicationIdleHook
#endif  
    #define SALOF_TASK_PRIO (0U)

#elif (SALOF_OS == SALOF_USING_TENCENTOS)
    #include "tos_k.h"
    #define salof_mutex     k_mutex_t*
    #define salof_sem       k_sem_t*
    #define salof_tcb       k_task_t*
    #define SALOF_TASK_PRIO (TOS_CFG_TASK_PRIO_MAX - 2u)
    #undef  SALOF_USING_IDLE_HOOK
    
#elif (SALOF_OS == SALOF_USING_RTT)
    #include <rtconfig.h>
    #include <rtthread.h>
    #include <rthw.h>
    #include <stdio.h>
    #define salof_mutex     rt_mutex_t
    #define salof_sem       rt_sem_t
    #define salof_tcb       rt_thread_t
    #define SALOF_TASK_PRIO (RT_THREAD_PRIORITY_MAX - 1)
    
#elif (SALOF_OS == SALOF_USING_LINUX)
    #include "pthread.h"
    #include "memory.h"
    #include <semaphore.h>
    #include <stdlib.h>
    #include <stdio.h>
    #define salof_mutex     pthread_mutex_t*
    #define salof_sem       sem_t*
    #define salof_tcb       pthread_t*
    #define SALOF_TASK_PRIO (0U)
    #undef  SALOF_USING_IDLE_HOOK

#else
    #error "not supported OS type"
#endif

void *salof_alloc(unsigned int size);
void salof_free(void *mem);
salof_tcb salof_task_create(const char *name,
                            void (*task_entry)(void *param),
                            void * const param,
                            unsigned int stack_size,
                            unsigned int priority,
                            unsigned int tick);
salof_mutex salof_mutex_create(void);
void salof_mutex_delete(salof_mutex mutex);
int salof_mutex_pend(salof_mutex mutex, unsigned int timeout);
int salof_mutex_post(salof_mutex mutex);
salof_sem salof_sem_create(void);
void salof_sem_delete(salof_sem sem);
int salof_sem_pend(salof_sem sem, unsigned int timeout);
int salof_sem_post(salof_sem sem);
unsigned int salof_get_tick(void);
char *salof_get_task_name(void);
extern int send_buff(char *buf, int len);

#endif

#endif // !_SALOF_DEFCONFIG_H_

