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

#ifndef _NIMBLE_NPL_OS_H_
#define _NIMBLE_NPL_OS_H_

#include <stdint.h>
#include <stdbool.h>
#include "event/callback.h"
#include "mutex.h"
#include "semaphore.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_NPL_OS_ALIGNMENT    4

#define BLE_NPL_TIME_FOREVER    UINT32_MAX

typedef uint32_t ble_npl_time_t;
typedef int32_t ble_npl_stime_t;

extern volatile int ble_npl_in_critical;

struct ble_npl_event {
    event_callback_t e;
    void *arg;
};

struct ble_npl_eventq {
    event_queue_t q;
};

struct ble_npl_callout {
    xtimer_t timer;
    ble_npl_time_t target_ticks;
    struct ble_npl_event e;
    event_queue_t *q;
};

struct ble_npl_mutex {
    mutex_t mu;
};

struct ble_npl_sem {
    sem_t sem;
};

static inline bool
ble_npl_os_started(void)
{
    return true;
}

static inline void *
ble_npl_get_current_task_id(void)
{
    return (void *)(uint32_t)thread_getpid();
}

static inline void
ble_npl_eventq_init(struct ble_npl_eventq *evq)
{
    event_queue_init_detached(&evq->q);
}

static inline void
ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    event_post(&evq->q, &ev->e.super);
}

static inline struct ble_npl_event *
ble_npl_eventq_get(struct ble_npl_eventq *evq, ble_npl_time_t tmo)
{
    if (evq->q.waiter == NULL) {
        event_queue_claim(&evq->q);
    }

    if (tmo == 0) {
        return (struct ble_npl_event *)event_get(&evq->q);
    } else if (tmo == BLE_NPL_TIME_FOREVER) {
        return (struct ble_npl_event *)event_wait(&evq->q);
    } else {
        return (struct ble_npl_event *)event_wait_timeout(&evq->q, (tmo * 1000));
    }
}

static inline void
ble_npl_eventq_remove(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    event_cancel(&evq->q, &ev->e.super);
}

static inline void
ble_npl_event_run(struct ble_npl_event *ev)
{
    ev->e.super.handler(&ev->e.super);
}

static inline void
ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn, void *arg)
{
    /*
     * Need to clear list_node manually since init function below does not do
     * this.
     */
    ev->e.super.list_node.next = NULL;
    event_callback_init(&ev->e, (void(*)(void *))fn, ev);
    ev->arg = arg;
}

static inline bool
ble_npl_event_is_queued(struct ble_npl_event *ev)
{
    return (ev->e.super.list_node.next != NULL);
}

static inline void *
ble_npl_event_get_arg(struct ble_npl_event *ev)
{
    return ev->arg;
}

static inline void
ble_npl_event_set_arg(struct ble_npl_event *ev, void *arg)
{
    ev->arg = arg;
}

static inline ble_npl_error_t
ble_npl_mutex_init(struct ble_npl_mutex *mu)
{
    mutex_init(&mu->mu);
    return BLE_NPL_OK;
}

static inline ble_npl_error_t
ble_npl_mutex_pend(struct ble_npl_mutex *mu, ble_npl_time_t timeout)
{
    assert(timeout == BLE_NPL_TIME_FOREVER);
    (void)timeout;

    mutex_lock(&mu->mu);
    return BLE_NPL_OK;
}

static inline ble_npl_error_t
ble_npl_mutex_release(struct ble_npl_mutex *mu)
{
    mutex_unlock(&mu->mu);
    return BLE_NPL_OK;
}

static inline ble_npl_error_t
ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    int rc;

    rc = sem_init(&sem->sem, 0, tokens);

    return rc == 0 ? BLE_NPL_OK : BLE_NPL_ERROR;
}

static inline ble_npl_error_t
ble_npl_sem_release(struct ble_npl_sem *sem)
{
    int rc;

    rc = sem_post(&sem->sem);

    return rc == 0 ? BLE_NPL_OK : BLE_NPL_ERROR;
}

static inline uint16_t
ble_npl_sem_get_count(struct ble_npl_sem *sem)
{
    int val = 0;

    sem_getvalue(&sem->sem, &val);

    return (uint16_t)val;
}

static inline void
ble_npl_callout_stop(struct ble_npl_callout *co)
{
    xtimer_remove(&co->timer);
}

static inline bool
ble_npl_callout_is_active(struct ble_npl_callout *c)
{
    return (c->timer.target || c->timer.long_target);
}

static inline ble_npl_time_t
ble_npl_callout_get_ticks(struct ble_npl_callout *co)
{
    return co->target_ticks;
}

static inline void
ble_npl_callout_set_arg(struct ble_npl_callout *co, void *arg)
{
    co->e.arg = arg;
}

static inline uint32_t
ble_npl_time_get(void)
{
    return xtimer_now_usec() / 1000;
}

static inline ble_npl_error_t
ble_npl_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    *out_ticks = ms;
    return BLE_NPL_OK;
}

static inline ble_npl_error_t
ble_npl_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    *out_ms = ticks;
    return BLE_NPL_OK;
}

static inline ble_npl_time_t
ble_npl_time_ms_to_ticks32(uint32_t ms)
{
    return ms;
}

static inline uint32_t
ble_npl_time_ticks_to_ms32(ble_npl_time_t ticks)
{
    return ticks;
}

static inline void
ble_npl_time_delay(ble_npl_time_t ticks)
{
    xtimer_usleep(ticks * 1000);
}

static inline uint32_t
ble_npl_hw_enter_critical(void)
{
    uint32_t ctx = irq_disable();
    ++ble_npl_in_critical;
    return ctx;
}

static inline void
ble_npl_hw_exit_critical(uint32_t ctx)
{
    --ble_npl_in_critical;
    irq_restore((unsigned)ctx);
}

static inline bool
ble_npl_hw_is_in_critical(void)
{
    /*
     * XXX Currently RIOT does not support an API for finding out if interrupts
     *     are currently disabled, hence in a critical section in this context.
     *     So for now, we use this global variable to keep this state for us.
    -*/
    return (ble_npl_in_critical > 0);
}

#ifdef __cplusplus
}
#endif

#endif  /* _NPL_H_ */
