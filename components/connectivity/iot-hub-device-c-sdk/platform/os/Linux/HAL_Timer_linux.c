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

#include <sys/time.h>
#include <time.h>

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
    struct timeval  now, res;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    HAL_TIMESPEC_TO_TIMEVAL(ts, now);
    timersub(&timer->end_time, &now, &res);
    return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
}

/**
 * @brief Countdown ms.
 *
 * @param[in,out] timer @see Timer
 * @param[in] timeout_ms ms to count down
 */
void HAL_Timer_CountdownMs(Timer *timer, unsigned int timeout_ms)
{
    struct timeval  now;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    HAL_TIMESPEC_TO_TIMEVAL(ts, now);
    struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    timeradd(&now, &interval, &timer->end_time);
}

/**
 * @brief Countdown second
 *
 * @param[in,out] timer @see Timer
 * @param[in] timeout second to count down
 */
void HAL_Timer_Countdown(Timer *timer, unsigned int timeout)
{
    struct timeval  now;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    HAL_TIMESPEC_TO_TIMEVAL(ts, now);
    struct timeval interval = {timeout, 0};
    timeradd(&now, &interval, &timer->end_time);
}

/**
 * @brief Timer remain ms.
 *
 * @param[in] timer @see Timer
 * @return ms
 */
uint32_t HAL_Timer_Remain(Timer *timer)
{
    struct timeval  now, res;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    HAL_TIMESPEC_TO_TIMEVAL(ts, now);
    timersub(&timer->end_time, &now, &res);
    return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}

/**
 * @brief time format string
 *
 * @return time format string, such as "2021-05-31 15:58:46"
 */
char *HAL_Timer_Current(void)
{
    static char    time_str[20];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t now_time = tv.tv_sec;

    struct tm tm_tmp = *localtime(&now_time);
    strftime(time_str, sizeof(time_str), "%F %T", &tm_tmp);
    return time_str;
}

/**
 * @brief Get current utf timestamp of second
 *
 * @return timestamp
 */
uint32_t HAL_Timer_CurrentSec(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec;
}

/**
 * @brief Get utc time ms timestamp.
 *
 * @return timestamp
 */
uint64_t HAL_Timer_CurrentMs(void)
{
    struct timeval time_val = {0};
    uint64_t       time_ms;

    gettimeofday(&time_val, NULL);
    time_ms = time_val.tv_sec * 1000 + time_val.tv_usec / 1000;
    return time_ms;
}

/**
 * @brief Set system time using second timestamp
 *
 * @param[in] timestamp_sec timestamp to set
 * @return 0 for success
 */
int HAL_Timer_SetSystimeSec(uint32_t timestamp_sec)
{
    struct timeval stime;
    stime.tv_sec  = timestamp_sec;
    stime.tv_usec = 0;
    return settimeofday(&stime, NULL);
}

/**
 * @brief Set system time using ms timestamp
 *
 * @param[in] timestamp_ms
 * @return 0 for success
 */
int HAL_Timer_SetSystimeMs(uint64_t timestamp_ms)
{
    struct timeval stime;
    stime.tv_sec  = (timestamp_ms / 1000);
    stime.tv_usec = ((timestamp_ms % 1000) * 1000);
    return settimeofday(&stime, NULL);
}

#ifdef __cplusplus
}
#endif
