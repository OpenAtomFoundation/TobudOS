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

uint32_t HAL_UptimeMs(void)
{
#if (TOS_CFG_CPU_TICK_PER_SECOND == 1000)
    return (uint32_t)tos_systick_get();
#else
    k_tick_t tick = 0u;

    tick = tos_systick_get() * 1000;
    return (uint32_t)((tick + TOS_CFG_CPU_TICK_PER_SECOND - 1) / TOS_CFG_CPU_TICK_PER_SECOND);
#endif
}

#ifdef __cplusplus
}
#endif

