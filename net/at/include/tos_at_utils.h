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

