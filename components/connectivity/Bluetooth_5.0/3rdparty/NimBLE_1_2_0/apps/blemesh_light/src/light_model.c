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

#include "syscfg/syscfg.h"

#if MYNEWT_VAL(BLE_MESH_SHELL_MODELS)

#include "mesh/mesh.h"
#include "bsp.h"
#include "pwm/pwm.h"
#include "light_model.h"
#include "ws2812.h"

#if (!MYNEWT_VAL(USE_NEOPIXEL))
#if MYNEWT_VAL(PWM_0)
struct pwm_dev *pwm0;
#endif
#if MYNEWT_VAL(PWM_1)
struct pwm_dev *pwm1;
#endif
#if MYNEWT_VAL(PWM_2)
struct pwm_dev *pwm2;
#endif
#if MYNEWT_VAL(PWM_3)
struct pwm_dev *pwm3;
#endif

static uint16_t top_val;
#endif

#if (MYNEWT_VAL(USE_NEOPIXEL))
static uint32_t neopixel[WS2812_NUM_LED];
#endif

static u8_t gen_onoff_state;
static s16_t gen_level_state;

static void light_set_lightness(u8_t percentage)
{
#if (!MYNEWT_VAL(USE_NEOPIXEL))
	int rc;

	uint16_t pwm_val = (uint16_t) (percentage * top_val / 100);

#if MYNEWT_VAL(PWM_0)
	rc = pwm_set_duty_cycle(pwm0, 0, pwm_val);
	assert(rc == 0);
#endif
#if MYNEWT_VAL(PWM_1)
	rc = pwm_set_duty_cycle(pwm1, 0, pwm_val);
	assert(rc == 0);
#endif
#if MYNEWT_VAL(PWM_2)
	rc = pwm_set_duty_cycle(pwm2, 0, pwm_val);
	assert(rc == 0);
#endif
#if MYNEWT_VAL(PWM_3)
	rc = pwm_set_duty_cycle(pwm3, 0, pwm_val);
	assert(rc == 0);
#endif
#else
	int i;
	u32_t lightness;
	u8_t max_lightness = 0x1f;

	lightness = (u8_t) (percentage * max_lightness / 100);

	for (i = 0; i < WS2812_NUM_LED; i++) {
		neopixel[i] = (lightness | lightness << 8 | lightness << 16);
	}
	ws2812_write(neopixel);
#endif
}

static void update_light_state(void)
{
	u16_t level = (u16_t)gen_level_state;
	int percent = 100 * level / 0xffff;

	if (gen_onoff_state == 0) {
		percent = 0;
	}
	light_set_lightness((uint8_t) percent);
}

int light_model_gen_onoff_get(struct bt_mesh_model *model, u8_t *state)
{
	*state = gen_onoff_state;
	return 0;
}

int light_model_gen_onoff_set(struct bt_mesh_model *model, u8_t state)
{
	gen_onoff_state = state;
	update_light_state();
	return 0;
}

int light_model_gen_level_get(struct bt_mesh_model *model, s16_t *level)
{
	*level = gen_level_state;
	return 0;
}

int light_model_gen_level_set(struct bt_mesh_model *model, s16_t level)
{
	gen_level_state = level;
	if ((u16_t)gen_level_state > 0x0000) {
		gen_onoff_state = 1;
	}
	if ((u16_t)gen_level_state == 0x0000) {
		gen_onoff_state = 0;
	}
	update_light_state();
	return 0;
}

int light_model_light_lightness_get(struct bt_mesh_model *model, s16_t *lightness)
{
	return light_model_gen_level_get(model, lightness);
}

int light_model_light_lightness_set(struct bt_mesh_model *model, s16_t lightness)
{
	return light_model_gen_level_set(model, lightness);
}

#if (!MYNEWT_VAL(USE_NEOPIXEL))
struct pwm_dev_cfg dev_conf = {
	.n_cycles = 0,
	.int_prio = 3,
};

#if MYNEWT_VAL(PWM_0)
static struct pwm_chan_cfg led1_conf = {
	.pin = LED_1,
	.inverted = true,
};
#endif

#if MYNEWT_VAL(PWM_1)
static struct pwm_chan_cfg led2_conf = {
	.pin = LED_2,
	.inverted = true,
};
#endif

#if MYNEWT_VAL(PWM_2)
static struct pwm_chan_cfg led3_conf = {
	.pin = LED_3,
	.inverted = true,
};
#endif
#endif

#if MYNEWT_VAL(PWM_3)
static struct pwm_chan_cfg led4_conf = {
	.pin = LED_4,
	.inverted = true,
};
#endif

#if (!MYNEWT_VAL(USE_NEOPIXEL))
void init_pwm_dev(struct pwm_dev **pwm, char *dev_name, struct pwm_chan_cfg *chan_cfg)
{
	int rc = 0;

	*pwm = (struct pwm_dev *) os_dev_open(dev_name, 0, NULL);
	assert(pwm);
	rc = pwm_configure_device(*pwm, &dev_conf);
	assert(rc == 0);
	rc = pwm_configure_channel(*pwm, 0, chan_cfg);
	assert(rc == 0);
	rc = pwm_enable(*pwm);
	assert(rc == 0);
}

int pwm_init(void)
{

#if MYNEWT_VAL(PWM_0)
	init_pwm_dev(&pwm0, "pwm0", &led1_conf);
#endif

#if MYNEWT_VAL(PWM_1)
	init_pwm_dev(&pwm1, "pwm1", &led2_conf);
#endif

#if MYNEWT_VAL(PWM_2)
	init_pwm_dev(&pwm2, "pwm2", &led3_conf);
#endif

#if MYNEWT_VAL(PWM_3)
	init_pwm_dev(&pwm3, "pwm3", &led4_conf);
#endif

	if (!pwm0) {
		return 0;
	}

	top_val = (uint16_t) pwm_get_top_value(pwm0);
	update_light_state();

	return 0;
}
#endif
#endif

int light_model_init(void)
{
#if MYNEWT_VAL(BLE_MESH_SHELL_MODELS)
	int rc;
#if (!MYNEWT_VAL(USE_NEOPIXEL))
	rc = pwm_init();
	assert(rc == 0);
#else
	rc = ws2812_init();
	assert(rc == 0);
	update_light_state();
#endif
	return rc;
#else
	return 0;
#endif
}

