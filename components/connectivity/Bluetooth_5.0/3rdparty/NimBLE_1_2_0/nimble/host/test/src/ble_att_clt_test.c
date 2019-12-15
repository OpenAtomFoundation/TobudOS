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
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

/**
 * @return                      The handle of the new test connection.
 */
static uint16_t
ble_att_clt_test_misc_init(void)
{
    ble_hs_test_util_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}), NULL,
                                 NULL);
    return 2;
}

static void
ble_att_clt_test_misc_verify_tx_write(uint16_t handle_id, void *value,
                                      int value_len, int is_req)
{
    struct ble_att_write_req req;
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);

    if (is_req) {
        ble_att_write_req_parse(om->om_data, om->om_len, &req);
    } else {
        ble_att_write_cmd_parse(om->om_data, om->om_len, &req);
    }

    TEST_ASSERT(req.bawq_handle == handle_id);
    TEST_ASSERT(om->om_len == BLE_ATT_WRITE_REQ_BASE_SZ + value_len);
    TEST_ASSERT(memcmp(om->om_data + BLE_ATT_WRITE_REQ_BASE_SZ, value,
                       value_len) == 0);
}

static void
ble_att_clt_test_tx_write_req_or_cmd(uint16_t conn_handle, uint16_t handle,
                                     void *value, int value_len, int is_req)
{
    struct os_mbuf *om;
    int rc;

    om = ble_hs_test_util_om_from_flat(value, value_len);
    if (is_req) {
        rc = ble_att_clt_tx_write_req(conn_handle, handle, om);
    } else {
        rc = ble_att_clt_tx_write_cmd(conn_handle, handle, om);
    }
    TEST_ASSERT(rc == 0);
}

