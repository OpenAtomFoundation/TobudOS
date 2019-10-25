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
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

#define BLE_HS_PVCY_TEST_MAX_GAP_EVENTS 256
static struct ble_gap_event
ble_hs_pvcy_test_gap_events[BLE_HS_PVCY_TEST_MAX_GAP_EVENTS];
static int ble_hs_pvcy_test_num_gap_events;

static void
ble_hs_pvcy_test_util_init(void)
{
    ble_hs_test_util_init();
    ble_hs_pvcy_test_num_gap_events = 0;
}

static int
ble_hs_pvcy_test_util_gap_event(struct ble_gap_event *event, void *arg)
{
    TEST_ASSERT_FATAL(ble_hs_pvcy_test_num_gap_events <
                      BLE_HS_PVCY_TEST_MAX_GAP_EVENTS);
    ble_hs_pvcy_test_gap_events[ble_hs_pvcy_test_num_gap_events++] = *event;

    return 0;
}

static void
ble_hs_pvcy_test_util_all_gap_procs(int adv_status,
                                    int conn_status,
                                    int disc_status)
{
    struct ble_gap_disc_params disc_params;
    ble_addr_t peer_addr;
    int rc;

    /* Advertise. */
    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC,
                                    NULL, &ble_hs_test_util_adv_params,
                                    BLE_HS_FOREVER,
                                    ble_hs_pvcy_test_util_gap_event,
                                    NULL, 0, 0);
    TEST_ASSERT_FATAL(rc == adv_status);

    if (rc == 0) {
        rc = ble_hs_test_util_adv_stop(0);
        TEST_ASSERT_FATAL(rc == 0);
    }

    /* Connect. */
    peer_addr = (ble_addr_t){ BLE_ADDR_PUBLIC, {1,2,3,4,5,6} };
    rc = ble_hs_test_util_connect(BLE_ADDR_PUBLIC, &peer_addr,
                                  BLE_HS_FOREVER, NULL,
                                  ble_hs_pvcy_test_util_gap_event, NULL, 0);
    TEST_ASSERT_FATAL(rc == conn_status);

    if (rc == 0) {
        ble_hs_test_util_conn_cancel_full();
    }

    /* Discover. */
    disc_params = (struct ble_gap_disc_params){ 0 };
    rc = ble_hs_test_util_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER,
                               &disc_params, ble_hs_pvcy_test_util_gap_event,
                               NULL, -1, 0);
    TEST_ASSERT_FATAL(rc == disc_status);

    if (rc == 0) {
        rc = ble_hs_test_util_disc_cancel(0);
        TEST_ASSERT_FATAL(rc == 0);
    }
}

static void
ble_hs_pvcy_test_util_add_irk_set_acks(bool scanning, bool connecting)
{
    ble_hs_test_util_hci_ack_append(
        BLE_HCI_OP(BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_SET_ADV_ENABLE), 0);

    if (connecting) {
        ble_hs_test_util_hci_ack_append(
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_CREATE_CONN_CANCEL),
            0);
    }

    if (scanning) {
        ble_hs_test_util_hci_ack_append(
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_SCAN_ENABLE),
            0);
    }

    ble_hs_test_util_hci_ack_append(
        BLE_HCI_OP(BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_ADD_RESOLV_LIST), 0);
    ble_hs_test_util_hci_ack_append(
        BLE_HCI_OP(BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_SET_PRIVACY_MODE), 0);
}

static void
ble_hs_pvcy_test_util_start_host(int num_expected_irks)
{
    int rc;
    int i;

    /* Clear our IRK.  This ensures the full startup sequence, including
     * setting the default IRK, takes place.  We need this so that we can plan
     * which HCI acks to fake.
     */
    rc = ble_hs_test_util_set_our_irk((uint8_t[16]){0}, -1, 0);
    TEST_ASSERT_FATAL(rc == 0);
    ble_hs_test_util_hci_out_clear();

    ble_hs_test_util_hci_ack_set_startup();

    for (i = 0; i < num_expected_irks; i++) {
        ble_hs_pvcy_test_util_add_irk_set_acks(false, false);
    }

    ble_hs_enabled_state = BLE_HS_ENABLED_STATE_OFF;
    rc = ble_hs_start();
    TEST_ASSERT_FATAL(rc == 0);

    /* Discard startup HCI commands. */
    ble_hs_test_util_hci_out_adj(ble_hs_test_util_hci_startup_seq_cnt());
}

