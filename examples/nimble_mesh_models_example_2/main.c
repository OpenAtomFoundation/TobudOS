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
#include "mesh/mesh.h"

#include "app_gpio.h"
#include "storage.h"

#include "ble_mesh.h"
#include "device_composition.h"
#include "no_transition_work_handler.h"
#include "publisher.h"
#include "state_binding.h"
#include "transition.h"

static bool reset;


///////////////////////
// stub-s

/*
 * The "pull" of the gpio. This is either an input or an output.
 */
enum hal_gpio_pull {
    /** Pull-up/down not enabled */
    HAL_GPIO_PULL_NONE = 0,
    /** Pull-up enabled */
    HAL_GPIO_PULL_UP = 1,
    /** Pull-down enabled */
    HAL_GPIO_PULL_DOWN = 2
};
typedef enum hal_gpio_pull hal_gpio_pull_t;

/*
 * IRQ trigger type.
 */
enum hal_gpio_irq_trigger {
    HAL_GPIO_TRIG_NONE = 0,
    /** IRQ occurs on rising edge */
    HAL_GPIO_TRIG_RISING = 1,
    /** IRQ occurs on falling edge */
    HAL_GPIO_TRIG_FALLING = 2,
    /** IRQ occurs on either edge */
    HAL_GPIO_TRIG_BOTH = 3,
    /** IRQ occurs when line is low */
    HAL_GPIO_TRIG_LOW = 4,
    /** IRQ occurs when line is high */
    HAL_GPIO_TRIG_HIGH = 5
};
typedef enum hal_gpio_irq_trigger hal_gpio_irq_trig_t;


/* Function proto for GPIO irq handler functions */
typedef void (*hal_gpio_irq_handler_t)(void *arg);

int hal_gpio_irq_init(int pin, hal_gpio_irq_handler_t handler, void *arg,
                      hal_gpio_irq_trig_t trig, hal_gpio_pull_t pull)
{
    // printf("gpio irq init\n");
    return 0;
}

void hal_gpio_irq_enable(int pin)
{
    // printf("gpio irq enable\n");
}

void hal_gpio_init_out(int led, int s)
{
    // printf("gpio init out\n");
}

void hal_gpio_write(int led, int s)
{
    // printf("led[%d]: %s\n", s == 0 ? "OFF" : "ON");
}

int hal_gpio_read(int pin)
{

}

int __atomic_load_4(int *p)
{
    return *p;
}

int __atomic_fetch_and_4(int *p, int v)
{
    return *p & v;
}

int __atomic_fetch_or_4(int *p, int v)
{
    return *p | v;
}

int __atomic_exchange_4(int *p, int v)
{
    int old = *p;
    *p = v;
    return old;
}

void unreachable(void)
{
    while (1) {
        ;
    }
}

///////////////////////


static void light_default_var_init(void)
{
	gen_def_trans_time_srv_user_data.tt = 0x00;

	gen_power_onoff_srv_user_data.onpowerup = STATE_DEFAULT;

	light_lightness_srv_user_data.light_range_min = LIGHTNESS_MIN;
	light_lightness_srv_user_data.light_range_max = LIGHTNESS_MAX;
	light_lightness_srv_user_data.last = LIGHTNESS_MAX;
	light_lightness_srv_user_data.def = LIGHTNESS_MAX;

	/* Following 2 values are as per specification */
	light_ctl_srv_user_data.temp_range_min = TEMP_MIN;
	light_ctl_srv_user_data.temp_range_max = TEMP_MAX;

	light_ctl_srv_user_data.temp_def = TEMP_MIN;

	light_ctl_srv_user_data.lightness_temp_last =
		(u32_t) ((LIGHTNESS_MAX << 16) | TEMP_MIN);
}

