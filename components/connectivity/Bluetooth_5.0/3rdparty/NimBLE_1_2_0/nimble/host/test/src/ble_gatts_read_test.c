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
#include "host/ble_uuid.h"
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

#define BLE_GATTS_READ_TEST_CHR_1_UUID    0x1111
#define BLE_GATTS_READ_TEST_CHR_2_UUID    0x2222

static uint8_t ble_gatts_read_test_peer_addr[6] = {2,3,4,5,6,7};

static int
ble_gatts_read_test_util_access_1(uint16_t conn_handle,
                                  uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt,
                                  void *arg);

static int
ble_gatts_read_test_util_access_2(uint16_t conn_handle,
                                  uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt,
                                  void *arg);
static void
ble_gatts_read_test_misc_reg_cb(struct ble_gatt_register_ctxt *ctxt,
                                void *arg);

static const struct ble_gatt_svc_def ble_gatts_read_test_svcs[] = { {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(0x1234),
    .characteristics = (struct ble_gatt_chr_def[]) { {
        .uuid = BLE_UUID16_DECLARE(BLE_GATTS_READ_TEST_CHR_1_UUID),
        .access_cb = ble_gatts_read_test_util_access_1,
        .flags = BLE_GATT_CHR_F_READ
    }, {
        .uuid = BLE_UUID16_DECLARE(BLE_GATTS_READ_TEST_CHR_2_UUID),
        .access_cb = ble_gatts_read_test_util_access_2,
        .flags = BLE_GATT_CHR_F_READ
    }, {
        0
    } },
}, {
    0
} };

static uint16_t ble_gatts_read_test_chr_1_def_handle;
static uint16_t ble_gatts_read_test_chr_1_val_handle;
static uint8_t ble_gatts_read_test_chr_1_val[1024];
static int ble_gatts_read_test_chr_1_len;
static uint16_t ble_gatts_read_test_chr_2_def_handle;
static uint16_t ble_gatts_read_test_chr_2_val_handle;

static void
ble_gatts_read_test_misc_init(uint16_t *out_conn_handle)
{
    ble_hs_test_util_init();

    ble_hs_test_util_reg_svcs(ble_gatts_read_test_svcs,
                              ble_gatts_read_test_misc_reg_cb,
                              NULL);
    TEST_ASSERT_FATAL(ble_gatts_read_test_chr_1_def_handle != 0);
    TEST_ASSERT_FATAL(ble_gatts_read_test_chr_1_val_handle ==
                      ble_gatts_read_test_chr_1_def_handle + 1);
    TEST_ASSERT_FATAL(ble_gatts_read_test_chr_2_def_handle != 0);
    TEST_ASSERT_FATAL(ble_gatts_read_test_chr_2_val_handle ==
                      ble_gatts_read_test_chr_2_def_handle + 1);

    ble_hs_test_util_create_conn(2, ble_gatts_read_test_peer_addr, NULL, NULL);

    if (out_conn_handle != NULL) {
        *out_conn_handle = 2;
    }
}

static void
ble_gatts_read_test_misc_reg_cb(struct ble_gatt_register_ctxt *ctxt,
                                void *arg)
{
    uint16_t uuid16;

    if (ctxt->op == BLE_GATT_REGISTER_OP_CHR) {
        uuid16 = ble_uuid_u16(ctxt->chr.chr_def->uuid);
        switch (uuid16) {
        case BLE_GATTS_READ_TEST_CHR_1_UUID:
            ble_gatts_read_test_chr_1_def_handle = ctxt->chr.def_handle;
            ble_gatts_read_test_chr_1_val_handle = ctxt->chr.val_handle;
            break;

        case BLE_GATTS_READ_TEST_CHR_2_UUID:
            ble_gatts_read_test_chr_2_def_handle = ctxt->chr.def_handle;
            ble_gatts_read_test_chr_2_val_handle = ctxt->chr.val_handle;
            break;

        default:
            TEST_ASSERT_FATAL(0);
            break;
        }
    }
}

