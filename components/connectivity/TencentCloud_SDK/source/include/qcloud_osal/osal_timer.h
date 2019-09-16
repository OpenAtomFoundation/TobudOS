#ifndef _OSAL_TIMER_H_
#define  _OSAL_TIMER_H_

typedef struct osal_timer_st {
    uint32_t end_time;
} osal_timer_t;

__QCLOUD_OSAL__ int         osal_timer_is_expired(osal_timer_t *timer);

__QCLOUD_OSAL__ void        osal_timer_countdown_ms(osal_timer_t *timer, uint32_t timeout);

__QCLOUD_OSAL__ void        osal_timer_countdown(osal_timer_t *timer, uint32_t timeout);

__QCLOUD_OSAL__ uint32_t    osal_timer_remain(osal_timer_t *timer);

__QCLOUD_OSAL__ void        osal_timer_init(osal_timer_t *timer);

__QCLOUD_OSAL__ char       *osal_timer_current(void);

__QCLOUD_OSAL__ long        osal_timer_current_sec(void);

#endif

