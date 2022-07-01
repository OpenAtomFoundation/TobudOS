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
 * @file qcloud_iot_timer.c
 * @brief timer interface
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-04-07
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-04-07 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_timer.h"

/**
 * @brief Return if timer expired.
 *
 * @param[in] timer @see QcloudIotTimer
 * @return true expired
 * @return false no expired
 */
bool IOT_Timer_Expired(QcloudIotTimer *timer)
{
    return HAL_Timer_CurrentMs() > *timer ? 1 : 0;
}

/**
 * @brief Countdown ms.
 *
 * @param[in,out] timer @see QcloudIotTimer
 * @param[in] timeout_ms ms to count down
 */
void IOT_Timer_CountdownMs(QcloudIotTimer *timer, uint32_t timeout_ms)
{
    *timer = HAL_Timer_CurrentMs() + timeout_ms;
}

/**
 * @brief Countdown second
 *
 * @param[in,out] timer @see QcloudIotTimer
 * @param[in] timeout second to count down
 */
void IOT_Timer_Countdown(QcloudIotTimer *timer, uint32_t timeout)
{
    *timer = HAL_Timer_CurrentMs() + timeout * 1000;
}

/**
 * @brief QcloudIotTimer remain ms.
 *
 * @param[in] timer @see QcloudIotTimer
 * @return ms
 */
uint64_t IOT_Timer_Remain(QcloudIotTimer *timer)
{
    return *timer <= HAL_Timer_CurrentMs() ? 0 : *timer - HAL_Timer_CurrentMs();
}

/**
 * @brief Get current utf timestamp of second
 *
 * @return timestamp
 */
uint64_t IOT_Timer_CurrentSec(void)
{
    return HAL_Timer_CurrentMs() / 1000;
}
