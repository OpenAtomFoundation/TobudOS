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
#include "host/ble_uuid.h"
#include "ble_hs_test_util.h"

static struct ble_gatt_svc ble_gatt_find_s_test_svcs[256];
static int ble_gatt_find_s_test_num_svcs;
static int ble_gatt_find_s_test_proc_complete;

struct ble_gatt_find_s_test_entry {
    uint16_t inc_handle; /* 0 indicates no more entries. */
    uint16_t start_handle;
    uint16_t end_handle;
    const ble_uuid_t *uuid;
};

static void
ble_gatt_find_s_test_misc_init(void)
{
    ble_hs_test_util_init();
    ble_gatt_find_s_test_num_svcs = 0;
    ble_gatt_find_s_test_proc_complete = 0;
}

static int
ble_gatt_find_s_test_misc_cb(uint16_t conn_handle,
                             const struct ble_gatt_error *error,
                             const struct ble_gatt_svc *service,
                             void *arg)
{
    TEST_ASSERT(!ble_gatt_find_s_test_proc_complete);
    TEST_ASSERT(error != NULL);

    switch (error->status) {
    case 0:
        ble_gatt_find_s_test_svcs[ble_gatt_find_s_test_num_svcs++] = *service;
        break;

    case BLE_HS_EDONE:
        ble_gatt_find_s_test_proc_complete = 1;
        break;

    default:
        TEST_ASSERT(0);
        break;
    }

    return 0;
}

static void
ble_gatt_find_s_test_misc_verify_incs(
    struct ble_gatt_find_s_test_entry *entries)
{
    int i;

    for (i = 0; entries[i].inc_handle != 0; i++) {
        TEST_ASSERT(ble_gatt_find_s_test_svcs[i].start_handle ==
                    entries[i].start_handle);
        TEST_ASSERT(ble_gatt_find_s_test_svcs[i].end_handle ==
                    entries[i].end_handle);
        TEST_ASSERT(ble_uuid_cmp(&ble_gatt_find_s_test_svcs[i].uuid.u,
                                 entries[i].uuid) == 0);
    }

    TEST_ASSERT(i == ble_gatt_find_s_test_num_svcs);
    TEST_ASSERT(ble_gatt_find_s_test_proc_complete);
}

static int
ble_gatt_find_s_test_misc_rx_read_type(
    uint16_t conn_handle, struct ble_gatt_find_s_test_entry *entries)
{
    struct ble_att_read_type_rsp rsp;
    uint8_t buf[1024];
    int off;
    int rc;
    int i;

    memset(&rsp, 0, sizeof rsp);

    off = BLE_ATT_READ_TYPE_RSP_BASE_SZ;
    for (i = 0; entries[i].inc_handle != 0; i++) {
        if (rsp.batp_length == BLE_GATTS_INC_SVC_LEN_NO_UUID + 2) {
            break;
        }

        if (entries[i].uuid->type != BLE_UUID_TYPE_16) {
            if (rsp.batp_length != 0) {
                break;
            }
            rsp.batp_length = BLE_GATTS_INC_SVC_LEN_NO_UUID + 2;
        } else {
            rsp.batp_length = BLE_GATTS_INC_SVC_LEN_UUID + 2;
        }

        TEST_ASSERT_FATAL(off + rsp.batp_length <= sizeof buf);

        put_le16(buf + off, entries[i].inc_handle);
        off += 2;

        put_le16(buf + off, entries[i].start_handle);
        off += 2;

        put_le16(buf + off, entries[i].end_handle);
        off += 2;

        if (entries[i].uuid->type == BLE_UUID_TYPE_16) {
            put_le16(buf + off, ble_uuid_u16(entries[i].uuid));
            off += 2;
        }
    }

    if (i == 0) {
        ble_hs_test_util_rx_att_err_rsp(conn_handle, BLE_ATT_OP_READ_TYPE_REQ,
                                        BLE_ATT_ERR_ATTR_NOT_FOUND, 0);
        return 0;
    }

    ble_att_read_type_rsp_write(buf + 0, BLE_ATT_READ_TYPE_RSP_BASE_SZ, &rsp);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    TEST_ASSERT(rc == 0);

    return i;
}

static void
ble_gatt_find_s_test_misc_rx_read(uint16_t conn_handle, const ble_uuid_t *uuid)
{
    uint8_t buf[17];
    int rc;

    TEST_ASSERT(uuid->type == BLE_UUID_TYPE_128);

    buf[0] = BLE_ATT_OP_READ_RSP;
    ble_uuid_flat(uuid, buf + 1);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, 17);
    TEST_ASSERT(rc == 0);
}

