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

#include <stddef.h>
#include <errno.h>
#include <string.h>
#include "testutil/testutil.h"
#include "nimble/hci_common.h"
#include "ble_hs_test.h"
#include "host/ble_uuid.h"
#include "ble_hs_test_util.h"

static uint8_t *ble_att_svr_test_attr_r_1;
static uint16_t ble_att_svr_test_attr_r_1_len;
static uint8_t *ble_att_svr_test_attr_r_2;
static uint16_t ble_att_svr_test_attr_r_2_len;

static uint8_t ble_att_svr_test_attr_w_1[1024];
static uint16_t ble_att_svr_test_attr_w_1_len;
static uint8_t ble_att_svr_test_attr_w_2[1024];
static uint16_t ble_att_svr_test_attr_w_2_len;

static uint16_t ble_att_svr_test_n_conn_handle;
static uint16_t ble_att_svr_test_n_attr_handle;
static uint8_t ble_att_svr_test_attr_n[1024];
static uint16_t ble_att_svr_test_attr_n_len;

static void
ble_att_svr_test_assert_mbufs_freed(void)
{
    /* When checking for mbuf leaks, ensure no stale prep entries. */
    static const struct ble_hs_test_util_mbuf_params mbuf_params = {
        .prev_tx = 1,
        .rx_queue = 1,
        .prep_list = 0,
    };

    ble_hs_test_util_assert_mbufs_freed(&mbuf_params);
}

static int
ble_att_svr_test_misc_gap_cb(struct ble_gap_event *event, void *arg)
{
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_NOTIFY_RX:
        ble_att_svr_test_n_conn_handle = event->notify_rx.conn_handle;
        ble_att_svr_test_n_attr_handle = event->notify_rx.attr_handle;
        TEST_ASSERT_FATAL(OS_MBUF_PKTLEN(event->notify_rx.om) <=
                          sizeof ble_att_svr_test_attr_n);
        ble_att_svr_test_attr_n_len = OS_MBUF_PKTLEN(event->notify_rx.om);
        rc = os_mbuf_copydata(event->notify_rx.om, 0,
                              ble_att_svr_test_attr_n_len,
                              ble_att_svr_test_attr_n);
        TEST_ASSERT_FATAL(rc == 0);
        break;

    default:
        break;
    }

    return 0;
}

/**
 * @return                      The handle of the new test connection.
 */
static uint16_t
ble_att_svr_test_misc_init(uint16_t mtu)
{
    struct ble_l2cap_chan *chan;
    struct ble_hs_conn *conn;
    int rc;

    ble_hs_test_util_init();

    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 ble_att_svr_test_misc_gap_cb, NULL);

    ble_hs_lock();

    rc = ble_hs_misc_conn_chan_find(2, BLE_L2CAP_CID_ATT, &conn, &chan);
    TEST_ASSERT_FATAL(rc == 0);

    if (mtu != 0) {
        chan->my_mtu = mtu;
        chan->peer_mtu = mtu;
        chan->flags |= BLE_L2CAP_CHAN_F_TXED_MTU;
    }

    ble_hs_unlock();

    ble_att_svr_test_attr_r_1_len = 0;
    ble_att_svr_test_attr_r_2_len = 0;
    ble_att_svr_test_attr_w_1_len = 0;

    return 2;
}

static int
ble_att_svr_test_misc_attr_fn_r_1(uint16_t conn_handle, uint16_t attr_handle,
                                  uint8_t op, uint16_t offset,
                                  struct os_mbuf **om, void *arg)
{
    int rc;

