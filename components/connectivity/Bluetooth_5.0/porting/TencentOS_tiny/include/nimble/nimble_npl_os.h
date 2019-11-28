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

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tos.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _Static_assert
#define _Static_assert(...)
#endif

/* This should be compatible with k_tick_t */
typedef k_tick_t    ble_npl_time_t;
typedef int64_t     ble_npl_stime_t;

typedef void (*ble_npl_task_entry_t)(void *args);
typedef k_stack_t   ble_npl_stack_t;

#define BLE_NPL_OS_TASK_STACK_DEFINE(stk, size) ble_npl_stack_t stk[size]

#define BLE_NPL_OS_ALIGNMENT    4

#define BLE_NPL_TIME_FOREVER    TOS_TIME_FOREVER

extern volatile int ble_npl_in_critical;

struct ble_npl_event {
    bool queued;
    ble_npl_event_fn *fn;
    void *arg;
};

struct ble_npl_eventq {
    k_msg_q_t q;
};

struct ble_npl_callout {
    k_timer_t handle;
    struct ble_npl_eventq *evq;
    struct ble_npl_event ev;
};

struct ble_npl_mutex {
    k_mutex_t handle;
};

struct ble_npl_sem {
    k_sem_t handle;
};

struct ble_npl_task {
    k_task_t handle;
};

/*
 * Simple APIs are just defined as static inline below, but some are a bit more
 * complex or require some global state variables and thus are defined in .c
 * file instead and static inline wrapper just calls proper implementation.
 * We need declarations of these functions and they are defined in header below.
 */
#include "npl_tencentos_tiny.h"

static inline bool
ble_npl_os_started(void)
{
    return tos_knl_is_running();
}

static inline void *
ble_npl_get_current_task_id(void)
{
    return (void *)k_curr_task;
}

static inline void
ble_npl_eventq_init(struct ble_npl_eventq *evq)
{
    npl_tencentos_tiny_eventq_init(evq);
}

static inline struct ble_npl_event *
ble_npl_eventq_get(struct ble_npl_eventq *evq, ble_npl_time_t tmo)
{
    return npl_tencentos_tiny_eventq_get(evq, tmo);
}

static inline void
ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    npl_tencentos_tiny_eventq_put(evq, ev);
}

static inline void
ble_npl_eventq_remove(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    npl_tencentos_tiny_eventq_remove(evq, ev);
}

static inline void
ble_npl_event_run(struct ble_npl_event *ev)
{
    ev->fn(ev);
}

static inline bool
ble_npl_eventq_is_empty(struct ble_npl_eventq *evq)
{
    return npl_tencentos_tiny_eventq_is_empty(evq);
}

static inline void
ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn,
                   void *arg)
{
    memset(ev, 0, sizeof(*ev));
    ev->fn = fn;
    ev->arg = arg;
}

static inline bool
ble_npl_event_is_queued(struct ble_npl_event *ev)
{
    return ev->queued;
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
    return npl_tencentos_tiny_mutex_init(mu);
}

static inline ble_npl_error_t
ble_npl_mutex_pend(struct ble_npl_mutex *mu, ble_npl_time_t timeout)
{
    return npl_tencentos_tiny_mutex_pend(mu, timeout);
}

static inline ble_npl_error_t
ble_npl_mutex_release(struct ble_npl_mutex *mu)
{
    return npl_tencentos_tiny_mutex_release(mu);
}

static inline ble_npl_error_t
ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    return npl_tencentos_tiny_sem_init(sem, tokens);
}

static inline ble_npl_error_t
ble_npl_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout)
{
    return npl_tencentos_tiny_sem_pend(sem, timeout);
}

static inline ble_npl_error_t
ble_npl_sem_release(struct ble_npl_sem *sem)
{
    return npl_tencentos_tiny_sem_release(sem);
}

static inline uint16_t
ble_npl_sem_get_count(struct ble_npl_sem *sem)
{
    return npl_tencentos_tiny_sem_get_count(sem);
}

static inline void
ble_npl_callout_init(struct ble_npl_callout *co, struct ble_npl_eventq *evq,
                     ble_npl_event_fn *ev_cb, void *ev_arg)
{
    npl_tencentos_tiny_callout_init(co, evq, ev_cb, ev_arg);
}

static inline ble_npl_error_t
ble_npl_callout_reset(struct ble_npl_callout *co, ble_npl_time_t ticks)
{
    return npl_tencentos_tiny_callout_reset(co, ticks);
}

static inline void
ble_npl_callout_stop(struct ble_npl_callout *co)
{
    npl_tencentos_tiny_callout_stop(co);
}

static inline bool
ble_npl_callout_is_active(struct ble_npl_callout *co)
{
    return npl_tencentos_tiny_callout_is_active(co);
}

static inline ble_npl_time_t
ble_npl_callout_get_ticks(struct ble_npl_callout *co)
{
    return npl_tencentos_tiny_callout_get_ticks(co);
}

static inline ble_npl_time_t
ble_npl_callout_remaining_ticks(struct ble_npl_callout *co,
                                ble_npl_time_t time)
{
    return npl_tencentos_tiny_callout_remaining_ticks(co, time);
}

static inline void
ble_npl_callout_set_arg(struct ble_npl_callout *co, void *arg)
{
    co->ev.arg = arg;
}

static inline ble_npl_error_t
ble_npl_task_init(struct ble_npl_task *task, char *name, ble_npl_task_entry_t entry,
                                    void *arg, uint8_t prio, ble_npl_time_t sanity_itvl,
                                    ble_npl_stack_t *stack_bottom, uint16_t stack_size)
{
    return npl_tencentos_tiny_task_init(task, name, entry, arg, prio, sanity_itvl, stack_bottom, stack_size);
}

static inline ble_npl_time_t
ble_npl_time_get(void)
{
    return tos_systick_get();
}

static inline ble_npl_error_t
ble_npl_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    return npl_tencentos_tiny_time_ms_to_ticks(ms, out_ticks);
}

static inline ble_npl_error_t
ble_npl_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    return npl_tencentos_tiny_time_ticks_to_ms(ticks, out_ms);
}

static inline ble_npl_time_t
ble_npl_time_ms_to_ticks32(uint32_t ms)
{
    return tos_millisec2tick(ms);
}

static inline uint32_t
ble_npl_time_ticks_to_ms32(ble_npl_time_t ticks)
{
    return tos_tick2millisec(ticks);
}

static inline void
ble_npl_time_delay(ble_npl_time_t ticks)
{
    tos_task_delay(ticks);
}

static inline uint32_t
ble_npl_hw_enter_critical(void)
{
    cpu_cpsr_t cpsr;

    cpsr = tos_cpu_cpsr_save();
    ++ble_npl_in_critical;
    return cpsr;
}

static inline void
ble_npl_hw_exit_critical(uint32_t ctx)
{
    --ble_npl_in_critical;
    tos_cpu_cpsr_restore(ctx);
}

static inline bool
ble_npl_hw_is_in_critical(void)
{
    /*
     * XXX Currently TencentOS tiny does not support an API for finding out if interrupts
     *     are currently disabled, hence in a critical section in this context.
     *     So for now, we use this global variable to keep this state for us.
    -*/
    return (ble_npl_in_critical > 0);
}

#ifdef __cplusplus
}
#endif

#endif  /* _NPL_H_ */

