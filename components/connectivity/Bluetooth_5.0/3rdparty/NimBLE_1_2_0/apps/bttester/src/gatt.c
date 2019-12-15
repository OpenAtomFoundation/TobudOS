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

/* gatt.c - Bluetooth GATT Server Tester */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <assert.h>

#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "console/console.h"
#include "services/gatt/ble_svc_gatt.h"
#include "../../../nimble/host/src/ble_gatt_priv.h"

#include "bttester.h"

#define CONTROLLER_INDEX 0
#define MAX_BUFFER_SIZE 2048

#define BTP_GATT_PERM_READ	0x01

static const ble_uuid16_t BT_UUID_GATT_CEP = BLE_UUID16_INIT(0x2900);
static const ble_uuid16_t BT_UUID_GATT_CCC = BLE_UUID16_INIT(0x2902);

static const ble_uuid_t *uuid_pri =
	BLE_UUID16_DECLARE(BLE_ATT_UUID_PRIMARY_SERVICE);
static const ble_uuid_t *uuid_sec =
	BLE_UUID16_DECLARE(BLE_ATT_UUID_SECONDARY_SERVICE);
static const ble_uuid_t *uuid_chr =
	BLE_UUID16_DECLARE(BLE_ATT_UUID_CHARACTERISTIC);
static const ble_uuid_t *uuid_ccc =
	BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16);

static int gatt_chr_perm_map[] = {
	BLE_GATT_CHR_F_READ,
	BLE_GATT_CHR_F_WRITE,
	BLE_GATT_CHR_F_READ_ENC,
	BLE_GATT_CHR_F_WRITE_ENC,
	BLE_GATT_CHR_F_READ_AUTHEN,
	BLE_GATT_CHR_F_WRITE_AUTHEN,
	BLE_GATT_CHR_F_READ_AUTHOR,
	BLE_GATT_CHR_F_WRITE_AUTHOR,
};

static int gatt_dsc_perm_map[] = {
	BLE_ATT_F_READ,
	BLE_ATT_F_WRITE,
	BLE_ATT_F_READ_ENC,
	BLE_ATT_F_WRITE_ENC,
	BLE_ATT_F_READ_AUTHEN,
	BLE_ATT_F_WRITE_AUTHEN,
	BLE_ATT_F_READ_AUTHOR,
	BLE_ATT_F_WRITE_AUTHOR,
};

/* GATT server context */
#define SERVER_MAX_VALUES	14

static u16_t db_attr_min = 0xffff;
static u16_t db_attr_max = 0x0000;

struct gatt_value {
	struct os_mbuf *buf;
	u8_t enc_key_size;
	u8_t flags[1];
	u16_t val_handle;
	u8_t type;
	void *ptr;
};

enum {
	GATT_VALUE_TYPE_CHR,
	GATT_VALUE_TYPE_DSC,
};

static struct gatt_value gatt_values[SERVER_MAX_VALUES];

/* 0000xxxx-8c26-476f-89a7-a108033a69c7 */
#define PTS_UUID_DECLARE(uuid16)                                \
    ((const ble_uuid_t *) (&(ble_uuid128_t) BLE_UUID128_INIT(   \
        0xc7, 0x69, 0x3a, 0x03, 0x08, 0xa1, 0xa7, 0x89,         \
        0x6f, 0x47, 0x26, 0x8c, uuid16, uuid16 >> 8, 0x00, 0x00 \
    )))

#define  PTS_SVC                         0x0001
#define  PTS_CHR_NO_PERM                 0x0002
#define  PTS_CHR_READ                    0x0003
#define  PTS_CHR_RELIABLE_WRITE          0x0004
#define  PTS_CHR_WRITE_NO_RSP            0x0005
#define  PTS_CHR_READ_WRITE              0x0006
#define  PTS_CHR_READ_WRITE_ENC          0x0007
#define  PTS_CHR_READ_WRITE_AUTHEN       0x0008
#define  PTS_CHR_READ_WRITE_AUTHOR       0x0009
#define  PTS_DSC_READ                    0x000a
#define  PTS_DSC_WRITE                   0x000b
#define  PTS_DSC_READ_WRITE              0x000c
#define  PTS_DSC_READ_WRITE_ENC          0x000d
#define  PTS_DSC_READ_WRITE_AUTHEN       0x000e
#define  PTS_INC_SVC                     0x000f
#define  PTS_CHR_READ_WRITE_ALT          0x0010

/*
 * gatt_buf - cache used by a gatt client (to cache data read/discovered)
 * and gatt server (to store attribute user_data).
 * It is not intended to be used by client and server at the same time.
 */
static struct {
	u16_t len;
	u8_t buf[MAX_BUFFER_SIZE];
} gatt_buf;

static void *gatt_buf_add(const void *data, size_t len)
{
	void *ptr = gatt_buf.buf + gatt_buf.len;

	if ((len + gatt_buf.len) > MAX_BUFFER_SIZE) {
		return NULL;
	}

	if (data) {
		memcpy(ptr, data, len);
	} else {
		(void)memset(ptr, 0, len);
	}

	gatt_buf.len += len;

	SYS_LOG_DBG("%d/%d used", gatt_buf.len, MAX_BUFFER_SIZE);

	return ptr;
}

static void *gatt_buf_reserve(size_t len)
{
	return gatt_buf_add(NULL, len);
}

static void gatt_buf_clear(void)
{
	(void)memset(&gatt_buf, 0, sizeof(gatt_buf));
}

struct gatt_value *find_gatt_value_by_id(u16_t id)
{
	if (id < SERVER_MAX_VALUES) {
		return &gatt_values[id];
	}

	return NULL;
}

static int gatt_svr_access_cb(uint16_t conn_handle, uint16_t attr_handle,
			      struct ble_gatt_access_ctxt *ctxt,
			      void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
	{
		/*** Service: PTS test. */
		.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = PTS_UUID_DECLARE(PTS_SVC),
		.characteristics = (struct ble_gatt_chr_def[]) { {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_NO_PERM),
			.access_cb = gatt_svr_access_cb,
			.flags = 0,
			.arg = &gatt_values[0],
			.val_handle = &gatt_values[0].val_handle,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ),
			.access_cb = gatt_svr_access_cb,
			.flags = BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_NOTIFY |
				 BLE_GATT_CHR_F_INDICATE,
			.arg = &gatt_values[1],
			.val_handle = &gatt_values[1].val_handle,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_RELIABLE_WRITE),
			.access_cb = gatt_svr_access_cb,
			.flags = BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_RELIABLE_WRITE,
			.arg = &gatt_values[2],
			.val_handle = &gatt_values[2].val_handle,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_WRITE_NO_RSP),
			.access_cb = gatt_svr_access_cb,
			.flags = BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_WRITE_NO_RSP,
			.arg = &gatt_values[3],
			.val_handle = &gatt_values[3].val_handle,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE),
			.access_cb = gatt_svr_access_cb,
			.flags = BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_NOTIFY |
				 BLE_GATT_CHR_F_INDICATE,
			.arg = &gatt_values[4],
			.val_handle = &gatt_values[4].val_handle,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_ENC),
			.access_cb = gatt_svr_access_cb,
			.flags = BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_READ_ENC |
				 BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_WRITE_ENC,
			.min_key_size = 0x0f,
			.arg = &gatt_values[5],
			.val_handle = &gatt_values[5].val_handle,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_AUTHEN),
			.access_cb = gatt_svr_access_cb,
			.flags = BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_READ_AUTHEN |
				 BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_WRITE_AUTHEN,
			.arg = &gatt_values[6],
			.val_handle = &gatt_values[6].val_handle,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_AUTHOR),
			.access_cb = gatt_svr_access_cb,
			.flags = BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_READ_AUTHOR |
				 BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_WRITE_AUTHOR,
			.arg = &gatt_values[7],
			.val_handle = &gatt_values[7].val_handle,

			.descriptors = (struct ble_gatt_dsc_def[]){ {
				.uuid = PTS_UUID_DECLARE(PTS_DSC_READ),
				.access_cb = gatt_svr_access_cb,
				.att_flags = BLE_ATT_F_READ,
				.arg = &gatt_values[8],
			}, {
				.uuid = PTS_UUID_DECLARE(PTS_DSC_WRITE),
				.access_cb = gatt_svr_access_cb,
				.att_flags = BLE_ATT_F_WRITE,
				.arg = &gatt_values[9],
			}, {
				.uuid = PTS_UUID_DECLARE(PTS_DSC_READ_WRITE),
				.access_cb = gatt_svr_access_cb,
				.att_flags = BLE_ATT_F_READ | BLE_ATT_F_WRITE,
				.arg = &gatt_values[10],
			}, {
				.uuid = PTS_UUID_DECLARE(PTS_DSC_READ_WRITE_ENC),
				.access_cb = gatt_svr_access_cb,
				.att_flags = BLE_ATT_F_READ |
					     BLE_ATT_F_READ_ENC |
					     BLE_ATT_F_WRITE |
					     BLE_ATT_F_WRITE_ENC,
				.min_key_size = 0x0f,
				.arg = &gatt_values[11],
			}, {
				.uuid = PTS_UUID_DECLARE(PTS_DSC_READ_WRITE_AUTHEN),
				.access_cb = gatt_svr_access_cb,
				.att_flags = BLE_ATT_F_READ |
					     BLE_ATT_F_READ_AUTHEN |
					     BLE_ATT_F_WRITE |
					     BLE_ATT_F_WRITE_AUTHEN,
				.arg = &gatt_values[12],
			}, {
				0, /* No more descriptors in this characteristic. */
			} }
		}, {
			0, /* No more characteristics in this service. */
		} },
	},

	{
		0, /* No more services. */
	},
};

