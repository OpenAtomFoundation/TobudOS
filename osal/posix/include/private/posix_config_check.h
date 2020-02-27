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

#ifndef _POSIX_CONFIG_CHECK_H_
#define  _POSIX_CONFIG_CHECK_H_

#include "tos_config.h"
#include "private/posix_config_default.h"

#if     (TOS_CFG_MMHEAP_EN == 0u)
#error "INVALID config, Must enable TOS_CFG_MMHEAP_EN to use posix stuff"
#endif

#if     (POSIX_CFG_PTHREAD_COND_EN > 0u) && (TOS_CFG_SEM_EN == 0u)
#error "INVALID config, Must enable TOS_CFG_SEM_EN to use pthread_cond"
#endif

#if     (POSIX_CFG_PTHREAD_COND_EN > 0u) && (TOS_CFG_MUTEX_EN == 0u)
#error "INVALID config, Must enable TOS_CFG_MUTEX_EN to use pthread_cond"
#endif

#if     (POSIX_CFG_PTHREAD_MUTEX_EN > 0u) && (TOS_CFG_MUTEX_EN == 0u)
#error "INVALID config, Must enable TOS_CFG_MUTEX_EN to use pthread_mutex"
#endif

#if     (POSIX_CFG_PTHREAD_RWLOCK_EN > 0u) && (TOS_CFG_SEM_EN == 0u)
#error "INVALID config, Must enable TOS_CFG_SEM_EN to use pthread_rwlock"
#endif

#if     (POSIX_CFG_PTHREAD_RWLOCK_EN > 0u) && (TOS_CFG_MUTEX_EN == 0u)
#error "INVALID config, Must enable TOS_CFG_MUTEX_EN to use pthread_rwlock"
#endif

#if     (POSIX_CFG_SEM_EN > 0u) && (TOS_CFG_SEM_EN == 0u)
#error "INVALID config, Must enable TOS_CFG_SEM_EN to use posix sem"
#endif

#if     (POSIX_CFG_MQUEUE_EN > 0u) && (TOS_CFG_PRIORITY_MAIL_QUEUE_EN == 0u)
#error "INVALID config, Must enable TOS_CFG_PRIORITY_MAIL_QUEUE_EN to use posix mqueue"
#endif

#if     (POSIX_CFG_TIMER_EN > 0u) && (TOS_CFG_TIMER_EN == 0u)
#error "INVALID config, Must enable TOS_CFG_TIMER_EN to use posix timer"
#endif

#endif /* _POSIX_CONFIG_CHECK_H_ */

