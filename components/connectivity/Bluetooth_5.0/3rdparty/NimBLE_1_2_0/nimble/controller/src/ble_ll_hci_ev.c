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
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "syscfg/syscfg.h"
#include "nimble/ble.h"
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"
#include "controller/ble_ll.h"
#include "controller/ble_ll_hci.h"
#include "controller/ble_ll_ctrl.h"
#include "ble_ll_conn_priv.h"

#if (BLETEST_CONCURRENT_CONN_TEST == 1)
extern void bletest_ltk_req_reply(uint16_t handle);
#endif

/**
 * Send a data length change event for a connection to the host.
 *
 * @param connsm Pointer to connection state machine
 */
void
ble_ll_hci_ev_datalen_chg(struct ble_ll_conn_sm *connsm)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_DATA_LEN_CHG)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_DATA_LEN_CHG_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_DATA_LEN_CHG;
            put_le16(evbuf + 3, connsm->conn_handle);
            put_le16(evbuf + 5, connsm->eff_max_tx_octets);
            put_le16(evbuf + 7, connsm->eff_max_tx_time);
            put_le16(evbuf + 9, connsm->eff_max_rx_octets);
            put_le16(evbuf + 11, connsm->eff_max_rx_time);
            ble_ll_hci_event_send(evbuf);
        }
    }
}

/**
 * Send a connection parameter request event for a connection to the host.
 *
 * @param connsm Pointer to connection state machine
 */
void
ble_ll_hci_ev_rem_conn_parm_req(struct ble_ll_conn_sm *connsm,
                                struct ble_ll_conn_params *cp)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_REM_CONN_PARM_REQ)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_REM_CONN_PARM_REQ_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_REM_CONN_PARM_REQ;
            put_le16(evbuf + 3, connsm->conn_handle);
            put_le16(evbuf + 5, cp->interval_min);
            put_le16(evbuf + 7, cp->interval_max);
            put_le16(evbuf + 9, cp->latency);
            put_le16(evbuf + 11, cp->timeout);
            ble_ll_hci_event_send(evbuf);
        }
    }
}

/**
 * Send a connection update event.
 *
 * @param connsm Pointer to connection state machine
 * @param status The error code.
 */
void
ble_ll_hci_ev_conn_update(struct ble_ll_conn_sm *connsm, uint8_t status)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_CONN_UPD_COMPLETE)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_CONN_UPD_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_CONN_UPD_COMPLETE;
            evbuf[3] = status;
            put_le16(evbuf + 4, connsm->conn_handle);
            put_le16(evbuf + 6, connsm->conn_itvl);
            put_le16(evbuf + 8, connsm->slave_latency);
            put_le16(evbuf + 10, connsm->supervision_tmo);
            ble_ll_hci_event_send(evbuf);
        }
    }
}

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION)
void
ble_ll_hci_ev_encrypt_chg(struct ble_ll_conn_sm *connsm, uint8_t status)
{
    uint8_t evcode;
    uint8_t *evbuf;
    uint8_t evlen;

    if (CONN_F_ENC_CHANGE_SENT(connsm) == 0) {
        evcode = BLE_HCI_EVCODE_ENCRYPT_CHG;
        evlen = BLE_HCI_EVENT_ENCRYPT_CHG_LEN;
    } else {
        evcode = BLE_HCI_EVCODE_ENC_KEY_REFRESH;
        evlen = BLE_HCI_EVENT_ENC_KEY_REFRESH_LEN;
    }

    if (ble_ll_hci_is_event_enabled(evcode)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = evcode;
            evbuf[1] = evlen;
            evbuf[2] = status;
            put_le16(evbuf + 3, connsm->conn_handle);
            if (evcode == BLE_HCI_EVCODE_ENCRYPT_CHG) {
                if (status == BLE_ERR_SUCCESS) {
                    evbuf[5] = 0x01;
                } else {
                    evbuf[5] = 0;
                }
            }
            ble_ll_hci_event_send(evbuf);
        }
    }
    CONN_F_ENC_CHANGE_SENT(connsm) = 1;
}

/**
 * Send a long term key request event for a connection to the host.
 *
 * @param connsm Pointer to connection state machine
 */
int
ble_ll_hci_ev_ltk_req(struct ble_ll_conn_sm *connsm)
{
    int rc;
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_LT_KEY_REQ)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_LT_KEY_REQ_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_LT_KEY_REQ;
            put_le16(evbuf + 3, connsm->conn_handle);
            put_le64(evbuf + 5, connsm->enc_data.host_rand_num);
            put_le16(evbuf + 13, connsm->enc_data.enc_div);
            ble_ll_hci_event_send(evbuf);
        }
        rc = 0;
    } else {
        rc = -1;
    }

#if (BLETEST_CONCURRENT_CONN_TEST == 1)
    if (rc == 0) {
        bletest_ltk_req_reply(connsm->conn_handle);
    }
#endif
    return rc;
}
#endif

void
ble_ll_hci_ev_rd_rem_used_feat(struct ble_ll_conn_sm *connsm, uint8_t status)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_RD_REM_USED_FEAT)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_RD_REM_USED_FEAT_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_RD_REM_USED_FEAT;
            evbuf[3] = status;
            put_le16(evbuf + 4, connsm->conn_handle);
            memset(evbuf + 6, 0, BLE_HCI_RD_LOC_SUPP_FEAT_RSPLEN);
            evbuf[6] = connsm->conn_features;
            memcpy(evbuf + 7, connsm->remote_features, 7);
            ble_ll_hci_event_send(evbuf);
        }
    }
}