static const struct ble_gatt_svc_def *inc_svcs[] = {
	&gatt_svr_svcs[0],
	NULL,
};

static const struct ble_gatt_svc_def gatt_svr_inc_svcs[] = {
	{
		.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = PTS_UUID_DECLARE(PTS_INC_SVC),
		.includes = inc_svcs,
		.characteristics = (struct ble_gatt_chr_def[]) {{
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_ALT),
			.access_cb = gatt_svr_access_cb,
			.flags = BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_WRITE,
			.arg = &gatt_values[13],
			.val_handle = &gatt_values[13].val_handle,
			},
			{
				0,
			},
		},
	},

	{
		0, /* No more services. */
	},
};

static void init_gatt_values(void)
{
	int i = 0;
	const struct ble_gatt_svc_def *svc;
	const struct ble_gatt_chr_def *chr;
	const struct ble_gatt_dsc_def *dsc;

	/* GATT/SR/GAR/BV-05-C fails when a characteristic value is empty */

	for (svc = gatt_svr_svcs; svc && svc->uuid; svc++) {
		for (chr = svc->characteristics; chr && chr->uuid; chr++) {
			assert(i < SERVER_MAX_VALUES);
			gatt_values[i].type = GATT_VALUE_TYPE_CHR;
			gatt_values[i].ptr = (void *)chr;
			gatt_values[i].buf = os_msys_get(0, 0);
			os_mbuf_extend(gatt_values[i].buf, 1);
			++i;

			for (dsc = chr->descriptors; dsc && dsc->uuid; dsc++) {
				assert(i < SERVER_MAX_VALUES);
				gatt_values[i].type = GATT_VALUE_TYPE_DSC;
				gatt_values[i].ptr = (void *)dsc;
				gatt_values[i].buf = os_msys_get(0, 0);
				os_mbuf_extend(gatt_values[i].buf, 1);
				++i;
			}
		}
	}

	for (svc = gatt_svr_inc_svcs; svc && svc->uuid; svc++) {
		for (chr = svc->characteristics; chr && chr->uuid; chr++) {
			assert(i < SERVER_MAX_VALUES);
			gatt_values[i].type = GATT_VALUE_TYPE_CHR;
			gatt_values[i].ptr = (void *)chr;
			gatt_values[i].buf = os_msys_get(0, 0);
			os_mbuf_extend(gatt_values[i].buf, 1);
			++i;

			for (dsc = chr->descriptors; dsc && dsc->uuid; dsc++) {
				assert(i < SERVER_MAX_VALUES);
				gatt_values[i].type = GATT_VALUE_TYPE_DSC;
				gatt_values[i].ptr = (void *)dsc;
				gatt_values[i].buf = os_msys_get(0, 0);
				os_mbuf_extend(gatt_values[i].buf, 1);
				++i;
			}
		}
	}
}

/* Convert UUID from BTP command to bt_uuid */
static u8_t btp2bt_uuid(const u8_t *uuid, u8_t len,
			ble_uuid_any_t *bt_uuid)
{
	u16_t le16;

	switch (len) {
	case 0x02: /* UUID 16 */
		bt_uuid->u.type = BLE_UUID_TYPE_16;
		memcpy(&le16, uuid, sizeof(le16));
		BLE_UUID16(bt_uuid)->value = sys_le16_to_cpu(le16);
		break;
	case 0x10: /* UUID 128*/
		bt_uuid->u.type = BLE_UUID_TYPE_128;
		memcpy(BLE_UUID128(bt_uuid)->value, uuid, 16);
		break;
	default:
		return BTP_STATUS_FAILED;
	}

	return BTP_STATUS_SUCCESS;
}

static void supported_commands(u8_t *data, u16_t len)
{
	u8_t cmds[4];
	struct gatt_read_supported_commands_rp *rp = (void *) cmds;

	SYS_LOG_DBG("");

	memset(cmds, 0, sizeof(cmds));

	tester_set_bit(cmds, GATT_READ_SUPPORTED_COMMANDS);
	tester_set_bit(cmds, GATT_ADD_SERVICE);
	tester_set_bit(cmds, GATT_ADD_CHARACTERISTIC);
	tester_set_bit(cmds, GATT_ADD_DESCRIPTOR);
	tester_set_bit(cmds, GATT_ADD_INCLUDED_SERVICE);
	tester_set_bit(cmds, GATT_SET_VALUE);
	tester_set_bit(cmds, GATT_START_SERVER);
	tester_set_bit(cmds, GATT_SET_ENC_KEY_SIZE);
	tester_set_bit(cmds, GATT_EXCHANGE_MTU);
	tester_set_bit(cmds, GATT_DISC_ALL_PRIM_SVCS);
	tester_set_bit(cmds, GATT_DISC_PRIM_UUID);
	tester_set_bit(cmds, GATT_FIND_INCLUDED);
	tester_set_bit(cmds, GATT_DISC_ALL_CHRC);
	tester_set_bit(cmds, GATT_DISC_CHRC_UUID);
	tester_set_bit(cmds, GATT_DISC_ALL_DESC);
	tester_set_bit(cmds, GATT_READ);
	tester_set_bit(cmds, GATT_READ_LONG);
	tester_set_bit(cmds, GATT_READ_MULTIPLE);
	tester_set_bit(cmds, GATT_WRITE_WITHOUT_RSP);
#if 0
	tester_set_bit(cmds, GATT_SIGNED_WRITE_WITHOUT_RSP);
#endif
	tester_set_bit(cmds, GATT_WRITE);
	tester_set_bit(cmds, GATT_WRITE_LONG);
	tester_set_bit(cmds, GATT_CFG_NOTIFY);
	tester_set_bit(cmds, GATT_CFG_INDICATE);
	tester_set_bit(cmds, GATT_GET_ATTRIBUTES);
	tester_set_bit(cmds, GATT_GET_ATTRIBUTE_VALUE);

	tester_send(BTP_SERVICE_ID_GATT, GATT_READ_SUPPORTED_COMMANDS,
		    CONTROLLER_INDEX, (u8_t *) rp, sizeof(cmds));
}

const struct ble_gatt_svc_def *find_svc(ble_uuid_any_t *uuid)
{
	const struct ble_gatt_svc_def *svc;

	for (svc = gatt_svr_svcs; svc && svc->uuid; svc++) {
		if (ble_uuid_cmp(&uuid->u, svc->uuid) == 0) {
			return svc;
		}
	}

	for (svc = gatt_svr_inc_svcs; svc && svc->uuid; svc++) {
		if (ble_uuid_cmp(&uuid->u, svc->uuid) == 0) {
			return svc;
		}
	}

	return NULL;
}

const struct ble_gatt_chr_def *find_chr(ble_uuid_any_t *uuid)
{
	const struct ble_gatt_svc_def *svc;
	const struct ble_gatt_chr_def *chr;

	for (svc = gatt_svr_svcs; svc && svc->uuid; svc++) {
		for (chr = svc->characteristics; chr && chr->uuid; chr++) {
			if (ble_uuid_cmp(&uuid->u, chr->uuid) == 0) {
				return chr;
			}
		}
	}

	for (svc = gatt_svr_inc_svcs; svc && svc->uuid; svc++) {
		for (chr = svc->characteristics; chr && chr->uuid; chr++) {
			if (ble_uuid_cmp(&uuid->u, chr->uuid) == 0) {
				return chr;
			}
		}
	}

	return NULL;
}

const struct ble_gatt_dsc_def *find_dsc(ble_uuid_any_t *uuid)
{
	const struct ble_gatt_svc_def *svc;
	const struct ble_gatt_chr_def *chr;
	const struct ble_gatt_dsc_def *dsc;

	for (svc = gatt_svr_svcs; svc && svc->uuid; svc++) {
		for (chr = svc->characteristics; chr && chr->uuid; chr++) {
			for (dsc = chr->descriptors; dsc && dsc->uuid; dsc++) {
				if (ble_uuid_cmp(&uuid->u, dsc->uuid) == 0) {
					return dsc;
				}
			}
		}
	}

	for (svc = gatt_svr_inc_svcs; svc && svc->uuid; svc++) {
		for (chr = svc->characteristics; chr && chr->uuid; chr++) {
			for (dsc = chr->descriptors; dsc && dsc->uuid; dsc++) {
				if (ble_uuid_cmp(&uuid->u, dsc->uuid) == 0) {
					return dsc;
				}
			}
		}
	}

	return NULL;
}

