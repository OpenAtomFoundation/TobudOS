#ifndef _TOS_EVTDRV_ERR_H_
#define  _TOS_EVTDRV_ERR_H_

typedef enum evtdrv_err_en {
    EVTDRV_ERR_NONE                 = 0x0u,
    EVTDRV_ERR_MEM_ALLOC_FAILED     = 0x1u,
    EVTDRV_ERR_PTR_NULL             = 0x2u,
    EVTDRV_ERR_MSG_BUSY             = 0x3u,
    EVTDRV_ERR_TASK_INVALID         = 0x4u,
    EVTDRV_ERR_MMHEAP_NOT_ENABLED   = 0x5u,

    EVTDRV_ERR_EVENT_INVALID         = 0x10u,
    EVTDRV_ERR_EVENT_OVERFLOW       = 0x11u,

    EVTDRV_ERR_TIMER_ALREADY_EXIST   = 0x20u,
    EVTDRV_ERR_TIMER_INACTIVE       = 0x21u,
} evtdrv_err_t;

#endif

