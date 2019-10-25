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

#include <string.h>
#include <errno.h>
#include "testutil/testutil.h"
#include "nimble/ble.h"
#include "host/ble_uuid.h"
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

#define BLE_GATTS_NOTIFY_TEST_CHR_1_UUID    0x1111
#define BLE_GATTS_NOTIFY_TEST_CHR_2_UUID    0x2222

#define BLE_GATTS_NOTIFY_TEST_MAX_EVENTS    16

static uint8_t ble_gatts_notify_test_peer_addr[6] = {2,3,4,5,6,7};

static int
ble_gatts_notify_test_misc_access(uint16_t conn_handle,
                                  uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt,
                                  void *arg);
static void
ble_gatts_notify_test_misc_reg_cb(struct ble_gatt_register_ctxt *ctxt,
                                  void *arg);

static const struct ble_gatt_svc_def ble_gatts_notify_test_svcs[] = { {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(0x1234),
    .characteristics = (struct ble_gatt_chr_def[]) { {
        .uuid = BLE_UUID16_DECLARE(BLE_GATTS_NOTIFY_TEST_CHR_1_UUID),
        .access_cb = ble_gatts_notify_test_misc_access,
        .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY |
                 BLE_GATT_CHR_F_INDICATE,
    }, {
        .uuid = BLE_UUID16_DECLARE(BLE_GATTS_NOTIFY_TEST_CHR_2_UUID),
        .access_cb = ble_gatts_notify_test_misc_access,
        .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY |
                 BLE_GATT_CHR_F_INDICATE,
    }, {
        0
    } },
}, {
    0
} };

static uint16_t ble_gatts_notify_test_chr_1_def_handle;
static uint8_t ble_gatts_notify_test_chr_1_val[1024];
static int ble_gatts_notify_test_chr_1_len;
static uint16_t ble_gatts_notify_test_chr_2_def_handle;
static uint8_t ble_gatts_notify_test_chr_2_val[1024];
static int ble_gatts_notify_test_chr_2_len;

static struct ble_gap_event
ble_gatts_notify_test_events[BLE_GATTS_NOTIFY_TEST_MAX_EVENTS];

static int ble_gatts_notify_test_num_events;

typedef int ble_store_write_fn(int obj_type, const union ble_store_value *val);

typedef int ble_store_delete_fn(int obj_type, const union ble_store_key *key);

static int
ble_gatts_notify_test_util_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_NOTIFY_TX:
    case BLE_GAP_EVENT_SUBSCRIBE:
        TEST_ASSERT_FATAL(ble_gatts_notify_test_num_events <
                          BLE_GATTS_NOTIFY_TEST_MAX_EVENTS);

        ble_gatts_notify_test_events[ble_gatts_notify_test_num_events++] =
            *event;

    default:
        break;
    }

    return 0;
}

static uint16_t
ble_gatts_notify_test_misc_read_notify(uint16_t conn_handle,
                                       uint16_t chr_def_handle)
{
    struct ble_att_read_req req;
    struct os_mbuf *om;
    uint8_t buf[BLE_ATT_READ_REQ_SZ];
    uint16_t flags;
    int rc;

    req.barq_handle = chr_def_handle + 2;
    ble_att_read_req_write(buf, sizeof buf, &req);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    TEST_ASSERT(rc == 0);

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);
    TEST_ASSERT_FATAL(om->om_len == 3);
    TEST_ASSERT_FATAL(om->om_data[0] == BLE_ATT_OP_READ_RSP);

    flags = get_le16(om->om_data + 1);
    return flags;
}

static void
ble_gatts_notify_test_misc_try_enable_notify(uint16_t conn_handle,
                                             uint16_t chr_def_handle,
                                             uint16_t flags, int fail)
{
    struct ble_att_write_req req;
    uint8_t buf[BLE_ATT_WRITE_REQ_BASE_SZ + 2];
    int rc;

    req.bawq_handle = chr_def_handle + 2;
    ble_att_write_req_write(buf, sizeof buf, &req);

    put_le16(buf + BLE_ATT_WRITE_REQ_BASE_SZ, flags);
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    if (fail) {
        TEST_ASSERT_FATAL(rc != 0);
        ble_hs_test_util_verify_tx_err_rsp(BLE_ATT_OP_WRITE_REQ,
                                           req.bawq_handle,
                                           BLE_ATT_ERR_REQ_NOT_SUPPORTED);
    } else {
        TEST_ASSERT_FATAL(rc == 0);
        ble_hs_test_util_verify_tx_write_rsp();
    }
}

static void
ble_gatts_notify_test_misc_enable_notify(uint16_t conn_handle,
                                         uint16_t chr_def_handle,
                                         uint16_t flags)
{
    ble_gatts_notify_test_misc_try_enable_notify(conn_handle,
                                                 chr_def_handle,
                                                 flags, 0);
}

