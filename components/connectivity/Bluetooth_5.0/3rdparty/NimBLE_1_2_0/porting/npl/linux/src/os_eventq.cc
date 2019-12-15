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

#include "nimble/nimble_npl.h"
#include "wqueue.h"

extern "C" {

typedef wqueue<ble_npl_event *> wqueue_t;

static struct ble_npl_eventq dflt_evq;

struct ble_npl_eventq *
ble_npl_eventq_dflt_get(void)
{
    if (!dflt_evq.q) {
        dflt_evq.q = new wqueue_t();
    }

    return &dflt_evq;
}

void
ble_npl_eventq_init(struct ble_npl_eventq *evq)
{
    evq->q = new wqueue_t();
}

bool
ble_npl_eventq_is_empty(struct ble_npl_eventq *evq)
{
    wqueue_t *q = static_cast<wqueue_t *>(evq->q);

    if (q->size()) {
        return 1;
    } else {
        return 0;
    }
}

int
ble_npl_eventq_inited(const struct ble_npl_eventq *evq)
{
    return (evq->q != NULL);
}

void
ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    wqueue_t *q = static_cast<wqueue_t *>(evq->q);

    if (ev->ev_queued) {
        return;
    }

    ev->ev_queued = 1;
    q->put(ev);
}

struct ble_npl_event *ble_npl_eventq_get(struct ble_npl_eventq *evq,
                                         ble_npl_time_t tmo)
{
    struct ble_npl_event *ev;
    wqueue_t *q = static_cast<wqueue_t *>(evq->q);

    ev = q->get(tmo);

    if (ev) {
        ev->ev_queued = 0;
    }

    return ev;
}

void
ble_npl_eventq_run(struct ble_npl_eventq *evq)
{
    struct ble_npl_event *ev;

    ev = ble_npl_eventq_get(evq, BLE_NPL_TIME_FOREVER);
    ble_npl_event_run(ev);
}


// ========================================================================
//                         Event Implementation
// ========================================================================

void
ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn,
                   void *arg)
{
    memset(ev, 0, sizeof(*ev));
    ev->ev_cb = fn;
    ev->ev_arg = arg;
}

bool
ble_npl_event_is_queued(struct ble_npl_event *ev)
{
    return ev->ev_queued;
}

void *
ble_npl_event_get_arg(struct ble_npl_event *ev)
{
    return ev->ev_arg;
}

void
ble_npl_event_set_arg(struct ble_npl_event *ev, void *arg)
{
    ev->ev_arg = arg;
}

void
ble_npl_event_run(struct ble_npl_event *ev)
{
    assert(ev->ev_cb != NULL);

    ev->ev_cb(ev);
}

}
