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

#include "sysinit/sysinit.h"
#include "syscfg/syscfg.h"
#include "os/os.h"
#include "nimble/hci_common.h"
#include "testutil/testutil.h"
#include "ble_hs_test_util.h"
#include "ble_hs_test.h"

#if MYNEWT_VAL(SELFTEST)

int
main(int argc, char **argv)
{
    /* XXX: This test must come before the others; it causes privacy to be
     * enabled.  Subsequent tests depend on this.  This is wrong - each test
     * should enable privacy as needed, but the test util functions are so low
     * level that they make this very difficult to arrange (individual HCI
     * commands and responses).
     *
     * To fix this, we should implement a set of higher level BLE test
     * functions that don't require individual HCI commands to be specified.
     */
    ble_gap_test_suite_disc();

    ble_att_clt_suite();
    ble_att_svr_suite();
    ble_gap_test_suite_adv();
    ble_gap_test_suite_conn_cancel();
    ble_gap_test_suite_conn_find();
    ble_gap_test_suite_conn_gen();
    ble_gap_test_suite_conn_terminate();
    ble_gap_test_suite_mtu();
    ble_gap_test_suite_set_cb();
    ble_gap_test_suite_stop_adv();
    ble_gap_test_suite_timeout();
    ble_gap_test_suite_update_conn();
    ble_gap_test_suite_wl();
    ble_gatt_conn_suite();
    ble_gatt_disc_c_test_suite();
    ble_gatt_disc_d_test_suite();
    ble_gatt_disc_s_test_suite();
    ble_gatt_find_s_test_suite();
    ble_gatt_read_test_suite();
    ble_gatt_write_test_suite();
    ble_gatts_notify_suite();
    ble_gatts_read_test_suite();
    ble_gatts_reg_suite();
    ble_hs_adv_test_suite();
    ble_hs_conn_suite();
    ble_hs_hci_suite();
    ble_hs_id_test_suite_auto();
    ble_hs_pvcy_test_suite_irk();
    ble_l2cap_test_suite();
    ble_os_test_suite();
    ble_sm_gen_test_suite();
    ble_sm_lgcy_test_suite();
    ble_sm_sc_test_suite();
    ble_store_suite();
    ble_uuid_test_suite();

    return tu_any_failed;
}

#endif