static void
ble_gatts_notify_test_util_next_event(struct ble_gap_event *event)
{
    TEST_ASSERT_FATAL(ble_gatts_notify_test_num_events > 0);

    *event = *ble_gatts_notify_test_events;

    ble_gatts_notify_test_num_events--;
    if (ble_gatts_notify_test_num_events > 0) {
        memmove(ble_gatts_notify_test_events + 0,
                ble_gatts_notify_test_events + 1,
                ble_gatts_notify_test_num_events * sizeof *event);
    }
}

static void
ble_gatts_notify_test_util_verify_sub_event(uint16_t conn_handle,
                                            uint8_t attr_handle,
                                            uint8_t reason,
                                            uint8_t prevn, uint8_t curn,
                                            uint8_t previ, uint8_t curi)
{
    struct ble_gap_event event;

    ble_gatts_notify_test_util_next_event(&event);

    TEST_ASSERT(event.type == BLE_GAP_EVENT_SUBSCRIBE);
    TEST_ASSERT(event.subscribe.conn_handle == conn_handle);
    TEST_ASSERT(event.subscribe.attr_handle == attr_handle);
    TEST_ASSERT(event.subscribe.reason == reason);
    TEST_ASSERT(event.subscribe.prev_notify == prevn);
    TEST_ASSERT(event.subscribe.cur_notify == curn);
    TEST_ASSERT(event.subscribe.prev_indicate == previ);
    TEST_ASSERT(event.subscribe.cur_indicate == curi);
}

static void
ble_gatts_notify_test_util_verify_tx_event(uint16_t conn_handle,
                                           uint8_t attr_handle,
                                           int status,
                                           int indication)
{
    struct ble_gap_event event;

    ble_gatts_notify_test_util_next_event(&event);

    TEST_ASSERT(event.type == BLE_GAP_EVENT_NOTIFY_TX);
    TEST_ASSERT(event.notify_tx.status == status);
    TEST_ASSERT(event.notify_tx.conn_handle == conn_handle);
    TEST_ASSERT(event.notify_tx.attr_handle == attr_handle);
    TEST_ASSERT(event.notify_tx.indication == indication);
}

static void
ble_gatts_notify_test_util_verify_ack_event(uint16_t conn_handle,
                                            uint8_t attr_handle)
{
    ble_gatts_notify_test_util_verify_tx_event(conn_handle, attr_handle,
                                               BLE_HS_EDONE, 1);
}

static void
ble_gatts_notify_test_misc_init(uint16_t *out_conn_handle, int bonding,
                                uint16_t chr1_flags, uint16_t chr2_flags)
{
    struct ble_hs_conn *conn;
    uint16_t flags;
    int exp_num_cccds;

    ble_hs_test_util_init();
    ble_gatts_notify_test_num_events = 0;

    ble_hs_test_util_reg_svcs(ble_gatts_notify_test_svcs,
                              ble_gatts_notify_test_misc_reg_cb,
                              NULL);
    TEST_ASSERT_FATAL(ble_gatts_notify_test_chr_1_def_handle != 0);
    TEST_ASSERT_FATAL(ble_gatts_notify_test_chr_2_def_handle != 0);

    ble_hs_test_util_create_conn(2, ble_gatts_notify_test_peer_addr,
                                 ble_gatts_notify_test_util_gap_event, NULL);
    *out_conn_handle = 2;

    if (bonding) {
        ble_hs_lock();
        conn = ble_hs_conn_find(2);
        TEST_ASSERT_FATAL(conn != NULL);
        conn->bhc_sec_state.encrypted = 1;
        conn->bhc_sec_state.authenticated = 1;
        conn->bhc_sec_state.bonded = 1;
        ble_hs_unlock();
    }

    /* Ensure notifications disabled on new connection. */
    flags = ble_gatts_notify_test_misc_read_notify(
        2, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == 0);
    flags = ble_gatts_notify_test_misc_read_notify(
        2, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == 0);

    /* Set initial notification / indication state and verify that subscription
     * callback gets executed.
     */
    if (chr1_flags != 0) {
        ble_gatts_notify_test_misc_enable_notify(
            2, ble_gatts_notify_test_chr_1_def_handle, chr1_flags);

        ble_gatts_notify_test_util_verify_sub_event(
            *out_conn_handle,
            ble_gatts_notify_test_chr_1_def_handle + 1,
            BLE_GAP_SUBSCRIBE_REASON_WRITE,
            0, chr1_flags == BLE_GATTS_CLT_CFG_F_NOTIFY,
            0, chr1_flags == BLE_GATTS_CLT_CFG_F_INDICATE);
    }
    if (chr2_flags != 0) {
        ble_gatts_notify_test_misc_enable_notify(
            2, ble_gatts_notify_test_chr_2_def_handle, chr2_flags);

        ble_gatts_notify_test_util_verify_sub_event(
            *out_conn_handle,
            ble_gatts_notify_test_chr_2_def_handle + 1,
            BLE_GAP_SUBSCRIBE_REASON_WRITE,
            0, chr2_flags == BLE_GATTS_CLT_CFG_F_NOTIFY,
            0, chr2_flags == BLE_GATTS_CLT_CFG_F_INDICATE);
    }

    /* Ensure no extraneous subscription callbacks were executed. */
    TEST_ASSERT(ble_gatts_notify_test_num_events == 0);

    /* Toss both write responses. */
    ble_hs_test_util_prev_tx_queue_clear();

    /* Ensure notification / indication state reads back correctly. */
    flags = ble_gatts_notify_test_misc_read_notify(
        2, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == chr1_flags);
    flags = ble_gatts_notify_test_misc_read_notify(
        2, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == chr2_flags);

    /* Ensure both CCCDs still persisted. */
    if (bonding) {
        exp_num_cccds = (chr1_flags != 0) + (chr2_flags != 0);
    } else {
        exp_num_cccds = 0;
    }
    TEST_ASSERT(ble_hs_test_util_num_cccds() == exp_num_cccds);
}

