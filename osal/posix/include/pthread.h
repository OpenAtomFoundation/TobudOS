/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _POSIX_PTHREAD_H_
#define  _POSIX_PTHREAD_H_

#include "tos_k.h"

#include "stddef.h"
#include "limits.h"
#include "sys/types.h"
#include "sched.h"
#include "time.h"

__CDECLS_BEGIN

extern int __pthread_canceled;

#define PTHREAD_CANCELD                 ((void *)(&__pthread_canceled))

/* type of mutex */
#define PTHREAD_MUTEX_NORMAL            0
#define PTHREAD_MUTEX_ERRORCHECK        1
#define PTHREAD_MUTEX_RECURSIVE         2
#define PTHREAD_MUTEX_DEFAULT           PTHREAD_MUTEX_NORMAL

/* state of the thread */
#define PTHREAD_STATE_DETACHED          1       /* The thread is running but detached */
#define PTHREAD_STATE_RUNNING           2       /* The thread is running and will wait to join when it exits */
#define PTHREAD_STATE_JOIN              3       /* The thread has exited and is waiting to be joined */
#define PTHREAD_STATE_EXITED            4       /* The thread has exited and is ready to be reaped */

/* cancelability state */
#define PTHREAD_CANCEL_ENABLE           1
#define PTHREAD_CANCEL_DISABLE          2

/* cancelability type */
#define PTHREAD_CANCEL_ASYNCHRONOUS     1
#define PTHREAD_CANCEL_DEFERRED         2

/* values for detachstate*/
#define PTHREAD_CREATE_JOINABLE         1
#define PTHREAD_CREATE_DETACHED         2

/* values for inheritsched */
#define PTHREAD_INHERIT_SCHED           1
#define PTHREAD_EXPLICIT_SCHED          2

#define PTHREAD_ONCE_INIT               0

#define PTHREAD_COND_INITIALIZER

#define PTHREAD_MUTEX_INITIALIZER

#define PTHREAD_RWLOCK_INITIALIZER

#define __NOTSUPP__

__NOTSUPP__ int     pthread_atfork(void (*prepare)(void), void (*parent)(void), void(*child)(void));
__API__     int     pthread_attr_destroy(pthread_attr_t *attr);
__API__     int     pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
__NOTSUPP__ int     pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
__API__     int     pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);
__API__     int     pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);
__API__     int     pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
__NOTSUPP__ int     pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);
__API__     int     pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);
__API__     int     pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
__API__     int     pthread_attr_init(pthread_attr_t *attr);
__API__     int     pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
__NOTSUPP__ int     pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
__API__     int     pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
__API__     int     pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
__API__     int     pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
__NOTSUPP__ int     pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
__API__     int     pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
__API__     int     pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);

__API__     int     pthread_barrier_destroy(pthread_barrier_t *barrier);
__API__     int     pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count);
__API__     int     pthread_barrier_wait(pthread_barrier_t *barrier);
__NOTSUPP__ int     pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
__NOTSUPP__ int     pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared);
__NOTSUPP__ int     pthread_barrierattr_init(pthread_barrierattr_t *attr);
__NOTSUPP__ int     pthread_barrierattr_setpshared(pthread_barrierattr_t *, int pshared);

__API__     int     pthread_cancel(pthread_t thread);

__API__     int     pthread_cond_broadcast(pthread_cond_t *cond);
__API__     int     pthread_cond_destroy(pthread_cond_t *cond);
__API__     int     pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
__API__     int     pthread_cond_signal(pthread_cond_t *cond);
__API__     int     pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
__API__     int     pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
__NOTSUPP__ int     pthread_condattr_destroy(pthread_condattr_t *attr);
__NOTSUPP__ int     pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id);
__NOTSUPP__ int     pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);
__NOTSUPP__ int     pthread_condattr_init(pthread_condattr_t *attr);
__NOTSUPP__ int     pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);
__NOTSUPP__ int     pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);