    switch (op) {
    case BLE_ATT_ACCESS_OP_READ:
        if (offset > ble_att_svr_test_attr_r_1_len) {
            return BLE_ATT_ERR_INVALID_OFFSET;
        }

        rc = os_mbuf_append(*om, ble_att_svr_test_attr_r_1 + offset,
                            ble_att_svr_test_attr_r_1_len - offset);
        TEST_ASSERT_FATAL(rc == 0);
        return 0;

    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

static int
ble_att_svr_test_misc_attr_fn_r_2(uint16_t conn_handle, uint16_t attr_handle,
                                  uint8_t op, uint16_t offset,
                                  struct os_mbuf **om, void *arg)
{
    int rc;

    switch (op) {
    case BLE_ATT_ACCESS_OP_READ:
        if (offset > ble_att_svr_test_attr_r_2_len) {
            return BLE_ATT_ERR_INVALID_OFFSET;
        }

        rc = os_mbuf_append(*om, ble_att_svr_test_attr_r_2 + offset,
                            ble_att_svr_test_attr_r_2_len - offset);
        TEST_ASSERT_FATAL(rc == 0);
        return 0;

    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

static int
ble_att_svr_test_misc_attr_fn_r_err(uint16_t conn_handle, uint16_t attr_handle,
                                    uint8_t op, uint16_t offset,
                                    struct os_mbuf **om, void *arg)
{
    int rc;

    rc = os_mbuf_append(*om, (uint8_t[4]){1,2,3,4}, 4);
    TEST_ASSERT_FATAL(rc == 0);

    return BLE_ATT_ERR_UNLIKELY;
}

#define BLE_ATT_SVR_TEST_LAST_SVC  11
#define BLE_ATT_SVR_TEST_LAST_ATTR 24

static int
ble_att_svr_test_misc_attr_fn_r_group(uint16_t conn_handle,
                                      uint16_t attr_handle,
                                      uint8_t op,
                                      uint16_t offset,
                                      struct os_mbuf **om,
                                      void *arg)
{
    uint8_t *src;
    int rc;

    /* Service 0x1122 from 1 to 5 */
    /* Service 0x2233 from 6 to 10 */
    /* Service 010203...0f from 11 to 24 */

    static uint8_t vals[25][16] = {
        [1] =   { 0x22, 0x11 },
        [2] =   { 0x01, 0x11 },
        [3] =   { 0x02, 0x11 },
        [4] =   { 0x03, 0x11 },
        [5] =   { 0x04, 0x11 },
        [6] =   { 0x33, 0x22 },
        [7] =   { 0x01, 0x22 },
        [8] =   { 0x02, 0x22 },
        [9] =   { 0x03, 0x22 },
        [10] =  { 0x04, 0x22 },
        [11] =  { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 },
        [12] =  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
        [13] =  { 0xdd, 0xdd },
        [14] =  { 0x55, 0x55 },
        [15] =  { 0xdd, 0xdd },
        [16] =  { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
        [17] =  { 0xdd, 0xdd },
        [18] =  { 0x66, 0x66 },
        [19] =  { 0xdd, 0xdd },
        [20] =  { 0x77, 0x77 },
        [21] =  { 0xdd, 0xdd },
        [22] =  { 0x88, 0x88 },
        [23] =  { 0xdd, 0xdd },
        [24] =  { 0x99, 0x99 },
    };

    static uint8_t zeros[14];

    if (op != BLE_ATT_ACCESS_OP_READ) {
        return -1;
    }

    TEST_ASSERT_FATAL(attr_handle >= 1 &&
                      attr_handle <= BLE_ATT_SVR_TEST_LAST_ATTR);

    src = &vals[attr_handle][0];
    if (memcmp(src + 2, zeros, 14) == 0) {
        rc = os_mbuf_append(*om, src, 2);
    } else {
        rc = os_mbuf_append(*om, src, 16);
    }
    if (rc != 0) {
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    return 0;
}

static void
ble_att_svr_test_misc_register_uuid(const ble_uuid_t *uuid, uint8_t flags,
                                       uint16_t expected_handle,
                                       ble_att_svr_access_fn *fn)
{
    uint16_t handle;
    int rc;

    rc = ble_att_svr_register(uuid, flags, 0, &handle, fn, NULL);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(handle == expected_handle);
}

static void
ble_att_svr_test_misc_register_group_attrs(void)
{
    /* Service 0x1122 from 1 to 5 */
    /* Service 0x2233 from 6 to 10 */
    /* Service 010203...0f from 11 to 24 */

    static const ble_uuid16_t uuid_svc =
        BLE_UUID16_INIT(BLE_ATT_UUID_PRIMARY_SERVICE);
    static const ble_uuid16_t uuid_inc =
        BLE_UUID16_INIT(BLE_ATT_UUID_INCLUDE);
    static const ble_uuid16_t uuid_chr =
        BLE_UUID16_INIT(BLE_ATT_UUID_CHARACTERISTIC);
    static ble_uuid16_t uuids[24];

    int i;

    /* Service 0x1122 from 1 to 5 */
    ble_att_svr_test_misc_register_uuid(&uuid_svc.u, HA_FLAG_PERM_RW, 1,
                                        ble_att_svr_test_misc_attr_fn_r_group);
    for (i = 2; i <= 5; i++) {
        if ((i - 2) % 2 == 0) {
            ble_att_svr_test_misc_register_uuid(&uuid_chr.u, HA_FLAG_PERM_RW, i,
                                                ble_att_svr_test_misc_attr_fn_r_group);
        } else {
            uuids[i] = *BLE_UUID16(BLE_UUID16_DECLARE(i));
            ble_att_svr_test_misc_register_uuid(&uuids[i].u, HA_FLAG_PERM_RW, i,
                ble_att_svr_test_misc_attr_fn_r_group);
        }
    }

    /* Service 0x2233 from 6 to 10 */
    ble_att_svr_test_misc_register_uuid(&uuid_svc.u, HA_FLAG_PERM_RW, 6,
                                        ble_att_svr_test_misc_attr_fn_r_group);
    for (i = 7; i <= 10; i++) {
        ble_att_svr_test_misc_register_uuid(&uuid_inc.u, HA_FLAG_PERM_RW, i,
                                            ble_att_svr_test_misc_attr_fn_r_group);
    }

    /* Service 010203...0f from 11 to 24 */
    ble_att_svr_test_misc_register_uuid(&uuid_svc.u, HA_FLAG_PERM_RW, 11,
                                        ble_att_svr_test_misc_attr_fn_r_group);
    for (i = 12; i <= 24; i++) {
        if ((i - 12) % 2 == 0) {
            ble_att_svr_test_misc_register_uuid(&uuid_chr.u, HA_FLAG_PERM_RW, i,
                                                ble_att_svr_test_misc_attr_fn_r_group);
        } else {
            uuids[i] = *BLE_UUID16(BLE_UUID16_DECLARE(i));
            ble_att_svr_test_misc_register_uuid(&uuids[i].u, HA_FLAG_PERM_RW, i,
                ble_att_svr_test_misc_attr_fn_r_group);
        }
    }
}

static int
ble_att_svr_test_misc_attr_fn_rw_1(uint16_t conn_handle, uint16_t attr_handle,
                                   uint8_t op, uint16_t offset,
                                   struct os_mbuf **om, void *arg)
{
    int rc;

    switch (op) {
    case BLE_ATT_ACCESS_OP_READ:
        if (offset > ble_att_svr_test_attr_w_1_len) {
            return BLE_ATT_ERR_INVALID_OFFSET;
        }

        rc = os_mbuf_append(*om, ble_att_svr_test_attr_w_1 + offset,
                            ble_att_svr_test_attr_w_1_len - offset);
        TEST_ASSERT_FATAL(rc == 0);
        return 0;

    case BLE_ATT_ACCESS_OP_WRITE:
        rc = os_mbuf_copydata(*om, 0, OS_MBUF_PKTLEN(*om),
                              ble_att_svr_test_attr_w_1);
        TEST_ASSERT_FATAL(rc == 0);
        ble_att_svr_test_attr_w_1_len = OS_MBUF_PKTLEN(*om);
        return 0;

    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

static int
ble_att_svr_test_misc_attr_fn_w_1(uint16_t conn_handle, uint16_t attr_handle,
                                  uint8_t op, uint16_t offset,
                                  struct os_mbuf **om, void *arg)
{
    int rc;

    switch (op) {
    case BLE_ATT_ACCESS_OP_WRITE:
        rc = os_mbuf_copydata(*om, 0, OS_MBUF_PKTLEN(*om),
                              ble_att_svr_test_attr_w_1);
        TEST_ASSERT_FATAL(rc == 0);
        ble_att_svr_test_attr_w_1_len = OS_MBUF_PKTLEN(*om);
        return 0;

    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

static int
ble_att_svr_test_misc_attr_fn_w_2(uint16_t conn_handle, uint16_t attr_handle,
                                  uint8_t op, uint16_t offset,
                                  struct os_mbuf **om, void *arg)
{
    int rc;

    switch (op) {
    case BLE_ATT_ACCESS_OP_WRITE:
        rc = os_mbuf_copydata(*om, 0, OS_MBUF_PKTLEN(*om),
                              ble_att_svr_test_attr_w_2);
        TEST_ASSERT_FATAL(rc == 0);
        ble_att_svr_test_attr_w_2_len = OS_MBUF_PKTLEN(*om);
        return 0;

    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

static int
ble_att_svr_test_misc_attr_fn_w_fail(uint16_t conn_handle,
                                     uint16_t attr_handle,
                                     uint8_t op, uint16_t offset,
                                     struct os_mbuf **om, void *arg)
{
    return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
}

static void
ble_att_svr_test_misc_verify_w_1(void *data, int data_len)
{
    TEST_ASSERT(ble_att_svr_test_attr_w_1_len == data_len);
    TEST_ASSERT(memcmp(ble_att_svr_test_attr_w_1, data, data_len) == 0);
}

static void
ble_att_svr_test_misc_verify_w_2(void *data, int data_len)
{
    TEST_ASSERT(ble_att_svr_test_attr_w_2_len == data_len);
    TEST_ASSERT(memcmp(ble_att_svr_test_attr_w_2, data, data_len) == 0);
}

static void
ble_att_svr_test_misc_rx_read_mult_req(uint16_t conn_handle,
                                       uint16_t *handles, int num_handles,
                                       int success)
{
    int rc;

    rc = ble_hs_test_util_rx_att_read_mult_req(conn_handle, handles,
                                               num_handles);
    if (success) {
        TEST_ASSERT(rc == 0);
    } else {
        TEST_ASSERT(rc != 0);
    }
}

static void
ble_att_svr_test_misc_verify_tx_read_mult_rsp(
    uint16_t conn_handle, struct ble_hs_test_util_flat_attr *attrs,
    int num_attrs)
{
    struct ble_l2cap_chan *chan;
    struct os_mbuf *om;
    uint16_t attr_len;
    uint16_t mtu;
    uint8_t u8;
    int rc;
    int off;
    int i;

    om = ble_hs_test_util_prev_tx_dequeue();

    rc = os_mbuf_copydata(om, 0, 1, &u8);
    TEST_ASSERT(rc == 0);
    TEST_ASSERT(u8 == BLE_ATT_OP_READ_MULT_RSP);

    ble_hs_lock();

    rc = ble_hs_misc_conn_chan_find(conn_handle, BLE_L2CAP_CID_ATT,
                                    NULL, &chan);
    TEST_ASSERT_FATAL(rc == 0);
    mtu = ble_att_chan_mtu(chan);

    ble_hs_unlock();

    off = 1;
    for (i = 0; i < num_attrs; i++) {
        attr_len = min(attrs[i].value_len, mtu - off);

        rc = os_mbuf_cmpf(om, off, attrs[i].value, attr_len);
        TEST_ASSERT(rc == 0);

        off += attr_len;
    }

    TEST_ASSERT(OS_MBUF_PKTLEN(om) == off);
}

static void
ble_att_svr_test_misc_verify_all_read_mult(
    uint16_t conn_handle, struct ble_hs_test_util_flat_attr *attrs,
    int num_attrs)
{
    uint16_t handles[256];
    int i;

    TEST_ASSERT_FATAL(num_attrs <= sizeof handles / sizeof handles[0]);

    for (i = 0; i < num_attrs; i++) {
        handles[i] = attrs[i].handle;
    }

    ble_att_svr_test_misc_rx_read_mult_req(conn_handle, handles, num_attrs, 1);
    ble_att_svr_test_misc_verify_tx_read_mult_rsp(conn_handle,
                                                  attrs, num_attrs);
}

static void
ble_att_svr_test_misc_verify_tx_mtu_rsp(uint16_t conn_handle)
{
    struct ble_l2cap_chan *chan;
    struct ble_hs_conn *conn;
    uint16_t my_mtu;
    int rc;

    ble_hs_lock();

    rc = ble_att_conn_chan_find(conn_handle, &conn, &chan);
    assert(rc == 0);
    my_mtu = chan->my_mtu;

    ble_hs_unlock();

    ble_hs_test_util_verify_tx_mtu_cmd(0, my_mtu);
}

struct ble_att_svr_test_type_value_entry {
    uint16_t first;        /* 0 on last entry */
    uint16_t last;
};

static void
ble_att_svr_test_misc_verify_tx_find_type_value_rsp(
    struct ble_att_svr_test_type_value_entry *entries)
{
    struct ble_att_svr_test_type_value_entry *entry;
    struct os_mbuf *om;
    uint16_t u16;
    uint8_t op;
    int off;
    int rc;

    off = 0;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();

    rc = os_mbuf_copydata(om, off, 1, &op);
    TEST_ASSERT(rc == 0);
    off += 1;

    TEST_ASSERT(op == BLE_ATT_OP_FIND_TYPE_VALUE_RSP);

    for (entry = entries; entry->first != 0; entry++) {
        rc = os_mbuf_copydata(om, off, 2, &u16);
        TEST_ASSERT(rc == 0);
        put_le16(&u16, u16);
        TEST_ASSERT(u16 == entry->first);
        off += 2;

        rc = os_mbuf_copydata(om, off, 2, &u16);
        TEST_ASSERT(rc == 0);
        put_le16(&u16, u16);
        TEST_ASSERT(u16 == entry->last);
        off += 2;
    }

    /* Ensure there is no extra data in the response. */
    TEST_ASSERT(off == OS_MBUF_PKTHDR(om)->omp_len);
}

/** Returns the number of entries successfully verified. */

struct ble_att_svr_test_type_entry {
    uint16_t handle;  /* 0 on last entry */
    void *value;
    int value_len;
};

/** Returns the number of entries successfully verified. */
static void
ble_att_svr_test_misc_verify_tx_read_type_rsp(
    struct ble_att_svr_test_type_entry *entries)
{
    struct ble_att_svr_test_type_entry *entry;
    struct ble_att_read_type_rsp rsp;
    struct os_mbuf *om;
    uint16_t handle;
    uint8_t buf[512];
    int off;
    int rc;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT(om);

    ble_att_read_type_rsp_parse(om->om_data, om->om_len, &rsp);

    off = BLE_ATT_READ_TYPE_RSP_BASE_SZ;
    for (entry = entries; entry->handle != 0; entry++) {
        TEST_ASSERT_FATAL(rsp.batp_length ==
                          BLE_ATT_READ_TYPE_ADATA_BASE_SZ + entry->value_len);

        rc = os_mbuf_copydata(om, off, 2, &handle);
        TEST_ASSERT(rc == 0);
        handle = get_le16(&handle);
        TEST_ASSERT(handle == entry->handle);
        off += 2;

        rc = os_mbuf_copydata(om, off, entry->value_len, buf);
        TEST_ASSERT(rc == 0);
        TEST_ASSERT(memcmp(entry->value, buf, entry->value_len) == 0);
        off += entry->value_len;
    }

    /* Ensure there is no extra data in the response. */
    TEST_ASSERT(off == OS_MBUF_PKTLEN(om));
}

static void
ble_att_svr_test_misc_verify_tx_prep_write_rsp(uint16_t attr_handle,
                                               uint16_t offset,
                                               void *data, int data_len)
{
    struct ble_att_prep_write_cmd rsp;
    struct os_mbuf *om;
    uint8_t buf[1024];
    int rc;

    om = ble_hs_test_util_prev_tx_dequeue();

    rc = os_mbuf_copydata(om, 0, OS_MBUF_PKTLEN(om), buf);
    TEST_ASSERT_FATAL(rc == 0);

    ble_att_prep_write_rsp_parse(buf, sizeof buf, &rsp);

    TEST_ASSERT(rsp.bapc_handle == attr_handle);
    TEST_ASSERT(rsp.bapc_offset == offset);
    TEST_ASSERT(memcmp(buf + BLE_ATT_PREP_WRITE_CMD_BASE_SZ, data,
                       data_len) == 0);

    TEST_ASSERT(OS_MBUF_PKTLEN(om) ==
                BLE_ATT_PREP_WRITE_CMD_BASE_SZ + data_len);
}

static void
ble_att_svr_test_misc_verify_tx_exec_write_rsp(void)
{
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT(om);

    ble_att_exec_write_rsp_parse(om->om_data, om->om_len);
}

static void
ble_att_svr_test_misc_mtu_exchange(uint16_t my_mtu, uint16_t peer_sent,
                                   uint16_t peer_actual, uint16_t chan_mtu)
{
    struct ble_att_mtu_cmd req;
    struct ble_l2cap_chan *chan;
    struct ble_hs_conn *conn;
    uint16_t conn_handle;
    uint8_t buf[BLE_ATT_MTU_CMD_SZ];
    int rc;

    conn_handle = ble_att_svr_test_misc_init(my_mtu);

    req.bamc_mtu = peer_sent;
    ble_att_mtu_req_write(buf, sizeof buf, &req);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    TEST_ASSERT(rc == 0);

    ble_att_svr_test_misc_verify_tx_mtu_rsp(conn_handle);

    ble_hs_lock();
    rc = ble_hs_misc_conn_chan_find(conn_handle, BLE_L2CAP_CID_ATT,
                                    &conn, &chan);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(chan->peer_mtu == peer_actual);
    TEST_ASSERT(ble_att_chan_mtu(chan) == chan_mtu);
    ble_hs_unlock();

}

static void
ble_att_svr_test_misc_prep_write(uint16_t conn_handle, uint16_t attr_handle,
                                 uint16_t offset, void *data,
                                 int data_len, uint8_t error_code)
{
    int rc;

    rc = ble_hs_test_util_rx_att_prep_write_req(conn_handle, attr_handle,
                                                offset, data, data_len);
    if (error_code == 0) {
        TEST_ASSERT(rc == 0);
        ble_att_svr_test_misc_verify_tx_prep_write_rsp(attr_handle, offset,
                                                       data, data_len);
    } else {
        TEST_ASSERT(rc != 0);
        ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_PREP_WRITE_REQ,
                                                attr_handle, error_code);
    }
}

static void
ble_att_svr_test_misc_exec_write(uint16_t conn_handle, uint8_t flags,
                                 uint8_t error_code, uint16_t error_handle)
{
    int rc;

    rc = ble_hs_test_util_rx_att_exec_write_req(conn_handle, flags);
    if (error_code == 0) {
        TEST_ASSERT(rc == 0);
        ble_att_svr_test_misc_verify_tx_exec_write_rsp();
    } else {
        TEST_ASSERT(rc != 0);
        ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_EXEC_WRITE_REQ,
                                                error_handle, error_code);
    }
}

static void
ble_att_svr_test_misc_rx_notify(uint16_t conn_handle, uint16_t attr_handle,
                                void *attr_val, int attr_len, int good)
{
    struct ble_att_notify_req req;
    uint8_t buf[1024];
    int off;
    int rc;

    req.banq_handle = attr_handle;
    ble_att_notify_req_write(buf, sizeof buf, &req);
    off = BLE_ATT_NOTIFY_REQ_BASE_SZ;

    memcpy(buf + off, attr_val, attr_len);
    off += attr_len;

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    if (good) {
        TEST_ASSERT(rc == 0);
    } else {
        TEST_ASSERT(rc == BLE_HS_EBADDATA);
    }
}

static void
ble_att_svr_test_misc_verify_notify(uint16_t conn_handle, uint16_t attr_handle,
                                    void *attr_val, int attr_len, int good)
{
    ble_att_svr_test_n_conn_handle = 0xffff;
    ble_att_svr_test_n_attr_handle = 0;
    ble_att_svr_test_attr_n_len = 0;

    ble_att_svr_test_misc_rx_notify(conn_handle, attr_handle, attr_val,
                                    attr_len, good);

    if (good) {
        TEST_ASSERT(ble_att_svr_test_n_conn_handle == conn_handle);
        TEST_ASSERT(ble_att_svr_test_n_attr_handle == attr_handle);
        TEST_ASSERT(ble_att_svr_test_attr_n_len == attr_len);
        TEST_ASSERT(memcmp(ble_att_svr_test_attr_n, attr_val, attr_len) == 0);
    } else {
        TEST_ASSERT(ble_att_svr_test_n_conn_handle == 0xffff);
        TEST_ASSERT(ble_att_svr_test_n_attr_handle == 0);
        TEST_ASSERT(ble_att_svr_test_attr_n_len == 0);
    }
}

static void
ble_att_svr_test_misc_verify_tx_indicate_rsp(void)
{
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT(om);

    ble_att_indicate_rsp_parse(om->om_data, om->om_len);
}

static void
ble_att_svr_test_misc_rx_indicate(uint16_t conn_handle, uint16_t attr_handle,
                                  void *attr_val, int attr_len, int good)
{
    int rc;

    rc = ble_hs_test_util_rx_att_indicate_req(conn_handle, attr_handle,
                                              attr_val, attr_len);
    if (good) {
        TEST_ASSERT(rc == 0);
    } else {
        TEST_ASSERT(rc == BLE_HS_EBADDATA);
    }
}

static void
ble_att_svr_test_misc_verify_indicate(uint16_t conn_handle,
                                      uint16_t attr_handle,
                                      void *attr_val, int attr_len, int good)
{
    ble_att_svr_test_n_conn_handle = 0xffff;
    ble_att_svr_test_n_attr_handle = 0;
    ble_att_svr_test_attr_n_len = 0;

    ble_att_svr_test_misc_rx_indicate(conn_handle, attr_handle, attr_val,
                                      attr_len, good);

    if (good) {
        TEST_ASSERT(ble_att_svr_test_n_conn_handle == conn_handle);
        TEST_ASSERT(ble_att_svr_test_n_attr_handle == attr_handle);
        TEST_ASSERT(ble_att_svr_test_attr_n_len == attr_len);
        TEST_ASSERT(memcmp(ble_att_svr_test_attr_n, attr_val, attr_len) == 0);
        ble_att_svr_test_misc_verify_tx_indicate_rsp();
    } else {
        TEST_ASSERT(ble_att_svr_test_n_conn_handle == 0xffff);
        TEST_ASSERT(ble_att_svr_test_n_attr_handle == 0);
        TEST_ASSERT(ble_att_svr_test_attr_n_len == 0);
        TEST_ASSERT(ble_hs_test_util_prev_tx_queue_sz() == 0);
    }
}

TEST_CASE_SELF(ble_att_svr_test_mtu)
{
    /*** MTU too low; should pretend peer sent default value instead. */
    ble_att_svr_test_misc_mtu_exchange(BLE_ATT_MTU_DFLT, 5,
                                       BLE_ATT_MTU_DFLT, BLE_ATT_MTU_DFLT);

    /*** MTUs equal. */
    ble_att_svr_test_misc_mtu_exchange(50, 50, 50, 50);

    /*** Peer's higher than mine. */
    ble_att_svr_test_misc_mtu_exchange(50, 100, 100, 50);

    /*** Mine higher than peer's. */
    ble_att_svr_test_misc_mtu_exchange(100, 50, 50, 50);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_read)
{
    struct ble_hs_conn *conn;
    struct os_mbuf *om;
    uint16_t attr_handle;
    uint16_t conn_handle;
    const ble_uuid_t *uuid_sec = BLE_UUID128_DECLARE( \
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    const ble_uuid_t *uuid_bad = BLE_UUID128_DECLARE( \
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    const ble_uuid_t *uuid = BLE_UUID128_DECLARE( \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, );
    int rc;

    conn_handle = ble_att_svr_test_misc_init(0);

    /*** Nonexistent attribute. */
    attr_handle = 0;
    rc = ble_hs_test_util_rx_att_read_req(conn_handle, attr_handle);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_READ_REQ, 0,
                                       BLE_ATT_ERR_INVALID_HANDLE);

    /*** Application error. */
    rc = ble_att_svr_register(uuid_bad, HA_FLAG_PERM_RW, 0, &attr_handle,
                              ble_att_svr_test_misc_attr_fn_r_err, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_read_req(conn_handle, attr_handle);
    TEST_ASSERT(rc == BLE_HS_EAPP);
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_READ_REQ, attr_handle,
                                       BLE_ATT_ERR_UNLIKELY);

    /*** Successful read. */
    ble_att_svr_test_attr_r_1 = (uint8_t[]){0,1,2,3,4,5,6,7};
    ble_att_svr_test_attr_r_1_len = 8;
    rc = ble_att_svr_register(uuid, HA_FLAG_PERM_RW, 0, &attr_handle,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_read_req(conn_handle, attr_handle);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_rsp(
        ble_att_svr_test_attr_r_1, ble_att_svr_test_attr_r_1_len);

    /*** Partial read. */
    ble_att_svr_test_attr_r_1 =
        (uint8_t[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
                    22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
    ble_att_svr_test_attr_r_1_len = 40;

    rc = ble_hs_test_util_rx_att_read_req(conn_handle, attr_handle);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_rsp(ble_att_svr_test_attr_r_1,
                                        BLE_ATT_MTU_DFLT - 1);

    /*** Read requires encryption. */
    /* Insufficient authentication. */
    rc = ble_att_svr_register(uuid_sec, BLE_ATT_F_READ | BLE_ATT_F_READ_ENC, 0,
                              &attr_handle,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_read_req(conn_handle, attr_handle);
    TEST_ASSERT(rc == BLE_HS_ATT_ERR(BLE_ATT_ERR_INSUFFICIENT_AUTHEN));
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_READ_REQ, attr_handle,
                                       BLE_ATT_ERR_INSUFFICIENT_AUTHEN);

    /* Security check bypassed for local reads. */
    rc = ble_att_svr_read_local(attr_handle, &om);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(OS_MBUF_PKTLEN(om) == ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(os_mbuf_cmpf(om, 0, ble_att_svr_test_attr_r_1,
                               ble_att_svr_test_attr_r_1_len) == 0);
    rc = os_mbuf_free_chain(om);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure no response got sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /* Encrypt link; success. */
    ble_hs_lock();
    conn = ble_hs_conn_find(conn_handle);
    conn->bhc_sec_state.encrypted = 1;
    ble_hs_unlock();

    rc = ble_hs_test_util_rx_att_read_req(conn_handle, attr_handle);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_rsp(ble_att_svr_test_attr_r_1,
                                        BLE_ATT_MTU_DFLT - 1);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_read_blob)
{
    uint16_t attr_handle;
    uint16_t conn_handle;
    const ble_uuid_t *uuid = BLE_UUID128_DECLARE( \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    int rc;

    conn_handle = ble_att_svr_test_misc_init(0);

    /*** Nonexistent attribute. */
    rc = ble_hs_test_util_rx_att_read_blob_req(conn_handle, 0, 0);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_READ_BLOB_REQ, 0,
                                       BLE_ATT_ERR_INVALID_HANDLE);

    /*** Successful partial read. */
    ble_att_svr_test_attr_r_1 =
        (uint8_t[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
                    22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
    ble_att_svr_test_attr_r_1_len = 40;
    rc = ble_att_svr_register(uuid, HA_FLAG_PERM_RW, 0, &attr_handle,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_read_blob_req(conn_handle, attr_handle, 0);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_blob_rsp(ble_att_svr_test_attr_r_1,
                                                  BLE_ATT_MTU_DFLT - 1);

    /*** Read remainder of attribute. */
    rc = ble_hs_test_util_rx_att_read_blob_req(conn_handle, attr_handle,
                                           BLE_ATT_MTU_DFLT - 1);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_blob_rsp(
        ble_att_svr_test_attr_r_1 + BLE_ATT_MTU_DFLT - 1,
        40 - (BLE_ATT_MTU_DFLT - 1));

    /*** Zero-length read. */
    rc = ble_hs_test_util_rx_att_read_blob_req(conn_handle, attr_handle,
                                           ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_blob_rsp(ble_att_svr_test_attr_r_1,
                                                  0);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_read_mult)
{
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_svr_test_misc_init(0);

    struct ble_hs_test_util_flat_attr attrs[2] = {
        {
            .handle = 0,
            .offset = 0,
            .value = { 1, 2, 3, 4 },
            .value_len = 4,
        },
        {
            .handle = 0,
            .offset = 0,
            .value = { 2, 3, 4, 5, 6 },
            .value_len = 5,
        },
    };

    ble_att_svr_test_attr_r_1 = attrs[0].value;
    ble_att_svr_test_attr_r_1_len = attrs[0].value_len;
    ble_att_svr_test_attr_r_2 = attrs[1].value;
    ble_att_svr_test_attr_r_2_len = attrs[1].value_len;

    rc = ble_att_svr_register(BLE_UUID16_DECLARE(0x1111), HA_FLAG_PERM_RW, 0,
                              &attrs[0].handle,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_att_svr_register(BLE_UUID16_DECLARE(0x2222), HA_FLAG_PERM_RW, 0,
                              &attrs[1].handle,
                              ble_att_svr_test_misc_attr_fn_r_2, NULL);
    TEST_ASSERT(rc == 0);

    /*** Single nonexistent attribute. */
    ble_att_svr_test_misc_rx_read_mult_req(
        conn_handle, ((uint16_t[]){ 100 }), 1, 0);
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_READ_MULT_REQ,
                                            100, BLE_ATT_ERR_INVALID_HANDLE);

    /*** Single attribute. */
    ble_att_svr_test_misc_verify_all_read_mult(conn_handle, &attrs[0], 1);

    /*** Two attributes. */
    ble_att_svr_test_misc_verify_all_read_mult(conn_handle, attrs, 2);

    /*** Reverse order. */
    ble_att_svr_test_misc_verify_all_read_mult(conn_handle, attrs, 2);

    /*** Second attribute nonexistent; verify only error txed. */
    ble_att_svr_test_misc_rx_read_mult_req(
        conn_handle, ((uint16_t[]){ attrs[0].handle, 100 }), 2, 0);
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_READ_MULT_REQ,
                                            100, BLE_ATT_ERR_INVALID_HANDLE);

    /*** Response too long; verify only MTU bytes sent. */
    attrs[0].value_len = 20;
    memcpy(attrs[0].value,
           ((uint8_t[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19}),
           attrs[0].value_len);
    ble_att_svr_test_attr_r_1_len = attrs[0].value_len;

    attrs[1].value_len = 20;
    memcpy(attrs[1].value,
           ((uint8_t[]){
                22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39
           }),
           attrs[1].value_len);
    ble_att_svr_test_attr_r_2_len = attrs[1].value_len;

    ble_att_svr_test_misc_verify_all_read_mult(conn_handle, attrs, 2);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_write)
{
    struct ble_hs_conn *conn;
    uint16_t conn_handle;
    uint16_t attr_handle;
    const ble_uuid_t *uuid_sec = BLE_UUID128_DECLARE( \
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    const ble_uuid_t *uuid_rw = BLE_UUID128_DECLARE( \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    const ble_uuid_t *uuid_r = BLE_UUID128_DECLARE( \
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    int rc;

    static const uint8_t attr_val[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

    conn_handle = ble_att_svr_test_misc_init(0);

    /*** Nonexistent attribute. */
    rc = ble_hs_test_util_rx_att_write_req(conn_handle, 0,
                                           attr_val, sizeof attr_val);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_WRITE_REQ, 0, BLE_ATT_ERR_INVALID_HANDLE);

    /*** Write not permitted if non-local. */
    /* Non-local write (fail). */
    rc = ble_att_svr_register(uuid_r, BLE_ATT_F_READ, 0, &attr_handle,
                              ble_att_svr_test_misc_attr_fn_w_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_write_req(conn_handle, attr_handle,
                                           attr_val, sizeof attr_val);
    TEST_ASSERT(rc == BLE_HS_EREJECT);
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_WRITE_REQ,
                                       attr_handle,
                                       BLE_ATT_ERR_WRITE_NOT_PERMITTED);

    /* Local write (success). */
    rc = ble_hs_test_util_write_local_flat(attr_handle,
                                           attr_val, sizeof attr_val);
    TEST_ASSERT(rc == 0);

    /* Ensure no response got sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /*** Successful write. */
    rc = ble_att_svr_register(uuid_rw, HA_FLAG_PERM_RW, 0, &attr_handle,
                              ble_att_svr_test_misc_attr_fn_w_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_write_req(conn_handle, attr_handle,
                                           attr_val, sizeof attr_val);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_write_rsp();

    /*** Write requires encryption. */
    /* Insufficient authentication. */
    rc = ble_att_svr_register(uuid_sec, BLE_ATT_F_WRITE | BLE_ATT_F_WRITE_ENC,
                              0, &attr_handle,
                              ble_att_svr_test_misc_attr_fn_w_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_write_req(conn_handle, attr_handle,
                                           attr_val, sizeof attr_val);
    TEST_ASSERT(rc == BLE_HS_ATT_ERR(BLE_ATT_ERR_INSUFFICIENT_AUTHEN));
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_WRITE_REQ,
                                       attr_handle,
                                       BLE_ATT_ERR_INSUFFICIENT_AUTHEN);

    /* Security check bypassed for local writes. */
    rc = ble_hs_test_util_write_local_flat(attr_handle,
                                           attr_val, sizeof attr_val);
    TEST_ASSERT(rc == 0);

    /* Ensure no response got sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /* Encrypt link; success. */
    ble_hs_lock();
    conn = ble_hs_conn_find(conn_handle);
    conn->bhc_sec_state.encrypted = 1;
    ble_hs_unlock();

    rc = ble_hs_test_util_rx_att_write_req(conn_handle, attr_handle,
                                           attr_val, sizeof attr_val);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_write_rsp();

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_find_info)
{
    uint16_t conn_handle;
    uint16_t handle1;
    uint16_t handle2;
    uint16_t handle3;
    const ble_uuid_t *uuid1 =
        BLE_UUID128_DECLARE(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ,11, 12, 13, 14, 15);
    const ble_uuid_t *uuid2 =
        BLE_UUID128_DECLARE(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    const ble_uuid_t *uuid3 = BLE_UUID16_DECLARE(0x000f);
    int rc;

    conn_handle = ble_att_svr_test_misc_init(128);

    /*** Start handle of 0. */
    rc = ble_hs_test_util_rx_att_find_info_req(conn_handle, 0, 0);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_FIND_INFO_REQ, 0, BLE_ATT_ERR_INVALID_HANDLE);

    /*** Start handle > end handle. */
    rc = ble_hs_test_util_rx_att_find_info_req(conn_handle, 101, 100);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_FIND_INFO_REQ, 101, BLE_ATT_ERR_INVALID_HANDLE);

    /*** No attributes. */
    rc = ble_hs_test_util_rx_att_find_info_req(conn_handle, 200, 300);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_FIND_INFO_REQ, 200, BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** Range too late. */
    rc = ble_att_svr_register(uuid1, HA_FLAG_PERM_RW, 0, &handle1,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_find_info_req(conn_handle, 200, 300);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_FIND_INFO_REQ, 200, BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** One 128-bit entry. */
    rc = ble_hs_test_util_rx_att_find_info_req(conn_handle, handle1, handle1);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_find_info_rsp(
        ((struct ble_hs_test_util_att_info_entry[]) { {
            .handle = handle1,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        }, {
            .handle = 0,
        } }));

    /*** Two 128-bit entries. */
    rc = ble_att_svr_register(uuid2, HA_FLAG_PERM_RW, 0, &handle2,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_find_info_req(conn_handle, handle1, handle2);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_find_info_rsp(
        ((struct ble_hs_test_util_att_info_entry[]) { {
            .handle = handle1,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        }, {
            .handle = handle2,
            .uuid = BLE_UUID128_DECLARE(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
        }, {
            .handle = 0,
        } }));

    /*** Two 128-bit entries; 16-bit entry doesn't get sent. */
    rc = ble_att_svr_register(uuid3, HA_FLAG_PERM_RW, 0, &handle3,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_find_info_req(conn_handle, handle1, handle3);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_find_info_rsp(
        ((struct ble_hs_test_util_att_info_entry[]) { {
            .handle = handle1,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        }, {
            .handle = handle2,
            .uuid = BLE_UUID128_DECLARE(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
        }, {
            .handle = 0,
        } }));

    /*** Remaining 16-bit entry requested. */
    rc = ble_hs_test_util_rx_att_find_info_req(conn_handle, handle3, handle3);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_find_info_rsp(
        ((struct ble_hs_test_util_att_info_entry[]) { {
            .handle = handle3,
            .uuid = BLE_UUID16_DECLARE(0x000f),
        }, {
            .handle = 0,
        } }));

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_find_type_value)
{
    uint16_t conn_handle;
    uint16_t handle1;
    uint16_t handle2;
    uint16_t handle3;
    uint16_t handle4;
    uint16_t handle5;
    uint16_t handle_desc;
    const ble_uuid_t *uuid1 = BLE_UUID16_DECLARE(0x2800);
    const ble_uuid_t *uuid2 = BLE_UUID16_DECLARE(0x2803);
    const ble_uuid_t *uuid3 =
        BLE_UUID128_DECLARE(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    int rc;

    conn_handle = ble_att_svr_test_misc_init(128);

    ble_att_svr_test_attr_r_1 = (uint8_t[]){0x99, 0x99};
    ble_att_svr_test_attr_r_1_len = 2;

    /*** Start handle of 0. */
    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, 0, 0, 0x2800, ble_att_svr_test_attr_r_1,
        ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_FIND_TYPE_VALUE_REQ, 0,
        BLE_ATT_ERR_INVALID_HANDLE);

    /*** Start handle > end handle. */
    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, 101, 100, 0x2800, ble_att_svr_test_attr_r_1,
        ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_FIND_TYPE_VALUE_REQ, 101,
        BLE_ATT_ERR_INVALID_HANDLE);

    /*** No attributes. */
    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, 200, 300, 0x2800, ble_att_svr_test_attr_r_1,
        ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_FIND_TYPE_VALUE_REQ, 200,
        BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** Range too late. */
    rc = ble_att_svr_register(uuid1, HA_FLAG_PERM_RW, 0, &handle1,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, 200, 300, 0x2800, ble_att_svr_test_attr_r_1,
        ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_FIND_TYPE_VALUE_REQ, 200,
        BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** One entry, one attribute. */
    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, handle1, handle1, 0x2800, ble_att_svr_test_attr_r_1,
        ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_find_type_value_rsp(
        ((struct ble_att_svr_test_type_value_entry[]) { {
            .first = handle1,
            .last = handle1,
        }, {
            .first = 0,
        } }));

    /*** One entry, two attributes. */
    rc = ble_att_svr_register(uuid2, HA_FLAG_PERM_RW, 0, &handle2,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, handle1, handle2, 0x2800, ble_att_svr_test_attr_r_1,
        ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_find_type_value_rsp(
        ((struct ble_att_svr_test_type_value_entry[]) { {
            .first = handle1,
            .last = handle2,
        }, {
            .first = 0,
        } }));

    /*** Entry 1: four attributes; entry 2 (invalid value): one attribute;
     *   entry 3: one attribute; Check that invalid value is not returned. */
    ble_att_svr_test_attr_r_2 = (uint8_t[]){0x00, 0x00};
    ble_att_svr_test_attr_r_2_len = 2;

    rc = ble_att_svr_register(uuid3, HA_FLAG_PERM_RW, 0, &handle_desc,
                              ble_att_svr_test_misc_attr_fn_r_2, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_att_svr_register(uuid2, HA_FLAG_PERM_RW, 0, &handle3,
                              ble_att_svr_test_misc_attr_fn_r_2, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_att_svr_register(uuid1, HA_FLAG_PERM_RW, 0, &handle4,
                              ble_att_svr_test_misc_attr_fn_r_2, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_att_svr_register(uuid1, HA_FLAG_PERM_RW, 0, &handle5,
                              ble_att_svr_test_misc_attr_fn_r_1, NULL);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, 0x0001, 0xffff, 0x2800, ble_att_svr_test_attr_r_1,
        ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_find_type_value_rsp(
        ((struct ble_att_svr_test_type_value_entry[]) { {
            .first = handle1,
            .last = handle3,
        }, {
            .first = handle5,
            .last = handle5,
        }, {
            .first = 0,
        } }));

    /*** As above, check proper range is returned with smaller search range */
    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, 0x0001, 0x0001, 0x2800, ble_att_svr_test_attr_r_1,
        ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_find_type_value_rsp(
        ((struct ble_att_svr_test_type_value_entry[]) { {
            .first = handle1,
            .last = handle3,
        }, {
            .first = 0,
        } }));

    /*** As above, check grouping by Characteristic UUID */
    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, handle1, handle3, 0x2803, ble_att_svr_test_attr_r_1,
        ble_att_svr_test_attr_r_1_len);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_find_type_value_rsp(
        ((struct ble_att_svr_test_type_value_entry[]) { {
            .first = handle2,
            .last = handle_desc,
        }, {
            .first = 0,
        } }));

    ble_att_svr_test_assert_mbufs_freed();
}

static void
ble_att_svr_test_misc_read_type(uint16_t mtu)
{
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_svr_test_misc_init(mtu);

    /*** Start handle of 0. */
    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 0, 0,
                                                 BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_READ_TYPE_REQ, 0,
        BLE_ATT_ERR_INVALID_HANDLE);

    /*** Start handle > end handle. */
    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 101, 100,
                                                 BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_READ_TYPE_REQ, 101,
        BLE_ATT_ERR_INVALID_HANDLE);

    /*** No attributes. */
    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 1, 0xffff,
                                                 BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_READ_TYPE_REQ, 1,
        BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** Range too late. */
    ble_att_svr_test_misc_register_group_attrs();
    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 200, 300,
                                                 BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_READ_TYPE_REQ, 200,
        BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** One characteristic from one service. */
    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 1, 2,
                                                 BLE_ATT_UUID_CHARACTERISTIC);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_read_type_rsp(
        ((struct ble_att_svr_test_type_entry[]) { {
            .handle = 2,
            .value = (uint8_t[]){ 0x01, 0x11 },
            .value_len = 2,
        }, {
            .handle = 0,
        } }));

    /*** Both characteristics from one service. */
    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 1, 10,
                                                 BLE_ATT_UUID_CHARACTERISTIC);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_read_type_rsp(
        ((struct ble_att_svr_test_type_entry[]) { {
            .handle = 2,
            .value = (uint8_t[]){ 0x01, 0x11 },
            .value_len = 2,
        }, {
            .handle = 4,
            .value = (uint8_t[]){ 0x03, 0x11 },
            .value_len = 2,
        }, {
            .handle = 0,
        } }));

    /*** Ensure 16-bit and 128-bit values are retrieved separately. */
    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 11, 0xffff,
                                                 BLE_ATT_UUID_CHARACTERISTIC);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_read_type_rsp(
        ((struct ble_att_svr_test_type_entry[]) { {
            .handle = 12,
            .value = (uint8_t[]){ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
            .value_len = 16,
        }, {
            .handle = 0,
        } }));

    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 13, 0xffff,
                                                 BLE_ATT_UUID_CHARACTERISTIC);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_read_type_rsp(
        ((struct ble_att_svr_test_type_entry[]) { {
            .handle = 14,
            .value = (uint8_t[]){ 0x55, 0x55 },
            .value_len = 2,
        }, {
            .handle = 0,
        } }));

    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 15, 0xffff,
                                                 BLE_ATT_UUID_CHARACTERISTIC);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_read_type_rsp(
        ((struct ble_att_svr_test_type_entry[]) { {
            .handle = 16,
            .value = (uint8_t[]){ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
            .value_len = 16,
        }, {
            .handle = 0,
        } }));

    /*** Read until the end of the attribute list. */
    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 17, 0xffff,
                                                 BLE_ATT_UUID_CHARACTERISTIC);
    TEST_ASSERT(rc == 0);
    ble_att_svr_test_misc_verify_tx_read_type_rsp(
        ((struct ble_att_svr_test_type_entry[]) { {
            .handle = 18,
            .value = (uint8_t[]){ 0x66, 0x66 },
            .value_len = 2,
        }, {
            .handle = 20,
            .value = (uint8_t[]){ 0x77, 0x77 },
            .value_len = 2,
        }, {
            .handle = 22,
            .value = (uint8_t[]){ 0x88, 0x88 },
            .value_len = 2,
        }, {
            .handle = 24,
            .value = (uint8_t[]){ 0x99, 0x99 },
            .value_len = 2,
        }, {
            .handle = 0,
        } }));

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_read_type)
{
    ble_att_svr_test_misc_read_type(0);
    ble_att_svr_test_misc_read_type(128);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_read_group_type)
{
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_svr_test_misc_init(128);

    /*** Start handle of 0. */
    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 0, 0, BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_READ_GROUP_TYPE_REQ, 0,
        BLE_ATT_ERR_INVALID_HANDLE);

    /*** Start handle > end handle. */
    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 101, 100, BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_READ_GROUP_TYPE_REQ, 101,
        BLE_ATT_ERR_INVALID_HANDLE);

    /*** Invalid group UUID (0x1234). */
    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 110, 150, 0x1234);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_READ_GROUP_TYPE_REQ, 110,
        BLE_ATT_ERR_UNSUPPORTED_GROUP);

    /*** No attributes. */
    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 1, 0xffff, BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_READ_GROUP_TYPE_REQ, 1,
        BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** Range too late. */
    ble_att_svr_test_misc_register_group_attrs();

    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 200, 300, BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(
        BLE_ATT_OP_READ_GROUP_TYPE_REQ, 200,
        BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** One 16-bit UUID service. */
    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 1, 5, BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_group_type_rsp(
        ((struct ble_hs_test_util_att_group_type_entry[]) { {
            .start_handle = 1,
            .end_handle = 5,
            .uuid = BLE_UUID16_DECLARE(0x1122),
        }, {
            .start_handle = 0,
        } }));

    /*** Two 16-bit UUID services. */
    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 1, 10, BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_group_type_rsp(
        ((struct ble_hs_test_util_att_group_type_entry[]) { {
            .start_handle = 1,
            .end_handle = 5,
            .uuid = BLE_UUID16_DECLARE(0x1122),
        }, {
            .start_handle = 6,
            .end_handle = 10,
            .uuid = BLE_UUID16_DECLARE(0x2233),
        }, {
            .start_handle = 0,
        } }));

    /*** Two 16-bit UUID services; ensure 128-bit service not returned. */
    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 1, 100, BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_group_type_rsp(
        ((struct ble_hs_test_util_att_group_type_entry[]) { {
            .start_handle = 1,
            .end_handle = 5,
            .uuid = BLE_UUID16_DECLARE(0x1122),
        }, {
            .start_handle = 6,
            .end_handle = 10,
            .uuid = BLE_UUID16_DECLARE(0x2233),
        }, {
            .start_handle = 0,
        } }));

    /*** One 128-bit service. */
    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 11, 100, BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT(rc == 0);
    ble_hs_test_util_verify_tx_read_group_type_rsp(
        ((struct ble_hs_test_util_att_group_type_entry[]) { {
            .start_handle = 11,
            .end_handle = 0xffff,
            .uuid = BLE_UUID128_DECLARE(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
        }, {
            .start_handle = 0,
        } }));

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_prep_write)
{
    struct ble_hs_conn *conn;
    uint16_t conn_handle;
    int i;

    static uint8_t data[1024];

    conn_handle = ble_att_svr_test_misc_init(205);

    /* Initialize some attribute data. */
    for (i = 0; i < sizeof data; i++) {
        data[i] = i;
    }

    /* Register two writable attributes. */
    ble_att_svr_test_misc_register_uuid(BLE_UUID16_DECLARE(0x1234),
                                          HA_FLAG_PERM_RW, 1,
                                          ble_att_svr_test_misc_attr_fn_w_1);
    ble_att_svr_test_misc_register_uuid(BLE_UUID16_DECLARE(0x8989),
                                          HA_FLAG_PERM_RW, 2,
                                          ble_att_svr_test_misc_attr_fn_w_2);

    /* 3: not writable. */
    ble_att_svr_test_misc_register_uuid(BLE_UUID16_DECLARE(0xabab),
                                          BLE_ATT_F_READ, 3,
                                          ble_att_svr_test_misc_attr_fn_r_1);
    /* 4: Encryption required. */
    ble_att_svr_test_misc_register_uuid(
        BLE_UUID16_DECLARE(0xabac), BLE_ATT_F_WRITE | BLE_ATT_F_WRITE_ENC, 4,
        ble_att_svr_test_misc_attr_fn_w_1);

    /* 5: Encryption+authentication required. */
    ble_att_svr_test_misc_register_uuid(
        BLE_UUID16_DECLARE(0xabad),
        BLE_ATT_F_WRITE | BLE_ATT_F_WRITE_ENC | BLE_ATT_F_WRITE_AUTHEN,
        5, ble_att_svr_test_misc_attr_fn_w_1);

    /* 6: Write callback always fails. */
    ble_att_svr_test_misc_register_uuid(
        BLE_UUID16_DECLARE(0xabae), BLE_ATT_F_WRITE, 6,
        ble_att_svr_test_misc_attr_fn_w_fail);

    /*** Empty write succeeds. */
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     0, 0);

    /*** Empty cancel succeeds. */
    ble_att_svr_test_misc_exec_write(conn_handle, 0, 0, 0);

    /*** Failure for prep write to nonexistent attribute. */
    ble_att_svr_test_misc_prep_write(conn_handle, 53525, 0, data, 10,
                                     BLE_ATT_ERR_INVALID_HANDLE);

    /*** Failure due to write-not-permitted. */
    ble_att_svr_test_misc_prep_write(conn_handle, 3, 0, data, 35,
                                     BLE_ATT_ERR_WRITE_NOT_PERMITTED);

    /*** Failure due to insufficient authentication (encryption required). */
    ble_att_svr_test_misc_prep_write(conn_handle, 4, 0, data, 1,
                                     BLE_ATT_ERR_INSUFFICIENT_AUTHEN);

    /*** Encrypt connection; ensure previous prep write now succeeds. */
    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    conn->bhc_sec_state.encrypted = 1;
    ble_hs_unlock();

    ble_att_svr_test_misc_prep_write(conn_handle, 4, 0, data, 1, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, 0, 0, 0);

    /*** Failure due to insufficient authentication (not authenticated). */
    ble_att_svr_test_misc_prep_write(conn_handle, 5, 0, data, 35,
                                     BLE_ATT_ERR_INSUFFICIENT_AUTHEN);

    /*** Failure for write starting at nonzero offset. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 1, data, 10, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     BLE_ATT_ERR_INVALID_OFFSET, 1);
    ble_att_svr_test_misc_verify_w_1(NULL, 0);

    /*** Success for clear starting at nonzero offset. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 1, data, 10, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, 0, 0, 0);
    ble_att_svr_test_misc_verify_w_1(NULL, 0);

    /*** Failure for write with gap. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 0, data, 10, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 11, data, 10, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     BLE_ATT_ERR_INVALID_OFFSET, 1);
    ble_att_svr_test_misc_verify_w_1(NULL, 0);

    /*** Success for clear with gap. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 0, data, 10, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 11, data, 10, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, 0, 0, 0);
    ble_att_svr_test_misc_verify_w_1(NULL, 0);

    /*** Failure for overlong write. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 0, data, 200, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 200, data + 200, 200, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 400, data + 400, 200, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN, 1);
    ble_att_svr_test_misc_verify_w_1(NULL, 0);

    /*** Successful two part write. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 0, data, 20, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 20, data + 20, 20, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     0, 0);
    ble_att_svr_test_misc_verify_w_1(data, 40);

    /*** Successful three part write. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 0, data, 35, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 35, data + 35, 43, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 78, data + 78, 1, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     0, 0);
    ble_att_svr_test_misc_verify_w_1(data, 79);

    /*** Successful two part write to two attributes. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 0, data, 7, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 7, data + 7, 10, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 2, 0, data, 20, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 2, 20, data + 20, 10, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     0, 0);
    ble_att_svr_test_misc_verify_w_1(data, 17);
    ble_att_svr_test_misc_verify_w_2(data, 30);

    /*** Fail write to second attribute; ensure first write doesn't occur. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 0, data, 5, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 5, data + 5, 2, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 2, 0, data, 11, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 2, 12, data + 11, 19, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     BLE_ATT_ERR_INVALID_OFFSET, 2);
    ble_att_svr_test_misc_verify_w_1(data, 17);
    ble_att_svr_test_misc_verify_w_2(data, 30);

    /*** Successful out of order write to two attributes. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 0, data, 9, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 2, 0, data, 18, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 9, data + 9, 3, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 2, 18, data + 18, 43, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     0, 0);
    ble_att_svr_test_misc_verify_w_1(data, 12);
    ble_att_svr_test_misc_verify_w_2(data, 61);

    /*** Fail due to attribute callback error. */
    ble_att_svr_test_misc_prep_write(conn_handle, 6, 0, data, 35, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 6, 35, data + 35, 43, 0);
    ble_att_svr_test_misc_prep_write(conn_handle, 6, 78, data + 78, 1, 0);
    ble_att_svr_test_misc_exec_write(conn_handle, BLE_ATT_EXEC_WRITE_F_EXECUTE,
                                     BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN, 6);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_notify)
{
    uint16_t conn_handle;

    conn_handle = ble_att_svr_test_misc_init(0);

    /*** Successful notifies; verify callback is executed. */
    /* 3-length attribute. */
    ble_att_svr_test_misc_verify_notify(conn_handle, 10,
                                        (uint8_t[]) { 1, 2, 3 }, 3, 1);
    /* 1-length attribute. */
    ble_att_svr_test_misc_verify_notify(conn_handle, 1,
                                        (uint8_t[]) { 0xff }, 1, 1);
    /* 0-length attribute. */
    ble_att_svr_test_misc_verify_notify(conn_handle, 43, NULL, 0, 1);

    /*** Bad notifies; verify callback is not executed. */
    /* Attribute handle of 0. */
    ble_att_svr_test_misc_verify_notify(conn_handle, 0,
                                        (uint8_t[]) { 1, 2, 3 }, 3, 0);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_prep_write_tmo)
{
    int32_t ticks_from_now;
    uint16_t conn_handle;
    int rc;
    int i;

    static uint8_t data[1024];

    conn_handle = ble_att_svr_test_misc_init(205);

    /* Initialize some attribute data. */
    for (i = 0; i < sizeof data; i++) {
        data[i] = i;
    }

    /* Register a writable attribute. */
    ble_att_svr_test_misc_register_uuid(BLE_UUID16_DECLARE(0x1234),
                                          HA_FLAG_PERM_RW, 1,
                                          ble_att_svr_test_misc_attr_fn_w_1);

    /* Ensure timer is not set. */
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT_FATAL(ticks_from_now == BLE_HS_FOREVER);

    /* Receive a prepare write request. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 0, data, 7, 0);

    /* Ensure timer will expire in 30 seconds. */
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT(ticks_from_now == BLE_HS_ATT_SVR_QUEUED_WRITE_TMO);

    /* Almost let the timer expire. */
    os_time_advance(BLE_HS_ATT_SVR_QUEUED_WRITE_TMO - 1);
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT(ticks_from_now == 1);

    /* Receive a second prepare write request. */
    ble_att_svr_test_misc_prep_write(conn_handle, 1, 7, data + 7, 10, 0);

    /* Ensure timer got reset. */
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT(ticks_from_now == BLE_HS_ATT_SVR_QUEUED_WRITE_TMO);

    /* Allow the timer to expire. */
    ble_hs_test_util_hci_ack_set_disconnect(0);
    os_time_advance(BLE_HS_ATT_SVR_QUEUED_WRITE_TMO);
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT(ticks_from_now == BLE_HS_FOREVER);

    /* Ensure connection was terminated. */
    ble_hs_test_util_hci_verify_tx_disconnect(2, BLE_ERR_REM_USER_CONN_TERM);

    /* Free connection.  This is needed so that the prep write mbufs get
     * freed and no mbuf leak gets reported.
     */
    rc = ble_hs_atomic_conn_delete(conn_handle);
    TEST_ASSERT_FATAL(rc == 0);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_indicate)
{
    uint16_t conn_handle;

    conn_handle = ble_att_svr_test_misc_init(0);

    /*** Successful indicates; verify callback is executed. */
    /* 3-length attribute. */
    ble_att_svr_test_misc_verify_indicate(conn_handle, 10,
                                          (uint8_t[]) { 1, 2, 3 }, 3, 1);
    /* 1-length attribute. */
    ble_att_svr_test_misc_verify_indicate(conn_handle, 1,
                                          (uint8_t[]) { 0xff }, 1, 1);
    /* 0-length attribute. */
    ble_att_svr_test_misc_verify_indicate(conn_handle, 43, NULL, 0, 1);

    /*** Bad indicates; verify callback is not executed. */
    /* Attribute handle of 0. */
    ble_att_svr_test_misc_verify_indicate(conn_handle, 0,
                                          (uint8_t[]) { 1, 2, 3 }, 3, 0);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_oom)
{
    struct os_mbuf *oms;
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_svr_test_misc_init(0);

    /* Register an attribute (primary service) for incoming read commands. */
    ble_att_svr_test_misc_register_uuid(
        BLE_UUID16_DECLARE(BLE_ATT_UUID_PRIMARY_SERVICE),
        HA_FLAG_PERM_RW, 1, ble_att_svr_test_misc_attr_fn_rw_1);
    ble_att_svr_test_attr_w_1_len = 2;
    ble_att_svr_test_attr_w_1[0] = 0x12;
    ble_att_svr_test_attr_w_1[1] = 0x34;

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming request. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);

    /*** MTU; always respond affirmatively, even when no mbufs. */

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_mtu_cmd(conn_handle, 1, 100);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure we were able to send a real response. */
    ble_att_svr_test_misc_verify_tx_mtu_rsp(conn_handle);

    /*** Find information; always respond affirmatively, even when no mbufs. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_find_info_req(conn_handle, 1, 100);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure we were able to send a real response. */
    ble_hs_test_util_verify_tx_find_info_rsp(
        (struct ble_hs_test_util_att_info_entry[]) {
            { .handle = 1, .uuid = BLE_UUID16_DECLARE(BLE_ATT_UUID_PRIMARY_SERVICE) },
            { 0 },
        });

    /*** Find by type value. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_find_type_value_req(
        conn_handle, 1, 100, 0x0001, ((uint8_t[2]){0x99, 0x99}), 2);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOMEM);

    /* Ensure we were able to send an error response. */
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_FIND_TYPE_VALUE_REQ, 1,
                                       BLE_ATT_ERR_INSUFFICIENT_RES);

    /*** Read by type; always respond affirmatively, even when no mbufs. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_read_type_req16(conn_handle, 100, 0xffff,
                                                 BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOENT);

    /* Ensure we were able to send a non-OOM error response. */
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_READ_TYPE_REQ, 100,
                                       BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** Read; always respond affirmatively, even when no mbufs. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_read_req(conn_handle, 1);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure we were able to send a real response. */
    ble_hs_test_util_verify_tx_read_rsp(ble_att_svr_test_attr_w_1,
                                        ble_att_svr_test_attr_w_1_len);

    /*** Read blob; always respond affirmatively, even when no mbufs. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_read_blob_req(conn_handle, 1, 0);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure we were able to send a real response. */
    ble_hs_test_util_verify_tx_read_blob_rsp(ble_att_svr_test_attr_w_1,
                                             ble_att_svr_test_attr_w_1_len);

    /*** Read multiple. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_read_mult_req(conn_handle,
                                               ((uint16_t[2]){0x0001, 0x0002}),
                                               2);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOMEM);

    /* Ensure we were able to send an error response. */
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_READ_MULT_REQ, 0,
                                       BLE_ATT_ERR_INSUFFICIENT_RES);

    /***
     * Read by group type; always respond affirmatively, even when no
     * mbufs.
     */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_read_group_type_req16(
        conn_handle, 11, 100, BLE_ATT_UUID_PRIMARY_SERVICE);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOENT);

    /* Ensure we were able to send a non-OOM error response. */
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_READ_GROUP_TYPE_REQ, 11,
                                       BLE_ATT_ERR_ATTR_NOT_FOUND);

    /*** Write. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_write_req(conn_handle, 1,
                                           ((uint8_t[1]){1}), 1);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOMEM);

    /* Ensure we were able to send an error response. */
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_WRITE_REQ, 1,
                                       BLE_ATT_ERR_INSUFFICIENT_RES);

    /*** Write command; no response. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_write_cmd(conn_handle, 1,
                                           ((uint8_t[1]){1}), 1);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure no response sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /*** Prepare write. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_prep_write_req(conn_handle, 1, 0,
                                                ((uint8_t[1]){1}), 1);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOMEM);

    /* Ensure we were able to send an error response. */
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_PREP_WRITE_REQ, 1,
                                       BLE_ATT_ERR_INSUFFICIENT_RES);

    /*** Notify; no response. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_notify_req(conn_handle, 1,
                                            ((uint8_t[1]){1}), 1);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure no response sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /*** Indicate. */
    ble_hs_test_util_prev_tx_dequeue();

    /* Receive a request. */
    rc = ble_hs_test_util_rx_att_indicate_req(conn_handle, 1,
                                              ((uint8_t[1]){1}), 1);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOMEM);

    /* Ensure we were able to send a real response. */
    ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_INDICATE_REQ, 1,
                                       BLE_ATT_ERR_INSUFFICIENT_RES);

    rc = os_mbuf_free_chain(oms);
    TEST_ASSERT_FATAL(rc == 0);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_CASE_SELF(ble_att_svr_test_unsupported_req)
{
    uint16_t conn_handle;
    int rc;
    uint8_t buf[] = {0x3f, 0x00, 0x00, 0x01, 0x02, 0x03};

    conn_handle = ble_att_svr_test_misc_init(0);

    /* Put handle into buf */
    (*(uint16_t *)&buf[1]) = htole16(conn_handle);
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    TEST_ASSERT(rc != 0);
    ble_hs_test_util_verify_tx_err_rsp(0x3f, 0,
                                       BLE_ATT_ERR_REQ_NOT_SUPPORTED);

    /* Check for no response when unknown command is sent */
    buf[0] = 0x4f;
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                    buf, sizeof buf);
    TEST_ASSERT(rc != 0);

    /* Ensure no response sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    ble_att_svr_test_assert_mbufs_freed();
}

TEST_SUITE(ble_att_svr_suite)
{
    ble_att_svr_test_mtu();
    ble_att_svr_test_read();
    ble_att_svr_test_read_blob();
    ble_att_svr_test_read_mult();
    ble_att_svr_test_write();
    ble_att_svr_test_find_info();
    ble_att_svr_test_find_type_value();
    ble_att_svr_test_read_type();
    ble_att_svr_test_read_group_type();
    ble_att_svr_test_prep_write();
    ble_att_svr_test_prep_write_tmo();
    ble_att_svr_test_notify();
    ble_att_svr_test_indicate();
    ble_att_svr_test_oom();
    ble_att_svr_test_unsupported_req();
}
