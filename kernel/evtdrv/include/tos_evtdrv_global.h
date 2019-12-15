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

#ifndef _TOS_EVTDRV_GLOBAL_H_
#define  _TOS_EVTDRV_GLOBAL_H_

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

extern evtdrv_task_entry_t *evtdrv_task_table;
extern evtdrv_ttb_sz_t      evtdrv_task_table_size;

extern evtdrv_task_id_t     evtdrv_curr_task;

extern evtdrv_event_t      *evtdrv_events;

extern k_list_t             evtdrv_msg_list;

extern k_list_t             evtdrv_timer_list;

extern k_evtdrv_poll_t      evtdrv_poll;

extern evtdrv_tick_t        evtdrv_tick_count;

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

#endif /* _TOS_EVTDRV_GLOBAL_H_ */

