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

#ifndef _TOS_TASK_H_
#define  _TOS_TASK_H_

// task state is just a flag, indicating which manager list we are in.

// ready to schedule
// a task's pend_list is in readyqueue
#define K_TASK_STATE_READY                (k_task_state_t)0x0000

// delayed, or pend for a timeout
// a task's tick_list is in k_tick_list
#define K_TASK_STATE_SLEEP                (k_task_state_t)0x0001

// pend for something
// a task's pend_list is in some pend object's list
#define K_TASK_STATE_PEND                 (k_task_state_t)0x0002

// suspended
#define K_TASK_STATE_SUSPENDED            (k_task_state_t)0x0004

// deleted
#define K_TASK_STATE_DELETED              (k_task_state_t)0x0008

// actually we don't really need those TASK_STATE below, if you understand the task state deeply, the code can be much more elegant.

// we are pending, also we are waitting for a timeout(eg. tos_sem_pend with a valid timeout, not TOS_TIME_FOREVER)
// both a task's tick_list and pend_list is not empty
#define K_TASK_STATE_PENDTIMEOUT                      (k_task_state_t)(K_TASK_STATE_PEND | K_TASK_STATE_SLEEP)

// suspended when sleeping
#define K_TASK_STATE_SLEEP_SUSPENDED                  (k_task_state_t)(K_TASK_STATE_SLEEP | K_TASK_STATE_SUSPENDED)

// suspended when pending
#define K_TASK_STATE_PEND_SUSPENDED                   (k_task_state_t)(K_TASK_STATE_PEND | K_TASK_STATE_SUSPENDED)

// suspended when pendtimeout
#define K_TASK_STATE_PENDTIMEOUT_SUSPENDED            (k_task_state_t)(K_TASK_STATE_PENDTIMEOUT | K_TASK_STATE_SUSPENDED)


// if you configure TOS_CFG_TASK_PRIO_MAX as 10, means the priority for kernel is (0 ... 9]
// the priority 9(TOS_CFG_TASK_PRIO_MAX - 1) is only for idle, so avaliable priority for you is (0 ... 8]
#define K_TASK_PRIO_IDLE                                (k_prio_t)(TOS_CFG_TASK_PRIO_MAX - (k_prio_t)1u)
#define K_TASK_PRIO_INVALID                             (k_prio_t)(TOS_CFG_TASK_PRIO_MAX)

typedef void (*k_task_entry_t)(void *arg);

typedef void (*k_task_walker_t)(k_task_t *task);

/**
 * task control block
 */