static void
ble_gatt_find_s_test_misc_verify_tx_read_type(uint16_t start_handle,
                                              uint16_t end_handle)
{
    struct ble_att_read_type_req req;
    struct os_mbuf *om;
    uint16_t uuid16;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);

    ble_att_read_type_req_parse(om->om_data, om->om_len, &req);

    TEST_ASSERT(req.batq_start_handle == start_handle);
    TEST_ASSERT(req.batq_end_handle == end_handle);
    TEST_ASSERT(om->om_len == BLE_ATT_READ_TYPE_REQ_BASE_SZ + 2);
    uuid16 = get_le16(om->om_data + BLE_ATT_READ_TYPE_REQ_BASE_SZ);
    TEST_ASSERT(uuid16 == BLE_ATT_UUID_INCLUDE);
}

static void
ble_gatt_find_s_test_misc_verify_tx_read(uint16_t handle)
{
    struct ble_att_read_req req;
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);

    ble_att_read_req_parse(om->om_data, om->om_len, &req);

    TEST_ASSERT(req.barq_handle == handle);
    TEST_ASSERT(om->om_len == BLE_ATT_READ_REQ_SZ);
}

static void
ble_gatt_find_s_test_misc_find_inc(uint16_t conn_handle,
                                   uint16_t start_handle, uint16_t end_handle,
                                   struct ble_gatt_find_s_test_entry *entries)
{
    struct ble_gatt_svc service;
    int cur_start;
    int num_found;
    int idx;
    int rc;
    int i;

    rc = ble_gattc_find_inc_svcs(conn_handle, start_handle, end_handle,
                                 ble_gatt_find_s_test_misc_cb, &service);
    TEST_ASSERT(rc == 0);

    cur_start = start_handle;
    idx = 0;
    while (1) {
        ble_gatt_find_s_test_misc_verify_tx_read_type(cur_start, end_handle);
        num_found = ble_gatt_find_s_test_misc_rx_read_type(conn_handle,
                                                           entries + idx);
        if (num_found == 0) {
            break;
        }

        if (entries[idx].uuid->type == BLE_UUID_TYPE_128) {
            TEST_ASSERT(num_found == 1);
            ble_gatt_find_s_test_misc_verify_tx_read(
                entries[idx].start_handle);
            ble_gatt_find_s_test_misc_rx_read(conn_handle,
                                              entries[idx].uuid);
        }

        idx += num_found;
        cur_start = entries[idx - 1].inc_handle + 1;
    }
    TEST_ASSERT(idx == ble_gatt_find_s_test_num_svcs);
    TEST_ASSERT(ble_gatt_find_s_test_proc_complete);

    for (i = 0; i < ble_gatt_find_s_test_num_svcs; i++) {
        TEST_ASSERT(ble_gatt_find_s_test_svcs[i].start_handle ==
                    entries[i].start_handle);
        TEST_ASSERT(ble_gatt_find_s_test_svcs[i].end_handle ==
                    entries[i].end_handle);
        TEST_ASSERT(ble_uuid_cmp(&ble_gatt_find_s_test_svcs[i].uuid.u,
                                 entries[i].uuid) == 0);
    }
}

