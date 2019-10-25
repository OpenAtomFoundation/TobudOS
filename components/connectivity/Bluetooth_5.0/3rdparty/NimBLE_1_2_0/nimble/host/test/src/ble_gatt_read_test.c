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
#include <limits.h>
#include "testutil/testutil.h"
#include "nimble/ble.h"
#include "ble_hs_test.h"
#include "host/ble_uuid.h"
#include "ble_hs_test_util.h"

struct ble_gatt_read_test_attr {
    uint16_t conn_handle;
    uint16_t handle;
    uint8_t value_len;
    uint8_t value[BLE_ATT_ATTR_MAX_LEN];
};

#define BLE_GATT_READ_TEST_MAX_ATTRS    256

struct ble_gatt_read_test_attr
    ble_gatt_read_test_attrs[BLE_GATT_READ_TEST_MAX_ATTRS];
int ble_gatt_read_test_num_attrs;
int ble_gatt_read_test_complete;

uint16_t ble_gatt_read_test_bad_conn_handle;
int ble_gatt_read_test_bad_status;

static void
ble_gatt_read_test_misc_init(void)
{
    ble_hs_test_util_init();
    ble_gatt_read_test_num_attrs = 0;
    ble_gatt_read_test_complete = 0;
    ble_gatt_read_test_bad_conn_handle = 0;
    ble_gatt_read_test_bad_status = 0;

    memset(&ble_gatt_read_test_attrs[0], 0,
           sizeof ble_gatt_read_test_attrs[0]);
}

static int
ble_gatt_read_test_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                      struct ble_gatt_attr *attr, void *arg)
{
    struct ble_gatt_read_test_attr *dst;
    int *stop_after;
    int rc;

    stop_after = arg;

    TEST_ASSERT_FATAL(error != NULL);

    if (error->status != 0) {
        ble_gatt_read_test_bad_conn_handle = conn_handle;
        ble_gatt_read_test_bad_status = error->status;
        ble_gatt_read_test_complete = 1;
        return 0;
    }

    if (attr == NULL) {
        ble_gatt_read_test_complete = 1;
        return 0;
    }

    TEST_ASSERT_FATAL(ble_gatt_read_test_num_attrs <
                      BLE_GATT_READ_TEST_MAX_ATTRS);
    dst = ble_gatt_read_test_attrs + ble_gatt_read_test_num_attrs++;

    TEST_ASSERT_FATAL(OS_MBUF_PKTLEN(attr->om) <= sizeof dst->value);

    dst->conn_handle = conn_handle;
    dst->handle = attr->handle;
    dst->value_len = OS_MBUF_PKTLEN(attr->om);
    rc = os_mbuf_copydata(attr->om, 0, OS_MBUF_PKTLEN(attr->om), dst->value);
    TEST_ASSERT_FATAL(rc == 0);

    if (stop_after != NULL && *stop_after > 0) {
        (*stop_after)--;
        if (*stop_after == 0) {
            ble_gatt_read_test_complete = 1;
            return 1;
        }
    } else {
        ble_gatt_read_test_complete = 1;
    }

    return 0;
}

static int
ble_gatt_read_test_long_cb(uint16_t conn_handle,
                           const struct ble_gatt_error *error,
                           struct ble_gatt_attr *attr, void *arg)
{
    struct ble_gatt_read_test_attr *dst;
    int *reads_left;
    int rc;

    reads_left = arg;

    TEST_ASSERT_FATAL(error != NULL);

    if (error->status != 0) {
        ble_gatt_read_test_bad_conn_handle = conn_handle;
        ble_gatt_read_test_bad_status = error->status;
        ble_gatt_read_test_complete = 1;
        return 0;
    }

    if (attr == NULL) {
        ble_gatt_read_test_complete = 1;
        return 0;
    }

    dst = ble_gatt_read_test_attrs + 0;

    TEST_ASSERT_FATAL(OS_MBUF_PKTLEN(attr->om) <=
        dst->value_len + sizeof dst->value);
    TEST_ASSERT(attr->offset == dst->value_len);