static void add_service(u8_t *data, u16_t len)
{
	const struct gatt_add_service_cmd *cmd = (void *) data;
	struct gatt_add_service_rp rp;
	const struct ble_gatt_svc_def *svc;
	ble_uuid_any_t uuid;
	int type;

	SYS_LOG_DBG("");

	if (btp2bt_uuid(cmd->uuid, cmd->uuid_length, &uuid)) {
		goto fail;
	}

	switch (cmd->type) {
	case GATT_SERVICE_PRIMARY:
		type = BLE_GATT_SVC_TYPE_PRIMARY;
		break;
	case GATT_SERVICE_SECONDARY:
		type = BLE_GATT_SVC_TYPE_SECONDARY;
		break;
	default:
		SYS_LOG_ERR("Invalid service type");
		goto fail;
	}

	svc = find_svc(&uuid);
	if (svc == NULL) {
		SYS_LOG_ERR("Invalid service UUID");
		goto fail;
	}

	if (type != svc->type) {
		SYS_LOG_ERR("Invalid service type");
		goto fail;
	}

	/* TODO: set svc id */
	rp.svc_id = 0;

	tester_send(BTP_SERVICE_ID_GATT, GATT_ADD_SERVICE, CONTROLLER_INDEX,
		    (u8_t *) &rp, sizeof(rp));

	return;
fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_ADD_SERVICE, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static size_t read_value(uint16_t conn_handle, uint16_t attr_handle,
			 struct ble_gatt_access_ctxt *ctxt,
			 void *arg)
{
	const struct gatt_value *value = arg;
	char str[BLE_UUID_STR_LEN];
	int rc;

	SYS_LOG_DBG("");

	memset(str, '\0', sizeof(str));

	if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
		if (ctxt->chr->flags & BLE_GATT_CHR_F_READ_AUTHOR) {
			return BLE_ATT_ERR_INSUFFICIENT_AUTHOR;
		}

		ble_uuid_to_str(ctxt->chr->uuid, str);
	} else {
		if (ctxt->dsc->att_flags & BLE_ATT_F_READ_AUTHOR) {
			return BLE_ATT_ERR_INSUFFICIENT_AUTHOR;
		}

		ble_uuid_to_str(ctxt->dsc->uuid, str);
	}

	rc = os_mbuf_append(ctxt->om, value->buf->om_data, value->buf->om_len);
	return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static void attr_value_changed_ev(u16_t handle, const u8_t *value, u16_t len)
{
	u8_t buf[len + sizeof(struct gatt_attr_value_changed_ev)];
	struct gatt_attr_value_changed_ev *ev = (void *) buf;

	SYS_LOG_DBG("");

	ev->handle = sys_cpu_to_le16(handle);
	ev->data_length = sys_cpu_to_le16(len);
	memcpy(ev->data, value, len);

	tester_send(BTP_SERVICE_ID_GATT, GATT_EV_ATTR_VALUE_CHANGED,
		    CONTROLLER_INDEX, buf, sizeof(buf));
}

static size_t write_value(uint16_t conn_handle, uint16_t attr_handle,
			  struct ble_gatt_access_ctxt *ctxt,
			  void *arg)
{
	struct gatt_value *value = arg;
	uint16_t om_len, len;
	int rc;

	SYS_LOG_DBG("");

	if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
		if (ctxt->chr->flags & BLE_GATT_CHR_F_WRITE_AUTHOR) {
			return BLE_ATT_ERR_INSUFFICIENT_AUTHOR;
		}
	} else {
		if (ctxt->dsc->att_flags & BLE_ATT_F_WRITE_AUTHOR) {
			return BLE_ATT_ERR_INSUFFICIENT_AUTHOR;
		}
	}

	om_len = OS_MBUF_PKTLEN(ctxt->om);
	if (om_len > value->buf->om_len) {
		return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
	}

	rc = ble_hs_mbuf_to_flat(ctxt->om, value->buf->om_data,
				 value->buf->om_len, &len);
	if (rc != 0) {
		return BLE_ATT_ERR_UNLIKELY;
	}

	/* Maximum attribute value size is 512 bytes */
	assert(value->buf->om_len < 512);

	attr_value_changed_ev(attr_handle, value->buf->om_data,
			      value->buf->om_len);

	return 0;
}


static int gatt_svr_access_cb(uint16_t conn_handle, uint16_t attr_handle,
			      struct ble_gatt_access_ctxt *ctxt,
			      void *arg)
{
	SYS_LOG_DBG("");

	switch (ctxt->op) {
		case BLE_GATT_ACCESS_OP_READ_CHR:
		case BLE_GATT_ACCESS_OP_READ_DSC:
			return read_value(conn_handle, attr_handle,
					  ctxt, arg);
		case BLE_GATT_ACCESS_OP_WRITE_CHR:
		case BLE_GATT_ACCESS_OP_WRITE_DSC:
			return write_value(conn_handle, attr_handle,
					   ctxt, arg);
		default:
			assert(0);
			return BLE_ATT_ERR_UNLIKELY;
	}

	/* Unknown characteristic; the nimble stack should not have called this
	 * function.
	 */
	assert(0);
	return BLE_ATT_ERR_UNLIKELY;
}

static void add_characteristic(u8_t *data, u16_t len)
{
	const struct gatt_add_characteristic_cmd *cmd = (void *) data;
	struct gatt_add_characteristic_rp rp;
	const struct ble_gatt_chr_def *chr;
	ble_uuid_any_t uuid;
	uint16_t flags = 0;
	int i;

	SYS_LOG_DBG("");

	if (btp2bt_uuid(cmd->uuid, cmd->uuid_length, &uuid)) {
		goto fail;
	}

	/* characterisic must be added sequentially */
	if (cmd->svc_id) {
		goto fail;
	}

	for (i = 0; i < 8; ++i) {
		if (cmd->permissions & BIT(i)) {
			flags |= gatt_chr_perm_map[i];
		}
	}

	chr = find_chr(&uuid);
	if (chr == NULL) {
		SYS_LOG_ERR("Invalid characteristic UUID");
		goto fail;
	}

	if ((chr->flags > 0) && ((flags & chr->flags) == 0)) {
		SYS_LOG_ERR("Invalid flags");
		goto fail;
	}

	/* TODO: Set char id */
	rp.char_id = 0;

	tester_send(BTP_SERVICE_ID_GATT, GATT_ADD_CHARACTERISTIC,
		    CONTROLLER_INDEX, (u8_t *) &rp, sizeof(rp));
	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_ADD_CHARACTERISTIC,
		   CONTROLLER_INDEX, BTP_STATUS_FAILED);
}

static void add_descriptor(u8_t *data, u16_t len)
{
	const struct gatt_add_descriptor_cmd *cmd = (void *) data;
	struct gatt_add_descriptor_rp rp;
	const struct ble_gatt_dsc_def *dsc;
	ble_uuid_any_t uuid;
	uint16_t att_flags = 0;
	int i;

	SYS_LOG_DBG("");

	if (btp2bt_uuid(cmd->uuid, cmd->uuid_length, &uuid)) {
		goto fail;
	}

	/* descriptor can be added only sequentially */
	if (cmd->char_id) {
		goto fail;
	}

	att_flags = cmd->permissions;

	if (!ble_uuid_cmp(&uuid.u, &BT_UUID_GATT_CEP.u)) {
		/* TODO: */
	} else if (!ble_uuid_cmp(&uuid.u, &BT_UUID_GATT_CCC.u)) {
		/* handled by host */
	} else {
		dsc = find_dsc(&uuid);
		if (dsc == NULL) {
			SYS_LOG_ERR("Invalid characteristic UUID");
			goto fail;
		}

		for (i = 0; i < 8; ++i) {
			if (cmd->permissions & BIT(i)) {
				att_flags |= gatt_dsc_perm_map[i];
			}
		}

		if ((att_flags & dsc->att_flags) == 0) {
			SYS_LOG_ERR("Invalid flags");
			goto fail;
		}
	}

	rp.desc_id = 0;
	tester_send(BTP_SERVICE_ID_GATT, GATT_ADD_DESCRIPTOR, CONTROLLER_INDEX,
		    (u8_t *) &rp, sizeof(rp));
	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_ADD_DESCRIPTOR,
		   CONTROLLER_INDEX, BTP_STATUS_FAILED);
}

static void add_included(u8_t *data, u16_t len)
{
	const struct gatt_add_included_service_cmd *cmd = (void *) data;
	struct gatt_add_included_service_rp rp;
	u16_t included_service_id = 0;

	included_service_id = cmd->svc_id;

	rp.included_service_id = sys_cpu_to_le16(included_service_id);
	tester_send(BTP_SERVICE_ID_GATT, GATT_ADD_INCLUDED_SERVICE,
		    CONTROLLER_INDEX, (u8_t *) &rp, sizeof(rp));
}

