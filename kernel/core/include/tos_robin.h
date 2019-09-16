#ifndef _TOS_ROBIN_H_
#define  _TOS_ROBIN_H_

#if TOS_CFG_ROUND_ROBIN_EN > 0u

typedef enum k_robin_state_en {
    TOS_ROBIN_STATE_ENABLED,
    TOS_ROBIN_STATE_DISABLED,
} k_robin_state_t;

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
 * Set the round robin state and the default time slice of the task.
 *
 * @attention None
 *
 * @param[in]   robin_state         state of the round robin.
 * @param[in]   default_timeslice   default time slice of the task.
 *
 * @return  None
 */
__API__ void    tos_robin_config(k_robin_state_t robin_state, k_timeslice_t default_timeslice);

__KERNEL__ void robin_sched(k_prio_t prio);

#endif

#endif /* _TOS_ROBIN_H_ */

