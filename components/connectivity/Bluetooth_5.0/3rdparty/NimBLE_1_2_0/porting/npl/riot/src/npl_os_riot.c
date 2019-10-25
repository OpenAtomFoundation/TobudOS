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

#include <stddef.h>
#include "nimble/nimble_npl.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

volatile int ble_npl_in_critical = 0;

static void
_callout_fire(void *arg)
{
    struct ble_npl_callout *co = (struct ble_npl_callout *)arg;
    event_post(co->q, &co->e.e.super);
}

ble_npl_error_t
ble_npl_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout)
{
    int rc;
    struct timespec abs;
    uint64_t time;

    time = xtimer_now_usec64() + ble_npl_time_ticks_to_ms32(timeout) * 1000;
    abs.tv_sec = (time_t)(time / 1000000);
    abs.tv_nsec = (long)((time % 1000000) * 1000);

    rc = sem_timedwait(&sem->sem, &abs);

    return rc == 0 ? BLE_NPL_OK : BLE_NPL_ENOENT;
}

void
ble_npl_callout_init(struct ble_npl_callout *c, struct ble_npl_eventq *evq,
                     ble_npl_event_fn *ev_cb, void *ev_arg)
{
    c->timer.arg = (void *)c;
    c->timer.callback = _callout_fire;
    c->q = &evq->q;
    ble_npl_event_init(&c->e, ev_cb, ev_arg);
}

ble_npl_error_t
ble_npl_callout_reset(struct ble_npl_callout *c, ble_npl_time_t ticks)
{
    uint32_t now = xtimer_now_usec();
    c->target_ticks = (ble_npl_time_t)((now / 1000) + ticks);
    xtimer_set(&c->timer, ((ticks * 1000) - ((xtimer_now_usec() - now) / 1000)));
    return BLE_NPL_OK;
}

uint32_t
ble_npl_callout_remaining_ticks(struct ble_npl_callout *co,
                                ble_npl_time_t time)
{
    uint32_t now = xtimer_now_usec();
    return ((uint32_t)co->target_ticks) - (now / 1000);
}
