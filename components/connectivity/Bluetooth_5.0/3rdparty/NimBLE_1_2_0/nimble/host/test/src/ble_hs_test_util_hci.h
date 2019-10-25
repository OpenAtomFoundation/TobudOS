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

#ifndef H_BLE_HS_TEST_UTIL_HCI_
#define H_BLE_HS_TEST_UTIL_HCI_

#ifdef __cplusplus
extern "C" {
#endif

/* leave this as macro so it may be used for static const initialization */
#define ble_hs_hci_util_opcode_join(ogf, ocf) (((ogf) << 10) | (ocf))

#define BLE_HS_TEST_UTIL_PHONY_ACK_MAX  64
struct ble_hs_test_util_hci_ack {
    uint16_t opcode;
    uint8_t status;
    uint8_t evt_params[256];
    uint8_t evt_params_len;
};

struct ble_hs_test_util_hci_num_completed_pkts_entry {
    uint16_t handle_id; /* 0 for terminating entry in array. */
    uint16_t num_pkts;
};

/* $out queue */
void ble_hs_test_util_hci_out_adj(int count);
void *ble_hs_test_util_hci_out_first(void);
void *ble_hs_test_util_hci_out_last(void);
void ble_hs_test_util_hci_out_enqueue(void *cmd);
void ble_hs_test_util_hci_out_clear(void);

/* $build */
void ble_hs_test_util_hci_build_cmd_complete(uint8_t *dst, int len,
                                             uint8_t param_len,
                                             uint8_t num_pkts,
                                             uint16_t opcode);
void ble_hs_test_util_hci_build_cmd_status(uint8_t *dst, int len,
                                           uint8_t status, uint8_t num_pkts,
                                           uint16_t opcode);

/* $ack */
void ble_hs_test_util_hci_ack_set_params(uint16_t opcode, uint8_t status,
                                         void *params, uint8_t params_len);
void ble_hs_test_util_hci_ack_set(uint16_t opcode, uint8_t status);
void ble_hs_test_util_hci_ack_append_params(uint16_t opcode, uint8_t status,
                                            void *params, uint8_t params_len);
void ble_hs_test_util_hci_ack_append(uint16_t opcode, uint8_t status);
void ble_hs_test_util_hci_ack_set_seq(const struct ble_hs_test_util_hci_ack *acks);
void ble_hs_test_util_hci_ack_set_startup(void);
void ble_hs_test_util_hci_ack_set_disc(uint8_t own_addr_type,
                                       int fail_idx, uint8_t fail_status);
void ble_hs_test_util_hci_ack_set_disconnect(uint8_t hci_status);

int ble_hs_test_util_hci_startup_seq_cnt(void);

/* $verify tx */
void ble_hs_test_util_hci_verify_tx_add_irk(uint8_t addr_type,
                                            const uint8_t *addr,
                                            const uint8_t *peer_irk,
                                            const uint8_t *local_irk);
void ble_hs_test_util_hci_verify_tx_set_priv_mode(uint8_t addr_type,
                                                  const uint8_t *addr,
                                                  uint8_t priv_mode);
void ble_hs_test_util_hci_verify_tx_disconnect(uint16_t handle,
                                               uint8_t reason);
void ble_hs_test_util_hci_verify_tx_create_conn(
    const struct hci_create_conn *exp);
uint8_t *ble_hs_test_util_hci_verify_tx(uint8_t ogf, uint16_t ocf,
                                        uint8_t *out_param_len);

/* $rx */
void ble_hs_test_util_hci_rx_num_completed_pkts_event(
    struct ble_hs_test_util_hci_num_completed_pkts_entry *entries);
void ble_hs_test_util_hci_rx_disconn_complete_event(
    struct hci_disconn_complete *evt);
void ble_hs_test_util_hci_rx_conn_cancel_evt(void);

/* $misc */
int ble_hs_test_util_hci_misc_exp_status(int cmd_idx, int fail_idx,
                                         uint8_t fail_status);

#ifdef __cplusplus
}
#endif

#endif
