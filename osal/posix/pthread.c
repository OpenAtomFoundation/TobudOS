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

#include "private/posix_config.h"

#include "errno.h"
#include "pthread.h"
#include "private/pthread.h"
#include "private/time.h"

int __pthread_canceled;

__STATIC__ int pthreads_ready2reap = 0;

__STATIC__ void pthread_dead_reap(void)
{
    pthreads_ready2reap -= pthread_ctl_reap(pthreads_ready2reap);
}

__STATIC__ void pthread_entry(void *data)
{
    void *retval;
    pthread_ctl_t *the_ctl;

    the_ctl = (pthread_ctl_t *)data;

    retval = the_ctl->start_routine(the_ctl->arg);

    pthread_exit(retval);
}

__STATIC__ int pthread_is_cancel_pending(void)
{
    pthread_ctl_t *self_ctl;

    self_ctl = pthread_ctl_self();
    if (self_ctl &&
        self_ctl->cancelpending &&
        self_ctl->cancelstate == PTHREAD_CANCEL_ENABLE) {
        return K_TRUE;
    }

    return K_FALSE;
}

__NOTSUPP__ int pthread_atfork(void (*prepare)(void), void (*parent)(void), void(*child)(void))
{
    return EOPNOTSUPP;
}

__API__ int pthread_attr_destroy(pthread_attr_t *attr)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);

    memset(attr, 0, sizeof(pthread_attr_t));

    return 0;
}

__API__ int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(detachstate, EINVAL);

    *detachstate = attr->detachstate;

    return 0;
}

__NOTSUPP__ int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize)
{
    return EOPNOTSUPP;
}

__API__ int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(inheritsched, EINVAL);

    *inheritsched = attr->inheritsched;

    return 0;
}

__API__ int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(param, EINVAL);

    *param = attr->schedparam;

    return 0;
}

__API__ int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(policy, EINVAL);

    *policy = attr->schedpolicy;

    return 0;
}

__NOTSUPP__ int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope)
{
    return EOPNOTSUPP;
}

__API__ int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(stackaddr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(stacksize, EINVAL);

    if (!attr->stackaddr_valid || !attr->stacksize_valid) {
        return EINVAL;
    }

    *stackaddr = attr->stackaddr;
    *stacksize = attr->stacksize;

    return 0;
}

__API__ int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(stacksize, EINVAL);

    if (!attr->stacksize_valid) {
        return EINVAL;
    }

    *stacksize = attr->stacksize;

    return 0;
}

__API__ int pthread_attr_init(pthread_attr_t *attr)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);

    attr->detachstate                   = PTHREAD_DEFAULT_DETACH_STATE;
    attr->inheritsched                  = PTHREAD_DEFAULT_INHERIT_SCHED;
    attr->schedpolicy                   = PTHREAD_DEFAULT_SCHEDPOLICY;
    attr->schedparam.sched_priority    	= PTHREAD_DEFAULT_PRIORITY;

    attr->stackaddr_valid               = K_FALSE;
    attr->stacksize_valid               = K_FALSE;
    attr->stackaddr                     = K_NULL;
    attr->stacksize                     = 0;

    return 0;
}

__API__ int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);

    if (detachstate != PTHREAD_CREATE_JOINABLE &&
        detachstate != PTHREAD_CREATE_DETACHED) {
        return EINVAL;
    }

    attr->detachstate = detachstate;

    return 0;
}

__NOTSUPP__ int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize)
{
    return EOPNOTSUPP;
}

__API__ int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(inheritsched, EINVAL);

    if (inheritsched != PTHREAD_INHERIT_SCHED &&
        inheritsched != PTHREAD_EXPLICIT_SCHED) {
        return EINVAL;
    }

    attr->inheritsched = inheritsched;

    return 0;
}

__API__ int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(param, EINVAL);

    attr->schedparam = *param;

    return 0;
}

__API__ int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(policy, EINVAL);

    if (policy != SCHED_OTHER &&
        policy != SCHED_FIFO &&
        policy != SCHED_RR) {
        return EINVAL;
    }

    attr->schedpolicy = policy;

    return 0;
}

__NOTSUPP__ int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope)
{
    return EOPNOTSUPP;
}

