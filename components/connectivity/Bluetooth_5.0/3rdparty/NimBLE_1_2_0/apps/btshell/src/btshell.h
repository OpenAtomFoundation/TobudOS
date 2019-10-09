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

#ifndef H_BTSHELL_PRIV_
#define H_BTSHELL_PRIV_

#include <inttypes.h>
#include "os/mynewt.h"
#include "nimble/ble.h"
#include "nimble/nimble_opt.h"
#include "modlog/modlog.h"

#include "host/ble_gatt.h"
#include "host/ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ble_gap_white_entry;
struct ble_hs_adv_fields;
struct ble_gap_upd_params;
struct ble_gap_conn_params;
struct hci_adv_params;
struct ble_l2cap_sig_update_req;
struct ble_l2cap_sig_update_params;
union ble_store_value;
union ble_store_key;
struct ble_gap_adv_params;
struct ble_gap_conn_desc;
struct ble_gap_disc_params;

struct btshell_dsc {
    SLIST_ENTRY(btshell_dsc) next;
    struct ble_gatt_dsc dsc;
};
SLIST_HEAD(btshell_dsc_list, btshell_dsc);

struct btshell_chr {
    SLIST_ENTRY(btshell_chr) next;
    struct ble_gatt_chr chr;

    struct btshell_dsc_list dscs;
};
SLIST_HEAD(btshell_chr_list, btshell_chr);

struct btshell_svc {
    SLIST_ENTRY(btshell_svc) next;
    struct ble_gatt_svc svc;
    struct btshell_chr_list chrs;
};

SLIST_HEAD(btshell_svc_list, btshell_svc);

struct btshell_l2cap_coc {
    SLIST_ENTRY(btshell_l2cap_coc) next;
    struct ble_l2cap_chan *chan;
};

SLIST_HEAD(btshell_l2cap_coc_list, btshell_l2cap_coc);

struct btshell_conn {
    uint16_t handle;
    struct btshell_svc_list svcs;
    struct btshell_l2cap_coc_list coc_list;
};

struct btshell_scan_opts {
    uint16_t limit;
    uint8_t ignore_legacy:1;
    uint8_t periodic_only:1;
};

extern struct btshell_conn btshell_conns[MYNEWT_VAL(BLE_MAX_CONNECTIONS)];
extern int btshell_num_conns;

int btshell_exchange_mtu(uint16_t conn_handle);
int btshell_disc_svcs(uint16_t conn_handle);
int btshell_disc_svc_by_uuid(uint16_t conn_handle, const ble_uuid_t *uuid);
int btshell_disc_all_chrs(uint16_t conn_handle, uint16_t start_handle,
                           uint16_t end_handle);
int btshell_disc_chrs_by_uuid(uint16_t conn_handle, uint16_t start_handle,
                               uint16_t end_handle, const ble_uuid_t *uuid);
int btshell_disc_all_dscs(uint16_t conn_handle, uint16_t start_handle,
                          uint16_t end_handle);
int btshell_disc_full(uint16_t conn_handle);
int btshell_find_inc_svcs(uint16_t conn_handle, uint16_t start_handle,
                           uint16_t end_handle);
int btshell_read(uint16_t conn_handle, uint16_t attr_handle);
int btshell_read_long(uint16_t conn_handle, uint16_t attr_handle,
                      uint16_t offset);
int btshell_read_by_uuid(uint16_t conn_handle, uint16_t start_handle,
                          uint16_t end_handle, const ble_uuid_t *uuid);
int btshell_read_mult(uint16_t conn_handle, uint16_t *attr_handles,
                       int num_attr_handles);
int btshell_write(uint16_t conn_handle, uint16_t attr_handle,
                  struct os_mbuf *om);
int btshell_write_no_rsp(uint16_t conn_handle, uint16_t attr_handle,
                         struct os_mbuf *om);
int btshell_write_long(uint16_t conn_handle, uint16_t attr_handle,
                       uint16_t offset, struct os_mbuf *om);
int btshell_write_reliable(uint16_t conn_handle,
                           struct ble_gatt_attr *attrs, int num_attrs);
#if MYNEWT_VAL(BLE_EXT_ADV)
int btshell_ext_adv_configure(uint8_t instance,
                              const struct ble_gap_ext_adv_params *params,
                              int8_t *selected_tx_power);
