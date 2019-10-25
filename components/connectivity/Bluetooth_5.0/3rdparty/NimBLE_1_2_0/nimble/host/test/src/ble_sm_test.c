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
#include <errno.h>
#include "testutil/testutil.h"
#include "nimble/hci_common.h"
#include "nimble/nimble_opt.h"
#include "host/ble_sm.h"
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"
#include "ble_sm_test_util.h"

#if NIMBLE_BLE_SM

/*****************************************************************************
 * $misc                                                                     *
 *****************************************************************************/

TEST_CASE_SELF(ble_sm_test_case_f4)
{
	uint8_t u[32] = { 0xe6, 0x9d, 0x35, 0x0e, 0x48, 0x01, 0x03, 0xcc,
			  0xdb, 0xfd, 0xf4, 0xac, 0x11, 0x91, 0xf4, 0xef,
			  0xb9, 0xa5, 0xf9, 0xe9, 0xa7, 0x83, 0x2c, 0x5e,
			  0x2c, 0xbe, 0x97, 0xf2, 0xd2, 0x03, 0xb0, 0x20 };
	uint8_t v[32] = { 0xfd, 0xc5, 0x7f, 0xf4, 0x49, 0xdd, 0x4f, 0x6b,
			  0xfb, 0x7c, 0x9d, 0xf1, 0xc2, 0x9a, 0xcb, 0x59,
			  0x2a, 0xe7, 0xd4, 0xee, 0xfb, 0xfc, 0x0a, 0x90,
			  0x9a, 0xbb, 0xf6, 0x32, 0x3d, 0x8b, 0x18, 0x55 };
	uint8_t x[16] = { 0xab, 0xae, 0x2b, 0x71, 0xec, 0xb2, 0xff, 0xff,
			  0x3e, 0x73, 0x77, 0xd1, 0x54, 0x84, 0xcb, 0xd5 };
	uint8_t z = 0x00;
	uint8_t exp[16] = { 0x2d, 0x87, 0x74, 0xa9, 0xbe, 0xa1, 0xed, 0xf1,
			    0x1c, 0xbd, 0xa9, 0x07, 0xf1, 0x16, 0xc9, 0xf2 };
	uint8_t res[16];
	int err;

	err = ble_sm_alg_f4(u, v, x, z, res);
	TEST_ASSERT_FATAL(err == 0);
    TEST_ASSERT(memcmp(res, exp, 16) == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_sm_test_case_f5)
{
	uint8_t w[32] = { 0x98, 0xa6, 0xbf, 0x73, 0xf3, 0x34, 0x8d, 0x86,
			  0xf1, 0x66, 0xf8, 0xb4, 0x13, 0x6b, 0x79, 0x99,
			  0x9b, 0x7d, 0x39, 0x0a, 0xa6, 0x10, 0x10, 0x34,
			  0x05, 0xad, 0xc8, 0x57, 0xa3, 0x34, 0x02, 0xec };
	uint8_t n1[16] = { 0xab, 0xae, 0x2b, 0x71, 0xec, 0xb2, 0xff, 0xff,
			   0x3e, 0x73, 0x77, 0xd1, 0x54, 0x84, 0xcb, 0xd5 };
	uint8_t n2[16] = { 0xcf, 0xc4, 0x3d, 0xff, 0xf7, 0x83, 0x65, 0x21,
			   0x6e, 0x5f, 0xa7, 0x25, 0xcc, 0xe7, 0xe8, 0xa6 };
    uint8_t a1t = 0x00;
	uint8_t a1[6] = { 0xce, 0xbf, 0x37, 0x37, 0x12, 0x56 };
    uint8_t a2t = 0x00;
    uint8_t a2[6] = { 0xc1, 0xcf, 0x2d, 0x70, 0x13, 0xa7 };
	uint8_t exp_ltk[16] = { 0x38, 0x0a, 0x75, 0x94, 0xb5, 0x22, 0x05,
				0x98, 0x23, 0xcd, 0xd7, 0x69, 0x11, 0x79,
				0x86, 0x69 };
	uint8_t exp_mackey[16] = { 0x20, 0x6e, 0x63, 0xce, 0x20, 0x6a, 0x3f,
				   0xfd, 0x02, 0x4a, 0x08, 0xa1, 0x76, 0xf1,
				   0x65, 0x29 };
	uint8_t mackey[16], ltk[16];
	int err;

	err = ble_sm_alg_f5(w, n1, n2, a1t, a1, a2t, a2, mackey, ltk);
	TEST_ASSERT_FATAL(err == 0);
    TEST_ASSERT(memcmp(mackey, exp_mackey, 16) == 0);
    TEST_ASSERT(memcmp(ltk, exp_ltk, 16) == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_sm_test_case_f6)
{
	uint8_t w[16] = { 0x20, 0x6e, 0x63, 0xce, 0x20, 0x6a, 0x3f, 0xfd,
			  0x02, 0x4a, 0x08, 0xa1, 0x76, 0xf1, 0x65, 0x29 };
	uint8_t n1[16] = { 0xab, 0xae, 0x2b, 0x71, 0xec, 0xb2, 0xff, 0xff,
			   0x3e, 0x73, 0x77, 0xd1, 0x54, 0x84, 0xcb, 0xd5 };
	uint8_t n2[16] = { 0xcf, 0xc4, 0x3d, 0xff, 0xf7, 0x83, 0x65, 0x21,
			   0x6e, 0x5f, 0xa7, 0x25, 0xcc, 0xe7, 0xe8, 0xa6 };
	uint8_t r[16] = { 0xc8, 0x0f, 0x2d, 0x0c, 0xd2, 0x42, 0xda, 0x08,
			  0x54, 0xbb, 0x53, 0xb4, 0x3b, 0x34, 0xa3, 0x12 };
	uint8_t io_cap[3] = { 0x02, 0x01, 0x01 };
    uint8_t a1t = 0x00;
	uint8_t a1[6] = { 0xce, 0xbf, 0x37, 0x37, 0x12, 0x56 };
    uint8_t a2t = 0x00;
    uint8_t a2[6] = { 0xc1, 0xcf, 0x2d, 0x70, 0x13, 0xa7 };
	uint8_t exp[16] = { 0x61, 0x8f, 0x95, 0xda, 0x09, 0x0b, 0x6c, 0xd2,
			    0xc5, 0xe8, 0xd0, 0x9c, 0x98, 0x73, 0xc4, 0xe3 };
	uint8_t res[16];
	int err;

	err = ble_sm_alg_f6(w, n1, n2, r, io_cap, a1t, a1, a2t, a2, res);
	TEST_ASSERT_FATAL(err == 0);
    TEST_ASSERT(memcmp(res, exp, 16) == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_sm_test_case_g2)
{
	uint8_t u[32] = { 0xe6, 0x9d, 0x35, 0x0e, 0x48, 0x01, 0x03, 0xcc,
			  0xdb, 0xfd, 0xf4, 0xac, 0x11, 0x91, 0xf4, 0xef,
			  0xb9, 0xa5, 0xf9, 0xe9, 0xa7, 0x83, 0x2c, 0x5e,
			  0x2c, 0xbe, 0x97, 0xf2, 0xd2, 0x03, 0xb0, 0x20 };
	uint8_t v[32] = { 0xfd, 0xc5, 0x7f, 0xf4, 0x49, 0xdd, 0x4f, 0x6b,
			  0xfb, 0x7c, 0x9d, 0xf1, 0xc2, 0x9a, 0xcb, 0x59,
			  0x2a, 0xe7, 0xd4, 0xee, 0xfb, 0xfc, 0x0a, 0x90,
			  0x9a, 0xbb, 0xf6, 0x32, 0x3d, 0x8b, 0x18, 0x55 };
	uint8_t x[16] = { 0xab, 0xae, 0x2b, 0x71, 0xec, 0xb2, 0xff, 0xff,
			  0x3e, 0x73, 0x77, 0xd1, 0x54, 0x84, 0xcb, 0xd5 };
	uint8_t y[16] = { 0xcf, 0xc4, 0x3d, 0xff, 0xf7, 0x83, 0x65, 0x21,
			  0x6e, 0x5f, 0xa7, 0x25, 0xcc, 0xe7, 0xe8, 0xa6 };
	uint32_t exp_val = 0x2f9ed5ba % 1000000;
	uint32_t val;
	int err;

	err = ble_sm_alg_g2(u, v, x, y, &val);
	TEST_ASSERT_FATAL(err == 0);
	TEST_ASSERT(val == exp_val);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_sm_test_case_conn_broken)
{
    struct hci_disconn_complete disconn_evt;
    int rc;

    ble_sm_test_util_init();

    ble_sm_dbg_set_next_pair_rand(((uint8_t[16]){0}));

    ble_hs_test_util_create_conn(2, ((uint8_t[6]){1,2,3,5,6,7}),
                                 ble_sm_test_util_conn_cb, NULL);

    /* Initiate the pairing procedure. */
    rc = ble_hs_test_util_security_initiate(2, 0);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, BLE_SM_IOACT_NONE);

    /* Terminate the connection. */
    disconn_evt.connection_handle = 2;
    disconn_evt.status = 0;
    disconn_evt.reason = BLE_ERR_REM_USER_CONN_TERM;
    ble_gap_rx_disconn_complete(&disconn_evt);

    /* Verify security callback got called. */
    TEST_ASSERT(ble_sm_test_gap_status == BLE_HS_ENOTCONN);
    TEST_ASSERT(!ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(!ble_sm_test_sec_state.authenticated);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*****************************************************************************
 * $peer                                                                     *
 *****************************************************************************/

TEST_CASE_SELF(ble_sm_test_case_peer_fail_inval)
{
    /* Invalid role detected before other arguments. */
    ble_sm_test_util_peer_fail_inval(
        1,
        ((uint8_t[]){0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c}),
        ((uint8_t[]){0x03, 0x02, 0x01, 0x50, 0x13, 0x00}),
        ((struct ble_sm_pair_cmd[1]) { {
            .io_cap = 0x14,
            .oob_data_flag = 0,
            .authreq = 0x12,
            .max_enc_key_size = 20,
            .init_key_dist = 0x0b,
            .resp_key_dist = 0x11,
        } }),
        ((struct ble_sm_pair_fail[1]) { {
            .reason = BLE_SM_ERR_CMD_NOT_SUPP,
        } })
    );

    /* Invalid key size - too small. */
    ble_sm_test_util_peer_fail_inval(
        0,
        ((uint8_t[]){0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c}),
        ((uint8_t[]){0x03, 0x02, 0x01, 0x50, 0x13, 0x00}),
        ((struct ble_sm_pair_cmd[1]) { {
            .io_cap = 0x04,
            .oob_data_flag = 0,
            .authreq = 0x5,
            .max_enc_key_size = 6,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        } }),
        ((struct ble_sm_pair_fail[1]) { {
            .reason = BLE_SM_ERR_ENC_KEY_SZ,
        } })
    );

    /* Invalid key size - too large. */
    ble_sm_test_util_peer_fail_inval(
        0,
        ((uint8_t[]){0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c}),
        ((uint8_t[]){0x03, 0x02, 0x01, 0x50, 0x13, 0x00}),
        ((struct ble_sm_pair_cmd[1]) { {
            .io_cap = 0x04,
            .oob_data_flag = 0,
            .authreq = 0x5,
            .max_enc_key_size = 17,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        } }),
        ((struct ble_sm_pair_fail[1]) { {
            .reason = BLE_SM_ERR_INVAL,
        } })
    );

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_sm_test_case_peer_lgcy_fail_confirm)
{
    ble_sm_test_util_peer_lgcy_fail_confirm(
        ((uint8_t[]){0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c}),
        ((uint8_t[]){0x03, 0x02, 0x01, 0x50, 0x13, 0x00}),
        ((struct ble_sm_pair_cmd[1]) { {
            .io_cap = 0x04,
            .oob_data_flag = 0,
            .authreq = 0x05,
            .max_enc_key_size = 16,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        } }),
        ((struct ble_sm_pair_cmd[1]) { {
            .io_cap = 3,
            .oob_data_flag = 0,
            .authreq = 0,
            .max_enc_key_size = 16,
            .init_key_dist = 0,
            .resp_key_dist = 0,
        } }),
        ((struct ble_sm_pair_confirm[1]) { {
            .value = {
                0x0a, 0xac, 0xa2, 0xae, 0xa6, 0x98, 0xdc, 0x6d,
                0x65, 0x84, 0x11, 0x69, 0x47, 0x36, 0x8d, 0xa0,
            },
        } }),
        ((struct ble_sm_pair_confirm[1]) { {
            .value = {
                0x45, 0xd2, 0x2c, 0x38, 0xd8, 0x91, 0x4f, 0x19,
                0xa2, 0xd4, 0xfc, 0x7d, 0xad, 0x37, 0x79, 0xe0
            },
        } }),
        ((struct ble_sm_pair_random[1]) { {
            .value = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            },
        } }),
        ((struct ble_sm_pair_random[1]) { {
            .value = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            },
        } }),
        ((struct ble_sm_pair_fail[1]) { {
            .reason = BLE_SM_ERR_CONFIRM_MISMATCH,
        } })
    );

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_sm_test_case_peer_bonding_bad)
{
    ble_sm_test_util_peer_bonding_bad(0x5684, 32);
    ble_sm_test_util_peer_bonding_bad(54325, 65437);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_sm_test_case_peer_sec_req_inval)
{
    struct ble_sm_pair_fail fail;
    struct ble_sm_sec_req sec_req;
    int rc;

    ble_sm_test_util_init();

    ble_sm_dbg_set_next_pair_rand(((uint8_t[16]){0}));

    ble_hs_test_util_create_conn(2, ((uint8_t[6]){1,2,3,5,6,7}),
                                 ble_sm_test_util_conn_cb,
                                 NULL);

    /*** We are the slave; reject the security request. */
    ble_hs_atomic_conn_set_flags(2, BLE_HS_CONN_F_MASTER, 0);

    sec_req.authreq = 0;
    ble_sm_test_util_rx_sec_req(
        2, &sec_req, BLE_HS_SM_US_ERR(BLE_SM_ERR_CMD_NOT_SUPP));

    fail.reason = BLE_SM_ERR_CMD_NOT_SUPP;
    ble_sm_test_util_verify_tx_pair_fail(&fail);

    /*** Pairing already in progress; ignore security request. */
    ble_hs_atomic_conn_set_flags(2, BLE_HS_CONN_F_MASTER, 1);
    rc = ble_sm_pair_initiate(2);
    TEST_ASSERT_FATAL(rc == 0);
    ble_hs_test_util_prev_tx_queue_clear();

    ble_sm_test_util_rx_sec_req(2, &sec_req, BLE_HS_EALREADY);
    TEST_ASSERT(ble_hs_test_util_prev_tx_queue_sz() == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*****************************************************************************
 * $us                                                                       *
 *****************************************************************************/

TEST_CASE_SELF(ble_sm_test_case_us_fail_inval)
{
    struct ble_sm_test_params params;

    /* Invalid key size - too small. */
    params = (struct ble_sm_test_params) {
        .init_id_addr = {0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c},
        .resp_id_addr = {0x03, 0x02, 0x01, 0x50, 0x13, 0x00},
        .pair_req = (struct ble_sm_pair_cmd) {
            .io_cap = 3,
            .oob_data_flag = 0,
            .authreq = 0,
            .max_enc_key_size = 16,
            .init_key_dist = 0,
            .resp_key_dist = 0,
        },
        .pair_rsp = (struct ble_sm_pair_cmd) {
            .io_cap = 0x04,
            .oob_data_flag = 0,
            .authreq = 0x05,
            .max_enc_key_size = 6,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .pair_fail = (struct ble_sm_pair_fail) {
            .reason = BLE_SM_ERR_ENC_KEY_SZ,
        },
    };
    ble_sm_test_util_us_fail_inval(&params);

    /* Invalid key size - too large. */
    params = (struct ble_sm_test_params) {
        .init_id_addr = {0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c},
        .resp_id_addr = {0x03, 0x02, 0x01, 0x50, 0x13, 0x00},
        .pair_req = (struct ble_sm_pair_cmd) {
            .io_cap = 3,
            .oob_data_flag = 0,
            .authreq = 0,
            .max_enc_key_size = 16,
            .init_key_dist = 0,
            .resp_key_dist = 0,
        },
        .pair_rsp = (struct ble_sm_pair_cmd) {
            .io_cap = 0x04,
            .oob_data_flag = 0,
            .authreq = 0x05,
            .max_enc_key_size = 17,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .pair_fail = (struct ble_sm_pair_fail) {
            .reason = BLE_SM_ERR_INVAL,
        },
    };
    ble_sm_test_util_us_fail_inval(&params);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_sm_gen_test_suite)
{
    ble_sm_test_case_f4();
    ble_sm_test_case_f5();
    ble_sm_test_case_f6();
    ble_sm_test_case_g2();

    ble_sm_test_case_peer_fail_inval();
    ble_sm_test_case_peer_lgcy_fail_confirm();
    ble_sm_test_case_us_fail_inval();
    ble_sm_test_case_peer_bonding_bad();
    ble_sm_test_case_conn_broken();
    ble_sm_test_case_peer_sec_req_inval();
}
#endif
