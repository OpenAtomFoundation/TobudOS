#include "tos_evtdrv.h"

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

evtdrv_task_entry_t    *evtdrv_task_table           = K_NULL;
evtdrv_ttb_sz_t         evtdrv_task_table_size      = 0;

evtdrv_task_id_t        evtdrv_curr_task            = TOS_EVTDRV_TASK_ID_NONE;

evtdrv_event_t         *evtdrv_events               = K_NULL;

k_evtdrv_poll_t         evtdrv_poll                 = K_NULL;

evtdrv_tick_t           evtdrv_tick_count           = (evtdrv_tick_t)0u;

TOS_LIST_DEFINE(evtdrv_msg_list);

TOS_LIST_DEFINE(evtdrv_timer_list);

#endif

