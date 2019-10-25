#include "tos.h"

#if TOS_CFG_SEM_EN > 0u

__API__ k_err_t tos_sem_create(k_sem_t *sem, k_sem_cnt_t init_count)
{
    TOS_PTR_SANITY_CHECK(sem);

    pend_object_init(&sem->pend_obj, PEND_TYPE_SEM);
    sem->count = init_count;

    return K_ERR_NONE;
}

__API__ k_err_t tos_sem_destroy(k_sem_t *sem)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(sem);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!pend_object_verify(&sem->pend_obj, PEND_TYPE_SEM)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    if (!pend_is_nopending(&sem->pend_obj)) {
        pend_wakeup_all(&sem->pend_obj, PEND_STATE_DESTROY);
    }

    pend_object_deinit(&sem->pend_obj);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__STATIC__ k_err_t sem_do_post(k_sem_t *sem, opt_post_t opt)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(sem);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!pend_object_verify(&sem->pend_obj, PEND_TYPE_SEM)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    if (sem->count == (k_sem_cnt_t)-1) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_SEM_OVERFLOW;
    }

    if (pend_is_nopending(&sem->pend_obj)) {
        ++sem->count;
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    pend_wakeup(&sem->pend_obj, PEND_STATE_POST, opt);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_sem_post(k_sem_t *sem)
{
    return sem_do_post(sem, OPT_POST_ONE);
}

__API__ k_err_t tos_sem_post_all(k_sem_t *sem)
{
    return sem_do_post(sem, OPT_POST_ALL);
}

__API__ k_err_t tos_sem_pend(k_sem_t *sem, k_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(sem);
    TOS_IN_IRQ_CHECK();

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!pend_object_verify(&sem->pend_obj, PEND_TYPE_SEM)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    if (sem->count > (k_sem_cnt_t)0u) {
        --sem->count;
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (timeout == TOS_TIME_NOWAIT) { // no wait, return immediately
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_NOWAIT;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_SCHED_LOCKED;
    }

    pend_task_block(k_curr_task, &sem->pend_obj, timeout);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return pend_state2errno(k_curr_task->pend_state);
}

#endif // TOS_CFG_SEM_EN

