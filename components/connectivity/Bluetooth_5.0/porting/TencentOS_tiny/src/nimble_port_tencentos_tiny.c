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
#include <tos.h>
#include "syscfg/syscfg.h"
#include "nimble/nimble_port.h"

#if NIMBLE_CFG_CONTROLLER
static k_task_t ll_task_h;
static k_stack_t ll_task_stack[MYNEWT_VAL(BLE_LL_TASK_STACK_SIZE)];
#endif
static k_task_t host_task_h;
static k_stack_t host_task_stack[MYNEWT_VAL(BLE_HOST_TASK_STACK_SIZE)];

__WEAK__ void __aeabi_assert(const char *expr, const char *file, int line)
{
    while (1)
        ;
}

void
nimble_port_tencentos_tiny_init(k_task_entry_t host_task_fn)
{
    k_err_t err;

#if NIMBLE_CFG_CONTROLLER
    /*
     * Create task where NimBLE LL will run. This one is required as LL has its
     * own event queue and should have highest priority. The task function is
     * provided by NimBLE and in case of TencentOS tiny it does not need to be wrapped
     * since it has compatible prototype.
     */

    err = tos_task_create(&ll_task_h, "ll", nimble_port_ll_task_func, NULL,
                            MYNEWT_VAL(BLE_LL_TASK_PRIORITY),
                            ll_task_stack, MYNEWT_VAL(BLE_LL_TASK_STACK_SIZE),
                            0);
    if (err != K_ERR_NONE) {
        return;
    }
#endif

    /*
     * Create task where NimBLE host will run. It is not strictly necessary to
     * have separate task for NimBLE host, but since something needs to handle
     * default queue it is just easier to make separate task which does this.
     */
    err = tos_task_create(&host_task_h, "ble", host_task_fn, NULL,
                            MYNEWT_VAL(BLE_HOST_TASK_PRIORITY),
                            host_task_stack, MYNEWT_VAL(BLE_HOST_TASK_STACK_SIZE),
                            0);
    if (err != K_ERR_NONE) {
        return;
    }
}

