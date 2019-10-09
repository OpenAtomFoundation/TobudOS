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
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"
#include "ble_hs_test.h"
#include "testutil/testutil.h"
#include "ble_hs_test_util.h"

TEST_CASE_SELF(ble_hs_hci_test_event_bad)
{
    uint8_t *buf;
    int rc;

    /*** Invalid event code. */
    buf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
    TEST_ASSERT_FATAL(buf != NULL);

    buf[0] = 0xff;
    buf[1] = 0;
    rc = ble_hs_hci_evt_process(buf);
    TEST_ASSERT(rc == BLE_HS_ENOTSUP);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_hci_test_rssi)
{
    uint8_t params[BLE_HCI_READ_RSSI_ACK_PARAM_LEN];
    uint16_t opcode;
    int8_t rssi;
    int rc;

    opcode = ble_hs_hci_util_opcode_join(BLE_HCI_OGF_STATUS_PARAMS,
                                  BLE_HCI_OCF_RD_RSSI);

    /*** Success. */
    /* Connection handle. */
    put_le16(params + 0, 1);

    /* RSSI. */
    params[2] = -8;

    ble_hs_test_util_hci_ack_set_params(opcode, 0, params, sizeof params);

    rc = ble_hs_hci_util_read_rssi(1, &rssi);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(rssi == -8);

    /*** Failure: incorrect connection handle. */
    put_le16(params + 0, 99);

    ble_hs_test_util_hci_ack_set_params(opcode, 0, params, sizeof params);

    rc = ble_hs_hci_util_read_rssi(1, &rssi);
    TEST_ASSERT(rc == BLE_HS_ECONTROLLER);

    /*** Failure: params too short. */
    ble_hs_test_util_hci_ack_set_params(opcode, 0, params, sizeof params - 1);
    rc = ble_hs_hci_util_read_rssi(1, &rssi);
    TEST_ASSERT(rc == BLE_HS_ECONTROLLER);

    /*** Failure: params too long. */
    ble_hs_test_util_hci_ack_set_params(opcode, 0, params, sizeof params + 1);
    rc = ble_hs_hci_util_read_rssi(1, &rssi);
    TEST_ASSERT(rc == BLE_HS_ECONTROLLER);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_hci_acl_one_conn)
{
    struct ble_hs_test_util_hci_num_completed_pkts_entry ncpe[2];
    struct hci_disconn_complete evt;
    uint8_t peer_addr[6] = { 1, 2, 3, 4, 5, 6 };
    uint8_t data[256];
    int rc;
    int i;

    memset(ncpe, 0, sizeof(ncpe));
    for (i = 0; i < sizeof data; i++) {
        data[i] = i;
    }

    ble_hs_test_util_init();

    /* The controller has room for five 20-byte payloads. */
    rc = ble_hs_hci_set_buf_sz(20, 5);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 5);

    ble_hs_test_util_create_conn(1, peer_addr, NULL, NULL);

    /* Ensure the ATT doesn't truncate our data packets. */
    ble_hs_test_util_set_att_mtu(1, 256);

    /* Send two 3-byte data packets. */
    rc = ble_hs_test_util_gatt_write_no_rsp_flat(1, 100, data, 3);
    TEST_ASSERT_FATAL(rc == 0);
    rc = ble_hs_test_util_gatt_write_no_rsp_flat(1, 100, data, 3);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 3);

    /* Send fragmented packet (two fragments). */
    rc = ble_hs_test_util_gatt_write_no_rsp_flat(1, 100, data, 25);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 1);

    ble_hs_test_util_prev_tx_queue_clear();

    /* Receive a number-of-completed-packets event.  Ensure available buffer
     * count increases.
     */
    ncpe[0].handle_id = 1;
    ncpe[0].num_pkts = 3;
    ble_hs_test_util_hci_rx_num_completed_pkts_event(ncpe);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 4);

    /* Use all remaining buffers (four fragments). */
    rc = ble_hs_test_util_gatt_write_no_rsp_flat(1, 100, data, 70);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 0);

    /* Attempt to transmit eight more fragments. */
    rc = ble_hs_test_util_gatt_write_no_rsp_flat(1, 100, data, 160);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 0);

    /* Receive number-of-completed-packets: 5. */
    ncpe[0].handle_id = 1;
    ncpe[0].num_pkts = 5;
    ble_hs_test_util_hci_rx_num_completed_pkts_event(ncpe);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 0);

    /* Receive number-of-completed-packets: 4. */
    ncpe[0].handle_id = 1;
    ncpe[0].num_pkts = 5;
    ble_hs_test_util_hci_rx_num_completed_pkts_event(ncpe);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 1);

    /* Ensure the stalled fragments were sent in the expected order. */
    ble_hs_test_util_verify_tx_write_cmd(100, data, 70);
    ble_hs_test_util_verify_tx_write_cmd(100, data, 160);

    /* Receive a disconnection-complete event. Ensure available buffer count
     * increases.
     */
    evt.connection_handle = 1;
    evt.status = 0;
    evt.reason = BLE_ERR_CONN_TERM_LOCAL;
    ble_hs_test_util_hci_rx_disconn_complete_event(&evt);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 5);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_hci_acl_two_conn)
{
    struct ble_hs_test_util_hci_num_completed_pkts_entry ncpe[2];
    const struct ble_hs_conn *conn1;
    const struct ble_hs_conn *conn2;
    uint8_t peer_addr1[6] = { 1, 2, 3, 4, 5, 6 };
    uint8_t peer_addr2[6] = { 2, 3, 4, 5, 6, 7 };
    uint8_t data[256];
    int rc;
    int i;

    memset(ncpe, 0, sizeof(ncpe));
    for (i = 0; i < sizeof data; i++) {
        data[i] = i;
    }

    ble_hs_test_util_init();

    /* The controller has room for five 20-byte payloads*/
    rc = ble_hs_hci_set_buf_sz(20, 5);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 5);

    ble_hs_test_util_create_conn(1, peer_addr1, NULL, NULL);
    ble_hs_test_util_create_conn(2, peer_addr2, NULL, NULL);

    /* This test inspects the connection objects after unlocking the host
     * mutex.  It is not OK for real code to do this, but this test can assume
     * the connection list is unchanging.
     */
    ble_hs_lock();
    conn1 = ble_hs_conn_find_assert(1);
    conn2 = ble_hs_conn_find_assert(2);
    ble_hs_unlock();

    /* Ensure the ATT doesn't truncate our data packets. */
    ble_hs_test_util_set_att_mtu(1, 256);
    ble_hs_test_util_set_att_mtu(2, 256);

    /* Tx two fragments over connection 1. */
    rc = ble_hs_test_util_gatt_write_no_rsp_flat(1, 100, data, 25);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 3);
    TEST_ASSERT_FATAL(!(conn1->bhc_flags & BLE_HS_CONN_F_TX_FRAG));

    /* Tx two fragments over connection 2. */
    rc = ble_hs_test_util_gatt_write_no_rsp_flat(2, 100, data + 10, 25);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 1);
    TEST_ASSERT_FATAL(!(conn1->bhc_flags & BLE_HS_CONN_F_TX_FRAG));

    /* Tx four fragments over connection 2. */
    rc = ble_hs_test_util_gatt_write_no_rsp_flat(2, 100, data + 20, 70);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 0);
    TEST_ASSERT_FATAL(conn2->bhc_flags & BLE_HS_CONN_F_TX_FRAG);

    /* Tx four fragments over connection 1. */
    rc = ble_hs_test_util_gatt_write_no_rsp_flat(1, 100, data + 30, 70);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 0);
    TEST_ASSERT_FATAL(!(conn1->bhc_flags & BLE_HS_CONN_F_TX_FRAG));

    /**
     * controller: (11 222)
     *     conn 1: 1111
     *     conn 2: 222
     */

    /* Receive number-of-completed-packets: conn=2, num-pkts=1. */
    ncpe[0].handle_id = 2;
    ncpe[0].num_pkts = 1;
    ble_hs_test_util_hci_rx_num_completed_pkts_event(ncpe);

    /**
     * controller: (11 222)
     *     conn 1: 1111
     *     conn 2: 22
     */
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 0);
    TEST_ASSERT_FATAL(!(conn1->bhc_flags & BLE_HS_CONN_F_TX_FRAG));
    TEST_ASSERT_FATAL(conn2->bhc_flags & BLE_HS_CONN_F_TX_FRAG);

    /* Receive number-of-completed-packets: conn=1, num-pkts=1. */
    ncpe[0].handle_id = 1;
    ncpe[0].num_pkts = 1;
    ble_hs_test_util_hci_rx_num_completed_pkts_event(ncpe);

    /**
     * controller: (1 2222)
     *     conn 1: 1111
     *     conn 2: 2
     */
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 0);
    TEST_ASSERT_FATAL(!(conn1->bhc_flags & BLE_HS_CONN_F_TX_FRAG));
    TEST_ASSERT_FATAL(conn2->bhc_flags & BLE_HS_CONN_F_TX_FRAG);

    /* Receive number-of-completed-packets: conn=1, num-pkts=1. */
    ncpe[0].handle_id = 1;
    ncpe[0].num_pkts = 1;
    ble_hs_test_util_hci_rx_num_completed_pkts_event(ncpe);

    /**
     * controller: (22222)
     *     conn 1: 1111
     *     conn 2: -
     */
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 0);
    TEST_ASSERT_FATAL(!(conn1->bhc_flags & BLE_HS_CONN_F_TX_FRAG));
    TEST_ASSERT_FATAL(!(conn2->bhc_flags & BLE_HS_CONN_F_TX_FRAG));

    /* Receive number-of-completed-packets: conn=2, num-pkts=3. */
    ncpe[0].handle_id = 2;
    ncpe[0].num_pkts = 3;
    ble_hs_test_util_hci_rx_num_completed_pkts_event(ncpe);

    /**
     * controller: (11122)
     *     conn 1: 1
     *     conn 2: -
     */
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 0);
    TEST_ASSERT_FATAL(conn1->bhc_flags & BLE_HS_CONN_F_TX_FRAG);
    TEST_ASSERT_FATAL(!(conn2->bhc_flags & BLE_HS_CONN_F_TX_FRAG));

    /* Receive number-of-completed-packets: conn=2, num-pkts=2. */
    ncpe[0].handle_id = 2;
    ncpe[0].num_pkts = 2;
    ble_hs_test_util_hci_rx_num_completed_pkts_event(ncpe);

    /**
     * controller: (1111)
     *     conn 1: -
     *     conn 2: -
     */
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 1);
    TEST_ASSERT_FATAL(!(conn1->bhc_flags & BLE_HS_CONN_F_TX_FRAG));
    TEST_ASSERT_FATAL(!(conn2->bhc_flags & BLE_HS_CONN_F_TX_FRAG));

    /* Receive number-of-completed-packets: conn=1, num-pkts=4. */
    ncpe[0].handle_id = 1;
    ncpe[0].num_pkts = 4;
    ble_hs_test_util_hci_rx_num_completed_pkts_event(ncpe);

    /**
     * controller: ()
     *     conn 1: -
     *     conn 2: -
     */
    TEST_ASSERT_FATAL(ble_hs_hci_avail_pkts == 5);
    TEST_ASSERT_FATAL(!(conn1->bhc_flags & BLE_HS_CONN_F_TX_FRAG));
    TEST_ASSERT_FATAL(!(conn2->bhc_flags & BLE_HS_CONN_F_TX_FRAG));

    /*** Verify payloads. */
    ble_hs_test_util_verify_tx_write_cmd(100, data, 25);
    ble_hs_test_util_verify_tx_write_cmd(100, data + 10, 25);
    ble_hs_test_util_verify_tx_write_cmd(100, data + 20, 70);
    ble_hs_test_util_verify_tx_write_cmd(100, data + 30, 70);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_hs_hci_suite)
{
    ble_hs_hci_test_event_bad();
    ble_hs_hci_test_rssi();
    ble_hs_hci_acl_one_conn();
    ble_hs_hci_acl_two_conn();
}
