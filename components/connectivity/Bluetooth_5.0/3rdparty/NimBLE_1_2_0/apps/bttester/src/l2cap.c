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

/* l2cap.c - Bluetooth L2CAP Tester */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "syscfg/syscfg.h"

#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM)

#include "console/console.h"
#include "host/ble_gap.h"
#include "host/ble_l2cap.h"

#include "bttester.h"

#define CONTROLLER_INDEX             0
#define CHANNELS                     MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM)
#define TESTER_COC_MTU               (230)
#define TESTER_COC_BUF_COUNT         (3 * MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM))

static os_membuf_t tester_sdu_coc_mem[
	OS_MEMPOOL_SIZE(TESTER_COC_BUF_COUNT, TESTER_COC_MTU)
];

struct os_mbuf_pool sdu_os_mbuf_pool;
static struct os_mempool sdu_coc_mbuf_mempool;

static struct channel {
	u8_t chan_id; /* Internal number that identifies L2CAP channel. */
	u8_t state;
	struct ble_l2cap_chan *chan;
} channels[CHANNELS];

static u8_t recv_cb_buf[TESTER_COC_MTU + sizeof(struct l2cap_data_received_ev)];

struct channel *find_channel(struct ble_l2cap_chan *chan) {
	int i;

	for (i = 0; i < CHANNELS; ++i) {
		if (channels[i].chan == chan) {
			return &channels[i];
		}
	}

	return NULL;
}

static void
tester_l2cap_coc_recv(struct ble_l2cap_chan *chan, struct os_mbuf *sdu)
{
	SYS_LOG_DBG("LE CoC SDU received, chan: 0x%08lx, data len %d",
		    (uint32_t) chan, OS_MBUF_PKTLEN(sdu));

	os_mbuf_free_chain(sdu);
	sdu = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
	assert(sdu != NULL);

	ble_l2cap_recv_ready(chan, sdu);
}

static void recv_cb(uint16_t conn_handle, struct ble_l2cap_chan *chan,
		    struct os_mbuf *buf, void *arg)
{
	struct l2cap_data_received_ev *ev = (void *) recv_cb_buf;
	struct channel *channel = arg;

	ev->chan_id = channel->chan_id;
	ev->data_length = buf->om_len;
	memcpy(ev->data, buf->om_data, buf->om_len);

	tester_send(BTP_SERVICE_ID_L2CAP, L2CAP_EV_DATA_RECEIVED,
		    CONTROLLER_INDEX, recv_cb_buf, sizeof(*ev) + buf->om_len);

	tester_l2cap_coc_recv(chan, buf);
}

