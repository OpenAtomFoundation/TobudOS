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
#include <stddef.h>
#include <string.h>
#include "nimble/nimble_npl.h"

volatile int ble_npl_in_critical = 0;

void
npl_tencentos_tiny_eventq_init(struct ble_npl_eventq *evq)
{
    void *msg_pool = tos_mmheap_alloc(sizeof(void *));
    if (!msg_pool) {
        return;
    }

#define EVENT_Q_MSG_CNT     32
    tos_msg_q_create(&evq->q, msg_pool, EVENT_Q_MSG_CNT);
}

struct ble_npl_event *
npl_tencentos_tiny_eventq_get(struct ble_npl_eventq *evq, ble_npl_time_t tmo)
{
    struct ble_npl_event *ev = NULL;

    if (tos_msg_q_pend(&evq->q, (void **)&ev, tmo) != K_ERR_NONE) {
        return NULL;
    }

    if (ev) {
        ev->queued = false;
    }

    return ev;
}

void
npl_tencentos_tiny_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    k_err_t err;

    if (ev->queued) {
        return;
    }

    ev->queued = true;

    err = tos_msg_q_post(&evq->q, ev);

    assert(err == K_ERR_NONE);
}

void
npl_tencentos_tiny_eventq_remove(struct ble_npl_eventq *evq,
                      struct ble_npl_event *ev)
{
    k_err_t err;
    int i, count;
    struct ble_npl_event *ev_received;
    TOS_CPU_CPSR_ALLOC();

    if (!ev->queued) {
        return;
    }

    /*
     * XXX We cannot extract element from inside TencentOS tiny message queue so as a quick
     * workaround we'll just remove all elements and add them back except the
     * one we need to remove. This is silly, but works for now
     */

    TOS_CPU_INT_DISABLE();

    count = evq->q.ring_q.total;
    for (i = 0; i < count; ++i) {
        err = tos_msg_q_pend(&evq->q, (void **)&ev_received, TOS_TIME_NOWAIT);
        TOS_ASSERT(err == K_ERR_NONE);
        if (ev_received == ev) {
            continue;
        }

        err = tos_msg_q_post(&evq->q, ev_received);
        TOS_ASSERT(err == K_ERR_NONE);
    }

    TOS_CPU_INT_ENABLE();

    ev->queued = false;
}

bool
npl_tencentos_tiny_eventq_is_empty(struct ble_npl_eventq *evq)
{
    return tos_ring_q_is_empty(&evq->q.ring_q);
}

ble_npl_error_t
npl_tencentos_tiny_mutex_init(struct ble_npl_mutex *mu)
{
    k_err_t err;

    if (!mu) {
        return BLE_NPL_INVALID_PARAM;
    }

    err = tos_mutex_create(&mu->handle);

    return err == K_ERR_NONE ? BLE_NPL_OK : BLE_NPL_ERROR;
}

ble_npl_error_t
npl_tencentos_tiny_mutex_pend(struct ble_npl_mutex *mu, ble_npl_time_t timeout)
{
    k_err_t err;

    if (!mu) {
        return BLE_NPL_INVALID_PARAM;
    }

    err = tos_mutex_pend_timed(&mu->handle, timeout);

    return err == K_ERR_NONE ? BLE_NPL_OK : BLE_NPL_TIMEOUT;
}

ble_npl_error_t
npl_tencentos_tiny_mutex_release(struct ble_npl_mutex *mu)
{
    k_err_t err;

    if (!mu) {
        return BLE_NPL_INVALID_PARAM;
    }


    err = tos_mutex_post(&mu->handle);

    return err == BLE_NPL_OK ? BLE_NPL_OK : BLE_NPL_ERROR;
}

ble_npl_error_t
npl_tencentos_tiny_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    k_err_t err;

    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    err = tos_sem_create(&sem->handle, tokens);

    return err == K_ERR_NONE ? BLE_NPL_OK : BLE_NPL_ERROR;
}

ble_npl_error_t
npl_tencentos_tiny_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout)
{
    k_err_t err;

    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    err = tos_sem_pend(&sem->handle, timeout);

    return err == K_ERR_NONE ? BLE_NPL_OK : BLE_NPL_TIMEOUT;
}

