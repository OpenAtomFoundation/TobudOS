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

evtdrv_task_entry_t    *evtdrv_task_table           = K_NULL;
evtdrv_ttb_sz_t         evtdrv_task_table_size      = 0;

evtdrv_task_id_t        evtdrv_curr_task            = TOS_EVTDRV_TASK_ID_NONE;

evtdrv_event_t         *evtdrv_events               = K_NULL;

k_evtdrv_poll_t         evtdrv_poll                 = K_NULL;

evtdrv_tick_t           evtdrv_tick_count           = (evtdrv_tick_t)0u;

TOS_LIST_DEFINE(evtdrv_msg_list);

TOS_LIST_DEFINE(evtdrv_timer_list);

#endif

