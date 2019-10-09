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
#include "common.h"
#include "device_composition.h"
#include "state_binding.h"
#include "transition.h"

struct os_callout onoff_work;
struct os_callout level_lightness_work;
struct os_callout level_temp_work;
struct os_callout light_lightness_actual_work;
struct os_callout light_lightness_linear_work;
struct os_callout light_ctl_work;
struct os_callout light_ctl_temp_work;

struct os_callout dummy_timer;

u8_t transition_type, default_tt;
u32_t *ptr_counter;
struct os_callout *ptr_timer = &dummy_timer;

struct transition lightness_transition, temp_transition;

/* Function to calculate Remaining Time (Start) */

void calculate_rt(struct transition *transition)
{
	u8_t steps, resolution;
	s32_t duration_remainder;
	s64_t now;

	if (transition->just_started) {
		transition->rt = transition->tt;
	} else {
		now = k_uptime_get();
		duration_remainder = transition->total_duration -
				     (now - transition->start_timestamp);

		if (duration_remainder > 620000) {
			/* > 620 seconds -> resolution = 0b11 [10 minutes] */
			resolution = 0x03;
			steps = duration_remainder / 600000;
		} else if (duration_remainder > 62000) {
			/* > 62 seconds -> resolution = 0b10 [10 seconds] */
			resolution = 0x02;
			steps = duration_remainder / 10000;
		} else if (duration_remainder > 6200) {
			/* > 6.2 seconds -> resolution = 0b01 [1 seconds] */
			resolution = 0x01;
			steps = duration_remainder / 1000;
		} else if (duration_remainder > 0) {
			/* <= 6.2 seconds -> resolution = 0b00 [100 ms] */
			resolution = 0x00;
			steps = duration_remainder / 100;
		} else {
			resolution = 0x00;
			steps = 0x00;
		}

		transition->rt = (resolution << 6) | steps;
	}
}

/* Function to calculate Remaining Time (End) */

static void bound_states_transition_type_reassignment(u8_t type)
{
	switch (type) {
	case ONOFF:
	case LEVEL:
	case ACTUAL:
	case LINEAR:
		light_ctl_srv_user_data.transition = &lightness_transition;
		break;
	case CTL:
		light_ctl_srv_user_data.transition = &lightness_transition;
		gen_level_srv_s0_user_data.transition = &lightness_transition;
		break;
	case LEVEL_TEMP:
	case CTL_TEMP:
		gen_level_srv_s0_user_data.transition = &temp_transition;
		light_ctl_srv_user_data.transition = &temp_transition;
		break;
	default:
		break;
	}
}

static void tt_values_calculator(struct transition *transition)
{
	u8_t steps_multiplier, resolution;

	resolution = (transition->tt >> 6);
	steps_multiplier = (transition->tt & 0x3F);

	switch (resolution) {
	case 0:	/* 100ms */
		transition->total_duration = steps_multiplier * 100;
		break;
	case 1:	/* 1 second */
		transition->total_duration = steps_multiplier * 1000;
		break;
	case 2:	/* 10 seconds */
		transition->total_duration = steps_multiplier * 10000;
		break;
	case 3:	/* 10 minutes */
		transition->total_duration = steps_multiplier * 600000;
		break;
	}

	transition->counter = ((float) transition->total_duration / 100);

	if (transition->counter > DEVICE_SPECIFIC_RESOLUTION) {
		transition->counter = DEVICE_SPECIFIC_RESOLUTION;
	}

	ptr_counter = &transition->counter;
}

void onoff_tt_values(struct generic_onoff_state *state, u8_t tt, u8_t delay)
{
	bound_states_transition_type_reassignment(ONOFF);
	calculate_lightness_target_values(ONOFF);
	state->transition->tt = tt;
	state->transition->delay = delay;

	if (tt != 0) {
		tt_values_calculator(state->transition);
	} else {
		return;
	}

	state->transition->quo_tt = state->transition->total_duration /
				    state->transition->counter;

	state->tt_delta = ((float) (lightness - target_lightness) /
			   state->transition->counter);
}

