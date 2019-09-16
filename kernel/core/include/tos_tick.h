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

__KERNEL__ k_err_t      tick_list_add(k_task_t *task, k_tick_t timeout);

__KERNEL__ void         tick_list_remove(k_task_t *task);

#if TOS_CFG_TICKLESS_EN > 0u
__KERNEL__ k_tick_t     tick_next_expires_get(void);
#endif

#endif /* _TOS_TICK_H_ */

