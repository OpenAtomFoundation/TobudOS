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
 *
 * Copyright (c) 2017 Intel Corporation
 *
 */

#ifndef __BT_MESH_LIGHT_MODEL_H
#define __BT_MESH_LIGHT_MODEL_H

#include "syscfg/syscfg.h"
#include "mesh/mesh.h"

int light_model_gen_onoff_get(struct bt_mesh_model *model, u8_t *state);
int light_model_gen_onoff_set(struct bt_mesh_model *model, u8_t state);
int light_model_gen_level_get(struct bt_mesh_model *model, s16_t *level);
int light_model_gen_level_set(struct bt_mesh_model *model, s16_t level);
int light_model_light_lightness_get(struct bt_mesh_model *model, s16_t *lightness);
int light_model_light_lightness_set(struct bt_mesh_model *model, s16_t lightness);
int light_model_init(void);

#endif
