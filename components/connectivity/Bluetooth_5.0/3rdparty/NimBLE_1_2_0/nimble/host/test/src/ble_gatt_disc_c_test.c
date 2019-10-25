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
#include "host/ble_gatt.h"
#include "host/ble_uuid.h"
#include "ble_hs_test_util.h"

struct ble_gatt_disc_c_test_char {
    uint16_t def_handle;
    uint16_t val_handle;
    uint8_t properties;
    const ble_uuid_t *uuid;
};

#define BLE_GATT_DISC_C_TEST_MAX_CHARS  256
static struct ble_gatt_chr
    ble_gatt_disc_c_test_chars[BLE_GATT_DISC_C_TEST_MAX_CHARS];
static int ble_gatt_disc_c_test_num_chars;
static int ble_gatt_disc_c_test_rx_complete;

static void
ble_gatt_disc_c_test_init(void)
{
    ble_hs_test_util_init();

    ble_gatt_disc_c_test_num_chars = 0;
    ble_gatt_disc_c_test_rx_complete = 0;
}

static int
ble_gatt_disc_c_test_misc_rx_rsp_once(
    uint16_t conn_handle, struct ble_gatt_disc_c_test_char *chars)
{
    struct ble_att_read_type_rsp rsp;
    uint8_t buf[1024];
    int off;
    int rc;
    int i;

    /* Send the pending ATT Read By Type Request. */

    if (chars[0].uuid->type == BLE_UUID_TYPE_16) {
       rsp.batp_length = BLE_ATT_READ_TYPE_ADATA_BASE_SZ +
                         BLE_GATT_CHR_DECL_SZ_16;
    } else {
       rsp.batp_length = BLE_ATT_READ_TYPE_ADATA_BASE_SZ +
                         BLE_GATT_CHR_DECL_SZ_128;
    }

    ble_att_read_type_rsp_write(buf, BLE_ATT_READ_TYPE_RSP_BASE_SZ, &rsp);

    off = BLE_ATT_READ_TYPE_RSP_BASE_SZ;
    for (i = 0; ; i++) {
        if (chars[i].def_handle == 0) {
            /* No more services. */
            break;
        }

        /* If the value length is changing, we need a separate response. */
        if (((chars[i].uuid->type == BLE_UUID_TYPE_16) ^
            (chars[0].uuid->type == BLE_UUID_TYPE_16)) != 0) {
            break;
        }

        if (chars[i].uuid->type == BLE_UUID_TYPE_16) {
            if (off + BLE_ATT_READ_TYPE_ADATA_SZ_16 >
                ble_att_mtu(conn_handle)) {

                /* Can't fit any more entries. */
                break;
            }
        } else {
            if (off + BLE_ATT_READ_TYPE_ADATA_SZ_128 >
                ble_att_mtu(conn_handle)) {

                /* Can't fit any more entries. */
                break;
            }
        }

        put_le16(buf + off, chars[i].def_handle);
        off += 2;

        buf[off] = chars[i].properties;
        off++;

        put_le16(buf + off, chars[i].val_handle);
        off += 2;

        ble_uuid_flat(chars[i].uuid, buf + off);
        off += ble_uuid_length(chars[i].uuid);
    }

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    TEST_ASSERT(rc == 0);

    return i;
}

static void
ble_gatt_disc_c_test_misc_rx_rsp(uint16_t conn_handle,
                                 uint16_t end_handle,
                                 struct ble_gatt_disc_c_test_char *chars)
{
    int count;
    int idx;

    idx = 0;
    while (chars[idx].def_handle != 0) {
        count = ble_gatt_disc_c_test_misc_rx_rsp_once(conn_handle,
                                                      chars + idx);
        if (count == 0) {
            break;
        }
        idx += count;
    }

    if (chars[idx - 1].def_handle != end_handle) {
        /* Send the pending ATT Request. */
        ble_hs_test_util_rx_att_err_rsp(conn_handle, BLE_ATT_OP_READ_TYPE_REQ,
                                        BLE_ATT_ERR_ATTR_NOT_FOUND,
                                        chars[idx - 1].def_handle);
    }
}

