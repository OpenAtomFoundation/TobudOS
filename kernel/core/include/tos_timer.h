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

#ifndef _TOS_TIMER_H_
#define  _TOS_TIMER_H_

#if TOS_CFG_TIMER_EN > 0u

// if we just want the timer to run only once, this option should be passed to tos_timer_create.
#define TOS_OPT_TIMER_ONESHOT                   (k_opt_t)(0x0001u)

// if we want the timer run periodically, this option should be passed to tos_timer_create.
#define TOS_OPT_TIMER_PERIODIC                  (k_opt_t)(0x0002u)

typedef enum timer_change_type_en {
    TIMER_CHANGE_TYPE_DELAY,
    TIMER_CHANGE_TYPE_PERIOD,
} timer_change_type_t;

/**
 * state for timer
 */
typedef enum timer_state_en {
    TIMER_STATE_UNUSED,     /**< the timer has been destroyed */
    TIMER_STATE_STOPPED,    /**< the timer has been created but not been started, or just be stopped(tos_timer_stop) */
    TIMER_STATE_RUNNING,    /**< the timer has been created and been started */
    TIMER_STATE_COMPLETED   /**< the timer has finished its expires, it can only happen when the timer's opt is TOS_OPT_TIMER_ONESHOT */
} timer_state_t;

// callback function type for a timer
typedef void (*k_timer_callback_t)(void *arg);

/**
 * timer control block
 */
typedef struct k_timer_st {
    knl_obj_t               knl_obj;    /**< just for verification, test whether current object is really a timer */

    k_timer_callback_t      cb;         /**< callback when time is up */
    void                   *cb_arg;     /**< argument for callback */
    k_list_t                list;       /**< list for hooking us to the k_tick_list */
    k_tick_t                expires;    /**< how much time left until time expires */
    k_tick_t                delay;      /**< how much time from now to begin the first run of the timer */
    k_tick_t                period;     /**< if the time expires, how much time after should we begin the next round */
    k_opt_t                 opt;        /**< option for the timer, see TOS_OPT_TIMER_* */
    timer_state_t           state;      /**< state for the timer, see TIMER_STATE_* */
} k_timer_t;

typedef struct timer_control_st {
    k_tick_t    next_expires;
    k_list_t    list;
} timer_ctl_t;

/**
 * @brief Create a timer.
 * Create a timer.
 *
 * @attention I dont't think a timer need a name. If you do, help yourself.
 *
 * @param[in]   tmr         pointer to the handler of the timer.
 * @param[in]   delay       time interval for a timer to run.
 * @param[in]   period      period for a timer to restart to run.
 * @param[in]   callback    callback function called when the timer expires.
 * @param[in]   cb_arg      argument for the callback.
 * @param[in]   opt         option for the function call.
 *
 * @return  errcode
 * @retval  #K_ERR_TIMER_INVALID_PERIOD   period is invalid.
 * @retval  #K_ERR_TIMER_INVALID_DELAY    delay is invalid.
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_timer_create(k_timer_t *tmr, k_tick_t delay, k_tick_t period,
                                        k_timer_callback_t callback, void *cb_arg, k_opt_t opt);

/**
 * @brief Delete a timer.
 * Delete the timer.
 *
 * @attention None
 *
 * @param[in]   tmr         pointer to the handler of the timer.
 *
 * @return  errcode
 * @retval  #K_ERR_TIMER_INACTIVE         the timer is not active yet.
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_timer_destroy(k_timer_t *tmr);

/**
 * @brief Start a timer.
 * Start the timer to run.
 *
 * @attention None
 *
 * @param[in]   tmr         pointer to the handler of the timer.
 *
 * @return  errcode
 * @retval  #K_ERR_TIMER_INACTIVE         the timer is not active yet.
 * @retval  #K_ERR_TIMER_INVALID_STATE    state of the timer is invalid.
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_timer_start(k_timer_t *tmr);

/**
 * @brief Stop a timer.
 * Stop the timer from running.
 *
 * @attention None
 *
 * @param[in]   tmr         pointer to the handler of the timer.
 *
 * @return  errcode
 * @retval  #K_ERR_TIMER_INACTIVE       the timer is not active yet.
 * @retval  #K_ERR_TIMER_STOPPED        the timer is already stoppped.
 * @retval  #K_ERR_NONE                 return successfully.
 */
__API__ k_err_t tos_timer_stop(k_timer_t *tmr);

/**
 * @brief Change a timer's delay.
 *
 * @attention None
 *
 * @param[in]   tmr         pointer to the handler of the timer.
 * @param[in]   delay       new delay of the timer.
 *
 * @return  errcode
 * @retval  #K_ERR_TIMER_INACTIVE       the timer is not active yet.
 * @retval  #K_ERR_TIMER_RUNNING        the timer is running.
 * @retval  #K_ERR_TIMER_INVALID_DELAY  the delay is invalid.
 * @retval  #K_ERR_NONE                 return successfully.
 */
__API__ k_err_t tos_timer_delay_change(k_timer_t *tmr, k_tick_t delay);

/**
 * @brief Change a timer's period.
 *
 * @attention None
 *
 * @param[in]   tmr         pointer to the handler of the timer.
 * @param[in]   period      new period of the timer.
 *
 * @return  errcode
 * @retval  #K_ERR_TIMER_INACTIVE       the timer is not active yet.
 * @retval  #K_ERR_TIMER_RUNNING        the timer is running.
 * @retval  #K_ERR_TIMER_INVALID_PERIOD the period is invalid.
 * @retval  #K_ERR_NONE                 return successfully.
 */
__API__ k_err_t tos_timer_period_change(k_timer_t *tmr, k_tick_t period);


#if TOS_CFG_TIMER_AS_PROC > 0u

/**
 * @brief Timer update function.
 * When enable timer as a process function not a task, this will be the function entry.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  None
 */
__KERNEL__ void     timer_update(void);

#endif

__KERNEL__ k_err_t  timer_init(void);

__KERNEL__ k_tick_t timer_next_expires_get(void);

#endif

#endif /* _TOS_TIMER_H_ */

