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

/* gap.c - Bluetooth GAP Tester */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "host/ble_gap.h"
#include "console/console.h"

#include "../../../nimble/host/src/ble_hs_pvcy_priv.h"
#include "../../../nimble/host/src/ble_hs_hci_priv.h"
#include "../../../nimble/host/src/ble_sm_priv.h"

#include "atomic.h"
#include "bttester.h"

#define CONTROLLER_INDEX 0
#define CONTROLLER_NAME "btp_tester"

#define BLE_AD_DISCOV_MASK (BLE_HS_ADV_F_DISC_LTD | BLE_HS_ADV_F_DISC_GEN)
#define ADV_BUF_LEN (sizeof(struct gap_device_found_ev) + 2 * 31)

const uint8_t irk[16] = {
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
};

static atomic_t current_settings;
u8_t own_addr_type;
static ble_addr_t peer_id_addr;
static ble_addr_t peer_ota_addr;
static bool encrypted = false;

static int gap_conn_find_by_addr(const ble_addr_t *dev_addr,
				 struct ble_gap_conn_desc *out_desc)
{
	ble_addr_t addr = *dev_addr;

	if (memcmp(BLE_ADDR_ANY, &peer_id_addr, 6) == 0) {
		return ble_gap_conn_find_by_addr(&addr, out_desc);
	}

	if (BLE_ADDR_IS_RPA(&addr)) {
		if(ble_addr_cmp(&peer_ota_addr, &addr) != 0) {
			return -1;
		}

		return ble_gap_conn_find_by_addr(&addr, out_desc);
	} else {
		if(ble_addr_cmp(&peer_id_addr, &addr) != 0) {
			return -1;
		}

		if (BLE_ADDR_IS_RPA(&peer_ota_addr)) {
			/* Change addr type to ID addr */
			addr.type |= 2;
		}

		return ble_gap_conn_find_by_addr(&addr, out_desc);
	}
}

static int gap_event_cb(struct ble_gap_event *event, void *arg);

static void supported_commands(u8_t *data, u16_t len)
{
	u8_t cmds[3];
	struct gap_read_supported_commands_rp *rp = (void *) &cmds;

	SYS_LOG_DBG("");

	memset(cmds, 0, sizeof(cmds));

	tester_set_bit(cmds, GAP_READ_SUPPORTED_COMMANDS);
	tester_set_bit(cmds, GAP_READ_CONTROLLER_INDEX_LIST);
	tester_set_bit(cmds, GAP_READ_CONTROLLER_INFO);
	tester_set_bit(cmds, GAP_SET_CONNECTABLE);
	tester_set_bit(cmds, GAP_SET_DISCOVERABLE);
	tester_set_bit(cmds, GAP_START_ADVERTISING);
	tester_set_bit(cmds, GAP_STOP_ADVERTISING);
	tester_set_bit(cmds, GAP_START_DISCOVERY);
	tester_set_bit(cmds, GAP_STOP_DISCOVERY);
	tester_set_bit(cmds, GAP_CONNECT);
	tester_set_bit(cmds, GAP_DISCONNECT);
	tester_set_bit(cmds, GAP_SET_IO_CAP);
	tester_set_bit(cmds, GAP_PAIR);
	tester_set_bit(cmds, GAP_UNPAIR);
	tester_set_bit(cmds, GAP_PASSKEY_ENTRY);
	tester_set_bit(cmds, GAP_PASSKEY_CONFIRM);

	tester_send(BTP_SERVICE_ID_GAP, GAP_READ_SUPPORTED_COMMANDS,
		    CONTROLLER_INDEX, (u8_t *) rp, sizeof(cmds));
}

static void controller_index_list(u8_t *data,  u16_t len)
{
	struct gap_read_controller_index_list_rp *rp;
	u8_t buf[sizeof(*rp) + 1];

	SYS_LOG_DBG("");

	rp = (void *) buf;

	rp->num = 1;
	rp->index[0] = CONTROLLER_INDEX;

	tester_send(BTP_SERVICE_ID_GAP, GAP_READ_CONTROLLER_INDEX_LIST,
		    BTP_INDEX_NONE, (u8_t *) rp, sizeof(buf));
}

static int check_pub_addr_unassigned(void)
{
#ifdef ARCH_sim
	return 0;
#else
	uint8_t zero_addr[BLE_DEV_ADDR_LEN] = { 0 };

	return memcmp(MYNEWT_VAL(BLE_PUBLIC_DEV_ADDR),
		      zero_addr, BLE_DEV_ADDR_LEN) == 0;
#endif
}

