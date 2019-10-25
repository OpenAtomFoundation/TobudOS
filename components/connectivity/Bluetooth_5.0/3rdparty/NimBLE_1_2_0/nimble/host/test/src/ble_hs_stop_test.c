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
#include "host/ble_hs.h"
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

#define BHST_MAX_EVENTS     32

static struct ble_gap_event bhst_events[BHST_MAX_EVENTS];
static int bhst_num_events;

static struct ble_hs_stop_listener bhst_listener;
static struct os_sem bhst_sem;

static int
bhst_gap_event(struct ble_gap_event *event, void *arg)
{
    TEST_ASSERT_FATAL(bhst_num_events < BHST_MAX_EVENTS);

    bhst_events[bhst_num_events++] = *event;
    return 0;
}

static void
bhst_stop_cb(int status, void *arg)
{
    int rc;

    rc = os_sem_release(&bhst_sem);
    TEST_ASSERT_FATAL(rc == 0);
}

TEST_CASE_TASK(ble_hs_stop_test_new_procs)
{
    static const struct ble_gap_disc_params disc_params;
    static const struct ble_gap_adv_params adv_params;

    static const ble_addr_t peer_addr = {
        BLE_ADDR_PUBLIC,
        { 1, 2, 3, 4, 5, 6 }
    };

    int rc;

    rc = os_sem_init(&bhst_sem, 0);
    TEST_ASSERT_FATAL(rc == 0);

    /* Stop the host and wait for the stop procedure to complete. */
    ble_hs_test_util_hci_ack_set(
        BLE_HCI_OP(BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_SET_ADV_ENABLE), 0);

    rc = ble_hs_stop(&bhst_listener, bhst_stop_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);
    rc = os_sem_pend(&bhst_sem, OS_TIMEOUT_NEVER);
    TEST_ASSERT_FATAL(rc == 0);

    /*** Ensure all GAP procedures fail. */

    /* Advertise. */
    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, &adv_params,
                                    BLE_HS_FOREVER, bhst_gap_event, NULL,
                                    0, 0);
    TEST_ASSERT(rc == BLE_HS_EDISABLED);

    /* Discover. */
    rc = ble_hs_test_util_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER,
                               &disc_params, bhst_gap_event, NULL, 0, 0);
    TEST_ASSERT(rc == BLE_HS_EDISABLED);

    /* Connect. */
    rc = ble_hs_test_util_connect(BLE_OWN_ADDR_PUBLIC, &peer_addr,
                                  BLE_HS_FOREVER, NULL,
                                  bhst_gap_event, NULL, 0);
    TEST_ASSERT(rc == BLE_HS_EDISABLED);

    /*** Restart stack; ensure GAP procedures succeed. */

    ble_hs_test_util_hci_ack_set_startup();
    ble_hs_sched_start();

    /* Advertise. */
    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, &adv_params,
                                    BLE_HS_FOREVER, bhst_gap_event, NULL,
                                    0, 0);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_adv_stop(0);
    TEST_ASSERT(rc == 0);

    /* Discover. */
    rc = ble_hs_test_util_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER,
                               &disc_params, bhst_gap_event, NULL, 0, 0);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_disc_cancel(0);
    TEST_ASSERT(rc == 0);

    /* Connect. */
    rc = ble_hs_test_util_connect(BLE_OWN_ADDR_PUBLIC, &peer_addr,
                                  BLE_HS_FOREVER, NULL,
                                  bhst_gap_event, NULL, 0);
    TEST_ASSERT(rc == 0);

    rc = ble_hs_test_util_conn_cancel(0);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_TASK(ble_hs_stop_test_cur_procs)
{
    static const struct ble_gap_disc_params disc_params;
    static const struct ble_gap_adv_params adv_params;

    int rc;

    rc = os_sem_init(&bhst_sem, 0);
    TEST_ASSERT_FATAL(rc == 0);

    /* Advertise. */
    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, &adv_params,
                                    BLE_HS_FOREVER, bhst_gap_event, NULL,
                                    0, 0);
    TEST_ASSERT(rc == 0);

    /* Discover. */
    rc = ble_hs_test_util_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER,
                               &disc_params, bhst_gap_event, NULL, 0, 0);
    TEST_ASSERT(rc == 0);

    /* Preload the host with HCI acks for the cancel commands that will be sent
     * automatically when the host stops.
     */
    ble_hs_test_util_hci_ack_set(
        BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_ADV_ENABLE),
        0);
    ble_hs_test_util_hci_ack_append(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_SET_SCAN_ENABLE),
        0);

    /* Stop the host and wait for the stop procedure to complete. */
    bhst_num_events = 0;
    rc = ble_hs_stop(&bhst_listener, bhst_stop_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);
    rc = os_sem_pend(&bhst_sem, OS_TIMEOUT_NEVER);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure the GAP procedure cancellations were reported. */
    TEST_ASSERT_FATAL(bhst_num_events == 2);
    TEST_ASSERT(bhst_events[0].type == BLE_GAP_EVENT_ADV_COMPLETE);
    TEST_ASSERT(bhst_events[0].adv_complete.reason == BLE_HS_EPREEMPTED);
    TEST_ASSERT(bhst_events[1].type == BLE_GAP_EVENT_DISC_COMPLETE);
    TEST_ASSERT(bhst_events[1].disc_complete.reason == BLE_HS_EPREEMPTED);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

static void
bhst_pre_test(void *arg)
{
    ble_hs_test_util_init_no_sysinit_no_start();

    /* Preload the host with HCI acks for the startup sequence. */
    ble_hs_test_util_hci_ack_set_startup();
}

TEST_SUITE(ble_hs_stop_test_suite)
{
    tu_suite_set_pre_test_cb(bhst_pre_test, NULL);

    ble_hs_stop_test_new_procs();
    ble_hs_stop_test_cur_procs();
}

int
ble_stop_test_all(void)
{
    ble_hs_stop_test_suite();

    return tu_any_failed;
}
