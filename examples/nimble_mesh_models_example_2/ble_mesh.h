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

#ifndef _BLE_MESH_H
#define _BLE_MESH_H

#include "mesh/mesh.h"
#include "mesh/glue.h"

/* Model Operation Codes */
#define BT_MESH_MODEL_OP_GEN_ONOFF_GET		BT_MESH_MODEL_OP_2(0x82, 0x01)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET		BT_MESH_MODEL_OP_2(0x82, 0x02)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK	BT_MESH_MODEL_OP_2(0x82, 0x03)
#define BT_MESH_MODEL_OP_GEN_ONOFF_STATUS	BT_MESH_MODEL_OP_2(0x82, 0x04)

#define BT_MESH_MODEL_OP_GEN_LEVEL_GET		BT_MESH_MODEL_OP_2(0x82, 0x05)
#define BT_MESH_MODEL_OP_GEN_LEVEL_SET		BT_MESH_MODEL_OP_2(0x82, 0x06)
#define BT_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK	BT_MESH_MODEL_OP_2(0x82, 0x07)
#define BT_MESH_MODEL_OP_GEN_LEVEL_STATUS	BT_MESH_MODEL_OP_2(0x82, 0x08)
#define BT_MESH_MODEL_OP_GEN_DELTA_SET		BT_MESH_MODEL_OP_2(0x82, 0x09)
#define BT_MESH_MODEL_OP_GEN_DELTA_SET_UNACK	BT_MESH_MODEL_OP_2(0x82, 0x0A)
#define BT_MESH_MODEL_OP_GEN_MOVE_SET		BT_MESH_MODEL_OP_2(0x82, 0x0B)
#define BT_MESH_MODEL_OP_GEN_MOVE_SET_UNACK	BT_MESH_MODEL_OP_2(0x82, 0x0C)

#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_STATUS	BT_MESH_MODEL_OP_2(0x82, 0x10)

#define BT_MESH_MODEL_GEN_ONPOWERUP_STATUS	BT_MESH_MODEL_OP_2(0x82, 0x12)

#define BT_MESH_MODEL_LIGHT_LIGHTNESS_STATUS	BT_MESH_MODEL_OP_2(0x82, 0x4E)
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_LINEAR_STATUS \
						BT_MESH_MODEL_OP_2(0x82, 0x52)
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_LAST_STATUS \
						BT_MESH_MODEL_OP_2(0x82, 0x54)
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_DEFAULT_STATUS \
						BT_MESH_MODEL_OP_2(0x82, 0x56)
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_RANGE_STATUS \
						BT_MESH_MODEL_OP_2(0x82, 0x58)

#define BT_MESH_MODEL_LIGHT_CTL_STATUS		BT_MESH_MODEL_OP_2(0x82, 0x60)
#define BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_STATUS \
						BT_MESH_MODEL_OP_2(0x82, 0x63)
#define BT_MESH_MODEL_LIGHT_CTL_TEMP_STATUS	BT_MESH_MODEL_OP_2(0x82, 0x66)
#define BT_MESH_MODEL_LIGHT_CTL_DEFAULT_STATUS	BT_MESH_MODEL_OP_2(0x82, 0x68)

void blemesh_on_reset(int reason);
void blemesh_on_sync(void);
void init_pub(void);

#endif