TEST_CASE_SELF(ble_att_clt_test_tx_find_info)
{
    uint16_t conn_handle;
    int rc;

    ble_hs_test_util_assert_mbufs_freed(NULL);

    conn_handle = ble_att_clt_test_misc_init();

    /*** Success. */
    rc = ble_att_clt_tx_find_info(conn_handle, 1, 0xffff);
    TEST_ASSERT(rc == 0);

    /*** Error: start handle of 0. */
    rc = ble_att_clt_tx_find_info(conn_handle, 0, 0xffff);
    TEST_ASSERT(rc == BLE_HS_EINVAL);

    /*** Error: start handle greater than end handle. */
    rc = ble_att_clt_tx_find_info(conn_handle, 500, 499);
    TEST_ASSERT(rc == BLE_HS_EINVAL);

    /*** Success; start and end handles equal. */
    rc = ble_att_clt_tx_find_info(conn_handle, 500, 500);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_rx_find_info)
{
    struct ble_att_find_info_rsp rsp;
    uint16_t conn_handle;
    uint8_t buf[1024];
    uint8_t uuid128_1[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
    int off;
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    /*** One 128-bit UUID. */
    /* Receive response with attribute mapping. */
    off = 0;
    rsp.bafp_format = BLE_ATT_FIND_INFO_RSP_FORMAT_128BIT;
    ble_att_find_info_rsp_write(buf + off, sizeof buf - off, &rsp);
    off += BLE_ATT_FIND_INFO_RSP_BASE_SZ;

    put_le16(buf + off, 1);
    off += 2;
    memcpy(buf + off, uuid128_1, 16);
    off += 16;

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    TEST_ASSERT(rc == 0);

    /*** One 16-bit UUID. */
    /* Receive response with attribute mapping. */
    off = 0;
    rsp.bafp_format = BLE_ATT_FIND_INFO_RSP_FORMAT_16BIT;
    ble_att_find_info_rsp_write(buf + off, sizeof buf - off, &rsp);
    off += BLE_ATT_FIND_INFO_RSP_BASE_SZ;

    put_le16(buf + off, 2);
    off += 2;
    put_le16(buf + off, 0x000f);
    off += 2;

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    TEST_ASSERT(rc == 0);

    /*** Two 16-bit UUIDs. */
    /* Receive response with attribute mappings. */
    off = 0;
    rsp.bafp_format = BLE_ATT_FIND_INFO_RSP_FORMAT_16BIT;
    ble_att_find_info_rsp_write(buf + off, sizeof buf - off, &rsp);
    off += BLE_ATT_FIND_INFO_RSP_BASE_SZ;

    put_le16(buf + off, 3);
    off += 2;
    put_le16(buf + off, 0x0010);
    off += 2;

    put_le16(buf + off, 4);
    off += 2;
    put_le16(buf + off, 0x0011);
    off += 2;

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

static void
ble_att_clt_test_case_tx_write_req_or_cmd(int is_req)
{
    uint16_t conn_handle;
    uint8_t value300[500] = { 0 };
    uint8_t value5[5] = { 6, 7, 54, 34, 8 };

    conn_handle = ble_att_clt_test_misc_init();

    /*** 5-byte write. */
    ble_att_clt_test_tx_write_req_or_cmd(conn_handle, 0x1234, value5,
                                         sizeof value5, is_req);
    ble_att_clt_test_misc_verify_tx_write(0x1234, value5, sizeof value5,
                                          is_req);

    /*** Overlong write; verify command truncated to ATT MTU. */
    ble_att_clt_test_tx_write_req_or_cmd(conn_handle, 0xab83, value300,
                                         sizeof value300, is_req);
    ble_att_clt_test_misc_verify_tx_write(0xab83, value300,
                                          BLE_ATT_MTU_DFLT - 3, is_req);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

static void
ble_att_clt_test_misc_prep_good(uint16_t handle, uint16_t offset,
                                uint8_t *attr_data, uint16_t attr_data_len)
{
    struct ble_att_prep_write_cmd req;
    struct os_mbuf *om;
    uint16_t conn_handle;
    int rc;
    int i;

    conn_handle = ble_att_clt_test_misc_init();

    om = ble_hs_test_util_om_from_flat(attr_data, attr_data_len);
    rc = ble_att_clt_tx_prep_write(conn_handle, handle, offset, om);
    TEST_ASSERT(rc == 0);

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);
    TEST_ASSERT(om->om_len == BLE_ATT_PREP_WRITE_CMD_BASE_SZ + attr_data_len);

    ble_att_prep_write_req_parse(om->om_data, om->om_len, &req);
    TEST_ASSERT(req.bapc_handle == handle);
    TEST_ASSERT(req.bapc_offset == offset);
    for (i = 0; i < attr_data_len; i++) {
        TEST_ASSERT(om->om_data[BLE_ATT_PREP_WRITE_CMD_BASE_SZ + i] ==
                    attr_data[i]);
    }
}

static void
ble_att_clt_test_misc_exec_good(uint8_t flags)
{
    struct ble_att_exec_write_req req;
    struct os_mbuf *om;
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    rc = ble_att_clt_tx_exec_write(conn_handle, flags);
    TEST_ASSERT(rc == 0);

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);
    TEST_ASSERT(om->om_len == BLE_ATT_EXEC_WRITE_REQ_SZ);

    ble_att_exec_write_req_parse(om->om_data, om->om_len, &req);
    TEST_ASSERT(req.baeq_flags == flags);
}

static void
ble_att_clt_test_misc_prep_bad(uint16_t handle, uint16_t offset,
                               uint8_t *attr_data, uint16_t attr_data_len,
                               int status)
{
    struct os_mbuf *om;
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    om = ble_hs_test_util_om_from_flat(attr_data, attr_data_len);

    rc = ble_att_clt_tx_prep_write(conn_handle, handle, offset, om);
    TEST_ASSERT(rc == status);
}

static void
ble_att_clt_test_misc_tx_mtu(uint16_t conn_handle, uint16_t mtu, int status)
{
    int rc;

    rc = ble_att_clt_tx_mtu(conn_handle, mtu);
    TEST_ASSERT(rc == status);

    if (rc == 0) {
        ble_hs_test_util_verify_tx_mtu_cmd(1, mtu);
    }
}

TEST_CASE_SELF(ble_att_clt_test_tx_write)
{
    ble_att_clt_test_case_tx_write_req_or_cmd(0);
    ble_att_clt_test_case_tx_write_req_or_cmd(1);
}

TEST_CASE_SELF(ble_att_clt_test_tx_read)
{
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    /*** Success. */
    rc = ble_att_clt_tx_read(conn_handle, 1);
    TEST_ASSERT(rc == 0);

    /*** Error: handle of 0. */
    rc = ble_att_clt_tx_read(conn_handle, 0);
    TEST_ASSERT(rc == BLE_HS_EINVAL);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_rx_read)
{
    uint16_t conn_handle;
    uint8_t buf[1024];
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    /*** Basic success. */
    buf[0] = BLE_ATT_OP_READ_RSP;
    buf[1] = 0;
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, 2);
    TEST_ASSERT(rc == 0);

    /*** Larger response. */
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, 20);
    TEST_ASSERT(rc == 0);

    /*** Zero-length response. */
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, 1);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_tx_read_blob)
{
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    /*** Success. */
    rc = ble_att_clt_tx_read_blob(conn_handle, 1, 0);
    TEST_ASSERT(rc == 0);

    /*** Error: handle of 0. */
    rc = ble_att_clt_tx_read_blob(conn_handle, 0, 0);
    TEST_ASSERT(rc == BLE_HS_EINVAL);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_rx_read_blob)
{
    uint16_t conn_handle;
    uint8_t buf[1024];
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    /*** Basic success. */
    buf[0] = BLE_ATT_OP_READ_BLOB_RSP;
    buf[1] = 0;
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, 2);
    TEST_ASSERT(rc == 0);

    /*** Larger response. */
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, 20);
    TEST_ASSERT(rc == 0);

    /*** Zero-length response. */
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, 1);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_tx_read_mult)
{
    struct os_mbuf *om;
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    /*** Success. */
    rc = ble_att_clt_tx_read_mult(conn_handle, ((uint16_t[]){ 1, 2 }), 2);
    TEST_ASSERT(rc == 0);

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);
    TEST_ASSERT(om->om_len == BLE_ATT_READ_MULT_REQ_BASE_SZ + 4);

    ble_att_read_mult_req_parse(om->om_data, om->om_len);
    TEST_ASSERT(get_le16(om->om_data + BLE_ATT_READ_MULT_REQ_BASE_SZ) == 1);
    TEST_ASSERT(get_le16(om->om_data + BLE_ATT_READ_MULT_REQ_BASE_SZ + 2) == 2);

    /*** Error: no handles. */
    rc = ble_att_clt_tx_read_mult(conn_handle, NULL, 0);
    TEST_ASSERT(rc == BLE_HS_EINVAL);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_rx_read_mult)
{
    uint16_t conn_handle;
    uint8_t buf[1024];
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    /*** Basic success. */
    ble_att_read_mult_rsp_write(buf, sizeof buf);
    put_le16(buf + BLE_ATT_READ_MULT_RSP_BASE_SZ + 0, 12);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_READ_MULT_RSP_BASE_SZ + 2);
    TEST_ASSERT(rc == 0);

    /*** Larger response. */
    put_le16(buf + BLE_ATT_READ_MULT_RSP_BASE_SZ + 0, 12);
    put_le16(buf + BLE_ATT_READ_MULT_RSP_BASE_SZ + 2, 43);
    put_le16(buf + BLE_ATT_READ_MULT_RSP_BASE_SZ + 4, 91);
    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_READ_MULT_RSP_BASE_SZ + 6);
    TEST_ASSERT(rc == 0);

    /*** Zero-length response. */
    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_READ_MULT_RSP_BASE_SZ + 0);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_tx_prep_write)
{
    uint8_t attr_data[512];
    int i;

    for (i = 0; i < sizeof attr_data; i++) {
        attr_data[i] = i;
    }

    /*** Success. */
    ble_att_clt_test_misc_prep_good(123, 0, attr_data, 16);
    ble_att_clt_test_misc_prep_good(5432, 100, attr_data, 2);
    ble_att_clt_test_misc_prep_good(0x1234, 400, attr_data, 0);
    ble_att_clt_test_misc_prep_good(5432, 0, attr_data,
                                    BLE_ATT_MTU_DFLT -
                                        BLE_ATT_PREP_WRITE_CMD_BASE_SZ);
    ble_att_clt_test_misc_prep_good(0x1234, 507, attr_data, 5);

    /*** Error: handle of 0. */
    ble_att_clt_test_misc_prep_bad(0, 0, attr_data, 16, BLE_HS_EINVAL);

    /*** Error: offset + length greater than maximum attribute size. */
    ble_att_clt_test_misc_prep_bad(1, 507, attr_data, 6, BLE_HS_EINVAL);

    /*** Error: packet larger than MTU. */
    ble_att_clt_test_misc_prep_bad(1, 0, attr_data,
                                   BLE_ATT_MTU_DFLT -
                                       BLE_ATT_PREP_WRITE_CMD_BASE_SZ + 1,
                                   BLE_HS_EINVAL);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_rx_prep_write)
{
    struct ble_att_prep_write_cmd rsp;
    uint16_t conn_handle;
    uint8_t buf[1024];
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    /*** Basic success. */
    rsp.bapc_handle = 0x1234;
    rsp.bapc_offset = 0;
    ble_att_prep_write_rsp_write(buf, sizeof buf, &rsp);
    memset(buf + BLE_ATT_PREP_WRITE_CMD_BASE_SZ, 1, 5);
    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_PREP_WRITE_CMD_BASE_SZ + 5);
    TEST_ASSERT(rc == 0);

    /*** 0-length write. */
    rsp.bapc_handle = 0x1234;
    rsp.bapc_offset = 0;
    ble_att_prep_write_rsp_write(buf, sizeof buf, &rsp);
    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf, BLE_ATT_PREP_WRITE_CMD_BASE_SZ);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_tx_exec_write)
{
    uint16_t conn_handle;
    int rc;

    conn_handle = ble_att_clt_test_misc_init();

    /*** Success. */
    ble_att_clt_test_misc_exec_good(BLE_ATT_EXEC_WRITE_F_CANCEL);
    ble_att_clt_test_misc_exec_good(BLE_ATT_EXEC_WRITE_F_EXECUTE);

    /*** Success: nonzero == execute. */
    rc = ble_att_clt_tx_exec_write(conn_handle, 0x02);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_att_clt_test_tx_mtu)
{
    uint16_t conn_handle;

    conn_handle = ble_att_clt_test_misc_init();

    /*** Success. */
    ble_att_clt_test_misc_tx_mtu(conn_handle, 50, 0);

    /*** Error: repeated sends. */
    ble_att_clt_test_misc_tx_mtu(conn_handle, 50, BLE_HS_EALREADY);
    ble_att_clt_test_misc_tx_mtu(conn_handle, 60, BLE_HS_EALREADY);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_att_clt_suite)
{
    ble_att_clt_test_tx_find_info();
    ble_att_clt_test_rx_find_info();
    ble_att_clt_test_tx_read();
    ble_att_clt_test_rx_read();
    ble_att_clt_test_tx_read_blob();
    ble_att_clt_test_rx_read_blob();
    ble_att_clt_test_tx_read_mult();
    ble_att_clt_test_rx_read_mult();
    ble_att_clt_test_tx_write();
    ble_att_clt_test_tx_prep_write();
    ble_att_clt_test_rx_prep_write();
    ble_att_clt_test_tx_exec_write();
    ble_att_clt_test_tx_mtu();
}
