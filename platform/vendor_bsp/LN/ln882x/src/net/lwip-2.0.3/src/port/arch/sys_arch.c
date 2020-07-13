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
#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#include "arch/sys_arch.h"

#if LWIP_RESOURCE_TRACE
  int g_lwip_mutex_cnt = 0;
  int g_lwip_sem_cnt = 0;
  int g_lwip_mbox_cnt = 0;
#endif

#if (NO_SYS == 0)

#if defined(LWIP_PROVIDE_ERRNO)
  int errno;
#endif

/*
 * *---------------- Mutexes -----------------*
 * |err_t sys_mutex_new(sys_mutex_t *mutex)   |
 * |void  sys_mutex_free(sys_mutex_t *mutex)  |
 * |void  sys_mutex_lock(sys_mutex_t *mutex)  |
 * |void  sys_mutex_unlock(sys_mutex_t *mutex)|
 * *------------------------------------------*
 */
#if !LWIP_COMPAT_MUTEX
/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
    #if LWIP_RESOURCE_TRACE
    g_lwip_mutex_cnt++;
    #endif
    err_t err = OS_MutexCreate(mutex);
    if (err == OS_OK) {
        SYS_STATS_INC_USED(mutex);
        return ERR_OK;
    } else {
        SYS_STATS_INC(mutex.err);
        return ERR_MEM;
    }
}

/** Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock(sys_mutex_t *mutex)
{
    OS_MutexLock(mutex, OS_WAIT_FOREVER);
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
    OS_MutexUnlock(mutex);
}

/** Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free(sys_mutex_t *mutex)
{
    if (OS_MutexDelete(mutex) == OS_OK) {
        SYS_STATS_DEC(mutex.used);
    } else {
        LWIP_ERROR("sys_mutex_free: failed", 0, while(1));
        SYS_STATS_INC(mutex.err);
    }
    
    #if LWIP_RESOURCE_TRACE
    g_lwip_mutex_cnt--;
    #endif
}
#endif /* LWIP_COMPAT_MUTEX */

/*
 * *------------------------- Semaphore --------------------------------*
 * |err_t sys_sem_new(sys_sem_t *sem, u8_t count)                       |
 * |u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)              |
 * |void  sys_sem_signal(sys_sem_t *sem)                                |
 * |void  sys_sem_free(sys_sem_t *sem)                                  |
 * |int   sys_sem_valid(sys_sem_t *sem)         --->(In sys_arch.h file)|
 * |void  sys_sem_set_invalid(sys_mbox_t *mbox) --->(In sys_arch.h file)|
 * *--------------------------------------------------------------------*
 */
/** Create a new semaphore
 * @param sem pointer to the semaphore to create
 * @param count initial count of the semaphore
 * @return ERR_OK if successful, another err_t otherwise */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    #if LWIP_RESOURCE_TRACE
    g_lwip_sem_cnt++;
    #endif
    err_t err = OS_SemaphoreCreate(sem, count, OS_SEMAPHORE_MAX_COUNT);
    if (err == OS_OK) {
        SYS_STATS_INC_USED(sem);
        return ERR_OK;
    } else {
        SYS_STATS_INC(sem.err);
        return ERR_MEM;
    }
}

/** Signals a semaphore
 * @param sem the semaphore to signal */
void sys_sem_signal(sys_sem_t *sem)
{
    OS_SemaphoreRelease(sem);
}

/** Wait for a semaphore for the specified timeout
 * @param sem the semaphore to wait for
 * @param timeout timeout in milliseconds to wait (0 = wait forever)
 * @return time (in milliseconds) waited for the semaphore
 *         or SYS_ARCH_TIMEOUT on timeout */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    u32_t start_ticks = OS_GetTicks();
    if (OS_SemaphoreWait(sem, timeout == 0 ? OS_WAIT_FOREVER : timeout) != OS_OK)
        return SYS_ARCH_TIMEOUT;

    return OS_TicksToMSecs(OS_GetTicks() - start_ticks);
}

/** Delete a semaphore
 * @param sem semaphore to delete */
