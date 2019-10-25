#include <tos.h>

__STATIC_INLINE__ void task_reset(k_task_t *task)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_deinit(&task->knl_obj);
#endif

    tos_list_init(&task->tick_list);
    tos_list_init(&task->pend_list);

#if TOS_CFG_MUTEX_EN > 0u
    tos_list_init(&task->mutex_own_list);
    task->prio_pending = K_TASK_PRIO_INVALID;
#endif

    task->pend_state    = PEND_STATE_NONE;
    task->pending_obj   = (pend_obj_t *)K_NULL;

#if TOS_CFG_MSG_EN > 0u
    task->msg_addr      = K_NULL;
    task->msg_size      = 0;
#endif
}

__STATIC__ void task_exit(void)
{
    tos_task_destroy(K_NULL);
}

#if TOS_CFG_MUTEX_EN > 0u
__STATIC__ k_prio_t task_highest_pending_prio_get(k_task_t *task)
{
    k_list_t *curr;
    k_mutex_t *mutex;
    k_prio_t prio, highest_prio_pending = K_TASK_PRIO_INVALID;

    TOS_LIST_FOR_EACH(curr, &task->mutex_own_list) {
        mutex   = TOS_LIST_ENTRY(curr, k_mutex_t, owner_list);
        prio    = pend_highest_prio_get(&mutex->pend_obj);
        if (prio < highest_prio_pending) {
            highest_prio_pending = prio;
        }
    }
    return highest_prio_pending;
}

__STATIC__ void task_mutex_release(k_task_t *task)
{
    k_list_t *curr, *next;

    TOS_LIST_FOR_EACH_SAFE(curr, next, &task->mutex_own_list) {
        mutex_release(TOS_LIST_ENTRY(curr, k_mutex_t, owner_list));
    }
}
#endif

__API__ k_err_t tos_task_create(k_task_t *task,
                                            char *name,
                                            k_task_entry_t entry,
                                            void *arg,
                                            k_prio_t prio,
                                            k_stack_t *stk_base,
                                            size_t stk_size,
                                            k_timeslice_t timeslice)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_IN_IRQ_CHECK();

    TOS_PTR_SANITY_CHECK(task);
    TOS_PTR_SANITY_CHECK(entry);
    TOS_PTR_SANITY_CHECK(stk_base);

    if (unlikely(stk_size < sizeof(cpu_context_t))) {
        return K_ERR_TASK_STK_SIZE_INVALID;
    }

    if (unlikely(prio == K_TASK_PRIO_IDLE && !knl_is_idle(task))) {
        return K_ERR_TASK_PRIO_INVALID;
    }

    if (unlikely(prio > K_TASK_PRIO_IDLE)) {
        return K_ERR_TASK_PRIO_INVALID;
    }

    task_reset(task);
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_init(&task->knl_obj, KNL_OBJ_TYPE_TASK);
#endif

    task->sp        = cpu_task_stk_init((void *)entry, arg, (void *)task_exit, stk_base, stk_size);
    task->entry     = entry;
    task->arg       = arg;
    task->name      = name;
    task->prio      = prio;
    task->stk_base  = stk_base;
    task->stk_size  = stk_size;

#if TOS_CFG_ROUND_ROBIN_EN > 0u
    task->timeslice_reload = timeslice;

    if (timeslice == (k_timeslice_t)0u) {
        task->timeslice = k_robin_default_timeslice;
    } else {
        task->timeslice = timeslice;
    }
#endif

    TOS_CPU_INT_DISABLE();
    task_state_set_ready(task);
    readyqueue_add_tail(task);
    TOS_CPU_INT_ENABLE();

    if (tos_knl_is_running()) {
        knl_sched();
    }

    return K_ERR_NONE;
}

__API__ k_err_t tos_task_destroy(k_task_t *task)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_IN_IRQ_CHECK();

    if (unlikely(!task)) {
        task = k_curr_task;
    }

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&task->knl_obj, KNL_OBJ_TYPE_TASK)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    if (knl_is_idle(task)) {
        return K_ERR_TASK_DESTROY_IDLE;
    }

    if (knl_is_self(task) && knl_is_sched_locked()) {
        return K_ERR_SCHED_LOCKED;
    }

    TOS_CPU_INT_DISABLE();

#if TOS_CFG_MUTEX_EN > 0u
    // when we die, wakeup all the people in this land.
    if (!tos_list_empty(&task->mutex_own_list)) {
        task_mutex_release(task);
    }
