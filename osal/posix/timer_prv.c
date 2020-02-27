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

#include "tos_k.h"

#include "time.h"
#include "private/timer.h"

__STATIC__ ptimer_ctl_t *ptimer_ctl_table[TIMERS_MAX] = { 0 };

__KNL__ int timer_id_add(timer_t id, ptimer_ctl_t *ptimer_ctl)
{
    TOS_CPU_CPSR_ALLOC();

    if (id < 0 ||
        id >= TOS_COUNT_OF(ptimer_ctl_table) ||
        ptimer_ctl_table[id]) {
        return -1;
    }

    TOS_CPU_INT_DISABLE();

    ptimer_ctl_table[id] = ptimer_ctl;

    TOS_CPU_INT_ENABLE();

    return 0;
}

__KNL__ timer_t timer_id_alloc(void)
{
    TOS_CPU_CPSR_ALLOC();
    int i = 0;

    TOS_CPU_INT_DISABLE();

    for (i = 0; i < TOS_COUNT_OF(ptimer_ctl_table); ++i) {
        if (!ptimer_ctl_table[i]) {
            TOS_CPU_INT_ENABLE();
            return (timer_t)i;
        }
    }

    TOS_CPU_INT_ENABLE();
    return -1;
}

__KNL__ int timer_id_free(timer_t id)
{
    TOS_CPU_CPSR_ALLOC();

    if (id < 0 ||
        id >= TOS_COUNT_OF(ptimer_ctl_table) ||
        !ptimer_ctl_table[id]) {
        return -1;
    }

    TOS_CPU_INT_DISABLE();

    ptimer_ctl_table[id] = K_NULL;

    TOS_CPU_INT_ENABLE();

    return 0;
}

__KNL__ ptimer_ctl_t *timer_by_id(timer_t id)
{
    ptimer_ctl_t *the_ctl;

    the_ctl = ptimer_ctl_table[id];

    if (!the_ctl) {
        return K_NULL;
    }

    if (the_ctl->id != id) {
        return K_NULL;
    }

    return the_ctl;
}