static void set_value(u8_t *data, u16_t len)
{
	const struct gatt_set_value_cmd *cmd = (void *) data;
	const struct ble_gatt_chr_def *chr;
	struct gatt_value *gatt_value;
	u16_t value_len;
	const u8_t *value;

	SYS_LOG_DBG("%d id", cmd->attr_id);

	gatt_value = find_gatt_value_by_id(
		sys_le16_to_cpu(cmd->attr_id));
	assert(gatt_value != NULL);

	value_len = sys_le16_to_cpu(cmd->len);
	value = cmd->value;

	if (value_len > gatt_value->buf->om_len) {
		os_mbuf_extend(gatt_value->buf,
			       (value_len - gatt_value->buf->om_len));
	}

	memcpy(gatt_value->buf->om_data, value, value_len);

	if (gatt_value->type == GATT_VALUE_TYPE_CHR) {
		chr = gatt_value->ptr;
		if (chr->flags & BLE_GATT_CHR_F_INDICATE ||
		    chr->flags & BLE_GATT_CHR_F_NOTIFY) {
			ble_gatts_chr_updated(*chr->val_handle);
		}
	}

	tester_rsp(BTP_SERVICE_ID_GATT, GATT_SET_VALUE, CONTROLLER_INDEX,
		   BTP_STATUS_SUCCESS);
}

static void start_server(u8_t *data, u16_t len)
{
	struct gatt_start_server_rp rp;

	SYS_LOG_DBG("");

	ble_gatts_show_local();

	ble_svc_gatt_changed(0x0001, 0xffff);

	rp.db_attr_off = db_attr_min;
	rp.db_attr_cnt = (u8_t) (db_attr_max - db_attr_min + 1);

	tester_send(BTP_SERVICE_ID_GATT, GATT_START_SERVER, CONTROLLER_INDEX,
		    (u8_t *) &rp, sizeof(rp));
}

static void set_enc_key_size(u8_t *data, u16_t len)
{
	const struct gatt_set_enc_key_size_cmd *cmd = (void *) data;
	struct gatt_value *val;
	u8_t status = 0;

	/* Fail if requested key size is invalid */
	if (cmd->key_size < 0x07 || cmd->key_size > 0x0f) {
		status = BTP_STATUS_FAILED;
		goto fail;
	}

	val = find_gatt_value_by_id(sys_le16_to_cpu(cmd->attr_id));
	assert(val != NULL);
	assert(val->ptr != NULL);

	switch(val->type) {
	case GATT_VALUE_TYPE_CHR:
		((struct ble_gatt_chr_def *) val->ptr)->min_key_size =
			cmd->key_size;
		break;
	case GATT_VALUE_TYPE_DSC:
		((struct ble_gatt_dsc_def *) val->ptr)->min_key_size =
			cmd->key_size;
		break;
	default:
		break;
	}

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_SET_ENC_KEY_SIZE, CONTROLLER_INDEX,
		   status);
}

static int exchange_func(uint16_t conn_handle,
			 const struct ble_gatt_error *error,
			 uint16_t mtu, void *arg)
{
	SYS_LOG_DBG("");

	if (error->status) {
		tester_rsp(BTP_SERVICE_ID_GATT, GATT_EXCHANGE_MTU,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);

		return 0;
	}

	tester_rsp(BTP_SERVICE_ID_GATT, GATT_EXCHANGE_MTU, CONTROLLER_INDEX,
		   BTP_STATUS_SUCCESS);

	return 0;
}

static void exchange_mtu(u8_t *data, u16_t len)
{
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (ble_gattc_exchange_mtu(conn.conn_handle, exchange_func, NULL)) {
		goto fail;
	}

	return;
fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_EXCHANGE_MTU,
		   CONTROLLER_INDEX, BTP_STATUS_FAILED);
}


static void discover_destroy(void)
{
	gatt_buf_clear();
}

static int disc_prim_uuid_cb(uint16_t conn_handle,
			     const struct ble_gatt_error *error,
			     const struct ble_gatt_svc *gatt_svc, void *arg)
{
	struct gatt_disc_prim_uuid_rp *rp = (void *) gatt_buf.buf;
	struct gatt_service *service;
	const ble_uuid_any_t *uuid;
	u8_t uuid_length;
	u8_t opcode = (u8_t) (int) arg;

	SYS_LOG_DBG("");

	if (error->status != 0) {
		tester_send(BTP_SERVICE_ID_GATT, opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		discover_destroy();
		return 0;
	}

	uuid = &gatt_svc->uuid;
	uuid_length = (uint8_t) (uuid->u.type == BLE_UUID_TYPE_16 ? 2 : 16);

	service = gatt_buf_reserve(sizeof(*service) + uuid_length);
	if (!service) {
		tester_rsp(BTP_SERVICE_ID_GATT, opcode,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return BLE_HS_EDONE;
	}

	service->start_handle = sys_cpu_to_le16(gatt_svc->start_handle);
	service->end_handle = sys_cpu_to_le16(gatt_svc->end_handle);
	service->uuid_length = uuid_length;

	if (uuid->u.type == BLE_UUID_TYPE_16) {
		u16_t u16 = sys_cpu_to_le16(BLE_UUID16(uuid)->value);

		memcpy(service->uuid, &u16, uuid_length);
	} else {
		memcpy(service->uuid, BLE_UUID128(uuid)->value,
		       uuid_length);
	}

	rp->services_count++;

	return 0;
}

static void disc_all_prim_svcs(u8_t *data, u16_t len)
{
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_all_prim_svcs_rp))) {
		goto fail;
	}

	if (ble_gattc_disc_all_svcs(conn.conn_handle,
				    disc_prim_uuid_cb,
				    (void *) GATT_DISC_ALL_PRIM_SVCS)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_PRIM_SVCS,
		   CONTROLLER_INDEX, BTP_STATUS_FAILED);
}

