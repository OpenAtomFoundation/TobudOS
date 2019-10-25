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

#ifndef H_BLE_HS_TEST_
#define H_BLE_HS_TEST_

#include "testutil/testutil.h"

TEST_SUITE_DECL(ble_att_clt_suite);
TEST_SUITE_DECL(ble_att_svr_suite);
TEST_SUITE_DECL(ble_gap_test_suite_adv);
TEST_SUITE_DECL(ble_gap_test_suite_conn_cancel);
TEST_SUITE_DECL(ble_gap_test_suite_conn_find);
TEST_SUITE_DECL(ble_gap_test_suite_conn_gen);
TEST_SUITE_DECL(ble_gap_test_suite_conn_terminate);
TEST_SUITE_DECL(ble_gap_test_suite_disc);
TEST_SUITE_DECL(ble_gap_test_suite_mtu);
TEST_SUITE_DECL(ble_gap_test_suite_set_cb);
TEST_SUITE_DECL(ble_gap_test_suite_stop_adv);
TEST_SUITE_DECL(ble_gap_test_suite_timeout);
TEST_SUITE_DECL(ble_gap_test_suite_update_conn);
TEST_SUITE_DECL(ble_gap_test_suite_wl);
TEST_SUITE_DECL(ble_gatt_conn_suite);
TEST_SUITE_DECL(ble_gatt_disc_c_test_suite);
TEST_SUITE_DECL(ble_gatt_disc_d_test_suite);
TEST_SUITE_DECL(ble_gatt_disc_s_test_suite);
TEST_SUITE_DECL(ble_gatt_find_s_test_suite);
TEST_SUITE_DECL(ble_gatt_read_test_suite);
TEST_SUITE_DECL(ble_gatt_write_test_suite);
TEST_SUITE_DECL(ble_gatts_notify_suite);
TEST_SUITE_DECL(ble_gatts_read_test_suite);
TEST_SUITE_DECL(ble_gatts_reg_suite);
TEST_SUITE_DECL(ble_hs_adv_test_suite);
TEST_SUITE_DECL(ble_hs_conn_suite);
TEST_SUITE_DECL(ble_hs_hci_suite);
TEST_SUITE_DECL(ble_hs_id_test_suite_auto);
TEST_SUITE_DECL(ble_hs_pvcy_test_suite_irk);
TEST_SUITE_DECL(ble_l2cap_test_suite);
TEST_SUITE_DECL(ble_os_test_suite);
TEST_SUITE_DECL(ble_sm_gen_test_suite);
TEST_SUITE_DECL(ble_sm_lgcy_test_suite);
TEST_SUITE_DECL(ble_sm_sc_test_suite);
TEST_SUITE_DECL(ble_store_suite);
TEST_SUITE_DECL(ble_uuid_test_suite);

#endif
