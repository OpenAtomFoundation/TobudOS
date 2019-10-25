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

struct ble_gatt_disc_s_test_svc {
    uint16_t start_handle;
    uint16_t end_handle;
    const ble_uuid_t *uuid;
};

#define BLE_GATT_DISC_S_TEST_MAX_SERVICES  256
static struct ble_gatt_svc
    ble_gatt_disc_s_test_svcs[BLE_GATT_DISC_S_TEST_MAX_SERVICES];
static int ble_gatt_disc_s_test_num_svcs;
static int ble_gatt_disc_s_test_rx_complete;

static void
ble_gatt_disc_s_test_init(void)
{
    ble_hs_test_util_init();
    ble_gatt_disc_s_test_num_svcs = 0;
    ble_gatt_disc_s_test_rx_complete = 0;
}

static int
ble_gatt_disc_s_test_misc_svc_length(struct ble_gatt_disc_s_test_svc *service)
{
    if (service->uuid->type == BLE_UUID_TYPE_16) {
        return 6;
    } else {
        return 20;
    }
}

static int
ble_gatt_disc_s_test_misc_rx_all_rsp_once(
    uint16_t conn_handle, struct ble_gatt_disc_s_test_svc *services)
{
    struct ble_att_read_group_type_rsp rsp;
    uint8_t buf[1024];
    int off;
    int rc;
    int i;

    /* Send the pending ATT Read By Group Type Request. */

    rsp.bagp_length = ble_gatt_disc_s_test_misc_svc_length(services);
    ble_att_read_group_type_rsp_write(buf, BLE_ATT_READ_GROUP_TYPE_RSP_BASE_SZ,
                                      &rsp);

    off = BLE_ATT_READ_GROUP_TYPE_RSP_BASE_SZ;
    for (i = 0; ; i++) {
        if (services[i].start_handle == 0) {
            /* No more services. */
            break;
        }

        rc = ble_gatt_disc_s_test_misc_svc_length(services + i);
        if (rc != rsp.bagp_length) {
            /* UUID length is changing; Need a separate response. */
            break;
        }

        if (services[i].uuid->type == BLE_UUID_TYPE_16) {
            if (off + BLE_ATT_READ_GROUP_TYPE_ADATA_SZ_16 >
                ble_att_mtu(conn_handle)) {

                /* Can't fit any more entries. */
                break;
            }
        } else {
            if (off + BLE_ATT_READ_GROUP_TYPE_ADATA_SZ_128 >
                ble_att_mtu(conn_handle)) {

                /* Can't fit any more entries. */
                break;
            }
        }

        put_le16(buf + off, services[i].start_handle);
        off += 2;

        put_le16(buf + off, services[i].end_handle);
        off += 2;

        ble_uuid_flat(services[i].uuid, buf + off);
        off += ble_uuid_length(services[i].uuid);
    }

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    TEST_ASSERT(rc == 0);

    return i;
}

static void
ble_gatt_disc_s_test_misc_rx_all_rsp(
    uint16_t conn_handle, struct ble_gatt_disc_s_test_svc *services)
{
    int count;
    int idx;

    idx = 0;
    while (services[idx].start_handle != 0) {
        count = ble_gatt_disc_s_test_misc_rx_all_rsp_once(conn_handle,
                                                          services + idx);
        idx += count;
    }

    if (services[idx - 1].end_handle != 0xffff) {
        /* Send the pending ATT Request. */
        ble_hs_test_util_rx_att_err_rsp(conn_handle,
                                        BLE_ATT_OP_READ_GROUP_TYPE_REQ,
                                        BLE_ATT_ERR_ATTR_NOT_FOUND,
                                        services[idx - 1].start_handle);
    }
}

static int
ble_gatt_disc_s_test_misc_rx_uuid_rsp_once(
    uint16_t conn_handle, struct ble_gatt_disc_s_test_svc *services)
{
    uint8_t buf[1024];
    int off;
    int rc;
    int i;

    /* Send the pending ATT Find By Type Value Request. */

    buf[0] = BLE_ATT_OP_FIND_TYPE_VALUE_RSP;
    off = BLE_ATT_FIND_TYPE_VALUE_RSP_BASE_SZ;
    for (i = 0; ; i++) {
        if (services[i].start_handle == 0) {
            /* No more services. */
            break;
        }

        if (off + BLE_ATT_FIND_TYPE_VALUE_HINFO_BASE_SZ >
            ble_att_mtu(conn_handle)) {

            /* Can't fit any more entries. */
            break;
        }

        put_le16(buf + off, services[i].start_handle);
        off += 2;

        put_le16(buf + off, services[i].end_handle);
        off += 2;
    }

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    TEST_ASSERT(rc == 0);

    return i;
}

