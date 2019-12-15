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
#include <Windows.h>
#include "qcloud_iot_import.h"

static char now_time_str[20] = {0};
   
bool HAL_Timer_expired(Timer *timer) {
	if (GetTickCount64() >= timer->end_time) {
		return true;
	}

	return false;
}

void HAL_Timer_countdown_ms(Timer *timer, unsigned int timeout_ms) {
	timer->end_time = GetTickCount64() + timeout_ms;
}

void HAL_Timer_countdown(Timer *timer, unsigned int timeout) {
	timer->end_time = GetTickCount64() + (UINT64)timeout * 1000;
}

int HAL_Timer_remain(Timer* timer) {
	UINT64 now = GetTickCount64();

	if (now >= timer->end_time) {
		return 0;
	}

	return (int)(timer->end_time - now);
}

void HAL_Timer_init(Timer *timer) {
	timer->end_time = 0;
}

char* HAL_Timer_current(void) {
	static char time_str[64] = { '\0' };
	time_t now;
	struct tm tm_val;

	time(&now);
	localtime_s(&tm_val, &now);

	snprintf(time_str, sizeof(time_str), "%04d/%02d/%02d %02d:%02d:%02d", tm_val.tm_year+1900, tm_val.tm_mon+1, tm_val.tm_mday, tm_val.tm_hour, tm_val.tm_min, tm_val.tm_sec);

	return time_str;
}

long HAL_Timer_current_sec(void) {
	return (long)(GetTickCount64() / 1000);
}
    
#ifdef __cplusplus
}
#endif