static void unstalled_cb(uint16_t conn_handle, struct ble_l2cap_chan *chan,
			 int status, void *arg)
{
	if (status) {
		tester_rsp(BTP_SERVICE_ID_L2CAP, L2CAP_SEND_DATA,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
	} else {
		tester_rsp(BTP_SERVICE_ID_L2CAP, L2CAP_SEND_DATA,
			   CONTROLLER_INDEX, BTP_STATUS_SUCCESS);
	}
}

static struct channel *get_free_channel()
{
	u8_t i;
	struct channel *chan;

	for (i = 0; i < CHANNELS; i++) {
		if (channels[i].state) {
			continue;
		}

		chan = &channels[i];
		chan->chan_id = i;

		return chan;
	}

	return NULL;
}

static void connected_cb(uint16_t conn_handle, struct ble_l2cap_chan *chan,
			 void *arg)
{
	struct l2cap_connected_ev ev;
	struct ble_gap_conn_desc desc;
	struct channel *channel;

	channel = get_free_channel();
	if (!channel) {
		assert(0);
	}

	channel->chan = chan;
	channel->state = 0;

	ev.chan_id = channel->chan_id;
	channel->state = 1;
	channel->chan = chan;
	/* TODO: ev.psm */

	if (!ble_gap_conn_find(conn_handle, &desc)) {
		ev.address_type = desc.peer_ota_addr.type;
		memcpy(ev.address, desc.peer_ota_addr.val,
		       sizeof(ev.address));
	}

	tester_send(BTP_SERVICE_ID_L2CAP, L2CAP_EV_CONNECTED, CONTROLLER_INDEX,
		    (u8_t *) &ev, sizeof(ev));
}

static void disconnected_cb(uint16_t conn_handle, struct ble_l2cap_chan *chan,
			    void *arg)
{
	struct l2cap_disconnected_ev ev;
	struct ble_gap_conn_desc desc;
	struct channel *channel;

	memset(&ev, 0, sizeof(struct l2cap_disconnected_ev));

	channel = find_channel(chan);
	if (channel != NULL) {
		channel->state = 0;
		channel->chan = chan;

		ev.chan_id = channel->chan_id;
		/* TODO: ev.result */
		/* TODO: ev.psm */
	}

	if (!ble_gap_conn_find(conn_handle, &desc)) {
		ev.address_type = desc.peer_ota_addr.type;
		memcpy(ev.address, desc.peer_ota_addr.val,
		       sizeof(ev.address));
	}

	tester_send(BTP_SERVICE_ID_L2CAP, L2CAP_EV_DISCONNECTED,
		    CONTROLLER_INDEX, (u8_t *) &ev, sizeof(ev));
}

static int accept_cb(uint16_t conn_handle, uint16_t peer_mtu,
		     struct ble_l2cap_chan *chan)
{
	struct os_mbuf *sdu_rx;

	SYS_LOG_DBG("LE CoC accepting, chan: 0x%08lx, peer_mtu %d",
		    (uint32_t) chan, peer_mtu);

	sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
	if (!sdu_rx) {
		return BLE_HS_ENOMEM;
	}

	ble_l2cap_recv_ready(chan, sdu_rx);

	return 0;
}

static int
tester_l2cap_event(struct ble_l2cap_event *event, void *arg)
{
	switch (event->type) {
	case BLE_L2CAP_EVENT_COC_CONNECTED:
		if (event->connect.status) {
			console_printf("LE COC error: %d\n", event->connect.status);
			disconnected_cb(event->connect.conn_handle,
					event->connect.chan, arg);
			return 0;
		}

		console_printf("LE COC connected, conn: %d, chan: 0x%08lx, scid: 0x%04x, "
			       "dcid: 0x%04x, our_mtu: 0x%04x, peer_mtu: 0x%04x\n",
			       event->connect.conn_handle,
			       (uint32_t) event->connect.chan,
			       ble_l2cap_get_scid(event->connect.chan),
			       ble_l2cap_get_dcid(event->connect.chan),
			       ble_l2cap_get_our_mtu(event->connect.chan),
			       ble_l2cap_get_peer_mtu(event->connect.chan));

		connected_cb(event->connect.conn_handle,
			     event->connect.chan, arg);

		return 0;
	case BLE_L2CAP_EVENT_COC_DISCONNECTED:
		console_printf("LE CoC disconnected, chan: 0x%08lx\n",
			       (uint32_t) event->disconnect.chan);

		disconnected_cb(event->disconnect.conn_handle,
				event->disconnect.chan, arg);
		return 0;
	case BLE_L2CAP_EVENT_COC_ACCEPT:
		console_printf("LE CoC accept, chan: 0x%08lx, handle: %u, sdu_size: %u\n",
			       (uint32_t) event->accept.chan,
			       event->accept.conn_handle,
			       event->accept.peer_sdu_size);

		return accept_cb(event->accept.conn_handle,
				 event->accept.peer_sdu_size,
				 event->accept.chan);

	case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
		console_printf("LE CoC data received, chan: 0x%08lx, handle: %u, sdu_len: %u\n",
			       (uint32_t) event->receive.chan,
			       event->receive.conn_handle,
			       event->receive.sdu_rx->om_len);
		recv_cb(event->receive.conn_handle, event->receive.chan,
			event->receive.sdu_rx, arg);
		return 0;
	case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
		console_printf("LE CoC tx unstalled, chan: 0x%08lx, handle: %u, status: %d\n",
			       (uint32_t) event->tx_unstalled.chan,
			       event->tx_unstalled.conn_handle,
			       event->tx_unstalled.status);
		unstalled_cb(event->tx_unstalled.conn_handle,
			     event->tx_unstalled.chan,
			     event->tx_unstalled.status, arg);
		return 0;
	default:
		return 0;
	}
}

static void connect(u8_t *data, u16_t len)
{
	const struct l2cap_connect_cmd *cmd = (void *) data;
	struct l2cap_connect_rp rp;
	struct ble_gap_conn_desc desc;
	struct channel *chan;
	struct os_mbuf *sdu_rx;
	ble_addr_t *addr = (void *) data;
	int rc;

	SYS_LOG_DBG("connect: type: %d addr: %s", addr->type, bt_hex(addr->val, 6));

	rc = ble_gap_conn_find_by_addr(addr, &desc);
	if (rc) {
		SYS_LOG_ERR("GAP conn find failed");
		goto fail;
	}

	chan = get_free_channel();
	if (!chan) {
		SYS_LOG_ERR("No free channels");
		goto fail;
	}

	sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
	if (sdu_rx == NULL) {
		SYS_LOG_ERR("Failed to alloc buf");
		goto fail;
	}

	rc = ble_l2cap_connect(desc.conn_handle, htole16(cmd->psm),
			       TESTER_COC_MTU, sdu_rx,
			       tester_l2cap_event, chan);
	if (rc) {
		SYS_LOG_ERR("L2CAP connect failed\n");
		goto fail;
	}

	rp.chan_id = chan->chan_id;

	tester_send(BTP_SERVICE_ID_L2CAP, L2CAP_CONNECT, CONTROLLER_INDEX,
		    (u8_t *) &rp, sizeof(rp));

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_L2CAP, L2CAP_CONNECT, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void disconnect(u8_t *data, u16_t len)
{
	const struct l2cap_disconnect_cmd *cmd = (void *) data;
	struct channel *chan;
	u8_t status;
	int err;

	SYS_LOG_DBG("");

	chan = &channels[cmd->chan_id];

	err = ble_l2cap_disconnect(chan->chan);
	if (err) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	status = BTP_STATUS_SUCCESS;

rsp:
	tester_rsp(BTP_SERVICE_ID_L2CAP, L2CAP_DISCONNECT, CONTROLLER_INDEX,
		   status);
}

static void send_data(u8_t *data, u16_t len)
{
	const struct l2cap_send_data_cmd *cmd = (void *) data;
	struct channel *chan = &channels[cmd->chan_id];
	struct os_mbuf *sdu_tx = NULL;
	int rc;
	u16_t data_len = sys_le16_to_cpu(cmd->data_len);

	SYS_LOG_DBG("cmd->chan_id=%d", cmd->chan_id);

	/* FIXME: For now, fail if data length exceeds buffer length */
	if (data_len > TESTER_COC_MTU) {
		SYS_LOG_ERR("Data length exceeds buffer length");
		goto fail;
	}

	sdu_tx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
	if (sdu_tx == NULL) {
		SYS_LOG_ERR("No memory in the test sdu pool\n");
		goto fail;
	}

	os_mbuf_append(sdu_tx, cmd->data, data_len);

	rc = ble_l2cap_send(chan->chan, sdu_tx);
	if (rc == BLE_HS_ESTALLED) {
		/* Wait for TX_UNSTALLED event before sending response */
		/* ble_l2cap_send takes ownership of the sdu */
		return;
	} else if (rc == 0) {
		tester_rsp(BTP_SERVICE_ID_L2CAP, L2CAP_SEND_DATA, CONTROLLER_INDEX,
			   BTP_STATUS_SUCCESS);
		return;
	}

	SYS_LOG_ERR("Unable to send data: %d", rc);
	os_mbuf_free_chain(sdu_tx);

fail:
	tester_rsp(BTP_SERVICE_ID_L2CAP, L2CAP_SEND_DATA, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void listen(u8_t *data, u16_t len)
{
	const struct l2cap_listen_cmd *cmd = (void *) data;
	int rc;

	SYS_LOG_DBG("");

	/* TODO: Handle cmd->transport flag */
	rc = ble_l2cap_create_server(cmd->psm, TESTER_COC_MTU,
				     tester_l2cap_event, NULL);
	if (rc) {
		goto fail;
	}

	tester_rsp(BTP_SERVICE_ID_L2CAP, L2CAP_LISTEN, CONTROLLER_INDEX,
		   BTP_STATUS_SUCCESS);
	return;

fail:
	tester_rsp(BTP_SERVICE_ID_L2CAP, L2CAP_LISTEN, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void supported_commands(u8_t *data, u16_t len)
{
	u8_t cmds[1];
	struct l2cap_read_supported_commands_rp *rp = (void *) cmds;

	memset(cmds, 0, sizeof(cmds));

	tester_set_bit(cmds, L2CAP_READ_SUPPORTED_COMMANDS);
	tester_set_bit(cmds, L2CAP_CONNECT);
	tester_set_bit(cmds, L2CAP_DISCONNECT);
	tester_set_bit(cmds, L2CAP_LISTEN);
	tester_set_bit(cmds, L2CAP_SEND_DATA);

	tester_send(BTP_SERVICE_ID_L2CAP, L2CAP_READ_SUPPORTED_COMMANDS,
		    CONTROLLER_INDEX, (u8_t *) rp, sizeof(cmds));
}

void tester_handle_l2cap(u8_t opcode, u8_t index, u8_t *data,
			 u16_t len)
{
	switch (opcode) {
	case L2CAP_READ_SUPPORTED_COMMANDS:
		supported_commands(data, len);
		return;
	case L2CAP_CONNECT:
		connect(data, len);
		return;
	case L2CAP_DISCONNECT:
		disconnect(data, len);
		return;
	case L2CAP_SEND_DATA:
		send_data(data, len);
		return;
	case L2CAP_LISTEN:
		listen(data, len);
		return;
	default:
		tester_rsp(BTP_SERVICE_ID_L2CAP, opcode, index,
			   BTP_STATUS_UNKNOWN_CMD);
		return;
	}
}

u8_t tester_init_l2cap(void)
{
	int rc;

	/* For testing we want to support all the available channels */
	rc = os_mempool_init(&sdu_coc_mbuf_mempool, TESTER_COC_BUF_COUNT,
			     TESTER_COC_MTU, tester_sdu_coc_mem,
			     "tester_coc_sdu_pool");
	assert(rc == 0);

	rc = os_mbuf_pool_init(&sdu_os_mbuf_pool, &sdu_coc_mbuf_mempool,
			       TESTER_COC_MTU, TESTER_COC_BUF_COUNT);
	assert(rc == 0);

	return BTP_STATUS_SUCCESS;
}

u8_t tester_unregister_l2cap(void)
{
	return BTP_STATUS_SUCCESS;
}

#endif
