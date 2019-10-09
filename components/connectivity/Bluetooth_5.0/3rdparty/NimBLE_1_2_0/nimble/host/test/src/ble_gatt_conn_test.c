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

#define BLE_GATT_BREAK_TEST_READ_ATTR_HANDLE        0x9383
#define BLE_GATT_BREAK_TEST_WRITE_ATTR_HANDLE       0x1234

static uint8_t ble_gatt_conn_test_write_value[] = { 1, 3, 64, 21, 6 };

struct ble_gatt_conn_test_arg {
    uint16_t exp_conn_handle;
    int exp_status;
    int called;
};

static struct ble_gap_event ble_gatt_conn_test_gap_event;

static void
ble_gatt_conn_test_util_init(void)
{
    ble_hs_test_util_init();
    memset(&ble_gatt_conn_test_gap_event, -1,
           sizeof ble_gatt_conn_test_gap_event);
}

static int
ble_gatt_conn_test_indicate_cb(struct ble_gap_event *event, void *arg)
{
    /* Only record indication failures. */
    if (event->type == BLE_GAP_EVENT_NOTIFY_TX &&
        event->notify_tx.status != 0) {

        ble_gatt_conn_test_gap_event = *event;
    }
    return 0;
}

static int
ble_gatt_conn_test_attr_cb(uint16_t conn_handle, uint16_t attr_handle,
                           uint8_t op, uint16_t offset, struct os_mbuf **om,
                           void *arg)
{
    uint8_t *buf;

    switch (op) {
    case BLE_ATT_ACCESS_OP_READ:
        buf = os_mbuf_extend(*om, 1);
        TEST_ASSERT_FATAL(buf != NULL);
        *buf = 1;
        return 0;

    default:
        return -1;
    }
}

