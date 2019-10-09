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
  Unit tests for the ble_npl_eventq api:

  void ble_npl_eventq_init(struct ble_npl_eventq *);
  void ble_npl_eventq_put(struct ble_npl_eventq *, struct ble_npl_event *);
  struct ble_npl_event *ble_npl_eventq_get_no_wait(struct ble_npl_eventq *evq);
  struct ble_npl_event *ble_npl_eventq_get(struct ble_npl_eventq *);
  void ble_npl_eventq_run(struct ble_npl_eventq *evq);
  struct ble_npl_event *ble_npl_eventq_poll(struct ble_npl_eventq **, int, ble_npl_time_t);
  void ble_npl_eventq_remove(struct ble_npl_eventq *, struct ble_npl_event *);
  struct ble_npl_eventq *ble_npl_eventq_dflt_get(void);
*/

#include <assert.h>
#include <pthread.h>
#include "test_util.h"
#include "nimble/nimble_npl.h"

#define TEST_ARGS_VALUE  (55)
#define TEST_STACK_SIZE  (1024)

static bool                   s_tests_running = true;
static struct ble_npl_task    s_task_runner;
static struct ble_npl_task    s_task_dispatcher;

static struct ble_npl_eventq  s_eventq;
static struct ble_npl_event   s_event;
static int                    s_event_args = TEST_ARGS_VALUE;


void on_event(struct ble_npl_event *ev)
{
    VerifyOrQuit(ev->ev_arg == &s_event_args,
		 "callout: wrong args passed");

    VerifyOrQuit(*(int*)ev->ev_arg == TEST_ARGS_VALUE,
		 "callout: args corrupted");

    s_tests_running = false;
}

int test_init()
{
    //VerifyOrQuit(!ble_npl_eventq_inited(&s_eventq), "eventq: empty q initialized");
    ble_npl_eventq_init(&s_eventq);
    //VerifyOrQuit(ble_npl_eventq_inited(&s_eventq), "eventq: not initialized");

    return PASS;
}

int test_run()
{
    while (s_tests_running)
    {
        ble_npl_eventq_run(&s_eventq);
    }

    return PASS;
}

int test_put()
{
    s_event.ev_cb = on_event;
    s_event.ev_arg = &s_event_args;
    ble_npl_eventq_put(&s_eventq, &s_event);
    return PASS;
}

int test_get_no_wait()
{
    //struct ble_npl_event *ev = ble_npl_eventq_get_no_wait(&s_eventq);
    return FAIL;
}

int test_get()
{
    struct ble_npl_event *ev = ble_npl_eventq_get(&s_eventq,
						  BLE_NPL_WAIT_FOREVER);

    VerifyOrQuit(ev == &s_event,
		 "callout: wrong event passed");

    return PASS;
}


void *task_test_runner(void *args)
{
    int count = 1000000000;

    SuccessOrQuit(test_init(), "eventq_init failed");
    SuccessOrQuit(test_put(),  "eventq_put failed");
    SuccessOrQuit(test_get(),  "eventq_get failed");
    SuccessOrQuit(test_put(),  "eventq_put failed");
    SuccessOrQuit(test_run(),  "eventq_run failed");

    printf("All tests passed\n");
    exit(PASS);

    return NULL;
}

int main(void)
{
    SuccessOrQuit(ble_npl_task_init(&s_task_runner,
			      "task_test_runner",
			      task_test_runner,
			      NULL, 1, 0, NULL, 0),
		          "task: error initializing");

    while (1) {}
}
