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

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stddef.h>
#include "syscfg/syscfg.h"
#include "os/os_cputime.h"
#include "controller/ble_phy.h"
#include "controller/ble_ll.h"
#include "controller/ble_ll_xcvr.h"

#ifdef BLE_XCVR_RFCLK
int
ble_ll_xcvr_rfclk_state(void)
{
    uint32_t expiry;

    OS_ASSERT_CRITICAL();
    if (g_ble_ll_data.ll_rfclk_state == BLE_RFCLK_STATE_ON) {
        expiry = g_ble_ll_data.ll_rfclk_start_time;
        if ((int32_t)(os_cputime_get32() - expiry) >
                g_ble_ll_data.ll_xtal_ticks) {
            g_ble_ll_data.ll_rfclk_state = BLE_RFCLK_STATE_SETTLED;
        }
    }
    return g_ble_ll_data.ll_rfclk_state;
}

void
ble_ll_xcvr_rfclk_enable(void)
{
    OS_ASSERT_CRITICAL();
    if (g_ble_ll_data.ll_rfclk_state == BLE_RFCLK_STATE_OFF) {
        g_ble_ll_data.ll_rfclk_state = BLE_RFCLK_STATE_ON;
        ble_phy_rfclk_enable();
    }
}

void
ble_ll_xcvr_rfclk_disable(void)
{
    OS_ASSERT_CRITICAL();
    if (g_ble_ll_data.ll_rfclk_state != BLE_RFCLK_STATE_OFF) {
        ble_phy_rfclk_disable();
        g_ble_ll_data.ll_rfclk_state = BLE_RFCLK_STATE_OFF;
    }
}

void
ble_ll_xcvr_rfclk_stop(void)
{
    os_cputime_timer_stop(&g_ble_ll_data.ll_rfclk_timer);
    ble_ll_xcvr_rfclk_disable();
}

uint32_t
ble_ll_xcvr_rfclk_time_till_settled(void)
{
    int32_t dt;
    uint32_t rc;

    rc = 0;
    if (g_ble_ll_data.ll_rfclk_state == BLE_RFCLK_STATE_ON) {
        dt = (int32_t)(os_cputime_get32() - g_ble_ll_data.ll_rfclk_start_time);
        BLE_LL_ASSERT(dt >= 0);
        if (dt < g_ble_ll_data.ll_xtal_ticks) {
            rc = g_ble_ll_data.ll_xtal_ticks - (uint32_t)dt;
        }
    }

    return rc;
}

/**
 * Called when the timer to turn on the RF CLOCK expires. This function checks
 * the state of the clock. If the clock is off, the clock is turned on.
 * Otherwise, we just exit.
 *
 * Context: Interrupt
 *
 * @param arg
 */
void
ble_ll_xcvr_rfclk_timer_exp(void *arg)
{
    if (g_ble_ll_data.ll_rfclk_state == BLE_RFCLK_STATE_OFF) {
        ble_ll_xcvr_rfclk_start_now(os_cputime_get32());
    }
}

/**
 * This API is used to turn on the rfclock without setting the cputime timer to
 * start the clock at some later point.
 *
 * NOTE: presumes that the state of the rf clock was checked prior to calling.
 *
 * @param now
 */
void
ble_ll_xcvr_rfclk_start_now(uint32_t now)
{
    ble_ll_xcvr_rfclk_enable();
    g_ble_ll_data.ll_rfclk_start_time = now;
}

/**
 * Starts the timer that will turn the rf clock on. The 'cputime' is
 * the time at which the clock needs to be settled.
 *
 * @param cputime   Time at which rfclock should be on and settled.
 */
void
ble_ll_xcvr_rfclk_timer_start(uint32_t cputime)
{
    /*
     * If we are currently in an advertising event or a connection event,
     * no need to start the cputime timer
     */
    if ((g_ble_ll_data.ll_state == BLE_LL_STATE_ADV) ||
        (g_ble_ll_data.ll_state == BLE_LL_STATE_CONNECTION)) {
        return;
    }

    /* Account for the settling time */
    cputime -= g_ble_ll_data.ll_xtal_ticks;

    /*
     * If the timer is on the list, we need to see if its expiry is before
     * 'cputime'. If the expiry is before, no need to do anything. If it
     * is after, we need to stop the timer and start at new time.
     */
    if (g_ble_ll_data.ll_rfclk_timer.link.tqe_prev != NULL) {
        if ((int32_t)(cputime - g_ble_ll_data.ll_rfclk_timer.expiry) >= 0) {
            return;
        }
        os_cputime_timer_stop(&g_ble_ll_data.ll_rfclk_timer);
    }
    os_cputime_timer_start(&g_ble_ll_data.ll_rfclk_timer, cputime);
}
#endif
