/*
 * Copyright (c) 2017 Simon Goldschmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt
 *
 */

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

#if !NO_SYS

#if defined(LWIP_PROVIDE_ERRNO)
int errno;
#endif

u32_t sys_jiffies(void)
{
    return tos_systick_get();
}

u32_t sys_now(void)
{
    return tos_systick_get();
}

void sys_init(void)
{
}

sys_prot_t sys_arch_protect(void)
{
    TOS_CPU_CPSR_ALLOC();
    TOS_CPU_INT_DISABLE();
    return cpu_cpsr;
}

void sys_arch_unprotect(sys_prot_t cpu_cpsr)
{
    TOS_CPU_INT_ENABLE();
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    k_err_t rc;

    rc = tos_sem_create(sem, count);

#if SYS_STATS
    ++lwip_stats.sys.sem.used;
    if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used) {
        lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
    }
#endif

    if (rc == K_ERR_NONE) {
        return ERR_OK;
    }

#if SYS_STATS
    ++lwip_stats.sys.sem.err;
#endif

    printf("[sys_arch]:new sem fail!\n");
    return ERR_MEM;
}

void sys_sem_free(sys_sem_t *sem)
{
#if SYS_STATS
    --lwip_stats.sys.sem.used;
#endif

    tos_sem_destroy(sem);
}

int sys_sem_valid(sys_sem_t *sem)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    return sem->knl_obj.type == KNL_OBJ_TYPE_SEMAPHORE;
#else
    #error  "need TOS_CFG_OBJECT_VERIFY_EN > 0u"
#endif
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    sem->knl_obj.type = KNL_OBJ_TYPE_NONE;
#else
    #error  "need TOS_CFG_OBJECT_VERIFY_EN > 0u"
#endif
}

/*
   如果timeout参数不为零，则返回值为
   等待信号量所花费的毫秒数。如果
   信号量未在指定时间内发出信号，返回值为
   SYS_ARCH_TIMEOUT。如果线程不必等待信号量
   该函数返回零。 */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    k_tick_t wait_tick = 0;
    k_tick_t start_tick = 0;

    //看看信号量是否有效
    if (sem == SYS_SEM_NULL) {
        return SYS_ARCH_TIMEOUT;
    }

    //首先获取开始等待信号量的时钟节拍
    start_tick = sys_now();

    //timeout != 0，需要将ms换成系统的时钟节拍
    if (timeout != 0) {
        //将ms转换成时钟节拍
        wait_tick = timeout / (1000 / TOS_CFG_CPU_TICK_PER_SECOND);
        if (wait_tick == 0) {
            wait_tick = 1;
        }
    } else {
        wait_tick = TOS_TIME_FOREVER;  //一直阻塞
    }

    //等待成功，计算等待的时间，否则就表示等待超时
    if (tos_sem_pend(sem, wait_tick) == K_ERR_NONE) {
        return ((sys_now() - start_tick) * (1000 / TOS_CFG_CPU_TICK_PER_SECOND));
    }

    return SYS_ARCH_TIMEOUT;
}

void sys_sem_signal(sys_sem_t *sem)
{
    if (tos_sem_post(sem) != K_ERR_NONE) {
        printf("[sys_arch]:sem signal fail!\n");
    }
}

err_t sys_mutex_new(sys_mutex_t *mutex)
{
    k_err_t rc;

    rc = tos_mutex_create(mutex);
    if (rc == K_ERR_NONE) {
        return ERR_OK;
    }

    printf("[sys_arch]:new mutex fail!\n");
    return ERR_MEM;
}

void sys_mutex_free(sys_mutex_t *mutex)
{
    tos_mutex_destroy(mutex);
}

int sys_mutex_valid(sys_mutex_t *mutex)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    return mutex->knl_obj.type == KNL_OBJ_TYPE_MUTEX;
#else
    #error  "need TOS_CFG_OBJECT_VERIFY_EN > 0u"
#endif
}