void sys_sem_free(sys_sem_t *sem)
{
    if (OS_SemaphoreDelete(sem) == OS_OK) {
        SYS_STATS_DEC(sem.used);
    } else {
        LWIP_ERROR("sys_sem_free: failed", 0, while(1));
        SYS_STATS_INC(sem.err);
    }
    
    #if LWIP_RESOURCE_TRACE
    g_lwip_sem_cnt--;
    #endif
}


#if LWIP_MBOX_TRACE
  #define MBOX2Q(m)		(&((m)->q))
  #define MBOX_INC_USED(m)                                         \
    do {                                                           \
        if (++(m)->used > (m)->max) {                              \
            (m)->max = (m)->used;                                  \
            LWIP_PLATFORM_DIAG(("mbox %p, avail %u, max %u\n",(m), \
                                 (m)->avail, (m)->max));           \
        }                                                          \
    } while (0)
  #define MBOX_DEC_USED(m)	do { --(m)->used; } while (0)
  #define MBOX_INC_ERR(m)		do { ++(m)->err; } while (0)
#else /* LWIP_MBOX_TRACE */
  #define MBOX2Q(m)		(m)
  #define MBOX_INC_USED(m)	do { } while (0)
  #define MBOX_DEC_USED(m)	do { } while (0)
  #define MBOX_INC_ERR(m)		do { } while (0)
#endif /* LWIP_MBOX_TRACE */

/*
 * *-------------------------------- Mailbox ------------------------------*
 * |err_t sys_mbox_new(sys_mbox_t *mbox, int size)                         |
 * |void sys_mbox_free(sys_mbox_t *mbox)                                   |
 * |void sys_mbox_post(sys_mbox_t *mbox, void *msg)                        |
 * |err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)                    |
 * |u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout) |
 * |u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)             |
 * |int   sys_mbox_valid(sys_mbox_t *mbox)        --->(In sys_arch.h file) |
 * |void  sys_mbox_set_invalid(sys_mbox_t *mbox)  --->(In sys_arch.h file) |
 * *-----------------------------------------------------------------------*
 */
/** Create a new mbox of specified size
 * @param mbox pointer to the mbox to create
 * @param size (miminum) number of messages in this mbox
 * @return ERR_OK if successful, another err_t otherwise */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    #if LWIP_RESOURCE_TRACE
    g_lwip_mbox_cnt++;
    #endif
  
    err_t err = OS_MsgQueueCreate(MBOX2Q(mbox), size);
    if (err == OS_OK) {
        SYS_STATS_INC_USED(mbox);
          
        #if LWIP_MBOX_TRACE
        mbox->avail = size;
        mbox->used = 0;
        mbox->max = 0;
        mbox->err = 0;
        #endif
          
        return ERR_OK;
    } else {
        SYS_STATS_INC(mbox.err);
        return ERR_MEM;
    }
}

/** Post a message to an mbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL) */
#if LWIP_MBOX_DEBUG
  sys_mbox_t *g_trace_mbox = NULL;
  void _sys_mbox_post(sys_mbox_t *mbox, void *msg, const char *func, int line)
  {
      if (mbox == g_trace_mbox) {
          printf("%s(), mbox %p, msg %p, @ %s():%d\n", __func__, mbox, msg, func, line);
      }
      OS_MsgQueueSend(MBOX2Q(mbox), msg, OS_WAIT_FOREVER);
      MBOX_INC_USED(mbox);
  }
#else
  void sys_mbox_post(sys_mbox_t *mbox, void *msg)
  {
      OS_MsgQueueSend(MBOX2Q(mbox), msg, OS_WAIT_FOREVER);
      MBOX_INC_USED(mbox);
  }
#endif

