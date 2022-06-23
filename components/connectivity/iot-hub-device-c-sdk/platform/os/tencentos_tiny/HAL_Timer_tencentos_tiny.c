/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2022 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file HAL_Timer_tencentos_tiny.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-01-24
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-01-24 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "qcloud_iot_platform.h"

#include "tos_k.h"

static uint64_t sg_time_stamp_ms;

static uint64_t _tos_get_systick_ms(void)
{
#if (TOS_CFG_CPU_TICK_PER_SECOND == 1000)
    return tos_systick_get();
#else
    k_tick_t tick = 0u;

    tick = tos_systick_get() * 1000;
    return ((tick + TOS_CFG_CPU_TICK_PER_SECOND - 1) / TOS_CFG_CPU_TICK_PER_SECOND);
#endif
}

/**
 * @brief Get utc time ms timestamp.
 *
 * @return timestamp
 */
uint64_t HAL_Timer_CurrentMs(void)
{
    return sg_time_stamp_ms + _tos_get_systick_ms();
}

/**
 * @brief Time format string.
 *
 * @return time format string
 */
char *HAL_Timer_Current(void)
{
    static char time_stamp[32];
    HAL_Snprintf(time_stamp, sizeof(time_stamp), "%llu", HAL_Timer_CurrentMs() / 1000);
    return time_stamp;
}

/**
 * @brief Set system time using ms timestamp
 *
 * @param[in] timestamp_ms
 * @return 0 for success
 */
int HAL_Timer_SetSystimeMs(uint64_t timestamp_ms)
{
    sg_time_stamp_ms = timestamp_ms - _tos_get_systick_ms();
    return 0;
}