void level_tt_values(struct generic_level_state *state, u8_t tt, u8_t delay)
{
	if (state == &gen_level_srv_root_user_data) {
		bound_states_transition_type_reassignment(LEVEL);
		calculate_lightness_target_values(LEVEL);
	} else if (state == &gen_level_srv_s0_user_data) {
		bound_states_transition_type_reassignment(LEVEL_TEMP);
		calculate_temp_target_values(LEVEL_TEMP);
	}
	state->transition->tt = tt;
	state->transition->delay = delay;

	if (tt != 0) {
		tt_values_calculator(state->transition);
	} else {
		return;
	}

	state->transition->quo_tt = state->transition->total_duration /
				    state->transition->counter;

	state->tt_delta = ((float) (state->level - state->target_level) /
			   state->transition->counter);
}

void light_lightness_actual_tt_values(struct light_lightness_state *state,
				      u8_t tt, u8_t delay)
{
	bound_states_transition_type_reassignment(ACTUAL);
	calculate_lightness_target_values(ACTUAL);
	state->transition->tt = tt;
	state->transition->delay = delay;

	if (tt != 0) {
		tt_values_calculator(state->transition);
	} else {
		return;
	}

	state->transition->quo_tt = state->transition->total_duration /
				    state->transition->counter;

	state->tt_delta_actual =
		((float) (state->actual - state->target_actual) /
		 state->transition->counter);
}

void light_lightness_linear_tt_values(struct light_lightness_state *state,
				      u8_t tt, u8_t delay)
{
	bound_states_transition_type_reassignment(LINEAR);
	calculate_lightness_target_values(LINEAR);
	state->transition->tt = tt;
	state->transition->delay = delay;

	if (tt != 0) {
		tt_values_calculator(state->transition);
	} else {
		return;
	}

	state->transition->quo_tt = state->transition->total_duration /
				    state->transition->counter;

	state->tt_delta_linear =
		((float) (state->linear - state->target_linear) /
		 state->transition->counter);
}

void light_ctl_tt_values(struct light_ctl_state *state, u8_t tt, u8_t delay)
{
	bound_states_transition_type_reassignment(CTL);
	calculate_lightness_target_values(CTL);
	state->transition->tt = tt;
	state->transition->delay = delay;

	if (tt != 0) {
		tt_values_calculator(state->transition);
	} else {
		return;
	}

	state->transition->quo_tt = state->transition->total_duration /
				    state->transition->counter;

	state->tt_delta_lightness =
		((float) (state->lightness - state->target_lightness) /
		 state->transition->counter);

	state->tt_delta_temp =
		((float) (state->temp - state->target_temp) /
		 state->transition->counter);

	state->tt_delta_duv =
		((float) (state->delta_uv - state->target_delta_uv) /
		 state->transition->counter);
}

void light_ctl_temp_tt_values(struct light_ctl_state *state,
			      u8_t tt, u8_t delay)
{
	bound_states_transition_type_reassignment(CTL_TEMP);
	calculate_temp_target_values(CTL_TEMP);
	state->transition->tt = tt;
	state->transition->delay = delay;

	if (tt != 0) {
		tt_values_calculator(state->transition);
	} else {
		return;
	}

	state->transition->quo_tt = state->transition->total_duration /
				    state->transition->counter;

	state->tt_delta_temp = ((float) (state->temp - state->target_temp) /
				state->transition->counter);

	state->tt_delta_duv =
		((float) (state->delta_uv - state->target_delta_uv) /
		 state->transition->counter);
}

/* Timers related handlers & threads (Start) */
static void onoff_work_handler(struct os_event *work)
{
	struct generic_onoff_state *state = &gen_onoff_srv_root_user_data;

	if (state->transition->just_started) {
		state->transition->just_started = false;

		if (state->transition->counter == 0) {
			state_binding(ONOFF, IGNORE_TEMP);
			update_light_state();

			os_callout_stop(ptr_timer);
		} else {
			state->transition->start_timestamp = k_uptime_get();

			if (state->target_onoff == STATE_ON) {
				state->onoff = STATE_ON;
			}
		}

		return;
	}

	if (state->transition->counter != 0) {
		state->transition->counter--;

		lightness -= state->tt_delta;

		state_binding(IGNORE, IGNORE_TEMP);
		update_light_state();
	}

	if (state->transition->counter == 0) {
		state->onoff = state->target_onoff;
		lightness = target_lightness;

		state_binding(IGNORE, IGNORE_TEMP);
		update_light_state();

		os_callout_stop(ptr_timer);
	}
}

