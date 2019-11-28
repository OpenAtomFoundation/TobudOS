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

/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "console/console.h"

#include "common.h"
#include "ble_mesh.h"
#include "device_composition.h"

#define OOB_AUTH_ENABLE 1

#ifdef OOB_AUTH_ENABLE

static int output_number(bt_mesh_output_action_t action, u32_t number)
{
	printk("OOB Number: %lu\n", number);
	return 0;
}

static int output_string(const char *str)
{
	printk("OOB String: %s\n", str);
	return 0;
}

#endif

static void prov_complete(u16_t net_idx, u16_t addr)
{
	printk("Local node provisioned, primary address 0x%04x\n", addr);
}

static void prov_reset(void)
{
	bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);
}

static u8_t dev_uuid[16] = MYNEWT_VAL(BLE_MESH_DEV_UUID);

static const struct bt_mesh_prov prov = {
	.uuid = dev_uuid,

#ifdef OOB_AUTH_ENABLE

	.output_size = 6,
	.output_actions = BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING,
	.output_number = output_number,
	.output_string = output_string,

#endif

	.complete = prov_complete,
	.reset = prov_reset,
};

void blemesh_on_reset(int reason)
{
	BLE_HS_LOG(ERROR, "Resetting state; reason=%d\n", reason);
}

void blemesh_on_sync(void)
{
	int err;
	ble_addr_t addr;

	console_printf("Bluetooth initialized\n");

	/* Use NRPA */
	err = ble_hs_id_gen_rnd(1, &addr);
	assert(err == 0);
	err = ble_hs_id_set_rnd(addr.val);
	assert(err == 0);

	err = bt_mesh_init(addr.type, &prov, &comp);
	if (err) {
		console_printf("Initializing mesh failed (err %d)\n", err);
		return;
	}

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	if (bt_mesh_is_provisioned()) {
		console_printf("Mesh network restored from flash\n");
	}

	bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

	console_printf("Mesh initialized\n");

	bt_initialized();
}
