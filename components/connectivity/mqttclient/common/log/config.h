/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-25 23:56:34
 * @LastEditTime : 2020-01-18 13:50:21
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _SALOF_CONFIG_H_
#define _SALOF_CONFIG_H_

#define         USE_RTT             0
#define         USE_UCOSIII         1
#define         USE_FREERTOS        2
#define         USE_TENCENTOS       3
#define         USE_LINUX           4

#define         USE_LOG                     (1U)
#define         USE_SALOF                   (1U)
#define         SALOF_OS                    USE_TENCENTOS
#define         USE_IDLE_HOOK               (0U)

#define         LOG_LEVEL                   DEBUG_LEVEL   //WARN_LEVEL DEBUG_LEVEL
#define         LOG_COLOR                   (0U)
#define         LOG_TS                      (0U)
#define         LOG_TAR                     (0U)

#if USE_SALOF
#define         SALOF_BUFF_SIZE             (512U)
#define         SALOF_FIFO_SIZE             (1024*2U)
#define         SALOF_TASK_STACK_SIZE       (1024U)
#define         SALOF_TASK_TICK             (20U)
#endif

#if !defined(SALOF_OS)
    #error "SALOF_OS isn't defined in 'cmb_cfg.h'"
#endif

#if (SALOF_OS == USE_FREERTOS)
    #include "FreeRTOS.h"
    #include "task.h"
    #include "semphr.h"
    #define salof_mutex     SemaphoreHandle_t
    #define salof_tcb       TaskHandle_t
    #define salof_sem       salof_mutex
#if USE_IDLE_HOOK
    #define salof_handler   vApplicationIdleHook
#endif  
    #define SALOF_TASK_PRIO (0U)

#elif (SALOF_OS == USE_TENCENTOS)
    #include "tos_k.h"
    #define salof_mutex     k_mutex_t*
    #define salof_sem       k_sem_t*
    #define salof_tcb       k_task_t*
    #define SALOF_TASK_PRIO (TOS_CFG_TASK_PRIO_MAX - 2u)
    #undef  USE_IDLE_HOOK

#elif (SALOF_OS == USE_LINUX)
    #include "pthread.h"
    #include "memory.h"
    #include <semaphore.h>
    #include <stdio.h>
    #define salof_mutex     pthread_mutex_t*
    #define salof_sem       sem_t*
    #define salof_tcb       pthread_t*
    #define SALOF_TASK_PRIO (0U)
    #undef  USE_IDLE_HOOK
    
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
#endif // !_SALOF_CONFIG_H_