static void
ble_gatts_notify_test_disconnect(uint16_t conn_handle,
                                 uint8_t chr1_flags,
                                 uint8_t chr1_indicate_in_progress,
                                 uint8_t chr2_flags,
                                 uint8_t chr2_indicate_in_progress)
{
    ble_hs_test_util_conn_disconnect(conn_handle);

    if (chr1_indicate_in_progress) {
        ble_gatts_notify_test_util_verify_tx_event(
            conn_handle,
            ble_gatts_notify_test_chr_1_def_handle + 1,
            BLE_HS_ENOTCONN,
            1);
    }

    /* Verify subscription callback executed for each subscribed
     * characteristic.
     */
    if (chr1_flags != 0) {
        ble_gatts_notify_test_util_verify_sub_event(
            conn_handle,
            ble_gatts_notify_test_chr_1_def_handle + 1,
            BLE_GAP_SUBSCRIBE_REASON_TERM,
            chr1_flags == BLE_GATTS_CLT_CFG_F_NOTIFY, 0,
            chr1_flags == BLE_GATTS_CLT_CFG_F_INDICATE, 0);
    }

    if (chr2_indicate_in_progress) {
        ble_gatts_notify_test_util_verify_tx_event(
            conn_handle,
            ble_gatts_notify_test_chr_2_def_handle + 1,
            BLE_HS_ENOTCONN,
            1);
    }

    if (chr2_flags != 0) {
        ble_gatts_notify_test_util_verify_sub_event(
            conn_handle,
            ble_gatts_notify_test_chr_2_def_handle + 1,
            BLE_GAP_SUBSCRIBE_REASON_TERM,
            chr2_flags == BLE_GATTS_CLT_CFG_F_NOTIFY, 0,
            chr2_flags == BLE_GATTS_CLT_CFG_F_INDICATE, 0);
    }
}

static void
ble_gatts_notify_test_misc_reg_cb(struct ble_gatt_register_ctxt *ctxt,
                                  void *arg)
{
    uint16_t uuid16;

    if (ctxt->op == BLE_GATT_REGISTER_OP_CHR) {
        uuid16 = ble_uuid_u16(ctxt->chr.chr_def->uuid);
        switch (uuid16) {
        case BLE_GATTS_NOTIFY_TEST_CHR_1_UUID:
            ble_gatts_notify_test_chr_1_def_handle = ctxt->chr.def_handle;
            break;

        case BLE_GATTS_NOTIFY_TEST_CHR_2_UUID:
            ble_gatts_notify_test_chr_2_def_handle = ctxt->chr.def_handle;
            break;

        default:
            TEST_ASSERT_FATAL(0);
            break;
        }
    }
}

static int
ble_gatts_notify_test_misc_access(uint16_t conn_handle,
                                  uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt,
                                  void *arg)
{
    int rc;

    TEST_ASSERT_FATAL(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);
    TEST_ASSERT(conn_handle == 0xffff);

    if (attr_handle == ble_gatts_notify_test_chr_1_def_handle + 1) {
        TEST_ASSERT(ctxt->chr ==
                    &ble_gatts_notify_test_svcs[0].characteristics[0]);
        rc = os_mbuf_copyinto(ctxt->om, 0, ble_gatts_notify_test_chr_1_val,
                              ble_gatts_notify_test_chr_1_len);
        TEST_ASSERT_FATAL(rc == 0);
    } else if (attr_handle == ble_gatts_notify_test_chr_2_def_handle + 1) {
        TEST_ASSERT(ctxt->chr ==
                    &ble_gatts_notify_test_svcs[0].characteristics[1]);
        rc = os_mbuf_copyinto(ctxt->om, 0, ble_gatts_notify_test_chr_2_val,
                              ble_gatts_notify_test_chr_2_len);
        TEST_ASSERT_FATAL(rc == 0);
    } else {
        TEST_ASSERT_FATAL(0);
    }

    return 0;
}

