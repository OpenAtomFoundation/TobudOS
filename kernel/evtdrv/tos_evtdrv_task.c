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

#include "tos_evtdrv.h"

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

__API__ evtdrv_task_id_t tos_evtdrv_task_self(void)
{
    return evtdrv_curr_task;
}

__KNL__ evtdrv_bool_t evtdrv_task_id_is_invalid(evtdrv_task_id_t task_id)
{
    return task_id >= evtdrv_task_table_size;
}

__KNL__ evtdrv_bool_t evtdrv_task_is_self(evtdrv_task_id_t task_id)
{
    return task_id == evtdrv_curr_task;
}

__KNL__ void evtdrv_task_init(void)
{
    evtdrv_task_id_t i;

    for (i = 0; i < evtdrv_task_table_size; ++i) {
        (evtdrv_task_table[i])(TOS_EVTDRV_SYS_EVENT_INIT);
    }
}

#endif