static void
ble_hs_pvcy_test_util_add_irk_verify_tx(const ble_addr_t *peer_addr,
                                        const uint8_t *peer_irk,
                                        const uint8_t *local_irk,
                                        bool scanning,
                                        bool connecting)
{
    ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                   BLE_HCI_OCF_LE_SET_ADV_ENABLE,
                                   NULL);

    if (connecting) {
        ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                       BLE_HCI_OCF_LE_CREATE_CONN_CANCEL,
                                       NULL);
    }

    if (scanning) {
        ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                       BLE_HCI_OCF_LE_SET_SCAN_ENABLE,
                                       NULL);
    }

    ble_hs_test_util_hci_verify_tx_add_irk(peer_addr->type,
                                           peer_addr->val,
                                           peer_irk,
                                           local_irk);

    ble_hs_test_util_hci_verify_tx_set_priv_mode(peer_addr->type,
                                                 peer_addr->val,
                                                 BLE_GAP_PRIVATE_MODE_DEVICE);
}

static void
ble_hs_pvcy_test_util_add_irk(const ble_addr_t *peer_addr,
                              const uint8_t *peer_irk,
                              const uint8_t *local_irk,
                              bool scanning,
                              bool connecting)
{
    int num_acks;
    int rc;

    ble_hs_pvcy_test_util_add_irk_set_acks(scanning, connecting);

    rc = ble_hs_pvcy_add_entry(peer_addr->val, peer_addr->type, peer_irk);
    TEST_ASSERT_FATAL(rc == 0);

    num_acks = 3;
    if (scanning) {
        num_acks++;
    }
    if (connecting) {
        num_acks++;
    }
    ble_hs_test_util_hci_out_adj(-num_acks);
    ble_hs_pvcy_test_util_add_irk_verify_tx(peer_addr, peer_irk, local_irk,
                                            scanning, connecting);
}

