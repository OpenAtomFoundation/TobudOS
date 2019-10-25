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
#include "controller/ble_ll_test.h"
#include "controller/ble_ll_conn.h"
#include "ble_ll_csa2_test.h"

TEST_CASE_SELF(ble_ll_csa2_test_1)
{
    struct ble_ll_conn_sm conn;
    uint8_t rc;

    /*
     * Note: This test only verified mapped channel. Sample data also specifies
     * prn_e and unmapped channel values but those would require extra access
     * to internal state of algorithm which is not exposed.
     */

    memset(&conn, 0, sizeof(conn));

    CONN_F_CSA2_SUPP(&conn) = 1;

    /*
     * based on sample data from CoreSpec 5.0 Vol 6 Part C 3.1
     * (all channels used)
     */
    conn.channel_id = ((0x8e89bed6 & 0xffff0000) >> 16) ^
                       (0x8e89bed6 & 0x0000ffff);

    conn.num_used_chans = 37;
    conn.chanmap[0] = 0xff;
    conn.chanmap[1] = 0xff;
    conn.chanmap[2] = 0xff;
    conn.chanmap[3] = 0xff;
    conn.chanmap[4] = 0x1f;

    conn.event_cntr = 1;
    rc = ble_ll_conn_calc_dci(&conn, 0);
    TEST_ASSERT(rc == 20);

    conn.event_cntr = 2;
    rc = ble_ll_conn_calc_dci(&conn, 0);
    TEST_ASSERT(rc == 6);

    conn.event_cntr = 3;
    rc = ble_ll_conn_calc_dci(&conn, 0);
    TEST_ASSERT(rc == 21);
}

TEST_CASE_SELF(ble_ll_csa2_test_2)
{
    struct ble_ll_conn_sm conn;
    uint8_t rc;

    /*
     * Note: This test only verified mapped channel. Sample data also specifies
     * prn_e and unmapped channel values but those would require extra access
     * to internal state of algorithm which is not exposed.
     */

    memset(&conn, 0, sizeof(conn));

    CONN_F_CSA2_SUPP(&conn) = 1;

    /*
     * based on sample data from CoreSpec 5.0 Vol 6 Part C 3.2
     * (9 channels used)
     */
    conn.channel_id = ((0x8e89bed6 & 0xffff0000) >> 16) ^
                       (0x8e89bed6 & 0x0000ffff);

    conn.num_used_chans = 9;
    conn.chanmap[0] = 0x00;
    conn.chanmap[1] = 0x06;
    conn.chanmap[2] = 0xe0;
    conn.chanmap[3] = 0x00;
    conn.chanmap[4] = 0x1e;

    conn.event_cntr = 6;
    rc = ble_ll_conn_calc_dci(&conn, 0);
    TEST_ASSERT(rc == 23);

    conn.event_cntr = 7;
    rc = ble_ll_conn_calc_dci(&conn, 0);
    TEST_ASSERT(rc == 9);

    conn.event_cntr = 8;
    rc = ble_ll_conn_calc_dci(&conn, 0);
    TEST_ASSERT(rc == 34);
}

TEST_SUITE(ble_ll_csa2_test_suite)
{
    ble_ll_csa2_test_1();
    ble_ll_csa2_test_2();
}
