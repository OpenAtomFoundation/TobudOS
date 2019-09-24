#include "tos_evtdrv.h"

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

__API__ evtdrv_task_id_t tos_evtdrv_task_self(void)
{
    return evtdrv_curr_task;
}

__KERNEL__ evtdrv_bool_t evtdrv_task_id_is_invalid(evtdrv_task_id_t task_id)
{
    return task_id >= evtdrv_task_table_size;
}

__KERNEL__ evtdrv_bool_t evtdrv_task_is_self(evtdrv_task_id_t task_id)
{
    return task_id == evtdrv_curr_task;
}

__KERNEL__ void evtdrv_task_init(void)
{
    evtdrv_task_id_t i;

    for (i = 0; i < evtdrv_task_table_size; ++i) {
        (evtdrv_task_table[i])(TOS_EVTDRV_SYS_EVENT_INIT);
    }
}

#endif