static void level_lightness_work_handler(struct os_event *work)
{
	u8_t level;
	struct generic_level_state *state = &gen_level_srv_root_user_data;

	switch (transition_type) {
	case LEVEL_TT:
		level = LEVEL;
		break;
	case LEVEL_TT_DELTA:
		level = DELTA_LEVEL;
		break;
	case LEVEL_TT_MOVE:
		level = LEVEL;
		break;
	default:
		return;
	}

	if (state->transition->just_started) {
		state->transition->just_started = false;

		if (state->transition->counter == 0) {
			state_binding(level, IGNORE_TEMP);
			update_light_state();

			os_callout_stop(ptr_timer);
		} else {
			state->transition->start_timestamp = k_uptime_get();
		}

		return;
	}

	if (state->transition->counter != 0) {
		state->transition->counter--;

		state->level -= state->tt_delta;

		state_binding(level, IGNORE_TEMP);
		update_light_state();
	}

	if (state->transition->counter == 0) {
		state->level = state->target_level;

		state_binding(level, IGNORE_TEMP);
		update_light_state();

		os_callout_stop(ptr_timer);
	}
}

static void level_temp_work_handler(struct os_event *work)
{
	struct generic_level_state *state = &gen_level_srv_s0_user_data;

	switch (transition_type) {
	case LEVEL_TEMP_TT:
		break;
	case LEVEL_TEMP_TT_DELTA:
		break;
	case LEVEL_TEMP_TT_MOVE:
		break;
	default:
		return;
	}

	if (state->transition->just_started) {
		state->transition->just_started = false;

		if (state->transition->counter == 0) {
			state_binding(IGNORE, LEVEL_TEMP);
			update_light_state();

			os_callout_stop(ptr_timer);
		} else {
			state->transition->start_timestamp = k_uptime_get();
		}

		return;
	}

	if (state->transition->counter != 0) {
		state->transition->counter--;

		state->level -= state->tt_delta;

		state_binding(IGNORE, LEVEL_TEMP);
		update_light_state();
	}

	if (state->transition->counter == 0) {
		state->level = state->target_level;

		state_binding(IGNORE, LEVEL_TEMP);
		update_light_state();

		os_callout_stop(ptr_timer);
	}
}

static void light_lightness_actual_work_handler(struct os_event *work)
{
	struct light_lightness_state *state = &light_lightness_srv_user_data;

	if (state->transition->just_started) {
		state->transition->just_started = false;

		if (state->transition->counter == 0) {
			state_binding(ACTUAL, IGNORE_TEMP);
			update_light_state();

			os_callout_stop(ptr_timer);
		} else {
			state->transition->start_timestamp = k_uptime_get();
		}

		return;
	}

	if (state->transition->counter != 0) {
		state->transition->counter--;

		state->actual -= state->tt_delta_actual;

		state_binding(ACTUAL, IGNORE_TEMP);
		update_light_state();
	}

	if (state->transition->counter == 0) {
		state->actual = state->target_actual;

		state_binding(ACTUAL, IGNORE_TEMP);
		update_light_state();

		os_callout_stop(ptr_timer);
	}
}

static void light_lightness_linear_work_handler(struct os_event *work)
{
	struct light_lightness_state *state = &light_lightness_srv_user_data;

	if (state->transition->just_started) {
		state->transition->just_started = false;

		if (state->transition->counter == 0) {
			state_binding(LINEAR, IGNORE_TEMP);
			update_light_state();

			os_callout_stop(ptr_timer);
		} else {
			state->transition->start_timestamp = k_uptime_get();
		}

		return;
	}

	if (state->transition->counter != 0) {
		state->transition->counter--;

		state->linear -= state->tt_delta_linear;

		state_binding(LINEAR, IGNORE_TEMP);
		update_light_state();
	}

	if (state->transition->counter == 0) {
		state->linear = state->target_linear;

		state_binding(LINEAR, IGNORE_TEMP);
		update_light_state();

		os_callout_stop(ptr_timer);
	}
}

