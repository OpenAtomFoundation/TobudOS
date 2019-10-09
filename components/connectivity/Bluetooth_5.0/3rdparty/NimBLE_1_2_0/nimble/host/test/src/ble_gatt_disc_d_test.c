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

struct ble_gatt_disc_d_test_dsc {
    uint16_t chr_val_handle; /* 0 if last entry. */
    uint16_t dsc_handle;
    ble_uuid_any_t dsc_uuid;
};

#define BLE_GATT_DISC_D_TEST_MAX_DSCS  256
static struct ble_gatt_disc_d_test_dsc
    ble_gatt_disc_d_test_dscs[BLE_GATT_DISC_D_TEST_MAX_DSCS];
static int ble_gatt_disc_d_test_num_dscs;
static int ble_gatt_disc_d_test_rx_complete;

static void
ble_gatt_disc_d_test_init(void)
{
    ble_hs_test_util_init();

    ble_gatt_disc_d_test_num_dscs = 0;
    ble_gatt_disc_d_test_rx_complete = 0;
}

static int
ble_gatt_disc_d_test_misc_rx_rsp_once(
    uint16_t conn_handle, struct ble_gatt_disc_d_test_dsc *dscs)
{
    struct ble_att_find_info_rsp rsp;
    uint8_t buf[1024];
    int off;
    int rc;
    int i;

    /* Send the pending ATT Read By Type Request. */

    if (dscs[0].dsc_uuid.u.type == BLE_UUID_TYPE_16) {
        rsp.bafp_format = BLE_ATT_FIND_INFO_RSP_FORMAT_16BIT;
    } else {
        rsp.bafp_format = BLE_ATT_FIND_INFO_RSP_FORMAT_128BIT;
    }

    ble_att_find_info_rsp_write(buf, BLE_ATT_FIND_INFO_RSP_BASE_SZ, &rsp);

    off = BLE_ATT_FIND_INFO_RSP_BASE_SZ;
    for (i = 0; ; i++) {
        if (dscs[i].chr_val_handle == 0) {
            /* No more descriptors. */
            break;
        }

        if (dscs[i].dsc_uuid.u.type == BLE_UUID_TYPE_16) {
            if (off + BLE_ATT_FIND_INFO_IDATA_16_SZ >
                ble_att_mtu(conn_handle)) {

                /* Can't fit any more entries. */
                break;
            }
        } else {
            if (off + BLE_ATT_FIND_INFO_IDATA_128_SZ >
                ble_att_mtu(conn_handle)) {

                /* Can't fit any more entries. */
                break;
            }
        }

        /* If the value length is changing, we need a separate response. */
        if (((dscs[0].dsc_uuid.u.type == BLE_UUID_TYPE_16) ^
            (dscs[i].dsc_uuid.u.type == BLE_UUID_TYPE_16)) != 0) {
            break;
        }

        put_le16(buf + off, dscs[i].dsc_handle);
        off += 2;

        ble_uuid_flat(&dscs[i].dsc_uuid.u, buf + off);
        off += ble_uuid_length(&dscs[i].dsc_uuid.u);
    }

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    TEST_ASSERT(rc == 0);

    return i;
}

static void
ble_gatt_disc_d_test_misc_rx_rsp(uint16_t conn_handle,
                                 uint16_t end_handle,
                                 struct ble_gatt_disc_d_test_dsc *dscs)
{
    int count;
    int idx;

    idx = 0;
    while (dscs[idx].chr_val_handle != 0) {
        count = ble_gatt_disc_d_test_misc_rx_rsp_once(conn_handle, dscs + idx);
        if (count == 0) {
            break;
        }
        idx += count;
    }

    if (dscs[idx - 1].dsc_handle != end_handle) {
        /* Send the pending ATT Request. */
        ble_hs_test_util_rx_att_err_rsp(conn_handle, BLE_ATT_OP_FIND_INFO_REQ,
                                        BLE_ATT_ERR_ATTR_NOT_FOUND,
                                        end_handle);
    }
}

static void
ble_gatt_disc_d_test_misc_verify_dscs(struct ble_gatt_disc_d_test_dsc *dscs,
                                      int stop_after)
{
    int i;

    if (stop_after == 0) {
        stop_after = BLE_GATT_DISC_D_TEST_MAX_DSCS;
    }

    for (i = 0; i < stop_after && dscs[i].chr_val_handle != 0; i++) {
        TEST_ASSERT(dscs[i].chr_val_handle ==
                    ble_gatt_disc_d_test_dscs[i].chr_val_handle);
        TEST_ASSERT(dscs[i].dsc_handle ==
                    ble_gatt_disc_d_test_dscs[i].dsc_handle);
        TEST_ASSERT(ble_uuid_cmp(&dscs[i].dsc_uuid.u,
                                 &ble_gatt_disc_d_test_dscs[i].dsc_uuid.u) == 0);
    }

    TEST_ASSERT(i == ble_gatt_disc_d_test_num_dscs);
    TEST_ASSERT(ble_gatt_disc_d_test_rx_complete);
}

static int
ble_gatt_disc_d_test_misc_cb(uint16_t conn_handle,
                             const struct ble_gatt_error *error,
                             uint16_t chr_val_handle,
                             const struct ble_gatt_dsc *dsc,
                             void *arg)
{
    struct ble_gatt_disc_d_test_dsc *dst;
    int *stop_after;

    TEST_ASSERT(error != NULL);
    TEST_ASSERT(!ble_gatt_disc_d_test_rx_complete);

    stop_after = arg;

    switch (error->status) {
    case 0:
        TEST_ASSERT_FATAL(ble_gatt_disc_d_test_num_dscs <
                          BLE_GATT_DISC_D_TEST_MAX_DSCS);

        dst = ble_gatt_disc_d_test_dscs + ble_gatt_disc_d_test_num_dscs++;
        dst->chr_val_handle = chr_val_handle;
        dst->dsc_handle = dsc->handle;
        dst->dsc_uuid = dsc->uuid;
        break;

    case BLE_HS_EDONE:
        ble_gatt_disc_d_test_rx_complete = 1;
        break;

    default:
        TEST_ASSERT(0);
        break;
    }

    if (*stop_after > 0) {
        (*stop_after)--;
        if (*stop_after == 0) {
            ble_gatt_disc_d_test_rx_complete = 1;
            return 1;
        }
    }

    return 0;
}

