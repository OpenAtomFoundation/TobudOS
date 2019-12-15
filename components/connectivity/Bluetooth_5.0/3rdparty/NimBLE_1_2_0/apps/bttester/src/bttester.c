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

/* bttester.c - Bluetooth Tester */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "syscfg/syscfg.h"
#include "console/console.h"

#include "bttester_pipe.h"
#include "bttester.h"

#define CMD_QUEUED 2

static struct os_eventq avail_queue;
static struct os_eventq *cmds_queue;
static struct os_event bttester_ev[CMD_QUEUED];

struct btp_buf {
	struct os_event *ev;
	union {
		u8_t data[BTP_MTU];
		struct btp_hdr hdr;
	};
};

static struct btp_buf cmd_buf[CMD_QUEUED];

static void supported_commands(u8_t *data, u16_t len)
{
	u8_t buf[1];
	struct core_read_supported_commands_rp *rp = (void *) buf;

	memset(buf, 0, sizeof(buf));

	tester_set_bit(buf, CORE_READ_SUPPORTED_COMMANDS);
	tester_set_bit(buf, CORE_READ_SUPPORTED_SERVICES);
	tester_set_bit(buf, CORE_REGISTER_SERVICE);
	tester_set_bit(buf, CORE_UNREGISTER_SERVICE);

	tester_send(BTP_SERVICE_ID_CORE, CORE_READ_SUPPORTED_COMMANDS,
		    BTP_INDEX_NONE, (u8_t *) rp, sizeof(buf));
}

static void supported_services(u8_t *data, u16_t len)
{
	u8_t buf[1];
	struct core_read_supported_services_rp *rp = (void *) buf;

	memset(buf, 0, sizeof(buf));

	tester_set_bit(buf, BTP_SERVICE_ID_CORE);
	tester_set_bit(buf, BTP_SERVICE_ID_GAP);
	tester_set_bit(buf, BTP_SERVICE_ID_GATT);
#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM)
	tester_set_bit(buf, BTP_SERVICE_ID_L2CAP);
#endif /* MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) */
#if MYNEWT_VAL(BLE_MESH)
	tester_set_bit(buf, BTP_SERVICE_ID_MESH);
#endif /* MYNEWT_VAL(BLE_MESH) */

	tester_send(BTP_SERVICE_ID_CORE, CORE_READ_SUPPORTED_SERVICES,
		    BTP_INDEX_NONE, (u8_t *) rp, sizeof(buf));
}

static void register_service(u8_t *data, u16_t len)
{
	struct core_register_service_cmd *cmd = (void *) data;
	u8_t status;

	switch (cmd->id) {
	case BTP_SERVICE_ID_GAP:
		status = tester_init_gap();
		/* Rsp with success status will be handled by bt enable cb */
		if (status == BTP_STATUS_FAILED) {
			goto rsp;
		}
		return;
	case BTP_SERVICE_ID_GATT:
		status = tester_init_gatt();
		break;
#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM)
	case BTP_SERVICE_ID_L2CAP:
		status = tester_init_l2cap();
		break;
#endif /* MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) */
#if MYNEWT_VAL(BLE_MESH)
	case BTP_SERVICE_ID_MESH:
		status = tester_init_mesh();
		break;
#endif /* MYNEWT_VAL(BLE_MESH) */
	default:
		status = BTP_STATUS_FAILED;
		break;
	}

rsp:
	tester_rsp(BTP_SERVICE_ID_CORE, CORE_REGISTER_SERVICE, BTP_INDEX_NONE,
		   status);
}

static void unregister_service(u8_t *data, u16_t len)
{
	struct core_unregister_service_cmd *cmd = (void *) data;
	u8_t status;

	switch (cmd->id) {
	case BTP_SERVICE_ID_GAP:
		status = tester_unregister_gap();
		break;
	case BTP_SERVICE_ID_GATT:
		status = tester_unregister_gatt();
		break;
#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM)
	case BTP_SERVICE_ID_L2CAP:
		status = tester_unregister_l2cap();
		break;
#endif /* MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) */
#if MYNEWT_VAL(BLE_MESH)
	case BTP_SERVICE_ID_MESH:
		status = tester_unregister_mesh();
		break;
#endif /* MYNEWT_VAL(BLE_MESH) */
	default:
		status = BTP_STATUS_FAILED;
		break;
	}

	tester_rsp(BTP_SERVICE_ID_CORE, CORE_UNREGISTER_SERVICE, BTP_INDEX_NONE,
		   status);
}

static void handle_core(u8_t opcode, u8_t index, u8_t *data,
			u16_t len)
{
	if (index != BTP_INDEX_NONE) {
		tester_rsp(BTP_SERVICE_ID_CORE, opcode, index,
			   BTP_STATUS_FAILED);
		return;
	}

	switch (opcode) {
	case CORE_READ_SUPPORTED_COMMANDS:
		supported_commands(data, len);
		return;
	case CORE_READ_SUPPORTED_SERVICES:
		supported_services(data, len);
		return;
	case CORE_REGISTER_SERVICE:
		register_service(data, len);
		return;
	case CORE_UNREGISTER_SERVICE:
		unregister_service(data, len);
		return;
	default:
		tester_rsp(BTP_SERVICE_ID_CORE, opcode, BTP_INDEX_NONE,
			   BTP_STATUS_UNKNOWN_CMD);
		return;
	}
}

