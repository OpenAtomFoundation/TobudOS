/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

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