static void controller_info(u8_t *data, u16_t len)
{
	struct gap_read_controller_info_rp rp;
	u32_t supported_settings = 0;
	ble_addr_t addr;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_hs_pvcy_set_our_irk(irk);
	assert(rc == 0);

	memset(&rp, 0, sizeof(rp));

	rc = ble_hs_id_gen_rnd(MYNEWT_VAL(BTTESTER_USE_NRPA), &addr);
	assert(rc == 0);
	rc = ble_hs_id_set_rnd(addr.val);
	assert(rc == 0);

	if (MYNEWT_VAL(BTTESTER_PRIVACY_MODE)) {
		if (MYNEWT_VAL(BTTESTER_USE_NRPA)) {
			own_addr_type = BLE_OWN_ADDR_RANDOM;
		} else {
			own_addr_type = BLE_OWN_ADDR_RPA_RANDOM_DEFAULT;
		}
		atomic_set_bit(&current_settings, GAP_SETTINGS_PRIVACY);
		supported_settings |= BIT(GAP_SETTINGS_PRIVACY);
		memcpy(rp.address, addr.val, sizeof(rp.address));
	} else {
		if (check_pub_addr_unassigned()) {
			own_addr_type = BLE_OWN_ADDR_RANDOM;
			memcpy(rp.address, addr.val, sizeof(rp.address));
			supported_settings |= BIT(GAP_SETTINGS_STATIC_ADDRESS);
			atomic_set_bit(&current_settings,
				       GAP_SETTINGS_STATIC_ADDRESS);
		} else {
			own_addr_type = BLE_OWN_ADDR_PUBLIC;
			memcpy(rp.address, MYNEWT_VAL(BLE_PUBLIC_DEV_ADDR),
			       sizeof(rp.address));
		}
	}

	ble_hs_cfg.sm_mitm = 0;

	supported_settings |= BIT(GAP_SETTINGS_POWERED);
	supported_settings |= BIT(GAP_SETTINGS_CONNECTABLE);
	supported_settings |= BIT(GAP_SETTINGS_BONDABLE);
	supported_settings |= BIT(GAP_SETTINGS_LE);
	supported_settings |= BIT(GAP_SETTINGS_ADVERTISING);

	rp.supported_settings = sys_cpu_to_le32(supported_settings);
	rp.current_settings = sys_cpu_to_le32(current_settings);

	memcpy(rp.name, CONTROLLER_NAME, sizeof(CONTROLLER_NAME));

	tester_send(BTP_SERVICE_ID_GAP, GAP_READ_CONTROLLER_INFO,
		    CONTROLLER_INDEX, (u8_t *) &rp, sizeof(rp));
}

static struct ble_gap_adv_params adv_params = {
	.conn_mode = BLE_GAP_CONN_MODE_NON,
	.disc_mode = BLE_GAP_DISC_MODE_NON,
};

static void set_connectable(u8_t *data, u16_t len)
{
	const struct gap_set_connectable_cmd *cmd = (void *) data;
	struct gap_set_connectable_rp rp;

	SYS_LOG_DBG("");

	if (cmd->connectable) {
		atomic_set_bit(&current_settings, GAP_SETTINGS_CONNECTABLE);
		adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
	} else {
		atomic_clear_bit(&current_settings, GAP_SETTINGS_CONNECTABLE);
		adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
	}

	rp.current_settings = sys_cpu_to_le32(current_settings);

	tester_send(BTP_SERVICE_ID_GAP, GAP_SET_CONNECTABLE, CONTROLLER_INDEX,
		    (u8_t *) &rp, sizeof(rp));
}

static u8_t ad_flags = BLE_HS_ADV_F_BREDR_UNSUP;

