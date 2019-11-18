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
    
#include <time.h>
#include <sys/time.h>
    
#include "qcloud_iot_import.h"

static char now_time_str[20] = {0};
  
bool HAL_Timer_expired(Timer *timer) {
    struct timeval now, res;
    gettimeofday(&now, NULL);
    timersub(&timer->end_time, &now, &res);
    return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
}

void HAL_Timer_countdown_ms(Timer *timer, unsigned int timeout_ms) {
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    timeradd(&now, &interval, &timer->end_time);
}

void HAL_Timer_countdown(Timer *timer, unsigned int timeout) {
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval interval = {timeout, 0};
    timeradd(&now, &interval, &timer->end_time);
}

int HAL_Timer_remain(Timer *timer) {
    struct timeval now, res;
    gettimeofday(&now, NULL);
    timersub(&timer->end_time, &now, &res);
    return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}

void HAL_Timer_init(Timer *timer) {
    timer->end_time = (struct timeval) {0, 0};
}

char* HAL_Timer_current(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_t now_time = tv.tv_sec;

	struct tm tm_tmp = *localtime(&now_time);
	strftime(now_time_str, 20, "%F %T", &tm_tmp);

	return now_time_str;
}

long HAL_Timer_current_sec(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return tv.tv_sec;
}
    
#ifdef __cplusplus
}
#endif