__API__ int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(stackaddr, EINVAL);

    if (stacksize < PTHREAD_STK_SIZE_MIN) {
        return EINVAL;
    }

    attr->stackaddr         = stackaddr;
    attr->stackaddr_valid   = K_TRUE;

    attr->stacksize         = stacksize;
    attr->stacksize_valid   = K_TRUE;

    return 0;
}

__API__ int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);

    if (stacksize < PTHREAD_STK_SIZE_MIN) {
        return EINVAL;
    }

    attr->stacksize         = stacksize;
    attr->stacksize_valid   = K_TRUE;

    return 0;
}

#if POSIX_CFG_PTHREAD_BARRIER_EN > 0u

__API__ int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(barrier, EINVAL);

    kerr = tos_barrier_destroy((k_barrier_t *)barrier);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(barrier, EINVAL);

    kerr = tos_barrier_create((k_barrier_t *)barrier, (k_barrier_cnt_t)count);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(barrier, EINVAL);

    kerr = tos_barrier_pend((k_barrier_t *)barrier);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__NOTSUPP__ int pthread_barrierattr_destroy(pthread_barrierattr_t *attr)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_barrierattr_init(pthread_barrierattr_t *attr)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared)
{
    return EOPNOTSUPP;
}

#endif /* POSIX_CFG_PTHREAD_BARRIER_EN */

__API__ int pthread_cancel(pthread_t thread)
{
    pthread_ctl_t *the_ctl;

    pthread_lock();

    the_ctl = pthread_ctl_by_id(thread);
    if (!the_ctl) {
        pthread_unlock();
        return ESRCH;
    }

    the_ctl->cancelpending = K_TRUE;

    if (the_ctl->cancelstate == PTHREAD_CANCEL_DISABLE) {
        return EPERM;
    }

    if (the_ctl->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS) {
        tos_task_destroy(the_ctl->the_ktask);
    }

    pthread_unlock();

    return 0;
}

#if POSIX_CFG_PTHREAD_COND_EN > 0u

__API__ int pthread_cond_broadcast(pthread_cond_t *cond)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(cond, EINVAL);

    kerr = tos_sem_post_all((k_sem_t *)cond);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_cond_destroy(pthread_cond_t *cond)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(cond, EINVAL);

    kerr = tos_sem_destroy((k_sem_t *)cond);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(cond, EINVAL);

    kerr = tos_sem_create((k_sem_t *)cond, 0);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_cond_signal(pthread_cond_t *cond)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(cond, EINVAL);

    kerr = tos_sem_post((k_sem_t *)cond);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__STATIC__ int pthread_cond_do_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, k_tick_t timeout)
{
    k_err_t kerr;
    int errcode;

    if (mutex->kmutex.owner != tos_task_curr_task_get()) {
        return EPERM;
    }

    errcode = pthread_mutex_unlock(mutex);
    if (errcode != 0) {
        return EINVAL;
    }

    kerr = tos_sem_pend((k_sem_t *)cond, timeout);
    pthread_mutex_lock(mutex);

    if (kerr == K_ERR_NONE) {
        return 0;
    }

    if (kerr == K_ERR_PEND_TIMEOUT) {
        return ETIMEDOUT;
    }

    return EINVAL;
}

__API__ int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
    k_tick_t timeout;

    TOS_PTR_SANITY_CHECK_RC(cond, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(mutex, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(abstime, EINVAL);

    timeout = timespec_to_ktick(abstime);
    return pthread_cond_do_timedwait(cond, mutex, timeout);
}

__API__ int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    TOS_PTR_SANITY_CHECK_RC(cond, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(mutex, EINVAL);

    return pthread_cond_do_timedwait(cond, mutex, TOS_TIME_FOREVER);
}

__NOTSUPP__ int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_condattr_init(pthread_condattr_t *attr)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared)
{
    return EOPNOTSUPP;
}

#endif /* POSIX_CFG_PTHREAD_COND_EN */