typedef struct k_task_st {
    k_stack_t          *sp;                 /**< task stack pointer. This lady always comes first, we count on her in port_s.S for context switch. */

    knl_obj_t           knl_obj;            /**< just for verification, test whether current object is really a task. */

    char               *name;               /**< task name */
    k_task_entry_t      entry;              /**< task entry */
    void               *arg;                /**< argument for task entry */
    k_task_state_t      state;              /**< just state */
    k_prio_t            prio;               /**< just priority */

    k_stack_t          *stk_base;           /**< task stack base address */
    size_t              stk_size;           /**< stack size of the task */

#if TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u
    k_list_t            dead_list;          /**< when a dynamic allocated task destroyed, we hook the task's dead_list to the k_dead_task_list */
#endif

    k_list_t            stat_list;          /**< list for hooking us to the k_stat_list */

    k_tick_t            tick_expires;       /**< if we are in k_tick_list, how much time will we wait for? */

    k_list_t            tick_list;          /**< list for hooking us to the k_tick_list */
    k_list_t            pend_list;          /**< when we are ready, our pend_list is in readyqueue; when pend, in a certain pend object's list. */

#if TOS_CFG_MUTEX_EN > 0u
    k_list_t            mutex_own_list;     /**< the list hold all the mutex we own.
                                                When we die(tos_task_destroy), we have an obligation to wakeup all the task pending for those mutexs we own;
                                                if not, those pending tasks may never get a chance to wakeup. */
    k_prio_t            prio_pending;       /*< when tos_task_prio_change called, we may be just the owner of a mutex.
                                                to avoid PRIORITY INVERSION, must make sure our priority is higher than any one who is pending for
                                                the mutex we hold. So, if the prio_new of tos_task_prio_change is not appropriate
                                                (may against the principle of PRIORITY INVERSION), we just mark the prio_new here, do the real priority
                                                change in the right time(mutex_old_owner_release) later. */
#endif

    pend_obj_t         *pending_obj;       /**< if we are pending, which pend object's list we are in? */
    pend_state_t        pend_state;         /**< why we wakeup from a pend */

#if TOS_CFG_ROUND_ROBIN_EN > 0u
    k_timeslice_t       timeslice_reload;   /**< if current time slice is used up, use time_slice_reload to reload our time slice */
    k_timeslice_t       timeslice;          /**< how much time slice left for us? */
#endif

#if (TOS_CFG_MESSAGE_QUEUE_EN > 0u) || (TOS_CFG_PRIORITY_MESSAGE_QUEUE_EN > 0u)
    void               *msg;                /**< if we pend a message queue successfully, our msg will be set by the message queue poster */
#endif

#if (TOS_CFG_MAIL_QUEUE_EN > 0u) || (TOS_CFG_PRIORITY_MAIL_QUEUE_EN > 0u)
    void               *mail;               /**< if we pend a mail queue successfully, our mail and mail_size will be set by the message queue poster */
    size_t              mail_size;
#endif

#if TOS_CFG_EVENT_EN > 0u
    k_opt_t             opt_event_pend;     /**< if we are pending an event, what's the option for the pending(TOS_OPT_EVENT_PEND_*)? */
    k_event_flag_t      flag_expect;        /**< if we are pending an event, what event flag are we pending for ? */
    k_event_flag_t     *flag_match;         /**< if we pend an event successfully, flag_match will be set by the event poster, and will be returned
                                                    by tos_event_pend to the caller */
#endif
} k_task_t;

/**
 * @brief Create a task.
 * create a task.
 *
 * @attention None
 *
 * @param[in]   task        pointer to the handler of the task.
 * @param[in]   name        name of the task.
 * @param[in]   entry       running entry of the task.
 * @param[in]   arg         argument for the entry of the task.
 * @param[in]   prio        priority of the task.
 * @param[in]   stk_base    stack base address of the task.
 * @param[in]   stk_size    stack size of the task.
 * @param[in]   timeslice   time slice of the task.
 *
 * @return  errcode
 * @retval  #K_ERR_TASK_STK_SIZE_INVALID    stack size is invalid.
 * @retval  #K_ERR_TASK_PRIO_INVALID        priority is invalid.
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_task_create(k_task_t *task,
                                            char *name,
                                            k_task_entry_t entry,
                                            void *arg,
                                            k_prio_t prio,
                                            k_stack_t *stk_base,
                                            size_t stk_size,
                                            k_timeslice_t timeslice);

/**
 * @brief Destroy a task.
 * delete a task.
 *
 * @attention None
 *
 * @param[in]   task        pointer to the handler of the task to be deleted.
 *
 * @return  errcode
 * @retval  #K_ERR_TASK_DESTROY_IDLE    attempt to destroy idle task.
 * @retval  #K_ERR_NONE                 return successfully.
 */
__API__ k_err_t tos_task_destroy(k_task_t *task);

#if TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u

