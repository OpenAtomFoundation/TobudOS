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

#ifndef _TOS_EVTDRV_TIMER_H_
#define  _TOS_EVTDRV_TIMER_H_

#define EVTDRV_TIME_MILLISEC_PER_SEC        (1000u)

typedef enum evtdrv_timer_option_en {
    EVTDRV_TIMER_OPT_ONESHOT,
    EVTDRV_TIMER_OPT_PERIODIC,
} evtdrv_timer_opt_t;

typedef void (*evtdrv_timer_callback_t)(void *arg);

typedef struct evtdrv_timer_st {
    evtdrv_timer_callback_t     cb;
    void                       *cb_arg;

    k_list_t                    list;
    evtdrv_tick_t               expires;
    evtdrv_tick_t               period;

    evtdrv_timer_opt_t          opt;
} evtdrv_timer_t;

typedef struct evtdrv_timer_control_st {
    evtdrv_tick_t next_expires;
    evtdrv_tick_t list;
} evtdrv_timer_ctl_t;

/**
 * @brief Create a timer.
 * Create a timer.
 *
 * @attention
 *
 * @param[in]   tmr         pointer to the handler of the timer.
 * @param[in]   callback    callback function called when the timer expires.
 * @param[in]   cb_arg      argument for the callback.
 * @param[in]   opt         option for the function call.
 *
 * @return  errcode
 * @retval  #EVTDRV_ERR_PTR_NULL    task_id is invalid.
 * @retval  #EVTDRV_ERR_NONE        event_flags is not valid for user use.
 */
__API__ evtdrv_err_t tos_evtdrv_timer_create(evtdrv_timer_t *tmr,
                                                                evtdrv_timer_callback_t callback,
                                                                void *cb_arg,
                                                                evtdrv_timer_opt_t opt);

/**
 * @brief Start a timer.
 * Start the timer to run.
 *
 * @attention None
 *
 * @param[in]   tmr         pointer to the handler of the timer.
 *
 * @return  errcode
 * @retval  #EVTDRV_ERR_PTR_NULL                tmr is a NULL pointer.
 * @retval  #EVTDRV_ERR_TIMER_ALREADY_EXIST     tmr is already exist.
 * @retval  #EVTDRV_ERR_NONE                    return successfully.
 */
__API__ evtdrv_err_t tos_evtdrv_timer_start(evtdrv_timer_t *tmr, evtdrv_tick_t timeout);

/**
 * @brief Stop a timer.
 * Stop the timer from running.
 *
 * @attention None
 *
 * @param[in]   tmr         pointer to the handler of the timer.
 *
 * @return  errcode
 * @retval  #EVTDRV_ERR_PTR_NULL          tmr is a NULL pointer.
 * @retval  #EVTDRV_ERR_TIMER_INACTIVE    the timer is not active yet.
 * @retval  #EVTDRV_ERR_NONE              return successfully.
 */
__API__ evtdrv_err_t tos_evtdrv_timer_stop(evtdrv_timer_t *tmr);

__KNL__ void evtdrv_timer_update(void);

#endif /* _TOS_EVTDRV_TIMER_H_ */

