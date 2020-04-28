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

#ifndef _POSIX_SCHED_H_
#define  _POSIX_SCHED_H_

#include "tos_k.h"

#include "sys/types.h"
#include "time.h"

__CDECLS_BEGIN

/* Scheduling Psolicies, values for schedpolicy */
#define SCHED_FIFO      0   /* First in-first out (FIFO) scheduling policy. */
#define SCHED_RR        1   /* Round robin scheduling policy. */
#define SCHED_OTHER     2   /* Another scheduling policy. */

#define __NOTSUPP__

__API__     int sched_get_priority_max(int policy);
__API__     int sched_get_priority_min(int policy);
__API__     int sched_getparam(pid_t pid, struct sched_param *param);
__NOTSUPP__ int sched_getscheduler(pid_t pid);
__API__     int sched_rr_get_interval(pid_t pid, struct timespec *interval);
__API__     int sched_setparam(pid_t pid, const struct sched_param *param);
__NOTSUPP__ int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
__API__     int sched_yield(void);

__CDECLS_END

#endif /* _POSIX_SCHED_H_ */