static void
ble_gatt_disc_c_test_misc_verify_chars(struct ble_gatt_disc_c_test_char *chars,
                                       int stop_after)
{
    int i;

    if (stop_after == 0) {
        stop_after = BLE_GATT_DISC_C_TEST_MAX_CHARS;
    }

    for (i = 0; i < stop_after && chars[i].def_handle != 0; i++) {
        TEST_ASSERT(chars[i].def_handle ==
                    ble_gatt_disc_c_test_chars[i].def_handle);
        TEST_ASSERT(chars[i].val_handle ==
                    ble_gatt_disc_c_test_chars[i].val_handle);
        TEST_ASSERT(ble_uuid_cmp(chars[i].uuid,
                                 &ble_gatt_disc_c_test_chars[i].uuid.u) == 0);
    }

    TEST_ASSERT(i == ble_gatt_disc_c_test_num_chars);
    TEST_ASSERT(ble_gatt_disc_c_test_rx_complete);
}

static int
ble_gatt_disc_c_test_misc_cb(uint16_t conn_handle,
                             const struct ble_gatt_error *error,
                             const struct ble_gatt_chr *chr, void *arg)
{
    struct ble_gatt_chr *dst;
    int *stop_after;

    TEST_ASSERT(error != NULL);
    TEST_ASSERT(!ble_gatt_disc_c_test_rx_complete);

    stop_after = arg;

    switch (error->status) {
    case 0:
        TEST_ASSERT_FATAL(ble_gatt_disc_c_test_num_chars <
                          BLE_GATT_DISC_C_TEST_MAX_CHARS);

        dst = ble_gatt_disc_c_test_chars + ble_gatt_disc_c_test_num_chars++;
        *dst = *chr;
        break;

    case BLE_HS_EDONE:
        ble_gatt_disc_c_test_rx_complete = 1;
        break;

    default:
        TEST_ASSERT(0);
        break;
    }

    if (*stop_after > 0) {
        (*stop_after)--;
        if (*stop_after == 0) {
            ble_gatt_disc_c_test_rx_complete = 1;
            return 1;
        }
    }

    return 0;
}

static void
ble_gatt_disc_c_test_misc_all(uint16_t start_handle, uint16_t end_handle,
                              int stop_after,
                              struct ble_gatt_disc_c_test_char *chars)
{
    int num_left;
    int rc;

    ble_gatt_disc_c_test_init();

    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    num_left = stop_after;
    rc = ble_gattc_disc_all_chrs(2, start_handle, end_handle,
                                 ble_gatt_disc_c_test_misc_cb, &num_left);
    TEST_ASSERT(rc == 0);

    ble_gatt_disc_c_test_misc_rx_rsp(2, end_handle, chars);
    ble_gatt_disc_c_test_misc_verify_chars(chars, stop_after);
}

static void
ble_gatt_disc_c_test_misc_uuid(uint16_t start_handle, uint16_t end_handle,
                               int stop_after, const ble_uuid_t *uuid,
                               struct ble_gatt_disc_c_test_char *rsp_chars,
                               struct ble_gatt_disc_c_test_char *ret_chars)
{
    int rc;

    ble_gatt_disc_c_test_init();

    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    rc = ble_gattc_disc_chrs_by_uuid(2, start_handle, end_handle,
                                     uuid,
                                     ble_gatt_disc_c_test_misc_cb,
                                     &stop_after);
    TEST_ASSERT(rc == 0);

    ble_gatt_disc_c_test_misc_rx_rsp(2, end_handle, rsp_chars);
    ble_gatt_disc_c_test_misc_verify_chars(ret_chars, 0);
}