__API__     int     pthread_create(pthread_t *pthread, const pthread_attr_t *attr, void *(*entry)(void*), void *arg);
__API__     int     pthread_detach(pthread_t thread);
__API__     int     pthread_equal(pthread_t t1, pthread_t t2);
__API__     void    pthread_exit(void *value_ptr);

__NOTSUPP__ int     pthread_getconcurrency(void);
__NOTSUPP__ int     pthread_getcpuclockid(pthread_t thread_id, clockid_t *clock_id);
__API__     int     pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param);
__API__     void   *pthread_getspecific(pthread_key_t key);
__API__     int     pthread_join(pthread_t thread, void **value_ptr);

__API__     int     pthread_key_create(pthread_key_t *, void (*)(void*));
__API__     int     pthread_key_delete(pthread_key_t);

__NOTSUPP__ int     pthread_mutex_consistent(pthread_mutex_t *mutex);
__API__     int     pthread_mutex_destroy(pthread_mutex_t *mutex);
__NOTSUPP__ int     pthread_mutex_getprioceiling(const pthread_mutex_t *mutex, int *prioceiling);
__API__     int     pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
__API__     int     pthread_mutex_lock(pthread_mutex_t *mutex);
__NOTSUPP__ int     pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling, int *old_ceiling);
__API__     int     pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);
__API__     int     pthread_mutex_trylock(pthread_mutex_t *mutex);
__API__     int     pthread_mutex_unlock(pthread_mutex_t *mutex);
__API__     int     pthread_mutexattr_destroy(pthread_mutexattr_t *);
__NOTSUPP__ int     pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *mutex, int *prioceiling);
__NOTSUPP__ int     pthread_mutexattr_getprotocol(const pthread_mutexattr_t *mutex, int *protocol);
__NOTSUPP__ int     pthread_mutexattr_getpshared(const pthread_mutexattr_t *mutex, int *pshared);
__NOTSUPP__ int     pthread_mutexattr_getrobust(const pthread_mutexattr_t *mutex, int *robust);
__API__     int     pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
__API__     int     pthread_mutexattr_init(pthread_mutexattr_t *attr);
__NOTSUPP__ int     pthread_mutexattr_setprioceiling(pthread_mutexattr_t *mutex, int prioceiling);
__NOTSUPP__ int     pthread_mutexattr_setprotocol(pthread_mutexattr_t *mutex, int protocol);
__NOTSUPP__ int     pthread_mutexattr_setpshared(pthread_mutexattr_t *mutex, int pshared);
__NOTSUPP__ int     pthread_mutexattr_setrobust(pthread_mutexattr_t *mutex, int robust);
__API__     int     pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);

__API__     int     pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

__API__     int     pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
__API__     int     pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
__API__     int     pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
__API__     int     pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
__API__     int     pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
__API__     int     pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
__API__     int     pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
__API__     int     pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
__API__     int     pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
__NOTSUPP__ int     pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
__NOTSUPP__ int     pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared);
__NOTSUPP__ int     pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
__NOTSUPP__ int     pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

__API__     pthread_t   pthread_self(void);
__API__     int     pthread_setcancelstate(int state, int *oldstate);
__API__     int     pthread_setcanceltype(int type, int *oldtype);
__NOTSUPP__ int     pthread_setconcurrency(int new_level);
__API__     int     pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);
__API__     int     pthread_setschedprio(pthread_t, int);
__API__     int     pthread_setspecific(pthread_key_t key, const void *value);

__API__     int     pthread_spin_destroy(pthread_spinlock_t *lock);
__API__     int     pthread_spin_init(pthread_spinlock_t *lock, int pshared);
__API__     int     pthread_spin_lock(pthread_spinlock_t *lock);
__API__     int     pthread_spin_trylock(pthread_spinlock_t *lock);
__API__     int     pthread_spin_unlock(pthread_spinlock_t *lock);

__API__     void    pthread_testcancel(void);
__API__     void    pthread_cleanup_pop(int execute);
__API__     void    pthread_cleanup_push(void (*routine)(void*), void *arg);

__CDECLS_END

#endif /* _POSIX_PTHREAD_H_ */

