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
#include "os/os.h"
#include "testutil/testutil.h"
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"
#include "ble_hs_test.h"
#include "host/ble_gap.h"
#include "ble_hs_test_util.h"

#define BLE_OS_TEST_STACK_SIZE      256
#define BLE_OS_TEST_APP_STACK_SIZE  256

#define BLE_OS_TEST_APP_PRIO         9
#define BLE_OS_TEST_TASK_PRIO        10

static struct os_task ble_os_test_task;
static struct os_task ble_os_test_app_task;
static os_stack_t ble_os_test_stack[OS_STACK_ALIGN(BLE_OS_TEST_STACK_SIZE)];

static os_stack_t
ble_os_test_app_stack[OS_STACK_ALIGN(BLE_OS_TEST_APP_STACK_SIZE)];

static uint8_t ble_os_test_peer_addr[6] = { 1, 2, 3, 4, 5, 6 };

static void ble_os_test_app_task_handler(void *arg);

static int ble_os_test_gap_event_type;

static void
ble_os_test_init_app_task(void)
{
    int rc;

    rc = os_task_init(&ble_os_test_app_task,
                      "ble_gap_terminate_test_task",
                      ble_os_test_app_task_handler, NULL,
                      BLE_OS_TEST_APP_PRIO, OS_WAIT_FOREVER,
                      ble_os_test_app_stack,
                      OS_STACK_ALIGN(BLE_OS_TEST_APP_STACK_SIZE));
    TEST_ASSERT_FATAL(rc == 0);
}

static void
ble_os_test_misc_init(void)
{
    extern os_time_t g_os_time;

    ble_hs_test_util_init_no_start();

    /* Allow the OS to approach tick rollover.  This will help ensure host
     * timers don't break when the tick counter resets.
     */
    g_os_time = UINT32_MAX - 10 * OS_TICKS_PER_SEC;

    /* Receive acknowledgements for the startup sequence.  We sent the
     * corresponding requests when the host task was started.
     */
    ble_hs_test_util_hci_ack_set_startup();

    ble_os_test_init_app_task();
}

static int
ble_os_test_misc_conn_exists(uint16_t conn_handle)
{
    struct ble_hs_conn *conn;

    ble_hs_lock();

    if (conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        conn = ble_hs_conn_first();
    } else {
        conn = ble_hs_conn_find(conn_handle);
    }

    ble_hs_unlock();

    return conn != NULL;
}

static int
ble_gap_direct_connect_test_connect_cb(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int *cb_called;
    int rc;

    cb_called = arg;
    *cb_called = 1;

    TEST_ASSERT(event->type == BLE_GAP_EVENT_CONNECT);
    TEST_ASSERT(event->connect.status == 0);
    TEST_ASSERT(event->connect.conn_handle == 2);

    rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(desc.peer_id_addr.type == BLE_ADDR_PUBLIC);
    TEST_ASSERT(memcmp(desc.peer_id_addr.val, ble_os_test_peer_addr, 6) == 0);

    return 0;
}

static void
ble_gap_direct_connect_test_task_handler(void *arg)
{
    struct hci_le_conn_complete evt;
    ble_addr_t addr = { BLE_ADDR_PUBLIC, { 1, 2, 3, 4, 5, 6 }};
    int cb_called;
    int rc;

    /* Set the connect callback so we can verify that it gets called with the
     * proper arguments.
     */
    cb_called = 0;

    /* Make sure there are no created connections and no connections in
     * progress.
     */
    TEST_ASSERT(!ble_os_test_misc_conn_exists(BLE_HS_CONN_HANDLE_NONE));

    /* Initiate a direct connection. */
    ble_hs_test_util_connect(BLE_OWN_ADDR_PUBLIC, &addr, 0, NULL,
                             ble_gap_direct_connect_test_connect_cb,
                             &cb_called, 0);
    TEST_ASSERT(!ble_os_test_misc_conn_exists(BLE_HS_CONN_HANDLE_NONE));
    TEST_ASSERT(!cb_called);

    /* ble_gap_rx_conn_complete() will send extra HCI command, need phony ack */
    ble_hs_test_util_hci_ack_set(ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                             BLE_HCI_OCF_LE_RD_REM_FEAT), 0);

    /* Receive an HCI connection-complete event. */
    memset(&evt, 0, sizeof evt);
    evt.subevent_code = BLE_HCI_LE_SUBEV_CONN_COMPLETE;
    evt.status = BLE_ERR_SUCCESS;
    evt.connection_handle = 2;
    memcpy(evt.peer_addr, addr.val, 6);
    rc = ble_gap_rx_conn_complete(&evt, 0);
    TEST_ASSERT(rc == 0);

    /* The connection should now be created. */
    TEST_ASSERT(ble_os_test_misc_conn_exists(2));
    TEST_ASSERT(cb_called);

    tu_restart();
}