TEST_CASE_SELF(ble_gatt_find_s_test_1)
{
    /* Two 16-bit UUID services; one response. */
    ble_gatt_find_s_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);
    ble_gatt_find_s_test_misc_find_inc(2, 5, 10,
        ((struct ble_gatt_find_s_test_entry[]) { {
            .inc_handle = 6,
            .start_handle = 35,
            .end_handle = 49,
            .uuid = BLE_UUID16_DECLARE(0x5155),
        }, {
            .inc_handle = 9,
            .start_handle = 543,
            .end_handle = 870,
            .uuid = BLE_UUID16_DECLARE(0x1122),
        }, {
            0,
        } })
    );

    /* One 128-bit UUID service; two responses. */
    ble_gatt_find_s_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);
    ble_gatt_find_s_test_misc_find_inc(2, 34, 100,
        ((struct ble_gatt_find_s_test_entry[]) { {
            .inc_handle = 36,
            .start_handle = 403,
            .end_handle = 859,
            .uuid = BLE_UUID128_DECLARE(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
        }, {
            0,
        } })
    );

    /* Two 128-bit UUID service; four responses. */
    ble_gatt_find_s_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);
    ble_gatt_find_s_test_misc_find_inc(2, 34, 100,
        ((struct ble_gatt_find_s_test_entry[]) { {
            .inc_handle = 36,
            .start_handle = 403,
            .end_handle = 859,
            .uuid = BLE_UUID128_DECLARE(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
        }, {
            .inc_handle = 39,
            .start_handle = 900,
            .end_handle = 932,
            .uuid = BLE_UUID128_DECLARE(2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17),
        }, {
            0,
        } })
    );

    /* Two 16-bit UUID; three 128-bit UUID; seven responses. */
    ble_gatt_find_s_test_misc_init();
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);
    ble_gatt_find_s_test_misc_find_inc(2, 1, 100,
        ((struct ble_gatt_find_s_test_entry[]) { {
            .inc_handle = 36,
            .start_handle = 403,
            .end_handle = 859,
            .uuid = BLE_UUID128_DECLARE(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
        }, {
            .inc_handle = 37,
            .start_handle = 35,
            .end_handle = 49,
            .uuid = BLE_UUID16_DECLARE(0x5155),
        }, {
            .inc_handle = 38,
            .start_handle = 543,
            .end_handle = 870,
            .uuid = BLE_UUID16_DECLARE(0x1122),
        }, {
            .inc_handle = 39,
            .start_handle = 900,
            .end_handle = 932,
            .uuid = BLE_UUID128_DECLARE(2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17),
        }, {
            .inc_handle = 40,
            .start_handle = 940,
            .end_handle = 950,
            .uuid = BLE_UUID128_DECLARE(3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18),
        }, {
            0,
        } })
    );

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_find_s_test_oom)
{

    struct ble_gatt_find_s_test_entry incs[] = {
        {
            .inc_handle = 21,
            .start_handle = 800,
            .end_handle = 899,
            .uuid = BLE_UUID128_DECLARE(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        },
        {
            .inc_handle = 22,
            .start_handle = 900,
            .end_handle = 999,
            .uuid = BLE_UUID128_DECLARE(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
        },
        { 0 }
    };

    struct os_mbuf *oms;
    int32_t ticks_until;
    int rc;

    ble_gatt_find_s_test_misc_init();

    ble_hs_test_util_create_conn(1, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /* Initiate a discover all characteristics procedure. */
    rc = ble_gattc_find_inc_svcs(1, 20, 30,
                                 ble_gatt_find_s_test_misc_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    ble_gatt_find_s_test_misc_rx_read_type(1, incs);

    /* Ensure no follow-up request got sent.  It should not have gotten sent
     * due to mbuf exhaustion.
     */
    ble_hs_test_util_prev_tx_queue_clear();
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue_pullup() == NULL);

    /* Verify that we will resume the stalled GATT procedure in one second. */
    ticks_until = ble_gattc_timer();
    TEST_ASSERT(ticks_until == os_time_ms_to_ticks32(MYNEWT_VAL(BLE_GATT_RESUME_RATE)));

    /* Verify the procedure succeeds after mbufs become available. */
    rc = os_mbuf_free_chain(oms);
    TEST_ASSERT_FATAL(rc == 0);
    os_time_advance(ticks_until);
    ble_gattc_timer();

    /* We can't cause a memory exhaustion error on the follow up request.  The
     * GATT client frees the read response immediately before sending the
     * follow-up request, so there is always an mbuf available.
     */
    /* XXX: Find a way to test this. */
    ble_gatt_find_s_test_misc_rx_read(1, incs[0].uuid);

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    ble_gatt_find_s_test_misc_rx_read_type(1, incs + 1);

    /* Verify the procedure succeeds after mbufs become available. */
    rc = os_mbuf_free_chain(oms);
    TEST_ASSERT_FATAL(rc == 0);
    os_time_advance(ticks_until);
    ble_gattc_timer();

    ble_gatt_find_s_test_misc_rx_read(1, incs[1].uuid);

    ble_hs_test_util_rx_att_err_rsp(1,
                                    BLE_ATT_OP_READ_TYPE_REQ,
                                    BLE_ATT_ERR_ATTR_NOT_FOUND,
                                    1);

    ble_gatt_find_s_test_misc_verify_incs(incs);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_gatt_find_s_test_suite)
{
    ble_gatt_find_s_test_1();
    ble_gatt_find_s_test_oom();
}
