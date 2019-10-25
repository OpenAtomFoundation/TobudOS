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

/**
  Unit tests for the ble_npl_callout api:

  void ble_npl_callout_init(struct ble_npl_callout *cf, struct ble_npl_eventq *evq,
                       ble_npl_event_fn *ev_cb, void *ev_arg);
  int ble_npl_callout_reset(struct ble_npl_callout *, int32_t);
  int ble_npl_callout_queued(struct ble_npl_callout *c);
  void ble_npl_callout_stop(struct ble_npl_callout *c);
*/

#include "test_util.h"
#include "nimble/nimble_npl.h"

#define TEST_ARGS_VALUE  (55)
#define TEST_INTERVAL    (100)

static bool                   s_tests_running = true;
static struct ble_npl_task    s_task;
static struct ble_npl_callout s_callout;
static int                    s_callout_args = TEST_ARGS_VALUE;

static struct ble_npl_eventq  s_eventq;


void on_callout(struct ble_npl_event *ev)
{
    VerifyOrQuit(ev->ev_arg == &s_callout_args,
		 "callout: wrong args passed");

    VerifyOrQuit(*(int*)ev->ev_arg == TEST_ARGS_VALUE,
		 "callout: args corrupted");

    s_tests_running = false;
}

/**
 * ble_npl_callout_init(struct ble_npl_callout *c, struct ble_npl_eventq *evq,
 *                 ble_npl_event_fn *ev_cb, void *ev_arg)
 */
int test_init()
{
    ble_npl_callout_init(&s_callout,
		    &s_eventq,
		    on_callout,
		    &s_callout_args);
    return PASS;
}

int test_queued()
{
  //VerifyOrQuit(ble_npl_callout_queued(&s_callout),
  //	 "callout: not queued when expected");
    return PASS;
}

int test_reset()
{
    return ble_npl_callout_reset(&s_callout, TEST_INTERVAL);
}

int test_stop()
{
    return PASS;
}


/**
 * ble_npl_callout_init(struct ble_npl_callout *c, struct ble_npl_eventq *evq,
 *                 ble_npl_event_fn *ev_cb, void *ev_arg)
 */
void *test_task_run(void *args)
{
    SuccessOrQuit(test_init(),   "callout_init failed");
    SuccessOrQuit(test_queued(), "callout_queued failed");
    SuccessOrQuit(test_reset(),  "callout_reset failed");

    while (s_tests_running)
    {
        ble_npl_eventq_run(&s_eventq);
    }

    printf("All tests passed\n");
    exit(PASS);

    return NULL;
}

int main(void)
{
    ble_npl_eventq_init(&s_eventq);

    SuccessOrQuit(ble_npl_task_init(&s_task, "s_task", test_task_run,
			       NULL, 1, 0, NULL, 0),
		  "task: error initializing");

    while (1) {}
}