void
ble_ll_hci_ev_rd_rem_ver(struct ble_ll_conn_sm *connsm, uint8_t status)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_event_enabled(BLE_HCI_EVCODE_RD_REM_VER_INFO_CMP)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_RD_REM_VER_INFO_CMP;
            evbuf[1] = BLE_HCI_EVENT_RD_RM_VER_LEN;
            evbuf[2] = status;
            put_le16(evbuf + 3, connsm->conn_handle);
            evbuf[5] = connsm->vers_nr;
            put_le16(evbuf + 6, connsm->comp_id);
            put_le16(evbuf + 8, connsm->sub_vers_nr);
            ble_ll_hci_event_send(evbuf);
        }
    }
}

/**
 * Send a HW error to the host.
 *
 * @param hw_err
 *
 * @return int 0: event masked or event sent, -1 otherwise
 */
int
ble_ll_hci_ev_hw_err(uint8_t hw_err)
{
    int rc;
    uint8_t *evbuf;

    rc = 0;
    if (ble_ll_hci_is_event_enabled(BLE_HCI_EVCODE_HW_ERROR)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_HW_ERROR;
            evbuf[1] = BLE_HCI_EVENT_HW_ERROR_LEN;
            evbuf[2] = hw_err;
            ble_ll_hci_event_send(evbuf);
        } else {
            rc = -1;
        }
    }
    return rc;
}

void
ble_ll_hci_ev_databuf_overflow(void)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_event_enabled(BLE_HCI_EVCODE_DATA_BUF_OVERFLOW)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_DATA_BUF_OVERFLOW;
            evbuf[1] = BLE_HCI_EVENT_DATABUF_OVERFLOW_LEN;
            evbuf[2] = BLE_HCI_EVENT_ACL_BUF_OVERFLOW;
            ble_ll_hci_event_send(evbuf);
        }
    }
}

/**
 * Send a LE Channel Selection Algorithm event.
 *
 * @param connsm Pointer to connection state machine
 */
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CSA2)
void
ble_ll_hci_ev_le_csa(struct ble_ll_conn_sm *connsm)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_CHAN_SEL_ALG)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_SUBEV_CHAN_SEL_ALG_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_CHAN_SEL_ALG;
            put_le16(evbuf + 3, connsm->conn_handle);
            evbuf[5] = connsm->csmflags.cfbit.csa2_supp ? 0x01 : 0x00;
            ble_ll_hci_event_send(evbuf);
        }
    }
}
#endif

/**
 * Sends the LE Scan Request Received event
 *
 */
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
void
ble_ll_hci_ev_send_scan_req_recv(uint8_t adv_handle, const uint8_t *peer,
                                 uint8_t peer_addr_type)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_SCAN_REQ_RCVD)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_SUBEV_SCAN_REQ_RCVD_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_SCAN_REQ_RCVD;
            evbuf[3] = adv_handle;
            evbuf[4] = peer_addr_type;
            memcpy(&evbuf[5], peer, BLE_DEV_ADDR_LEN);
            ble_ll_hci_event_send(evbuf);
        }
    }
}
#endif

/**
 * Sends the  LE Scan Timeout Event
 *
 */
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
void
ble_ll_hci_ev_send_scan_timeout(void)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_SCAN_TIMEOUT)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_SUBEV_SCAN_TIMEOUT_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_SCAN_TIMEOUT;
            ble_ll_hci_event_send(evbuf);
        }
    }
}
#endif

/**
 * Sends the LE Advertising Set Terminated event
 *
 */
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
void
ble_ll_hci_ev_send_adv_set_terminated(uint8_t status, uint8_t adv_handle,
                                      uint16_t conn_handle, uint8_t events)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_ADV_SET_TERMINATED)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_SUBEV_ADV_SET_TERMINATED_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_ADV_SET_TERMINATED;
            evbuf[3] = status;
            evbuf[4] = adv_handle;
            put_le16(evbuf + 5, conn_handle);
            evbuf[7] = events;
            ble_ll_hci_event_send(evbuf);
        }
    }
}
#endif

/**
 * Send a PHY update complete event
 *
 * @param connsm Pointer to connection state machine
 * @param status error status of event
 */
#if (BLE_LL_BT5_PHY_SUPPORTED == 1)
int
ble_ll_hci_ev_phy_update(struct ble_ll_conn_sm *connsm, uint8_t status)
{
    int rc;
    uint8_t *evbuf;

    rc = 0;
    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_PHY_UPDATE_COMPLETE)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_PHY_UPD_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_PHY_UPDATE_COMPLETE;
            evbuf[3] = status;
            put_le16(evbuf + 4, connsm->conn_handle);
            evbuf[6] = connsm->phy_data.cur_tx_phy;
            evbuf[7] = connsm->phy_data.cur_rx_phy;
            ble_ll_hci_event_send(evbuf);
        } else {
            rc = BLE_ERR_MEM_CAPACITY;
        }
    }
    return rc;
}
#endif

void
ble_ll_hci_ev_send_vendor_err(char *file, uint32_t line)
{
    uint8_t *evbuf;
    uint8_t file_len = strlen(file);

    evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
    if (!evbuf) {
        return;
    }

    evbuf[0] = BLE_HCI_EVCODE_VENDOR_DEBUG;
    evbuf[1] = file_len + sizeof(line) + 1;
    /* Debug id for future use */
    evbuf[2] = 0x00;
    memcpy(&evbuf[3], file, file_len);
    put_le32(&evbuf[3] + file_len, line);
    ble_ll_hci_event_send(evbuf);
}