static void
ble_gatt_disc_d_test_misc_all(uint16_t chr_val_handle, uint16_t end_handle,
                              int stop_after,
                              struct ble_gatt_disc_d_test_dsc *dscs)
{
    int num_left;
    int rc;

    ble_gatt_disc_d_test_init();

    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    num_left = stop_after;
    rc = ble_gattc_disc_all_dscs(2, chr_val_handle, end_handle,
                                 ble_gatt_disc_d_test_misc_cb, &num_left);
    TEST_ASSERT(rc == 0);

    ble_gatt_disc_d_test_misc_rx_rsp(2, end_handle, dscs);
    ble_gatt_disc_d_test_misc_verify_dscs(dscs, stop_after);
}

TEST_CASE_SELF(ble_gatt_disc_d_test_1)
{
    /*** One 16-bit descriptor. */
    ble_gatt_disc_d_test_misc_all(5, 10, 0,
        ((struct ble_gatt_disc_d_test_dsc[]) { {
            .chr_val_handle = 5,
            .dsc_handle = 6,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x1234),
        }, {
            0
        } })
    );

    /*** Two 16-bit descriptors. */
    ble_gatt_disc_d_test_misc_all(50, 100, 0,
        ((struct ble_gatt_disc_d_test_dsc[]) { {
            .chr_val_handle = 50,
            .dsc_handle = 51,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x1111),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 52,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x2222),
        }, {
            0
        } })
    );

    /*** Five 16-bit descriptors. */
    ble_gatt_disc_d_test_misc_all(50, 100, 0,
        ((struct ble_gatt_disc_d_test_dsc[]) { {
            .chr_val_handle = 50,
            .dsc_handle = 51,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x1111),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 52,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x2222),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 53,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x3333),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 54,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x4444),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 55,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x5555),
        }, {
            0
        } })
    );

    /*** Interleaved 16-bit and 128-bit descriptors. */
    ble_gatt_disc_d_test_misc_all(50, 100, 0,
        ((struct ble_gatt_disc_d_test_dsc[]) { {
            .chr_val_handle = 50,
            .dsc_handle = 51,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x1111),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 52,
            .dsc_uuid.u128 = BLE_UUID128_INIT( 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 53,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x3333),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 54,
            .dsc_uuid.u128 = BLE_UUID128_INIT(1,0,4,0,6,9,17,7,8,43,7,4,12,43,19,35),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 55,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x5555),
        }, {
            0
        } })
    );

    /*** Ends with final handle ID. */
    ble_gatt_disc_d_test_misc_all(50, 52, 0,
        ((struct ble_gatt_disc_d_test_dsc[]) { {
            .chr_val_handle = 50,
            .dsc_handle = 51,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x1111),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 52,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x2222),
        }, {
            0
        } })
    );

    /*** Stop after two descriptors. */
    ble_gatt_disc_d_test_misc_all(50, 100, 2,
        ((struct ble_gatt_disc_d_test_dsc[]) { {
            .chr_val_handle = 50,
            .dsc_handle = 51,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x1111),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 52,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x2222),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 53,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x3333),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 54,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x4444),
        }, {
            .chr_val_handle = 50,
            .dsc_handle = 55,
            .dsc_uuid.u16 = BLE_UUID16_INIT(0x5555),
        }, {
            0
        } })
    );

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_disc_d_test_oom_all)
{
    struct ble_gatt_disc_d_test_dsc dscs[] = {
        {
            .chr_val_handle = 543,
            .dsc_handle = 548,
            .dsc_uuid.u128 = BLE_UUID128_INIT(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
        },
        {
            .chr_val_handle = 543,
            .dsc_handle = 549,
            .dsc_uuid.u128 = BLE_UUID128_INIT(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
        },
        { 0 }
    };

    struct os_mbuf *oms;
    int32_t ticks_until;
    int stop_after;
    int num_dscs;
    int rc;

    ble_gatt_disc_d_test_init();

    ble_hs_test_util_create_conn(1, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /* Initiate a discover all characteristics procedure. */
    stop_after = 0;
    rc = ble_gattc_disc_all_dscs(1, 543, 560,
                                 ble_gatt_disc_d_test_misc_cb, &stop_after);
    TEST_ASSERT_FATAL(rc == 0);

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    num_dscs = ble_gatt_disc_d_test_misc_rx_rsp_once(1, dscs);

    /* Make sure there are still undiscovered services. */
    TEST_ASSERT_FATAL(num_dscs < sizeof dscs / sizeof dscs[0] - 1);

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
    ble_gatt_disc_d_test_misc_rx_rsp_once(1, dscs + num_dscs);

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

    ble_hs_test_util_rx_att_err_rsp(1,
                                    BLE_ATT_OP_READ_TYPE_REQ,
                                    BLE_ATT_ERR_ATTR_NOT_FOUND,
                                    1);
    ble_gatt_disc_d_test_misc_verify_dscs(dscs, 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_gatt_disc_d_test_suite)
{
    ble_gatt_disc_d_test_1();
    ble_gatt_disc_d_test_oom_all();
}