    if (attr->offset == 0) {
        dst->conn_handle = conn_handle;
        dst->handle = attr->handle;
    } else {
        TEST_ASSERT(conn_handle == dst->conn_handle);
        TEST_ASSERT(attr->handle == dst->handle);
    }
    rc = os_mbuf_copydata(attr->om, 0, OS_MBUF_PKTLEN(attr->om),
                          dst->value + dst->value_len);
    TEST_ASSERT_FATAL(rc == 0);
    dst->value_len += OS_MBUF_PKTLEN(attr->om);

    if (reads_left != NULL && *reads_left > 0) {
        (*reads_left)--;
        if (*reads_left == 0) {
            ble_gatt_read_test_complete = 1;
            return 1;
        }
    }

    return 0;
}

static void
ble_gatt_read_test_misc_rx_rsp_good_raw(uint16_t conn_handle,
                                        uint8_t att_op,
                                        const void *data, int data_len)
{
    uint8_t buf[1024];
    int rc;

    TEST_ASSERT_FATAL(data_len <= sizeof buf);

    /* Send the pending ATT Read Request. */

    buf[0] = att_op;
    memcpy(buf + 1, data, data_len);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, 1 + data_len);
    TEST_ASSERT(rc == 0);
}

static void
ble_gatt_read_test_misc_rx_rsp_good(uint16_t conn_handle,
                                    struct ble_hs_test_util_flat_attr *attr)
{
    ble_gatt_read_test_misc_rx_rsp_good_raw(conn_handle, BLE_ATT_OP_READ_RSP,
                                            attr->value,
                                            attr->value_len);
}

static void
ble_gatt_read_test_misc_rx_rsp_bad(uint16_t conn_handle,
                                   uint8_t att_error, uint16_t err_handle)
{
    /* Send the pending ATT Read Request. */

    ble_hs_test_util_rx_att_err_rsp(conn_handle, BLE_ATT_OP_READ_REQ,
                                    att_error, err_handle);
}

static int
ble_gatt_read_test_misc_uuid_rx_rsp_good(
    uint16_t conn_handle, struct ble_hs_test_util_flat_attr *attrs)
{
    struct ble_att_read_type_rsp rsp;
    uint8_t buf[1024];
    int prev_len;
    int off;
    int rc;
    int i;

    if (ble_gatt_read_test_complete || attrs[0].handle == 0) {
        return 0;
    }

    /* Send the pending ATT Read By Type Request. */

    rsp.batp_length = 2 + attrs[0].value_len;
    ble_att_read_type_rsp_write(buf, sizeof buf, &rsp);

    prev_len = 0;
    off = BLE_ATT_READ_TYPE_RSP_BASE_SZ;
    for (i = 0; attrs[i].handle != 0; i++) {
        if (prev_len != 0 && prev_len != attrs[i].value_len) {
            break;
        }
        prev_len = attrs[i].value_len;

        put_le16(buf + off, attrs[i].handle);
        off += 2;

        memcpy(buf + off, attrs[i].value, attrs[i].value_len);
        off += attrs[i].value_len;
    }

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    TEST_ASSERT(rc == 0);

    return i;
}