/** Try to post a message to an mbox - may fail if full or ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL) */
#if LWIP_MBOX_DEBUG
  err_t _sys_mbox_trypost(sys_mbox_t *mbox, void *msg, const char *func, int line)
  {
      if (mbox == g_trace_mbox) {
          printf("%s(), mbox %p, msg %p, @ %s():%d\n", __func__, mbox, msg, func, line);
      }
      err_t err = OS_MsgQueueSend(MBOX2Q(mbox), msg, 0);
      if (err == OS_OK) {
          MBOX_INC_USED(mbox);
          return ERR_OK;
      } else {
          MBOX_INC_ERR(mbox);
          return ERR_TIMEOUT;
      }
  }
#else
  err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
  {
      err_t err = OS_MsgQueueSend(MBOX2Q(mbox), msg, 0);
      if (err == OS_OK) {
          MBOX_INC_USED(mbox);
          return ERR_OK;
      } else {
          MBOX_INC_ERR(mbox);
          return ERR_TIMEOUT;
      }
  }
#endif

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
           or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter! */
#if LWIP_MBOX_DEBUG
  u32_t _sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout, const char *func, int line)
  {
      u32_t start_ticks = OS_GetTicks();

      if (mbox == g_trace_mbox) {
          printf("%s(), mbox %p, timeout 0x%x, @ %s():%d\n", __func__, mbox, timeout, func, line);
      }

      if (timeout == 0) {
          u32_t wait = 10;
          while (1) 
          {
              if (OS_MsgQueueReceive(MBOX2Q(mbox), msg, wait * 60 * 1000) != OS_OK) {
                  printf("%s(), mbox %p, wait %u min time out, @ %s():%d\n", __func__, mbox, wait, func, line);
                  g_trace_mbox = mbox;
              } else {
                  if (mbox == g_trace_mbox) {
                      printf("%s(), mbox %p, timeout 0x%x, success, msg %p @ %s():%d\n", __func__, mbox, timeout, *msg, func, line);
                      g_trace_mbox = NULL;
                  }
                  break;
              }
          }
      } else {
          if (OS_MsgQueueReceive(MBOX2Q(mbox), msg, timeout) != OS_OK)
              return SYS_ARCH_TIMEOUT;
      }

      MBOX_DEC_USED(mbox);
      return OS_TicksToMSecs(OS_GetTicks() - start_ticks);
  }
#else
  u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
  {
      u32_t start_ticks = OS_GetTicks();
      if (OS_MsgQueueReceive(MBOX2Q(mbox), msg, timeout == 0 ? OS_WAIT_FOREVER : timeout) != OS_OK)
          return SYS_ARCH_TIMEOUT;

      MBOX_DEC_USED(mbox);
      return OS_TicksToMSecs(OS_GetTicks() - start_ticks);
  }
#endif

/** Try to wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty */
#if LWIP_MBOX_DEBUG
  u32_t _sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg, const char *func, int line)
  {
      if (mbox == g_trace_mbox) {
          printf("%s(), mbox %p, @ %s():%d\n", __func__, mbox, func, line);
      }
      if (OS_MsgQueueReceive(MBOX2Q(mbox), msg, 0) != OS_OK) {
          return SYS_MBOX_EMPTY;
      } else {
          MBOX_DEC_USED(mbox);
          return ERR_OK;
      }
  }
#else
  u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
  {
      if (OS_MsgQueueReceive(MBOX2Q(mbox), msg, 0) != OS_OK) {
          return SYS_MBOX_EMPTY;
      } else {
          MBOX_DEC_USED(mbox);
          return ERR_OK;
      }
  }
#endif

/** Delete an mbox
 * @param mbox mbox to delete */
#if LWIP_MBOX_DEBUG
  void _sys_mbox_free(sys_mbox_t *mbox, const char *func, int line)
  {
      if (mbox == g_trace_mbox) {
          printf("%s(), mbox %p, @ %s():%d\n", __func__, mbox, func, line);
          g_trace_mbox = NULL;
      }
      
      #if LWIP_MBOX_TRACE
      LWIP_PLATFORM_DIAG(("free mbox %p, avail %u, used %u, max %u, err %u\n",
      mbox, mbox->avail, mbox->used, mbox->max, mbox->err));
      #endif
      
      if (OS_MsgQueueDelete(MBOX2Q(mbox)) == OS_OK) {
          SYS_STATS_DEC(mbox.used);
      } else {
          LWIP_ERROR("sys_mbox_free: mbox not empty", 0, while(1));
          SYS_STATS_INC(mbox.err);
      }
      
      #if LWIP_RESOURCE_TRACE
      g_lwip_mbox_cnt--;
      #endif
  }