__API__ int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg)
{
    k_err_t kerr;
    pthread_t id;
    int errcode;
    int is_stk_need_free = K_FALSE;
    void *stackaddr = K_NULL;
    size_t stacksize;
    char name[K_TASK_NAME_MAX];
    static uint16_t pthrd_id = 0;
    pthread_attr_t the_attr;
    pthread_ctl_t *self_ctl = K_NULL, *the_ctl = K_NULL;

    TOS_IN_IRQ_CHECK();

    TOS_PTR_SANITY_CHECK_RC(thread, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(start_routine, EINVAL);

    id = pthread_id_alloc();
    if (id == -1) {
        return ENOMEM;
    }

    if (!attr) {
        pthread_attr_init(&the_attr);
    } else {
        the_attr = *attr;
    }

    self_ctl = pthread_ctl_self();

    if (the_attr.inheritsched == PTHREAD_INHERIT_SCHED) {
        if (!self_ctl) {
            /* cannot inherit sched policy from non-POSIX thread */
            errcode = EPERM;
            goto errout0;
        }

        the_attr.schedpolicy    = self_ctl->attr.schedpolicy;
        the_attr.schedparam     = self_ctl->attr.schedparam;
    }

    if (the_attr.stackaddr_valid) {
        /* means we called pthread_attr_setstack and returned OK once before*/
        stackaddr   = the_attr.stackaddr;
        stacksize   = the_attr.stacksize;
    } else if (the_attr.stacksize_valid) {
        /* we called pthread_attr_setstacksize and returned OK once before */
        stacksize   = the_attr.stacksize + PTHREAD_INFO_SIZE;
    } else {
        /* neither the pthread_attr_setstack nor pthread_attr_setstacksize has beed called */
        stacksize   = PTHREAD_DEFAULT_STACKSIZE;
    }

    if (!the_attr.stackaddr_valid) {
        stackaddr   = tos_mmheap_alloc(stacksize);
        if (!stackaddr) {
            errcode = ENOMEM;
            goto errout0;
        }

        is_stk_need_free = K_TRUE;
    }

    /* we keep our little secret onto the bottom of stack, avoiding of multi-times malloc */
    the_ctl                 = (pthread_ctl_t *)stackaddr;
    the_ctl->stackaddr      = (is_stk_need_free ? stackaddr : K_NULL);
    the_ctl->start_routine  = start_routine;
    the_ctl->arg            = arg;
    memset(&the_ctl->ktask, 0, sizeof(k_task_t));

    stackaddr               = (void *)((cpu_addr_t)stackaddr + PTHREAD_INFO_SIZE);
    stacksize              -= PTHREAD_INFO_SIZE;
    snprintf(name, sizeof(name), "pthrd%d", pthrd_id++);

    if (the_attr.detachstate == PTHREAD_CREATE_JOINABLE) {
        kerr = tos_sem_create(&the_ctl->joinner_sem, 0);
        if (kerr != K_ERR_NONE) {
            errcode = EBUSY;
            goto errout0;
        }
    }

    tos_knl_sched_lock();

    pthread_dead_reap();

    kerr = tos_task_create(&the_ctl->ktask, name,
                            pthread_entry, (void *)the_ctl,
                            the_attr.schedparam.sched_priority,
                            stackaddr, stacksize,
                            PTHREAD_DEFAULT_TIMESLICE);
    if (kerr != K_ERR_NONE) {
        errcode = EBUSY;
        tos_knl_sched_unlock();
        goto errout1;
    }

    the_ctl->threadstate    = the_attr.detachstate == PTHREAD_CREATE_JOINABLE ?
                                PTHREAD_STATE_RUNNING : PTHREAD_STATE_DETACHED;

    the_ctl->id             = id;
    the_ctl->attr           = the_attr;
    the_ctl->retval         = K_NULL;
    the_ctl->the_ktask      = &the_ctl->ktask;

    the_ctl->cancelstate    = PTHREAD_CANCEL_ENABLE;
    the_ctl->canceltype     = PTHREAD_CANCEL_DEFERRED;
    the_ctl->cancelpending  = K_FALSE;

    the_ctl->thread_data    = K_NULL;
    tos_slist_init(&the_ctl->cleanup_ctl_list);

    pthread_id_add(id, the_ctl);

    *thread = id;

    tos_knl_sched_unlock();

    return ENOERR;

errout1:
    tos_sem_destroy(&the_ctl->joinner_sem);

errout0:
    if (is_stk_need_free) {
        /* stack is allocated by us */
        tos_mmheap_free(stackaddr);
    }

    return errcode;
}

__API__ int pthread_detach(pthread_t thread)
{
    int errcode = 0;
    pthread_ctl_t *the_ctl;

    pthread_lock();

    the_ctl = pthread_ctl_by_id(thread);
    if (!the_ctl) {
        errcode = ESRCH;
    } else if (the_ctl->threadstate == PTHREAD_STATE_DETACHED) {
        /* already detached */
        errcode = EINVAL;
    } else {
        the_ctl->threadstate = PTHREAD_STATE_DETACHED;
        /* make any who is joining for us wakeup and return(I am detached now!) */
        tos_sem_post_all(&the_ctl->joinner_sem);
    }

    pthread_dead_reap();

    pthread_unlock();

    return errcode;
}

__API__ int pthread_equal(pthread_t t1, pthread_t t2)
{
    return t1 == t2;
}

__API__ void pthread_exit(void *value_ptr)
{
    void *value;
    int key = 0;
    int destructor_called = K_FALSE, destructor_iterations = 0;
    key_destructor_t key_destructor = K_NULL;
    pthread_ctl_t *self_ctl;
    pthread_cleanup_ctl_t *cleanup_ctl, *tmp;

    self_ctl = pthread_ctl_self();
    if (!self_ctl) {
        /* this is a non-POSIX thread */
        return;
    }

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, K_NULL);

    /* call all the cleanup routine */
    TOS_SLIST_FOR_EACH_ENTRY_SAFE(cleanup_ctl, tmp, pthread_cleanup_ctl_t, list, &self_ctl->cleanup_ctl_list) {
        cleanup_ctl->routine(cleanup_ctl->arg);
        tos_slist_del(&cleanup_ctl->list, &self_ctl->cleanup_ctl_list);
        tos_mmheap_free(cleanup_ctl);
    }

    /* call destructor for each key */
    do {
        for (key = 0; key < PTHREAD_KEYS_MAX; ++key) {
            /* key is not created */
            if (!pthread_key_is_alloc(key)) {
                continue;
            }

            /* destrutor is not register-ed */
            key_destructor = pthread_key_destructor_get(key);
            if (!key_destructor) {
                continue;
            }

            if (self_ctl->thread_data) {
                continue;
            }

            value = self_ctl->thread_data[key];
            self_ctl->thread_data[key] = K_NULL;
            key_destructor(value);

            destructor_called = K_TRUE;
        }

        ++destructor_iterations;
    } while (destructor_called && (destructor_iterations <= PTHREAD_DESTRUCTOR_ITERATIONS));

    /* donot forget this */
    if (self_ctl->thread_data) {
        tos_mmheap_free(self_ctl->thread_data);
    }

    pthread_lock();

    self_ctl->retval = value_ptr;

    if (self_ctl->threadstate == PTHREAD_STATE_DETACHED) {
        /* ready to die totally */
        self_ctl->threadstate = PTHREAD_STATE_EXITED;
        ++pthreads_ready2reap;
    } else {
        /* we are dying, but we still waiting for someone to join us(reap for us)*/
        self_ctl->threadstate = PTHREAD_STATE_JOIN;
    }

    /* wakeup all the joniners */
    tos_sem_post_all(&self_ctl->joinner_sem);

    pthread_unlock();

    /* will invoke a knl_sched at last */
    tos_task_destroy(self_ctl->the_ktask);
}