static int
ble_gatts_read_test_util_access_1(uint16_t conn_handle,
                                  uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt,
                                  void *arg)
{
    int rc;

    TEST_ASSERT_FATAL(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);
    TEST_ASSERT_FATAL(attr_handle == ble_gatts_read_test_chr_1_val_handle);

    TEST_ASSERT(ctxt->chr ==
                &ble_gatts_read_test_svcs[0].characteristics[0]);

    rc = os_mbuf_append(ctxt->om, ble_gatts_read_test_chr_1_val,
                        ble_gatts_read_test_chr_1_len);
    TEST_ASSERT(rc == 0);

    return 0;
}

static int
ble_gatts_read_test_util_access_2(uint16_t conn_handle,
                                  uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt,
                                  void *arg)
{
    uint8_t *buf;

    TEST_ASSERT_FATAL(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);
    TEST_ASSERT_FATAL(attr_handle == ble_gatts_read_test_chr_2_def_handle + 1);

    TEST_ASSERT(ctxt->chr ==
                &ble_gatts_read_test_svcs[0].characteristics[1]);

    buf = os_mbuf_extend(ctxt->om, 6);
    TEST_ASSERT_FATAL(buf != NULL);

    buf[0] = 0;
    buf[1] = 10;
    buf[2] = 20;
    buf[3] = 30;
    buf[4] = 40;
    buf[5] = 50;

    return 0;
}

static void
ble_gatts_read_test_once(uint16_t conn_handle, uint16_t attr_id,
                         void *expected_value, uint16_t expected_len)
{
    struct ble_att_read_req read_req;
    uint8_t buf[BLE_ATT_READ_REQ_SZ];
    int rc;

    read_req.barq_handle = attr_id;
    ble_att_read_req_write(buf, sizeof buf, &read_req);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_verify_tx_read_rsp(expected_value, expected_len);
}

TEST_CASE_SELF(ble_gatts_read_test_case_basic)
{
    uint16_t conn_handle;

    ble_gatts_read_test_misc_init(&conn_handle);

    /*** Application points attribute at static data. */
    ble_gatts_read_test_chr_1_val[0] = 1;
    ble_gatts_read_test_chr_1_val[1] = 2;
    ble_gatts_read_test_chr_1_val[2] = 3;
    ble_gatts_read_test_chr_1_len = 3;
    ble_gatts_read_test_once(conn_handle,
                             ble_gatts_read_test_chr_1_val_handle,
                             ble_gatts_read_test_chr_1_val,
                             ble_gatts_read_test_chr_1_len);

    /*** Application uses stack-provided buffer for dynamic attribute. */
    ble_gatts_read_test_once(conn_handle,
                             ble_gatts_read_test_chr_2_def_handle + 1,
                             ((uint8_t[6]){0,10,20,30,40,50}), 6);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_read_test_case_long)
{
    struct ble_att_read_blob_req read_blob_req;
    struct ble_att_read_req read_req;
    uint8_t buf[max(BLE_ATT_READ_REQ_SZ, BLE_ATT_READ_BLOB_REQ_SZ)];
    uint16_t conn_handle;
    int rc;
    int i;

    ble_gatts_read_test_misc_init(&conn_handle);

    /*** Prepare characteristic value. */
    ble_gatts_read_test_chr_1_len = 40;
    for (i = 0; i < ble_gatts_read_test_chr_1_len; i++) {
        ble_gatts_read_test_chr_1_val[i] = i;
    }

    /* Receive first read request. */
    read_req.barq_handle = ble_gatts_read_test_chr_1_val_handle;
    ble_att_read_req_write(buf, sizeof buf, &read_req);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_verify_tx_read_rsp(ble_gatts_read_test_chr_1_val, 22);

    /* Receive follow-up read blob request. */
    read_blob_req.babq_handle = ble_gatts_read_test_chr_1_val_handle;
    read_blob_req.babq_offset = 22;
    ble_att_read_blob_req_write(buf, sizeof buf, &read_blob_req);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    TEST_ASSERT(rc == 0);

    /* Ensure response starts at appropriate offset (22). */
    ble_hs_test_util_verify_tx_read_blob_rsp(
        ble_gatts_read_test_chr_1_val + 22, 18);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_gatts_read_test_suite)
{
    ble_gatts_read_test_case_basic();
    ble_gatts_read_test_case_long();
}
