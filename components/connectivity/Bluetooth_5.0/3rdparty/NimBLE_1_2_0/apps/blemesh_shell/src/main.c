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
#include "os/mynewt.h"
#include "mesh/mesh.h"
#include "console/console.h"
#include "hal/hal_system.h"
#include "hal/hal_gpio.h"
#include "bsp/bsp.h"
#include "shell/shell.h"

/* BLE */
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "mesh/glue.h"
#include "mesh/testing.h"


void net_recv_ev(uint8_t ttl, uint8_t ctl, uint16_t src, uint16_t dst,
                 const void *payload, size_t payload_len)
{
    console_printf("Received net packet: ttl 0x%02x ctl 0x%02x src 0x%04x "
                   "dst 0x%04x " "payload_len %d\n", ttl, ctl, src, dst,
                   payload_len);
}

static void model_bound_cb(u16_t addr, struct bt_mesh_model *model,
                           u16_t key_idx)
{
    console_printf("Model bound: remote addr 0x%04x key_idx 0x%04x model %p\n",
                   addr, key_idx, model);
}

static void model_unbound_cb(u16_t addr, struct bt_mesh_model *model,
                             u16_t key_idx)
{
    console_printf("Model unbound: remote addr 0x%04x key_idx 0x%04x "
                   "model %p\n", addr, key_idx, model);
}

static void invalid_bearer_cb(u8_t opcode)
{
    console_printf("Invalid bearer: opcode 0x%02x\n", opcode);
}

static void incomp_timer_exp_cb(void)
{
    console_printf("Incomplete timer expired\n");
}

static struct bt_test_cb bt_test_cb = {
    .mesh_net_recv = net_recv_ev,
    .mesh_model_bound = model_bound_cb,
    .mesh_model_unbound = model_unbound_cb,
    .mesh_prov_invalid_bearer = invalid_bearer_cb,
    .mesh_trans_incomp_timer_exp = incomp_timer_exp_cb,
};

static void
blemesh_on_reset(int reason)
{
    BLE_HS_LOG(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
blemesh_on_sync(void)
{
    console_printf("Bluetooth initialized\n");

    shell_register_default_module("mesh");

    if (IS_ENABLED(CONFIG_BT_TESTING)) {
        bt_test_cb_register(&bt_test_cb);
    }
}

int
main(void)
{
    /* Initialize OS */
    sysinit();

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = blemesh_on_reset;
    ble_hs_cfg.sync_cb = blemesh_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    hal_gpio_init_out(LED_2, 0);

    while (1) {
        os_eventq_run(os_eventq_dflt_get());
    }
    return 0;
}