__NOTSUPP__ int pthread_getconcurrency(void)
{
    return EOPNOTSUPP;
}

__API__ int pthread_getcpuclockid(pthread_t thread_id, clockid_t *clock_id)
{
    return EOPNOTSUPP;
}

__API__ int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param)
{
    pthread_ctl_t *the_ctl;

    pthread_lock();

    the_ctl = pthread_ctl_by_id(thread);

    if (!the_ctl) {
        pthread_unlock();
        return ESRCH;
    }

    if (policy) {
        *policy = the_ctl->attr.schedpolicy;
    }

    if (param) {
        *param = the_ctl->attr.schedparam;
    }

    pthread_unlock();

    return 0;
}

__API__ void *pthread_getspecific(pthread_key_t key)
{
    pthread_ctl_t *self_ctl;

    if (key >= PTHREAD_KEYS_MAX || key < 0) {
        return K_NULL;
    }

    self_ctl = pthread_ctl_self();
    if (!self_ctl || !self_ctl->thread_data) {
        /* this is a non-POSIX thread, or thread_data is empty */
        return K_NULL;
    }

    return self_ctl->thread_data[key];
}

__API__ int pthread_join(pthread_t thread, void **value_ptr)
{
    k_err_t kerr;
    int errcode = 0;
    pthread_ctl_t *self_ctl, *the_ctl;

    pthread_testcancel();

    pthread_lock();

    pthread_dead_reap();

    pthread_unlock();

    self_ctl = pthread_ctl_self();
    if (!self_ctl) {
        /* a non-POSIX thread */
        errcode = EPERM;
        goto errout;
    }

    the_ctl = pthread_ctl_by_id(thread);
    if (!the_ctl) {
        errcode = ESRCH;
        goto errout;
    }

    if (the_ctl == self_ctl) {
        errcode = EDEADLK;
        goto errout;
    }

    if (the_ctl->attr.detachstate == PTHREAD_CREATE_DETACHED) {
        /* the target thread is not joinable */
        errcode = EPERM;
        goto errout;
    }

    if (the_ctl->threadstate == PTHREAD_STATE_DETACHED &&
        the_ctl->threadstate == PTHREAD_STATE_EXITED) {
        errcode = EINVAL;
        goto errout;
    }

    if (the_ctl->threadstate == PTHREAD_STATE_JOIN) {
        /* the thread is exit, and waiting to be joined */
        ;
    } else if (the_ctl->threadstate == PTHREAD_STATE_RUNNING) {
        while (the_ctl->threadstate == PTHREAD_STATE_RUNNING) {
            kerr = tos_sem_pend(&the_ctl->joinner_sem, TOS_TIME_FOREVER);
            if (kerr != K_ERR_NONE) {
                errcode = ESRCH;
                goto errout;
            }

            if (pthread_is_cancel_pending()) {
                /* someone cancel us, goto errout and suicide in testcancel */
                errcode = EAGAIN;
                goto errout;
            }
        }
    }

    if (value_ptr) {
        *value_ptr = the_ctl->retval;
    }

    /* the guy now happy to die */
    the_ctl->threadstate = PTHREAD_STATE_EXITED;

    ++pthreads_ready2reap;

    pthread_lock();

    pthread_dead_reap();

    pthread_unlock();

errout:
    pthread_testcancel();
    return errcode;
}

