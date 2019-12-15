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

#include "test_util.h"
#include "nimble/nimble_npl.h"

#include <pthread.h>

#define TASK0_ARG     55
#define TASK1_ARG     66

static struct ble_npl_task s_task[2];
static int            s_task_arg[2] =
{
    TASK0_ARG, TASK1_ARG
};


void *task0_run(void *args)
{
    int i = 10000;
    VerifyOrQuit(args == &s_task_arg[0], "Wrong args passed to task0");

    while (i--)
    {
    }

    return NULL;
}

void *task1_run(void *args)
{
    int i = 10000;
    VerifyOrQuit(args == &s_task_arg[1], "Wrong args passed to task0");

    while (i--)
    {
    }

    printf("All tests passed\n");
    exit(PASS);

    return NULL;
}

/**
 * Unit test for initializing a task.
 *
 * int ble_npl_task_init(struct ble_npl_task *t, const char *name, ble_npl_task_func_t func,
 *                  void *arg, uint8_t prio, ble_npl_time_t sanity_itvl,
 *                  ble_npl_stack_t *stack_bottom, uint16_t stack_size)
 *
 */
int test_init()
{
    int err;
    err = ble_npl_task_init(NULL,
		       "Null task",
		       NULL, NULL, 1, 0, NULL, 0);
    VerifyOrQuit(err, "ble_npl_task_init accepted NULL parameters.");

    err = ble_npl_task_init(&s_task[0],
		       "s_task[0]",
		       task0_run, &s_task_arg[0], 1, 0, NULL, 0);
    SuccessOrQuit(err, "ble_npl_task_init failed.");

    err = ble_npl_task_init(&s_task[1],
		       "s_task[1]",
		       task1_run, &s_task_arg[1], 1, 0, NULL, 0);

    return err;
}

int main(void)
{
    int ret = PASS;
    SuccessOrQuit(test_init(),    "Failed: ble_npl_task_init");

    pthread_exit(&ret);

    return ret;
}
