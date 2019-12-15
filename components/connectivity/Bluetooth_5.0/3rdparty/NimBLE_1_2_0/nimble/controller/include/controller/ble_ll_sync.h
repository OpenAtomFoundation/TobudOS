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

#ifndef H_BLE_LL_SYNC_
#define H_BLE_LL_SYNC_

#include <stdint.h>

#include "nimble/ble.h"
#include "controller/ble_ll_hci.h"

#ifdef __cplusplus
extern "C" {
#endif

int ble_ll_sync_create(uint8_t *cmdbuf);
int ble_ll_sync_cancel(ble_ll_hci_post_cmd_complete_cb *post_cmd_cb);
int ble_ll_sync_terminate(uint8_t *cmdbuf);
int ble_ll_sync_list_add(uint8_t *cmdbuf);
int ble_ll_sync_list_remove(uint8_t *cmdbuf);
int ble_ll_sync_list_clear(void);
int ble_ll_sync_list_size(uint8_t *rspbuf, uint8_t *rsplen);

void ble_ll_sync_info_event(const uint8_t *addr, uint8_t addr_type,
                            uint8_t sid, struct ble_mbuf_hdr *rxhdr,
                            const uint8_t *syncinfo);

int ble_ll_sync_rx_isr_start(uint8_t pdu_type, struct ble_mbuf_hdr *rxhdr);
int ble_ll_sync_rx_isr_end(uint8_t *rxbuf, struct ble_mbuf_hdr *rxhdr);
void ble_ll_sync_rx_pkt_in(struct os_mbuf *rxpdu, struct ble_mbuf_hdr *hdr);
void ble_ll_sync_wfr_timer_exp(void);
void ble_ll_sync_halt(void);

uint32_t ble_ll_sync_get_event_end_time(void);

void ble_ll_sync_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* H_BLE_LL_SYNC_ */
