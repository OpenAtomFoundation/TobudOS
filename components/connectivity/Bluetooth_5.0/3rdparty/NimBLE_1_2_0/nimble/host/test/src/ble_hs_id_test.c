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
ble_hs_id_test_util_infer_auto(int privacy, uint8_t *own_addr_type)
{
    int rc;

    rc = ble_hs_id_infer_auto(privacy, own_addr_type);

    return rc;
}

TEST_CASE_SELF(ble_hs_id_test_case_auto_none)
{
    uint8_t own_addr_type;
    int rc;

    ble_hs_test_util_init();

    /* Clear public address. */
    ble_hs_id_set_pub((uint8_t[6]){ 0, 0, 0, 0, 0, 0 });

    rc = ble_hs_id_test_util_infer_auto(0, &own_addr_type);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOADDR);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_id_test_case_auto_public)
{
    uint8_t own_addr_type;
    int rc;

    ble_hs_test_util_init();

    rc = ble_hs_id_test_util_infer_auto(0, &own_addr_type);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(own_addr_type == BLE_OWN_ADDR_PUBLIC);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_id_test_case_auto_random)
{
    uint8_t own_addr_type;
    int rc;

    ble_hs_test_util_init();

    /* Configure a random address. */
    ble_hs_test_util_set_static_rnd_addr((uint8_t[6]){ 1, 2, 3, 4, 5, 0xc0 });

    rc = ble_hs_id_test_util_infer_auto(0, &own_addr_type);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(own_addr_type == BLE_OWN_ADDR_RANDOM);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_id_test_case_auto_rpa_pub)
{
    uint8_t own_addr_type;
    int rc;

    ble_hs_test_util_init();

    rc = ble_hs_id_test_util_infer_auto(1, &own_addr_type);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(own_addr_type == BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_id_test_case_auto_rpa_rnd)
{
    uint8_t own_addr_type;
    int rc;

    ble_hs_test_util_init();

    /* Configure a random address. */
    ble_hs_test_util_set_static_rnd_addr((uint8_t[6]){ 1, 2, 3, 4, 5, 0xc0 });

    rc = ble_hs_id_test_util_infer_auto(1, &own_addr_type);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(own_addr_type == BLE_OWN_ADDR_RPA_RANDOM_DEFAULT);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_hs_id_test_suite_auto)
{
    ble_hs_id_test_case_auto_none();
    ble_hs_id_test_case_auto_public();
    ble_hs_id_test_case_auto_random();
    ble_hs_id_test_case_auto_rpa_pub();
    ble_hs_id_test_case_auto_rpa_rnd();
}
