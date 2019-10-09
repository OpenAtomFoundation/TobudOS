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
#include "host/ble_hs_adv.h"
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

static int
ble_hs_conn_test_util_any()
{
    struct ble_hs_conn *conn;

    ble_hs_lock();
    conn = ble_hs_conn_first();
    ble_hs_unlock();

    return conn != NULL;
}

TEST_CASE_SELF(ble_hs_conn_test_direct_connect_success)
{
    struct hci_le_conn_complete evt;
    struct ble_l2cap_chan *chan;
    struct ble_hs_conn *conn;
    ble_addr_t addr = { BLE_ADDR_PUBLIC, { 1, 2, 3, 4, 5, 6 }};
    int rc;

    ble_hs_test_util_init();

    /* Ensure no current or pending connections. */
    TEST_ASSERT(!ble_gap_master_in_progress());
    TEST_ASSERT(!ble_hs_conn_test_util_any());

    /* Initiate connection. */
    rc = ble_hs_test_util_connect(BLE_OWN_ADDR_PUBLIC,
                                        &addr, 0, NULL, NULL, NULL, 0);
    TEST_ASSERT(rc == 0);

    TEST_ASSERT(ble_gap_master_in_progress());

    /* ble_gap_rx_conn_complete() will send extra HCI command, need phony ack */
    ble_hs_test_util_hci_ack_set(ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                             BLE_HCI_OCF_LE_RD_REM_FEAT), 0);

    /* Receive successful connection complete event. */
    memset(&evt, 0, sizeof evt);
    evt.subevent_code = BLE_HCI_LE_SUBEV_CONN_COMPLETE;
    evt.status = BLE_ERR_SUCCESS;
    evt.connection_handle = 2;
    evt.role = BLE_HCI_LE_CONN_COMPLETE_ROLE_MASTER;
    memcpy(evt.peer_addr, addr.val, 6);
    rc = ble_gap_rx_conn_complete(&evt, 0);
    TEST_ASSERT(rc == 0);
    TEST_ASSERT(!ble_gap_master_in_progress());

    ble_hs_lock();

    conn = ble_hs_conn_first();
    TEST_ASSERT_FATAL(conn != NULL);
    TEST_ASSERT(conn->bhc_handle == 2);
    TEST_ASSERT(memcmp(conn->bhc_peer_addr.val, addr.val, 6) == 0);

    chan = ble_hs_conn_chan_find_by_scid(conn, BLE_L2CAP_CID_ATT);
    TEST_ASSERT_FATAL(chan != NULL);
    TEST_ASSERT(chan->my_mtu == MYNEWT_VAL(BLE_ATT_PREFERRED_MTU));
    TEST_ASSERT(chan->peer_mtu == 0);

    ble_hs_unlock();

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_conn_test_direct_connectable_success)
{
    struct hci_le_conn_complete evt;
    struct ble_gap_adv_params adv_params;
    struct ble_l2cap_chan *chan;
    struct ble_hs_conn *conn;
    ble_addr_t addr = { BLE_ADDR_PUBLIC, { 1, 2, 3, 4, 5, 6 }};
    int rc;

    ble_hs_test_util_init();

    /* Ensure no current or pending connections. */
    TEST_ASSERT(!ble_gap_master_in_progress());
    TEST_ASSERT(!ble_gap_adv_active());
    TEST_ASSERT(!ble_hs_conn_test_util_any());

    /* Initiate advertising. */
    adv_params = ble_hs_test_util_adv_params;
    adv_params.conn_mode = BLE_GAP_CONN_MODE_DIR;
    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC,
                                    &addr, &adv_params, BLE_HS_FOREVER,
                                    NULL, NULL, 0, 0);
    TEST_ASSERT(rc == 0);

    TEST_ASSERT(!ble_gap_master_in_progress());
    TEST_ASSERT(ble_gap_adv_active());

    /* ble_gap_rx_conn_complete() will send extra HCI command, need phony ack */
    ble_hs_test_util_hci_ack_set(ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                             BLE_HCI_OCF_LE_RD_REM_FEAT), 0);

    /* Receive successful connection complete event. */
    memset(&evt, 0, sizeof evt);
    evt.subevent_code = BLE_HCI_LE_SUBEV_CONN_COMPLETE;
    evt.status = BLE_ERR_SUCCESS;
    evt.connection_handle = 2;
    evt.role = BLE_HCI_LE_CONN_COMPLETE_ROLE_SLAVE;
    memcpy(evt.peer_addr, addr.val, 6);
    rc = ble_gap_rx_conn_complete(&evt, 0);
    TEST_ASSERT(rc == 0);
    TEST_ASSERT(!ble_gap_master_in_progress());
    TEST_ASSERT(!ble_gap_adv_active());

    ble_hs_lock();

    conn = ble_hs_conn_first();
    TEST_ASSERT_FATAL(conn != NULL);
    TEST_ASSERT(conn->bhc_handle == 2);
    TEST_ASSERT(memcmp(conn->bhc_peer_addr.val, addr.val, 6) == 0);

    chan = ble_hs_conn_chan_find_by_scid(conn, BLE_L2CAP_CID_ATT);
    TEST_ASSERT_FATAL(chan != NULL);
    TEST_ASSERT(chan->my_mtu == MYNEWT_VAL(BLE_ATT_PREFERRED_MTU));
    TEST_ASSERT(chan->peer_mtu == 0);

    ble_hs_unlock();

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_conn_test_undirect_connectable_success)
{
    struct ble_hs_adv_fields adv_fields;
    struct hci_le_conn_complete evt;
    struct ble_gap_adv_params adv_params;
    struct ble_l2cap_chan *chan;
    struct ble_hs_conn *conn;
    ble_addr_t addr = { BLE_ADDR_PUBLIC, { 1, 2, 3, 4, 5, 6 }};
    int rc;

    ble_hs_test_util_init();

    /* Ensure no current or pending connections. */
    TEST_ASSERT(!ble_gap_master_in_progress());
    TEST_ASSERT(!ble_gap_adv_active());
    TEST_ASSERT(!ble_hs_conn_test_util_any());

    /* Initiate advertising. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.tx_pwr_lvl_is_present = 1;
    rc = ble_hs_test_util_adv_set_fields(&adv_fields, 0, 0);
    TEST_ASSERT_FATAL(rc == 0);

    adv_params = ble_hs_test_util_adv_params;
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC,
                                    &addr, &adv_params,
                                    BLE_HS_FOREVER,
                                    NULL, NULL, 0, 0);
    TEST_ASSERT(rc == 0);

    TEST_ASSERT(!ble_gap_master_in_progress());
    TEST_ASSERT(ble_gap_adv_active());

    /* ble_gap_rx_conn_complete() will send extra HCI command, need phony ack */
    ble_hs_test_util_hci_ack_set(ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                             BLE_HCI_OCF_LE_RD_REM_FEAT), 0);

    /* Receive successful connection complete event. */
    memset(&evt, 0, sizeof evt);
    evt.subevent_code = BLE_HCI_LE_SUBEV_CONN_COMPLETE;
    evt.status = BLE_ERR_SUCCESS;
    evt.connection_handle = 2;
    evt.role = BLE_HCI_LE_CONN_COMPLETE_ROLE_SLAVE;
    memcpy(evt.peer_addr, addr.val, 6);
    rc = ble_gap_rx_conn_complete(&evt, 0);
    TEST_ASSERT(rc == 0);
    TEST_ASSERT(!ble_gap_master_in_progress());
    TEST_ASSERT(!ble_gap_adv_active());

    ble_hs_lock();

    conn = ble_hs_conn_first();
    TEST_ASSERT_FATAL(conn != NULL);
    TEST_ASSERT(conn->bhc_handle == 2);
    TEST_ASSERT(memcmp(conn->bhc_peer_addr.val, addr.val, 6) == 0);

    chan = ble_hs_conn_chan_find_by_scid(conn, BLE_L2CAP_CID_ATT);
    TEST_ASSERT_FATAL(chan != NULL);
    TEST_ASSERT(chan->my_mtu == MYNEWT_VAL(BLE_ATT_PREFERRED_MTU));
    TEST_ASSERT(chan->peer_mtu == 0);

    ble_hs_unlock();

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_hs_conn_suite)
{
    ble_hs_conn_test_direct_connect_success();
    ble_hs_conn_test_direct_connectable_success();
    ble_hs_conn_test_undirect_connectable_success();
}
