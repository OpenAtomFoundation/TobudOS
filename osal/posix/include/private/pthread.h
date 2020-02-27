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

#ifndef _POSIX_PRIVATE_PTHREAD_H_
#define  _POSIX_PRIVATE_PTHREAD_H_

#include "tos_k.h"

#include "private/posix_config.h"

#include "sys/types.h"

__CDECLS_BEGIN

typedef struct pthread_control_st {
    uint16_t                threadstate     : 4;
    uint16_t                cancelstate     : 2;
    uint16_t                canceltype      : 2;
    uint16_t                cancelpending   : 1;

    pthread_t               id;
    pthread_attr_t          attr;
    k_task_t                ktask;
    k_task_t               *the_ktask;

    k_sem_t                 joinner_sem;

    void                   *(*start_routine)(void *);   /* start routine of the pthread */
    void                   *arg;                        /* argument to start routine */
    void                   *retval;                     /* return value of start routine */
    void                   *stackaddr;                  /* memory of address */

    k_slist_t               cleanup_ctl_list;

    void                  **thread_data;
} pthread_ctl_t;

typedef struct pthread_cleanup_control_st {
    void        (*routine)(void *);     /* function to be called */
    void        *arg;                   /* argument for the routine */
    k_slist_t   list;
} pthread_cleanup_ctl_t;

#define PTHREAD_KEYS_MAX                (POSIX_CFG_PTHREAD_KEYS_MAX)

typedef void (*key_destructor_t)(void*);

typedef struct pthread_key_control_st {
    k_bmtbl_t           key_bitmap_tbl[TOS_BITMAP_SIZE(PTHREAD_KEYS_MAX)];
    k_bitmap_t          key_bitmap;
    key_destructor_t    destructors[PTHREAD_KEYS_MAX];
} pthread_key_ctl_t;

#define PTHREAD_INFO_SIZE                   (sizeof(pthread_ctl_t))
#define PTHREAD_STK_SIZE_MIN                (K_TASK_STK_SIZE_MIN + PTHREAD_INFO_SIZE)

#define PTHREAD_DEFAULT_TIMESLICE           20
#define PTHREAD_DEFAULT_STACKSIZE           (2048 + PTHREAD_INFO_SIZE)
#define PTHREAD_DEFAULT_INHERIT_SCHED       PTHREAD_INHERIT_SCHED
#define PTHREAD_DEFAULT_SCHEDPOLICY         SCHED_OTHER
#define PTHREAD_DEFAULT_PRIORITY            (TOS_CFG_TASK_PRIO_MAX / 2)
#define PTHREAD_DEFAULT_DETACH_STATE        PTHREAD_CREATE_JOINABLE

#define PTHREAD_DESTRUCTOR_ITERATIONS       (POSIX_CFG_PTHREAD_DESTRUCTOR_ITERATIONS)

#define PTHREAD_THREADS_MAX                 (POSIX_CFG_PTHREAD_THREADS_MAX)

__KNL__pthread_ctl_t *pthread_ctl_self(void);

__KNL__ pthread_ctl_t *pthread_ctl_by_id(pthread_t id);

__KNL__ int pthread_id_add(pthread_t id, pthread_ctl_t *info);

__KNL__ pthread_t pthread_id_alloc(void);

__KNL__ int pthread_id_free(pthread_t id);

__KNL__ void pthread_data_clear(pthread_key_t key);

__KNL__ int pthread_key_ctl_init(void);

__KNL__ pthread_key_t pthread_key_alloc(void);

__KNL__ int pthread_key_is_alloc(pthread_key_t key);

__KNL__ int pthread_key_free(pthread_key_t key);

__KNL__ int pthread_key_destructor_register(pthread_key_t key, key_destructor_t destructor);

__KNL__ key_destructor_t pthread_key_destructor_get(pthread_key_t key);

__KNL__ int pthread_ctl_reap(int pthreads_ready2reap);

__KNL__ void pthread_lock(void);

__KNL__ void pthread_unlock(void);

__KNL__ int pthread_lock_init(void);

__KNL__ int pthread_init(void);

__CDECLS_END

#endif /* _POSIX_PRIVATE_PTHREAD_PRV_H_ */

