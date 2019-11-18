/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "qcloud_iot_import.h"

#include "tos.h"

static char now_time_str[20] = {0};

uint32_t HAL_GetTimeMs(void)
{
#if (TOS_CFG_CPU_TICK_PER_SECOND == 1000)
    return tos_systick_get();
#else
    k_tick_t tick = 0u;

    tick = tos_systick_get() * 1000;
    return ((tick + TOS_CFG_CPU_TICK_PER_SECOND - 1) / TOS_CFG_CPU_TICK_PER_SECOND);
#endif
}

/*Get timestamp*/
long HAL_Timer_current_sec(void)
{
	return HAL_GetTimeMs() / 1000;
}

char* HAL_Timer_current(void)
{
	long time_sec;

	time_sec = HAL_Timer_current_sec();
	memset(now_time_str, 0, 20);
	snprintf(now_time_str, 20, "%ld", time_sec);

	return now_time_str;
}

bool HAL_Timer_expired(Timer *timer)
{
    return HAL_GetTimeMs() > timer->end_time ? 1 : 0;
}

void HAL_Timer_countdown_ms(Timer *timer, unsigned int timeout_ms)
{
	timer->end_time = HAL_GetTimeMs();
    timer->end_time += timeout_ms;
}

void HAL_Timer_countdown(Timer *timer, unsigned int timeout)
{
    timer->end_time = HAL_GetTimeMs();
	timer->end_time += timeout * 1000;
}

int HAL_Timer_remain(Timer *timer)
{
    uint32_t now;

    now = HAL_GetTimeMs();
    if (timer->end_time <= now) {
        return 0;
    }

    return timer->end_time - now;
}

void HAL_Timer_init(Timer *timer)
{
      timer->end_time = 0;
}

#ifdef __cplusplus
}
#endif
