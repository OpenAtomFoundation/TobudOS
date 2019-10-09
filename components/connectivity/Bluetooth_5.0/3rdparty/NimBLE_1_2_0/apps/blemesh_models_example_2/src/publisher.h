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

#ifndef _PUBLISHER_H
#define _PUBLISHER_H

/* Others */
#define LEVEL_S0   -32768
#define LEVEL_S25  -16384
#define LEVEL_S50  0
#define LEVEL_S75  16384
#define LEVEL_S100 32767

#define LEVEL_U0   0
#define LEVEL_U25  16384
#define LEVEL_U50  32768
#define LEVEL_U75  49152
#define LEVEL_U100 65535

void randomize_publishers_TID(void);
void publish(struct os_event *work);

#endif
