#ifndef _TOS_EVTDRV_GLOBAL_H_
#define  _TOS_EVTDRV_GLOBAL_H_

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

extern evtdrv_task_entry_t *evtdrv_task_table;
extern evtdrv_ttb_sz_t      evtdrv_task_table_size;

extern evtdrv_task_id_t     evtdrv_curr_task;

extern evtdrv_event_t      *evtdrv_events;

extern k_list_t             evtdrv_msg_list;

extern k_list_t             evtdrv_timer_list;

extern k_evtdrv_poll_t      evtdrv_poll;

extern evtdrv_tick_t        evtdrv_tick_count;

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

#endif /* _TOS_EVTDRV_GLOBAL_H_ */