static void
ble_hs_pvcy_test_util_add_arbitrary_irk(bool scanning, bool connecting)
{
    ble_addr_t peer_addr;

    peer_addr = (ble_addr_t) {
        .type = BLE_ADDR_PUBLIC,
        .val = {1,2,3,4,5,6},
    };
    ble_hs_pvcy_test_util_add_irk(
        &peer_addr,
        (uint8_t[16]){1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
        ble_hs_pvcy_default_irk,
        scanning,
        connecting);
}

static void
ble_hs_pvcy_test_util_restore_irk(const struct ble_store_value_sec *value_sec,
                                  bool scanning,
                                  bool connecting)
{
    int rc;

    ble_hs_pvcy_test_util_add_irk_set_acks(scanning, connecting);

    rc = ble_store_write_peer_sec(value_sec);
    TEST_ASSERT_FATAL(rc == 0);

    ble_hs_pvcy_test_util_add_irk_verify_tx(&value_sec->peer_addr,
                                            value_sec->irk,
                                            ble_hs_pvcy_default_irk,
                                            scanning,
                                            connecting);
}

TEST_CASE_SELF(ble_hs_pvcy_test_case_restore_irks)
{
    struct ble_store_value_sec value_sec1;
    struct ble_store_value_sec value_sec2;

    ble_hs_pvcy_test_util_init();

    /*** No persisted IRKs. */
    ble_hs_pvcy_test_util_start_host(0);

    /*** One persisted IRK. */

    /* Persist IRK; ensure it automatically gets added to the list. */
    value_sec1 = (struct ble_store_value_sec) {
        .peer_addr = { BLE_ADDR_PUBLIC, { 1, 2, 3, 4, 5, 6 } },
        .key_size = 16,
        .ediv = 1,
        .rand_num = 2,
        .irk = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 },
        .irk_present = 1,
    };
    ble_hs_pvcy_test_util_restore_irk(&value_sec1, false, false);

    /* Ensure it gets added to list on startup. */
    ble_hs_pvcy_test_util_start_host(1);
    ble_hs_pvcy_test_util_add_irk_verify_tx(&value_sec1.peer_addr,
                                            value_sec1.irk,
                                            ble_hs_pvcy_default_irk,
                                            false, false);

    /* Two persisted IRKs. */
    value_sec2 = (struct ble_store_value_sec) {
        .peer_addr = { BLE_ADDR_PUBLIC, { 2, 3, 4, 5, 6, 7 } },
        .key_size = 16,
        .ediv = 12,
        .rand_num = 20,
        .irk = { 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 9, 9, 9, 9, 9, 10 },
        .irk_present = 1,
    };
    ble_hs_pvcy_test_util_restore_irk(&value_sec2, false, false);

    /* Ensure both get added to list on startup. */
    ble_hs_pvcy_test_util_start_host(2);
    ble_hs_pvcy_test_util_add_irk_verify_tx(&value_sec1.peer_addr,
                                            value_sec1.irk,
                                            ble_hs_pvcy_default_irk,
                                            false, false);
    ble_hs_pvcy_test_util_add_irk_verify_tx(&value_sec2.peer_addr,
                                            value_sec2.irk,
                                            ble_hs_pvcy_default_irk,
                                            false, false);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/** No active GAP procedures. */
TEST_CASE_SELF(ble_hs_pvcy_test_case_add_irk_idle)
{
    ble_hs_pvcy_test_util_init();

    ble_hs_pvcy_test_util_add_arbitrary_irk(false, false);
    TEST_ASSERT(ble_hs_pvcy_test_num_gap_events == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*** Advertising active. */
TEST_CASE_SELF(ble_hs_pvcy_test_case_add_irk_adv)
{
    int rc;

    ble_hs_pvcy_test_util_init();

    /* Start an advertising procedure. */
    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC,
                                    NULL, &ble_hs_test_util_adv_params,
                                    BLE_HS_FOREVER,
                                    ble_hs_pvcy_test_util_gap_event,
                                    NULL, 0, 0);
    TEST_ASSERT_FATAL(rc == 0);

    ble_hs_pvcy_test_util_add_arbitrary_irk(false, false);

    TEST_ASSERT(ble_hs_pvcy_test_num_gap_events == 1);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].type ==
                BLE_GAP_EVENT_ADV_COMPLETE);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].adv_complete.reason ==
                BLE_HS_EPREEMPTED);

    /* Ensure GAP procedures are no longer preempted. */
    ble_hs_pvcy_test_util_all_gap_procs(0, 0, 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*** Discovery active. */
TEST_CASE_SELF(ble_hs_pvcy_test_case_add_irk_disc)
{
    struct ble_gap_disc_params disc_params;
    int rc;

    ble_hs_pvcy_test_util_init();

    /* Start an advertising procedure. */
    disc_params = (struct ble_gap_disc_params){ 0 };
    rc = ble_hs_test_util_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER,
                               &disc_params, ble_hs_pvcy_test_util_gap_event,
                               NULL, -1, 0);
    TEST_ASSERT_FATAL(rc == 0);

    ble_hs_pvcy_test_util_add_arbitrary_irk(true, false);

    TEST_ASSERT(ble_hs_pvcy_test_num_gap_events == 1);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].type ==
                BLE_GAP_EVENT_DISC_COMPLETE);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].disc_complete.reason ==
                BLE_HS_EPREEMPTED);

    /* Ensure GAP procedures are no longer preempted. */
    ble_hs_pvcy_test_util_all_gap_procs(0, 0, 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*** Connect active. */
TEST_CASE_SELF(ble_hs_pvcy_test_case_add_irk_conn)
{
    ble_addr_t peer_addr;
    int rc;

    ble_hs_pvcy_test_util_init();

    /* Start a connect procedure. */
    peer_addr = (ble_addr_t){ BLE_ADDR_PUBLIC, {1,2,3,4,5,6} };
    rc = ble_hs_test_util_connect(BLE_ADDR_PUBLIC, &peer_addr,
                                  BLE_HS_FOREVER, NULL,
                                  ble_hs_pvcy_test_util_gap_event, NULL, 0);
    TEST_ASSERT_FATAL(rc == 0);

    ble_hs_pvcy_test_util_add_arbitrary_irk(false, true);

    /* Cancel is now in progress. */
    TEST_ASSERT(ble_hs_pvcy_test_num_gap_events == 0);

    /* Ensure no GAP procedures are allowed. */
    ble_hs_pvcy_test_util_all_gap_procs(BLE_HS_EPREEMPTED,
                                        BLE_HS_EALREADY,
                                        BLE_HS_EBUSY);

    /* Receive cancel event. */
    ble_hs_test_util_rx_conn_cancel_evt();

    TEST_ASSERT(ble_hs_pvcy_test_num_gap_events == 1);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].type ==
                BLE_GAP_EVENT_CONNECT);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].connect.status ==
                BLE_HS_EPREEMPTED);

    /* Ensure GAP procedures are no longer preempted. */
    ble_hs_pvcy_test_util_all_gap_procs(0, 0, 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*** Advertising and discovery active. */
TEST_CASE_SELF(ble_hs_pvcy_test_case_add_irk_adv_disc)
{
    struct ble_gap_disc_params disc_params;
    int rc;

    ble_hs_pvcy_test_util_init();

    /* Start an advertising procedure. */
    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC,
                                    NULL, &ble_hs_test_util_adv_params,
                                    BLE_HS_FOREVER,
                                    ble_hs_pvcy_test_util_gap_event,
                                    NULL, 0, 0);
    TEST_ASSERT_FATAL(rc == 0);

    /* Start a discovery procedure. */
    disc_params = (struct ble_gap_disc_params){ 0 };
    rc = ble_hs_test_util_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER,
                               &disc_params, ble_hs_pvcy_test_util_gap_event,
                               NULL, -1, 0);
    TEST_ASSERT_FATAL(rc == 0);

    ble_hs_pvcy_test_util_add_arbitrary_irk(true, false);

    TEST_ASSERT(ble_hs_pvcy_test_num_gap_events == 2);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].type ==
                BLE_GAP_EVENT_ADV_COMPLETE);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].adv_complete.reason ==
                BLE_HS_EPREEMPTED);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[1].type ==
                BLE_GAP_EVENT_DISC_COMPLETE);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[1].disc_complete.reason ==
                BLE_HS_EPREEMPTED);

    /* Ensure GAP procedures are no longer preempted. */
    ble_hs_pvcy_test_util_all_gap_procs(0, 0, 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*** Advertising and connecting active. */
TEST_CASE_SELF(ble_hs_pvcy_test_case_add_irk_adv_conn)
{
    ble_addr_t peer_addr;
    int rc;

    ble_hs_pvcy_test_util_init();

    /* Start an advertising procedure. */
    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC,
                                    NULL, &ble_hs_test_util_adv_params,
                                    BLE_HS_FOREVER,
                                    ble_hs_pvcy_test_util_gap_event,
                                    NULL, 0, 0);
    TEST_ASSERT_FATAL(rc == 0);

    /* Start a connect procedure. */
    peer_addr = (ble_addr_t){ BLE_ADDR_PUBLIC, {1,2,3,4,5,6} };
    rc = ble_hs_test_util_connect(BLE_ADDR_PUBLIC, &peer_addr,
                                  BLE_HS_FOREVER, NULL,
                                  ble_hs_pvcy_test_util_gap_event, NULL, 0);
    TEST_ASSERT_FATAL(rc == 0);

    ble_hs_pvcy_test_util_add_arbitrary_irk(false, true);

    /* Cancel is now in progress. */
    TEST_ASSERT(ble_hs_pvcy_test_num_gap_events == 1);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].type ==
                BLE_GAP_EVENT_ADV_COMPLETE);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[0].adv_complete.reason ==
                BLE_HS_EPREEMPTED);

    /* Ensure no GAP procedures are allowed. */
    ble_hs_pvcy_test_util_all_gap_procs(BLE_HS_EPREEMPTED,
                                        BLE_HS_EALREADY,
                                        BLE_HS_EBUSY);

    /* Receive cancel event. */
    ble_hs_test_util_rx_conn_cancel_evt();

    TEST_ASSERT(ble_hs_pvcy_test_num_gap_events == 2);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[1].type ==
                BLE_GAP_EVENT_CONNECT);
    TEST_ASSERT(ble_hs_pvcy_test_gap_events[1].connect.status ==
                BLE_HS_EPREEMPTED);

    /* Ensure GAP procedures are no longer preempted. */
    ble_hs_pvcy_test_util_all_gap_procs(0, 0, 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_hs_pvcy_test_suite_irk)
{
    ble_hs_pvcy_test_case_restore_irks();
    ble_hs_pvcy_test_case_add_irk_idle();
    ble_hs_pvcy_test_case_add_irk_adv();
    ble_hs_pvcy_test_case_add_irk_disc();
    ble_hs_pvcy_test_case_add_irk_conn();
    ble_hs_pvcy_test_case_add_irk_adv_disc();
    ble_hs_pvcy_test_case_add_irk_adv_conn();
}