__API__ int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
    pthread_key_t the_key;

    TOS_PTR_SANITY_CHECK_RC(key, -1);

    the_key = pthread_key_alloc();
    if (the_key == -1) {
        *key = -1;
        return EAGAIN;
    }

    pthread_lock();

    pthread_key_destructor_register(the_key, destructor);

    pthread_data_clear(the_key);

    pthread_unlock();

    *key = the_key;

    return 0;
}

__API__ int pthread_key_delete(pthread_key_t key)
{
    int rc;

    pthread_lock();

    rc = pthread_key_free(key);

    pthread_unlock();

    return rc;
}

#if POSIX_CFG_PTHREAD_MUTEX_EN > 0u

__NOTSUPP__ int pthread_mutex_consistent(pthread_mutex_t *mutex)
{
    return EOPNOTSUPP;
}

__API__ int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(mutex, EINVAL);

    kerr = tos_mutex_destroy(&mutex->kmutex);
    if (kerr != K_ERR_NONE) {
        return EINVAL;
    }

    pthread_mutexattr_init(&mutex->attr);

    return 0;
}

__API__ int pthread_mutex_getprioceiling(const pthread_mutex_t *mutex, int *prioceiling)
{
    return EOPNOTSUPP;
}

__API__ int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(mutex, EINVAL);

    if (!attr) {
        pthread_mutexattr_init(&mutex->attr);
        attr = &mutex->attr;
    } else {
        mutex->attr = *attr;
    }

    kerr = tos_mutex_create(&mutex->kmutex);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(mutex, EINVAL);

    if (mutex->attr.type != PTHREAD_MUTEX_RECURSIVE &&
        mutex->kmutex.owner == tos_task_curr_task_get()) {
        /* RECURSIVE is not permitted, and we are the owner */
        return EPERM;
    }

    /* the k_mutex_t is born to support RECURSIVE */
    kerr = tos_mutex_pend(&mutex->kmutex);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling, int *old_ceiling)
{
    return EOPNOTSUPP;
}