/**
 * @brief Create a task with a dynamic allocated task handler and stack.
 * create a task with a dynamic allocated task handler and stack.
 *
 * @attention a task created by tos_task_create_dyn, should be destroyed by tos_task_destroy_dyn.
 * @param[out]  task        dynamic allocated task handler.
 * @param[in]   name        name of the task.
 * @param[in]   entry       running entry of the task.
 * @param[in]   arg         argument for the entry of the task.
 * @param[in]   prio        priority of the task.
 * @param[in]   stk_size    stack size of the task.
 * @param[in]   timeslice   time slice of the task.
 *
 * @return  errcode
 * @retval  #K_ERR_TASK_STK_SIZE_INVALID    stack size is invalid.
 * @retval  #K_ERR_TASK_PRIO_INVALID        priority is invalid.
 * @retval  #K_ERR_TASK_OUT_OF_MEMORY       out of memory(insufficient heap memory).
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_task_create_dyn(k_task_t **task,
                                                    char *name,
                                                    k_task_entry_t entry,
                                                    void *arg,
                                                    k_prio_t prio,
                                                    size_t stk_size,
                                                    k_timeslice_t timeslice);

/**
 * @brief Destroy a dynamic created task.
 * delete a dynamic created task.
 *
 * @attention the API to destroy a dynamic created task.
 *
 * @param[in]   task        pointer to the handler of the task to be deleted.
 *
 * @return  errcode
 * @retval  #K_ERR_TASK_DESTROY_IDLE    attempt to destroy idle task.
 * @retval  #K_ERR_NONE                 return successfully.
 */
__API__ k_err_t tos_task_destroy_dyn(k_task_t *task);

#endif

/**
 * @brief Delay current task for ticks.
 * Delay for a specified amount of ticks.
 *
 * @attention None
 *
 * @param[in]   delay       amount of ticks to delay.
 *
 * @return  errcode
 * @retval  #K_ERR_DELAY_ZERO     delay is zero.
 * @retval  #K_ERR_NONE           return successfully.
 */
__API__ k_err_t tos_task_delay(k_tick_t delay);

/**
 * @brief Resume task from delay.
 * Resume a delayed task from delay.
 *
 * @attention None
 *
 * @param[in]   task        the pointer to the handler of the task.
 *
 * @return  errcode
 * @retval  #K_ERR_TASK_NOT_DELAY         task is not delayed.
 * @retval  #K_ERR_TASK_SUSPENDED         task is suspended.
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_task_delay_abort(k_task_t *task);

/**
 * @brief Suspend a task.
 * Bring a task to sleep.
 *
 * @attention None
 *
 * @param[in]   task        pointer to the handler of the task to be resume.
 *
 * @return  errcode
 * @retval  #K_ERR_TASK_SUSPEND_IDLE  attempt to suspend idle task.
 * @retval  #K_ERR_NONE               return successfully.
 */
__API__ k_err_t tos_task_suspend(k_task_t *task);

/**
 * @brief Resume a task.
 * Bring a task to run.
 *
 * @attention None
 *
 * @param[in]   task        pointer to the handler of the task to be resume.
 *
 * @return  errcode
 * @retval  #K_ERR_TASK_RESUME_SELF   attempt to resume self-task.
 * @retval  #K_ERR_NONE               return successfully.
 */
__API__ k_err_t tos_task_resume(k_task_t *task);

/**
 * @brief Change task priority.
 * Change a priority of the task.
 *
 * @attention None
 *
 * @param[in]   task        pointer to the handler of the task to be resume.
 * @param[in]   prio_new    new priority.
 *
 * @return  errcode
 * @retval  #K_ERR_TASK_PRIO_INVALID  new priority is invalid.
 * @retval  #K_ERR_NONE               return successfully.
 */
__API__ k_err_t tos_task_prio_change(k_task_t *task, k_prio_t prio_new);

/**
 * @brief Quit schedule this time.
 * Quit the cpu this time.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  None
 */
__API__ void    tos_task_yield(void);

/**
 * @brief Get current running task.
 * Get current running task.
 *
 * @attention is kernel is not running, you'll get K_NULL
 *
 * @param   None
 *
 * @return  current running task handler
 */
__API__ k_task_t *tos_task_curr_task_get(void);


#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u

/**
 * @brief Get the maximum stack draught depth of a task.
 *
 * @attention None
 *
 * @param[in]   task        pointer to the handler of the task.
 * @param[out]  depth       task stack draught depth.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                 get depth successfully.
 * @retval  #K_ERR_TASK_STK_OVERFLOW    task stack is overflow.
 */
__API__ k_err_t tos_task_stack_draught_depth(k_task_t *task, int *depth);