#else
  void sys_mbox_free(sys_mbox_t *mbox)
  {
      #if LWIP_MBOX_TRACE
      LWIP_PLATFORM_DIAG(("free mbox %p, avail %u, used %u, max %u, err %u\n",
      mbox, mbox->avail, mbox->used, mbox->max, mbox->err));
      #endif
    
      if (OS_MsgQueueDelete(MBOX2Q(mbox)) == OS_OK) {
          SYS_STATS_DEC(mbox.used);
      } else {
          LWIP_ERROR("sys_mbox_free: mbox not empty", 0, while(1));
          SYS_STATS_INC(mbox.err);
      }
      
      #if LWIP_RESOURCE_TRACE
      g_lwip_mbox_cnt--;
      #endif
  }
#endif  

  
/* Support only one tcpip thread to save space */
static OS_Thread_t g_lwip_tcpip_thread;

/** The only thread function:
 * Creates a new thread
 * @param name human-readable name for the thread (used for debugging purposes)
 * @param thread thread-function
 * @param arg parameter passed to 'thread'
 * @param stacksize stack size in bytes for the new thread (may be ignored by ports)
 * @param prio priority of the new thread (may be ignored by ports) */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    if (OS_ThreadIsValid(&g_lwip_tcpip_thread))
        return NULL;

    if (OS_ThreadCreate(&g_lwip_tcpip_thread, name, (OS_ThreadEntry_t)thread, arg, (OS_Priority)prio, stacksize) != OS_OK) {
        return NULL;
    }
    return &g_lwip_tcpip_thread;
}

#endif /* (NO_SYS == 0) */

  
#if (SYS_LIGHTWEIGHT_PROT && SYS_ARCH_PROTECT_USE_MUTEX)
  /** mutex for SYS_ARCH_PROTECT */
  OS_Mutex_t g_lwip_sys_mutex;
#endif


/** sys_init() must be called before anthing else. */
void sys_init(void)
{
    #if (SYS_LIGHTWEIGHT_PROT && SYS_ARCH_PROTECT_USE_MUTEX)
    OS_RecursiveMutexCreate(&g_lwip_sys_mutex);
    #endif
}

/** Returns the current time in milliseconds,
 * may be the same as sys_jiffies or at least based on it. */
u32_t sys_now(void)
{
    return OS_TicksToMSecs(OS_GetTicks());
}

#if LWIP_RESOURCE_TRACE
  extern int g_lwip_socket_cnt;

  void lwip_resource_info(void)
  {
      LWIP_PLATFORM_DIAG(("<<< lwip resource info >>>\n"));
      LWIP_PLATFORM_DIAG(("g_lwip_socket_cnt %d\n", g_lwip_socket_cnt));
      LWIP_PLATFORM_DIAG(("g_lwip_mutex_cnt  %d\n", g_lwip_mutex_cnt));
      LWIP_PLATFORM_DIAG(("g_lwip_sem_cnt    %d\n", g_lwip_sem_cnt));
      LWIP_PLATFORM_DIAG(("g_lwip_mbox_cnt   %d\n", g_lwip_mbox_cnt));
  }
#endif /* LWIP_RESOURCE_TRACE */


/** ----------------Redirection Lwip (mem.c) API--------------------- **/
__attribute__((weak)) void mem_init(void)
{
}

void * mem_calloc(mem_size_t count, mem_size_t size)
{
    /* not support calloc yet */
    return NULL;
}

void * mem_trim(void *mem, mem_size_t size)
{
    /* not support trim yet */
    return mem;
}

void * mem_malloc(mem_size_t size)
{
    return OS_Malloc(size);
}

void mem_free(void *mem)
{
    OS_Free(mem);
}