void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    mutex->knl_obj.type = KNL_OBJ_TYPE_NONE;
#else
    #error  "need TOS_CFG_OBJECT_VERIFY_EN > 0u"
#endif
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    tos_mutex_pend(mutex);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    tos_mutex_post(mutex);
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
    k_err_t rc;
    sys_thread_t task;
    k_stack_t *task_stack;

    task = tos_mmheap_alloc(sizeof(k_task_t));
    if (!task) {
        printf("[sys_arch]:memalloc k_task_t fail!\n");
        return NULL;
    }

    task_stack = tos_mmheap_alloc(stacksize);
    if (!task_stack) {
        printf("[sys_arch]:memalloc task stack fail!\n");
        return NULL;
    }

    /* 创建MidPriority_Task任务 */
    rc = tos_task_create(task, (char*)name, function, arg,
                            prio, task_stack, stacksize, 20);
    if (rc != K_ERR_NONE) {
        printf("[sys_arch]:create task fail! code: %d \r\n", rc);
        return NULL;
    }

    return task;
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    k_err_t rc;
    void *msg_q_pool;

#define MBOX_MSG_MAX        32
    msg_q_pool = tos_mmheap_alloc(sizeof(void *) * MBOX_MSG_MAX);
    if (!msg_q_pool) {
        return ERR_MEM;
    }

    rc = tos_msg_q_create(mbox, msg_q_pool, MBOX_MSG_MAX);
    if (rc != K_ERR_NONE) {
        printf("Create mbox fail! code : %d \r\n", rc);
        return ERR_MEM;
    }

    return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    tos_mmheap_free(mbox->ring_q.pool);
    tos_msg_q_destroy(mbox);
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    return mbox->knl_obj.type == KNL_OBJ_TYPE_MESSAGE_QUEUE;
#else
    #error  "need TOS_CFG_OBJECT_VERIFY_EN > 0u"
#endif
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    mbox->knl_obj.type = KNL_OBJ_TYPE_NONE;
#else
    #error  "need TOS_CFG_OBJECT_VERIFY_EN > 0u"
#endif
}

void sys_mbox_post(sys_mbox_t *q, void *msg)
{
    tos_msg_q_post(q, msg);
}

err_t sys_mbox_trypost(sys_mbox_t *q, void *msg)
{
    if (tos_msg_q_post(q, msg) == K_ERR_NONE) {
        return ERR_OK;
    }

    return ERR_MEM;
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *q, void *msg)
{
    return sys_mbox_trypost(q, msg);
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *q, void **msg, u32_t timeout)
{
    void *dummyptr;
    k_tick_t wait_tick = 0;
    k_tick_t start_tick = 0;

    if (!msg) { // 看看存储消息的地方是否有效
        msg = &dummyptr;
    }

    // 首先获取开始等待信号量的时钟节拍
    start_tick = sys_now();

    // timeout != 0，需要将ms换成系统的时钟节拍
    if (timeout != 0) {
        //将ms转换成时钟节拍
        wait_tick = timeout / (1000 / TOS_CFG_CPU_TICK_PER_SECOND);
        if (wait_tick == 0) {
            wait_tick = 1;
        }
    } else { // 一直阻塞
        wait_tick = TOS_TIME_FOREVER;
    }

    // 等待成功，计算等待的时间，否则就表示等待超时
    if (tos_msg_q_pend(q,&(*msg), wait_tick) == K_ERR_NONE) {
        return ((sys_now() - start_tick) * (1000 / TOS_CFG_CPU_TICK_PER_SECOND));
    }

    *msg = NULL;
    return SYS_ARCH_TIMEOUT;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *q, void **msg)
{

    void *dummyptr;

    if (!msg) {
        msg = &dummyptr;
    }

    if (tos_msg_q_pend(q, &(*msg), 0) == K_ERR_NONE) {
        return ERR_OK;
    }

    return SYS_MBOX_EMPTY;
}

#endif /* !NO_SYS */

