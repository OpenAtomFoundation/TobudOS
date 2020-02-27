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

#ifndef _POSIX_TIME_H_
#define  _POSIX_TIME_H_

#include "tos_k.h"

#include "sys/types.h"
#include "signal.h"

__CDECLS_BEGIN

struct tm {
    int     tm_sec;     /* Seconds [0,60] */
    int     tm_min;     /* Minutes [0,59] */
    int     tm_hour;    /* Hour [0,23] */
    int     tm_mday;    /* Day of month [1,31] */
    int     tm_mon;     /* Month of year [0,11] */
    int     tm_year;    /* Years since 1900 */
    int     tm_wday;    /* Day of week [0,6] (Sunday =0) */
    int     tm_yday;    /* Day of year [0,365] */
};

struct timespec {
    time_t  tv_sec;     /* seconds */
    long    tv_nsec;    /* nanoseconds */
};

struct itimerspec {
    struct timespec  it_interval;   /* timer period */
    struct timespec it_value;       /* timer expiration */
};

#define CLOCK_REALTIME          0

#define __NOTSUPP__

__API__     char       *asctime(const struct tm *timeptr);
__API__     char       *asctime_r(const struct tm *asctime_r, char *buf);
__API__     clock_t     clock(void);
__API__     int         clock_getcpuclockid(pid_t pid, clockid_t *clock_id);
__API__     int         clock_getres(clockid_t clock_id, struct timespec *res);
__API__     int         clock_gettime(clockid_t clock_id, struct timespec *tp);
__API__     int         clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp, struct timespec *rmtp);
__API__     int         clock_settime(clockid_t clock_id, const struct timespec *tp);
__API__     char       *ctime(const time_t *clock);
__API__     char       *ctime_r(const time_t *clock, char *buf);
__API__     double      difftime(time_t time1, time_t time0);
__API__     struct tm   *getdate(const char *string);
__API__     struct tm   *gmtime(const time_t *timer);
__API__     struct tm   *gmtime_r(const time_t *timer, struct tm *result);
__API__     struct tm   *localtime(const time_t *timer);
__API__     struct tm   *localtime_r(const time_t *timer, struct tm *result);
__API__     time_t      mktime(struct tm *timeptr);
__API__     int         nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
__API__     size_t      strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr);
#if 0
__API__     size_t      strftime_l(char *s, size_t maxsize, const char *format, const struct tm *timeptr, locale_t locale);
#endif
__API__     char       *strptime(const char *buf, const char *format, struct tm *tm);
__API__     time_t      time(time_t *tloc);
__API__     int         timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid);
__API__     int         timer_delete(timer_t timerid);
__NOTSUPP__ int         timer_getoverrun(timer_t timerid);
__API__     int         timer_gettime(timer_t timerid, struct itimerspec *value);
__API__     int         timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue);
__API__     void        tzset(void);

__CDECLS_END

#endif /* _POSIX_TIME_H_ */

