/**
 * @file   TencentOS_thread.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_TENCENT_OS_THREAD_H_
#define _KERNEL_OS_TENCENT_OS_THREAD_H_

#include "./TencentOS_Adapter/TencentOS_common.h"
#include "./TencentOS_Adapter/TencentOS_time.h"
#include "tos_k.h"

#ifdef __cplusplus
extern "C" {
#endif

/* thread priority */
#define OS_THREAD_PRIO_SYS_CTRL OS_PRIORITY_ABOVE_NORMAL
#define OS_THREAD_PRIO_LWIP     OS_PRIORITY_NORMAL
#define OS_THREAD_PRIO_CONSOLE  OS_PRIORITY_ABOVE_NORMAL
#define OS_THREAD_PRIO_APP      OS_PRIORITY_NORMAL

typedef k_task_entry_t OS_ThreadEntry_t;
typedef k_task_t     * OS_ThreadHandle;

typedef struct OS_Thread {
    k_task_t * handle;
} OS_Thread_t;


OS_Status OS_ThreadCreateTimeslice(OS_Thread_t *thread, const char *name, OS_ThreadEntry_t entry, \
                           void *arg, OS_Priority priority, uint32_t stackSize, uint32_t timeslice);

static __always_inline  OS_Status OS_ThreadCreate(OS_Thread_t *thread, const char *name,
                                                  OS_ThreadEntry_t entry, void *arg,
                                                  OS_Priority priority, uint32_t stackSize)
{
    return OS_ThreadCreateTimeslice(thread,name,entry,arg,priority,stackSize,0u);
}

OS_Status OS_ThreadDelete(OS_Thread_t *thread);


static __always_inline int OS_ThreadIsValid(OS_Thread_t *thread)
{
	return ((thread->handle != NULL) && (thread->handle->state != K_TASK_STATE_DELETED));
}


static __always_inline void OS_ThreadYield(void)
{
	tos_task_yield();
}

static __always_inline void OS_ThreadSuspendScheduler(void)
{
    tos_knl_sched_lock();
}
static __always_inline void OS_ThreadResumeScheduler(void)
{
    tos_knl_sched_unlock();
}


//0 = kernel is not running. Non-0  = kernel is running.
static __always_inline int OS_ThreadIsSchedulerRunning(void)
{
	return tos_knl_is_running();
}


#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_TENCENT_OS_THREAD_H_ */
