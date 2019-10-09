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
#include <string.h>
#include "testutil/testutil.h"
#include "ble_hs_test.h"
#include "host/ble_uuid.h"
#include "ble_hs_test_util.h"

TEST_CASE_SELF(ble_uuid_test)
{
    uint8_t buf_16[2] = { 0x00, 0x18 };
    uint8_t buf_128[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    const ble_uuid_t *uuid16_1 = BLE_UUID16_DECLARE(0x1800);
    const ble_uuid_t *uuid16_2 = BLE_UUID16_DECLARE(0x1801);

    const ble_uuid_t *uuid128_1 =
        BLE_UUID128_DECLARE(0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF);
    const ble_uuid_t *uuid128_2 =
        BLE_UUID128_DECLARE(0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xEE);

    ble_uuid_any_t uuid;
    int rc;

    rc = ble_uuid_init_from_buf(&uuid, buf_16, 2);
    TEST_ASSERT(rc == 0);

    rc = ble_uuid_cmp(&uuid.u, uuid16_1);
    TEST_ASSERT(rc == 0);

    rc = ble_uuid_cmp(&uuid.u, uuid16_2);
    TEST_ASSERT(rc != 0);

    rc = ble_uuid_cmp(uuid16_1, uuid16_2);
    TEST_ASSERT(rc != 0);

    rc = ble_uuid_init_from_buf(&uuid, buf_128, 16);
    TEST_ASSERT(rc == 0);

    rc = ble_uuid_cmp(&uuid.u, uuid128_1);
    TEST_ASSERT(rc == 0);

    rc = ble_uuid_cmp(&uuid.u, uuid128_2);
    TEST_ASSERT(rc != 0);

    rc = ble_uuid_cmp(uuid128_1, uuid128_2);
    TEST_ASSERT(rc != 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_uuid_test_suite)
{
    ble_uuid_test();
}
