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

/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sysinit/sysinit.h"

#include "modlog/modlog.h"
#include "host/ble_uuid.h"
#include "host/ble_hs.h"

#include "bttester.h"

static void on_reset(int reason)
{
	MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void on_sync(void)
{
	MODLOG_DFLT(INFO, "Bluetooth initialized\n");

	tester_init();
}

int main(int argc, char **argv)
{
	int rc;

#ifdef ARCH_sim
	mcu_sim_parse_args(argc, argv);
#endif

	/* Initialize OS */
	sysinit();

	/* Initialize the NimBLE host configuration. */
	ble_hs_cfg.reset_cb = on_reset;
	ble_hs_cfg.sync_cb = on_sync;
	ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb,
	ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

	rc = gatt_svr_init();
	assert(rc == 0);

	while (1) {
		os_eventq_run(os_eventq_dflt_get());
	}
	return 0;
}
