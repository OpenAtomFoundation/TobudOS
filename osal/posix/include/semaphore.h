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

#ifndef _POSIX_SEMAPHORE_H_
#define  _POSIX_SEMAPHORE_H_

#include "tos_k.h"

#include "time.h"

__CDECLS_BEGIN

typedef k_sem_t     sem_t;

#define __NOTSUPP__

__NOTSUPP__ int     sem_close(sem_t *sem);
__API__     int     sem_destroy(sem_t *sem);
__API__     int     sem_getvalue(sem_t *sem, int *sval);
__API__     int     sem_init(sem_t *sem, int pshared, unsigned value);
__NOTSUPP__ sem_t   *sem_open(const char *name, int oflag, ...);
__API__     int     sem_post(sem_t *sem);
__API__     int     sem_timedwait(sem_t *sem, const struct timespec *abstime);
__API__     int     sem_trywait(sem_t *sem);
__NOTSUPP__ int     sem_unlink(const char *name);
__API__     int     sem_wait(sem_t *sem);

__CDECLS_END

#endif /* _POSIX_SEMAPHORE_H_ */