int btshell_ext_adv_start(uint8_t instance, int duration,
                          int max_events, bool restart);
int btshell_ext_adv_stop(uint8_t instance);
#endif
int btshell_adv_start(uint8_t own_addr_type, const ble_addr_t *direct_addr,
                      int32_t duration_ms,
                      const struct ble_gap_adv_params *params,
                      bool restart);
int btshell_adv_stop(void);
int btshell_conn_initiate(uint8_t own_addr_type, const ble_addr_t *peer_addr,
                          int32_t duration_ms,
                          struct ble_gap_conn_params *params);
int btshell_ext_conn_initiate(uint8_t own_addr_type,
                              const ble_addr_t *peer_addr,
                              int32_t duration_ms,
                              struct ble_gap_conn_params *phy_1m_params,
                              struct ble_gap_conn_params *phy_2m_params,
                              struct ble_gap_conn_params *phy_coded_params);
int btshell_conn_cancel(void);
int btshell_term_conn(uint16_t conn_handle, uint8_t reason);
int btshell_wl_set(ble_addr_t *addrs, int addrs_count);
int btshell_scan(uint8_t own_addr_type, int32_t duration_ms,
                 const struct ble_gap_disc_params *disc_params, void *cb_args);
int btshell_ext_scan(uint8_t own_addr_type, uint16_t duration, uint16_t period,
                     uint8_t filter_duplicates, uint8_t filter_policy,
                     uint8_t limited,
                     const struct ble_gap_ext_disc_params *uncoded_params,
                     const struct ble_gap_ext_disc_params *coded_params,
                     void *cb_args);
int btshell_scan_cancel(void);
int btshell_update_conn(uint16_t conn_handle,
                         struct ble_gap_upd_params *params);
void btshell_notify(uint16_t attr_handle);
int btshell_datalen(uint16_t conn_handle, uint16_t tx_octets,
                    uint16_t tx_time);
int btshell_l2cap_update(uint16_t conn_handle,
                          struct ble_l2cap_sig_update_params *params);
int btshell_sec_start(uint16_t conn_handle);
int btshell_sec_pair(uint16_t conn_handle);
int btshell_sec_unpair(ble_addr_t *peer_addr);
int btshell_sec_restart(uint16_t conn_handle, uint8_t key_size,
                        uint8_t *ltk, uint16_t ediv,
                        uint64_t rand_val, int auth);
int btshell_tx_start(uint16_t conn_handle, uint16_t len, uint16_t rate,
                     uint16_t num);
void btshell_tx_stop(void);
int btshell_rssi(uint16_t conn_handle, int8_t *out_rssi);
int btshell_l2cap_create_srv(uint16_t psm, int accept_response);
int btshell_l2cap_connect(uint16_t conn, uint16_t psm);
int btshell_l2cap_disconnect(uint16_t conn, uint16_t idx);
int btshell_l2cap_send(uint16_t conn, uint16_t idx, uint16_t bytes);

int btshell_gap_event(struct ble_gap_event *event, void *arg);
void btshell_sync_stats(uint16_t handle);

/** GATT server. */
#define GATT_SVR_SVC_ALERT_UUID               0x1811
#define GATT_SVR_CHR_SUP_NEW_ALERT_CAT_UUID   0x2A47
#define GATT_SVR_CHR_NEW_ALERT                0x2A46
#define GATT_SVR_CHR_SUP_UNR_ALERT_CAT_UUID   0x2A48
#define GATT_SVR_CHR_UNR_ALERT_STAT_UUID      0x2A45
#define GATT_SVR_CHR_ALERT_NOT_CTRL_PT        0x2A44

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);
void gatt_svr_print_svcs(void);

/** Misc. */
void print_bytes(const uint8_t *bytes, int len);
void print_mbuf(const struct os_mbuf *om);
void print_addr(const void *addr);
void print_uuid(const ble_uuid_t *uuid);
int svc_is_empty(const struct btshell_svc *svc);
uint16_t chr_end_handle(const struct btshell_svc *svc,
                        const struct btshell_chr *chr);
int chr_is_empty(const struct btshell_svc *svc, const struct btshell_chr *chr);
void print_conn_desc(const struct ble_gap_conn_desc *desc);
void print_svc(struct btshell_svc *svc);

#ifdef __cplusplus
}
#endif

#endif