static void
ble_gatts_notify_test_misc_rx_indicate_rsp(uint16_t conn_handle,
                                           uint16_t attr_handle)
{
    uint8_t buf[BLE_ATT_INDICATE_RSP_SZ];
    int rc;

    ble_att_indicate_rsp_write(buf, sizeof buf);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    TEST_ASSERT(rc == 0);

    ble_gatts_notify_test_util_verify_ack_event(conn_handle, attr_handle);
}

static void
ble_gatts_notify_test_misc_verify_tx_n(uint16_t conn_handle,
                                       uint16_t attr_handle,
                                       const uint8_t *attr_data, int attr_len)
{
    struct ble_att_notify_req req;
    struct os_mbuf *om;
    int i;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);

    ble_att_notify_req_parse(om->om_data, om->om_len, &req);
    TEST_ASSERT(req.banq_handle == attr_handle);

    for (i = 0; i < attr_len; i++) {
        TEST_ASSERT(om->om_data[BLE_ATT_NOTIFY_REQ_BASE_SZ + i] ==
                    attr_data[i]);
    }

    ble_gatts_notify_test_util_verify_tx_event(conn_handle, attr_handle, 0, 0);
}

static void
ble_gatts_notify_test_misc_verify_tx_i(uint16_t conn_handle,
                                       uint16_t attr_handle,
                                       const uint8_t *attr_data, int attr_len)
{
    struct ble_att_indicate_req req;
    struct os_mbuf *om;
    int i;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);

    ble_att_indicate_req_parse(om->om_data, om->om_len, &req);
    TEST_ASSERT(req.baiq_handle == attr_handle);

    for (i = 0; i < attr_len; i++) {
        TEST_ASSERT(om->om_data[BLE_ATT_INDICATE_REQ_BASE_SZ + i] ==
                    attr_data[i]);
    }

    ble_gatts_notify_test_util_verify_tx_event(conn_handle, attr_handle, 0, 1);
}

static void
ble_gatts_notify_test_misc_verify_tx_gen(uint16_t conn_handle, int attr_idx,
                                         uint8_t chr_flags)
{
    uint16_t attr_handle;
    uint16_t attr_len;
    void *attr_val;

    switch (attr_idx) {
    case 1:
        attr_handle = ble_gatts_notify_test_chr_1_def_handle + 1;
        attr_len = ble_gatts_notify_test_chr_1_len;
        attr_val = ble_gatts_notify_test_chr_1_val;
        break;

    case 2:
        attr_handle = ble_gatts_notify_test_chr_2_def_handle + 1;
        attr_len = ble_gatts_notify_test_chr_2_len;
        attr_val = ble_gatts_notify_test_chr_2_val;
        break;

    default:
        TEST_ASSERT_FATAL(0);
        break;
    }

    switch (chr_flags) {
    case 0:
        break;

    case BLE_GATTS_CLT_CFG_F_NOTIFY:
        ble_gatts_notify_test_misc_verify_tx_n(conn_handle, attr_handle,
                                               attr_val, attr_len);
        break;

    case BLE_GATTS_CLT_CFG_F_INDICATE:
        ble_gatts_notify_test_misc_verify_tx_i(conn_handle, attr_handle,
                                               attr_val, attr_len);
        break;

    default:
        TEST_ASSERT_FATAL(0);
        break;
    }
}

static void
ble_gatts_notify_test_restore_bonding(uint16_t conn_handle,
                                      uint8_t chr1_flags, uint8_t chr1_tx,
                                      uint8_t chr2_flags, uint8_t chr2_tx)
{
    struct ble_hs_conn *conn;

    ble_hs_lock();
    conn = ble_hs_conn_find(conn_handle);
    TEST_ASSERT_FATAL(conn != NULL);
    conn->bhc_sec_state.encrypted = 1;
    conn->bhc_sec_state.authenticated = 1;
    conn->bhc_sec_state.bonded = 1;
    ble_hs_unlock();

    ble_gatts_bonding_restored(conn_handle);

    /* Verify subscription callback executed for each subscribed
     * characteristic.
     */
    if (chr1_flags != 0) {
        ble_gatts_notify_test_util_verify_sub_event(
            conn_handle,
            ble_gatts_notify_test_chr_1_def_handle + 1,
            BLE_GAP_SUBSCRIBE_REASON_RESTORE,
            0, chr1_flags == BLE_GATTS_CLT_CFG_F_NOTIFY,
            0, chr1_flags == BLE_GATTS_CLT_CFG_F_INDICATE);

    }
    if (chr1_tx) {
        ble_gatts_notify_test_misc_verify_tx_gen(conn_handle, 1, chr1_flags);
    }

    if (chr2_flags != 0) {
        ble_gatts_notify_test_util_verify_sub_event(
            conn_handle,
            ble_gatts_notify_test_chr_2_def_handle + 1,
            BLE_GAP_SUBSCRIBE_REASON_RESTORE,
            0, chr2_flags == BLE_GATTS_CLT_CFG_F_NOTIFY,
            0, chr2_flags == BLE_GATTS_CLT_CFG_F_INDICATE);
    }
    if (chr2_tx) {
        ble_gatts_notify_test_misc_verify_tx_gen(conn_handle, 2, chr2_flags);
    }
}

