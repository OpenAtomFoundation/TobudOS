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

#ifndef H_BLE_SM_TEST_UTIL_
#define H_BLE_SM_TEST_UTIL_

#ifdef __cplusplus
extern "C" {
#endif

struct ble_sm_test_passkey_info {
    struct ble_sm_io passkey;
    uint32_t exp_numcmp;
    unsigned io_before_rx:1;
};

struct ble_sm_test_params {
    uint8_t init_addr_type;
    uint8_t init_id_addr[6];
    uint8_t init_rpa[6];
    uint8_t resp_addr_type;
    uint8_t resp_id_addr[6];
    uint8_t resp_rpa[6];
    struct ble_sm_test_passkey_info passkey_info;

    struct ble_sm_sec_req sec_req;
    struct ble_sm_pair_cmd pair_req;
    struct ble_sm_pair_cmd pair_rsp;
    struct ble_sm_pair_confirm confirm_req[20];
    struct ble_sm_pair_confirm confirm_rsp[20];
    struct ble_sm_pair_random random_req[20];
    struct ble_sm_pair_random random_rsp[20];
    struct ble_sm_id_info id_info_req;
    struct ble_sm_id_info id_info_rsp;
    struct ble_sm_id_addr_info id_addr_info_req;
    struct ble_sm_id_addr_info id_addr_info_rsp;
    struct ble_sm_sign_info sign_info_req;
    struct ble_sm_sign_info sign_info_rsp;
    struct ble_sm_pair_fail pair_fail;

    int pair_alg;
    unsigned authenticated:1;

    /*** Secure connections fields. */
    uint8_t ltk[16];
    uint8_t our_priv_key[32];
    struct ble_sm_public_key public_key_req;
    struct ble_sm_public_key public_key_rsp;
    struct ble_sm_dhkey_check dhkey_check_req;
    struct ble_sm_dhkey_check dhkey_check_rsp;

    /*** Legacy fields. */
    uint8_t stk[16];
    struct ble_sm_enc_info enc_info_req;
    struct ble_sm_enc_info enc_info_rsp;
    struct ble_sm_master_id master_id_req;
    struct ble_sm_master_id master_id_rsp;
};

extern int ble_sm_test_gap_event;
extern int ble_sm_test_gap_status;
extern struct ble_gap_sec_state ble_sm_test_sec_state;

extern int ble_sm_test_store_obj_type;
extern union ble_store_key ble_sm_test_store_key;
extern union ble_store_value ble_sm_test_store_value;

void ble_sm_test_util_init(void);
int ble_sm_test_util_conn_cb(struct ble_gap_event *ctxt, void *arg);
void ble_sm_test_util_io_inject(struct ble_sm_test_passkey_info *passkey_info,
                                uint8_t cur_sm_state);
void ble_sm_test_util_io_inject_bad(uint16_t conn_handle,
                                    uint8_t correct_io_act);
void ble_sm_test_util_io_check_pre(
    struct ble_sm_test_passkey_info *passkey_info,
    uint8_t cur_sm_state);
void ble_sm_test_util_io_check_post(
    struct ble_sm_test_passkey_info *passkey_info,
    uint8_t cur_sm_state);
void ble_sm_test_util_rx_sec_req(uint16_t conn_handle,
                                 struct ble_sm_sec_req *cmd,
                                 int exp_status);
void ble_sm_test_util_verify_tx_pair_fail(struct ble_sm_pair_fail *exp_cmd);
void ble_sm_test_util_us_lgcy_good(struct ble_sm_test_params *params);
void ble_sm_test_util_peer_fail_inval(int we_are_master,
                                      uint8_t *init_addr,
                                      uint8_t *resp_addr,
                                      struct ble_sm_pair_cmd *pair_req,
                                      struct ble_sm_pair_fail *pair_fail);
void ble_sm_test_util_peer_lgcy_fail_confirm(
    uint8_t *init_addr,
    uint8_t *resp_addr,
    struct ble_sm_pair_cmd *pair_req,
    struct ble_sm_pair_cmd *pair_rsp,
    struct ble_sm_pair_confirm *confirm_req,
    struct ble_sm_pair_confirm *confirm_rsp,
    struct ble_sm_pair_random *random_req,
    struct ble_sm_pair_random *random_rsp,
    struct ble_sm_pair_fail *fail_rsp);

void ble_sm_test_util_peer_lgcy_good_once(struct ble_sm_test_params *params);
void ble_sm_test_util_peer_lgcy_good(struct ble_sm_test_params *params);
void ble_sm_test_util_peer_bonding_bad(uint16_t ediv, uint64_t rand_num);
void ble_sm_test_util_peer_sc_good(struct ble_sm_test_params *params);
void ble_sm_test_util_us_sc_good(struct ble_sm_test_params *params);
void ble_sm_test_util_us_fail_inval(struct ble_sm_test_params *params);

#ifdef __cplusplus
}
#endif

#endif
