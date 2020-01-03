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

#include <tos_k.h>
#include <string.h>

#include "tos_k.h"
#include "qcloud.h"

static char now_time_str[20] = {0};

__QCLOUD_OSAL__ uint32_t osal_uptime_ms(void)
{
#if (TOS_CFG_CPU_TICK_PER_SECOND == 1000)
    return (uint32_t)tos_systick_get();
#else
    k_tick_t tick = 0u;

    tick = tos_systick_get() * 1000;
    return (uint32_t)((tick + TOS_CFG_CPU_TICK_PER_SECOND - 1) / TOS_CFG_CPU_TICK_PER_SECOND);
#endif
}

__QCLOUD_OSAL__ long osal_timer_current_sec(void)
{
	return osal_uptime_ms() / 1000 + 50 * 365 * 24 * 3600; // 100 years
}

__QCLOUD_OSAL__ char *osal_timer_current(void)
{
	long time_sec;

	time_sec = osal_timer_current_sec();
	memset(now_time_str, 0, 20);
	snprintf(now_time_str, 20, "%d",time_sec);

	return now_time_str;
}

__QCLOUD_OSAL__ int osal_timer_is_expired(osal_timer_t *timer)
{
    return osal_uptime_ms() > timer->end_time ? 1 : 0;
}

__QCLOUD_OSAL__ void osal_timer_countdown_ms(osal_timer_t *timer, uint32_t timeout)
{
	timer->end_time = osal_uptime_ms();
    timer->end_time += timeout;
}

__QCLOUD_OSAL__ void osal_timer_countdown(osal_timer_t *timer, uint32_t timeout)
{
    timer->end_time = osal_uptime_ms();
	timer->end_time += timeout * 1000;
}

__QCLOUD_OSAL__ uint32_t osal_timer_remain(osal_timer_t *timer)
{
    uint32_t now;

    now = osal_uptime_ms();
    if (timer->end_time <= now) {
        return 0;
    }

    return timer->end_time - now;
}

__QCLOUD_OSAL__ void osal_timer_init(osal_timer_t *timer)
{
    timer->end_time = 0;
}

#ifdef __cplusplus
}
#endif

