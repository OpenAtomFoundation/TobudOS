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
#include "sysinit/sysinit.h"
#include "os/os.h"
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
#include "mesh/model_srv.h"
#include "light_model.h"


static void model_bound_cb(u16_t addr, struct bt_mesh_model *model,
                           u16_t key_idx)
{
    int rc;

    console_printf("Model bound: remote addr 0x%04x key_idx 0x%04x model %p\n",
                   addr, key_idx, model);

    if (model->id != BT_MESH_MODEL_ID_GEN_LEVEL_SRV) {
        return;
    }

    /* Hack for demo purposes */
    rc = bt_test_bind_app_key_to_model(model, key_idx,
                                       BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV);

    if (rc) {
        console_printf("Failed to bind light lightness srv model to app_key");
    } else {
        console_printf("Successfuly bound light lightness srv model to app_key");
    }
}

static struct bt_test_cb bt_test_cb = {
    .mesh_model_bound = model_bound_cb,
};

static void
blemesh_on_reset(int reason)
{
    BLE_HS_LOG(ERROR, "Resetting state; reason=%d\n", reason);
}

static struct bt_mesh_gen_onoff_srv_cb gen_onoff_srv_cb = {
        .get = light_model_gen_onoff_get,
        .set = light_model_gen_onoff_set,
};
static struct bt_mesh_gen_level_srv_cb gen_level_srv_cb = {
        .get = light_model_gen_level_get,
        .set = light_model_gen_level_set,
};
static struct bt_mesh_light_lightness_srv_cb light_lightness_srv_cb = {
        .get = light_model_light_lightness_get,
        .set = light_model_light_lightness_set,
};

static void
blemesh_on_sync(void)
{
    console_printf("Bluetooth initialized\n");

    shell_register_default_module("mesh");

    bt_test_cb_register(&bt_test_cb);

    light_model_init();
    bt_mesh_set_gen_onoff_srv_cb(&gen_onoff_srv_cb);
    bt_mesh_set_gen_level_srv_cb(&gen_level_srv_cb);
    bt_mesh_set_light_lightness_srv_cb(&light_lightness_srv_cb);

    console_printf("Mesh initialized\n");

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    if (bt_mesh_is_provisioned()) {
        printk("Mesh network restored from flash\n");
    }

    /* Hack for demo purposes */
    bt_test_shell_init();
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

    while (1) {
        os_eventq_run(os_eventq_dflt_get());
    }
    return 0;
}