#endif

/**
 * @brief Walk through all the tasks in the statistic list.
 *
 * @attention None
 *
 * @param[in]   walker      a function involved when meeting each tasks in the list.
 *
 * @return  None
 */
__API__ void tos_task_walkthru(k_task_walker_t walker);

/**
 * @brief A debug API for display all tasks information.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  None
 */
__DEBUG__ void tos_task_info_display(void);

__KERNEL__ void task_free_all(void);

__KERNEL__ __STATIC_INLINE__ int task_state_is_ready(k_task_t *task)
{
    return task->state == K_TASK_STATE_READY;
}

__KERNEL__ __STATIC_INLINE__ int task_state_is_sleeping(k_task_t *task)
{
    return task->state & K_TASK_STATE_SLEEP;
}

__KERNEL__ __STATIC_INLINE__ int task_state_is_pending(k_task_t *task)
{
    return task->state & K_TASK_STATE_PEND;
}

__KERNEL__ __STATIC_INLINE__ int task_state_is_suspended(k_task_t *task)
{
    return task->state & K_TASK_STATE_SUSPENDED;
}

__KERNEL__ __STATIC_INLINE__ void task_state_reset_pending(k_task_t *task)
{
    task->state &= ~K_TASK_STATE_PEND;
}

__KERNEL__ __STATIC_INLINE__ void task_state_reset_sleeping(k_task_t *task)
{
    task->state &= ~K_TASK_STATE_SLEEP;
}

__KERNEL__ __STATIC_INLINE__ void task_state_reset_suspended(k_task_t *task)
{
    task->state &= ~K_TASK_STATE_SUSPENDED;
}

__KERNEL__ __STATIC_INLINE__ void task_state_set_suspended(k_task_t *task)
{
    task->state |= K_TASK_STATE_SUSPENDED;
}

__KERNEL__ __STATIC_INLINE__ void task_state_set_pend(k_task_t *task)
{
    task->state |= K_TASK_STATE_PEND;
}

__KERNEL__ __STATIC_INLINE__ void task_state_set_ready(k_task_t *task)
{
    task->state = K_TASK_STATE_READY;
}

__KERNEL__ __STATIC_INLINE__ void task_state_set_deleted(k_task_t *task)
{
    task->state = K_TASK_STATE_DELETED;
}

__KERNEL__ __STATIC_INLINE__ void task_state_set_sleeping(k_task_t *task)
{
    task->state |= K_TASK_STATE_SLEEP;
}

__DEBUG__ __STATIC_INLINE__ void task_default_walker(k_task_t *task)
{
    char *state_str = "ABNORMAL";

    state_str = state_str;
    tos_kprintln("tsk name: %s", task->name);

    if (task->state == K_TASK_STATE_PENDTIMEOUT_SUSPENDED) {
        state_str = "PENDTIMEOUT_SUSPENDED";
    } else if (task->state == K_TASK_STATE_PEND_SUSPENDED) {
        state_str = "PEND_SUSPENDED";
    } else if (task->state == K_TASK_STATE_SLEEP_SUSPENDED) {
        state_str = "SLEEP_SUSPENDED";
    } else if (task->state == K_TASK_STATE_PENDTIMEOUT) {
        state_str = "PENDTIMEOUT";
    } else if (task->state == K_TASK_STATE_SUSPENDED) {
        state_str = "SUSPENDED";
    } else if (task->state == K_TASK_STATE_PEND) {
        state_str = "PEND";
    } else if (task->state == K_TASK_STATE_SLEEP) {
        state_str = "SLEEP";
    } else if (task->state == K_TASK_STATE_READY) {
        state_str = "READY";
    }
    tos_kprintln("tsk stat: %s", state_str);

    tos_kprintln("stk size: %d", task->stk_size);
    tos_kprintln("stk base: 0x%p", task->stk_base);
    tos_kprintln("stk top : 0x%p", task->stk_base + task->stk_size);
    tos_kprintf("\n");
}

#endif /* _TOS_TASK_H_ */

