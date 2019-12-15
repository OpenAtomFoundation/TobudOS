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

/**
 * Legacy pairing
 * Master: peer
 * Pair algorithm: just works
 * Initiator IO capabilities: 4
 * Responder IO capabilities: 3
 * Bonding: true
 * Initiator address type: BLE_ADDR_PUBLIC
 * Responder address type: BLE_ADDR_PUBLIC
 * Initiator key distribution: 7
 * Responder key distribution: 7
 */
TEST_CASE_SELF(ble_sm_lgcy_peer_jw_iio4_rio3_b1_iat0_rat0_ik7_rk7)
{
    struct ble_sm_test_params params;

    params = (struct ble_sm_test_params) {
        .init_id_addr = {
            0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c,
        },
        .resp_id_addr = {
            0x33, 0x22, 0x11, 0x00, 0x45, 0x0a,
        },
        .pair_req = {
            .io_cap = 0x04,
            .oob_data_flag = 0x00,
            .authreq = 0x05,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .pair_rsp = {
            .io_cap = 0x03,
            .oob_data_flag = 0x00,
            .authreq = 0x09,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .confirm_req[0] = {
            .value = {
                0xcd, 0x5b, 0x79, 0x29, 0x53, 0x31, 0x56, 0x23,
                0x2c, 0x08, 0xed, 0x81, 0x16, 0x55, 0x8e, 0x01,
            },
        },
        .confirm_rsp[0] = {
            .value = {
                0x49, 0x39, 0x22, 0x0f, 0x7b, 0x1b, 0x80, 0xcd,
                0xbe, 0x89, 0xd1, 0x4c, 0xbd, 0x6f, 0xda, 0x2c,
            },
        },
        .random_req[0] = {
            .value = {
                0x7f, 0x42, 0xc0, 0x2f, 0x1d, 0x07, 0x37, 0xfc,
                0x04, 0x5b, 0x05, 0x9a, 0xed, 0x67, 0xa5, 0x68,
            },
        },
        .random_rsp[0] = {
            .value = {
                0x42, 0x1a, 0x58, 0xa2, 0x3b, 0x80, 0xde, 0xef,
                0x95, 0x0d, 0xf7, 0xca, 0x06, 0x05, 0x01, 0x3c,
            },
        },
        .enc_info_req = {
            .ltk = {
                0x2f, 0x9b, 0x16, 0xff, 0xf3, 0x73, 0x30, 0x08,
                0xa8, 0xe5, 0x01, 0xb1, 0x3b, 0xe1, 0x87, 0x00,
            },
        },
        .master_id_req = {
            .ediv = 0xf8e0,
            .rand_val = 0xef7c818b00000000,
        },
        .id_info_req = {
            .irk = {
                0xef, 0x8d, 0xe2, 0x16, 0x4f, 0xec, 0x43, 0x0d,
                0xbf, 0x5b, 0xdd, 0x34, 0xc0, 0x53, 0x1e, 0xb8,
            },
        },
        .id_addr_info_req = {
            .addr_type = 0,
            .bd_addr = {
                0x33, 0x22, 0x11, 0x00, 0x45, 0x0a,
            },
        },
        .sign_info_req = {
            .sig_key = {
                0xc6, 0x17, 0xc0, 0x02, 0x40, 0x0d, 0x27, 0x51,
                0x8a, 0x77, 0xb5, 0xae, 0xd8, 0xa9, 0x7a, 0x7a,
            },
        },
        .enc_info_rsp = {
            .ltk = {
                0xd7, 0x07, 0x22, 0x79, 0x24, 0xc6, 0xcb, 0x4d,
                0xa3, 0xdd, 0x01, 0xfb, 0x48, 0x87, 0xd4, 0xcf,
            },
        },
        .master_id_rsp = {
            .ediv = 0x9a39,
            .rand_val = 0x8e76d9b00000000,
        },
        .id_info_rsp = {
            .irk = {
                0xeb, 0x8a, 0x06, 0xc4, 0x93, 0x51, 0x04, 0xb3,
                0x8b, 0xbf, 0xe8, 0x1f, 0x0e, 0x96, 0x2a, 0x54,
            },
        },
        .id_addr_info_rsp = {
            .addr_type = 0,
            .bd_addr = {
                0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c,
            },
        },
        .sign_info_rsp = {
            .sig_key = {
                0x14, 0x55, 0x93, 0xe1, 0xd1, 0xe7, 0xc4, 0x5d,
                0x35, 0x97, 0xd3, 0x05, 0x30, 0xc8, 0x9d, 0x83,
            },
        },
        .stk = {
            0x1c, 0xd7, 0xb6, 0x35, 0x48, 0xfc, 0x9f, 0xef,
            0x0e, 0x2f, 0x51, 0x77, 0xed, 0xdd, 0xbc, 0xaf,
        },
        .pair_alg = 0,
        .authenticated = false,
        .passkey_info = {
            .passkey = {
                .action = BLE_SM_IOACT_NONE,
            },
        },
    };
    ble_sm_test_util_peer_lgcy_good(&params);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/**
 * Legacy pairing
 * Master: peer
 * Pair algorithm: passkey entry
 * Initiator IO capabilities: 4
 * Responder IO capabilities: 0
 * Bonding: true
 * Initiator address type: BLE_ADDR_PUBLIC
 * Responder address type: BLE_ADDR_PUBLIC
 * Initiator key distribution: 7
 * Responder key distribution: 7
 */
TEST_CASE_SELF(ble_sm_lgcy_peer_pk_iio4_rio0_b1_iat0_rat0_ik7_rk7)
{
    struct ble_sm_test_params params;

    params = (struct ble_sm_test_params) {
        .init_id_addr = {
            0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c,
        },
        .resp_id_addr = {
            0x33, 0x22, 0x11, 0x00, 0x45, 0x0a,
        },
        .pair_req = {
            .io_cap = 0x04,
            .oob_data_flag = 0x00,
            .authreq = 0x05,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .pair_rsp = {
            .io_cap = 0x00,
            .oob_data_flag = 0x00,
            .authreq = 0x09,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .confirm_req[0] = {
            .value = {
                0xa0, 0x10, 0x4a, 0xaa, 0x8b, 0x53, 0x78, 0xbb,
                0xd2, 0xae, 0x71, 0x1f, 0x4e, 0x00, 0x70, 0x8b,
            },
        },
        .confirm_rsp[0] = {
            .value = {
                0x62, 0xf3, 0xba, 0x0e, 0xe5, 0xbe, 0x2e, 0xd8,
                0x25, 0xb2, 0xec, 0x4c, 0x28, 0x77, 0x28, 0x60,
            },
        },
        .random_req[0] = {
            .value = {
                0x84, 0xcf, 0xe4, 0x04, 0x7d, 0xf3, 0xfc, 0xa1,
                0x3f, 0x75, 0xd6, 0x5a, 0x7c, 0xb7, 0xa4, 0x39,
            },
        },
        .random_rsp[0] = {
            .value = {
                0xef, 0x6a, 0x61, 0x6e, 0x02, 0x60, 0x7f, 0x5d,
                0x7f, 0x0d, 0xa6, 0x3c, 0x06, 0x1a, 0x5d, 0xd6,
            },
        },
        .enc_info_req = {
            .ltk = {
                0xad, 0x01, 0x6d, 0x76, 0xa9, 0xd0, 0x23, 0xc9,
                0x40, 0x0c, 0xbf, 0x2a, 0x4c, 0x23, 0x31, 0xc5,
            },
        },
        .master_id_req = {
            .ediv = 0xa74f,
            .rand_val = 0x81cab3fd00000000,
        },
        .id_info_req = {
            .irk = {
                0xef, 0x8d, 0xe2, 0x16, 0x4f, 0xec, 0x43, 0x0d,
                0xbf, 0x5b, 0xdd, 0x34, 0xc0, 0x53, 0x1e, 0xb8,
            },
        },
        .id_addr_info_req = {
            .addr_type = 0,
            .bd_addr = {
                0x33, 0x22, 0x11, 0x00, 0x45, 0x0a,
            },
        },
        .sign_info_req = {
            .sig_key = {
                0x60, 0x08, 0x49, 0x00, 0x6d, 0x76, 0x98, 0x73,
                0x9c, 0x95, 0xc4, 0xd9, 0xe8, 0x3a, 0x69, 0xbb,
            },
        },
        .enc_info_rsp = {
            .ltk = {
                0x5b, 0x73, 0x39, 0xd9, 0x51, 0x3d, 0x92, 0xa4,
                0x34, 0x65, 0xa5, 0x70, 0x49, 0xbe, 0x11, 0x28,
            },
        },
        .master_id_rsp = {
            .ediv = 0x9705,
            .rand_val = 0x592f1e8d00000000,
        },
        .id_info_rsp = {
            .irk = {
                0xeb, 0x8a, 0x06, 0xc4, 0x93, 0x51, 0x04, 0xb3,
                0x8b, 0xbf, 0xe8, 0x1f, 0x0e, 0x96, 0x2a, 0x54,
            },
        },
        .id_addr_info_rsp = {
            .addr_type = 0,
            .bd_addr = {
                0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c,
            },
        },
        .sign_info_rsp = {
            .sig_key = {
                0xc9, 0x9b, 0xf2, 0x75, 0xb7, 0x0d, 0xe8, 0x60,
                0x3d, 0xf0, 0xd6, 0xa8, 0x16, 0xc5, 0x6c, 0x2a,
            },
        },
        .stk = {
            0xf2, 0x3c, 0x36, 0xc4, 0xa1, 0xfb, 0x5a, 0xa7,
            0x96, 0x20, 0xe4, 0x29, 0xb7, 0x58, 0x22, 0x7a,
        },
        .pair_alg = 1,
        .authenticated = true,
        .passkey_info = {
            .passkey = {
                .action = BLE_SM_IOACT_DISP,
                .passkey = 46128,
            },
        },
    };
    ble_sm_test_util_peer_lgcy_good(&params);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/**
 * Legacy pairing
 * Master: us
 * Pair algorithm: just works
 * Initiator IO capabilities: 3
 * Responder IO capabilities: 3
 * Bonding: true
 * Initiator address type: BLE_ADDR_PUBLIC
 * Responder address type: BLE_ADDR_RANDOM
 * Initiator key distribution: 7
 * Responder key distribution: 5
 */
TEST_CASE_SELF(ble_sm_lgcy_us_jw_iio3_rio3_b1_iat0_rat1_ik7_rk5)
{
    struct ble_sm_test_params params;

    params = (struct ble_sm_test_params) {
        .init_id_addr = {
            0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a,
        },
        .resp_addr_type = BLE_ADDR_RANDOM,
        .resp_id_addr = {
            0x11, 0x22, 0x11, 0x22, 0x11, 0xcc,
        },
        .pair_req = {
            .io_cap = 0x03,
            .oob_data_flag = 0x00,
            .authreq = 0x01,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .pair_rsp = {
            .io_cap = 0x03,
            .oob_data_flag = 0x00,
            .authreq = 0x01,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x05,
        },
        .confirm_req[0] = {
            .value = {
                0x1c, 0xb6, 0x10, 0xea, 0x02, 0x08, 0x90, 0x64,
                0xc7, 0xf8, 0xe5, 0x9c, 0xb4, 0x3a, 0x18, 0xca,
            },
        },
        .confirm_rsp[0] = {
            .value = {
                0xb8, 0x6f, 0xd1, 0xc6, 0x74, 0x35, 0xa3, 0x94,
                0x68, 0x2f, 0xf1, 0x4c, 0x78, 0x44, 0xe8, 0x0d,
            },
        },
        .random_req[0] = {
            .value = {
                0x40, 0x48, 0x17, 0x4d, 0x42, 0xa0, 0xf8, 0xd5,
                0xbf, 0x65, 0x67, 0xb8, 0x5e, 0x57, 0x38, 0xac,
            },
        },
        .random_rsp[0] = {
            .value = {
                0x2c, 0xa1, 0xb1, 0xf5, 0x54, 0x9b, 0x43, 0xe9,
                0xb0, 0x62, 0x6a, 0xb0, 0x02, 0xb8, 0x6c, 0xca,
            },
        },
        .enc_info_req = {
            .ltk = {
                0x01, 0x15, 0xb6, 0x93, 0xc9, 0xff, 0xfe, 0x27,
                0x02, 0x41, 0xfd, 0x7b, 0x0e, 0x31, 0xd4, 0xa6,
            },
        },
        .master_id_req = {
            .ediv = 0xe4fb,
            .rand_val = 0x8eee76b100000000,
        },
        .sign_info_req = {
            .sig_key = {
                0x00, 0x2d, 0xf6, 0x3e, 0x5e, 0x0f, 0xd1, 0xe8,
                0x4e, 0x5f, 0x61, 0x1c, 0x2c, 0x0b, 0xa5, 0x51,
            },
        },
        .enc_info_rsp = {
            .ltk = {
                0x88, 0xbc, 0x95, 0x8d, 0xaa, 0x26, 0x8d, 0xd5,
                0x18, 0xc9, 0x06, 0x70, 0xc2, 0x30, 0x56, 0x4c,
            },
        },
        .master_id_rsp = {
            .ediv = 0x4413,
            .rand_val = 0xfad1c27300000000,
        },
        .id_info_rsp = {
            .irk = {
                0xef, 0x8d, 0xe2, 0x16, 0x4f, 0xec, 0x43, 0x0d,
                0xbf, 0x5b, 0xdd, 0x34, 0xc0, 0x53, 0x1e, 0xb8,
            },
        },
        .id_addr_info_rsp = {
            .addr_type = 0,
            .bd_addr = {
                0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a,
            },
        },
        .sign_info_rsp = {
            .sig_key = {
                0x03, 0xad, 0xa4, 0xe1, 0x34, 0x76, 0x95, 0x54,
                0xe5, 0x8f, 0xa4, 0x06, 0x72, 0xe6, 0xfc, 0x65,
            },
        },
        .stk = {
            0x31, 0x54, 0x42, 0x6c, 0x1c, 0x03, 0x36, 0x44,
            0x0b, 0x72, 0x90, 0xa5, 0x1f, 0x79, 0x5b, 0xe9,
        },
        .pair_alg = 0,
        .authenticated = false,
        .passkey_info = {
            .passkey = {
                .action = BLE_SM_IOACT_NONE,
            },
        },
    };
    ble_sm_test_util_us_lgcy_good(&params);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/**
 * Legacy pairing
 * Master: us
 * Pair algorithm: passkey entry
 * Initiator IO capabilities: 4
 * Responder IO capabilities: 2
 * Bonding: true
 * Initiator address type: BLE_ADDR_PUBLIC
 * Responder address type: BLE_ADDR_RANDOM
 * Initiator key distribution: 7
 * Responder key distribution: 5
 */
TEST_CASE_SELF(ble_sm_lgcy_us_pk_iio4_rio2_b1_iat0_rat1_ik7_rk5)
{
    struct ble_sm_test_params params;

    params = (struct ble_sm_test_params) {
        .init_id_addr = {
            0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a,
        },
        .resp_addr_type = BLE_ADDR_RANDOM,
        .resp_id_addr = {
            0x11, 0x22, 0x11, 0x22, 0x11, 0xcc,
        },
        .pair_req = {
            .io_cap = 0x04,
            .oob_data_flag = 0x00,
            .authreq = 0x0d,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .pair_rsp = {
            .io_cap = 0x02,
            .oob_data_flag = 0x00,
            .authreq = 0x05,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x05,
        },
        .confirm_req[0] = {
            .value = {
                0xb5, 0xd4, 0xc5, 0xe8, 0xef, 0xef, 0xd8, 0xd7,
                0x2b, 0x14, 0x34, 0x35, 0x29, 0x18, 0xda, 0x12,
            },
        },
        .confirm_rsp[0] = {
            .value = {
                0x1a, 0x03, 0x20, 0xda, 0x60, 0x21, 0x9b, 0x4b,
                0x5d, 0x45, 0x90, 0x64, 0xe1, 0x24, 0x2c, 0xb6,
            },
        },
        .random_req[0] = {
            .value = {
                0x45, 0xa3, 0x1a, 0x0b, 0xf6, 0x0f, 0x7c, 0xcf,
                0x1a, 0xfb, 0xfc, 0x1a, 0xad, 0x62, 0x0e, 0x76,
            },
        },
        .random_rsp[0] = {
            .value = {
                0x82, 0xbb, 0x9f, 0x67, 0xc4, 0x88, 0xcb, 0x58,
                0xee, 0xf9, 0x34, 0x35, 0x23, 0xa3, 0xd0, 0x22,
            },
        },
        .enc_info_req = {
            .ltk = {
                0xfa, 0x43, 0x8f, 0x1f, 0xe6, 0x2a, 0x94, 0x5b,
                0x54, 0x89, 0x2b, 0x0f, 0xd7, 0x23, 0x77, 0x9e,
            },
        },
        .master_id_req = {
            .ediv = 0x88b3,
            .rand_val = 0x7c970e18dec74560,
        },
        .sign_info_req = {
            .sig_key = {
                0x2e, 0x70, 0x3c, 0xbf, 0x20, 0xbe, 0x7d, 0x2d,
                0xb3, 0x50, 0x46, 0x33, 0x4c, 0x20, 0x0e, 0xc8,
            },
        },
        .enc_info_rsp = {
            .ltk = {
                0xc1, 0x64, 0x33, 0x10, 0x0f, 0x70, 0x2f, 0x9c,
                0xe7, 0x31, 0xc5, 0x32, 0xdd, 0x98, 0x16, 0x75,
            },
        },
        .master_id_rsp = {
            .ediv = 0x1c19,
            .rand_val = 0xef308872dc2a4cc2,
        },
        .id_info_rsp = {
            .irk = {
                0xef, 0x8d, 0xe2, 0x16, 0x4f, 0xec, 0x43, 0x0d,
                0xbf, 0x5b, 0xdd, 0x34, 0xc0, 0x53, 0x1e, 0xb8,
            },
        },
        .id_addr_info_rsp = {
            .addr_type = 0,
            .bd_addr = {
                0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a,
            },
        },
        .sign_info_rsp = {
            .sig_key = {
                0xd7, 0x75, 0xfa, 0xed, 0xd7, 0xdd, 0x7b, 0xb3,
                0xa4, 0x20, 0xea, 0x2f, 0x75, 0x60, 0xb1, 0x84,
            },
        },
        .stk = {
            0x9e, 0xe8, 0x35, 0x22, 0xb6, 0xbb, 0x54, 0x0d,
            0x48, 0x1b, 0x25, 0xa0, 0xd8, 0xe2, 0xa5, 0x08,
        },
        .pair_alg = 1,
        .authenticated = true,
        .passkey_info = {
            .passkey = {
                .action = BLE_SM_IOACT_DISP,
                .passkey = 46128,
            },
        },
    };
    ble_sm_test_util_us_lgcy_good(&params);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/**
 * Legacy pairing
 * Master: us
 * Pair algorithm: out of band
 * Initiator IO capabilities: 3
 * Responder IO capabilities: 3
 * Bonding: true
 * Initiator address type: BLE_ADDR_PUBLIC
 * Responder address type: BLE_ADDR_PUBLIC
 * Initiator key distribution: 7
 * Responder key distribution: 7
 */
TEST_CASE_SELF(ble_sm_lgcy_us_ob_iio3_rio3_b1_iat0_rat0_ik7_rk7)
{
    struct ble_sm_test_params params;

    params = (struct ble_sm_test_params) {
        .init_id_addr = {
            0x01, 0x01, 0x01, 0x07, 0x08, 0x01,
        },
        .resp_id_addr = {
            0x66, 0x33, 0x22, 0x66, 0x55, 0x11,
        },
        .pair_req = {
            .io_cap = 0x03,
            .oob_data_flag = 0x01,
            .authreq = 0x05,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .pair_rsp = {
            .io_cap = 0x03,
            .oob_data_flag = 0x01,
            .authreq = 0x05,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .confirm_req[0] = {
            .value = {
                0x2c, 0x3f, 0x3e, 0xf5, 0x39, 0x50, 0x78, 0x4a,
                0x3e, 0x14, 0x1a, 0x51, 0xfb, 0x8d, 0x6c, 0x10,
            },
        },
        .confirm_rsp[0] = {
            .value = {
                0xa9, 0x5c, 0x18, 0xb1, 0xdb, 0x51, 0x53, 0xa5,
                0xd3, 0xe7, 0x72, 0x17, 0xfb, 0xa8, 0xfb, 0xeb,
            },
        },
        .random_req[0] = {
            .value = {
                0x40, 0x2f, 0x42, 0xba, 0x10, 0x7b, 0x22, 0x65,
                0x84, 0xef, 0x63, 0xdf, 0x84, 0x7b, 0x04, 0xef,
            },
        },
        .random_rsp[0] = {
            .value = {
                0x94, 0xdc, 0x3c, 0xef, 0x65, 0xf7, 0x99, 0x2e,
                0x50, 0x29, 0x97, 0x2a, 0x57, 0xfd, 0xe6, 0x6a,
            },
        },
        .enc_info_req = {
            .ltk = {
                0x8c, 0x8e, 0x57, 0xba, 0x17, 0xbb, 0x04, 0xb5,
                0x16, 0xad, 0x31, 0x37, 0xf8, 0x3e, 0x4f, 0x21,
            },
        },
        .master_id_req = {
            .ediv = 0xaaa4,
            .rand_val = 0xc0c830e300000000,
        },
        .id_info_req = {
            .irk = {
                0xef, 0x8d, 0xe2, 0x16, 0x4f, 0xec, 0x43, 0x0d,
                0xbf, 0x5b, 0xdd, 0x34, 0xc0, 0x53, 0x1e, 0xb8,
            },
        },
        .id_addr_info_req = {
            .addr_type = 0,
            .bd_addr = {
                0x66, 0x33, 0x22, 0x66, 0x55, 0x11,
            },
        },
        .sign_info_req = {
            .sig_key = {
                0x5a, 0xe4, 0x2b, 0x40, 0x3a, 0x34, 0x1d, 0x94,
                0x56, 0x7d, 0xf4, 0x41, 0x23, 0x81, 0xc4, 0x11,
            },
        },
        .enc_info_rsp = {
            .ltk = {
                0xa6, 0x8e, 0xa0, 0xa4, 0x02, 0x64, 0x4c, 0x09,
                0x31, 0x25, 0x8a, 0x4f, 0x49, 0x35, 0xb0, 0x1f,
            },
        },
        .master_id_rsp = {
            .ediv = 0x57a3,
            .rand_val = 0x8276af9000000000,
        },
        .id_info_rsp = {
            .irk = {
                0xef, 0x8d, 0xe2, 0x16, 0x4f, 0xec, 0x43, 0x0d,
                0xbf, 0x5b, 0xdd, 0x34, 0xc0, 0x53, 0x1e, 0xb8,
            },
        },
        .id_addr_info_rsp = {
            .addr_type = 0,
            .bd_addr = {
                0x01, 0x01, 0x01, 0x07, 0x08, 0x01,
            },
        },
        .sign_info_rsp = {
            .sig_key = {
                0x8e, 0xef, 0x53, 0x5c, 0x1b, 0x21, 0x67, 0x8d,
                0x07, 0x5e, 0xaa, 0xe8, 0x41, 0xa9, 0x36, 0xcf,
            },
        },
        .stk = {
            0x4c, 0xd4, 0xa7, 0xee, 0x83, 0xcd, 0xd1, 0x9e,
            0x84, 0xeb, 0xb8, 0xd2, 0xaf, 0x4a, 0x71, 0x2e,
        },
        .pair_alg = 2,
        .authenticated = 1,
        .passkey_info = {
            .passkey = {
                .action = BLE_SM_IOACT_OOB,
                .oob = {
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                },
            },
        },
    };
    ble_sm_test_util_us_lgcy_good(&params);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/**
 * Legacy pairing
 * Master: peer
 * Pair algorithm: passkey entry
 * Initiator IO capabilities: 4
 * Responder IO capabilities: 4
 * Bonding: true
 * Initiator address type: BLE_ADDR_PUBLIC
 * Responder address type: BLE_ADDR_PUBLIC
 * Initiator key distribution: 7
 * Responder key distribution: 7
 */
TEST_CASE_SELF(ble_sm_lgcy_peer_pk_iio4_rio4_b1_iat0_rat0_ik7_rk7)
{
    struct ble_sm_test_params params;

    params = (struct ble_sm_test_params) {
        .init_id_addr = {
            0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c,
        },
        .resp_id_addr = {
            0x33, 0x22, 0x11, 0x00, 0x45, 0x0a,
        },
        .pair_req = {
            .io_cap = 0x04,
            .oob_data_flag = 0x00,
            .authreq = 0x05,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .pair_rsp = {
            .io_cap = 0x04,
            .oob_data_flag = 0x00,
            .authreq = 0x0d,
            .max_enc_key_size = 0x10,
            .init_key_dist = 0x07,
            .resp_key_dist = 0x07,
        },
        .confirm_req[0] = {
            .value = {
                0x93, 0x64, 0xb1, 0xb0, 0x07, 0x41, 0x22, 0xe7,
                0x3e, 0x5a, 0x87, 0xf5, 0x1f, 0x25, 0x79, 0x11,
            },
        },
        .confirm_rsp[0] = {
            .value = {
                0x2d, 0x40, 0x15, 0xc4, 0x21, 0xeb, 0xd5, 0x73,
                0xc8, 0x5d, 0xb8, 0xb9, 0x45, 0x31, 0xd5, 0x58,
            },
        },
        .random_req[0] = {
            .value = {
                0x8c, 0x2c, 0x3b, 0xf3, 0x90, 0xaa, 0x2e, 0xcf,
                0xc7, 0x5b, 0xf6, 0xae, 0xb6, 0x4c, 0xc3, 0x61,
            },
        },
        .random_rsp[0] = {
            .value = {
                0x7a, 0x94, 0x97, 0x0a, 0xbe, 0xaf, 0xc0, 0x6b,
                0xd4, 0xf4, 0x04, 0xd1, 0x21, 0x46, 0x34, 0xf3,
            },
        },
        .enc_info_req = {
            .ltk = {
                0x3a, 0x10, 0xd1, 0xab, 0x13, 0xee, 0x16, 0xee,
                0xcf, 0xae, 0xf1, 0x63, 0xf0, 0x6f, 0xb0, 0x89,
            },
        },
        .master_id_req = {
            .ediv = 0xb634,
            .rand_val = 0xa99ac2007b4278a8,
        },
        .id_info_req = {
            .irk = {
                0xef, 0x8d, 0xe2, 0x16, 0x4f, 0xec, 0x43, 0x0d,
                0xbf, 0x5b, 0xdd, 0x34, 0xc0, 0x53, 0x1e, 0xb8,
            },
        },
        .id_addr_info_req = {
            .addr_type = 0,
            .bd_addr = {
                0x33, 0x22, 0x11, 0x00, 0x45, 0x0a,
            },
        },
        .sign_info_req = {
            .sig_key = {
                0x51, 0x4b, 0x7b, 0x31, 0xf7, 0xa6, 0x8a, 0x60,
                0x4f, 0x10, 0x04, 0x5f, 0xb8, 0xee, 0xf6, 0xb3,
            },
        },
        .enc_info_rsp = {
            .ltk = {
                0xa1, 0x1d, 0xdd, 0xee, 0x85, 0xcb, 0xe0, 0x48,
                0x1e, 0xdd, 0xa4, 0x9d, 0xed, 0x3f, 0x15, 0x17,
            },
        },
        .master_id_rsp = {
            .ediv = 0x7e06,
            .rand_val = 0xe6077f688c5ca67,
        },
        .id_info_rsp = {
            .irk = {
                0xeb, 0x8a, 0x06, 0xc4, 0x93, 0x51, 0x04, 0xb3,
                0x8b, 0xbf, 0xe8, 0x1f, 0x0e, 0x96, 0x2a, 0x54,
            },
        },
        .id_addr_info_rsp = {
            .addr_type = 0,
            .bd_addr = {
                0xe1, 0xfc, 0xda, 0xf4, 0xb7, 0x6c,
            },
        },
        .sign_info_rsp = {
            .sig_key = {
                0x16, 0x7a, 0x2e, 0x9d, 0x43, 0x4d, 0x7b, 0x0b,
                0x88, 0xe2, 0x11, 0xb0, 0x4d, 0xa1, 0xed, 0x08,
            },
        },
        .stk = {
            0x6c, 0x3e, 0x78, 0x47, 0xe8, 0x57, 0x9f, 0xe9,
            0x3a, 0x8f, 0x0a, 0xbb, 0xd4, 0x60, 0xf6, 0x0d,
        },
        .pair_alg = 1,
        .authenticated = true,
        .passkey_info = {
            .passkey = {
                .action = BLE_SM_IOACT_INPUT,
                .passkey = 449182,
            },
        },
    };
    ble_sm_test_util_peer_lgcy_good(&params);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_sm_lgcy_test_suite)
{
    /*** No privacy. */

    /* Peer as initiator. */
    ble_sm_lgcy_peer_jw_iio4_rio3_b1_iat0_rat0_ik7_rk7();
    ble_sm_lgcy_peer_pk_iio4_rio0_b1_iat0_rat0_ik7_rk7();
    ble_sm_lgcy_peer_pk_iio4_rio4_b1_iat0_rat0_ik7_rk7();

    /* Us as initiator. */
    ble_sm_lgcy_us_jw_iio3_rio3_b1_iat0_rat1_ik7_rk5();
    ble_sm_lgcy_us_pk_iio4_rio2_b1_iat0_rat1_ik7_rk5();
    ble_sm_lgcy_us_ob_iio3_rio3_b1_iat0_rat0_ik7_rk7();
}

#endif
