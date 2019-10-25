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

bool
ble_npl_os_started(void)
{
    return 0;
}

void *
ble_npl_get_current_task_id(void)
{
    return NULL;
}

void
ble_npl_eventq_init(struct ble_npl_eventq *evq)
{

}

struct ble_npl_event *
ble_npl_eventq_get(struct ble_npl_eventq *evq, ble_npl_time_t tmo)
{
}

void
ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
}

void
ble_npl_eventq_remove(struct ble_npl_eventq *evq,
                      struct ble_npl_event *ev)
{

}

void
ble_npl_event_run(struct ble_npl_event *ev)
{

}

void
ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn,
                   void *arg)
{

}

bool ble_npl_event_is_queued(struct ble_npl_event *ev)
{
    return false;
}

void *
ble_npl_event_get_arg(struct ble_npl_event *ev)
{
    return NULL;
}

void
ble_npl_event_set_arg(struct ble_npl_event *ev, void *arg)
{

}

ble_npl_error_t
ble_npl_mutex_init(struct ble_npl_mutex *mu)
{
    return BLE_NPL_ENOENT;
}

ble_npl_error_t
ble_npl_mutex_pend(struct ble_npl_mutex *mu, ble_npl_time_t timeout)
{
    return BLE_NPL_ENOENT;
}

ble_npl_error_t
ble_npl_mutex_release(struct ble_npl_mutex *mu)
{
    return BLE_NPL_ENOENT;
}

ble_npl_error_t
ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    return BLE_NPL_ENOENT;
}

ble_npl_error_t
ble_npl_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout)
{
    return BLE_NPL_ENOENT;
}

ble_npl_error_t
ble_npl_sem_release(struct ble_npl_sem *sem)
{
    return BLE_NPL_ENOENT;
}

uint16_t
ble_npl_sem_get_count(struct ble_npl_sem *sem)
{
    return 0;
}

void
ble_npl_callout_init(struct ble_npl_callout *c, struct ble_npl_eventq *evq,
                     ble_npl_event_fn *ev_cb, void *ev_arg)
{
}

ble_npl_error_t
ble_npl_callout_reset(struct ble_npl_callout *c, ble_npl_time_t ticks)
{
    return BLE_NPL_ENOENT;
}

void
ble_npl_callout_stop(struct ble_npl_callout *co)
{

}

bool
ble_npl_callout_is_active(struct ble_npl_callout *c)
{
    return false;
}

ble_npl_time_t
ble_npl_callout_get_ticks(struct ble_npl_callout *co)
{
    return 0;
}

uint32_t
ble_npl_time_get(void)
{
    return 0;
}

ble_npl_error_t
ble_npl_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    return BLE_NPL_ENOENT;
}

ble_npl_error_t
ble_npl_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    return BLE_NPL_ENOENT;
}

ble_npl_time_t
ble_npl_time_ms_to_ticks32(uint32_t ms)
{
    return 0;
}

uint32_t
ble_npl_time_ticks_to_ms32(ble_npl_time_t ticks)
{
    return 0;
}

uint32_t
ble_npl_hw_enter_critical(void)
{
    return 0;
}

void
ble_npl_hw_exit_critical(uint32_t ctx)
{

}
