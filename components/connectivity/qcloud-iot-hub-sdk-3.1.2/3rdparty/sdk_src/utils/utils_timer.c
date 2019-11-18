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
    
#include "utils_timer.h"
    
bool expired(Timer *timer) {
    return HAL_Timer_expired(timer);
}

void countdown_ms(Timer *timer, unsigned int timeout_ms) {
    HAL_Timer_countdown_ms(timer, timeout_ms);
}

void countdown(Timer *timer, unsigned int timeout) {
    HAL_Timer_countdown(timer, timeout);
}

int left_ms(Timer *timer) {
    return HAL_Timer_remain(timer);
}

void InitTimer(Timer *timer) {
    HAL_Timer_init(timer);
}
    
#ifdef __cplusplus
}
#endif