static void light_ctl_work_handler(struct os_event *work)
{
	struct light_ctl_state *state = &light_ctl_srv_user_data;

	if (state->transition->just_started) {
		state->transition->just_started = false;

		if (state->transition->counter == 0) {
			state_binding(CTL, CTL_TEMP);
			update_light_state();

			os_callout_stop(ptr_timer);
		} else {
			state->transition->start_timestamp = k_uptime_get();
		}

		return;
	}

	if (state->transition->counter != 0) {
		state->transition->counter--;

		/* Lightness */
		state->lightness -= state->tt_delta_lightness;

		/* Temperature */
		state->temp -= state->tt_delta_temp;

		/* Delta_UV */
		state->delta_uv -= state->tt_delta_duv;

		state_binding(CTL, CTL_TEMP);
		update_light_state();
	}

	if (state->transition->counter == 0) {
		state->lightness = state->target_lightness;
		state->temp = state->target_temp;
		state->delta_uv = state->target_delta_uv;

		state_binding(CTL, CTL_TEMP);
		update_light_state();

		os_callout_stop(ptr_timer);
	}
}

static void light_ctl_temp_work_handler(struct os_event *work)
{
	struct light_ctl_state *state = &light_ctl_srv_user_data;

	if (state->transition->just_started) {
		state->transition->just_started = false;

		if (state->transition->counter == 0) {
			state_binding(IGNORE, CTL_TEMP);
			update_light_state();

			os_callout_stop(ptr_timer);
		} else {
			state->transition->start_timestamp = k_uptime_get();
		}

		return;
	}

	if (state->transition->counter != 0) {
		state->transition->counter--;

		/* Temperature */
		state->temp -= state->tt_delta_temp;

		/* Delta UV */
		state->delta_uv -= state->tt_delta_duv;

		state_binding(IGNORE, CTL_TEMP);
		update_light_state();
	}

	if (state->transition->counter == 0) {
		state->temp = state->target_temp;
		state->delta_uv = state->target_delta_uv;

		state_binding(IGNORE, CTL_TEMP);
		update_light_state();

		os_callout_stop(ptr_timer);
	}
}

static void dummy_timer_handler(struct os_event *ev)
{ }

static void onoff_tt_handler(struct os_event *ev)
{
	struct generic_onoff_state *state = ev->ev_arg;

	assert(state != NULL);
	os_callout_reset(&onoff_work, 0);
	os_callout_reset(&state->transition->timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(state->transition->quo_tt)));
}

static void level_lightness_tt_handler(struct os_event *ev)
{
	struct generic_level_state *state = ev->ev_arg;

	assert(state != NULL);
	os_callout_reset(&level_lightness_work, 0);
	os_callout_reset(&state->transition->timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(state->transition->quo_tt)));
}

static void level_temp_tt_handler(struct os_event *ev)
{
	struct generic_level_state *state = ev->ev_arg;

	assert(state != NULL);
	os_callout_reset(&level_temp_work, 0);
	os_callout_reset(&state->transition->timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(state->transition->quo_tt)));
}

static void light_lightness_actual_tt_handler(struct os_event *ev)
{
	struct light_lightness_state *state = ev->ev_arg;

	assert(state != NULL);
	os_callout_reset(&light_lightness_actual_work, 0);
	os_callout_reset(&state->transition->timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(state->transition->quo_tt)));
}

static void light_lightness_linear_tt_handler(struct os_event *ev)
{
	struct light_lightness_state *state = ev->ev_arg;

	assert(state != NULL);
	os_callout_reset(&light_lightness_linear_work, 0);
	os_callout_reset(&state->transition->timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(state->transition->quo_tt)));
}

static void light_ctl_tt_handler(struct os_event *ev)
{
	struct light_ctl_state *state = ev->ev_arg;

	assert(state != NULL);
	os_callout_reset(&light_ctl_work, 0);
	os_callout_reset(&state->transition->timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(state->transition->quo_tt)));
}

