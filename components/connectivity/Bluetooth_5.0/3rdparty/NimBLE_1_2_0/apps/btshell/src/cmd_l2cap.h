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

#ifndef CMD_L2CAP_H
#define CMD_L2CAP_H

#include "cmd.h"

int cmd_l2cap_update(int argc, char **argv);
int cmd_l2cap_create_server(int argc, char **argv);
int cmd_l2cap_connect(int argc, char **argv);
int cmd_l2cap_disconnect(int argc, char **argv);
int cmd_l2cap_send(int argc, char **argv);
int cmd_l2cap_show_coc(int argc, char **argv);

#endif
