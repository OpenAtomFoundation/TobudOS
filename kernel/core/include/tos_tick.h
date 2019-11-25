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

#ifndef _TOS_TICK_H_
#define  _TOS_TICK_H_

/**
 * @brief Systick interrupt handler.
 * systick interrupt handler.
 *
 * @attention called from the systick interrupt entrance.
 *
 * @param   None
 *
 * @return  None
 */
__API__ void    tos_tick_handler(void);

__KERNEL__ void         tick_update(k_tick_t tick);

__KERNEL__ void         tick_list_add(k_task_t *task, k_tick_t timeout);

__KERNEL__ void         tick_list_remove(k_task_t *task);

#if TOS_CFG_TICKLESS_EN > 0u
__KERNEL__ k_tick_t     tick_next_expires_get(void);
#endif

#endif /* _TOS_TICK_H_ */