static void
ble_gatt_read_test_misc_verify_good(struct ble_hs_test_util_flat_attr *attr)
{
    int rc;

    ble_gatt_read_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /* Exchange MTU: We need plus 1 for the read response opcode */
    ble_hs_test_util_set_att_mtu(2, attr->value_len + 1);

    rc = ble_gattc_read(2, attr->handle, ble_gatt_read_test_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    ble_gatt_read_test_misc_rx_rsp_good(2, attr);

    TEST_ASSERT(ble_gatt_read_test_num_attrs == 1);
    TEST_ASSERT(ble_gatt_read_test_attrs[0].conn_handle == 2);
    TEST_ASSERT(ble_gatt_read_test_attrs[0].handle == attr->handle);
    TEST_ASSERT(ble_gatt_read_test_attrs[0].value_len == attr->value_len);
    TEST_ASSERT(memcmp(ble_gatt_read_test_attrs[0].value, attr->value,
                       attr->value_len) == 0);
}

static void
ble_gatt_read_test_misc_verify_bad(uint8_t att_status,
                                   struct ble_hs_test_util_flat_attr *attr)
{
    int rc;

    ble_gatt_read_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    rc = ble_gattc_read(2, attr->handle, ble_gatt_read_test_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    ble_gatt_read_test_misc_rx_rsp_bad(2, att_status, attr->handle);

    TEST_ASSERT(ble_gatt_read_test_num_attrs == 0);
    TEST_ASSERT(ble_gatt_read_test_bad_conn_handle == 2);
    TEST_ASSERT(ble_gatt_read_test_bad_status ==
                BLE_HS_ERR_ATT_BASE + att_status);
    TEST_ASSERT(!ble_gattc_any_jobs());
}

static void
ble_gatt_read_test_misc_uuid_verify_good(
    uint16_t start_handle, uint16_t end_handle, const ble_uuid_t *uuid,
    int stop_after, struct ble_hs_test_util_flat_attr *attrs)
{
    int num_read;
    int idx;
    int rc;
    int i;

    ble_gatt_read_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    rc = ble_gattc_read_by_uuid(2, start_handle, end_handle, uuid,
                                ble_gatt_read_test_cb, &stop_after);
    TEST_ASSERT_FATAL(rc == 0);

    idx = 0;
    while (1) {
        num_read = ble_gatt_read_test_misc_uuid_rx_rsp_good(2, attrs + idx);
        if (num_read == 0) {
            ble_hs_test_util_rx_att_err_rsp(2, BLE_ATT_OP_READ_TYPE_REQ,
                                            BLE_ATT_ERR_ATTR_NOT_FOUND,
                                            start_handle);
            break;
        }

        idx += num_read;
    }

    TEST_ASSERT(ble_gatt_read_test_complete);
    TEST_ASSERT(idx == ble_gatt_read_test_num_attrs);

    for (i = 0; i < idx; i++) {
        TEST_ASSERT(ble_gatt_read_test_attrs[i].conn_handle == 2);
        TEST_ASSERT(ble_gatt_read_test_attrs[i].handle == attrs[i].handle);
        TEST_ASSERT(ble_gatt_read_test_attrs[i].value_len ==
                    attrs[i].value_len);
        TEST_ASSERT(memcmp(ble_gatt_read_test_attrs[i].value, attrs[i].value,
                           attrs[i].value_len) == 0);
    }
    TEST_ASSERT(!ble_gattc_any_jobs());
}

static void
ble_gatt_read_test_misc_long_verify_good(
    int max_reads, struct ble_hs_test_util_flat_attr *attr)
{
    int reads_left;
    int chunk_sz;
    int rem_len;
    int att_op;
    uint16_t offset = 0;
    int off;
    int rc;

    ble_gatt_read_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    if (max_reads == 0) {
        max_reads = INT_MAX;
    }
    reads_left = max_reads;
    rc = ble_gattc_read_long(2, attr->handle, offset,
                             ble_gatt_read_test_long_cb, &reads_left);
    TEST_ASSERT_FATAL(rc == 0);

    off = 0;
    rem_len = attr->value_len;
    do {
        if (rem_len > BLE_ATT_MTU_DFLT - 1) {
            chunk_sz = BLE_ATT_MTU_DFLT - 1;
        } else {
            chunk_sz = rem_len;
        }
        if (off == 0) {
            att_op = BLE_ATT_OP_READ_RSP;
        } else {
            att_op = BLE_ATT_OP_READ_BLOB_RSP;
        }
        ble_gatt_read_test_misc_rx_rsp_good_raw(2, att_op,
                                                attr->value + off, chunk_sz);
        rem_len -= chunk_sz;
        off += chunk_sz;
    } while (rem_len > 0 && reads_left > 0);

    TEST_ASSERT(ble_gatt_read_test_complete);
    TEST_ASSERT(!ble_gattc_any_jobs());
    TEST_ASSERT(ble_gatt_read_test_attrs[0].conn_handle == 2);
    TEST_ASSERT(ble_gatt_read_test_attrs[0].handle == attr->handle);
    if (reads_left > 0) {
        TEST_ASSERT(ble_gatt_read_test_attrs[0].value_len == attr->value_len);
    }
    TEST_ASSERT(memcmp(ble_gatt_read_test_attrs[0].value, attr->value,
                       ble_gatt_read_test_attrs[0].value_len) == 0);
}

static void
ble_gatt_read_test_misc_long_verify_bad(
    uint8_t att_status, struct ble_hs_test_util_flat_attr *attr)
{
    uint16_t offset = 0;
    int rc;

    ble_gatt_read_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    rc = ble_gattc_read_long(2, attr->handle, offset,
                             ble_gatt_read_test_long_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    ble_gatt_read_test_misc_rx_rsp_bad(2, att_status, attr->handle);

    TEST_ASSERT(ble_gatt_read_test_num_attrs == 0);
    TEST_ASSERT(ble_gatt_read_test_bad_conn_handle == 2);
    TEST_ASSERT(ble_gatt_read_test_bad_status ==
                BLE_HS_ERR_ATT_BASE + att_status);
    TEST_ASSERT(!ble_gattc_any_jobs());
}

static int
ble_gatt_read_test_misc_extract_handles(
    struct ble_hs_test_util_flat_attr *attrs, uint16_t *handles)
{
    int i;

    for (i = 0; attrs[i].handle != 0; i++) {
        handles[i] = attrs[i].handle;
    }
    return i;
}

static void
ble_gatt_read_test_misc_mult_verify_good(
    struct ble_hs_test_util_flat_attr *attrs)
{
    uint8_t expected_value[512];
    uint16_t handles[256];
    int num_attrs;
    int chunk_sz;
    int off;
    int rc;
    int i;

    ble_gatt_read_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    num_attrs = ble_gatt_read_test_misc_extract_handles(attrs, handles);

    off = 0;
    for (i = 0; i < num_attrs; i++) {
        if (attrs[i].value_len > BLE_ATT_MTU_DFLT - 1 - off) {
            chunk_sz = BLE_ATT_MTU_DFLT - 1 - off;
        } else {
            chunk_sz = attrs[i].value_len;
        }

        if (chunk_sz > 0) {
            memcpy(expected_value + off, attrs[i].value, chunk_sz);
            off += chunk_sz;
        }
    }

    rc = ble_gattc_read_mult(2, handles, num_attrs,
                             ble_gatt_read_test_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    ble_gatt_read_test_misc_rx_rsp_good_raw(2, BLE_ATT_OP_READ_MULT_RSP,
                                            expected_value, off);

    TEST_ASSERT(ble_gatt_read_test_complete);
    TEST_ASSERT(!ble_gattc_any_jobs());
    TEST_ASSERT(ble_gatt_read_test_attrs[0].conn_handle == 2);
    TEST_ASSERT(ble_gatt_read_test_attrs[0].value_len == off);
    TEST_ASSERT(memcmp(ble_gatt_read_test_attrs[0].value, expected_value,
                       off) == 0);
}

static void
ble_gatt_read_test_misc_mult_verify_bad(
    uint8_t att_status, uint16_t err_handle,
    struct ble_hs_test_util_flat_attr *attrs)
{
    uint16_t handles[256];
    int num_attrs;
    int rc;

    ble_gatt_read_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    num_attrs = ble_gatt_read_test_misc_extract_handles(attrs, handles);

    rc = ble_gattc_read_mult(2, handles, num_attrs,
                             ble_gatt_read_test_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    ble_gatt_read_test_misc_rx_rsp_bad(2, att_status, err_handle);

    TEST_ASSERT(ble_gatt_read_test_num_attrs == 0);
    TEST_ASSERT(ble_gatt_read_test_bad_conn_handle == 2);
    TEST_ASSERT(ble_gatt_read_test_bad_status ==
                BLE_HS_ERR_ATT_BASE + att_status);
    TEST_ASSERT(!ble_gattc_any_jobs());
}

TEST_CASE_SELF(ble_gatt_read_test_by_handle)
{
    /* Read a seven-byte attribute. */
    ble_gatt_read_test_misc_verify_good(
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 43,
        .value = { 1,2,3,4,5,6,7 },
        .value_len = 7
    } });

    /* Read a one-byte attribute. */
    ble_gatt_read_test_misc_verify_good(
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 0x5432,
        .value = { 0xff },
        .value_len = 1
    } });

    /* Read a 200-byte attribute. */
    ble_gatt_read_test_misc_verify_good(
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 815,
        .value = { 0 },
        .value_len = 200,
    } });

    /* Fail due to attribute not found. */
    ble_gatt_read_test_misc_verify_bad(BLE_ATT_ERR_ATTR_NOT_FOUND,
        (struct ble_hs_test_util_flat_attr[]) { {
            .handle = 719,
            .value = { 1,2,3,4,5,6,7 },
            .value_len = 7
        } });

    /* Fail due to invalid PDU. */
    ble_gatt_read_test_misc_verify_bad(BLE_ATT_ERR_INVALID_PDU,
        (struct ble_hs_test_util_flat_attr[]) { {
            .handle = 65,
            .value = { 0xfa, 0x4c },
            .value_len = 2
        } });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_read_test_by_uuid)
{
    /* Read a single seven-byte attribute. */
    ble_gatt_read_test_misc_uuid_verify_good(1, 100, BLE_UUID16_DECLARE(0x1234), 0,
        (struct ble_hs_test_util_flat_attr[]) { {
            .handle = 43,
            .value = { 1,2,3,4,5,6,7 },
            .value_len = 7
        }, {
            0,
        } });

    /* Read two seven-byte attributes; one response. */
    ble_gatt_read_test_misc_uuid_verify_good(1, 100, BLE_UUID16_DECLARE(0x1234), 0,
        (struct ble_hs_test_util_flat_attr[]) { {
            .handle = 43,
            .value = { 1,2,3,4,5,6,7 },
            .value_len = 7
        }, {
            .handle = 44,
            .value = { 2,3,4,5,6,7,8 },
            .value_len = 7
        }, {
            0,
        } });

    /* Read two attributes; two responses. */
    ble_gatt_read_test_misc_uuid_verify_good(1, 100, BLE_UUID16_DECLARE(0x1234), 0,
        (struct ble_hs_test_util_flat_attr[]) { {
            .handle = 43,
            .value = { 1,2,3,4,5,6,7 },
            .value_len = 7
        }, {
            .handle = 44,
            .value = { 2,3,4 },
            .value_len = 3
        }, {
            0,
        } });

    /* Stop after three reads. */
    ble_gatt_read_test_misc_uuid_verify_good(1, 100, BLE_UUID16_DECLARE(0x1234), 3,
        (struct ble_hs_test_util_flat_attr[]) { {
            .handle = 43,
            .value = { 1,2,3,4,5,6,7 },
            .value_len = 7
        }, {
            .handle = 44,
            .value = { 2,3,4 },
            .value_len = 3
        }, {
            .handle = 45,
            .value = { 2,3,4 },
            .value_len = 3
        }, {
            .handle = 46,
            .value = { 3,4,5,6 },
            .value_len = 4
        }, {
            .handle = 47,
            .value = { 2,3,4 },
            .value_len = 3
        }, {
            0,
        } });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_read_test_long)
{
    uint8_t data512[512];
    int i;

    for (i = 0; i < sizeof data512; i++) {
        data512[i] = i;
    }

    /* Read a seven-byte attribute. */
    ble_gatt_read_test_misc_long_verify_good(0,
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 43,
        .value = { 1,2,3,4,5,6,7 },
        .value_len = 7
    } });

    /* Read a zero-byte attribute. */
    ble_gatt_read_test_misc_long_verify_good(0,
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 43,
        .value = { 0 },
        .value_len = 0
    } });

    /* Read a 60-byte attribute; three requests. */
    ble_gatt_read_test_misc_long_verify_good(0,
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 34,
        .value = {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
            33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
            49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60
        },
        .value_len = 60
    } });

    /* Stop after two reads. */
    ble_gatt_read_test_misc_long_verify_good(2,
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 34,
        .value = {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
            33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
            49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60
        },
        .value_len = 60
    } });

    /* Fail due to attribute not found. */
    ble_gatt_read_test_misc_long_verify_bad(BLE_ATT_ERR_ATTR_NOT_FOUND,
        (struct ble_hs_test_util_flat_attr[]) { {
            .handle = 719,
            .value = { 1, 2, 3, 4, 5, 6, 7 },
            .value_len = 7
        } });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_read_test_mult)
{
    uint8_t data512[512];
    int i;

    for (i = 0; i < sizeof data512; i++) {
        data512[i] = i;
    }

    /* Read one attribute. */
    ble_gatt_read_test_misc_mult_verify_good(
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 43,
        .value = { 0, 1, 2, 3, 4, 5, 6, 7 },
        .value_len = 7
    }, {
        0
    } });

    /* Read two attributes. */
    ble_gatt_read_test_misc_mult_verify_good(
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 43,
        .value = { 0, 1, 2, 3, 4, 5, 6, 7 },
        .value_len = 7,
    }, {
        .handle = 44,
        .value = { 8, 9, 10, 11 },
        .value_len = 4,
    }, {
        0
    } });

    /* Read two attributes (swap order). */
    ble_gatt_read_test_misc_mult_verify_good(
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 44,
        .value = { 8, 9, 10, 11 },
        .value_len = 4,
    }, {
        .handle = 43,
        .value = { 0, 1, 2, 3, 4, 5, 6, 7 },
        .value_len = 7,
    }, {
        0
    } });

    /* Read five attributes. */
    ble_gatt_read_test_misc_mult_verify_good(
        (struct ble_hs_test_util_flat_attr[]) { {
        .handle = 43,
        .value = { 0, 1, 2, 3, 4, 5, 6, 7 },
        .value_len = 7,
    }, {
        .handle = 44,
        .value = { 8, 9, 10, 11 },
        .value_len = 4,
    }, {
        .handle = 145,
        .value = { 12, 13 },
        .value_len = 2,
    }, {
        .handle = 191,
        .value = { 14, 15, 16 },
        .value_len = 3,
    }, {
        .handle = 352,
        .value = { 17, 18, 19, 20 },
        .value_len = 4,
    }, {
        0
    } });

    /* Fail due to attribute not found. */
    ble_gatt_read_test_misc_mult_verify_bad(BLE_ATT_ERR_ATTR_NOT_FOUND, 719,
        (struct ble_hs_test_util_flat_attr[]) { {
            .handle = 719,
            .value = { 1,2,3,4,5,6,7 },
            .value_len = 7
        }, {
            0
        } });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_read_test_concurrent)
{
    int rc;
    int i;

    ble_gatt_read_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /***
     * Perform three concurrent reads.  Assert that each response is correctly
     * matched up with its corresponding GATT procedure.
     */

    struct ble_hs_test_util_flat_attr attrs[3] = {
        {
            .handle = 1,
            .offset = 0,
            .value_len = 3,
            .value = { 1, 2, 3 },
        },
        {
            .handle = 2,
            .offset = 0,
            .value_len = 4,
            .value = { 2, 3, 4, 5 },
        },
        {
            .handle = 3,
            .offset = 0,
            .value_len = 5,
            .value = { 3, 4, 5, 6, 7 },
        },
    };

    rc = ble_gattc_read(2, attrs[0].handle, ble_gatt_read_test_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);
    rc = ble_gattc_read(2, attrs[1].handle, ble_gatt_read_test_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);
    rc = ble_gattc_read(2, attrs[2].handle, ble_gatt_read_test_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    ble_gatt_read_test_misc_rx_rsp_good(2, attrs + 0);
    ble_gatt_read_test_misc_rx_rsp_good(2, attrs + 1);
    ble_gatt_read_test_misc_rx_rsp_good(2, attrs + 2);

    TEST_ASSERT(ble_gatt_read_test_num_attrs == 3);

    for (i = 0; i < 3; i++) {
        TEST_ASSERT(ble_gatt_read_test_attrs[i].handle == attrs[i].handle);
        TEST_ASSERT(ble_gatt_read_test_attrs[i].value_len ==
                    attrs[i].value_len);
        TEST_ASSERT(memcmp(ble_gatt_read_test_attrs[i].value, attrs[i].value,
                           attrs[i].value_len) == 0);
    }

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_read_test_long_oom)
{
    static const struct ble_hs_test_util_flat_attr attr = {
        .handle = 34,
        .value = {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
            33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
            49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60
        },
        .value_len = 60,
    };

    struct os_mbuf *oms;
    int32_t ticks_until;
    int reads_left;
    int chunk_sz;
    uint16_t offset = 0;
    int off;
    int rc;

    ble_gatt_read_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /* Initiate a read long procedure. */
    off = 0;
    reads_left = 0;
    rc = ble_gattc_read_long(2, attr.handle, offset, ble_gatt_read_test_long_cb,
                             &reads_left);
    TEST_ASSERT_FATAL(rc == 0);

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    chunk_sz = ble_att_mtu(2) - BLE_ATT_READ_RSP_BASE_SZ;
    ble_gatt_read_test_misc_rx_rsp_good_raw(2, BLE_ATT_OP_READ_RSP,
                                            attr.value + off, chunk_sz);
    off += chunk_sz;

    /* Ensure no follow-up request got sent.  It should not have gotten sent
     * due to mbuf exhaustion.
     */
    ble_hs_test_util_prev_tx_queue_clear();
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue_pullup() == NULL);

    /* Verify that we will resume the stalled GATT procedure in one second. */
    ticks_until = ble_gattc_timer();
    TEST_ASSERT(ticks_until == os_time_ms_to_ticks32(MYNEWT_VAL(BLE_GATT_RESUME_RATE)));

    /* Verify the procedure proceeds after mbufs become available. */
    rc = os_mbuf_free_chain(oms);
    TEST_ASSERT_FATAL(rc == 0);
    os_time_advance(ticks_until);
    ble_gattc_timer();

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    chunk_sz = ble_att_mtu(2) - BLE_ATT_READ_RSP_BASE_SZ;
    ble_gatt_read_test_misc_rx_rsp_good_raw(2, BLE_ATT_OP_READ_RSP,
                                            attr.value + off, chunk_sz);
    off += chunk_sz;

    /* Ensure no follow-up request got sent.  It should not have gotten sent
     * due to mbuf exhaustion.
     */
    ble_hs_test_util_prev_tx_queue_clear();
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue_pullup() == NULL);

    /* Verify that we will resume the stalled GATT procedure in one second. */
    ticks_until = ble_gattc_timer();
    TEST_ASSERT(ticks_until == os_time_ms_to_ticks32(MYNEWT_VAL(BLE_GATT_RESUME_RATE)));

    /* Verify that procedure completes when mbufs are available. */
    rc = os_mbuf_free_chain(oms);
    TEST_ASSERT_FATAL(rc == 0);
    os_time_advance(ticks_until);
    ble_gattc_timer();

    chunk_sz = attr.value_len - off;
    ble_gatt_read_test_misc_rx_rsp_good_raw(2, BLE_ATT_OP_READ_RSP,
                                            attr.value + off, chunk_sz);
    off += chunk_sz;

    TEST_ASSERT(ble_gatt_read_test_complete);
    TEST_ASSERT(!ble_gattc_any_jobs());
    TEST_ASSERT(ble_gatt_read_test_attrs[0].conn_handle == 2);
    TEST_ASSERT(ble_gatt_read_test_attrs[0].handle == attr.handle);
    TEST_ASSERT(ble_gatt_read_test_attrs[0].value_len == attr.value_len);
    TEST_ASSERT(memcmp(ble_gatt_read_test_attrs[0].value, attr.value,
                       ble_gatt_read_test_attrs[0].value_len) == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_gatt_read_test_suite)
{
    ble_gatt_read_test_by_handle();
    ble_gatt_read_test_by_uuid();
    ble_gatt_read_test_long();
    ble_gatt_read_test_mult();
    ble_gatt_read_test_concurrent();
    ble_gatt_read_test_long_oom();
}
