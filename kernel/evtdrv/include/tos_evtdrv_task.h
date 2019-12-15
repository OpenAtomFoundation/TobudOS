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

#ifndef _TOS_EVTDRV_TASK_H_
#define  _TOS_EVTDRV_TASK_H_

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

#define TOS_EVTDRV_TASK_ID_NONE       ((evtdrv_task_id_t)-1)

// return event handled
typedef evtdrv_event_flag_t (*evtdrv_task_entry_t)(evtdrv_event_flag_t event_flags);

#define EVTDRV_TASK_ID2TASK(task_id)    (evtdrv_task_entry_t)(evtdrv_task_table[task_id])

/**
 * @brief Get the actived("running") task id.
 *
 * @attention
 *
 * @return  the actived("running") task id.
 */
__API__ evtdrv_task_id_t tos_evtdrv_task_self(void);

__KERNEL__ evtdrv_bool_t evtdrv_task_id_is_invalid(evtdrv_task_id_t task_id);

__KERNEL__ evtdrv_bool_t evtdrv_task_is_self(evtdrv_task_id_t task_id);

__KERNEL__ void evtdrv_task_init(void);

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

#endif /* _TOS_EVTDRV_TASK_H_ */