static void disc_prim_uuid(u8_t *data, u16_t len)
{
	const struct gatt_disc_prim_uuid_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	ble_uuid_any_t uuid;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (btp2bt_uuid(cmd->uuid, cmd->uuid_length, &uuid)) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_prim_uuid_rp))) {
		goto fail;
	}

	if (ble_gattc_disc_svc_by_uuid(conn.conn_handle,
				       &uuid.u, disc_prim_uuid_cb,
				       (void *) GATT_DISC_PRIM_UUID)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_PRIM_UUID, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static int find_included_cb(uint16_t conn_handle,
			    const struct ble_gatt_error *error,
			    const struct ble_gatt_svc *gatt_svc, void *arg)
{
	struct gatt_find_included_rp *rp = (void *) gatt_buf.buf;
	struct gatt_included *included;
	const ble_uuid_any_t *uuid;
	int service_handle = (int) arg;
	u8_t uuid_length;

	SYS_LOG_DBG("");

	if (error->status != 0) {
		tester_send(BTP_SERVICE_ID_GATT, GATT_FIND_INCLUDED,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		discover_destroy();
		return 0;
	}

	uuid = &gatt_svc->uuid;
	uuid_length = (uint8_t) (uuid->u.type == BLE_UUID_TYPE_16 ? 2 : 16);


	included = gatt_buf_reserve(sizeof(*included) + uuid_length);
	if (!included) {
		tester_rsp(BTP_SERVICE_ID_GATT, GATT_FIND_INCLUDED,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return BLE_HS_EDONE;
	}

	/* FIXME */
	included->included_handle = sys_cpu_to_le16(service_handle + 1 +
						    rp->services_count);
	included->service.start_handle = sys_cpu_to_le16(gatt_svc->start_handle);
	included->service.end_handle = sys_cpu_to_le16(gatt_svc->end_handle);
	included->service.uuid_length = uuid_length;

	if (uuid->u.type == BLE_UUID_TYPE_16) {
		u16_t u16 = sys_cpu_to_le16(BLE_UUID16(uuid)->value);

		memcpy(included->service.uuid, &u16, uuid_length);
	} else {
		memcpy(included->service.uuid, BLE_UUID128(uuid)->value,
		       uuid_length);
	}

	rp->services_count++;

	return 0;
}

static void find_included(u8_t *data, u16_t len)
{
	const struct gatt_find_included_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	uint16_t start_handle, end_handle;
	int service_handle_arg;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_find_included_rp))) {
		goto fail;
	}

	start_handle = sys_le16_to_cpu(cmd->start_handle);
	end_handle = sys_le16_to_cpu(cmd->end_handle);
	service_handle_arg = start_handle;

	if (ble_gattc_find_inc_svcs(conn.conn_handle, start_handle, end_handle,
				    find_included_cb,
				    (void *)service_handle_arg)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_FIND_INCLUDED, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static int disc_chrc_cb(uint16_t conn_handle,
			const struct ble_gatt_error *error,
			const struct ble_gatt_chr *gatt_chr, void *arg)
{
	struct gatt_disc_chrc_rp *rp = (void *) gatt_buf.buf;
	struct gatt_characteristic *chrc;
	const ble_uuid_any_t *uuid;
	u8_t btp_opcode = (uint8_t) (int) arg;
	u8_t uuid_length;

	SYS_LOG_DBG("");

	if (error->status != 0) {
		tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		discover_destroy();
		return 0;
	}

	uuid = &gatt_chr->uuid;
	uuid_length = (uint8_t) (uuid->u.type == BLE_UUID_TYPE_16 ? 2 : 16);

	chrc = gatt_buf_reserve(sizeof(*chrc) + uuid_length);
	if (!chrc) {
		tester_rsp(BTP_SERVICE_ID_GATT, btp_opcode,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return BLE_HS_EDONE;
	}

	chrc->characteristic_handle = sys_cpu_to_le16(gatt_chr->def_handle);
	chrc->properties = gatt_chr->properties;
	chrc->value_handle = sys_cpu_to_le16(gatt_chr->val_handle);
	chrc->uuid_length = uuid_length;

	if (uuid->u.type == BLE_UUID_TYPE_16) {
		u16_t u16 = sys_cpu_to_le16(BLE_UUID16(uuid)->value);

		memcpy(chrc->uuid, &u16, uuid_length);
	} else {
		memcpy(chrc->uuid, BLE_UUID128(uuid)->value,
		       uuid_length);
	}

	rp->characteristics_count++;

	return 0;
}

static void disc_all_chrc(u8_t *data, u16_t len)
{
	const struct gatt_disc_all_chrc_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	uint16_t start_handle, end_handle;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		SYS_LOG_DBG("Conn find failed");
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_chrc_rp))) {
		SYS_LOG_DBG("Buf reserve failed");
		goto fail;
	}

	start_handle = sys_le16_to_cpu(cmd->start_handle);
	end_handle = sys_le16_to_cpu(cmd->end_handle);

	rc = ble_gattc_disc_all_chrs(conn.conn_handle, start_handle, end_handle,
				     disc_chrc_cb, (void *)GATT_DISC_ALL_CHRC);
	if (rc) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_CHRC, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void disc_chrc_uuid(u8_t *data, u16_t len)
{
	const struct gatt_disc_chrc_uuid_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	uint16_t start_handle, end_handle;
	ble_uuid_any_t uuid;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (btp2bt_uuid(cmd->uuid, cmd->uuid_length, &uuid)) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_chrc_rp))) {
		goto fail;
	}

	start_handle = sys_le16_to_cpu(cmd->start_handle);
	end_handle = sys_le16_to_cpu(cmd->end_handle);

	if (ble_gattc_disc_chrs_by_uuid(conn.conn_handle, start_handle,
					end_handle, &uuid.u, disc_chrc_cb,
					(void *)GATT_DISC_CHRC_UUID)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_CHRC_UUID, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static int disc_all_desc_cb(uint16_t conn_handle,
			    const struct ble_gatt_error *error,
			    uint16_t chr_val_handle,
			    const struct ble_gatt_dsc *gatt_dsc,
			    void *arg)
{
	struct gatt_disc_all_desc_rp *rp = (void *) gatt_buf.buf;
	struct gatt_descriptor *dsc;
	const ble_uuid_any_t *uuid;
	u8_t uuid_length;

	SYS_LOG_DBG("");

	if (error->status != 0) {
		tester_send(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_DESC,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		discover_destroy();
		return 0;
	}

	uuid = &gatt_dsc->uuid;
	uuid_length = (uint8_t) (uuid->u.type == BLE_UUID_TYPE_16 ? 2 : 16);

	dsc = gatt_buf_reserve(sizeof(*dsc) + uuid_length);
	if (!dsc) {
		tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_DESC,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return BLE_HS_EDONE;
	}

	dsc->descriptor_handle = sys_cpu_to_le16(gatt_dsc->handle);
	dsc->uuid_length = uuid_length;

	if (uuid->u.type == BLE_UUID_TYPE_16) {
		u16_t u16 = sys_cpu_to_le16(BLE_UUID16(uuid)->value);

		memcpy(dsc->uuid, &u16, uuid_length);
	} else {
		memcpy(dsc->uuid, BLE_UUID128(uuid)->value,
		       uuid_length);
	}

	rp->descriptors_count++;

	return 0;
}

static void disc_all_desc(u8_t *data, u16_t len)
{
	const struct gatt_disc_all_desc_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	uint16_t start_handle, end_handle;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_all_desc_rp))) {
		goto fail;
	}

	start_handle = sys_le16_to_cpu(cmd->start_handle) - 1;
	end_handle = sys_le16_to_cpu(cmd->end_handle);

	rc = ble_gattc_disc_all_dscs(conn.conn_handle, start_handle, end_handle,
				     disc_all_desc_cb, NULL);

	SYS_LOG_DBG("rc=%d", rc);

	if (rc) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_DESC, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void read_destroy()
{
	gatt_buf_clear();
}

static int read_cb(uint16_t conn_handle,
		   const struct ble_gatt_error *error,
		   struct ble_gatt_attr *attr,
		   void *arg)
{
	struct gatt_read_rp *rp = (void *) gatt_buf.buf;
	u8_t btp_opcode = (uint8_t) (int) arg;

	SYS_LOG_DBG("status=%d", error->status);

	if (error->status != 0 && error->status != BLE_HS_EDONE) {
		rp->att_response = (uint8_t) BLE_HS_ATT_ERR(error->status);
		tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		read_destroy();
		return 0;
	}

	if (!gatt_buf_add(attr->om->om_data, attr->om->om_len)) {
		tester_rsp(BTP_SERVICE_ID_GATT, btp_opcode,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		read_destroy();
		return 0;
	}

	rp->data_length += attr->om->om_len;

	tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
		    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
	read_destroy();

	return 0;
}

static int read_long_cb(uint16_t conn_handle,
			const struct ble_gatt_error *error,
			struct ble_gatt_attr *attr,
			void *arg)
{
	struct gatt_read_rp *rp = (void *) gatt_buf.buf;
	u8_t btp_opcode = (uint8_t) (int) arg;

	SYS_LOG_DBG("status=%d", error->status);

	if (error->status != 0 && error->status != BLE_HS_EDONE) {
		rp->att_response = (uint8_t) BLE_HS_ATT_ERR(error->status);
		tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		read_destroy();
		return 0;
	}

	if (error->status == BLE_HS_EDONE) {
		tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		read_destroy();
		return 0;
	}

	if (gatt_buf_add(attr->om->om_data, attr->om->om_len) == NULL) {
		tester_rsp(BTP_SERVICE_ID_GATT, btp_opcode,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		read_destroy();
		return 0;
	}

	rp->data_length += attr->om->om_len;

	return 0;
}

static void read(u8_t *data, u16_t len)
{
	const struct gatt_read_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_read_rp))) {
		goto fail;
	}

	if (ble_gattc_read(conn.conn_handle, sys_le16_to_cpu(cmd->handle),
			   read_cb, (void *)GATT_READ)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_READ, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void read_uuid(u8_t *data, u16_t len)
{
	const struct gatt_read_uuid_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	ble_uuid_any_t uuid;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (btp2bt_uuid(cmd->uuid, cmd->uuid_length, &uuid)) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_read_rp))) {
		goto fail;
	}

	if (ble_gattc_read_by_uuid(conn.conn_handle,
				   sys_le16_to_cpu(cmd->start_handle),
				   sys_le16_to_cpu(cmd->end_handle),
				   &uuid.u,
				   read_long_cb, (void *)GATT_READ_UUID)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_READ, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void read_long(u8_t *data, u16_t len)
{
	const struct gatt_read_long_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_read_rp))) {
		goto fail;
	}

	if (ble_gattc_read_long(conn.conn_handle,
				sys_le16_to_cpu(cmd->handle),
				sys_le16_to_cpu(cmd->offset),
				read_long_cb, (void *)GATT_READ_LONG)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_READ_LONG, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void read_multiple(u8_t *data, u16_t len)
{
	const struct gatt_read_multiple_cmd *cmd = (void *) data;
	u16_t handles[cmd->handles_count];
	struct ble_gap_conn_desc conn;
	int rc, i;

	SYS_LOG_DBG("");

	for (i = 0; i < ARRAY_SIZE(handles); i++) {
		handles[i] = sys_le16_to_cpu(cmd->handles[i]);
	}

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_read_rp))) {
		goto fail;
	}

	if (ble_gattc_read_mult(conn.conn_handle, handles,
				cmd->handles_count, read_cb,
				(void *)GATT_READ_MULTIPLE)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_READ_MULTIPLE, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void write_without_rsp(u8_t *data, u16_t len, u8_t op,
			      bool sign)
{
	const struct gatt_write_without_rsp_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	u8_t status = BTP_STATUS_SUCCESS;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	if (ble_gattc_write_no_rsp_flat(conn.conn_handle,
					sys_le16_to_cpu(cmd->handle),
					cmd->data,
					sys_le16_to_cpu(cmd->data_length))) {
		status = BTP_STATUS_FAILED;
	}

rsp:
	tester_rsp(BTP_SERVICE_ID_GATT, op, CONTROLLER_INDEX, status);
}

static int write_rsp(uint16_t conn_handle,
		     const struct ble_gatt_error *error,
		     struct ble_gatt_attr *attr,
		     void *arg)
{
	uint8_t err = (uint8_t) error->status;
	u8_t btp_opcode = (uint8_t) (int) arg;

	SYS_LOG_DBG("");

	tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
		    CONTROLLER_INDEX, &err,
		    sizeof(err));
	return 0;
}