__API__ int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime)
{
    k_err_t kerr;
    k_tick_t ktick;

    TOS_PTR_SANITY_CHECK_RC(mutex, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(abstime, EINVAL);

    if (mutex->attr.type != PTHREAD_MUTEX_RECURSIVE &&
        mutex->kmutex.owner == tos_task_curr_task_get()) {
        /* RECURSIVE is not permitted, and we are the owner */
        return EPERM;
    }

    ktick   = timespec_to_ktick(abstime);
    kerr    = tos_mutex_pend_timed(&mutex->kmutex, ktick);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(mutex, EINVAL);

    if (mutex->attr.type != PTHREAD_MUTEX_RECURSIVE &&
        mutex->kmutex.owner == tos_task_curr_task_get()) {
        /* RECURSIVE is not permitted, and we are the owner */
        return EPERM;
    }

    kerr = tos_mutex_pend_timed(&mutex->kmutex, TOS_TIME_NOWAIT);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(mutex, EINVAL);

    if ((!mutex->kmutex.owner ||
        mutex->kmutex.owner != tos_task_curr_task_get()) &&
        mutex->attr.type == PTHREAD_MUTEX_ERRORCHECK) {
        /* the mutex is not locked or not locked by us, and type is PTHREAD_MUTEX_ERRORCHECK */
        return EPERM;
    }

    kerr = tos_mutex_post(&mutex->kmutex);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);

    attr->type = 0xF;

    return 0;
}

__NOTSUPP__ int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *mutex, int *prioceiling)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *mutex, int *protocol)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_mutexattr_getpshared(const pthread_mutexattr_t *mutex, int *pshared)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_mutexattr_getrobust(const pthread_mutexattr_t *mutex, int *robust)
{
    return EOPNOTSUPP;
}

__API__ int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(type, EINVAL);

    *type = attr->type;

    return 0;
}
__API__ int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);

    attr->type = PTHREAD_MUTEX_DEFAULT;

    return 0;
}
__NOTSUPP__ int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_mutexattr_setprotocol(pthread_mutexattr_t *mutex, int protocol)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_mutexattr_setpshared(pthread_mutexattr_t *mutex, int pshared)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_mutexattr_setrobust(pthread_mutexattr_t *mutex, int robust)
{
    return EOPNOTSUPP;
}

__API__ int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    TOS_PTR_SANITY_CHECK_RC(attr, EINVAL);

    if (type < PTHREAD_MUTEX_NORMAL ||
        type > PTHREAD_MUTEX_DEFAULT) {
        return EINVAL;
    }

    attr->type = type;

    return 0;
}

#endif /* POSIX_CFG_PTHREAD_MUTEX_EN */

#if POSIX_CFG_PTHREAD_RWLOCK_EN > 0u

__API__ int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    pthread_once_t old;

    TOS_PTR_SANITY_CHECK_RC(once_control, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(init_routine, EINVAL);

    pthread_lock();

    old = *once_control;
    *once_control = 1;

    pthread_unlock();

    if (!old) {
        init_routine();
    }

    return 0;
}