TEST_CASE_SELF(ble_gatts_notify_test_n)
{
    static const uint8_t fourbytes[] = { 1, 2, 3, 4 };
    struct os_mbuf *om;
    uint16_t conn_handle;
    uint16_t flags;
    int rc;

    ble_gatts_notify_test_misc_init(&conn_handle, 0,
                                    BLE_GATTS_CLT_CFG_F_NOTIFY,
                                    BLE_GATTS_CLT_CFG_F_NOTIFY);

    /* Ensure notifications read back as enabled. */
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == BLE_GATTS_CLT_CFG_F_NOTIFY);
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == BLE_GATTS_CLT_CFG_F_NOTIFY);

    /* Verify custom notification data. */
    om = ble_hs_mbuf_from_flat(fourbytes, sizeof fourbytes);
    TEST_ASSERT_FATAL(om != NULL);

    rc = ble_gattc_notify_custom(conn_handle,
                                 ble_gatts_notify_test_chr_1_def_handle + 1,
                                 om);
    TEST_ASSERT_FATAL(rc == 0);

    ble_gatts_notify_test_misc_verify_tx_n(
        conn_handle,
        ble_gatts_notify_test_chr_1_def_handle + 1,
        fourbytes,
        sizeof fourbytes);

    /* Update characteristic 1's value. */
    ble_gatts_notify_test_chr_1_len = 1;
    ble_gatts_notify_test_chr_1_val[0] = 0xab;
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Verify notification sent properly. */
    ble_gatts_notify_test_misc_verify_tx_n(
        conn_handle,
        ble_gatts_notify_test_chr_1_def_handle + 1,
        ble_gatts_notify_test_chr_1_val,
        ble_gatts_notify_test_chr_1_len);

    /* Update characteristic 2's value. */
    ble_gatts_notify_test_chr_2_len = 16;
    memcpy(ble_gatts_notify_test_chr_2_val,
           ((uint8_t[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}), 16);
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_2_def_handle + 1);

    /* Verify notification sent properly. */
    ble_gatts_notify_test_misc_verify_tx_n(
        conn_handle,
        ble_gatts_notify_test_chr_2_def_handle + 1,
        ble_gatts_notify_test_chr_2_val,
        ble_gatts_notify_test_chr_2_len);

    /***
     * Disconnect, modify characteristic values, and reconnect.  Ensure
     * notifications are not sent and are no longer enabled.
     */

    ble_gatts_notify_test_disconnect(conn_handle,
                                     BLE_GATTS_CLT_CFG_F_NOTIFY, 0,
                                     BLE_GATTS_CLT_CFG_F_NOTIFY, 0);

    /* Update characteristic 1's value. */
    ble_gatts_notify_test_chr_1_len = 1;
    ble_gatts_notify_test_chr_1_val[0] = 0xdd;
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Update characteristic 2's value. */
    ble_gatts_notify_test_chr_2_len = 16;
    memcpy(ble_gatts_notify_test_chr_2_val,
           ((uint8_t[]){1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}), 16);
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_2_def_handle + 1);

    ble_hs_test_util_create_conn(conn_handle, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 ble_gatts_notify_test_util_gap_event, NULL);

    /* Ensure no notifications sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /* Ensure notifications disabled. */
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == 0);
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_notify_test_i)
{
    static const uint8_t fourbytes[] = { 1, 2, 3, 4 };
    struct os_mbuf *om;
    uint16_t conn_handle;
    uint16_t flags;
    int rc;

    ble_gatts_notify_test_misc_init(&conn_handle, 0,
                                    BLE_GATTS_CLT_CFG_F_INDICATE,
                                    BLE_GATTS_CLT_CFG_F_INDICATE);

    /* Verify custom indication data. */
    om = ble_hs_mbuf_from_flat(fourbytes, sizeof fourbytes);
    TEST_ASSERT_FATAL(om != NULL);

    rc = ble_gattc_indicate_custom(conn_handle,
                                   ble_gatts_notify_test_chr_1_def_handle + 1,
                                   om);
    TEST_ASSERT_FATAL(rc == 0);

    ble_gatts_notify_test_misc_verify_tx_i(
        conn_handle,
        ble_gatts_notify_test_chr_1_def_handle + 1,
        fourbytes,
        sizeof fourbytes);

    /* Receive the confirmation for the indication. */
    ble_gatts_notify_test_misc_rx_indicate_rsp(
        conn_handle,
        ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Update characteristic 1's value. */
    ble_gatts_notify_test_chr_1_len = 1;
    ble_gatts_notify_test_chr_1_val[0] = 0xab;
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Verify indication sent properly. */
    ble_gatts_notify_test_misc_verify_tx_i(
        conn_handle,
        ble_gatts_notify_test_chr_1_def_handle + 1,
        ble_gatts_notify_test_chr_1_val,
        ble_gatts_notify_test_chr_1_len);

    /* Update characteristic 2's value. */
    ble_gatts_notify_test_chr_2_len = 16;
    memcpy(ble_gatts_notify_test_chr_2_val,
           ((uint8_t[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}), 16);
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_2_def_handle + 1);

    /* Verify the second indication doesn't get sent until the first is
     * confirmed.
     */
    TEST_ASSERT(ble_hs_test_util_prev_tx_queue_sz() == 0);

    /* Receive the confirmation for the first indication. */
    ble_gatts_notify_test_misc_rx_indicate_rsp(
        conn_handle,
        ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Verify indication sent properly. */
    ble_gatts_notify_test_misc_verify_tx_i(
        conn_handle,
        ble_gatts_notify_test_chr_2_def_handle + 1,
        ble_gatts_notify_test_chr_2_val,
        ble_gatts_notify_test_chr_2_len);

    /* Receive the confirmation for the second indication. */
    ble_gatts_notify_test_misc_rx_indicate_rsp(
        conn_handle,
        ble_gatts_notify_test_chr_2_def_handle + 1);

    /* Verify no pending GATT jobs. */
    TEST_ASSERT(!ble_gattc_any_jobs());

    /***
     * Disconnect, modify characteristic values, and reconnect.  Ensure
     * indications are not sent and are no longer enabled.
     */

    ble_gatts_notify_test_disconnect(conn_handle,
                                     BLE_GATTS_CLT_CFG_F_INDICATE, 0,
                                     BLE_GATTS_CLT_CFG_F_INDICATE, 0);

    /* Update characteristic 1's value. */
    ble_gatts_notify_test_chr_1_len = 1;
    ble_gatts_notify_test_chr_1_val[0] = 0xdd;
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Update characteristic 2's value. */
    ble_gatts_notify_test_chr_2_len = 16;
    memcpy(ble_gatts_notify_test_chr_2_val,
           ((uint8_t[]){1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}), 16);
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_2_def_handle + 1);

    ble_hs_test_util_create_conn(conn_handle, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 ble_gatts_notify_test_util_gap_event, NULL);

    /* Ensure no indications sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /* Ensure indications disabled. */
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == 0);
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_notify_test_bonded_n)
{
    uint16_t conn_handle;
    uint16_t flags;

    ble_gatts_notify_test_misc_init(&conn_handle, 1,
                                    BLE_GATTS_CLT_CFG_F_NOTIFY,
                                    BLE_GATTS_CLT_CFG_F_NOTIFY);

    /* Disconnect. */
    ble_gatts_notify_test_disconnect(conn_handle,
                                     BLE_GATTS_CLT_CFG_F_NOTIFY, 0,
                                     BLE_GATTS_CLT_CFG_F_NOTIFY, 0);

    /* Ensure both CCCDs still persisted. */
    TEST_ASSERT(ble_hs_test_util_num_cccds() == 2);

    /* Update characteristic 1's value. */
    ble_gatts_notify_test_chr_1_len = 1;
    ble_gatts_notify_test_chr_1_val[0] = 0xdd;
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Update characteristic 2's value. */
    ble_gatts_notify_test_chr_2_len = 16;
    memcpy(ble_gatts_notify_test_chr_2_val,
           ((uint8_t[]){1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}), 16);
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_2_def_handle + 1);

    /* Reconnect; ensure notifications don't get sent while unbonded and that
     * notifications appear disabled.
     */

    ble_hs_test_util_create_conn(conn_handle, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 ble_gatts_notify_test_util_gap_event, NULL);

    ble_gatts_notify_test_num_events = 0;
    /* Ensure no notifications sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /* Ensure notifications disabled. */
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == 0);
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == 0);

    /* Simulate a successful encryption procedure (bonding restoration). */
    ble_gatts_notify_test_restore_bonding(conn_handle,
                                          BLE_GATTS_CLT_CFG_F_NOTIFY, 1,
                                          BLE_GATTS_CLT_CFG_F_NOTIFY, 1);

    /* Ensure notifications enabled. */
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == BLE_GATTS_CLT_CFG_F_NOTIFY);
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == BLE_GATTS_CLT_CFG_F_NOTIFY);

    /* Ensure both CCCDs still persisted. */
    TEST_ASSERT(ble_hs_test_util_num_cccds() == 2);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_notify_test_bonded_i)
{
    uint16_t conn_handle;
    uint16_t flags;

    ble_gatts_notify_test_misc_init(&conn_handle, 1,
                                    BLE_GATTS_CLT_CFG_F_INDICATE,
                                    BLE_GATTS_CLT_CFG_F_INDICATE);

    /* Disconnect. */
    ble_gatts_notify_test_disconnect(conn_handle,
                                     BLE_GATTS_CLT_CFG_F_INDICATE, 0,
                                     BLE_GATTS_CLT_CFG_F_INDICATE, 0);

    /* Ensure both CCCDs still persisted. */
    TEST_ASSERT(ble_hs_test_util_num_cccds() == 2);

    /* Update characteristic 1's value. */
    ble_gatts_notify_test_chr_1_len = 1;
    ble_gatts_notify_test_chr_1_val[0] = 0xab;
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Update characteristic 2's value. */
    ble_gatts_notify_test_chr_2_len = 16;
    memcpy(ble_gatts_notify_test_chr_2_val,
           ((uint8_t[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}), 16);
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_2_def_handle + 1);

    /* Reconnect; ensure notifications don't get sent while unbonded and that
     * notifications appear disabled.
     */

    ble_hs_test_util_create_conn(conn_handle, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 ble_gatts_notify_test_util_gap_event, NULL);

    /* Ensure no indications sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /* Ensure notifications disabled. */
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == 0);
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == 0);

    /* Simulate a successful encryption procedure (bonding restoration). */
    ble_gatts_notify_test_restore_bonding(conn_handle,
                                          BLE_GATTS_CLT_CFG_F_INDICATE, 1,
                                          BLE_GATTS_CLT_CFG_F_INDICATE, 0);

    /* Verify the second indication doesn't get sent until the first is
     * confirmed.
     */
    TEST_ASSERT(ble_hs_test_util_prev_tx_queue_sz() == 0);

    /* Receive the confirmation for the first indication. */
    ble_gatts_notify_test_misc_rx_indicate_rsp(
        conn_handle,
        ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Verify indication sent properly. */
    ble_gatts_notify_test_misc_verify_tx_i(
        conn_handle,
        ble_gatts_notify_test_chr_2_def_handle + 1,
        ble_gatts_notify_test_chr_2_val,
        ble_gatts_notify_test_chr_2_len);

    /* Receive the confirmation for the second indication. */
    ble_gatts_notify_test_misc_rx_indicate_rsp(
        conn_handle,
        ble_gatts_notify_test_chr_2_def_handle + 1);

    /* Verify no pending GATT jobs. */
    TEST_ASSERT(!ble_gattc_any_jobs());

    /* Ensure notifications enabled. */
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == BLE_GATTS_CLT_CFG_F_INDICATE);
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == BLE_GATTS_CLT_CFG_F_INDICATE);

    /* Ensure both CCCDs still persisted. */
    TEST_ASSERT(ble_hs_test_util_num_cccds() == 2);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_notify_test_bonded_i_no_ack)
{
    struct ble_store_value_cccd value_cccd;
    struct ble_store_key_cccd key_cccd;
    uint16_t conn_handle;
    uint16_t flags;
    int rc;

    ble_gatts_notify_test_misc_init(&conn_handle, 1,
                                    BLE_GATTS_CLT_CFG_F_INDICATE, 0);

    /* Update characteristic 1's value. */
    ble_gatts_notify_test_chr_1_len = 1;
    ble_gatts_notify_test_chr_1_val[0] = 0xab;
    ble_gatts_chr_updated(ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Verify indication sent properly. */
    ble_gatts_notify_test_misc_verify_tx_i(
        conn_handle,
        ble_gatts_notify_test_chr_1_def_handle + 1,
        ble_gatts_notify_test_chr_1_val,
        ble_gatts_notify_test_chr_1_len);

    /* Verify 'updated' state is still persisted. */
    key_cccd.peer_addr = *BLE_ADDR_ANY;
    key_cccd.chr_val_handle = ble_gatts_notify_test_chr_1_def_handle + 1;
    key_cccd.idx = 0;

    rc = ble_store_read_cccd(&key_cccd, &value_cccd);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(value_cccd.value_changed);

    /* Disconnect. */
    ble_gatts_notify_test_disconnect(conn_handle,
                                     BLE_GATTS_CLT_CFG_F_INDICATE, 1, 0, 0);

    /* Ensure CCCD still persisted. */
    TEST_ASSERT(ble_hs_test_util_num_cccds() == 1);

    /* Reconnect. */
    ble_hs_test_util_create_conn(conn_handle, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 ble_gatts_notify_test_util_gap_event, NULL);

    /* Simulate a successful encryption procedure (bonding restoration). */
    ble_gatts_notify_test_restore_bonding(conn_handle,
                                          BLE_GATTS_CLT_CFG_F_INDICATE, 1,
                                          0, 0);

    /* Receive the confirmation for the indication. */
    ble_gatts_notify_test_misc_rx_indicate_rsp(
        conn_handle,
        ble_gatts_notify_test_chr_1_def_handle + 1);

    /* Verify no pending GATT jobs. */
    TEST_ASSERT(!ble_gattc_any_jobs());

    /* Ensure indication enabled. */
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_1_def_handle);
    TEST_ASSERT(flags == BLE_GATTS_CLT_CFG_F_INDICATE);
    flags = ble_gatts_notify_test_misc_read_notify(
        conn_handle, ble_gatts_notify_test_chr_2_def_handle);
    TEST_ASSERT(flags == 0);

    /* Ensure CCCD still persisted. */
    TEST_ASSERT(ble_hs_test_util_num_cccds() == 1);

    /* Verify 'updated' state is no longer persisted. */
    rc = ble_store_read_cccd(&key_cccd, &value_cccd);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(!value_cccd.value_changed);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_notify_test_disallowed)
{
    uint16_t chr1_val_handle;
    uint16_t chr2_val_handle;
    uint16_t chr3_val_handle;

    const struct ble_gatt_svc_def svcs[] = { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(1),
            .access_cb = ble_gatts_notify_test_misc_access,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
            .val_handle = &chr1_val_handle,
        }, {
            .uuid = BLE_UUID16_DECLARE(2),
            .access_cb = ble_gatts_notify_test_misc_access,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_INDICATE,
            .val_handle = &chr2_val_handle,
        }, {
            .uuid = BLE_UUID16_DECLARE(3),
            .access_cb = ble_gatts_notify_test_misc_access,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY |
                     BLE_GATT_CHR_F_INDICATE,
            .val_handle = &chr3_val_handle,
        }, {
            0
        } },
    }, {
        0
    } };

    ble_hs_test_util_init();

    ble_hs_test_util_reg_svcs(svcs, NULL, NULL);
    TEST_ASSERT_FATAL(chr1_val_handle != 0);
    TEST_ASSERT_FATAL(chr2_val_handle != 0);
    TEST_ASSERT_FATAL(chr3_val_handle != 0);

    ble_hs_test_util_create_conn(2, ble_gatts_notify_test_peer_addr,
                                 ble_gatts_notify_test_util_gap_event, NULL);

    /* Attempt to enable notifications on chr1 should succeed. */
    ble_gatts_notify_test_misc_try_enable_notify(
        2, chr1_val_handle - 1, BLE_GATTS_CLT_CFG_F_NOTIFY, 0);

    /* Attempt to enable indications on chr1 should fail. */
    ble_gatts_notify_test_misc_try_enable_notify(
        2, chr1_val_handle - 1, BLE_GATTS_CLT_CFG_F_INDICATE, 1);

    /* Attempt to enable notifications on chr2 should fail. */
    ble_gatts_notify_test_misc_try_enable_notify(
        2, chr2_val_handle - 1, BLE_GATTS_CLT_CFG_F_NOTIFY, 1);

    /* Attempt to enable indications on chr2 should succeed. */
    ble_gatts_notify_test_misc_try_enable_notify(
        2, chr2_val_handle - 1, BLE_GATTS_CLT_CFG_F_INDICATE, 0);

    /* Attempt to enable notifications on chr3 should succeed. */
    ble_gatts_notify_test_misc_try_enable_notify(
        2, chr3_val_handle - 1, BLE_GATTS_CLT_CFG_F_NOTIFY, 0);

    /* Attempt to enable indications on chr3 should succeed. */
    ble_gatts_notify_test_misc_try_enable_notify(
        2, chr3_val_handle - 1, BLE_GATTS_CLT_CFG_F_INDICATE, 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_gatts_notify_suite)
{
    ble_gatts_notify_test_n();
    ble_gatts_notify_test_i();

    ble_gatts_notify_test_bonded_n();
    ble_gatts_notify_test_bonded_i();

    ble_gatts_notify_test_bonded_i_no_ack();

    ble_gatts_notify_test_disallowed();

    /* XXX: Test corner cases:
     *     o Bonding after CCCD configuration.
     *     o Disconnect prior to rx of indicate ack.
     */
}
