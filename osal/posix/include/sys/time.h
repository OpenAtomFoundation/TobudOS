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

#ifndef _POSIX_SYS_TIME_H_
#define  _POSIX_SYS_TIME_H_

#include "sys/types.h"
#include "time.h"

__CDECLS_BEGIN

struct timeval {
    long    tv_sec;
    long    tv_usec;
};

struct itimerval {
    struct timeval  it_interval;    /* timer interval */
    struct timeval  it_value;       /* current value */
};

#define TIMEVAL_TO_TIMESPEC(tv, ts) {                   \
    (ts)->tv_sec = (tv)->tv_sec;                        \
    (ts)->tv_nsec = (tv)->tv_usec * 1000;               \
}

#define TIMESPEC_TO_TIMEVAL(tv, ts) {                   \
    (tv)->tv_sec = (ts)->tv_sec;                        \
    (tv)->tv_usec = (ts)->tv_nsec / 1000;               \
}

struct timezone {
    int     tz_minuteswest;     /* minutes west of Greenwich */
    int     tz_dsttime;         /* type of dst correction */
};

__API__ int getitimer(int which, struct itimerval *value);
__API__ int gettimeofday(struct timeval *tp, void *tzp);
__API__ int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue);
#if 0
int   select(int, fd_set *restrict, fd_set *restrict, fd_set *restrict, struct timeval *restrict);
#endif
__API__ int utimes(const char *path, const struct timeval [2]);

__CDECLS_END

#endif /* _POSIX_SYS_TIME_H_ */

