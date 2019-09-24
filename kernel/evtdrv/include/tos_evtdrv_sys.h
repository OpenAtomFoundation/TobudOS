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