TEST_CASE_SELF(ble_gatt_disc_c_test_disc_all)
{
    /*** One 16-bit characteristic. */
    ble_gatt_disc_c_test_misc_all(50, 100, 0,
                                  (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, { 0 }
    });

    /*** Two 16-bit characteristics. */
    ble_gatt_disc_c_test_misc_all(50, 100, 0,
                                  (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 57,
            .val_handle = 58,
            .uuid = BLE_UUID16_DECLARE(0x64ba),
        }, { 0 }
    });

    /*** Five 16-bit characteristics. */
    ble_gatt_disc_c_test_misc_all(50, 100, 0,
                                  (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 57,
            .val_handle = 58,
            .uuid = BLE_UUID16_DECLARE(0x64ba),
        }, {
            .def_handle = 59,
            .val_handle = 60,
            .uuid = BLE_UUID16_DECLARE(0x5372),
        }, {
            .def_handle = 61,
            .val_handle = 62,
            .uuid = BLE_UUID16_DECLARE(0xab93),
        }, {
            .def_handle = 63,
            .val_handle = 64,
            .uuid = BLE_UUID16_DECLARE(0x0023),
        }, { 0 }
    });

    /*** Interleaved 16-bit and 128-bit characteristics. */
    ble_gatt_disc_c_test_misc_all(50, 100, 0,
                                  (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 83,
            .val_handle = 84,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 87,
            .val_handle = 88,
            .uuid = BLE_UUID128_DECLARE(0, 1, 2, 3, 4, 5, 6, 7,
                                        8, 9, 10, 11, 12, 13, 14, 15),
        }, {
            .def_handle = 91,
            .val_handle = 92,
            .uuid = BLE_UUID16_DECLARE(0x0003),
        }, {
            .def_handle = 93,
            .val_handle = 94,
            .uuid = BLE_UUID128_DECLARE(1, 0, 4, 0, 6, 9, 17, 7,
                                        8, 43, 7, 4, 12, 43, 19, 35),
        }, {
            .def_handle = 98,
            .val_handle = 99,
            .uuid = BLE_UUID16_DECLARE(0xabfa),
        }, { 0 }
    });

    /*** Ends with final handle ID. */
    ble_gatt_disc_c_test_misc_all(50, 100, 0,
                                  (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 99,
            .val_handle = 100,
            .uuid = BLE_UUID16_DECLARE(0x64ba),
        }, { 0 }
    });

    /*** Stop after two characteristics. */
    ble_gatt_disc_c_test_misc_all(50, 100, 2,
                                  (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 57,
            .val_handle = 58,
            .uuid = BLE_UUID16_DECLARE(0x64ba),
        }, {
            .def_handle = 59,
            .val_handle = 60,
            .uuid = BLE_UUID16_DECLARE(0x5372),
        }, {
            .def_handle = 61,
            .val_handle = 62,
            .uuid = BLE_UUID16_DECLARE(0xab93),
        }, {
            .def_handle = 63,
            .val_handle = 64,
            .uuid = BLE_UUID16_DECLARE(0x0023),
        }, { 0 }
    });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_disc_c_test_disc_uuid)
{
    /*** One 16-bit characteristic. */
    ble_gatt_disc_c_test_misc_uuid(50, 100, 0, BLE_UUID16_DECLARE(0x2010),
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, { 0 } },
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, { 0 } }
    );

    /*** No matching characteristics. */
    ble_gatt_disc_c_test_misc_uuid(50, 100, 0, BLE_UUID16_DECLARE(0x2010),
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x1234),
        }, { 0 } },
        (struct ble_gatt_disc_c_test_char[]) {
        { 0 } }
    );

    /*** 2/5 16-bit characteristics. */
    ble_gatt_disc_c_test_misc_uuid(50, 100, 0, BLE_UUID16_DECLARE(0x2010),
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 57,
            .val_handle = 58,
            .uuid = BLE_UUID16_DECLARE(0x64ba),
        }, {
            .def_handle = 59,
            .val_handle = 60,
            .uuid = BLE_UUID16_DECLARE(0x5372),
        }, {
            .def_handle = 61,
            .val_handle = 62,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 63,
            .val_handle = 64,
            .uuid = BLE_UUID16_DECLARE(0x0023),
        }, { 0 } },
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 61,
            .val_handle = 62,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, { 0 } }
    );

    /*** Interleaved 16-bit and 128-bit characteristics. */
    ble_gatt_disc_c_test_misc_uuid(
        50, 100, 0,
        BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 83,
            .val_handle = 84,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 87,
            .val_handle = 88,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        }, {
            .def_handle = 91,
            .val_handle = 92,
            .uuid = BLE_UUID16_DECLARE(0x0003),
        }, {
            .def_handle = 93,
            .val_handle = 94,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        }, {
            .def_handle = 98,
            .val_handle = 99,
            .uuid = BLE_UUID16_DECLARE(0xabfa),
        }, { 0 } },
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 87,
            .val_handle = 88,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        }, {
            .def_handle = 93,
            .val_handle = 94,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        }, { 0 } }
    );

    /*** Ends with final handle ID. */
    ble_gatt_disc_c_test_misc_uuid(50, 100, 0, BLE_UUID16_DECLARE(0x64ba),
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 99,
            .val_handle = 100,
            .uuid = BLE_UUID16_DECLARE(0x64ba),
        }, { 0 } },
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 99,
            .val_handle = 100,
            .uuid = BLE_UUID16_DECLARE(0x64ba),
        }, { 0 } }
    );

    /*** Stop after first characteristic. */
    ble_gatt_disc_c_test_misc_uuid(50, 100, 1, BLE_UUID16_DECLARE(0x2010),
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 57,
            .val_handle = 58,
            .uuid = BLE_UUID16_DECLARE(0x64ba),
        }, {
            .def_handle = 59,
            .val_handle = 60,
            .uuid = BLE_UUID16_DECLARE(0x5372),
        }, {
            .def_handle = 61,
            .val_handle = 62,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, {
            .def_handle = 63,
            .val_handle = 64,
            .uuid = BLE_UUID16_DECLARE(0x0023),
        }, { 0 } },
        (struct ble_gatt_disc_c_test_char[]) {
        {
            .def_handle = 55,
            .val_handle = 56,
            .uuid = BLE_UUID16_DECLARE(0x2010),
        }, { 0 } }
    );

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_disc_c_test_oom_all)
{
    /* Retrieve enough characteristics to require two transactions. */
    struct ble_gatt_disc_c_test_char chrs[] = {
        {
            .def_handle = 93,
            .val_handle = 94,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        },
        {
            .def_handle = 95,
            .val_handle = 96,
            .uuid = BLE_UUID128_DECLARE(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
        },
        { 0 }
    };

    struct os_mbuf *oms;
    int32_t ticks_until;
    int stop_after;
    int num_chrs;
    int rc;

    ble_gatt_disc_c_test_init();

    ble_hs_test_util_create_conn(1, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /* Initiate a discover all characteristics procedure. */
    stop_after = 0;
    rc = ble_gattc_disc_all_chrs(1, 1, 0xffff,
                                 ble_gatt_disc_c_test_misc_cb, &stop_after);
    TEST_ASSERT_FATAL(rc == 0);

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    num_chrs = ble_gatt_disc_c_test_misc_rx_rsp_once(1, chrs);

    /* Make sure there are still undiscovered characteristics. */
    TEST_ASSERT_FATAL(num_chrs < sizeof chrs / sizeof chrs[0] - 1);

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
    ble_gatt_disc_c_test_misc_rx_rsp_once(1, chrs + num_chrs);

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

    ble_hs_test_util_rx_att_err_rsp(1,
                                    BLE_ATT_OP_READ_TYPE_REQ,
                                    BLE_ATT_ERR_ATTR_NOT_FOUND,
                                    1);
    ble_gatt_disc_c_test_misc_verify_chars(chrs, 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_disc_c_test_oom_uuid)
{
    /* Retrieve enough characteristics to require two transactions. */
    struct ble_gatt_disc_c_test_char chrs[] = {
        {
            .def_handle = 93,
            .val_handle = 94,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        },
        {
            .def_handle = 95,
            .val_handle = 96,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        },
        { 0 }
    };

    struct os_mbuf *oms;
    int32_t ticks_until;
    int stop_after;
    int num_chrs;
    int rc;

    ble_gatt_disc_c_test_init();

    ble_hs_test_util_create_conn(1, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /* Initiate a discover characteristics by UUID procedure. */
    stop_after = 0;
    rc = ble_gattc_disc_chrs_by_uuid(1, 1, 0xffff, chrs[0].uuid,
                                     ble_gatt_disc_c_test_misc_cb,
                                     &stop_after);
    TEST_ASSERT_FATAL(rc == 0);

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    num_chrs = ble_gatt_disc_c_test_misc_rx_rsp_once(1, chrs);

    /* Make sure there are still undiscovered characteristics. */
    TEST_ASSERT_FATAL(num_chrs < sizeof chrs / sizeof chrs[0] - 1);

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
    ble_gatt_disc_c_test_misc_rx_rsp_once(1, chrs + num_chrs);

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

    ble_hs_test_util_rx_att_err_rsp(1,
                                    BLE_ATT_OP_READ_TYPE_REQ,
                                    BLE_ATT_ERR_ATTR_NOT_FOUND,
                                    1);
    ble_gatt_disc_c_test_misc_verify_chars(chrs, 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_gatt_disc_c_test_suite)
{
    ble_gatt_disc_c_test_disc_all();
    ble_gatt_disc_c_test_disc_uuid();
    ble_gatt_disc_c_test_oom_all();
    ble_gatt_disc_c_test_oom_uuid();
}
