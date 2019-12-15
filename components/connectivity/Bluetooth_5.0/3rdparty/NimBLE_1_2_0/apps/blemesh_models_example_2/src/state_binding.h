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

#ifndef _STATE_BINDING_H
#define _STATE_BINDING_H

enum state_binding {
	ONPOWERUP = 0x01,
	ONOFF,
	LEVEL,
	DELTA_LEVEL,
	ACTUAL,
	LINEAR,
	CTL,
	IGNORE,

	ONOFF_TEMP,
	LEVEL_TEMP,
	CTL_TEMP,
	IGNORE_TEMP
};

extern u16_t lightness, target_lightness;
extern s16_t temperature, target_temperature;

void state_binding(u8_t lightness, u8_t temperature);
void calculate_lightness_target_values(u8_t type);
void calculate_temp_target_values(u8_t type);

#endif
