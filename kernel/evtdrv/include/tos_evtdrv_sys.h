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

#ifndef _TOS_EVTDRV_SYS_H_
#define  _TOS_EVTDRV_SYS_H_

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

typedef void (*k_evtdrv_poll_t)(void);

/**
 * @brief Initialize the event-driven system.
 *
 * @attention event-driven is a simplified schedule model to support the none-context-swith-based multi-task programming.("the big while 1")
 * must enable TOS_CFG_MMHEAP_EN to use event-driven.
 *
 * @param[in]   tasks               array of the tasks.
 * @param[in]   task_table_size     size of the tasks.
 * @param[in]   poll                the user defined poll function.
 *
 * @return  errcode
 * @retval  #EVTDRV_ERR_MMHEAP_NOT_ENABLED    mmheap is not enabled.
 * @retval  #EVTDRV_ERR_MEM_ALLOC_FAILED      memory allocate failed.
 * @retval  #EVTDRV_ERR_NONE                  initialize successfully.
 */
__API__ evtdrv_err_t tos_evtdrv_sys_init(evtdrv_task_entry_t tasks[], evtdrv_ttb_sz_t task_table_size, k_evtdrv_poll_t poll);

/**
 * @brief Start the event-driven system.
 *
 * @attention start the event-driven multi-task "schedule".
 *
 * @return  None
 */
__API__ void tos_evtdrv_sys_start(void);

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

#endif /* _TOS_EVTDRV_SYS_H_ */