static void
ble_gatt_disc_s_test_misc_rx_uuid_rsp(
    uint16_t conn_handle, struct ble_gatt_disc_s_test_svc *services)
{
    int count;
    int idx;

    idx = 0;
    while (services[idx].start_handle != 0) {
        count = ble_gatt_disc_s_test_misc_rx_uuid_rsp_once(conn_handle,
                                                           services + idx);
        idx += count;
    }

    if (services[idx - 1].end_handle != 0xffff) {
        /* Send the pending ATT Request. */
        ble_hs_test_util_rx_att_err_rsp(conn_handle,
                                        BLE_ATT_OP_FIND_TYPE_VALUE_REQ,
                                        BLE_ATT_ERR_ATTR_NOT_FOUND,
                                        services[idx - 1].start_handle);
    }
}

static void
ble_gatt_disc_s_test_misc_verify_services(
    struct ble_gatt_disc_s_test_svc *services)
{
    int i;

    for (i = 0; services[i].start_handle != 0; i++) {
        TEST_ASSERT(services[i].start_handle ==
                    ble_gatt_disc_s_test_svcs[i].start_handle);
        TEST_ASSERT(services[i].end_handle ==
                    ble_gatt_disc_s_test_svcs[i].end_handle);

        TEST_ASSERT(ble_uuid_cmp(services[i].uuid,
                    &ble_gatt_disc_s_test_svcs[i].uuid.u) == 0);
    }

    TEST_ASSERT(i == ble_gatt_disc_s_test_num_svcs);
    TEST_ASSERT(ble_gatt_disc_s_test_rx_complete);
}

static int
ble_gatt_disc_s_test_misc_disc_cb(uint16_t conn_handle,
                                  const struct ble_gatt_error *error,
                                  const struct ble_gatt_svc *service,
                                  void *arg)
{
    TEST_ASSERT(error != NULL);
    TEST_ASSERT(!ble_gatt_disc_s_test_rx_complete);

    switch (error->status) {
    case 0:
        TEST_ASSERT(service != NULL);
        TEST_ASSERT_FATAL(ble_gatt_disc_s_test_num_svcs <
                          BLE_GATT_DISC_S_TEST_MAX_SERVICES);
        ble_gatt_disc_s_test_svcs[ble_gatt_disc_s_test_num_svcs++] = *service;
        break;

    case BLE_HS_EDONE:
        TEST_ASSERT(service == NULL);
        ble_gatt_disc_s_test_rx_complete = 1;
        break;

    case BLE_HS_ETIMEOUT:
        ble_gatt_disc_s_test_rx_complete = 1;
        break;

    default:
        TEST_ASSERT(0);
    }

    return 0;
}

static void
ble_gatt_disc_s_test_misc_good_all(struct ble_gatt_disc_s_test_svc *services)
{
    int rc;

    ble_gatt_disc_s_test_init();

    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    rc = ble_gattc_disc_all_svcs(2, ble_gatt_disc_s_test_misc_disc_cb, NULL);
    TEST_ASSERT(rc == 0);

    ble_gatt_disc_s_test_misc_rx_all_rsp(2, services);
    ble_gatt_disc_s_test_misc_verify_services(services);
}

static void
ble_gatt_disc_s_test_misc_good_uuid(
    struct ble_gatt_disc_s_test_svc *services)
{
    int rc;

    ble_gatt_disc_s_test_init();

    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    rc = ble_gattc_disc_svc_by_uuid(2, services[0].uuid,
                                    ble_gatt_disc_s_test_misc_disc_cb, NULL);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_verify_tx_disc_svc_uuid(services[0].uuid);

    ble_gatt_disc_s_test_misc_rx_uuid_rsp(2, services);
    ble_gatt_disc_s_test_misc_verify_services(services);
}