__API__ int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(rwlock, EINVAL);

    kerr = tos_rwlock_destroy((k_rwlock_t *)rwlock);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(rwlock, EINVAL);

    kerr = tos_rwlock_create((k_rwlock_t *)rwlock);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(rwlock, EINVAL);

    kerr = tos_rwlock_rpend((k_rwlock_t *)rwlock);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime)
{
    k_err_t kerr;
    k_tick_t ktick;

    TOS_PTR_SANITY_CHECK_RC(rwlock, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(abstime, EINVAL);

    ktick   = timespec_to_ktick(abstime);
    kerr    = tos_rwlock_rpend_timed((k_rwlock_t *)rwlock, ktick);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime)
{
    k_err_t kerr;
    k_tick_t ktick;

    TOS_PTR_SANITY_CHECK_RC(rwlock, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(abstime, EINVAL);

    ktick   = timespec_to_ktick(abstime);
    kerr    = tos_rwlock_wpend_timed((k_rwlock_t *)rwlock, ktick);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}


__API__ int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(rwlock, EINVAL);

    kerr = tos_rwlock_rpend_try((k_rwlock_t *)rwlock);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(rwlock, EINVAL);

    kerr = tos_rwlock_wpend_try((k_rwlock_t *)rwlock);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(rwlock, EINVAL);

    kerr = tos_rwlock_post((k_rwlock_t *)rwlock);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(rwlock, EINVAL);

    kerr = tos_rwlock_wpend((k_rwlock_t *)rwlock);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__NOTSUPP__ int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_rwlockattr_init(pthread_rwlockattr_t *attr)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared)
{
    return EOPNOTSUPP;
}

#endif /* POSIX_CFG_PTHREAD_RWLOCK_EN */

__API__ pthread_t pthread_self(void)
{
    pthread_ctl_t *self_ctl;

    self_ctl = pthread_ctl_self();
    if (!self_ctl) {
        /* a non-POSIX thread */
        return -1;
    }

    return self_ctl->id;
}

__API__ int pthread_setcancelstate(int state, int *oldstate)
{
    pthread_ctl_t *self_ctl;

    if (state != PTHREAD_CANCEL_ENABLE &&
        state != PTHREAD_CANCEL_DISABLE) {
        return EINVAL;
    }

    self_ctl = pthread_ctl_self();
    if (!self_ctl) {
        /* this is a non-POSIX thread */
        return EPERM;
    }

    if (oldstate) {
        *oldstate = self_ctl->cancelstate;
    }

    self_ctl->cancelstate = state;

    return 0;
}

__API__ int pthread_setcanceltype(int type, int *oldtype)
{
    pthread_ctl_t *self_ctl;

    if (type != PTHREAD_CANCEL_ASYNCHRONOUS &&
        type != PTHREAD_CANCEL_DEFERRED) {
        return EINVAL;
    }

    self_ctl = pthread_ctl_self();
    if (!self_ctl) {
        /* this is a non-POSIX thread */
        return EPERM;
    }

    if (oldtype) {
        *oldtype = self_ctl->canceltype;
    }

    self_ctl->canceltype = type;

    return 0;
}

__NOTSUPP__ int pthread_setconcurrency(int new_level)
{
    return EOPNOTSUPP;
}

__API__ int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param)
{
    pthread_ctl_t *the_ctl;

    TOS_PTR_SANITY_CHECK_RC(param, EINVAL);

    if (policy != SCHED_OTHER &&
        policy != SCHED_FIFO &&
        policy != SCHED_RR) {
        return EINVAL;
    }

    pthread_lock();

    the_ctl = pthread_ctl_by_id(thread);

    if (!the_ctl) {
        pthread_unlock();
        return ESRCH;
    }

    the_ctl->attr.schedpolicy   = policy;
    the_ctl->attr.schedparam    = *param;

    if (the_ctl->the_ktask) {
        tos_task_prio_change(the_ctl->the_ktask, param->sched_priority);
    }

    pthread_unlock();

    return 0;
}

__API__ int pthread_setschedprio(pthread_t thread, int prio)
{
    pthread_ctl_t *the_ctl;

    pthread_lock();

    the_ctl = pthread_ctl_by_id(thread);

    if (!the_ctl) {
        pthread_unlock();
        return ESRCH;
    }

    the_ctl->attr.schedparam.sched_priority = prio;

    if (the_ctl->the_ktask) {
        tos_task_prio_change(the_ctl->the_ktask, prio);
    }

    pthread_unlock();

    return 0;
}

