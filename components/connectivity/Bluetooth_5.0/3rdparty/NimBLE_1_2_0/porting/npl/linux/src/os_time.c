/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "os/os.h"
#include "nimble/nimble_npl.h"

#include <unistd.h>
#include <time.h>

/**
 * Return ticks [ms] since system start as uint32_t.
 */
ble_npl_time_t
ble_npl_time_get(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)) {
        return 0;
    }
    return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}


ble_npl_error_t ble_npl_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    *out_ticks = ms;

    return BLE_NPL_OK;
}


ble_npl_error_t ble_npl_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    *out_ms = ticks;

    return BLE_NPL_OK;
}

ble_npl_time_t ble_npl_time_ms_to_ticks32(uint32_t ms)
{
    return ms;
}

uint32_t ble_npl_time_ticks_to_ms32(ble_npl_time_t ticks)
{
    return ticks;
}

void
ble_npl_time_delay(ble_npl_time_t ticks)
{
    struct timespec sleep_time;
    long ms = ble_npl_time_ticks_to_ms32(ticks);
    uint32_t s = ms / 1000;

    ms -= s * 1000;
    sleep_time.tv_sec = s;
    sleep_time.tv_nsec = ms * 1000000;

    nanosleep(&sleep_time, NULL);
}