TEST_CASE_SELF(ble_gatt_disc_s_test_disc_all)
{
    /*** One 128-bit service. */
    ble_gatt_disc_s_test_misc_good_all((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 0 }
    });

    /*** Two 128-bit services. */
    ble_gatt_disc_s_test_misc_good_all((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 10, 50,   BLE_UUID128_DECLARE(2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ), },
        { 0 }
    });

    /*** Five 128-bit services. */
    ble_gatt_disc_s_test_misc_good_all((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 10, 50,   BLE_UUID128_DECLARE(2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ), },
        { 80, 120,  BLE_UUID128_DECLARE(3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 ), },
        { 123, 678, BLE_UUID128_DECLARE(4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 ), },
        { 751, 999, BLE_UUID128_DECLARE(5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 ), },
        { 0 }
    });

    /*** One 128-bit service, one 16-bit-service. */
    ble_gatt_disc_s_test_misc_good_all((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 6, 7,     BLE_UUID16_DECLARE(0x1234) },
        { 0 }
    });

    /*** End with handle 0xffff. */
    ble_gatt_disc_s_test_misc_good_all((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 7, 0xffff,BLE_UUID128_DECLARE(2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ), },
        { 0 }
    });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_disc_s_test_disc_uuid)
{
    /*** 128-bit service; one entry. */
    ble_gatt_disc_s_test_misc_good_uuid((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 0 }
    });

    /*** 128-bit service; two entries. */
    ble_gatt_disc_s_test_misc_good_uuid((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 8, 43,    BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 0 }
    });

    /*** 128-bit service; five entries. */
    ble_gatt_disc_s_test_misc_good_uuid((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 8, 43,    BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 67, 100,  BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 102, 103, BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 262, 900, BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 0 }
    });

    /*** 128-bit service; end with handle 0xffff. */
    ble_gatt_disc_s_test_misc_good_uuid((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 7, 0xffff,BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 0 }
    });

    /*** 16-bit service; one entry. */
    ble_gatt_disc_s_test_misc_good_uuid((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID16_DECLARE(0x1234) },
        { 0 }
    });

    /*** 16-bit service; two entries. */
    ble_gatt_disc_s_test_misc_good_uuid((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID16_DECLARE(0x1234) },
        { 85, 243,  BLE_UUID16_DECLARE(0x1234) },
        { 0 }
    });

    /*** 16-bit service; five entries. */
    ble_gatt_disc_s_test_misc_good_uuid((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID16_DECLARE(0x1234) },
        { 85, 243,  BLE_UUID16_DECLARE(0x1234) },
        { 382, 383, BLE_UUID16_DECLARE(0x1234) },
        { 562, 898, BLE_UUID16_DECLARE(0x1234) },
        { 902, 984, BLE_UUID16_DECLARE(0x1234) },
        { 0 }
    });

    /*** 16-bit service; end with handle 0xffff. */
    ble_gatt_disc_s_test_misc_good_uuid((struct ble_gatt_disc_s_test_svc[]) {
        { 1, 5,     BLE_UUID16_DECLARE(0x1234) },
        { 9, 0xffff,BLE_UUID16_DECLARE(0x1234) },
        { 0 }
    });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_disc_s_test_oom_all)
{
    struct ble_gatt_disc_s_test_svc svcs[] = {
        { 1, 5,     BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 6, 10,    BLE_UUID128_DECLARE(2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ), },
        { 0 },
    };

    struct os_mbuf *oms;
    int32_t ticks_until;
    int num_svcs;
    int rc;

    ble_gatt_disc_s_test_init();

    ble_hs_test_util_create_conn(1, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /* Initiate a discover all services procedure. */
    rc = ble_gattc_disc_all_svcs(1, ble_gatt_disc_s_test_misc_disc_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    num_svcs = ble_gatt_disc_s_test_misc_rx_all_rsp_once(1, svcs);

    /* Make sure there are still undiscovered services. */
    TEST_ASSERT_FATAL(num_svcs < sizeof svcs / sizeof svcs[0] - 1);

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
    ble_gatt_disc_s_test_misc_rx_all_rsp_once(1, svcs + num_svcs);

    /* Ensure no follow-up request got sent.  It should not have gotten sent
     * due to mbuf exhaustion.
     */
    ble_hs_test_util_prev_tx_queue_clear();
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue_pullup() == NULL);

    /* Verify that we will resume the stalled GATT procedure in one second. */
    ticks_until = ble_gattc_timer();
    TEST_ASSERT(ticks_until == os_time_ms_to_ticks32(MYNEWT_VAL(BLE_GATT_RESUME_RATE)));

    rc = os_mbuf_free_chain(oms);
    TEST_ASSERT_FATAL(rc == 0);
    os_time_advance(ticks_until);
    ble_gattc_timer();

    ble_hs_test_util_rx_att_err_rsp(1,
                                    BLE_ATT_OP_READ_GROUP_TYPE_REQ,
                                    BLE_ATT_ERR_ATTR_NOT_FOUND,
                                    1);
    ble_gatt_disc_s_test_misc_verify_services(svcs);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_disc_s_test_oom_uuid)
{
    /* Retrieve enough services to require two transactions. */
    struct ble_gatt_disc_s_test_svc svcs[] = {
        { 1, 5,   BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 6, 10,  BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 11, 15, BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 16, 20, BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 21, 25, BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 26, 30, BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 0 },
    };

    struct os_mbuf *oms;
    int32_t ticks_until;
    int num_svcs;
    int rc;

    ble_gatt_disc_s_test_init();

    ble_hs_test_util_create_conn(1, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /* Initiate a discover all services procedure. */
    rc = ble_gattc_disc_svc_by_uuid(1, svcs[0].uuid,
                                    ble_gatt_disc_s_test_misc_disc_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    num_svcs = ble_gatt_disc_s_test_misc_rx_uuid_rsp_once(1, svcs);

    /* Make sure there are still undiscovered services. */
    TEST_ASSERT_FATAL(num_svcs < sizeof svcs / sizeof svcs[0] - 1);

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
    ble_gatt_disc_s_test_misc_rx_uuid_rsp_once(1, svcs + num_svcs);

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
                                    BLE_ATT_OP_READ_GROUP_TYPE_REQ,
                                    BLE_ATT_ERR_ATTR_NOT_FOUND,
                                    1);
    ble_gatt_disc_s_test_misc_verify_services(svcs);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatt_disc_s_test_oom_timeout)
{
    struct ble_gatt_disc_s_test_svc svcs[] = {
        { 1, 5,  BLE_UUID128_DECLARE(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ), },
        { 6, 10, BLE_UUID128_DECLARE(2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ), },
        { 0 },
    };

    struct os_mbuf *oms_temp;
    struct os_mbuf *oms;
    int32_t ticks_until;
    int rc;
    int i;

    ble_gatt_disc_s_test_init();

    ble_hs_test_util_create_conn(1, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 NULL, NULL);

    /* Initiate a discover all services procedure. */
    rc = ble_gattc_disc_all_svcs(1, ble_gatt_disc_s_test_misc_disc_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    /* Exhaust the msys pool.  Leave one mbuf for the forthcoming response. */
    oms = ble_hs_test_util_mbuf_alloc_all_but(1);
    ble_gatt_disc_s_test_misc_rx_all_rsp_once(1, svcs);

    /* Keep trying to resume for 30 seconds, but never free any mbufs.  Verify
     * procedure eventually times out.
     */
    for (i = 0; i < 30; i++) {
        /* Ensure no follow-up request got sent.  It should not have gotten
         * sent due to mbuf exhaustion.
         */
        ble_hs_test_util_prev_tx_queue_clear();
        TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue_pullup() == NULL);

        oms_temp = ble_hs_test_util_mbuf_alloc_all_but(0);
        if (oms_temp != NULL) {
            os_mbuf_concat(oms, oms_temp);
        }

        /* Verify that we will resume the stalled GATT procedure in one
         * second.
         */
        ticks_until = ble_gattc_timer();
        TEST_ASSERT(ticks_until == os_time_ms_to_ticks32(MYNEWT_VAL(BLE_GATT_RESUME_RATE)));

        os_time_advance(ticks_until);
    }

    /* Verify the procedure has timed out.  The connection should now be
     * in the process of being terminated.  XXX: Check this.
     */
    ble_hs_test_util_hci_ack_set_disconnect(0);
    ble_gattc_timer();

    ticks_until = ble_gattc_timer();
    TEST_ASSERT(ticks_until == BLE_HS_FOREVER);
    TEST_ASSERT(!ble_gattc_any_jobs());

    rc = os_mbuf_free_chain(oms);
    TEST_ASSERT_FATAL(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_gatt_disc_s_test_suite)
{
    ble_gatt_disc_s_test_disc_all();
    ble_gatt_disc_s_test_disc_uuid();
    ble_gatt_disc_s_test_oom_all();
    ble_gatt_disc_s_test_oom_uuid();
    ble_gatt_disc_s_test_oom_timeout();
}