__API__ int pthread_setspecific(pthread_key_t key, const void *value)
{
    int i = 0;
    pthread_ctl_t *self_ctl;

    if (key >= PTHREAD_KEYS_MAX || key < 0) {
        return EINVAL;
    }

    self_ctl = pthread_ctl_self();
    if (!self_ctl) {
        /* this is a non-POSIX thread */
        return EPERM;
    }

    if (!self_ctl->thread_data) {
        self_ctl->thread_data = (void **)tos_mmheap_alloc(sizeof(void *) * PTHREAD_KEYS_MAX);
        if (!self_ctl->thread_data) {
            return ENOMEM;
        }

        for (i = 0; i < PTHREAD_KEYS_MAX; ++i) {
            self_ctl->thread_data[i] = K_NULL;
        }
    }

    self_ctl->thread_data[key] = (void *)value;

    return 0;
}

#if POSIX_CFG_PTHREAD_SPIN_EN > 0u

__API__ int pthread_spin_destroy(pthread_spinlock_t *lock)
{
    TOS_PTR_SANITY_CHECK_RC(lock, EINVAL);

    if (lock->is_destroyed) {
        return 0;
    }

    lock->is_destroyed  = K_TRUE;
    lock->is_locked     = K_FALSE;

    return 0;
}

__API__ int pthread_spin_init(pthread_spinlock_t *lock, int pshared)
{
    TOS_PTR_SANITY_CHECK_RC(lock, EINVAL);

    lock->is_destroyed  = K_FALSE;
    lock->is_locked     = K_FALSE;

    return 0;
}

__API__ int pthread_spin_lock(pthread_spinlock_t *lock)
{
    TOS_PTR_SANITY_CHECK_RC(lock, EINVAL);

    if (lock->is_destroyed) {
        return EPERM;
    }

    while (!lock->is_locked) {
        lock->is_locked = K_TRUE;
    }

    return 0;
}

__API__ int pthread_spin_trylock(pthread_spinlock_t *lock)
{
    TOS_PTR_SANITY_CHECK_RC(lock, EINVAL);

    if (lock->is_destroyed) {
        return EPERM;
    }

    if (lock->is_locked) {
        return EBUSY;
    }

    lock->is_locked = K_TRUE;
    return 0;
}

__API__ int pthread_spin_unlock(pthread_spinlock_t *lock)
{
    TOS_PTR_SANITY_CHECK_RC(lock, EINVAL);

    if (lock->is_destroyed) {
        return EPERM;
    }

    if (!lock->is_locked) {
        return EPERM;
    }

    lock->is_locked = K_FALSE;

    return 0;
}

#endif /* POSIX_CFG_PTHREAD_SPIN_EN */

__API__ void pthread_testcancel(void)
{
    if (pthread_is_cancel_pending()) {
        pthread_exit(PTHREAD_CANCELD);
    }
}

__API__ void pthread_cleanup_pop(int execute)
{
    pthread_ctl_t *self_ctl;
    pthread_cleanup_ctl_t *cleanup_ctl;

    self_ctl = pthread_ctl_self();
    if (!self_ctl) {
        /* this is a non-POSIX thread */
        return;
    }

    pthread_lock();

    if (tos_slist_empty(&self_ctl->cleanup_ctl_list)) {
        pthread_unlock();
        return;
    }

    cleanup_ctl = TOS_SLIST_FIRST_ENTRY(&self_ctl->cleanup_ctl_list, pthread_cleanup_ctl_t, list);
    tos_slist_del_head(&self_ctl->cleanup_ctl_list);

    pthread_unlock();

    if (execute) {
        cleanup_ctl->routine(cleanup_ctl->arg);
    }

    tos_mmheap_free(cleanup_ctl);
}

__API__ void pthread_cleanup_push(void (*routine)(void*), void *arg)
{
    pthread_ctl_t *self_ctl;
    pthread_cleanup_ctl_t *cleanup_ctl;

    if (!routine) {
        return;
    }

    self_ctl = pthread_ctl_self();
    if (!self_ctl) {
        /* this is a non-POSIX thread */
        return;
    }

    cleanup_ctl = (pthread_cleanup_ctl_t *)tos_mmheap_alloc(sizeof(pthread_cleanup_ctl_t));
    if (!cleanup_ctl) {
        return;
    }

    pthread_lock();
    tos_slist_add_head(&cleanup_ctl->list, &self_ctl->cleanup_ctl_list);
    pthread_unlock();
}