TEST_CASE_SELF(ble_gap_direct_connect_test_case)
{
    ble_os_test_misc_init();

    os_task_init(&ble_os_test_task,
                 "ble_gap_direct_connect_test_task",
                 ble_gap_direct_connect_test_task_handler, NULL,
                 BLE_OS_TEST_TASK_PRIO, OS_WAIT_FOREVER, ble_os_test_stack,
                 OS_STACK_ALIGN(BLE_OS_TEST_STACK_SIZE));

    os_start();

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

static int
ble_os_disc_test_cb(struct ble_gap_event *event, void *arg)
{
    int *cb_called;

    cb_called = arg;
    *cb_called = 1;

    TEST_ASSERT(event->type == BLE_GAP_EVENT_DISC_COMPLETE);

    return 0;
}

static void
ble_os_disc_test_task_handler(void *arg)
{
    struct ble_gap_disc_params disc_params;
    int cb_called;
    int rc;

    /* Receive acknowledgements for the startup sequence.  We sent the
     * corresponding requests when the host task was started.
     */
    ble_hs_test_util_hci_ack_set_startup();

    /* Set the connect callback so we can verify that it gets called with the
     * proper arguments.
     */
    cb_called = 0;

    os_time_delay(10);

    /* Make sure there are no created connections and no connections in
     * progress.
     */
    TEST_ASSERT(!ble_os_test_misc_conn_exists(BLE_HS_CONN_HANDLE_NONE));
    TEST_ASSERT(!ble_gap_master_in_progress());

    /* Initiate the general discovery procedure with a 300 ms timeout. */
    memset(&disc_params, 0, sizeof disc_params);
    rc = ble_hs_test_util_disc(BLE_OWN_ADDR_PUBLIC, 300, &disc_params,
                               ble_os_disc_test_cb,
                               &cb_called, 0, 0);
    TEST_ASSERT(rc == 0);
    TEST_ASSERT(!ble_os_test_misc_conn_exists(BLE_HS_CONN_HANDLE_NONE));
    TEST_ASSERT(ble_gap_master_in_progress());
    TEST_ASSERT(!cb_called);

    /* Receive acks from the controller. */
    TEST_ASSERT(!ble_os_test_misc_conn_exists(BLE_HS_CONN_HANDLE_NONE));
    TEST_ASSERT(ble_gap_master_in_progress());
    TEST_ASSERT(!cb_called);

    /* Wait 100 ms; verify scan still in progress. */
    os_time_delay(100 * OS_TICKS_PER_SEC / 1000);
    TEST_ASSERT(!ble_os_test_misc_conn_exists(BLE_HS_CONN_HANDLE_NONE));
    TEST_ASSERT(ble_gap_master_in_progress());
    TEST_ASSERT(!cb_called);

    ble_hs_test_util_hci_ack_set(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_SET_SCAN_ENABLE),
        0);

    /* Wait 250 more ms; verify scan completed. */
    os_time_delay(250 * OS_TICKS_PER_SEC / 1000);
    TEST_ASSERT(!ble_os_test_misc_conn_exists(BLE_HS_CONN_HANDLE_NONE));
    TEST_ASSERT(!ble_gap_master_in_progress());
    TEST_ASSERT(cb_called);

    tu_restart();
}