static void set_discoverable(u8_t *data, u16_t len)
{
	const struct gap_set_discoverable_cmd *cmd = (void *) data;
	struct gap_set_discoverable_rp rp;

	SYS_LOG_DBG("");

	switch (cmd->discoverable) {
	case GAP_NON_DISCOVERABLE:
		ad_flags &= ~(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_DISC_LTD);
		adv_params.disc_mode = BLE_GAP_DISC_MODE_NON;
		atomic_clear_bit(&current_settings, GAP_SETTINGS_DISCOVERABLE);
		break;
	case GAP_GENERAL_DISCOVERABLE:
		ad_flags &= ~BLE_HS_ADV_F_DISC_LTD;
		ad_flags |= BLE_HS_ADV_F_DISC_GEN;
		adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
		atomic_set_bit(&current_settings, GAP_SETTINGS_DISCOVERABLE);
		break;
	case GAP_LIMITED_DISCOVERABLE:
		ad_flags &= ~BLE_HS_ADV_F_DISC_GEN;
		ad_flags |= BLE_HS_ADV_F_DISC_LTD;
		adv_params.disc_mode = BLE_GAP_DISC_MODE_LTD;
		atomic_set_bit(&current_settings, GAP_SETTINGS_DISCOVERABLE);
		break;
	default:
		tester_rsp(BTP_SERVICE_ID_GAP, GAP_SET_DISCOVERABLE,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		return;
	}

	rp.current_settings = sys_cpu_to_le32(current_settings);

	tester_send(BTP_SERVICE_ID_GAP, GAP_SET_DISCOVERABLE, CONTROLLER_INDEX,
		    (u8_t *) &rp, sizeof(rp));
}

static struct bt_data ad[10] = {
	BT_DATA(BLE_HS_ADV_TYPE_FLAGS, &ad_flags, sizeof(ad_flags)),
};
static struct bt_data sd[10];

static int set_ad(const struct bt_data *ad, size_t ad_len,
		  u8_t *buf, u8_t *buf_len)
{
	int i;

	for (i = 0; i < ad_len; i++) {
		buf[(*buf_len)++] = ad[i].data_len + 1;
		buf[(*buf_len)++] = ad[i].type;

		memcpy(&buf[*buf_len], ad[i].data,
		       ad[i].data_len);
		*buf_len += ad[i].data_len;
	}

	return 0;
}

static void start_advertising(const u8_t *data, u16_t len)
{
	const struct gap_start_advertising_cmd *cmd = (void *) data;
	struct gap_start_advertising_rp rp;
	int32_t duration_ms = BLE_HS_FOREVER;
	uint8_t buf[BLE_HS_ADV_MAX_SZ];
	uint8_t buf_len = 0;
	u8_t adv_len, sd_len;
	int err;

	int i;

	SYS_LOG_DBG("");

	for (i = 0, adv_len = 1; i < cmd->adv_data_len; adv_len++) {
		if (adv_len >= ARRAY_SIZE(ad)) {
			SYS_LOG_ERR("ad[] Out of memory");
			goto fail;
		}

		ad[adv_len].type = cmd->adv_data[i++];
		ad[adv_len].data_len = cmd->adv_data[i++];
		ad[adv_len].data = &cmd->adv_data[i];
		i += ad[adv_len].data_len;
	}

	for (i = 0, sd_len = 0; i < cmd->scan_rsp_len; sd_len++) {
		if (sd_len >= ARRAY_SIZE(sd)) {
			SYS_LOG_ERR("sd[] Out of memory");
			goto fail;
		}

		sd[sd_len].type = cmd->scan_rsp[i++];
		sd[sd_len].data_len = cmd->scan_rsp[i++];
		sd[sd_len].data = &cmd->scan_rsp[i];
		i += sd[sd_len].data_len;
	}

	err = set_ad(ad, adv_len, buf, &buf_len);
	if (err) {
		goto fail;
	}

	err = ble_gap_adv_set_data(buf, buf_len);
	if (err != 0) {
		goto fail;
	}

	if (sd_len) {
		buf_len = 0;

		err = set_ad(sd, sd_len, buf, &buf_len);
		if (err) {
			SYS_LOG_ERR("Advertising failed: err %d", err);
			goto fail;
		}

		err = ble_gap_adv_rsp_set_data(buf, buf_len);
		if (err != 0) {
			SYS_LOG_ERR("Advertising failed: err %d", err);
			goto fail;
		}
	}

	if (adv_params.disc_mode == BLE_GAP_DISC_MODE_LTD) {
		duration_ms = MYNEWT_VAL(BTTESTER_LTD_ADV_TIMEOUT);
	}

	err = ble_gap_adv_start(own_addr_type, NULL, duration_ms,
				&adv_params, gap_event_cb, NULL);
	if (err) {
		SYS_LOG_ERR("Advertising failed: err %d", err);
		goto fail;
	}

	atomic_set_bit(&current_settings, GAP_SETTINGS_ADVERTISING);
	rp.current_settings = sys_cpu_to_le32(current_settings);

	tester_send(BTP_SERVICE_ID_GAP, GAP_START_ADVERTISING, CONTROLLER_INDEX,
		    (u8_t *) &rp, sizeof(rp));
	return;
fail:
	tester_rsp(BTP_SERVICE_ID_GAP, GAP_START_ADVERTISING, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void stop_advertising(const u8_t *data, u16_t len)
{
	struct gap_stop_advertising_rp rp;

	SYS_LOG_DBG("");

	if (ble_gap_adv_stop() != 0) {
		tester_rsp(BTP_SERVICE_ID_GAP, GAP_STOP_ADVERTISING,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		return;
	}

	atomic_clear_bit(&current_settings, GAP_SETTINGS_ADVERTISING);
	rp.current_settings = sys_cpu_to_le32(current_settings);

	tester_send(BTP_SERVICE_ID_GAP, GAP_STOP_ADVERTISING, CONTROLLER_INDEX,
		    (u8_t *) &rp, sizeof(rp));
}

static u8_t get_ad_flags(u8_t *data, u8_t data_len)
{
	u8_t len, i;

	/* Parse advertisement to get flags */
	for (i = 0; i < data_len; i += len - 1) {
		len = data[i++];
		if (!len) {
			break;
		}

		/* Check if field length is correct */
		if (len > (data_len - i) || (data_len - i) < 1) {
			break;
		}

		switch (data[i++]) {
		case BLE_HS_ADV_TYPE_FLAGS:
			return data[i];
		default:
			break;
		}
	}

	return 0;
}

static u8_t discovery_flags;
static struct os_mbuf *adv_buf;

static void store_adv(const ble_addr_t *addr, s8_t rssi,
		      u8_t *data, u8_t len)
{
	struct gap_device_found_ev *ev;

	/* cleanup */
	net_buf_simple_init(adv_buf, 0);

	ev = net_buf_simple_add(adv_buf, sizeof(*ev));

	memcpy(ev->address, addr->val, sizeof(ev->address));
	ev->address_type = addr->type;
	ev->rssi = rssi;
	ev->flags = GAP_DEVICE_FOUND_FLAG_AD | GAP_DEVICE_FOUND_FLAG_RSSI;
	ev->eir_data_len = len;
	memcpy(net_buf_simple_add(adv_buf, len), data, len);
}

static void device_found(ble_addr_t *addr, s8_t rssi, u8_t evtype,
			 u8_t *data, u8_t len)
{
	struct gap_device_found_ev *ev;
	ble_addr_t a;

	/* if General/Limited Discovery - parse Advertising data to get flags */
	if (!(discovery_flags & GAP_DISCOVERY_FLAG_LE_OBSERVE) &&
	    (evtype != BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP)) {
		u8_t flags = get_ad_flags(data, len);

		/* ignore non-discoverable devices */
		if (!(flags & BLE_AD_DISCOV_MASK)) {
			SYS_LOG_DBG("Non discoverable, skipping");
			return;
		}

		/* if Limited Discovery - ignore general discoverable devices */
		if ((discovery_flags & GAP_DISCOVERY_FLAG_LIMITED) &&
		    !(flags & BLE_HS_ADV_F_DISC_LTD)) {
			SYS_LOG_DBG("General discoverable, skipping");
			return;
		}
	}

	/* attach Scan Response data */
	if (evtype == BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP) {
		/* skip if there is no pending advertisement */
		if (!adv_buf->om_len) {
			SYS_LOG_INF("No pending advertisement, skipping");
			return;
		}

		ev = (void *) adv_buf->om_data;
		a.type = ev->address_type;
		memcpy(a.val, ev->address, sizeof(a.val));

		/*
		 * in general, the Scan Response comes right after the
		 * Advertisement, but if not if send stored event and ignore
		 * this one
		 */
		if (ble_addr_cmp(addr, &a)) {
			SYS_LOG_INF("Address does not match, skipping");
			goto done;
		}

		ev->eir_data_len += len;
		ev->flags |= GAP_DEVICE_FOUND_FLAG_SD;

		memcpy(net_buf_simple_add(adv_buf, len), data, len);

		goto done;
	}

	/*
	 * if there is another pending advertisement, send it and store the
	 * current one
	 */
	if (adv_buf->om_len) {
		tester_send(BTP_SERVICE_ID_GAP, GAP_EV_DEVICE_FOUND,
			    CONTROLLER_INDEX, adv_buf->om_data,
			    adv_buf->om_len);
	}

	store_adv(addr, rssi, data, len);

	/* if Active Scan and scannable event - wait for Scan Response */
	if ((discovery_flags & GAP_DISCOVERY_FLAG_LE_ACTIVE_SCAN) &&
	    (evtype == BLE_HCI_ADV_RPT_EVTYPE_ADV_IND ||
	     evtype == BLE_HCI_ADV_RPT_EVTYPE_SCAN_IND)) {
		SYS_LOG_DBG("Waiting for scan response");
		return;
	}
done:
	tester_send(BTP_SERVICE_ID_GAP, GAP_EV_DEVICE_FOUND,
		    CONTROLLER_INDEX, adv_buf->om_data, adv_buf->om_len);
}

static int discovery_cb(struct ble_gap_event *event, void *arg)
{
	if (event->type == BLE_GAP_EVENT_DISC) {
		device_found(&event->disc.addr, event->disc.rssi,
			     event->disc.event_type, event->disc.data,
			     event->disc.length_data);
	}

	return 0;
}

static void start_discovery(const u8_t *data, u16_t len)
{
	const struct gap_start_discovery_cmd *cmd = (void *) data;
	struct ble_gap_disc_params params = {0};
	u8_t status;

	SYS_LOG_DBG("");

	/* only LE scan is supported */
	if (cmd->flags & GAP_DISCOVERY_FLAG_BREDR) {
		status = BTP_STATUS_FAILED;
		goto reply;
	}

	params.passive = (cmd->flags & GAP_DISCOVERY_FLAG_LE_ACTIVE_SCAN) == 0;
	params.limited = (cmd->flags & GAP_DISCOVERY_FLAG_LIMITED) > 0;
	params.filter_duplicates = 1;

	if (ble_gap_disc(own_addr_type, BLE_HS_FOREVER,
			 &params, discovery_cb, NULL) != 0) {
		status = BTP_STATUS_FAILED;
		goto reply;
	}

	net_buf_simple_init(adv_buf, 0);
	discovery_flags = cmd->flags;

	status = BTP_STATUS_SUCCESS;
reply:
	tester_rsp(BTP_SERVICE_ID_GAP, GAP_START_DISCOVERY, CONTROLLER_INDEX,
		   status);
}

static void stop_discovery(const u8_t *data, u16_t len)
{
	u8_t status = BTP_STATUS_SUCCESS;

	SYS_LOG_DBG("");

	if (ble_gap_disc_cancel() != 0) {
		status = BTP_STATUS_FAILED;
	}

	tester_rsp(BTP_SERVICE_ID_GAP, GAP_STOP_DISCOVERY, CONTROLLER_INDEX,
		   status);
}

#if MYNEWT_VAL(BTTESTER_CONN_PARAM_UPDATE)
static void conn_param_update_cb(uint16_t conn_handle, int status, void *arg)
{
	console_printf("conn param update complete; conn_handle=%d status=%d\n",
		       conn_handle, status);
}

static void conn_param_update_slave(u16_t conn_handle)
{
	int rc;
	struct ble_l2cap_sig_update_params params;

	params.itvl_min = 0x0032;
	params.itvl_max = 0x0046;
	params.slave_latency = 0;
	params.timeout_multiplier = 0x07d0;

	rc = ble_l2cap_sig_update(conn_handle, &params,
		conn_param_update_cb, NULL);
	assert(rc == 0);
}

static void conn_param_update_master(u16_t conn_handle)
{
	int rc;
	struct ble_gap_upd_params params;

	params.itvl_min = 0x0032;
	params.itvl_max = 0x0046;
	params.latency = 0;
	params.supervision_timeout = 0x07d0;
	params.min_ce_len = 0;
	params.max_ce_len = 0;

	rc = ble_gap_update_params(conn_handle, &params);
	assert(rc == 0);
}
#endif

/* Bluetooth Core Spec v5.1 | Section 10.7.1
 * If a privacy-enabled Peripheral, that has a stored bond,
 * receives a resolvable private address, the Host may resolve
 * the resolvable private address [...]
 * If the resolution is successful, the Host may accept the connection.
 * If the resolution procedure fails, then the Host shall disconnect
 * with the error code "Authentication failure" [...]
 */
static void periph_privacy(struct ble_gap_conn_desc desc)
{
#if !MYNEWT_VAL(BTTESTER_PRIVACY_MODE)
	return;
#endif
	int count;

	SYS_LOG_DBG("");

	ble_store_util_count(BLE_STORE_OBJ_TYPE_PEER_SEC, &count);
	if (count > 0 && BLE_ADDR_IS_RPA(&desc.peer_id_addr)) {
		SYS_LOG_DBG("Authentication failure, disconnecting");
		ble_gap_terminate(desc.conn_handle, BLE_ERR_AUTH_FAIL);
	}
}

static void le_connected(u16_t conn_handle, int status)
{
	struct ble_gap_conn_desc desc;
	struct gap_device_connected_ev ev;
	ble_addr_t *addr;
	int rc;

	SYS_LOG_DBG("");

	if (status != 0) {
		return;
	}

	rc = ble_gap_conn_find(conn_handle, &desc);
	if (rc) {
		return;
	}

	peer_id_addr = desc.peer_id_addr;
	peer_ota_addr = desc.peer_ota_addr;

	addr = &desc.peer_id_addr;

	memcpy(ev.address, addr->val, sizeof(ev.address));
	ev.address_type = addr->type;


#if MYNEWT_VAL(BTTESTER_CONN_PARAM_UPDATE)
	if (desc.role == BLE_GAP_ROLE_MASTER) {
		conn_param_update_master(conn_handle);
	} else {
		conn_param_update_slave(conn_handle);
	}
#endif

	tester_send(BTP_SERVICE_ID_GAP, GAP_EV_DEVICE_CONNECTED,
		    CONTROLLER_INDEX, (u8_t *) &ev, sizeof(ev));

	periph_privacy(desc);
}

static void le_disconnected(struct ble_gap_conn_desc *conn, int reason)
{
	struct gap_device_disconnected_ev ev;
	ble_addr_t *addr = &conn->peer_ota_addr;

	SYS_LOG_DBG("");

	memcpy(ev.address, addr->val, sizeof(ev.address));
	ev.address_type = addr->type;

	tester_send(BTP_SERVICE_ID_GAP, GAP_EV_DEVICE_DISCONNECTED,
		    CONTROLLER_INDEX, (u8_t *) &ev, sizeof(ev));
}

static void auth_passkey_display(u16_t conn_handle, unsigned int passkey)
{
	struct ble_gap_conn_desc desc;
	struct gap_passkey_display_ev ev;
	ble_addr_t *addr;
	struct ble_sm_io pk;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find(conn_handle, &desc);
	if (rc) {
		return;
	}

	rc = ble_hs_hci_util_rand(&pk.passkey, sizeof(pk.passkey));
	assert(rc == 0);
	/* Max value is 999999 */
	pk.passkey %= 1000000;
	pk.action = BLE_SM_IOACT_DISP;

	rc = ble_sm_inject_io(conn_handle, &pk);
	assert(rc == 0);

	addr = &desc.peer_ota_addr;

	memcpy(ev.address, addr->val, sizeof(ev.address));
	ev.address_type = addr->type;
	ev.passkey = sys_cpu_to_le32(pk.passkey);

	tester_send(BTP_SERVICE_ID_GAP, GAP_EV_PASSKEY_DISPLAY,
		    CONTROLLER_INDEX, (u8_t *) &ev, sizeof(ev));
}

static void auth_passkey_entry(u16_t conn_handle)
{
	struct ble_gap_conn_desc desc;
	struct gap_passkey_entry_req_ev ev;
	ble_addr_t *addr;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find(conn_handle, &desc);
	if (rc) {
		return;
	}

	addr = &desc.peer_ota_addr;

	memcpy(ev.address, addr->val, sizeof(ev.address));
	ev.address_type = addr->type;

	tester_send(BTP_SERVICE_ID_GAP, GAP_EV_PASSKEY_ENTRY_REQ,
		    CONTROLLER_INDEX, (u8_t *) &ev, sizeof(ev));
}

static void auth_passkey_numcmp(u16_t conn_handle, unsigned int passkey)
{
	struct ble_gap_conn_desc desc;
	struct gap_passkey_confirm_req_ev ev;
	ble_addr_t *addr;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find(conn_handle, &desc);
	if (rc) {
		return;
	}

	addr = &desc.peer_ota_addr;

	memcpy(ev.address, addr->val, sizeof(ev.address));
	ev.address_type = addr->type;
	ev.passkey = sys_cpu_to_le32(passkey);

	tester_send(BTP_SERVICE_ID_GAP, GAP_EV_PASSKEY_CONFIRM_REQ,
		    CONTROLLER_INDEX, (u8_t *) &ev, sizeof(ev));
}

static void le_passkey_action(u16_t conn_handle,
			      struct ble_gap_passkey_params *params)
{
	SYS_LOG_DBG("");

	switch (params->action) {
	case BLE_SM_IOACT_INPUT:
		auth_passkey_entry(conn_handle);
		break;
	case BLE_SM_IOACT_DISP:
		auth_passkey_display(conn_handle, params->numcmp);
		break;
	case BLE_SM_IOACT_NUMCMP:
		auth_passkey_numcmp(conn_handle, params->numcmp);
		break;
	default:
		break;
	}
}

static void le_identity_resolved(u16_t conn_handle)
{
	struct ble_gap_conn_desc desc;
	struct gap_identity_resolved_ev ev;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find(conn_handle, &desc);
	if (rc) {
		return;
	}

	peer_id_addr = desc.peer_id_addr;
	peer_ota_addr = desc.peer_ota_addr;

	ev.address_type = desc.peer_ota_addr.type;
	memcpy(ev.address, desc.peer_ota_addr.val, sizeof(ev.address));

	ev.identity_address_type = desc.peer_id_addr.type;
	memcpy(ev.identity_address, desc.peer_id_addr.val,
	       sizeof(ev.identity_address));

	tester_send(BTP_SERVICE_ID_GAP, GAP_EV_IDENTITY_RESOLVED,
		    CONTROLLER_INDEX, (u8_t *) &ev, sizeof(ev));
}

static void le_encryption_changed(struct ble_gap_conn_desc *desc)
{
	encrypted = (bool) desc->sec_state.encrypted;
}

static void print_bytes(const uint8_t *bytes, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		console_printf("%s0x%02x", i != 0 ? ":" : "", bytes[i]);
	}
}

static void print_mbuf(const struct os_mbuf *om)
{
	int colon;

	colon = 0;
	while (om != NULL) {
		if (colon) {
			console_printf(":");
		} else {
			colon = 1;
		}
		print_bytes(om->om_data, om->om_len);
		om = SLIST_NEXT(om, om_next);
	}
}

static void print_addr(const void *addr)
{
	const uint8_t *u8p;

	u8p = addr;
	console_printf("%02x:%02x:%02x:%02x:%02x:%02x",
		       u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}

static void print_conn_desc(const struct ble_gap_conn_desc *desc)
{
	console_printf("handle=%d our_ota_addr_type=%d our_ota_addr=",
		       desc->conn_handle, desc->our_ota_addr.type);
	print_addr(desc->our_ota_addr.val);
	console_printf(" our_id_addr_type=%d our_id_addr=",
		       desc->our_id_addr.type);
	print_addr(desc->our_id_addr.val);
	console_printf(" peer_ota_addr_type=%d peer_ota_addr=",
		       desc->peer_ota_addr.type);
	print_addr(desc->peer_ota_addr.val);
	console_printf(" peer_id_addr_type=%d peer_id_addr=",
		       desc->peer_id_addr.type);
	print_addr(desc->peer_id_addr.val);
	console_printf(" conn_itvl=%d conn_latency=%d supervision_timeout=%d "
		       "encrypted=%d authenticated=%d bonded=%d\n",
		       desc->conn_itvl, desc->conn_latency,
		       desc->supervision_timeout,
		       desc->sec_state.encrypted,
		       desc->sec_state.authenticated,
		       desc->sec_state.bonded);
}

static void adv_complete(void)
{
	struct gap_new_settings_ev ev;

	atomic_clear_bit(&current_settings, GAP_SETTINGS_ADVERTISING);
	ev.current_settings = sys_cpu_to_le32(current_settings);

	tester_send(BTP_SERVICE_ID_GAP, GAP_EV_NEW_SETTINGS, CONTROLLER_INDEX,
		    (u8_t *) &ev, sizeof(ev));
}

static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
	struct ble_gap_conn_desc desc;
	int rc;

	switch (event->type) {
	case BLE_GAP_EVENT_ADV_COMPLETE:
		console_printf("advertising complete; reason=%d\n",
			       event->adv_complete.reason);
		break;
	case BLE_GAP_EVENT_CONNECT:
		console_printf("connection %s; status=%d ",
			       event->connect.status == 0 ? "established" : "failed",
			       event->connect.status);
		if (event->connect.status == 0) {
			rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
			assert(rc == 0);
			print_conn_desc(&desc);
		}

		if (desc.role == BLE_GAP_ROLE_SLAVE) {
			adv_complete();
		}

		le_connected(event->connect.conn_handle,
			     event->connect.status);
		break;
	case BLE_GAP_EVENT_DISCONNECT:
		console_printf("disconnect; reason=%d ", event->disconnect.reason);
		print_conn_desc(&event->disconnect.conn);
		le_disconnected(&event->disconnect.conn,
				event->disconnect.reason);
		break;
	case BLE_GAP_EVENT_ENC_CHANGE:
		console_printf("encryption change event; status=%d ", event->enc_change.status);
		rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
		assert(rc == 0);
		print_conn_desc(&desc);
		le_encryption_changed(&desc);
		break;
	case BLE_GAP_EVENT_PASSKEY_ACTION:
		console_printf("passkey action event; action=%d",
			       event->passkey.params.action);
		if (event->passkey.params.action == BLE_SM_IOACT_NUMCMP) {
			console_printf(" numcmp=%lu",
				       (unsigned long)event->passkey.params.numcmp);
		}
		console_printf("\n");
		le_passkey_action(event->passkey.conn_handle,
				  &event->passkey.params);
		break;
	case BLE_GAP_EVENT_IDENTITY_RESOLVED:
		console_printf("identity resolved ");
		rc = ble_gap_conn_find(event->identity_resolved.conn_handle, &desc);
		assert(rc == 0);
		print_conn_desc(&desc);
		le_identity_resolved(event->identity_resolved.conn_handle);
		break;
	case BLE_GAP_EVENT_NOTIFY_RX:
		console_printf("notification rx event; attr_handle=%d indication=%d "
			       "len=%d data=",
			       event->notify_rx.attr_handle,
			       event->notify_rx.indication,
			       OS_MBUF_PKTLEN(event->notify_rx.om));

		print_mbuf(event->notify_rx.om);
		console_printf("\n");
		tester_gatt_notify_rx_ev(event->notify_rx.conn_handle,
					 event->notify_rx.attr_handle,
					 event->notify_rx.indication,
					 event->notify_rx.om);
		break;
	case BLE_GAP_EVENT_SUBSCRIBE:
		console_printf("subscribe event; conn_handle=%d attr_handle=%d "
			       "reason=%d prevn=%d curn=%d previ=%d curi=%d\n",
			       event->subscribe.conn_handle,
			       event->subscribe.attr_handle,
			       event->subscribe.reason,
			       event->subscribe.prev_notify,
			       event->subscribe.cur_notify,
			       event->subscribe.prev_indicate,
			       event->subscribe.cur_indicate);
		tester_gatt_subscribe_ev(event->subscribe.conn_handle,
					 event->subscribe.attr_handle,
					 event->subscribe.reason,
					 event->subscribe.prev_notify,
					 event->subscribe.cur_notify,
					 event->subscribe.prev_indicate,
					 event->subscribe.cur_indicate);
		break;
	case BLE_GAP_EVENT_REPEAT_PAIRING:
		console_printf("repeat pairing event; conn_handle=%d "
			       "cur_key_sz=%d cur_auth=%d cur_sc=%d "
			       "new_key_sz=%d new_auth=%d new_sc=%d "
			       "new_bonding=%d\n",
			event->repeat_pairing.conn_handle,
			event->repeat_pairing.cur_key_size,
			event->repeat_pairing.cur_authenticated,
			event->repeat_pairing.cur_sc,
			event->repeat_pairing.new_key_size,
			event->repeat_pairing.new_authenticated,
			event->repeat_pairing.new_sc,
			event->repeat_pairing.new_bonding);
		rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
		assert(rc == 0);
		rc = ble_store_util_delete_peer(&desc.peer_id_addr);
		assert(rc == 0);
		return BLE_GAP_REPEAT_PAIRING_RETRY;
	default:
		break;
	}

	return 0;
}

static void connect(const u8_t *data, u16_t len)
{
	struct ble_gap_conn_params conn_params = { 0 };
	u8_t status;
	int rc;

	SYS_LOG_DBG("");

	conn_params.scan_itvl = 0x0010;
	conn_params.scan_window = 0x0010;
	conn_params.itvl_min = BLE_GAP_INITIAL_CONN_ITVL_MIN;
	conn_params.itvl_max = BLE_GAP_INITIAL_CONN_ITVL_MAX;
	conn_params.latency = 0;
	conn_params.supervision_timeout = 0x0100;
	conn_params.min_ce_len = 0x0010;
	conn_params.max_ce_len = 0x0300;

	rc = ble_gap_connect(own_addr_type, (ble_addr_t *) data,
			     0, &conn_params,
			     gap_event_cb, NULL);
	if (rc) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	status = BTP_STATUS_SUCCESS;

rsp:
	tester_rsp(BTP_SERVICE_ID_GAP, GAP_CONNECT, CONTROLLER_INDEX, status);
}

static void disconnect(const u8_t *data, u16_t len)
{
	struct ble_gap_conn_desc desc;
	u8_t status;
	int rc;

	SYS_LOG_DBG("");

	rc = gap_conn_find_by_addr((ble_addr_t *)data, &desc);
	if (rc) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	if (ble_gap_terminate(desc.conn_handle, BLE_ERR_REM_USER_CONN_TERM)) {
		status = BTP_STATUS_FAILED;
	} else {
		status = BTP_STATUS_SUCCESS;
	}

rsp:
	tester_rsp(BTP_SERVICE_ID_GAP, GAP_DISCONNECT, CONTROLLER_INDEX,
		   status);
}

static void set_io_cap(const u8_t *data, u16_t len)
{
	const struct gap_set_io_cap_cmd *cmd = (void *) data;
	u8_t status;

	SYS_LOG_DBG("");

	switch (cmd->io_cap) {
	case GAP_IO_CAP_DISPLAY_ONLY:
		ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_DISP_ONLY;
		ble_hs_cfg.sm_mitm = 1;
		break;
	case GAP_IO_CAP_KEYBOARD_DISPLAY:
		ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_KEYBOARD_DISP;
		ble_hs_cfg.sm_mitm = 1;
		break;
	case GAP_IO_CAP_NO_INPUT_OUTPUT:
		ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;
		ble_hs_cfg.sm_mitm = 0;
		break;
		break;
	case GAP_IO_CAP_KEYBOARD_ONLY:
		ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_KEYBOARD_ONLY;
		ble_hs_cfg.sm_mitm = 1;
		break;
	case GAP_IO_CAP_DISPLAY_YESNO:
		ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_DISP_YES_NO;
		ble_hs_cfg.sm_mitm = 1;
		break;
	default:
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	status = BTP_STATUS_SUCCESS;

rsp:
	tester_rsp(BTP_SERVICE_ID_GAP, GAP_SET_IO_CAP, CONTROLLER_INDEX,
		   status);
}

static void pair(const u8_t *data, u16_t len)
{
	struct ble_gap_conn_desc desc;
	u8_t status;
	int rc;

	SYS_LOG_DBG("");

	rc = gap_conn_find_by_addr((ble_addr_t *)data, &desc);
	if (rc) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	if (ble_gap_security_initiate(desc.conn_handle)) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	status = BTP_STATUS_SUCCESS;

rsp:
	tester_rsp(BTP_SERVICE_ID_GAP, GAP_PAIR, CONTROLLER_INDEX, status);
}

static void unpair(const u8_t *data, u16_t len)
{
	u8_t status;
	int err;

	SYS_LOG_DBG("");

	err = ble_gap_unpair((ble_addr_t *) data);
	status = (uint8_t) (err != 0 ? BTP_STATUS_FAILED : BTP_STATUS_SUCCESS);
	tester_rsp(BTP_SERVICE_ID_GAP, GAP_UNPAIR, CONTROLLER_INDEX, status);
}

static void passkey_entry(const u8_t *data, u16_t len)
{
	const struct gap_passkey_entry_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc desc;
	struct ble_sm_io pk;
	u8_t status;
	int rc;

	SYS_LOG_DBG("");

	rc = gap_conn_find_by_addr((ble_addr_t *)data, &desc);
	if (rc) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	pk.action = BLE_SM_IOACT_INPUT;
	pk.passkey = sys_le32_to_cpu(cmd->passkey);

	rc = ble_sm_inject_io(desc.conn_handle, &pk);
	if (rc) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	status = BTP_STATUS_SUCCESS;

rsp:
	tester_rsp(BTP_SERVICE_ID_GAP, GAP_PASSKEY_ENTRY, CONTROLLER_INDEX,
		   status);
}

static void passkey_confirm(const u8_t *data, u16_t len)
{
	const struct gap_passkey_confirm_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc desc;
	struct ble_sm_io pk;
	u8_t status;
	int rc;

	SYS_LOG_DBG("");

	rc = gap_conn_find_by_addr((ble_addr_t *)data, &desc);
	if (rc) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	pk.action = BLE_SM_IOACT_NUMCMP;
	pk.numcmp_accept = cmd->match;

	rc = ble_sm_inject_io(desc.conn_handle, &pk);
	if (rc) {
		console_printf("sm inject io failed");
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	status = BTP_STATUS_SUCCESS;

rsp:
	tester_rsp(BTP_SERVICE_ID_GAP, GAP_PASSKEY_CONFIRM, CONTROLLER_INDEX,
		   status);
}

void tester_handle_gap(u8_t opcode, u8_t index, u8_t *data,
		       u16_t len)
{
	switch (opcode) {
	case GAP_READ_SUPPORTED_COMMANDS:
	case GAP_READ_CONTROLLER_INDEX_LIST:
		if (index != BTP_INDEX_NONE){
			tester_rsp(BTP_SERVICE_ID_GAP, opcode, index,
				   BTP_STATUS_FAILED);
			return;
		}
		break;
	default:
		if (index != CONTROLLER_INDEX){
			tester_rsp(BTP_SERVICE_ID_GAP, opcode, index,
				   BTP_STATUS_FAILED);
			return;
		}
		break;
	}

	switch (opcode) {
	case GAP_READ_SUPPORTED_COMMANDS:
		supported_commands(data, len);
		return;
	case GAP_READ_CONTROLLER_INDEX_LIST:
		controller_index_list(data, len);
		return;
	case GAP_READ_CONTROLLER_INFO:
		controller_info(data, len);
		return;
	case GAP_SET_CONNECTABLE:
		set_connectable(data, len);
		return;
	case GAP_SET_DISCOVERABLE:
		set_discoverable(data, len);
		return;
	case GAP_START_ADVERTISING:
		start_advertising(data, len);
		return;
	case GAP_STOP_ADVERTISING:
		stop_advertising(data, len);
		return;
	case GAP_START_DISCOVERY:
		start_discovery(data, len);
		return;
	case GAP_STOP_DISCOVERY:
		stop_discovery(data, len);
		return;
	case GAP_CONNECT:
		connect(data, len);
		return;
	case GAP_DISCONNECT:
		disconnect(data, len);
		return;
	case GAP_SET_IO_CAP:
		set_io_cap(data, len);
		return;
	case GAP_PAIR:
		pair(data, len);
		return;
	case GAP_UNPAIR:
		unpair(data, len);
		return;
	case GAP_PASSKEY_ENTRY:
		passkey_entry(data, len);
		return;
	case GAP_PASSKEY_CONFIRM:
		passkey_confirm(data, len);
		return;
	default:
		tester_rsp(BTP_SERVICE_ID_GAP, opcode, index,
			   BTP_STATUS_UNKNOWN_CMD);
		return;
	}
}

static void tester_init_gap_cb(int err)
{
	if (err) {
		tester_rsp(BTP_SERVICE_ID_CORE, CORE_REGISTER_SERVICE,
			   BTP_INDEX_NONE, BTP_STATUS_FAILED);
		return;
	}

	atomic_clear(&current_settings);
	atomic_set_bit(&current_settings, GAP_SETTINGS_POWERED);
	atomic_set_bit(&current_settings, GAP_SETTINGS_BONDABLE);
	atomic_set_bit(&current_settings, GAP_SETTINGS_LE);

	tester_rsp(BTP_SERVICE_ID_CORE, CORE_REGISTER_SERVICE, BTP_INDEX_NONE,
		   BTP_STATUS_SUCCESS);
}

u8_t tester_init_gap(void)
{
	adv_buf = NET_BUF_SIMPLE(ADV_BUF_LEN);

	tester_init_gap_cb(BTP_STATUS_SUCCESS);
	return BTP_STATUS_SUCCESS;
}

u8_t tester_unregister_gap(void)
{
	return BTP_STATUS_SUCCESS;
}