static int
ble_gatt_conn_test_mtu_cb(uint16_t conn_handle,
                          const struct ble_gatt_error *error,
                          uint16_t mtu, void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(mtu == 0);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_disc_all_svcs_cb(uint16_t conn_handle,
                                    const struct ble_gatt_error *error,
                                    const struct ble_gatt_svc *service,
                                    void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(service == NULL);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_disc_svc_uuid_cb(uint16_t conn_handle,
                                    const struct ble_gatt_error *error,
                                    const struct ble_gatt_svc *service,
                                    void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(service == NULL);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_find_inc_svcs_cb(uint16_t conn_handle,
                                    const struct ble_gatt_error *error,
                                    const struct ble_gatt_svc *service,
                                    void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(service == NULL);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_disc_all_chrs_cb(uint16_t conn_handle,
                                    const struct ble_gatt_error *error,
                                    const struct ble_gatt_chr *chr, void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(chr == NULL);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_disc_chr_uuid_cb(uint16_t conn_handle,
                                    const struct ble_gatt_error *error,
                                    const struct ble_gatt_chr *chr, void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(chr == NULL);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_disc_all_dscs_cb(uint16_t conn_handle,
                                    const struct ble_gatt_error *error,
                                    uint16_t chr_val_handle,
                                    const struct ble_gatt_dsc *dsc,
                                    void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(dsc == NULL);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_read_cb(uint16_t conn_handle,
                           const struct ble_gatt_error *error,
                           struct ble_gatt_attr *attr, void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(attr == NULL);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_read_uuid_cb(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                struct ble_gatt_attr *attr, void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(attr == NULL);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_read_long_cb(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                struct ble_gatt_attr *attr, void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(attr == NULL);

    cb_arg->called++;

    return 0;
}
static int
ble_gatt_conn_test_read_mult_cb(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                struct ble_gatt_attr *attr, void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(attr->om == NULL);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_write_cb(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            struct ble_gatt_attr *attr,
                            void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(attr != NULL);
    TEST_ASSERT(attr->handle == BLE_GATT_BREAK_TEST_WRITE_ATTR_HANDLE);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_write_long_cb(uint16_t conn_handle,
                                 const struct ble_gatt_error *error,
                                 struct ble_gatt_attr *attr, void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(attr != NULL);
    TEST_ASSERT(attr->handle == BLE_GATT_BREAK_TEST_WRITE_ATTR_HANDLE);

    cb_arg->called++;

    return 0;
}

static int
ble_gatt_conn_test_write_rel_cb(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                struct ble_gatt_attr *attrs,
                                uint8_t num_attrs,
                                void *arg)
{
    struct ble_gatt_conn_test_arg *cb_arg;

    cb_arg = arg;

    TEST_ASSERT(cb_arg->exp_conn_handle == conn_handle);
    TEST_ASSERT(!cb_arg->called);
    TEST_ASSERT_FATAL(error != NULL);
    TEST_ASSERT(error->status == cb_arg->exp_status);
    TEST_ASSERT(attrs != NULL);

    cb_arg->called++;

    return 0;
}

TEST_CASE_SELF(ble_gatt_conn_test_disconnect)
{
    struct ble_gatt_conn_test_arg mtu_arg            = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg disc_all_svcs_arg  = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg disc_svc_uuid_arg  = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg find_inc_svcs_arg  = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg disc_all_chrs_arg  = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg disc_chr_uuid_arg  = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg disc_all_dscs_arg  = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg read_arg           = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg read_uuid_arg      = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg read_long_arg      = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg read_mult_arg      = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg write_arg          = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg write_long_arg     = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_conn_test_arg write_rel_arg      = { 0, BLE_HS_ENOTCONN };
    struct ble_gatt_attr attr;
    uint16_t attr_handle;
    uint16_t offset = 0;
    int rc;

    ble_gatt_conn_test_util_init();

    /*** Register an attribute to allow indicatations to be sent. */
    rc = ble_att_svr_register(BLE_UUID16_DECLARE(0x1212), BLE_ATT_F_READ, 0,
                              &attr_handle,
                              ble_gatt_conn_test_attr_cb, NULL);
    TEST_ASSERT(rc == 0);

    /* Create three connections. */
    ble_hs_test_util_create_conn(1, ((uint8_t[]){1,2,3,4,5,6,7,8}),
                                 ble_gatt_conn_test_indicate_cb, NULL);
    ble_hs_test_util_create_conn(2, ((uint8_t[]){2,3,4,5,6,7,8,9}),
                                 ble_gatt_conn_test_indicate_cb, NULL);
    ble_hs_test_util_create_conn(3, ((uint8_t[]){3,4,5,6,7,8,9,10}),
                                 ble_gatt_conn_test_indicate_cb, NULL);

    /*** Schedule some GATT procedures. */
    /* Connection 1. */
    mtu_arg.exp_conn_handle = 1;
    ble_gattc_exchange_mtu(1, ble_gatt_conn_test_mtu_cb, &mtu_arg);

    disc_all_svcs_arg.exp_conn_handle = 1;
    rc = ble_gattc_disc_all_svcs(1, ble_gatt_conn_test_disc_all_svcs_cb,
                                 &disc_all_svcs_arg);
    TEST_ASSERT_FATAL(rc == 0);

    disc_svc_uuid_arg.exp_conn_handle = 1;
    rc = ble_gattc_disc_svc_by_uuid(1, BLE_UUID16_DECLARE(0x1111),
                                    ble_gatt_conn_test_disc_svc_uuid_cb,
                                    &disc_svc_uuid_arg);
    TEST_ASSERT_FATAL(rc == 0);

    find_inc_svcs_arg.exp_conn_handle = 1;
    rc = ble_gattc_find_inc_svcs(1, 1, 0xffff,
                                 ble_gatt_conn_test_find_inc_svcs_cb,
                                 &find_inc_svcs_arg);
    TEST_ASSERT_FATAL(rc == 0);

    disc_all_chrs_arg.exp_conn_handle = 1;
    rc = ble_gattc_disc_all_chrs(1, 1, 0xffff,
                                 ble_gatt_conn_test_disc_all_chrs_cb,
                                 &disc_all_chrs_arg);
    TEST_ASSERT_FATAL(rc == 0);

    /* Connection 2. */
    disc_all_dscs_arg.exp_conn_handle = 2;
    rc = ble_gattc_disc_all_dscs(2, 3, 0xffff,
                                 ble_gatt_conn_test_disc_all_dscs_cb,
                                 &disc_all_dscs_arg);

    disc_chr_uuid_arg.exp_conn_handle = 2;
    rc = ble_gattc_disc_chrs_by_uuid(2, 2, 0xffff, BLE_UUID16_DECLARE(0x2222),
                                     ble_gatt_conn_test_disc_chr_uuid_cb,
                                     &disc_chr_uuid_arg);

    read_arg.exp_conn_handle = 2;
    rc = ble_gattc_read(2, BLE_GATT_BREAK_TEST_READ_ATTR_HANDLE,
                        ble_gatt_conn_test_read_cb, &read_arg);
    TEST_ASSERT_FATAL(rc == 0);

    read_uuid_arg.exp_conn_handle = 2;
    rc = ble_gattc_read_by_uuid(2, 1, 0xffff, BLE_UUID16_DECLARE(0x3333),
                                ble_gatt_conn_test_read_uuid_cb,
                                &read_uuid_arg);
    TEST_ASSERT_FATAL(rc == 0);

    read_long_arg.exp_conn_handle = 2;
    rc = ble_gattc_read_long(2, BLE_GATT_BREAK_TEST_READ_ATTR_HANDLE, offset,
                             ble_gatt_conn_test_read_long_cb, &read_long_arg);
    TEST_ASSERT_FATAL(rc == 0);

    /* Connection 3. */
    read_mult_arg.exp_conn_handle = 3;
    rc = ble_gattc_read_mult(3, ((uint16_t[3]){5,6,7}), 3,
                             ble_gatt_conn_test_read_mult_cb, &read_mult_arg);
    TEST_ASSERT_FATAL(rc == 0);

    write_arg.exp_conn_handle = 3;
    rc = ble_hs_test_util_gatt_write_flat(
        3, BLE_GATT_BREAK_TEST_WRITE_ATTR_HANDLE,
        ble_gatt_conn_test_write_value, sizeof ble_gatt_conn_test_write_value,
        ble_gatt_conn_test_write_cb, &write_arg);
    TEST_ASSERT_FATAL(rc == 0);

    write_long_arg.exp_conn_handle = 3;
    rc = ble_hs_test_util_gatt_write_long_flat(
        3, BLE_GATT_BREAK_TEST_WRITE_ATTR_HANDLE,
        ble_gatt_conn_test_write_value, sizeof ble_gatt_conn_test_write_value,
        ble_gatt_conn_test_write_long_cb, &write_long_arg);
    TEST_ASSERT_FATAL(rc == 0);

    attr.handle = 8;
    attr.offset = 0;
    attr.om = os_msys_get_pkthdr(0, 0);
    write_rel_arg.exp_conn_handle = 3;
    rc = ble_gattc_write_reliable(
        3, &attr, 1, ble_gatt_conn_test_write_rel_cb, &write_rel_arg);
    TEST_ASSERT_FATAL(rc == 0);

    rc = ble_gattc_indicate(3, attr_handle);
    TEST_ASSERT_FATAL(rc == 0);

    /*** Start the procedures. */

    /*** Break the connections; verify proper callbacks got called. */
    /* Connection 1. */
    ble_gattc_connection_broken(1);
    TEST_ASSERT(mtu_arg.called == 1);
    TEST_ASSERT(disc_all_svcs_arg.called == 1);
    TEST_ASSERT(disc_svc_uuid_arg.called == 1);
    TEST_ASSERT(find_inc_svcs_arg.called == 1);
    TEST_ASSERT(disc_all_chrs_arg.called == 1);
    TEST_ASSERT(disc_chr_uuid_arg.called == 0);
    TEST_ASSERT(disc_all_dscs_arg.called == 0);
    TEST_ASSERT(read_arg.called == 0);
    TEST_ASSERT(read_uuid_arg.called == 0);
    TEST_ASSERT(read_long_arg.called == 0);
    TEST_ASSERT(read_mult_arg.called == 0);
    TEST_ASSERT(write_arg.called == 0);
    TEST_ASSERT(write_long_arg.called == 0);
    TEST_ASSERT(write_rel_arg.called == 0);
    TEST_ASSERT(ble_gatt_conn_test_gap_event.type == 255);

    /* Connection 2. */
    ble_gattc_connection_broken(2);
    TEST_ASSERT(mtu_arg.called == 1);
    TEST_ASSERT(disc_all_svcs_arg.called == 1);
    TEST_ASSERT(disc_svc_uuid_arg.called == 1);
    TEST_ASSERT(find_inc_svcs_arg.called == 1);
    TEST_ASSERT(disc_all_chrs_arg.called == 1);
    TEST_ASSERT(disc_chr_uuid_arg.called == 1);
    TEST_ASSERT(disc_all_dscs_arg.called == 1);
    TEST_ASSERT(read_arg.called == 1);
    TEST_ASSERT(read_uuid_arg.called == 1);
    TEST_ASSERT(read_long_arg.called == 1);
    TEST_ASSERT(read_mult_arg.called == 0);
    TEST_ASSERT(write_arg.called == 0);
    TEST_ASSERT(write_long_arg.called == 0);
    TEST_ASSERT(write_rel_arg.called == 0);
    TEST_ASSERT(ble_gatt_conn_test_gap_event.type == 255);

    /* Connection 3. */
    ble_gattc_connection_broken(3);
    TEST_ASSERT(mtu_arg.called == 1);
    TEST_ASSERT(disc_all_svcs_arg.called == 1);
    TEST_ASSERT(disc_svc_uuid_arg.called == 1);
    TEST_ASSERT(find_inc_svcs_arg.called == 1);
    TEST_ASSERT(disc_all_chrs_arg.called == 1);
    TEST_ASSERT(disc_chr_uuid_arg.called == 1);
    TEST_ASSERT(disc_all_dscs_arg.called == 1);
    TEST_ASSERT(read_arg.called == 1);
    TEST_ASSERT(read_uuid_arg.called == 1);
    TEST_ASSERT(read_long_arg.called == 1);
    TEST_ASSERT(read_mult_arg.called == 1);
    TEST_ASSERT(write_arg.called == 1);
    TEST_ASSERT(write_long_arg.called == 1);
    TEST_ASSERT(write_rel_arg.called == 1);
    TEST_ASSERT(ble_gatt_conn_test_gap_event.type == BLE_GAP_EVENT_NOTIFY_TX);
    TEST_ASSERT(ble_gatt_conn_test_gap_event.notify_tx.status ==
                BLE_HS_ENOTCONN);
    TEST_ASSERT(ble_gatt_conn_test_gap_event.notify_tx.conn_handle == 3);
    TEST_ASSERT(ble_gatt_conn_test_gap_event.notify_tx.attr_handle ==
                attr_handle);
    TEST_ASSERT(ble_gatt_conn_test_gap_event.notify_tx.indication);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

static void
ble_gatt_conn_test_util_timeout(uint16_t conn_handle,
                                struct ble_gatt_conn_test_arg *arg)
{
    struct hci_disconn_complete evt;
    int32_t ticks_from_now;

    ticks_from_now = ble_gattc_timer();
    TEST_ASSERT(ticks_from_now == 30 * OS_TICKS_PER_SEC);

    os_time_advance(29 * OS_TICKS_PER_SEC);
    ticks_from_now = ble_gattc_timer();
    TEST_ASSERT(ticks_from_now == 1 * OS_TICKS_PER_SEC);

    ble_hs_test_util_hci_ack_set_disconnect(0);
    os_time_advance(1 * OS_TICKS_PER_SEC);
    ticks_from_now = ble_gattc_timer();
    TEST_ASSERT(ticks_from_now == BLE_HS_FOREVER);

    /* Ensure connection was terminated due to proecedure timeout. */
    evt.connection_handle = conn_handle;
    evt.status = 0;
    evt.reason = BLE_ERR_REM_USER_CONN_TERM;
    ble_hs_test_util_hci_rx_disconn_complete_event(&evt);

    /* Ensure GATT callback was called with timeout status. */
    if (arg != NULL) {
        TEST_ASSERT(arg->called == 1);
    }
}

TEST_CASE_SELF(ble_gatt_conn_test_timeout)
{
    static const uint8_t peer_addr[6] = { 1, 2, 3, 4, 5, 6 };

    struct ble_gatt_conn_test_arg mtu_arg            = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg disc_all_svcs_arg  = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg disc_svc_uuid_arg  = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg find_inc_svcs_arg  = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg disc_all_chrs_arg  = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg disc_chr_uuid_arg  = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg disc_all_dscs_arg  = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg read_arg           = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg read_uuid_arg      = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg read_long_arg      = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg read_mult_arg      = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg write_arg          = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg write_long_arg     = { 1, BLE_HS_ETIMEOUT };
    struct ble_gatt_conn_test_arg write_rel_arg      = { 1, BLE_HS_ETIMEOUT };

    struct ble_gatt_attr attr;
    int32_t ticks_from_now;
    uint16_t attr_handle;
    uint16_t offset = 0;
    int rc;

    ble_gatt_conn_test_util_init();

    ticks_from_now = ble_gattc_timer();
    TEST_ASSERT(ticks_from_now == BLE_HS_FOREVER);

    /*** Register an attribute to allow indicatations to be sent. */
    rc = ble_att_svr_register(BLE_UUID16_DECLARE(0x1212), BLE_ATT_F_READ, 0,
                              &attr_handle,
                              ble_gatt_conn_test_attr_cb, NULL);
    TEST_ASSERT(rc == 0);

    /*** MTU. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_exchange_mtu(1, ble_gatt_conn_test_mtu_cb, &mtu_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &mtu_arg);

    /*** Discover all services. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_disc_all_svcs(1, ble_gatt_conn_test_disc_all_svcs_cb,
                                 &disc_all_svcs_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &disc_all_svcs_arg);

    /*** Discover services by UUID. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_disc_svc_by_uuid(1, BLE_UUID16_DECLARE(0x1111),
                                    ble_gatt_conn_test_disc_svc_uuid_cb,
                                    &disc_svc_uuid_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &disc_svc_uuid_arg);

    /*** Find included services. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_find_inc_svcs(1, 1, 0xffff,
                                 ble_gatt_conn_test_find_inc_svcs_cb,
                                 &find_inc_svcs_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &find_inc_svcs_arg);

    /*** Discover all characteristics. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_disc_all_chrs(1, 1, 0xffff,
                                 ble_gatt_conn_test_disc_all_chrs_cb,
                                 &disc_all_chrs_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &disc_all_chrs_arg);

    /*** Discover all descriptors. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_disc_all_dscs(1, 3, 0xffff,
                                 ble_gatt_conn_test_disc_all_dscs_cb,
                                 &disc_chr_uuid_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &disc_chr_uuid_arg);

    /*** Discover characteristics by UUID. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_disc_chrs_by_uuid(1, 2, 0xffff, BLE_UUID16_DECLARE(0x2222),
                                     ble_gatt_conn_test_disc_chr_uuid_cb,
                                     &disc_all_dscs_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &disc_all_dscs_arg);

    /*** Read. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_read(1, BLE_GATT_BREAK_TEST_READ_ATTR_HANDLE,
                        ble_gatt_conn_test_read_cb, &read_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &read_arg);

    /*** Read by UUID. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_read_by_uuid(1, 1, 0xffff, BLE_UUID16_DECLARE(0x3333),
                                ble_gatt_conn_test_read_uuid_cb,
                                &read_uuid_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &read_uuid_arg);

    /*** Read long. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_read_long(1, BLE_GATT_BREAK_TEST_READ_ATTR_HANDLE, offset,
                             ble_gatt_conn_test_read_long_cb,
                             &read_long_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &read_long_arg);

    /*** Read multiple. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_read_mult(1, ((uint16_t[3]){5,6,7}), 3,
                             ble_gatt_conn_test_read_mult_cb,
                             &read_mult_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &read_mult_arg);

    /*** Write. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_hs_test_util_gatt_write_flat(
        1, BLE_GATT_BREAK_TEST_WRITE_ATTR_HANDLE,
        ble_gatt_conn_test_write_value, sizeof ble_gatt_conn_test_write_value,
        ble_gatt_conn_test_write_cb, &write_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &write_arg);

    /*** Write long. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_hs_test_util_gatt_write_long_flat(
        1, BLE_GATT_BREAK_TEST_WRITE_ATTR_HANDLE,
        ble_gatt_conn_test_write_value, sizeof ble_gatt_conn_test_write_value,
        ble_gatt_conn_test_write_long_cb, &write_long_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &write_long_arg);

    /*** Write reliable. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    attr.handle = 8;
    attr.offset = 0;
    attr.om = os_msys_get_pkthdr(0, 0);
    rc = ble_gattc_write_reliable(
        1, &attr, 1, ble_gatt_conn_test_write_rel_cb, &write_rel_arg);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, &write_rel_arg);

    /*** Indication. */
    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);
    rc = ble_gattc_indicate(1, attr_handle);
    TEST_ASSERT_FATAL(rc == 0);
    ble_gatt_conn_test_util_timeout(1, NULL);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_gatt_conn_suite)
{
    ble_gatt_conn_test_disconnect();
    ble_gatt_conn_test_timeout();
}