#endif

    if (task_state_is_ready(task)) { // that's simple, good kid
        readyqueue_remove(task);
    }
    if (task_state_is_sleeping(task)) {
        tick_list_remove(task);
    }
    if (task_state_is_pending(task)) {
        pend_list_remove(task);
    }

    task_reset(task);
    task_state_set_deleted(task);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ void tos_task_yield(void)
{
    TOS_CPU_CPSR_ALLOC();

    if (knl_is_inirq()) {
        return;
    }

    TOS_CPU_INT_DISABLE();

    readyqueue_remove(k_curr_task);
    readyqueue_add_tail(k_curr_task);

    TOS_CPU_INT_ENABLE();
    knl_sched();
}

__API__ k_err_t tos_task_prio_change(k_task_t *task, k_prio_t prio_new)
{
    TOS_CPU_CPSR_ALLOC();
#if TOS_CFG_MUTEX_EN > 0u
    k_prio_t highest_pending_prio;
#endif

    TOS_PTR_SANITY_CHECK(task);
    TOS_IN_IRQ_CHECK();

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&task->knl_obj, KNL_OBJ_TYPE_TASK)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    if (unlikely(prio_new >= K_TASK_PRIO_IDLE)) {
        return K_ERR_TASK_PRIO_INVALID;
    }

    TOS_CPU_INT_DISABLE();

    if (task->prio == prio_new) { // just kidding
        TOS_CPU_INT_ENABLE();
        knl_sched();
        return K_ERR_NONE;
    }

#if TOS_CFG_MUTEX_EN > 0u
    if (!tos_list_empty(&task->mutex_own_list)) {
        highest_pending_prio = task_highest_pending_prio_get(task);
        if (prio_new > highest_pending_prio) {
            task->prio_pending  = prio_new;
            prio_new            = highest_pending_prio;
        }
    }
#endif

    if (task_state_is_pending(task)) {
        task->prio = prio_new;
        pend_list_adjust(task);
    } else if (task_state_is_sleeping(task)) {
        task->prio = prio_new;
    } else if (task_state_is_ready(task)) { // good kid
        readyqueue_remove(task);

        /* ATTENTION:
            must do the prio assignment after readyqueue_remove
            otherwise the k_rdyq.highest_prio refresh in readyqueue_remove will be wrong.
         */
        task->prio = prio_new;
        if (knl_is_self(task)) {
            readyqueue_add_head(task);
        } else {
            readyqueue_add_tail(task);
        }
    }

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_task_suspend(k_task_t *task)
{
    TOS_CPU_CPSR_ALLOC();

    if (unlikely(!task)) {
        task = k_curr_task;
    }

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&task->knl_obj, KNL_OBJ_TYPE_TASK)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    if (knl_is_idle(task)) {
        return K_ERR_TASK_SUSPEND_IDLE;
    }

    if (unlikely(knl_is_self(task)) && knl_is_sched_locked()) { // if not you, who?
        return K_ERR_SCHED_LOCKED;
    }

    TOS_CPU_INT_DISABLE();

    if (task_state_is_ready(task)) { // kill the good kid
        readyqueue_remove(task);
    }
    task_state_set_suspended(task);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_task_resume(k_task_t *task)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(task);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&task->knl_obj, KNL_OBJ_TYPE_TASK)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    if (unlikely(knl_is_self(task))) {
        return K_ERR_TASK_RESUME_SELF;
    }

    TOS_CPU_INT_DISABLE();

    if (!task_state_is_suspended(task)) {
        TOS_CPU_INT_ENABLE();
        knl_sched();
        return K_ERR_NONE;
    }

    task_state_reset_suspended(task);
    if (task_state_is_ready(task)) { // we are good kid now
        readyqueue_add(task);
    }

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_task_delay(k_tick_t delay)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_IN_IRQ_CHECK();

    if (knl_is_sched_locked()) {
        return K_ERR_SCHED_LOCKED;
    }

    if (unlikely(delay == (k_tick_t)0u)) {
        tos_task_yield();
        return K_ERR_NONE;
    }

    TOS_CPU_INT_DISABLE();

    if (tick_list_add(k_curr_task, delay) != K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_DELAY_FOREVER;
    }

    readyqueue_remove(k_curr_task);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_task_delay_abort(k_task_t *task)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(task);
    TOS_IN_IRQ_CHECK();

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&task->knl_obj, KNL_OBJ_TYPE_TASK)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    if (knl_is_self(task) || !task_state_is_sleeping(task)) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_TASK_NOT_DELAY;
    }

    if (task_state_is_suspended(task)) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_TASK_SUSPENDED;
    }

    tick_list_remove(task);
    readyqueue_add(task);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u

__API__ k_err_t tos_task_stack_draught_depth(k_task_t *task, int *depth)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t rc;

    TOS_PTR_SANITY_CHECK(depth);

    if (unlikely(!task)) {
        task = k_curr_task;
    }

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&task->knl_obj, KNL_OBJ_TYPE_TASK)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();
    rc = cpu_task_stack_draught_depth(task->stk_base, task->stk_size, depth);
    TOS_CPU_INT_ENABLE();

    return rc;
}

#endif

