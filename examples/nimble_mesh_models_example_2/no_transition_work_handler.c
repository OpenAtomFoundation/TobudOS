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

#include "ble_mesh.h"
#include "device_composition.h"

#include "storage.h"

static void unsolicitedly_publish_states_work_handler(struct ble_npl_event *work)
{
	gen_onoff_publish(&root_models[2]);
	gen_level_publish(&root_models[4]);
	light_lightness_publish(&root_models[11]);
	light_lightness_linear_publish(&root_models[11]);
	light_ctl_publish(&root_models[14]);

	gen_level_publish(&s0_models[0]);
	light_ctl_temp_publish(&s0_models[2]);
}

struct ble_npl_callout unsolicitedly_publish_states_work;

static void unsolicitedly_publish_states_timer_handler(struct ble_npl_event *dummy)
{
	ble_npl_callout_reset(&unsolicitedly_publish_states_work, 0);
}

struct ble_npl_callout unsolicitedly_publish_states_timer;

static void save_lightness_temp_last_values_timer_handler(struct ble_npl_event *dummy)
{
	save_on_flash(LIGHTNESS_TEMP_LAST_STATE);
}

struct ble_npl_callout save_lightness_temp_last_values_timer;

static void no_transition_work_handler(struct ble_npl_event *work)
{
	ble_npl_callout_reset(&unsolicitedly_publish_states_timer,
			 ble_npl_time_ms_to_ticks32(K_MSEC(5000)));

	/* If Lightness & Temperature values remains stable for
	 * 10 Seconds then & then only get stored on SoC flash.
	 */
	if (gen_power_onoff_srv_user_data.onpowerup == STATE_RESTORE) {
		ble_npl_callout_reset(&save_lightness_temp_last_values_timer,
				 ble_npl_time_ms_to_ticks32(
					 K_MSEC(10000)));
	}
}

struct ble_npl_callout no_transition_work;

void no_transition_work_init(void)
{
	ble_npl_callout_init(&no_transition_work, nimble_port_get_dflt_eventq(),
			no_transition_work_handler, NULL);
	ble_npl_callout_init(&save_lightness_temp_last_values_timer,
			nimble_port_get_dflt_eventq(),
			save_lightness_temp_last_values_timer_handler,
			NULL);
	ble_npl_callout_init(&unsolicitedly_publish_states_work, nimble_port_get_dflt_eventq(),
			unsolicitedly_publish_states_work_handler, NULL);
	ble_npl_callout_init(&unsolicitedly_publish_states_timer, nimble_port_get_dflt_eventq(),
			unsolicitedly_publish_states_timer_handler, NULL);
}
