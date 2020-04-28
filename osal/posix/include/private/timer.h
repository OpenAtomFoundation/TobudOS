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

#ifndef _POSIX_PRIVATE_TIMER_H_
#define  _POSIX_PRIVATE_TIMER_H_

#include "tos_k.h"

#include "private/posix_config.h"

#include "signal.h"

__CDECLS_BEGIN

#define TIMERS_MAX                 (POSIX_CFG_TIMERS_MAX)

typedef struct ptimer_control_st {
    timer_t         id;
    k_timer_t       ktimer;

    void            (*sigev_notify_function)(union sigval);
    union sigval    sigev_value;
} ptimer_ctl_t;

__KNL__ int timer_id_add(timer_t id, ptimer_ctl_t *ptimer_ctl);

__KNL__ timer_t timer_id_alloc(void);

__KNL__ int timer_id_free(timer_t id);

__KNL__ ptimer_ctl_t *timer_by_id(timer_t id);

__CDECLS_END

#endif /* _POSIX_PRIVATE_TIMER_H_*/

