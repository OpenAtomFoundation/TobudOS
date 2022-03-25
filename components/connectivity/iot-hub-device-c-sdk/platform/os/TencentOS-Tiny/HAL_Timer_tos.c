/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
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
 * @file HAL_Timer_linux.c
 * @brief Linux timer function
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-31
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-31 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifdef __cplusplus
extern "C" {
#endif
    
#include "tos_k.h"

#include "qcloud_iot_platform.h"

#define HAL_TIMESPEC_TO_TIMEVAL(ts, tv) \
    {                                   \
        tv.tv_sec  = ts.tv_sec;         \
        tv.tv_usec = ts.tv_nsec / 1000; \
    }

/**
 * @brief Return if timer expired.
 *
 * @param[in] timer @see Timer
 * @return true expired
 * @return false no expired
 */
bool HAL_Timer_Expired(Timer *timer)
{
    k_tick_t now;
    
    now = tos_systick_get();
    
    return now >= timer->end_time ? true : false;
}

/**
 * @brief Countdown ms.
 *
 * @param[in,out] timer @see Timer
 * @param[in] timeout_ms ms to count down
 */
void HAL_Timer_CountdownMs(Timer *timer, unsigned int timeout_ms)
{
    k_tick_t tick;
    k_tick_t now;
    
    tick = tos_millisec2tick(timeout_ms);
    now = tos_systick_get();
    
    timer->end_time = now + tick;

    return;
}

/**
 * @brief Countdown second
 *
 * @param[in,out] timer @see Timer
 * @param[in] timeout second to count down
 */
void HAL_Timer_Countdown(Timer *timer, unsigned int timeout)
{
    k_tick_t now;
    k_tick_t tick;
    
    tick = timeout * TOS_CFG_CPU_TICK_PER_SECOND;
    now = tos_systick_get();
    
    timer->end_time = now + tick;
    
    return;
}

/**
 * @brief Timer remain ms.
 *
 * @param[in] timer @see Timer
 * @return ms
 */
uint32_t HAL_Timer_Remain(Timer *timer)
{
    k_tick_t now;
    
    now = tos_systick_get();
    if (now >= timer->end_time) {
        return 0;
    }
    
    now = tos_systick_get();
    
    return (k_time_t)(((timer->end_time) - now + TOS_CFG_CPU_TICK_PER_SECOND - 1) / TOS_CFG_CPU_TICK_PER_SECOND);
}

/**
 * @brief time format string
 *
 * @return time format string, such as "2021-05-31 15:58:46"
 */
char *HAL_Timer_Current(void)
{
    return NULL;
}

/**
 * @brief Get current utf timestamp of second
 *
 * @return timestamp
 */
uint32_t HAL_Timer_CurrentSec(void)
{
    return 0;
}

/**
 * @brief Get utc time ms timestamp.
 *
 * @return timestamp
 */
uint64_t HAL_Timer_CurrentMs(void)
{
    return 0;
}

/**
 * @brief Set system time using second timestamp
 *
 * @param[in] timestamp_sec timestamp to set
 * @return 0 for success
 */
int HAL_Timer_SetSystimeSec(uint32_t timestamp_sec)
{
    return 0;
}

/**
 * @brief Set system time using ms timestamp
 *
 * @param[in] timestamp_ms
 * @return 0 for success
 */
int HAL_Timer_SetSystimeMs(uint64_t timestamp_ms)
{
    return 0;
}

#ifdef __cplusplus
}
#endif
