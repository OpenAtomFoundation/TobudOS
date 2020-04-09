/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "lwip/opt.h"

#if (NO_SYS == 0)
#include "osal/osal.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* mutex */
#define LWIP_COMPAT_MUTEX           0//Our OS supports mutex semaphore,so there is no need to enable compatible mode.
typedef OS_Mutex_t sys_mutex_t;
#define sys_mutex_valid(x)          OS_MutexIsValid(x)
#define sys_mutex_set_invalid(x)    OS_MutexSetInvalid(x)

/* semaphore */
typedef OS_Semaphore_t sys_sem_t;
#define sys_sem_valid(x)            OS_SemaphoreIsValid(x)
#define sys_sem_set_invalid(x)      OS_SemaphoreSetInvalid(x)

/* mail box */
#if LWIP_MBOX_TRACE
  typedef struct {
    OS_MsgQueue_t q;
    u8_t avail;
    u8_t used;
    u8_t max;
    u32_t err;
  } sys_mbox_t;
  #define sys_mbox_valid(x)           OS_MsgQueueIsValid(&((x)->q))
  #define sys_mbox_set_invalid(x)     OS_MsgQueueSetInvalid(&((x)->q))
#else /* LWIP_MBOX_TRACE */
	typedef OS_MsgQueue_t sys_mbox_t;
	#define sys_mbox_valid(x)           OS_MsgQueueIsValid(x)
	#define sys_mbox_set_invalid(x)     OS_MsgQueueSetInvalid(x)
#endif /* LWIP_MBOX_TRACE */

/* thread */
typedef OS_Thread_t * sys_thread_t;

/* sleep */
#define sys_msleep(msec)            OS_MsDelay(msec)

/* Ticks/jiffies since power up. */
#define sys_jiffies()               OS_GetTicks()

#define SYS_ARCH_PROTECT_USE_MUTEX  0 /* system arch protection using mutex */

/* protection */
#if SYS_LIGHTWEIGHT_PROT
  #if SYS_ARCH_PROTECT_USE_MUTEX
    extern OS_Mutex_t g_lwip_sys_mutex;
    #define SYS_ARCH_DECL_PROTECT(lev)
    #define SYS_ARCH_PROTECT(lev)       OS_RecursiveMutexLock(&g_lwip_sys_mutex, OS_WAIT_FOREVER)
    #define SYS_ARCH_UNPROTECT(lev)     OS_RecursiveMutexUnlock(&g_lwip_sys_mutex)
  #else /* SYS_ARCH_PROTECT_USE_MUTEX */
    #define SYS_ARCH_DECL_PROTECT(lev)
    #define SYS_ARCH_PROTECT(lev)       OS_ThreadSuspendScheduler()
    #define SYS_ARCH_UNPROTECT(lev)     OS_ThreadResumeScheduler()
    #endif /* SYS_ARCH_PROTECT_USE_MUTEX */
#else /* SYS_LIGHTWEIGHT_PROT */
  #define SYS_ARCH_DECL_PROTECT(lev)
  #define SYS_ARCH_PROTECT(lev)
  #define SYS_ARCH_UNPROTECT(lev)
#endif /* SYS_LIGHTWEIGHT_PROT */

#ifdef  __cplusplus
}
#endif

#endif /* (NO_SYS == 0) */
#endif /* __ARCH_SYS_ARCH_H__ */
