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

#ifndef CMD_GATT_H
#define CMD_GATT_H

#include "cmd.h"

int cmd_gatt_discover_characteristic(int argc, char **argv);
int cmd_gatt_discover_descriptor(int argc, char **argv);
int cmd_gatt_discover_service(int argc, char **argv);
int cmd_gatt_discover_full(int argc, char **argv);
int cmd_gatt_find_included_services(int argc, char **argv);
int cmd_gatt_exchange_mtu(int argc, char **argv);
int cmd_gatt_notify(int argc, char **argv);
int cmd_gatt_read(int argc, char **argv);
int cmd_gatt_service_changed(int argc, char **argv);
int cmd_gatt_service_visibility(int argc, char **argv);
int cmd_gatt_show(int argc, char **argv);
int cmd_gatt_show_local(int argc, char **argv);
int cmd_gatt_write(int argc, char **argv);

#endif
