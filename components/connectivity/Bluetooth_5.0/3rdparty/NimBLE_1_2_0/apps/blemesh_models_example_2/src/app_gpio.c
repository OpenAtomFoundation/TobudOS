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

#include "bsp/bsp.h"
#include "console/console.h"
#include "hal/hal_gpio.h"
#include "mesh/mesh.h"

#include "app_gpio.h"
#include "publisher.h"

int button_device[] = {
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
};

int led_device[] = {
	LED_1,
	LED_2,
	LED_3,
	LED_4,
};

static struct os_callout button_work;

static void button_pressed(struct os_event *ev)
{
	os_callout_reset(&button_work, 0);
}

static struct os_event button_event;

static void gpio_irq_handler(void *arg)
{
	button_event.ev_arg = arg;
	os_eventq_put(os_eventq_dflt_get(), &button_event);
}

void app_gpio_init(void)
{
	/* LEDs configiuratin & setting */

	hal_gpio_init_out(led_device[0], 1);
	hal_gpio_init_out(led_device[1], 1);
	hal_gpio_init_out(led_device[2], 1);
	hal_gpio_init_out(led_device[3], 1);

	/* Buttons configiuratin & setting */

	os_callout_init(&button_work, os_eventq_dflt_get(), publish, NULL);

	button_event.ev_cb = button_pressed;

	hal_gpio_irq_init(button_device[0], gpio_irq_handler, NULL,
			  HAL_GPIO_TRIG_FALLING, HAL_GPIO_PULL_UP);
	hal_gpio_irq_enable(button_device[0]);

	hal_gpio_irq_init(button_device[1], gpio_irq_handler, NULL,
			  HAL_GPIO_TRIG_FALLING, HAL_GPIO_PULL_UP);
	hal_gpio_irq_enable(button_device[1]);

	hal_gpio_irq_init(button_device[2], gpio_irq_handler, NULL,
			  HAL_GPIO_TRIG_FALLING, HAL_GPIO_PULL_UP);
	hal_gpio_irq_enable(button_device[2]);

	hal_gpio_irq_init(button_device[3], gpio_irq_handler, NULL,
			  HAL_GPIO_TRIG_FALLING, HAL_GPIO_PULL_UP);
	hal_gpio_irq_enable(button_device[3]);
}

