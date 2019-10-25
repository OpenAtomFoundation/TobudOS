#ifndef _TOS_SCHED_H_
#define  _TOS_SCHED_H_

#define K_PRIO_TBL_SIZE         ((TOS_CFG_TASK_PRIO_MAX + 31) / 32)
#define K_PRIO_TBL_SLOT_SIZE    (32u)

#define K_PRIO_NDX(prio)            ((prio) >> 5u) /* prio / 32u */
#define K_PRIO_BIT(prio)            (1u << (K_PRIO_TBL_SLOT_SIZE - 1u - ((prio) & (K_PRIO_TBL_SLOT_SIZE - 1u))))

typedef struct readyqueue_st {
    k_list_t    task_list_head[TOS_CFG_TASK_PRIO_MAX];
    uint32_t    prio_mask[K_PRIO_TBL_SIZE];
    k_prio_t    highest_prio;
} readyqueue_t;

__KERNEL__ void         readyqueue_init(void);

__KERNEL__ int          readyqueue_is_prio_onlyone(k_prio_t prio);

__KERNEL__ k_task_t    *readyqueue_first_task_get(k_prio_t prio);

__KERNEL__ k_task_t    *readyqueue_highest_ready_task_get(void);

__KERNEL__ void         readyqueue_add_head(k_task_t *task);

__KERNEL__ void         readyqueue_add_tail(k_task_t *task);

__KERNEL__ void         readyqueue_add(k_task_t *task);

__KERNEL__ void         readyqueue_remove(k_task_t *task);

__KERNEL__ void         readyqueue_move_head_to_tail(k_prio_t prio);

#endif /* _TOS_SCHED_H_ */