ble_npl_error_t
npl_tencentos_tiny_sem_release(struct ble_npl_sem *sem)
{
    k_err_t err;

    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    err = tos_sem_post(&sem->handle);

    return err == K_ERR_NONE ? BLE_NPL_OK : BLE_NPL_ERROR;
}

uint16_t
npl_tencentos_tiny_sem_get_count(struct ble_npl_sem *sem)
{
    TOS_CPU_CPSR_ALLOC();
    k_sem_cnt_t sem_cnt;

    TOS_CPU_INT_DISABLE();
    sem_cnt = sem->handle.count;
    TOS_CPU_INT_ENABLE();

    return sem_cnt;
}

static void
os_callout_timer_cb(void *arg)
{
    struct ble_npl_callout *co;

    co = (struct ble_npl_callout *)arg;
    assert(co);

    if (co->evq) {
        ble_npl_eventq_put(co->evq, &co->ev);
    } else {
        co->ev.fn(&co->ev);
    }
}

ble_npl_error_t
npl_tencentos_tiny_task_init(struct ble_npl_task *task, char *name, ble_npl_task_entry_t entry,
                                    void *arg, uint8_t prio, ble_npl_time_t sanity_itvl,
                                    ble_npl_stack_t *stack_bottom, uint16_t stack_size)
{
    k_err_t err;

    err = tos_task_create(&task->handle, name, entry, arg, prio, stack_bottom, stack_size, 0);
    assert(err == K_ERR_NONE);

    return BLE_NPL_OK;
}

void
npl_tencentos_tiny_callout_init(struct ble_npl_callout *co, struct ble_npl_eventq *evq,
                     ble_npl_event_fn *ev_cb, void *ev_arg)
{
    k_err_t err;

    memset(co, 0, sizeof(*co));
    err = tos_timer_create(&co->handle, 1, 0, os_callout_timer_cb, co, TOS_OPT_TIMER_ONESHOT);
    assert(err == K_ERR_NONE);

    co->evq = evq;
    ble_npl_event_init(&co->ev, ev_cb, ev_arg);
}

ble_npl_error_t
npl_tencentos_tiny_callout_reset(struct ble_npl_callout *co, ble_npl_time_t ticks)
{
    if (ticks == 0) {
        ticks = 1;
    }

    tos_timer_stop(&co->handle);
    tos_timer_delay_change(&co->handle, ticks);
    tos_timer_start(&co->handle);

    return BLE_NPL_OK;
}

void
npl_tencentos_tiny_callout_stop(struct ble_npl_callout *co)
{
    tos_timer_stop(&co->handle);
}

bool
npl_tencentos_tiny_callout_is_active(struct ble_npl_callout *co)
{
    return co->handle.state == TIMER_STATE_RUNNING;
}

ble_npl_time_t
npl_tencentos_tiny_callout_get_ticks(struct ble_npl_callout *co)
{
    return co->handle.expires;
}

ble_npl_time_t
npl_tencentos_tiny_callout_remaining_ticks(struct ble_npl_callout *co,
                                     ble_npl_time_t now)
{
    k_tick_t expires;

    expires = co->handle.expires;

    if (expires > now) {
        return expires - now;
    } else {
        return 0;
    }
}

ble_npl_error_t
npl_tencentos_tiny_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    ble_npl_time_t ticks;

    ticks = tos_millisec2tick(ms);
    if (ticks > UINT32_MAX) {
        return BLE_NPL_EINVAL;
    }

    *out_ticks = ticks;

    return BLE_NPL_OK;
}

ble_npl_error_t
npl_tencentos_tiny_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    uint64_t ms;

    ms = ((uint64_t)ticks * K_TIME_MILLISEC_PER_SEC / TOS_CFG_CPU_TICK_PER_SECOND);
    if (ms > UINT32_MAX) {
        return BLE_NPL_EINVAL;
    }

    *out_ms = (uint32_t)ms;

    return BLE_NPL_OK;
}

