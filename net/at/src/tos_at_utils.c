#include "tos_k.h"
#include "tos_at.h"

void at_delay(k_tick_t tick)
{
    k_tick_t now;

    now = tos_systick_get();
    while ((tos_systick_get() - now) < tick) {
        ;
    }
}

void at_delay_ms(uint32_t millisec)
{
    k_tick_t tick;

    tick = tos_millisec2tick(millisec);
    at_delay(tick);
}

int at_timer_is_expired(at_timer_t *tmr)
{
    k_tick_t now;

    if (!tmr) {
        return -1;
    }

    now = tos_systick_get();
    return now >= tmr->end_time;
}

void at_timer_countdown(at_timer_t *tmr, k_tick_t tick)
{
    k_tick_t now;

    if (!tmr) {
        return;
    }

    now = tos_systick_get();
    tmr->end_time = now + tick;
}

void at_timer_countdown_ms(at_timer_t *tmr, uint32_t millisec)
{
    k_tick_t expire;

    if (!tmr) {
        return;
    }

    expire = tos_millisec2tick(millisec);
    at_timer_countdown(tmr, expire);
}

k_tick_t at_timer_remain(at_timer_t *tmr)
{
    k_tick_t now;

    now = tos_systick_get();

    if (at_timer_is_expired(tmr)) {
        return (k_tick_t)0u;
    }

    return tmr->end_time - now;
}

void at_timer_init(at_timer_t *tmr)
{
    if (!tmr) {
        return;
    }

    tmr->end_time = 0;
}

