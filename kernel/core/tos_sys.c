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

#include <tos.h>

__API__ k_err_t tos_knl_init(void)
{
    k_err_t err;

    cpu_init();

    readyqueue_init();

#if TOS_CFG_MMHEAP_EN > 0
#if TOS_CFG_MMHEAP_DEFAULT_POOL_EN > 0u
    err = mmheap_init_with_pool(k_mmheap_default_pool, TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE);
#else
    err = mmheap_init();
#endif
    if (err != K_ERR_NONE) {
        return err;
    }
#endif

    err = knl_idle_init();
    if (err != K_ERR_NONE) {
        return err;
    }

#if TOS_CFG_TIMER_EN > 0
    err = timer_init();
    if (err != K_ERR_NONE) {
        return err;
    }
#endif

#if TOS_CFG_PWR_MGR_EN > 0U
    pm_init();
#endif

#if TOS_CFG_TICKLESS_EN > 0u
    tickless_init();
#endif

    return K_ERR_NONE;
}

__API__ void tos_knl_irq_enter(void)
{
    if (!tos_knl_is_running()) {
        return;
    }

    if (unlikely(k_irq_nest_cnt >= K_NESTING_LIMIT_IRQ)) {
        return;
    }

    ++k_irq_nest_cnt;
}

__API__ void tos_knl_irq_leave(void)
{
    TOS_CPU_CPSR_ALLOC();

    if (!tos_knl_is_running()) {
        return;
    }

    TOS_CPU_INT_DISABLE();
    if (!knl_is_inirq()) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    --k_irq_nest_cnt;

    if (knl_is_inirq()) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    k_next_task = readyqueue_highest_ready_task_get();
    if (knl_is_self(k_next_task)) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    cpu_irq_context_switch();
    TOS_CPU_INT_ENABLE();
}

__API__ k_err_t tos_knl_sched_lock(void)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_IN_IRQ_CHECK();

    if (!tos_knl_is_running()) {
        return K_ERR_KNL_NOT_RUNNING;
    }

    if (k_sched_lock_nest_cnt >= K_NESTING_LIMIT_SCHED_LOCK) {
        return K_ERR_LOCK_NESTING_OVERFLOW;
    }

    TOS_CPU_INT_DISABLE();
    ++k_sched_lock_nest_cnt;
    TOS_CPU_INT_ENABLE();
    return K_ERR_NONE;
}

__API__ k_err_t tos_knl_sched_unlock(void)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_IN_IRQ_CHECK();

    if (!tos_knl_is_running()) {
        return K_ERR_KNL_NOT_RUNNING;
    }

    if (!knl_is_sched_locked()) {
        return K_ERR_SCHED_NOT_LOCKED;
    }

    TOS_CPU_INT_DISABLE();
    --k_sched_lock_nest_cnt;
    TOS_CPU_INT_ENABLE();

    knl_sched();
    return K_ERR_NONE;
}

__API__ k_err_t tos_knl_start(void)
{
    if (tos_knl_is_running()) {
        return K_ERR_KNL_RUNNING;
    }

    k_next_task = readyqueue_highest_ready_task_get();
    k_curr_task = k_next_task;
    k_knl_state = KNL_STATE_RUNNING;
    cpu_sched_start();

    return K_ERR_NONE;
}

__API__ int tos_knl_is_running(void)
{
    return k_knl_state == KNL_STATE_RUNNING;
}

#if TOS_CFG_TICKLESS_EN > 0u

/**
 * @brief Get the remain ticks of the first oncoming task.
 *
 * @return The remian ticks of the first oncoming task to be scheduled.
 */
__KERNEL__ k_tick_t knl_next_expires_get(void)
{
    k_tick_t tick_next_expires;
#if TOS_CFG_TIMER_EN > 0u
    k_tick_t timer_next_expires;
#endif

    tick_next_expires = tick_next_expires_get();

#if TOS_CFG_TIMER_EN > 0u
    timer_next_expires = timer_next_expires_get();
#endif

#if TOS_CFG_TIMER_EN > 0u
    return tick_next_expires < timer_next_expires ? tick_next_expires : timer_next_expires;
#else
    return tick_next_expires;
#endif
}

#endif

#if TOS_CFG_OBJECT_VERIFY_EN > 0u

__KERNEL__ int knl_object_verify(knl_obj_t *knl_obj, knl_obj_type_t type)
{
    return knl_obj->type == type;
}

__KERNEL__ void knl_object_init(knl_obj_t *knl_obj, knl_obj_type_t type)
{
    knl_obj->type = type;
}

__KERNEL__ void knl_object_deinit(knl_obj_t *knl_obj)
{
    knl_obj->type = KNL_OBJ_TYPE_NONE;
}

#endif

#if TOS_CFG_MMHEAP_EN > 0u

__KERNEL__ void knl_object_alloc_reset(knl_obj_t *knl_obj)
{
    knl_obj->alloc_type = KNL_OBJ_ALLOC_TYPE_NONE;
}

__KERNEL__ void knl_object_alloc_set_dynamic(knl_obj_t *knl_obj)
{
    knl_obj->alloc_type = KNL_OBJ_ALLOC_TYPE_DYNAMIC;
}

__KERNEL__ void knl_object_alloc_set_static(knl_obj_t *knl_obj)
{
    knl_obj->alloc_type = KNL_OBJ_ALLOC_TYPE_STATIC;
}

__KERNEL__ int knl_object_alloc_is_dynamic(knl_obj_t *knl_obj)
{
    return knl_obj->alloc_type == KNL_OBJ_ALLOC_TYPE_DYNAMIC;
}

__KERNEL__ int knl_object_alloc_is_static(knl_obj_t *knl_obj)
{
    return knl_obj->alloc_type == KNL_OBJ_ALLOC_TYPE_STATIC;
}

#endif

__KERNEL__ void knl_sched(void)
{
    TOS_CPU_CPSR_ALLOC();

    if (knl_is_inirq()) {
        return;
    }

    if (knl_is_sched_locked()) {
        return;
    }

    TOS_CPU_INT_DISABLE();
    k_next_task = readyqueue_highest_ready_task_get();
    if (knl_is_self(k_next_task)) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    cpu_context_switch();
    TOS_CPU_INT_ENABLE();
}

__KERNEL__ int knl_is_sched_locked(void)
{
    return k_sched_lock_nest_cnt > 0u;
}

__KERNEL__ int knl_is_inirq(void)
{
    return k_irq_nest_cnt > 0u;
}

__KERNEL__ int knl_is_idle(k_task_t *task)
{
    return task == &k_idle_task;
}

__KERNEL__ int knl_is_self(k_task_t *task)
{
    return task == k_curr_task;
}

__STATIC__ void knl_idle_entry(void *arg)
{
    arg = arg; // make compiler happy

    while (K_TRUE) {
#if TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u
        task_free_all();
#endif

#if TOS_CFG_PWR_MGR_EN > 0u
        pm_power_manager();
#endif
    }
}

__KERNEL__ k_err_t knl_idle_init(void)
{
    return tos_task_create(&k_idle_task, "idle",
            knl_idle_entry, K_NULL,
            K_TASK_PRIO_IDLE,
            k_idle_task_stk_addr,
            k_idle_task_stk_size,
            0);
}