static void light_ctl_temp_tt_handler(struct os_event *ev)
{
	struct light_ctl_state *state = ev->ev_arg;

	assert(state != NULL);
	os_callout_reset(&light_ctl_temp_work, 0);
	os_callout_reset(&state->transition->timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(state->transition->quo_tt)));
}
/* Timers related handlers & threads (End) */

/* Messages handlers (Start) */
void onoff_handler(struct generic_onoff_state *state)
{
	ptr_timer = &state->transition->timer;

	os_callout_init(ptr_timer, os_eventq_dflt_get(),
			onoff_tt_handler, NULL);
	ptr_timer->c_ev.ev_arg = state;
	os_callout_reset(ptr_timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(5 * state->transition->delay)));
}

void level_lightness_handler(struct generic_level_state *state)
{
	ptr_timer = &state->transition->timer;

	os_callout_init(ptr_timer, os_eventq_dflt_get(),
			level_lightness_tt_handler, NULL);
	ptr_timer->c_ev.ev_arg = state;
	os_callout_reset(ptr_timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(5 * state->transition->delay)));
}

void level_temp_handler(struct generic_level_state *state)
{
	ptr_timer = &state->transition->timer;

	os_callout_init(ptr_timer, os_eventq_dflt_get(),
			level_temp_tt_handler, NULL);
	ptr_timer->c_ev.ev_arg = state;
	os_callout_reset(ptr_timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(5 * state->transition->delay)));
}

void light_lightness_actual_handler(struct light_lightness_state *state)
{
	ptr_timer = &state->transition->timer;

	os_callout_init(ptr_timer, os_eventq_dflt_get(),
			light_lightness_actual_tt_handler, NULL);
	ptr_timer->c_ev.ev_arg = state;
	os_callout_reset(ptr_timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(5 * state->transition->delay)));
}

void light_lightness_linear_handler(struct light_lightness_state *state)
{
	ptr_timer = &state->transition->timer;

	os_callout_init(ptr_timer, os_eventq_dflt_get(),
			light_lightness_linear_tt_handler, NULL);
	ptr_timer->c_ev.ev_arg = state;
	os_callout_reset(ptr_timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(5 * state->transition->delay)));
}

void light_ctl_handler(struct light_ctl_state *state)
{
	ptr_timer = &state->transition->timer;

	os_callout_init(ptr_timer, os_eventq_dflt_get(),
			light_ctl_tt_handler, NULL);
	ptr_timer->c_ev.ev_arg = state;
	os_callout_reset(ptr_timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(5 * state->transition->delay)));
}

void light_ctl_temp_handler(struct light_ctl_state *state)
{
	ptr_timer = &state->transition->timer;

	os_callout_init(ptr_timer, os_eventq_dflt_get(),
			light_ctl_temp_tt_handler, NULL);
	ptr_timer->c_ev.ev_arg = state;
	os_callout_reset(ptr_timer,
			 os_time_ms_to_ticks32(
				 K_MSEC(5 * state->transition->delay)));
}
/* Messages handlers (End) */

void transition_timers_init(void)
{
	os_callout_init(&onoff_work, os_eventq_dflt_get(),
			onoff_work_handler, NULL);

	os_callout_init(&level_lightness_work, os_eventq_dflt_get(),
			level_lightness_work_handler, NULL);
	os_callout_init(&level_temp_work, os_eventq_dflt_get(),
			level_temp_work_handler, NULL);

	os_callout_init(&light_lightness_actual_work,
			os_eventq_dflt_get(),
			light_lightness_actual_work_handler, NULL);
	os_callout_init(&light_lightness_linear_work,
			os_eventq_dflt_get(),
			light_lightness_linear_work_handler, NULL);

	os_callout_init(&light_ctl_work, os_eventq_dflt_get(),
			light_ctl_work_handler, NULL);
	os_callout_init(&light_ctl_temp_work, os_eventq_dflt_get(),
			light_ctl_temp_work_handler, NULL);

	os_callout_init(&dummy_timer, os_eventq_dflt_get(),
			dummy_timer_handler, NULL);
}

