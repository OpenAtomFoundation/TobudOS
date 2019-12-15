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
#include <string.h>
#include "os/os.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_NPL_TIME_FOREVER    (OS_TIMEOUT_NEVER)

typedef os_time_t ble_npl_time_t;
typedef os_stime_t ble_npl_stime_t;

/*
 * This allows to cast between ble_npl_* and os_* structs to make NPL for Mynewt
 * just a shim layer on top of kernel/os.
 */

struct ble_npl_event {
    struct os_event ev;
};

struct ble_npl_eventq {
    struct os_eventq evq;
};

struct ble_npl_callout {
    struct os_callout co;
};

struct ble_npl_mutex {
    struct os_mutex mu;
};

struct ble_npl_sem {
    struct os_sem sem;
};

static inline bool
ble_npl_os_started(void)
{
    return os_started();
}

static inline void *
ble_npl_get_current_task_id(void)
{
    return os_sched_get_current_task();
}

static inline struct ble_npl_eventq *
ble_npl_eventq_dflt_get(void)
{
    return (struct ble_npl_eventq *) os_eventq_dflt_get();
}

static inline void
ble_npl_eventq_init(struct ble_npl_eventq *evq)
{
    os_eventq_init(&evq->evq);
}

static inline struct ble_npl_event *
ble_npl_eventq_get(struct ble_npl_eventq *evq, ble_npl_time_t tmo)
{
    struct os_event *ev;

    if (tmo == BLE_NPL_TIME_FOREVER) {
        ev = os_eventq_get(&evq->evq);
    } else {
        ev = os_eventq_poll((struct os_eventq **)&evq, 1, tmo);
    }

    return (struct ble_npl_event *)ev;
}

static inline void
ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    os_eventq_put(&evq->evq, &ev->ev);
}

static inline void
ble_npl_eventq_remove(struct ble_npl_eventq *evq,
                      struct ble_npl_event *ev)
{
    os_eventq_remove(&evq->evq, &ev->ev);
}

static inline void
ble_npl_event_run(struct ble_npl_event *ev)
{
    ev->ev.ev_cb(&ev->ev);
}

static inline bool
ble_npl_eventq_is_empty(struct ble_npl_eventq *evq)
{
    return STAILQ_EMPTY(&evq->evq.evq_list);
}

static inline void
ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn,
                   void *arg)
{
    memset(ev, 0, sizeof(*ev));
    ev->ev.ev_queued = 0;
    ev->ev.ev_cb = (os_event_fn *)fn;
    ev->ev.ev_arg = arg;
}

static inline bool
ble_npl_event_is_queued(struct ble_npl_event *ev)
{
    return ev->ev.ev_queued;
}

static inline void *
ble_npl_event_get_arg(struct ble_npl_event *ev)
{
    return ev->ev.ev_arg;
}

static inline void
ble_npl_event_set_arg(struct ble_npl_event *ev, void *arg)
{
    ev->ev.ev_arg = arg;
}

static inline ble_npl_error_t
ble_npl_mutex_init(struct ble_npl_mutex *mu)
{
    return (ble_npl_error_t)os_mutex_init(&mu->mu);
}

static inline ble_npl_error_t
ble_npl_mutex_pend(struct ble_npl_mutex *mu, ble_npl_time_t timeout)
{
    return (ble_npl_error_t)os_mutex_pend(&mu->mu, timeout);
}

static inline ble_npl_error_t
ble_npl_mutex_release(struct ble_npl_mutex *mu)
{
    return (ble_npl_error_t)os_mutex_release(&mu->mu);
}

static inline ble_npl_error_t
ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    return (ble_npl_error_t)os_sem_init(&sem->sem, tokens);
}

static inline ble_npl_error_t
ble_npl_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout)
{
    return (ble_npl_error_t)os_sem_pend(&sem->sem, timeout);
}

static inline ble_npl_error_t
ble_npl_sem_release(struct ble_npl_sem *sem)
{
    return (ble_npl_error_t)os_sem_release(&sem->sem);
}

static inline uint16_t
ble_npl_sem_get_count(struct ble_npl_sem *sem)
{
    return os_sem_get_count(&sem->sem);
}

static inline void
ble_npl_callout_init(struct ble_npl_callout *co, struct ble_npl_eventq *evq,
                     ble_npl_event_fn *ev_cb, void *ev_arg)
{
    os_callout_init(&co->co, &evq->evq, (os_event_fn *)ev_cb, ev_arg);
}

static inline ble_npl_error_t
ble_npl_callout_reset(struct ble_npl_callout *co, ble_npl_time_t ticks)
{
    return (ble_npl_error_t)os_callout_reset(&co->co, ticks);
}

static inline void
ble_npl_callout_stop(struct ble_npl_callout *co)
{
    os_callout_stop(&co->co);
}

static inline bool
ble_npl_callout_is_active(struct ble_npl_callout *co)
{
    return os_callout_queued(&co->co);
}

static inline ble_npl_time_t
ble_npl_callout_get_ticks(struct ble_npl_callout *co)
{
    return co->co.c_ticks;
}

static inline ble_npl_time_t
ble_npl_callout_remaining_ticks(struct ble_npl_callout *co,
                                ble_npl_time_t time)
{
    return os_callout_remaining_ticks(&co->co, time);
}

static inline void
ble_npl_callout_set_arg(struct ble_npl_callout *co,
                        void *arg)
{
    co->co.c_ev.ev_arg = arg;
}

static inline uint32_t
ble_npl_time_get(void)
{
    return os_time_get();
}

static inline ble_npl_error_t
ble_npl_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    return (ble_npl_error_t)os_time_ms_to_ticks(ms, out_ticks);
}

static inline ble_npl_error_t
ble_npl_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    return os_time_ticks_to_ms(ticks, out_ms);
}

static inline ble_npl_time_t
ble_npl_time_ms_to_ticks32(uint32_t ms)
{
    return os_time_ms_to_ticks32(ms);
}

static inline uint32_t
ble_npl_time_ticks_to_ms32(ble_npl_time_t ticks)
{
    return os_time_ticks_to_ms32(ticks);
}

static inline void
ble_npl_time_delay(ble_npl_time_t ticks)
{
    return os_time_delay(ticks);
}

static inline uint32_t
ble_npl_hw_enter_critical(void)
{
    return os_arch_save_sr();
}

static inline void
ble_npl_hw_exit_critical(uint32_t ctx)
{
    os_arch_restore_sr(ctx);
}

#ifdef __cplusplus
}
#endif

#endif  /* _NPL_H_ */
