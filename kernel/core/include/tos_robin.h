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

#ifndef _TOS_ROBIN_H_
#define  _TOS_ROBIN_H_

__CDECLS_BEGIN

#if TOS_CFG_ROUND_ROBIN_EN > 0u

/**
 * @brief Set time slice.
 * Set time slice of a task.
 *
 * @attention None
 *
 * @param[in]   task        pointer to the handler of the task.
 * @param[in]   timeslice   time slice of the task
 *
 * @return  None
 */
__API__ void    tos_robin_timeslice_set(k_task_t *task, k_timeslice_t timeslice);

/**
 * @brief Configure round robin.
 * Set the default time slice of the task.
 *
 * @attention None
 *
 * @param[in]   default_timeslice   default time slice of the task.
 *
 * @return  None
 */
__API__ void    tos_robin_default_timeslice_config(k_timeslice_t default_timeslice);

__KNL__ void    robin_sched(k_prio_t prio);

#endif /* TOS_CFG_ROUND_ROBIN_EN */

__CDECLS_END

#endif /* _TOS_ROBIN_H_ */