TEST_CASE_SELF(ble_os_disc_test_case)
{
    ble_os_test_misc_init();

    os_task_init(&ble_os_test_task,
                 "ble_os_disc_test_task",
                 ble_os_disc_test_task_handler, NULL,
                 BLE_OS_TEST_TASK_PRIO, OS_WAIT_FOREVER, ble_os_test_stack,
                 OS_STACK_ALIGN(BLE_OS_TEST_STACK_SIZE));

    os_start();

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

static int
ble_gap_terminate_cb(struct ble_gap_event *event, void *arg)
{
    int *disconn_handle;

    ble_os_test_gap_event_type = event->type;

    if (event->type == BLE_GAP_EVENT_DISCONNECT) {
        disconn_handle = arg;
        *disconn_handle = event->disconnect.conn.conn_handle;
    }

    return 0;
}

static void
ble_gap_terminate_test_task_handler(void *arg)
{
    struct hci_disconn_complete disconn_evt;
    struct hci_le_conn_complete conn_evt;
    ble_addr_t addr1 = { BLE_ADDR_PUBLIC, { 1, 2, 3, 4, 5, 6 }};
    ble_addr_t addr2 = { BLE_ADDR_PUBLIC, { 2, 3, 4, 5, 6, 7 }};
    int disconn_handle;
    int rc;

    /* Receive acknowledgements for the startup sequence.  We sent the
     * corresponding requests when the host task was started.
     */
    ble_hs_test_util_hci_ack_set_startup();

    /* Set the connect callback so we can verify that it gets called with the
     * proper arguments.
     */
    disconn_handle = 0;

    /* Make sure there are no created connections and no connections in
     * progress.
     */
    TEST_ASSERT(!ble_os_test_misc_conn_exists(BLE_HS_CONN_HANDLE_NONE));
    TEST_ASSERT(!ble_gap_master_in_progress());

    /* Create two direct connections. */
    ble_hs_test_util_connect(BLE_OWN_ADDR_PUBLIC,
                             &addr1, 0, NULL, ble_gap_terminate_cb,
                             &disconn_handle, 0);
    /* ble_gap_rx_conn_complete() will send extra HCI command, need phony ack */
    ble_hs_test_util_hci_ack_set(ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                             BLE_HCI_OCF_LE_RD_REM_FEAT), 0);
    memset(&conn_evt, 0, sizeof conn_evt);
    conn_evt.subevent_code = BLE_HCI_LE_SUBEV_CONN_COMPLETE;
    conn_evt.status = BLE_ERR_SUCCESS;
    conn_evt.connection_handle = 1;
    memcpy(conn_evt.peer_addr, addr1.val, 6);
    rc = ble_gap_rx_conn_complete(&conn_evt, 0);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_connect(BLE_OWN_ADDR_PUBLIC,
                             &addr2, 0, NULL, ble_gap_terminate_cb,
                             &disconn_handle, 0);
    /* ble_gap_rx_conn_complete() will send extra HCI command, need phony ack */
    ble_hs_test_util_hci_ack_set(ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                             BLE_HCI_OCF_LE_RD_REM_FEAT), 0);
    memset(&conn_evt, 0, sizeof conn_evt);
    conn_evt.subevent_code = BLE_HCI_LE_SUBEV_CONN_COMPLETE;
    conn_evt.status = BLE_ERR_SUCCESS;
    conn_evt.connection_handle = 2;
    memcpy(conn_evt.peer_addr, addr2.val, 6);
    rc = ble_gap_rx_conn_complete(&conn_evt, 0);
    TEST_ASSERT(rc == 0);

    TEST_ASSERT_FATAL(ble_os_test_misc_conn_exists(1));
    TEST_ASSERT_FATAL(ble_os_test_misc_conn_exists(2));

    /* Terminate the first one. */
    rc = ble_hs_test_util_conn_terminate(1, 0);
    TEST_ASSERT(rc == 0);
    disconn_evt.connection_handle = 1;
    disconn_evt.status = 0;
    disconn_evt.reason = BLE_ERR_REM_USER_CONN_TERM;
    ble_hs_test_util_hci_rx_disconn_complete_event(&disconn_evt);
    TEST_ASSERT(ble_os_test_gap_event_type == BLE_GAP_EVENT_DISCONNECT);
    TEST_ASSERT(disconn_handle == 1);
    TEST_ASSERT_FATAL(!ble_os_test_misc_conn_exists(1));
    TEST_ASSERT_FATAL(ble_os_test_misc_conn_exists(2));

    /* Terminate the second one. */
    rc = ble_hs_test_util_conn_terminate(2, 0);
    TEST_ASSERT(rc == 0);
    disconn_evt.connection_handle = 2;
    disconn_evt.status = 0;
    disconn_evt.reason = BLE_ERR_REM_USER_CONN_TERM;
    ble_hs_test_util_hci_rx_disconn_complete_event(&disconn_evt);
    TEST_ASSERT(ble_os_test_gap_event_type == BLE_GAP_EVENT_DISCONNECT);
    TEST_ASSERT(disconn_handle == 2);
    TEST_ASSERT_FATAL(!ble_os_test_misc_conn_exists(1));
    TEST_ASSERT_FATAL(!ble_os_test_misc_conn_exists(2));

    tu_restart();
}

static void
ble_os_test_app_task_handler(void *arg)
{
    while (1) {
        os_eventq_run(os_eventq_dflt_get());
    }
}

TEST_CASE_SELF(ble_gap_terminate_test_case)
{
    ble_os_test_misc_init();

    os_task_init(&ble_os_test_task,
                 "ble_gap_terminate_test_task",
                 ble_gap_terminate_test_task_handler, NULL,
                 BLE_OS_TEST_TASK_PRIO, OS_WAIT_FOREVER, ble_os_test_stack,
                 OS_STACK_ALIGN(BLE_OS_TEST_STACK_SIZE));

    os_start();

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_os_test_suite)
{
    ble_os_disc_test_case();
    ble_gap_direct_connect_test_case();
    ble_gap_terminate_test_case();
}