static void light_default_status_init(void)
{
	u16_t lightness;

	lightness = (u16_t) (light_ctl_srv_user_data.lightness_temp_last >> 16);

	if (lightness) {
		gen_onoff_srv_root_user_data.onoff = STATE_ON;
	} else {
		gen_onoff_srv_root_user_data.onoff = STATE_OFF;
	}

	/* Retrieve Default Lightness & Temperature Values */

	if (light_ctl_srv_user_data.lightness_temp_def) {
		light_ctl_srv_user_data.lightness_def = (u16_t)
			(light_ctl_srv_user_data.lightness_temp_def >> 16);

		light_ctl_srv_user_data.temp_def = (u16_t)
			(light_ctl_srv_user_data.lightness_temp_def);
	}

	light_lightness_srv_user_data.def =
		light_ctl_srv_user_data.lightness_def;

	light_ctl_srv_user_data.temp = light_ctl_srv_user_data.temp_def;

	/* Retrieve Range of Lightness & Temperature */

	if (light_lightness_srv_user_data.lightness_range) {
		light_lightness_srv_user_data.light_range_max = (u16_t)
			(light_lightness_srv_user_data.lightness_range >> 16);

		light_lightness_srv_user_data.light_range_min = (u16_t)
			(light_lightness_srv_user_data.lightness_range);
	}

	if (light_ctl_srv_user_data.temperature_range) {
		light_ctl_srv_user_data.temp_range_max = (u16_t)
			(light_ctl_srv_user_data.temperature_range >> 16);

		light_ctl_srv_user_data.temp_range_min = (u16_t)
			(light_ctl_srv_user_data.temperature_range);
	}

	switch (gen_power_onoff_srv_user_data.onpowerup) {
		case STATE_OFF:
			gen_onoff_srv_root_user_data.onoff = STATE_OFF;
			state_binding(ONOFF, ONOFF_TEMP);
			break;
		case STATE_DEFAULT:
			gen_onoff_srv_root_user_data.onoff = STATE_ON;
			state_binding(ONOFF, ONOFF_TEMP);
			break;
		case STATE_RESTORE:
			light_lightness_srv_user_data.last = (u16_t)
				(light_ctl_srv_user_data.lightness_temp_last >> 16);

			light_ctl_srv_user_data.temp =
				(u16_t) (light_ctl_srv_user_data.lightness_temp_last);

			state_binding(ONPOWERUP, ONOFF_TEMP);
			break;
	}

	default_tt = gen_def_trans_time_srv_user_data.tt;
}

void update_light_state(void)
{
	u8_t power, color;

	power = 100 * ((float) lightness / 65535);
	color = 100 * ((float) (temperature + 32768) / 65535);

	printk("power-> %d, color-> %d\n", power, color);

	if (lightness) {
		/* LED1 On */
		hal_gpio_write(led_device[0], 0);
	} else {
		/* LED1 Off */
		hal_gpio_write(led_device[0], 1);
	}

	if (power < 50) {
		/* LED3 On */
		hal_gpio_write(led_device[2], 0);
	} else {
		/* LED3 Off */
		hal_gpio_write(led_device[2], 1);
	}

	if (color < 50) {
		/* LED4 On */
		hal_gpio_write(led_device[3], 0);
	} else {
		/* LED4 Off */
		hal_gpio_write(led_device[3], 1);
	}

	if (*ptr_counter == 0 || reset == false) {
		reset = true;
		ble_npl_callout_reset(&no_transition_work, 0);
	}
}

static void short_time_multireset_bt_mesh_unprovisioning(void)
{
	if (reset_counter >= 4) {
		reset_counter = 0;
		printk("BT Mesh reset\n");
		bt_mesh_reset();
	} else {
		printk("Reset Counter -> %d\n", reset_counter);
		reset_counter++;
	}

	save_on_flash(RESET_COUNTER);
}

static void reset_counter_timer_handler(struct ble_npl_event *dummy)
{
	reset_counter = 0;
	save_on_flash(RESET_COUNTER);
	printk("Reset Counter set to Zero\n");
}

struct ble_npl_callout reset_counter_timer;

static void init_timers(void)
{

	ble_npl_callout_init(&reset_counter_timer, nimble_port_get_dflt_eventq(),
			reset_counter_timer_handler, NULL);
	ble_npl_callout_reset(&reset_counter_timer,
			 ble_npl_time_ms_to_ticks32(K_MSEC(7000)));

	no_transition_work_init();
}

void bt_initialized(void)
{
	light_default_status_init();

	update_light_state();

	randomize_publishers_TID();

	short_time_multireset_bt_mesh_unprovisioning();
}

int ble_boot(void)
{
#ifdef ARCH_sim
	mcu_sim_parse_args(argc, argv);
#endif

	light_default_var_init();

	app_gpio_init();

	init_timers();

	transition_timers_init();

	init_pub();

#if 0 // disable this...
	ps_settings_init();
#endif

	printk("Initializing...\n");

	/* Initialize the NimBLE host configuration. */
	ble_hs_cfg.reset_cb = blemesh_on_reset;
	ble_hs_cfg.sync_cb = blemesh_on_sync;
	ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    extern void nimble_port_run(void);
    nimble_port_tencentos_tiny_init(nimble_port_run);

	return 0;
}


k_task_t ble_boot_task;
k_stack_t ble_boot_stack[512];

int main(void)
{
    board_init();

    /* Initialize OS */
    tos_knl_init();

    nimble_port_init();

    tos_task_create(&ble_boot_task, "boot", ble_boot, NULL,
                        4,
                        ble_boot_stack, sizeof(ble_boot_stack),
                        0);
    tos_knl_start();
}