static void write(u8_t *data, u16_t len)
{
	const struct gatt_write_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (ble_gattc_write_flat(conn.conn_handle, sys_le16_to_cpu(cmd->handle),
				 cmd->data, sys_le16_to_cpu(cmd->data_length),
				 write_rsp, (void *) GATT_WRITE)) {
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_WRITE, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void write_long(u8_t *data, u16_t len)
{
	const struct gatt_write_long_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	struct os_mbuf *om = NULL;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	om = ble_hs_mbuf_from_flat(cmd->data, sys_le16_to_cpu(cmd->data_length));
	/* This is required, because Nimble checks if
	 * the data is longer than offset
	 */
	if (os_mbuf_extend(om, cmd->offset + 1) == NULL) {
		goto fail;
	}

	if (ble_gattc_write_long(conn.conn_handle, sys_le16_to_cpu(cmd->handle),
				 sys_le16_to_cpu(cmd->offset), om, write_rsp,
				 (void *) GATT_WRITE_LONG)) {
		goto fail;
	}

	return;

fail:
	os_mbuf_free_chain(om);

	tester_rsp(BTP_SERVICE_ID_GATT, GATT_WRITE_LONG, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static int reliable_write_rsp(uint16_t conn_handle,
			      const struct ble_gatt_error *error,
			      struct ble_gatt_attr *attrs,
			      uint8_t num_attrs,
			      void *arg)
{
	uint8_t err = (uint8_t) error->status;

	SYS_LOG_DBG("");

	tester_send(BTP_SERVICE_ID_GATT, GATT_RELIABLE_WRITE,
		    CONTROLLER_INDEX, &err,
		    sizeof(err));
	return 0;
}

static void reliable_write(u8_t *data, u16_t len)
{
	const struct gatt_reliable_write_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	struct ble_gatt_attr attr;
	struct os_mbuf *om = NULL;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	om = ble_hs_mbuf_from_flat(cmd->data, sys_le16_to_cpu(cmd->data_length));
	/* This is required, because Nimble checks if
	 * the data is longer than offset
	 */
	if (os_mbuf_extend(om, sys_le16_to_cpu(cmd->offset) + 1) == NULL) {
		goto fail;
	}

	attr.handle = sys_le16_to_cpu(cmd->handle);
	attr.offset = sys_le16_to_cpu(cmd->offset);
	attr.om = om;

	if (ble_gattc_write_reliable(conn.conn_handle, &attr, 1,
				     reliable_write_rsp, NULL)) {
		goto fail;
	}

	return;

	fail:
	os_mbuf_free_chain(om);

	tester_rsp(BTP_SERVICE_ID_GATT, GATT_WRITE_LONG, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static struct bt_gatt_subscribe_params {
	u16_t ccc_handle;
	u16_t value;
	u16_t value_handle;
} subscribe_params;

/* ev header + default MTU_ATT-3 */
static u8_t ev_buf[33];

int tester_gatt_notify_rx_ev(u16_t conn_handle, u16_t attr_handle,
			     u8_t indication, struct os_mbuf *om)
{
	struct gatt_notification_ev *ev = (void *) ev_buf;
	struct ble_gap_conn_desc conn;
	const ble_addr_t *addr;
	int rc;

	SYS_LOG_DBG("");

	if (!subscribe_params.ccc_handle) {
		return 0;
	}

	rc = ble_gap_conn_find(conn_handle, &conn);
	if (rc) {
		return -1;
	}

	addr = &conn.peer_ota_addr;

	ev->type = (u8_t) (indication ? 0x02 : 0x01);
	ev->handle = sys_cpu_to_le16(attr_handle);
	ev->data_length = sys_cpu_to_le16(om->om_len);
	memcpy(ev->data, om->om_data, om->om_len);
	memcpy(ev->address, addr->val, sizeof(ev->address));
	ev->address_type = addr->type;

	tester_send(BTP_SERVICE_ID_GATT, GATT_EV_NOTIFICATION,
		    CONTROLLER_INDEX, ev_buf, sizeof(*ev) + om->om_len);

	return 0;

}

int tester_gatt_subscribe_ev(u16_t conn_handle, u16_t attr_handle, u8_t reason,
			     u8_t prev_notify, u8_t cur_notify,
			     u8_t prev_indicate, u8_t cur_indicate)
{
	SYS_LOG_DBG("");

	if (cur_notify == 0 && cur_indicate == 0) {
		SYS_LOG_INF("Unsubscribed");
		memset(&subscribe_params, 0, sizeof(subscribe_params));
		return 0;
	}

	if (cur_notify) {
		SYS_LOG_INF("Subscribed to notifications");
	}

	if (cur_indicate) {
		SYS_LOG_INF("Subscribed to indications");
	}

	return 0;
}

static int enable_subscription(u16_t conn_handle, u16_t ccc_handle,
			       u16_t value)
{
	u8_t op;

	SYS_LOG_DBG("");

	op = (uint8_t) (value == 0x0001 ? GATT_CFG_NOTIFY :
			GATT_CFG_INDICATE);

	if (ble_gattc_write_flat(conn_handle, ccc_handle,
				 &value, sizeof(value), NULL, NULL)) {
		return -EINVAL;
	}

	subscribe_params.ccc_handle = value;

	tester_rsp(BTP_SERVICE_ID_GATT, op, CONTROLLER_INDEX,
		   BTP_STATUS_SUCCESS);
	return 0;
}

static int disable_subscription(u16_t conn_handle, u16_t ccc_handle)
{
	u16_t value = 0x00;

	SYS_LOG_DBG("");

	/* Fail if CCC handle doesn't match */
	if (ccc_handle != subscribe_params.ccc_handle) {
		SYS_LOG_ERR("CCC handle doesn't match");
		return -EINVAL;
	}

	if (ble_gattc_write_no_rsp_flat(conn_handle, ccc_handle,
					&value, sizeof(value))) {
		return -EINVAL;
	}

	subscribe_params.ccc_handle = 0;
	return 0;
}

static void config_subscription(u8_t *data, u16_t len, u8_t op)
{
	const struct gatt_cfg_notify_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	u16_t ccc_handle = sys_le16_to_cpu(cmd->ccc_handle);
	u8_t status;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		tester_rsp(BTP_SERVICE_ID_GATT, op, CONTROLLER_INDEX,
			   BTP_STATUS_FAILED);
		return;
	}

	if (cmd->enable) {
		u16_t value;

		if (op == GATT_CFG_NOTIFY) {
			value = 0x0001;
		} else {
			value = 0x0002;
		}

		/* on success response will be sent from callback */
		if (enable_subscription(conn.conn_handle,
					ccc_handle, value) == 0) {
			return;
		}

		status = BTP_STATUS_FAILED;
	} else {
		if (disable_subscription(conn.conn_handle, ccc_handle) < 0) {
			status = BTP_STATUS_FAILED;
		} else {
			status = BTP_STATUS_SUCCESS;
		}
	}

	SYS_LOG_DBG("Config subscription (op %u) status %u", op, status);

	tester_rsp(BTP_SERVICE_ID_GATT, op, CONTROLLER_INDEX, status);
}

struct get_attrs_foreach_data {
	ble_uuid_any_t *uuid;
	u16_t start_handle, end_handle;
	struct os_mbuf *buf;
	u8_t count;
};

static u8_t foreach_get_attrs(u16_t handle,
			      u8_t permission,
			      const ble_uuid_t *uuid,
			      struct get_attrs_foreach_data *foreach)
{
	struct gatt_attr *gatt_attr;
	char uuid_buf[BLE_UUID_STR_LEN];

	if (handle < foreach->start_handle && handle > foreach->end_handle) {
		return 0;
	}

	if (foreach->uuid && ble_uuid_cmp(&foreach->uuid->u, uuid)) {
		return 0;
	}

	SYS_LOG_DBG("hdl 0x%04x perm 0x%02x uuid %s", handle, permission,
		    ble_uuid_to_str(uuid, uuid_buf));

	gatt_attr = net_buf_simple_add(foreach->buf, sizeof(*gatt_attr));
	gatt_attr->handle = sys_cpu_to_le16(handle);
	gatt_attr->permission = permission;

	if (uuid->type == BLE_UUID_TYPE_16) {
		gatt_attr->type_length = 2;
		net_buf_simple_add_le16(foreach->buf,
					BLE_UUID16(uuid)->value);
	} else {
		gatt_attr->type_length = 16;
		net_buf_simple_add_mem(foreach->buf,
				       BLE_UUID128(uuid)->value,
				       gatt_attr->type_length);
	}

	foreach->count++;

	return 0;
}

static u8_t flags2perm(u16_t flags, bool chr)
{
	u8_t permission = 0;
	int *flag_arr;
	int flag_arr_len;
	int i;

	if (chr) {
		flag_arr = gatt_chr_perm_map;
		flag_arr_len = ARRAY_SIZE(gatt_chr_perm_map);

		/* FIXME: Handle WRITE_NO_RSP */
		if (flags & BLE_GATT_CHR_F_WRITE_NO_RSP) {
			permission |= BIT(1);
		}
	} else {
		flag_arr = gatt_dsc_perm_map;
		flag_arr_len = ARRAY_SIZE(gatt_dsc_perm_map);
	}

	for (i = 0; i < flag_arr_len; ++i) {
		if (flags & flag_arr[i]) {
			permission |= BIT(i);
		}
	}

	return permission;
}

static void get_attrs_cb(const struct ble_gatt_svc_def *svc,
			 uint16_t handle, uint16_t end_group_handle,
			 void *arg)
{
	struct get_attrs_foreach_data *foreach = arg;
	const struct ble_gatt_chr_def *chr;
	const struct ble_gatt_dsc_def *dsc;

	SYS_LOG_DBG("");

	if (svc->type == BLE_GATT_SVC_TYPE_PRIMARY) {
		foreach_get_attrs(handle, BTP_GATT_PERM_READ,
				  uuid_pri, foreach);
	} else if (svc->type == BLE_GATT_SVC_TYPE_SECONDARY) {
		foreach_get_attrs(handle, BTP_GATT_PERM_READ,
				  uuid_sec, foreach);
	}

	/* TODO: iterate over included services*/

	for (chr = svc->characteristics; chr && chr->uuid; ++chr) {
		handle += 1;
		foreach_get_attrs(handle, BTP_GATT_PERM_READ,
				  uuid_chr, foreach);
		handle += 1;
		foreach_get_attrs(handle, flags2perm(chr->flags, true),
				  chr->uuid, foreach);

		if ((chr->flags & BLE_GATT_CHR_F_NOTIFY) ||
		    (chr->flags & BLE_GATT_CHR_F_INDICATE)) {
			handle += 1;

			foreach_get_attrs(handle,
					  flags2perm(BLE_ATT_F_READ |
						     BLE_ATT_F_WRITE,
						     false), uuid_ccc, foreach);
		}

		for (dsc = chr->descriptors; dsc && dsc->uuid; ++dsc) {
			handle += 1;
			foreach_get_attrs(handle, flags2perm(chr->flags, false),
					  dsc->uuid, foreach);
		}
	}
}

static void get_attrs(u8_t *data, u16_t len)
{
	const struct gatt_get_attributes_cmd *cmd = (void *) data;
	struct gatt_get_attributes_rp *rp;
	struct os_mbuf *buf = os_msys_get(0, 0);
	struct get_attrs_foreach_data foreach;
	u16_t start_handle, end_handle;
	ble_uuid_any_t uuid;
	char str[BLE_UUID_STR_LEN];

	SYS_LOG_DBG("");

	memset(str, 0, sizeof(str));
	memset(&uuid, 0, sizeof(uuid));
	start_handle = sys_le16_to_cpu(cmd->start_handle);
	end_handle = sys_le16_to_cpu(cmd->end_handle);

	if (cmd->type_length) {
		if (btp2bt_uuid(cmd->type, cmd->type_length, &uuid)) {
			goto fail;
		}

		ble_uuid_to_str(&uuid.u, str);
		SYS_LOG_DBG("start 0x%04x end 0x%04x, uuid %s", start_handle,
			    end_handle, str);

		foreach.uuid = &uuid;
	} else {
		SYS_LOG_DBG("start 0x%04x end 0x%04x", start_handle, end_handle);
		foreach.uuid = NULL;
	}

	net_buf_simple_init(buf, sizeof(*rp));

	foreach.start_handle = start_handle;
	foreach.end_handle = end_handle;
	foreach.buf = buf;
	foreach.count = 0;

	ble_gatts_lcl_svc_foreach(get_attrs_cb, &foreach);

	rp = (void *) net_buf_simple_push(buf, sizeof(*rp));
	rp->attrs_count = foreach.count;

	tester_send(BTP_SERVICE_ID_GATT, GATT_GET_ATTRIBUTES, CONTROLLER_INDEX,
		    buf->om_data, buf->om_len);

	goto free;
fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_GET_ATTRIBUTES, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
free:
	os_mbuf_free_chain(buf);
}

static int access_svc(uint16_t conn_handle, uint16_t attr_handle,
		      struct ble_gatt_access_ctxt *ctxt, void *arg)
{
	const struct ble_gatt_svc_def *svc = arg;

	if (svc->uuid->type == BLE_UUID_TYPE_16) {
		net_buf_simple_add_le16(ctxt->om,
					BLE_UUID16(svc->uuid)->value);
	} else {
		os_mbuf_append(ctxt->om,
			       BLE_UUID128(svc->uuid)->value,
			       16);
	}

	return 0;
}

static int access_chr(uint16_t conn_handle, uint16_t attr_handle,
		      struct ble_gatt_access_ctxt *ctxt, void *arg)
{
	const struct ble_gatt_chr_def *chr = arg;

	net_buf_simple_add_u8(ctxt->om, flags2perm(chr->flags, true));
	net_buf_simple_add_le16(ctxt->om, *chr->val_handle);

	if (chr->uuid->type == BLE_UUID_TYPE_16) {
		net_buf_simple_add_le16(ctxt->om,
					BLE_UUID16(chr->uuid)->value);
	} else {
		os_mbuf_append(ctxt->om,
			       BLE_UUID128(chr->uuid)->value,
			       16);
	}

	return 0;
}

static int access_cccd(uint16_t conn_handle, uint16_t attr_handle,
		       struct ble_gatt_access_ctxt *ctxt, void *arg)
{
	return ble_gatts_clt_cfg_access(conn_handle, attr_handle,
					ctxt->op, 0, &ctxt->om, arg);
}

static int foreach_get_attr_val(u16_t handle,
				ble_gatt_access_fn access_cb,
				void *arg,
				struct ble_gatt_access_ctxt *ctxt,
				u16_t rx_handle)

{
	struct gatt_get_attribute_value_rp *rp;
	uint16_t hdr_len;
	int rc;

	if (handle != rx_handle) {
		return 0;
	}

	SYS_LOG_DBG("handle=%d", handle);

	rp = net_buf_simple_add(ctxt->om, sizeof(*rp));

	SYS_LOG_DBG("len=%d", ctxt->om->om_len);
	hdr_len = ctxt->om->om_len;

	rc = access_cb(1, handle, ctxt, arg);

	rp->att_response = (uint8_t) rc;
	SYS_LOG_DBG("len=%d", ctxt->om->om_len);
	rp->value_length = ctxt->om->om_len - hdr_len;

	return BLE_HS_EDONE;
}

static void get_attr_val_cb(const struct ble_gatt_svc_def *svc,
			    uint16_t handle, uint16_t end_group_handle,
			    void *arg)
{
	struct get_attrs_foreach_data *foreach = arg;
	struct os_mbuf *buf = foreach->buf;
	struct ble_gatt_access_ctxt ctxt;
	const struct ble_gatt_chr_def *chr;
	const struct ble_gatt_dsc_def *dsc;
	int rc;

	SYS_LOG_DBG("");

	ctxt.om = buf;

	rc = foreach_get_attr_val(handle, access_svc, (void *) svc,
				  &ctxt, foreach->start_handle);
	if (rc == BLE_HS_EDONE) {
		return;
	}

	/* TODO: iterate over included services*/

	for (chr = svc->characteristics; chr && chr->uuid; ++chr) {
		handle += 1;
		rc = foreach_get_attr_val(handle, access_chr, (void *) chr,
					  &ctxt, foreach->start_handle);
		if (rc == BLE_HS_EDONE) {
			return;
		}

		handle += 1;
		ctxt.op = BLE_GATT_ACCESS_OP_READ_CHR;
		ctxt.chr = chr;

		rc = foreach_get_attr_val(handle,
					  chr->access_cb, chr->arg,
					  &ctxt, foreach->start_handle);
		if (rc == BLE_HS_EDONE) {
			return;
		}

		if ((chr->flags & BLE_GATT_CHR_F_NOTIFY) ||
		    (chr->flags & BLE_GATT_CHR_F_INDICATE)) {
			handle += 1;
			ctxt.op = BLE_GATT_ACCESS_OP_READ_CHR;

			rc = foreach_get_attr_val(handle,
						  access_cccd, NULL,
						  &ctxt, foreach->start_handle);
			if (rc == BLE_HS_EDONE) {
				return;
			}
		}

		for (dsc = chr->descriptors; dsc && dsc->uuid; ++dsc) {
			handle += 1;

			ctxt.op = BLE_GATT_ACCESS_OP_READ_DSC;
			ctxt.dsc = dsc;

			rc = foreach_get_attr_val(handle,
						  dsc->access_cb, dsc->arg,
						  &ctxt, foreach->start_handle);
			if (rc == BLE_HS_EDONE) {
				return;
			}
		}
	}
}

static void get_attr_val(u8_t *data, u16_t len)
{
	const struct gatt_get_attribute_value_cmd *cmd = (void *) data;
	struct os_mbuf *buf = os_msys_get(0, 0);
	struct get_attrs_foreach_data foreach;
	u16_t handle = sys_cpu_to_le16(cmd->handle);

	SYS_LOG_DBG("handle=%d", handle);

	memset(&foreach, 0, sizeof(foreach));
	net_buf_simple_init(buf, 0);

	foreach.buf = buf;
	foreach.start_handle = handle;

	ble_gatts_lcl_svc_foreach(get_attr_val_cb, &foreach);

	if (buf->om_len) {
		tester_send(BTP_SERVICE_ID_GATT, GATT_GET_ATTRIBUTE_VALUE,
			    CONTROLLER_INDEX, buf->om_data, buf->om_len);
	} else {
		tester_rsp(BTP_SERVICE_ID_GATT, GATT_GET_ATTRIBUTE_VALUE,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
	}

	os_mbuf_free_chain(buf);
}

void tester_handle_gatt(u8_t opcode, u8_t index, u8_t *data,
			u16_t len)
{
	switch (opcode) {
	case GATT_READ_SUPPORTED_COMMANDS:
		supported_commands(data, len);
		return;
	case GATT_ADD_SERVICE:
		add_service(data, len);
		return;
	case GATT_ADD_CHARACTERISTIC:
		add_characteristic(data, len);
		return;
	case GATT_ADD_DESCRIPTOR:
		add_descriptor(data, len);
		return;
	case GATT_ADD_INCLUDED_SERVICE:
		add_included(data, len);
		return;
	case GATT_SET_VALUE:
		set_value(data, len);
		return;
	case GATT_START_SERVER:
		start_server(data, len);
		return;
	case GATT_SET_ENC_KEY_SIZE:
		set_enc_key_size(data, len);
		return;
	case GATT_EXCHANGE_MTU:
		exchange_mtu(data, len);
		return;
	case GATT_DISC_ALL_PRIM_SVCS:
		disc_all_prim_svcs(data, len);
		return;
	case GATT_DISC_PRIM_UUID:
		disc_prim_uuid(data, len);
		return;
	case GATT_FIND_INCLUDED:
		find_included(data, len);
		return;
	case GATT_DISC_ALL_CHRC:
		disc_all_chrc(data, len);
		return;
	case GATT_DISC_CHRC_UUID:
		disc_chrc_uuid(data, len);
		return;
	case GATT_DISC_ALL_DESC:
		disc_all_desc(data, len);
		return;
	case GATT_READ:
		read(data, len);
		return;
	case GATT_READ_UUID:
		read_uuid(data, len);
		return;
	case GATT_READ_LONG:
		read_long(data, len);
		return;
	case GATT_READ_MULTIPLE:
		read_multiple(data, len);
		return;
	case GATT_WRITE_WITHOUT_RSP:
		write_without_rsp(data, len, opcode, false);
		return;
#if 0
	case GATT_SIGNED_WRITE_WITHOUT_RSP:
		write_without_rsp(data, len, opcode, true);
		return;
#endif
	case GATT_WRITE:
		write(data, len);
		return;
	case GATT_WRITE_LONG:
		write_long(data, len);
		return;
	case GATT_RELIABLE_WRITE:
		reliable_write(data, len);
		return;
	case GATT_CFG_NOTIFY:
	case GATT_CFG_INDICATE:
		config_subscription(data, len, opcode);
		return;
	case GATT_GET_ATTRIBUTES:
		get_attrs(data, len);
		return;
	case GATT_GET_ATTRIBUTE_VALUE:
		get_attr_val(data, len);
		return;
	default:
		tester_rsp(BTP_SERVICE_ID_GATT, opcode, index,
			   BTP_STATUS_UNKNOWN_CMD);
		return;
	}
}

static bool gatt_svr_svc_exists(const struct ble_gatt_svc_def *svc_def)
{
	const struct ble_gatt_svc_def *svc;

	for (svc = &gatt_svr_svcs[0]; svc && svc->uuid; svc++) {
		if (svc == svc_def) {
			return true;
		}
	}

	for (svc = &gatt_svr_inc_svcs[0]; svc && svc->uuid; svc++) {
		if (svc == svc_def) {
			return true;
		}
	}

	return false;
}

static bool gatt_svr_chr_exists(const struct ble_gatt_chr_def *chr_def)
{
	const struct ble_gatt_svc_def *svc;
	const struct ble_gatt_chr_def *chr;

	for (svc = &gatt_svr_svcs[0]; svc && svc->uuid; svc++) {
		for (chr = &svc->characteristics[0]; chr && chr->uuid; chr++) {
			if (chr == chr_def) {
				return true;
			}
		}
	}

	for (svc = &gatt_svr_inc_svcs[0]; svc && svc->uuid; svc++) {
		for (chr = &svc->characteristics[0]; chr && chr->uuid; chr++) {
			if (chr == chr_def) {
				return true;
			}
		}
	}

	return false;
}

static bool gatt_svr_dsc_exists(const struct ble_gatt_dsc_def *dsc_def)
{
	const struct ble_gatt_svc_def *svc;
	const struct ble_gatt_chr_def *chr;
	const struct ble_gatt_dsc_def *dsc;

	for (svc = &gatt_svr_svcs[0]; svc && svc->uuid; svc++) {
		for (chr = &svc->characteristics[0]; chr && chr->uuid; chr++) {
			for (dsc = &chr->descriptors[0];
			     dsc && dsc->uuid; dsc++) {
				if (dsc == dsc_def) {
					return true;
				}
			}
		}
	}

	for (svc = &gatt_svr_inc_svcs[0]; svc && svc->uuid; svc++) {
		for (chr = &svc->characteristics[0]; chr && chr->uuid; chr++) {
			for (dsc = &chr->descriptors[0];
			     dsc && dsc->uuid; dsc++) {
				if (dsc == dsc_def) {
					return true;
				}
			}
		}
	}

	return false;
}

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
	char buf[BLE_UUID_STR_LEN];

	switch (ctxt->op) {
		case BLE_GATT_REGISTER_OP_SVC:
			MODLOG_DFLT(DEBUG,
				    "registered service %s with handle=%d\n",
				    ble_uuid_to_str(ctxt->svc.svc_def->uuid,
						    buf),
				    ctxt->svc.handle);
			if (gatt_svr_svc_exists(ctxt->svc.svc_def)) {
				if (ctxt->svc.handle < db_attr_min) {
					db_attr_min = ctxt->svc.handle;
				}
				if (ctxt->svc.handle > db_attr_max) {
					db_attr_max = ctxt->svc.handle;
				}
			}
			break;

		case BLE_GATT_REGISTER_OP_CHR:
			MODLOG_DFLT(DEBUG, "registering characteristic %s with "
					   "def_handle=%d val_handle=%d\n",
				    ble_uuid_to_str(ctxt->chr.chr_def->uuid,
						    buf),
				    ctxt->chr.def_handle,
				    ctxt->chr.val_handle);
			if (gatt_svr_chr_exists(ctxt->chr.chr_def)) {
				if (ctxt->chr.def_handle < db_attr_min) {
					db_attr_min = ctxt->chr.def_handle;
				}
				if (ctxt->chr.val_handle < db_attr_min) {
					db_attr_min = ctxt->chr.val_handle;
				}
				if (ctxt->chr.def_handle > db_attr_max) {
					db_attr_max = ctxt->chr.def_handle;
				}
				if (ctxt->chr.val_handle > db_attr_max) {
					db_attr_max = ctxt->chr.val_handle;
				}
			}
			break;

		case BLE_GATT_REGISTER_OP_DSC:
			MODLOG_DFLT(DEBUG,
				    "registering descriptor %s with handle=%d\n",
				    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
				    ctxt->dsc.handle);
			if (gatt_svr_dsc_exists(ctxt->dsc.dsc_def)) {
				if (ctxt->dsc.handle < db_attr_min) {
					db_attr_min = ctxt->dsc.handle;
				}
				if (ctxt->dsc.handle > db_attr_max) {
					db_attr_max = ctxt->dsc.handle;
				}
			}
			break;

		/* FIXME: handle incldued services*/

		default:
			assert(0);
			break;
	}
}

int gatt_svr_init(void)
{
	int rc;

	rc = ble_gatts_count_cfg(gatt_svr_svcs);
	if (rc != 0) {
		return rc;
	}

	rc = ble_gatts_add_svcs(gatt_svr_svcs);
	if (rc != 0) {
		return rc;
	}

	rc = ble_gatts_count_cfg(gatt_svr_inc_svcs);
	if (rc != 0) {
		return rc;
	}

	rc = ble_gatts_add_svcs(gatt_svr_inc_svcs);
	if (rc != 0) {
		return rc;
	}

	init_gatt_values();

	return 0;
}

u8_t tester_init_gatt(void)
{
	return BTP_STATUS_SUCCESS;
}

u8_t tester_unregister_gatt(void)
{
	return BTP_STATUS_SUCCESS;
}
