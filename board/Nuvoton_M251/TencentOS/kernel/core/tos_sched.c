#include <tos.h>

__STATIC__ k_prio_t readyqueue_prio_highest_get(void)
{
    uint32_t *tbl;
    k_prio_t prio;

    prio    = 0;
    tbl     = &k_rdyq.prio_mask[0];

    while (*tbl == 0) {
        prio += K_PRIO_TBL_SLOT_SIZE;
        ++tbl;
    }
    prio += tos_cpu_clz(*tbl);
    return prio;
}

__STATIC_INLINE__ void readyqueue_prio_insert(k_prio_t prio)
{
    k_rdyq.prio_mask[K_PRIO_NDX(prio)] |= K_PRIO_BIT(prio);
}

__STATIC_INLINE__ void readyqueue_prio_remove(k_prio_t prio)
{
    k_rdyq.prio_mask[K_PRIO_NDX(prio)] &= ~K_PRIO_BIT(prio);
}

__STATIC_INLINE__ void readyqueue_prio_mark(k_prio_t prio)
{
    readyqueue_prio_insert(prio);

    if (prio < k_rdyq.highest_prio) {
        k_rdyq.highest_prio = prio;
    }
}

/**
 * when this function involved, must be at least one task in the task list of the certain priority
 */
__KERNEL__ int readyqueue_is_prio_onlyone(k_prio_t prio)
{
    k_list_t *task_list;
    k_task_t *task;

    task_list = &k_rdyq.task_list_head[prio];
    task = TOS_LIST_FIRST_ENTRY(task_list, k_task_t, pend_list);
    return task->pend_list.next == task_list;
}

__KERNEL__ k_task_t *readyqueue_first_task_get(k_prio_t prio)
{
    k_list_t *task_list;

    task_list = &k_rdyq.task_list_head[prio];
    return TOS_LIST_FIRST_ENTRY_OR_NULL(task_list, k_task_t, pend_list);
}

__KERNEL__ k_task_t *readyqueue_highest_ready_task_get(void)
{
    k_list_t *task_list;

    task_list = &k_rdyq.task_list_head[k_rdyq.highest_prio];
    return TOS_LIST_FIRST_ENTRY(task_list, k_task_t, pend_list);
}

__KERNEL__ void readyqueue_init(void)
{
    uint8_t i;

    k_rdyq.highest_prio = TOS_CFG_TASK_PRIO_MAX;

    for (i = 0; i < TOS_CFG_TASK_PRIO_MAX; ++i) {
        tos_list_init(&k_rdyq.task_list_head[i]);
    }

    for (i = 0; i < K_PRIO_TBL_SIZE; ++i) {
        k_rdyq.prio_mask[i] = 0;
    }
}

__DEBUG__ void readyqueue_walkthru(void)
{
    uint8_t i;
    k_task_t *task;
    k_list_t *task_list, *curr;

    tos_kprintf("==========================\n");
    tos_kprintf("%d\n", k_rdyq.highest_prio);

    for (i = 0; i < TOS_CFG_TASK_PRIO_MAX; ++i) {
        task_list = &k_rdyq.task_list_head[i];
        if (!tos_list_empty(task_list)) {
            TOS_LIST_FOR_EACH(curr, task_list) {
                task = TOS_LIST_ENTRY(curr, k_task_t, pend_list);
                tos_kprintf("----  %d   %d   [%d]  %s\n", task->prio, i, task->state, task->name);
            }
        }
    }
    tos_kprintf("\n\n");
}

__KERNEL__ void readyqueue_add_head(k_task_t *task)
{
    k_prio_t task_prio;
    k_list_t *task_list;

    task_prio = task->prio;
    task_list = &k_rdyq.task_list_head[task_prio];

    if (tos_list_empty(task_list)) {
        readyqueue_prio_mark(task_prio);
    }

    tos_list_add(&task->pend_list, task_list);
}

__KERNEL__ void readyqueue_add_tail(k_task_t *task)
{
    k_prio_t task_prio;
    k_list_t *task_list;

    task_prio = task->prio;
    task_list = &k_rdyq.task_list_head[task_prio];

    if (tos_list_empty(task_list)) {
        readyqueue_prio_mark(task_prio);
    }

    tos_list_add_tail(&task->pend_list, task_list);
}

__KERNEL__ void readyqueue_add(k_task_t *task)
{
    if (task->prio == k_curr_task->prio) {
        readyqueue_add_tail(task);
    } else {
        readyqueue_add_head(task);
    }
}

__KERNEL__ void readyqueue_remove(k_task_t *task)
{
    k_prio_t task_prio;
    k_list_t *task_list;

    task_prio = task->prio;
    task_list = &k_rdyq.task_list_head[task_prio];

    tos_list_del(&task->pend_list);

    if (tos_list_empty(task_list)) {
        readyqueue_prio_remove(task_prio);
    }

    if (task_prio == k_rdyq.highest_prio) {
        k_rdyq.highest_prio = readyqueue_prio_highest_get();
    }
}

__KERNEL__ void readyqueue_move_head_to_tail(k_prio_t prio)
{
    k_list_t *task_list;

    task_list = &k_rdyq.task_list_head[prio];

    if (!tos_list_empty(task_list)) {
        tos_list_move_tail(task_list->next, task_list);
    }
}

