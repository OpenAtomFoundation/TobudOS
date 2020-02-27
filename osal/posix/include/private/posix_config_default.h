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

#ifndef _POSIX_CONFIG_DEFAULT_H_
#define  _POSIX_CONFIG_DEFAULT_H_

#ifndef POSIX_CFG_PTHREAD_THREADS_MAX
#define POSIX_CFG_PTHREAD_THREADS_MAX               32
#endif

#ifndef POSIX_CFG_PTHREAD_DESTRUCTOR_ITERATIONS
#define POSIX_CFG_PTHREAD_DESTRUCTOR_ITERATIONS     4
#endif

#ifndef POSIX_CFG_PTHREAD_KEYS_MAX
#define POSIX_CFG_PTHREAD_KEYS_MAX                  8
#endif

#ifndef POSIX_CFG_TIMERS_MAX
#define POSIX_CFG_TIMERS_MAX                        8
#endif

#ifndef POSIX_CFG_MQUEUE_MAX
#define POSIX_CFG_MQUEUE_MAX                        8
#endif

#ifndef POSIX_CFG_PTHREAD_BARRIER_EN
#define POSIX_CFG_PTHREAD_BARRIER_EN                1u
#endif

#ifndef POSIX_CFG_PTHREAD_COND_EN
#define POSIX_CFG_PTHREAD_COND_EN                   1u
#endif

#ifndef POSIX_CFG_PTHREAD_MUTEX_EN
#define POSIX_CFG_PTHREAD_MUTEX_EN                  1u
#endif

#ifndef POSIX_CFG_PTHREAD_RWLOCK_EN
#define POSIX_CFG_PTHREAD_RWLOCK_EN                 1u
#endif

#ifndef POSIX_CFG_PTHREAD_SPIN_EN
#define POSIX_CFG_PTHREAD_SPIN_EN                   1u
#endif

#ifndef POSIX_CFG_SEM_EN
#define POSIX_CFG_SEM_EN                            1u
#endif

#ifndef POSIX_CFG_MQUEUE_EN
#define POSIX_CFG_MQUEUE_EN                         1u
#endif

#ifndef POSIX_CFG_TIMER_EN
#define POSIX_CFG_TIMER_EN                          1u
#endif

#endif /* _POSIX_CONFIG_DEFAULT_H_ */

