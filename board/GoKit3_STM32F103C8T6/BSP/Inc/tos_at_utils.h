#ifndef _TOS_AT_UTILS_H_
#define  _TOS_AT_UTILS_H_

typedef struct at_timer_st {
    k_tick_t    end_time;
} at_timer_t;

void at_delay(k_tick_t tick);

void at_delay_ms(uint32_t millisec);

int at_timer_is_expired(at_timer_t *tmr);

void at_timer_countdown(at_timer_t *tmr, k_tick_t tick);

void at_timer_countdown_ms(at_timer_t *tmr, uint32_t millisec);

k_tick_t at_timer_remain(at_timer_t *tmr);

void at_timer_init(at_timer_t *tmr);

#endif

