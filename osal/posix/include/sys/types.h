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

#ifndef _POSIX_SYS_TYPES_H_
#define  _POSIX_SYS_TYPES_H_

#include "stddef.h"
#include "stdint.h"

typedef uint32_t    blkcnt_t;           /* Used for file block counts. */
typedef size_t      blksize_t;           /* Used for block sizes. */

typedef uint64_t    clock_t;            /* Used for system times in clock ticks or CLOCKS_PER_SEC; see <time.h>. */
typedef uint32_t    clockid_t;           /* Used for clock ID type in the clock and timer functions. */

typedef uint32_t    dev_t;             /* Used for device IDs. */
typedef uint32_t    fsblkcnt_t;         /* Used for file system block counts. */
typedef uint32_t    fsfilcnt_t;         /* Used for file system file counts. */
typedef uint32_t    gid_t;              /* Used for group IDs. */
typedef uint32_t    id_t;               /* Used as a general identifier; can be used to contain at least a pid_t, uid_t, or gid_t. */
typedef uint32_t    ino_t;              /* Used for file serial numbers. */
typedef uint32_t    key_t;              /* Used for XSI interprocess communication. */
typedef uint32_t    mode_t;             /* Used for some file attributes. */
typedef uint32_t    nlink_t;            /* Used for link counts. */
typedef uint32_t    off_t;              /* Used for file sizes. */
typedef int         pid_t;              /* Used for process IDs and process group IDs. */
typedef uint32_t    uid_t;              /* Used for user IDs. */

#if 0   /* we donnot typedef a size_t or ssize_t here, use what is supplied by stddef.h */
size_t  /* Used for sizes of objects. */
ssize_t /* Used for a count of bytes or an error indication. */
#endif

typedef int         ssize_t;

typedef uint32_t    suseconds_t;        /* Used for time in microseconds. */

typedef uint32_t    time_t;                 /* Used for time in seconds. */

struct sched_param {
    int     sched_priority;     /* Process or thread execution scheduling priority. */
};

/* Used to identify a thread attribute object. */
typedef struct pthread_attr_st {
    uint32_t            detachstate         : 2;
    uint32_t            inheritsched        : 2;    /* inherit parent priority/policy? */
    uint32_t            schedpolicy         : 2;    /* pthread scheduler policy */
    uint32_t            stackaddr_valid     : 1;
    uint32_t            stacksize_valid     : 1;
    struct sched_param  schedparam;
    void               *stackaddr;      /* address of memory to be used as stack */
    size_t              stacksize;      /* size of the stack allocated for the pthread */
} pthread_attr_t;

/* Used to identify a barrier. */
typedef k_barrier_t     pthread_barrier_t;

/* Used to define a barrier attributes object. */
typedef uint32_t        pthread_barrierattr_t;

/* Used for condition variables. */
typedef k_sem_t         pthread_cond_t;

/* Used to identify a condition attribute object. */
typedef int             pthread_condattr_t;

/* Used for thread-specific data keys. */
typedef int             pthread_key_t;

/* Used to identify a mutex attribute object. */
typedef struct pthread_mutexattr_st {
    uint8_t type        : 4;
    uint8_t reserved    : 4;
} pthread_mutexattr_t;

/* Used for mutexes. */
typedef struct pthread_mutex_st {
    k_mutex_t           kmutex;
    pthread_mutexattr_t attr;
} pthread_mutex_t;

/* Used for dynamic package initialization. */
typedef int             pthread_once_t;

/* Used for read-write locks. */
typedef k_rwlock_t      pthread_rwlock_t;

/* Used for read-write lock attributes. */
typedef int             pthread_rwlockattr_t;

/* Used to identify a spin lock. */
typedef struct pthread_spinlock_st {
    uint8_t  is_destroyed   : 1;
    uint8_t  is_locked      : 1;
} pthread_spinlock_t;

/* Used to identify a thread. */
typedef pid_t           pthread_t;

/* Used for timer ID returned by timer_create(). */
typedef int             timer_t;

/* Used to identify a trace stream attributes object */
typedef int             trace_attr_t;

/* Used to identify a trace event type. */
typedef int             trace_event_id_t;

/* Used to identify a trace event type set. */
typedef int             trace_event_set_t;

/* Used to identify a trace stream. */
typedef int             trace_id_t;

#endif /* _POSIX_SYS_TYPES_H_ */

