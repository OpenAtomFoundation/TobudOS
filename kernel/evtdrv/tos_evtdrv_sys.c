#include "tos_evtdrv.h"

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

__API__ evtdrv_err_t tos_evtdrv_sys_init(evtdrv_task_entry_t tasks[], evtdrv_ttb_sz_t task_table_size, k_evtdrv_poll_t poll)
{
    evtdrv_err_t err;

    evtdrv_task_table       = &tasks[0];
    evtdrv_task_table_size  = task_table_size;

    evtdrv_poll                 = poll;

#if TOS_CFG_MMHEAP_EN > 0
    mmheap_init(k_mmheap_pool, TOS_CFG_MMHEAP_POOL_SIZE);
#else
    return EVTDRV_ERR_MMHEAP_NOT_ENABLED;
#endif

    err = evtdrv_event_init();
    if (err != EVTDRV_ERR_NONE) {
        return err;
    }

    evtdrv_msg_init();

    evtdrv_task_init();

    return EVTDRV_ERR_NONE;
}

__STATIC__ void evtdrv_sys_run(void)
{
    evtdrv_event_flag_t event_flag = TOS_EVTDRV_EVENT_NONE;
    evtdrv_task_id_t task_id = TOS_EVTDRV_TASK_ID_NONE;

    evtdrv_timer_update();

    if (evtdrv_poll) {
        evtdrv_poll();
    }

    event_flag = evtdrv_event_fetch(&task_id);
    if (event_flag != TOS_EVTDRV_EVENT_NONE) {
        evtdrv_curr_task = task_id;
        event_flag = (EVTDRV_TASK_ID2TASK(task_id))(event_flag);
        evtdrv_curr_task = TOS_EVTDRV_TASK_ID_NONE;

        // after task process, event that the task handled is returned.
        tos_evtdrv_event_reset(task_id, event_flag);
    } else {
#if TOS_CFG_PWR_MGR_EN > 0u
        pm_power_manager();
#endif
    }
}

__API__ void tos_evtdrv_sys_start(void)
{
    while (K_TRUE) {
        evtdrv_sys_run();
    }
}

#endif

