#include "tos.h"

#if TOS_CFG_ROUND_ROBIN_EN > 0u

__API__ void tos_robin_config(k_robin_state_t robin_state, k_timeslice_t default_timeslice)
{
    TOS_CPU_CPSR_ALLOC();
    TOS_CPU_INT_DISABLE();

    k_robin_state = robin_state;

    if (default_timeslice > (k_timeslice_t)0u) {
        k_robin_default_timeslice = default_timeslice;
    } else {
        k_robin_default_timeslice = TOS_CFG_CPU_TICK_PER_SECOND / 10;
    }
    TOS_CPU_INT_ENABLE();
}

__API__ void tos_robin_timeslice_set(k_task_t *task, k_timeslice_t timeslice)
{
    TOS_CPU_CPSR_ALLOC();

    if (!task) {
        task = k_curr_task;
    }

    TOS_CPU_INT_DISABLE();

    if (timeslice == (k_timeslice_t)0u) {
        task->timeslice_reload = k_robin_default_timeslice;
    } else {
        task->timeslice_reload = timeslice;
    }

    if (task->timeslice_reload > task->timeslice) {
        task->timeslice = task->timeslice_reload;
    }
    TOS_CPU_INT_ENABLE();
}

__KERNEL__ void robin_sched(k_prio_t prio)
{
    TOS_CPU_CPSR_ALLOC();
    k_task_t *task;

    if (k_robin_state != TOS_ROBIN_STATE_ENABLED) {
        return;
    }

    TOS_CPU_INT_DISABLE();

    task = readyqueue_first_task_get(prio);
    if (!task || knl_is_idle(task)) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    if (readyqueue_is_prio_onlyone(prio)) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    if (task->timeslice > (k_timeslice_t)0u) {
        --task->timeslice;
    }

    if (task->timeslice > (k_timeslice_t)0u) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    readyqueue_move_head_to_tail(k_curr_task->prio);

    task = readyqueue_first_task_get(prio);
    if (task->timeslice_reload == (k_timeslice_t)0u) {
        task->timeslice = k_robin_default_timeslice;
    } else {
        task->timeslice = task->timeslice_reload;
    }

    TOS_CPU_INT_ENABLE();
    knl_sched();
}

#endif

