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

#include "mqueue.h"
#include "private/mqueue.h"

__STATIC__ mqueue_ctl_t *mqueue_ctl_table[MQUEUE_MAX] = { 0 };

__KNL__ int mqueue_id_add(mqd_t id, mqueue_ctl_t *mqueue_ctl)
{
    TOS_CPU_CPSR_ALLOC();

    if (id < 0 ||
        id >= TOS_COUNT_OF(mqueue_ctl_table) ||
        mqueue_ctl_table[id]) {
        return -1;
    }

    TOS_CPU_INT_DISABLE();

    mqueue_ctl_table[id] = mqueue_ctl;

    TOS_CPU_INT_ENABLE();

    return 0;
}

__KNL__ mqd_t mqueue_id_alloc(void)
{
    TOS_CPU_CPSR_ALLOC();
    int i = 0;

    TOS_CPU_INT_DISABLE();

    for (i = 0; i < TOS_COUNT_OF(mqueue_ctl_table); ++i) {
        if (!mqueue_ctl_table[i]) {
            TOS_CPU_INT_ENABLE();
            return (mqd_t)i;
        }
    }

    TOS_CPU_INT_ENABLE();
    return -1;
}

__KNL__ int mqueue_id_free(mqd_t id)
{
    TOS_CPU_CPSR_ALLOC();

    if (id < 0 ||
        id >= TOS_COUNT_OF(mqueue_ctl_table) ||
        !mqueue_ctl_table[id]) {
        return -1;
    }

    TOS_CPU_INT_DISABLE();

    mqueue_ctl_table[id] = K_NULL;

    TOS_CPU_INT_ENABLE();

    return 0;
}

__KNL__ mqueue_ctl_t *mqueue_by_id(mqd_t id)
{
    mqueue_ctl_t *the_ctl;

    the_ctl = mqueue_ctl_table[id];

    if (!the_ctl) {
        return K_NULL;
    }

    if (the_ctl->id != id) {
        return K_NULL;
    }

    return the_ctl;
}

