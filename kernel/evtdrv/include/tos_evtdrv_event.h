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

#ifndef _TOS_EVTDRV_EVENT_H_
#define  _TOS_EVTDRV_EVENT_H_

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

typedef uint16_t    evtdrv_event_flag_t;
typedef uint16_t    evtdrv_event_nesting_t;

#define EVTDRV_SYS_EVENT_MASK     (evtdrv_event_flag_t)(0x3 << (sizeof(evtdrv_event_flag_t) * 8 - 2))
#define EVTDRV_USR_EVENT_MASK     (evtdrv_event_flag_t)(~(EVTDRV_SYS_EVENT_MASK))

/*  highest two bits are reserved for system event
    0x8000 init all tasks
    0x4000 msg received
*/
#define TOS_EVTDRV_EVENT_NONE       (evtdrv_event_flag_t)0x0u
#define TOS_EVTDRV_SYS_EVENT_INIT   (evtdrv_event_flag_t)(0x2 << (sizeof(evtdrv_event_flag_t) * 8 - 2))
#define TOS_EVTDRV_SYS_EVENT_MSG    (evtdrv_event_flag_t)(0x1 << (sizeof(evtdrv_event_flag_t) * 8 - 2))

typedef struct evtdrv_event_st {
	evtdrv_event_flag_t		flags;
	evtdrv_event_nesting_t	nesting[sizeof(evtdrv_event_flag_t) * 8];
} evtdrv_event_t;

/**
 * @brief Set event to the target task.
 * Set event to the target task, next time the target task is "scheduled", the task will handle the event.
 *
 * @attention None
 *
 * @param[in]   task_id     id of the target task.
 * @param[in]   event_flags the event flags set to the task
 *
 * @return  errcode
 * @retval  #EVTDRV_ERR_TASK_INVALID  task id is invalid.
 * @retval  #EVTDRV_ERR_NONE          return successfully.
 */
__API__ evtdrv_err_t tos_evtdrv_event_set(evtdrv_task_id_t task_id, evtdrv_event_flag_t event_flags);

/**
 * @brief Reset the event of the target task.
 *
 * @attention None
 *
 * @param[in]   task_id     id of the target task.
 * @param[in]   event_flags the event flags reset of the task
 *
 * @return  errcode
 * @retval  #EVTDRV_ERR_TASK_INVALID  task id is invalid.
 * @retval  #EVTDRV_ERR_NONE          return successfully.
 */
__API__ evtdrv_err_t tos_evtdrv_event_reset(evtdrv_task_id_t task_id, evtdrv_event_flag_t event_flags);

__KERNEL__ evtdrv_err_t evtdrv_event_init(void);

__KERNEL__ evtdrv_event_flag_t evtdrv_event_fetch(evtdrv_task_id_t *task_id);

__STATIC_INLINE__ evtdrv_bool_t evtdrv_event_is_usr(evtdrv_event_flag_t event_flag)
{
    return event_flag & EVTDRV_USR_EVENT_MASK;
}

__STATIC_INLINE__ evtdrv_bool_t evtdrv_event_is_sys(evtdrv_event_flag_t event_flag)
{
    return event_flag & EVTDRV_SYS_EVENT_MASK;
}

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

#endif /* _TOS_EVTDRV_EVENT_H_ */