static void cmd_handler(struct os_event *ev)
{
	u16_t len;
	struct btp_buf *cmd;

	if (!ev || !ev->ev_arg) {
		return;
	}

	cmd = ev->ev_arg;

	len = sys_le16_to_cpu(cmd->hdr.len);
	if (MYNEWT_VAL(BTTESTER_BTP_LOG)) {
		console_printf("[DBG] received %d bytes: %s\n",
			       sizeof(cmd->hdr) + len,
			       bt_hex(cmd->data,
				      sizeof(cmd->hdr) + len));
	}

	/* TODO
	 * verify if service is registered before calling handler
	 */

	switch (cmd->hdr.service) {
		case BTP_SERVICE_ID_CORE:
			handle_core(cmd->hdr.opcode, cmd->hdr.index,
				    cmd->hdr.data, len);
			break;
		case BTP_SERVICE_ID_GAP:
			tester_handle_gap(cmd->hdr.opcode, cmd->hdr.index,
					  cmd->hdr.data, len);
			break;
		case BTP_SERVICE_ID_GATT:
			tester_handle_gatt(cmd->hdr.opcode, cmd->hdr.index,
					   cmd->hdr.data, len);
			break;
#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM)
		case BTP_SERVICE_ID_L2CAP:
			tester_handle_l2cap(cmd->hdr.opcode, cmd->hdr.index,
					    cmd->hdr.data, len);
			break;
#endif /* MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) */
#if MYNEWT_VAL(BLE_MESH)
		case BTP_SERVICE_ID_MESH:
			tester_handle_mesh(cmd->hdr.opcode, cmd->hdr.index,
					   cmd->hdr.data, len);
			break;
#endif /* MYNEWT_VAL(BLE_MESH) */
		default:
			tester_rsp(cmd->hdr.service, cmd->hdr.opcode,
				   cmd->hdr.index, BTP_STATUS_FAILED);
			break;
	}

	os_eventq_put(&avail_queue, ev);
}

static u8_t *recv_cb(u8_t *buf, size_t *off)
{
	struct btp_hdr *cmd = (void *) buf;
	struct os_event *new_ev;
	struct btp_buf *new_buf, *old_buf;
	u16_t len;

	if (*off < sizeof(*cmd)) {
		return buf;
	}

	len = sys_le16_to_cpu(cmd->len);
	if (len > BTP_MTU - sizeof(*cmd)) {
		SYS_LOG_ERR("BT tester: invalid packet length");
		*off = 0;
		return buf;
	}

	if (*off < sizeof(*cmd) + len) {
		return buf;
	}

	new_ev = os_eventq_get_no_wait(&avail_queue);
	if (!new_ev) {
		SYS_LOG_ERR("BT tester: RX overflow");
		*off = 0;
		return buf;
	}

	old_buf = CONTAINER_OF(buf, struct btp_buf, data);
	os_eventq_put(cmds_queue, old_buf->ev);

	new_buf = new_ev->ev_arg;
	*off = 0;
	return new_buf->data;
}

static void avail_queue_init(void)
{
	int i;

	os_eventq_init(&avail_queue);

	for (i = 0; i < CMD_QUEUED; i++) {
		cmd_buf[i].ev = &bttester_ev[i];
		bttester_ev[i].ev_cb = cmd_handler;
		bttester_ev[i].ev_arg = &cmd_buf[i];

		os_eventq_put(&avail_queue, &bttester_ev[i]);
	}
}

void bttester_evq_set(struct os_eventq *evq)
{
	cmds_queue = evq;
}

void tester_init(void)
{
	struct os_event *ev;
	struct btp_buf *buf;

	avail_queue_init();
	bttester_evq_set(os_eventq_dflt_get());

	ev = os_eventq_get(&avail_queue);
	buf = ev->ev_arg;

	if (bttester_pipe_init()) {
		SYS_LOG_ERR("Failed to initialize pipe");
		return;
	}

	bttester_pipe_register(buf->data, BTP_MTU, recv_cb);

	tester_send(BTP_SERVICE_ID_CORE, CORE_EV_IUT_READY, BTP_INDEX_NONE,
		    NULL, 0);
}

void tester_send(u8_t service, u8_t opcode, u8_t index, u8_t *data,
		 size_t len)
{
	struct btp_hdr msg;

	msg.service = service;
	msg.opcode = opcode;
	msg.index = index;
	msg.len = len;

	bttester_pipe_send((u8_t *)&msg, sizeof(msg));
	if (data && len) {
		bttester_pipe_send(data, len);
	}

	if (MYNEWT_VAL(BTTESTER_BTP_LOG)) {
		console_printf("[DBG] send %d bytes hdr: %s\n", sizeof(msg),
			       bt_hex((char *) &msg, sizeof(msg)));
		if (data && len) {
			console_printf("[DBG] send %d bytes data: %s\n", len,
				       bt_hex((char *) data, len));
		}
	}
}

void tester_rsp(u8_t service, u8_t opcode, u8_t index, u8_t status)
{
	struct btp_status s;

	if (status == BTP_STATUS_SUCCESS) {
		tester_send(service, opcode, index, NULL, 0);
		return;
	}

	s.code = status;
	tester_send(service, BTP_STATUS, index, (u8_t *) &s, sizeof(s));
}
