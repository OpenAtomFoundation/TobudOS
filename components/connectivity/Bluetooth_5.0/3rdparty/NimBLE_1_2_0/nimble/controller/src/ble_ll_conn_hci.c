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
#include <string.h>
#include <assert.h>
#include "syscfg/syscfg.h"
#include "os/os.h"
#include "nimble/ble.h"
#include "nimble/nimble_opt.h"
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"
#include "controller/ble_ll.h"
#include "controller/ble_ll_utils.h"
#include "controller/ble_ll_hci.h"
#include "controller/ble_ll_conn.h"
#include "controller/ble_ll_ctrl.h"
#include "controller/ble_ll_scan.h"
#include "controller/ble_ll_adv.h"
#include "ble_ll_conn_priv.h"

/*
 * Used to limit the rate at which we send the number of completed packets
 * event to the host. This is the os time at which we can send an event.
 */
static ble_npl_time_t g_ble_ll_last_num_comp_pkt_evt;
extern uint8_t *g_ble_ll_conn_comp_ev;

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
static const uint8_t ble_ll_valid_conn_phy_mask = (BLE_HCI_LE_PHY_1M_PREF_MASK
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_2M_PHY)
                                | BLE_HCI_LE_PHY_2M_PREF_MASK
#endif
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
                                | BLE_HCI_LE_PHY_CODED_PREF_MASK
#endif
                              );
static const uint8_t ble_ll_conn_required_phy_mask = (BLE_HCI_LE_PHY_1M_PREF_MASK
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
                            | BLE_HCI_LE_PHY_CODED_PREF_MASK
#endif
                            );
#endif

/**
 * Allocate an event to send a connection complete event when initiating
 *
 * @return int 0: success -1: failure
 */
static int
ble_ll_init_alloc_conn_comp_ev(void)
{
    int rc;
    uint8_t *evbuf;

    rc = 0;
    evbuf = g_ble_ll_conn_comp_ev;
    if (evbuf == NULL) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (!evbuf) {
            rc = -1;
        } else {
            g_ble_ll_conn_comp_ev = evbuf;
        }
    }

    return rc;
}

/**
 * Called to check that the connection parameters are within range
 *
 * @param itvl_min
 * @param itvl_max
 * @param latency
 * @param spvn_tmo
 *
 * @return int BLE_ERR_INV_HCI_CMD_PARMS if invalid parameters, 0 otherwise
 */
int
ble_ll_conn_hci_chk_conn_params(uint16_t itvl_min, uint16_t itvl_max,
                                uint16_t latency, uint16_t spvn_tmo)
{
    uint32_t spvn_tmo_usecs;
    uint32_t min_spvn_tmo_usecs;

    if ((itvl_min > itvl_max) ||
        (itvl_min < BLE_HCI_CONN_ITVL_MIN) ||
        (itvl_max > BLE_HCI_CONN_ITVL_MAX) ||
        (latency > BLE_HCI_CONN_LATENCY_MAX) ||
        (spvn_tmo < BLE_HCI_CONN_SPVN_TIMEOUT_MIN) ||
        (spvn_tmo > BLE_HCI_CONN_SPVN_TIMEOUT_MAX)) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /*
    * Supervision timeout (in msecs) must be more than:
    *  (1 + connLatency) * connIntervalMax * 1.25 msecs * 2.
    */
    spvn_tmo_usecs = spvn_tmo;
    spvn_tmo_usecs *= (BLE_HCI_CONN_SPVN_TMO_UNITS * 1000);
    min_spvn_tmo_usecs = (uint32_t)itvl_max * 2 * BLE_LL_CONN_ITVL_USECS;
    min_spvn_tmo_usecs *= (1 + latency);
    if (spvn_tmo_usecs <= min_spvn_tmo_usecs) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    return BLE_ERR_SUCCESS;
}

/**
 * Send a connection complete event
 *
 * @param status The BLE error code associated with the event
 */
void
ble_ll_conn_comp_event_send(struct ble_ll_conn_sm *connsm, uint8_t status,
                            uint8_t *evbuf, struct ble_ll_adv_sm *advsm)
{
    uint8_t peer_addr_type;
    uint8_t enabled;
    uint8_t enh_enabled;
    uint8_t *evdata;
    uint8_t *rpa;

    enabled = ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_CONN_COMPLETE);
    enh_enabled = ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_ENH_CONN_COMPLETE);

    if (enabled || enh_enabled) {
        /* Put common elements in event */
        evbuf[0] = BLE_HCI_EVCODE_LE_META;
        if (enh_enabled) {
            evbuf[1] = BLE_HCI_LE_ENH_CONN_COMPLETE_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_ENH_CONN_COMPLETE;
        } else {
            evbuf[1] = BLE_HCI_LE_CONN_COMPLETE_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_CONN_COMPLETE;
        }
        evbuf[3] = status;

        if (connsm) {
            put_le16(evbuf + 4, connsm->conn_handle);

            evbuf[6] = connsm->conn_role - 1;
            peer_addr_type = connsm->peer_addr_type;

            evdata = evbuf + 14;
            if (enh_enabled) {
                memset(evdata, 0, 2 * BLE_DEV_ADDR_LEN);
                if (connsm->conn_role == BLE_LL_CONN_ROLE_MASTER) {
                    if (connsm->inita_identity_used) {
                        /* We used identity address in CONNECT_IND which can be just fine if
                         * a) it was direct advertising we replied to and remote uses its identity address
                         * in device privacy mode or IRK is all zeros.
                         * b) peer uses RPA and this is first time we connect to him
                         */
                        rpa = NULL;
                    } else  if (connsm->own_addr_type > BLE_HCI_ADV_OWN_ADDR_RANDOM) {
                        rpa = ble_ll_scan_get_local_rpa();
                    } else {
                        rpa = NULL;
                    }
                } else {
                    rpa = ble_ll_adv_get_local_rpa(advsm);
                }
                if (rpa) {
                    memcpy(evdata, rpa, BLE_DEV_ADDR_LEN);
                }

                /* We need to adjust peer type if device connected using RPA
                 * and was resolved since RPA needs to be added to HCI event.
                 */
                 if (connsm->peer_addr_type < BLE_HCI_CONN_PEER_ADDR_PUBLIC_IDENT
                         && (connsm->rpa_index > -1)) {
                     peer_addr_type += 2;
                 }

                if (peer_addr_type > BLE_HCI_CONN_PEER_ADDR_RANDOM) {
                    if (connsm->conn_role == BLE_LL_CONN_ROLE_MASTER) {
                        rpa = ble_ll_scan_get_peer_rpa();
                    } else {
                        rpa = ble_ll_adv_get_peer_rpa(advsm);
                    }
                    memcpy(evdata + 6, rpa, BLE_DEV_ADDR_LEN);
                }
                evdata += 12;
            } else {
                if (peer_addr_type > BLE_HCI_CONN_PEER_ADDR_RANDOM) {
                    peer_addr_type -= 2;
                }
            }

            evbuf[7] = peer_addr_type;
            memcpy(evbuf + 8, connsm->peer_addr, BLE_DEV_ADDR_LEN);

            put_le16(evdata, connsm->conn_itvl);
            put_le16(evdata + 2, connsm->slave_latency);
            put_le16(evdata + 4, connsm->supervision_tmo);
            evdata[6] = connsm->master_sca;
        } else {
            /* zero remaining bytes of event (2 bytes used for subevent opcode
             * and status)
             **/
            memset(&evbuf[4], 0, evbuf[1] - 2);
        }
        ble_ll_hci_event_send(evbuf);
    }
}


/**
 * Called to create and send the number of completed packets event to the
 * host.
 */
void
ble_ll_conn_num_comp_pkts_event_send(struct ble_ll_conn_sm *connsm)
{
    /** The maximum number of handles that will fit in an event buffer. */
    static const int max_handles =
        (BLE_LL_MAX_EVT_LEN - BLE_HCI_EVENT_HDR_LEN - 1) / 4;

    int event_sent;
    uint8_t *evbuf;
    uint8_t *handle_ptr;
    uint8_t handles;

    if (connsm == NULL) {
        goto skip_conn;
    }

    /*
     * At some periodic rate, make sure we go through all active connections
     * and send the number of completed packet events. We do this mainly
     * because the spec says we must update the host even though no packets
     * have completed but there are data packets in the controller buffers
     * (i.e. enqueued in a connection state machine).
     */
    if ((ble_npl_stime_t)(ble_npl_time_get() - g_ble_ll_last_num_comp_pkt_evt) <
                                            MYNEWT_VAL(BLE_NUM_COMP_PKT_RATE)) {
        /*
         * If this connection has completed packets, send an event right away.
         * We do this to increase throughput but we dont want to search the
         * entire active list every time.
         */
        if (connsm->completed_pkts) {
            evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
            if (evbuf) {
                evbuf[0] = BLE_HCI_EVCODE_NUM_COMP_PKTS;
                evbuf[1] = (2 * sizeof(uint16_t)) + 1;
                evbuf[2] = 1;
                put_le16(evbuf + 3, connsm->conn_handle);
                put_le16(evbuf + 5, connsm->completed_pkts);
                ble_ll_hci_event_send(evbuf);
                connsm->completed_pkts = 0;
            }
        }
        return;
    }

    /* Iterate through all the active, created connections */
skip_conn:
    evbuf = NULL;
    handles = 0;
    handle_ptr = NULL;
    event_sent = 0;
    SLIST_FOREACH(connsm, &g_ble_ll_conn_active_list, act_sle) {
        /*
         * Only look at connections that we have sent a connection complete
         * event and that either has packets enqueued or has completed packets.
         */
        if ((connsm->conn_state != BLE_LL_CONN_STATE_IDLE) &&
            (connsm->completed_pkts || !STAILQ_EMPTY(&connsm->conn_txq))) {
            /* If no buffer, get one, If cant get one, leave. */
            if (!evbuf) {
                evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
                if (!evbuf) {
                    break;
                }
                handles = 0;
                handle_ptr = evbuf + 3;
            }

            /* Add handle and complete packets */
            put_le16(handle_ptr, connsm->conn_handle);
            put_le16(handle_ptr + 2, connsm->completed_pkts);
            connsm->completed_pkts = 0;
            handle_ptr += (2 * sizeof(uint16_t));
            ++handles;

            /* Send now if the buffer is full. */
            if (handles == max_handles) {
                evbuf[0] = BLE_HCI_EVCODE_NUM_COMP_PKTS;
                evbuf[1] = (handles * 2 * sizeof(uint16_t)) + 1;
                evbuf[2] = handles;
                ble_ll_hci_event_send(evbuf);
                evbuf = NULL;
                handles = 0;
                event_sent = 1;
            }
        }
    }

    /* Send event if there is an event to send */
    if (evbuf) {
        evbuf[0] = BLE_HCI_EVCODE_NUM_COMP_PKTS;
        evbuf[1] = (handles * 2 * sizeof(uint16_t)) + 1;
        evbuf[2] = handles;
        ble_ll_hci_event_send(evbuf);
        event_sent = 1;
    }

    if (event_sent) {
        g_ble_ll_last_num_comp_pkt_evt = ble_npl_time_get();
    }
}

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_PING)
/**
 * Send a authenticated payload timeout event
 *
 * NOTE: we currently only send this event when we have a reason to send it;
 * not when it fails.
 *
 * @param reason The BLE error code to send as a disconnect reason
 */
void
ble_ll_auth_pyld_tmo_event_send(struct ble_ll_conn_sm *connsm)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_event_enabled(BLE_HCI_EVCODE_AUTH_PYLD_TMO)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_AUTH_PYLD_TMO;
            evbuf[1] = sizeof(uint16_t);
            put_le16(evbuf + 2, connsm->conn_handle);
            ble_ll_hci_event_send(evbuf);
        }
    }
}
#endif

/**
 * Send a disconnection complete event.
 *
 * NOTE: we currently only send this event when we have a reason to send it;
 * not when it fails.
 *
 * @param reason The BLE error code to send as a disconnect reason
 */
void
ble_ll_disconn_comp_event_send(struct ble_ll_conn_sm *connsm, uint8_t reason)
{
    uint8_t *evbuf;

    if (ble_ll_hci_is_event_enabled(BLE_HCI_EVCODE_DISCONN_CMP)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_DISCONN_CMP;
            evbuf[1] = BLE_HCI_EVENT_DISCONN_COMPLETE_LEN;
            evbuf[2] = BLE_ERR_SUCCESS;
            put_le16(evbuf + 3, connsm->conn_handle);
            evbuf[5] = reason;
            ble_ll_hci_event_send(evbuf);
        }
    }
}

static int
ble_ll_conn_hci_chk_scan_params(uint16_t itvl, uint16_t window)
{
    /* Check interval and window */
    if ((itvl < BLE_HCI_SCAN_ITVL_MIN) ||
        (itvl > BLE_HCI_SCAN_ITVL_MAX) ||
        (window < BLE_HCI_SCAN_WINDOW_MIN) ||
        (window > BLE_HCI_SCAN_WINDOW_MAX) ||
        (itvl < window)) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    return 0;
}

/**
 * Process the HCI command to create a connection.
 *
 * Context: Link Layer task (HCI command processing)
 *
 * @param cmdbuf
 *
 * @return int
 */
int
ble_ll_conn_create(uint8_t *cmdbuf)
{
    int rc;
    struct hci_create_conn ccdata;
    struct hci_create_conn *hcc;
    struct ble_ll_conn_sm *connsm;

    /* If we are already creating a connection we should leave */
    if (g_ble_ll_conn_create_sm) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    /* If already enabled, we return an error */
    if (ble_ll_scan_enabled()) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    /* Retrieve command data */
    hcc = &ccdata;
    hcc->scan_itvl = get_le16(cmdbuf);
    hcc->scan_window = get_le16(cmdbuf + 2);

    rc = ble_ll_conn_hci_chk_scan_params(hcc->scan_itvl, hcc->scan_window);
    if (rc) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Check filter policy */
    hcc->filter_policy = cmdbuf[4];
    if (hcc->filter_policy > BLE_HCI_INITIATOR_FILT_POLICY_MAX) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Get peer address type and address only if no whitelist used */
    if (hcc->filter_policy == 0) {
        hcc->peer_addr_type = cmdbuf[5];
        if (hcc->peer_addr_type > BLE_HCI_CONN_PEER_ADDR_MAX) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        memcpy(&hcc->peer_addr, cmdbuf + 6, BLE_DEV_ADDR_LEN);
    }

    /* Get own address type (used in connection request) */
    hcc->own_addr_type = cmdbuf[12];
    if (hcc->own_addr_type > BLE_HCI_ADV_OWN_ADDR_MAX) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Check connection interval, latency and supervision timeoout */
    hcc->conn_itvl_min = get_le16(cmdbuf + 13);
    hcc->conn_itvl_max = get_le16(cmdbuf + 15);
    hcc->conn_latency = get_le16(cmdbuf + 17);
    hcc->supervision_timeout = get_le16(cmdbuf + 19);
    rc = ble_ll_conn_hci_chk_conn_params(hcc->conn_itvl_min,
                                         hcc->conn_itvl_max,
                                         hcc->conn_latency,
                                         hcc->supervision_timeout);
    if (rc) {
        return rc;
    }

    /* Min/max connection event lengths */
    hcc->min_ce_len = get_le16(cmdbuf + 21);
    hcc->max_ce_len = get_le16(cmdbuf + 23);
    if (hcc->min_ce_len > hcc->max_ce_len) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Make sure we can allocate an event to send the connection complete */
    if (ble_ll_init_alloc_conn_comp_ev()) {
        return BLE_ERR_MEM_CAPACITY;
    }

    /* Make sure we can accept a connection! */
    connsm = ble_ll_conn_sm_get();
    if (connsm == NULL) {
        return BLE_ERR_CONN_LIMIT;
    }

    /* Initialize state machine in master role and start state machine */
    ble_ll_conn_master_init(connsm, hcc);
    ble_ll_conn_sm_new(connsm);
    /* CSA will be selected when advertising is received */

    /* Start scanning */
    rc = ble_ll_scan_initiator_start(hcc, &connsm->scansm);
    if (rc) {
        SLIST_REMOVE(&g_ble_ll_conn_active_list,connsm,ble_ll_conn_sm,act_sle);
        STAILQ_INSERT_TAIL(&g_ble_ll_conn_free_list, connsm, free_stqe);
    } else {
        /* Set the connection state machine we are trying to create. */
        g_ble_ll_conn_create_sm = connsm;
    }

    return rc;
}

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
static void
ble_ll_conn_init_hcc_params(struct hci_ext_create_conn *hcc,
                            int valid_param_idx)
{
    struct hci_ext_conn_params *hcc_params = &hcc->params[valid_param_idx];

    if (valid_param_idx != 0 && !(hcc->init_phy_mask & BLE_PHY_MASK_1M)) {
        hcc->params[0] = *hcc_params;
    }

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_2M_PHY)
    if (valid_param_idx != 1 && !(hcc->init_phy_mask & BLE_PHY_MASK_2M)) {
        hcc->params[1] = *hcc_params;
    }
#endif

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
    if (valid_param_idx != 2 && !(hcc->init_phy_mask & BLE_PHY_MASK_CODED)) {
        hcc->params[2] = *hcc_params;
    }
#endif
}

int
ble_ll_ext_conn_create(uint8_t *cmdbuf, uint8_t cmdlen)
{
    int rc;
    struct hci_ext_create_conn ccdata;
    struct hci_ext_create_conn *hcc;
    struct hci_ext_conn_params *hcc_params;
    struct ble_ll_conn_sm *connsm;
    int valid_param_idx = -1;
    int iter;

    /* validate length */
    if (cmdlen < 10) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* If we are already creating a connection we should leave */
    if (g_ble_ll_conn_create_sm) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    /* If already enabled, we return an error */
    if (ble_ll_scan_enabled()) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    /* Retrieve command data */
    hcc = &ccdata;
    memset (hcc, 0, sizeof(*hcc));

    /* Check filter policy */
    hcc->filter_policy = cmdbuf[0];
    if (hcc->filter_policy > BLE_HCI_INITIATOR_FILT_POLICY_MAX) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Get own address type (used in connection request) */
    hcc->own_addr_type = cmdbuf[1];
    if (hcc->own_addr_type > BLE_HCI_ADV_OWN_ADDR_MAX) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Get peer address type and address only if no whitelist used */
    if (hcc->filter_policy == 0) {
        hcc->peer_addr_type = cmdbuf[2];
        if (hcc->peer_addr_type > BLE_HCI_CONN_PEER_ADDR_MAX) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        memcpy(&hcc->peer_addr, cmdbuf + 3, BLE_DEV_ADDR_LEN);
    }

    hcc->init_phy_mask = cmdbuf[9];
    if (hcc->init_phy_mask & ~ble_ll_valid_conn_phy_mask) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    if (!(hcc->init_phy_mask & ble_ll_conn_required_phy_mask)) {
        /* At least one of those need to be set */
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    iter = 10;
    if (hcc->init_phy_mask & BLE_PHY_MASK_1M) {
        /* validate length */
        if (cmdlen < iter + 16) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        hcc_params = &hcc->params[0];
        hcc_params->scan_itvl = get_le16(cmdbuf + iter);
        hcc_params->scan_window = get_le16(cmdbuf + iter + 2);
        iter += 4;

        rc = ble_ll_conn_hci_chk_scan_params(hcc_params->scan_itvl,
                                             hcc_params->scan_window);
        if (rc) {
            return rc;
        }

        hcc_params->conn_itvl_min = get_le16(cmdbuf + iter);
        hcc_params->conn_itvl_max = get_le16(cmdbuf + iter + 2);
        hcc_params->conn_latency = get_le16(cmdbuf + iter + 4);
        hcc_params->supervision_timeout = get_le16(cmdbuf + iter + 6);
        rc = ble_ll_conn_hci_chk_conn_params(hcc_params->conn_itvl_min,
                                             hcc_params->conn_itvl_max,
                                             hcc_params->conn_latency,
                                             hcc_params->supervision_timeout);
        if (rc) {
            return rc;
        }
        iter += 8;

        /* Min/max connection event lengths */
        hcc_params->min_ce_len = get_le16(cmdbuf + iter);
        hcc_params->max_ce_len = get_le16(cmdbuf + iter + 2);
        if (hcc_params->min_ce_len > hcc_params->max_ce_len) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        iter += 4;
        valid_param_idx = 0;
    }

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_2M_PHY)
    if (hcc->init_phy_mask & BLE_PHY_MASK_2M) {
        /* validate length */
        if (cmdlen < iter + 16) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        /* Move to connection parameters */
        hcc_params = &hcc->params[1];
        iter += 4;

        hcc_params->conn_itvl_min = get_le16(cmdbuf + iter);
        hcc_params->conn_itvl_max = get_le16(cmdbuf + iter + 2);
        hcc_params->conn_latency = get_le16(cmdbuf + iter + 4);
        hcc_params->supervision_timeout = get_le16(cmdbuf + iter + 6);
        rc = ble_ll_conn_hci_chk_conn_params(hcc_params->conn_itvl_min,
                                             hcc_params->conn_itvl_max,
                                             hcc_params->conn_latency,
                                             hcc_params->supervision_timeout);
        if (rc) {
            return rc;
        }
        iter += 8;

        /* Min/max connection event lengths */
        hcc_params->min_ce_len = get_le16(cmdbuf + iter);
        hcc_params->max_ce_len = get_le16(cmdbuf + iter + 2);
        if (hcc_params->min_ce_len > hcc_params->max_ce_len) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        iter += 4;
    }
#endif

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
    if (hcc->init_phy_mask & BLE_PHY_MASK_CODED) {
        /* validate length */
        if (cmdlen < iter + 16) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        hcc_params = &hcc->params[2];
        hcc_params->scan_itvl = get_le16(cmdbuf + iter);
        hcc_params->scan_window = get_le16(cmdbuf + iter + 2);
        iter += 4;

        rc = ble_ll_conn_hci_chk_scan_params(hcc_params->scan_itvl,
                                             hcc_params->scan_window);
        if (rc) {
            return rc;
        }

        hcc_params->conn_itvl_min = get_le16(cmdbuf + iter);
        hcc_params->conn_itvl_max = get_le16(cmdbuf + iter + 2);
        hcc_params->conn_latency = get_le16(cmdbuf + iter + 4);
        hcc_params->supervision_timeout = get_le16(cmdbuf + iter + 6);
        rc = ble_ll_conn_hci_chk_conn_params(hcc_params->conn_itvl_min,
                                             hcc_params->conn_itvl_max,
                                             hcc_params->conn_latency,
                                             hcc_params->supervision_timeout);
        if (rc) {
            return rc;
        }
        iter += 8;

        /* Min/max connection event lengths */
        hcc_params->min_ce_len = get_le16(cmdbuf + iter);
        hcc_params->max_ce_len = get_le16(cmdbuf + iter + 2);
        if (hcc_params->min_ce_len > hcc_params->max_ce_len) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        iter += 4;
        if (valid_param_idx < 0) {
            valid_param_idx = 2;
        }
    }
#endif

    /* Make sure we can allocate an event to send the connection complete */
    if (ble_ll_init_alloc_conn_comp_ev()) {
        return BLE_ERR_MEM_CAPACITY;
    }

    /* Make sure we can accept a connection! */
    connsm = ble_ll_conn_sm_get();
    if (connsm == NULL) {
        return BLE_ERR_CONN_LIMIT;
    }

    ble_ll_conn_init_hcc_params(hcc, valid_param_idx);

    /* Initialize state machine in master role and start state machine */
    ble_ll_conn_ext_master_init(connsm, hcc);
    ble_ll_conn_sm_new(connsm);

    /* CSA will be selected when advertising is received */

    /* Start scanning */
    rc = ble_ll_scan_ext_initiator_start(hcc, &connsm->scansm);
    if (rc) {
        SLIST_REMOVE(&g_ble_ll_conn_active_list,connsm,ble_ll_conn_sm,act_sle);
        STAILQ_INSERT_TAIL(&g_ble_ll_conn_free_list, connsm, free_stqe);
    } else {
        /* Set the connection state machine we are trying to create. */
        g_ble_ll_conn_create_sm = connsm;
    }

    return rc;
}
#endif

static int
ble_ll_conn_process_conn_params(uint8_t *cmdbuf, struct ble_ll_conn_sm *connsm)
{
    int rc;
    struct hci_conn_update *hcu;

    /* Retrieve command data */
    hcu = &connsm->conn_param_req;
    hcu->handle = connsm->conn_handle;
    hcu->conn_itvl_min = get_le16(cmdbuf + 2);
    hcu->conn_itvl_max = get_le16(cmdbuf + 4);
    hcu->conn_latency = get_le16(cmdbuf + 6);
    hcu->supervision_timeout = get_le16(cmdbuf + 8);
    hcu->min_ce_len = get_le16(cmdbuf + 10);
    hcu->max_ce_len = get_le16(cmdbuf + 12);

    /* Check that parameter values are in range */
    rc = ble_ll_conn_hci_chk_conn_params(hcu->conn_itvl_min,
                                         hcu->conn_itvl_max,
                                         hcu->conn_latency,
                                         hcu->supervision_timeout);

    /* Check valid min/max ce length */
    if (rc || (hcu->min_ce_len > hcu->max_ce_len)) {
        hcu->handle = 0;
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }
    return rc;
}

/**
 * Called when the host issues the read remote features command
 *
 * @param cmdbuf
 *
 * @return int
 */
int
ble_ll_conn_hci_read_rem_features(uint8_t *cmdbuf)
{
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    /* If no connection handle exit with error */
    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        return BLE_ERR_UNK_CONN_ID;
    }

    /* If already pending exit with error */
    if (connsm->csmflags.cfbit.pending_hci_rd_features) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    /*
     * Start control procedure if we did not receive peer's features and did not
     * start procedure already.
     */
    if (!connsm->csmflags.cfbit.rxd_features &&
                !IS_PENDING_CTRL_PROC(connsm, BLE_LL_CTRL_PROC_FEATURE_XCHG)) {
        if ((connsm->conn_role == BLE_LL_CONN_ROLE_SLAVE) &&
            !(ble_ll_read_supp_features() & BLE_LL_FEAT_SLAVE_INIT)) {
                return BLE_ERR_CMD_DISALLOWED;
        }

        ble_ll_ctrl_proc_start(connsm, BLE_LL_CTRL_PROC_FEATURE_XCHG);
    }

    connsm->csmflags.cfbit.pending_hci_rd_features = 1;

    return BLE_ERR_SUCCESS;
}

/**
 * Called to process a connection update command.
 *
 * @param cmdbuf
 *
 * @return int
 */
int
ble_ll_conn_hci_update(uint8_t *cmdbuf)
{
    int rc;
    uint8_t ctrl_proc;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;
    struct hci_conn_update *hcu;

    /*
     * XXX: must deal with slave not supporting this feature and using
     * conn update! Right now, we only check if WE support the connection
     * parameters request procedure. We dont check if the remote does.
     * We should also be able to deal with sending the parameter request,
     * getting an UNKOWN_RSP ctrl pdu and resorting to use normal
     * connection update procedure.
     */

    /* If no connection handle exit with error */
    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        return BLE_ERR_UNK_CONN_ID;
    }

    /* Better not have this procedure ongoing! */
    if (IS_PENDING_CTRL_PROC(connsm, BLE_LL_CTRL_PROC_CONN_PARAM_REQ) ||
        IS_PENDING_CTRL_PROC(connsm, BLE_LL_CTRL_PROC_CONN_UPDATE)) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    /* See if this feature is supported on both sides */
    if ((connsm->conn_features & BLE_LL_FEAT_CONN_PARM_REQ) == 0) {
        if (connsm->conn_role == BLE_LL_CONN_ROLE_SLAVE) {
            return BLE_ERR_UNSUPP_REM_FEATURE;
        }
        ctrl_proc = BLE_LL_CTRL_PROC_CONN_UPDATE;
    } else {
        ctrl_proc = BLE_LL_CTRL_PROC_CONN_PARAM_REQ;
    }

    /*
     * If we are a slave and the master has initiated the procedure already
     * we should deny the slave request for now. If we are a master and the
     * slave has initiated the procedure, we need to send a reject to the
     * slave.
     */
    if (connsm->csmflags.cfbit.awaiting_host_reply) {
        if (connsm->conn_role == BLE_LL_CONN_ROLE_SLAVE) {
            return BLE_ERR_LMP_COLLISION;
        } else {
            connsm->csmflags.cfbit.awaiting_host_reply = 0;

            /* XXX: If this fails no reject ind will be sent! */
            ble_ll_ctrl_reject_ind_send(connsm, connsm->host_reply_opcode,
                                        BLE_ERR_LMP_COLLISION);
        }
    }

    /*
     * If we are a slave and the master has initiated the channel map
     * update procedure we should deny the slave request for now.
     */
    if (connsm->csmflags.cfbit.chanmap_update_scheduled) {
        if (connsm->conn_role == BLE_LL_CONN_ROLE_SLAVE) {
            return BLE_ERR_DIFF_TRANS_COLL;
        }
    }

    /* Retrieve command data */
    hcu = &connsm->conn_param_req;
    hcu->handle = handle;
    hcu->conn_itvl_min = get_le16(cmdbuf + 2);
    hcu->conn_itvl_max = get_le16(cmdbuf + 4);
    hcu->conn_latency = get_le16(cmdbuf + 6);
    hcu->supervision_timeout = get_le16(cmdbuf + 8);
    hcu->min_ce_len = get_le16(cmdbuf + 10);
    hcu->max_ce_len = get_le16(cmdbuf + 12);
    if (hcu->min_ce_len > hcu->max_ce_len) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Check that parameter values are in range */
    rc = ble_ll_conn_hci_chk_conn_params(hcu->conn_itvl_min,
                                         hcu->conn_itvl_max,
                                         hcu->conn_latency,
                                         hcu->supervision_timeout);
    if (!rc) {
        /* Start the control procedure */
        ble_ll_ctrl_proc_start(connsm, ctrl_proc);
    }

    return rc;
}

int
ble_ll_conn_hci_param_reply(uint8_t *cmdbuf, int positive_reply,
                            uint8_t *rspbuf, uint8_t *rsplen)
{
    int rc;
    uint8_t ble_err;
    uint8_t *dptr;
    uint8_t rsp_opcode;
    uint8_t len;
    uint16_t handle;
    struct os_mbuf *om;
    struct ble_ll_conn_sm *connsm;

    handle = get_le16(cmdbuf);

    /* See if we support this feature */
    if ((ble_ll_read_supp_features() & BLE_LL_FEAT_CONN_PARM_REQ) == 0) {
        rc = BLE_ERR_UNKNOWN_HCI_CMD;
        goto done;
    }

    /* If we dont have a handle we cant do anything */
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rc = BLE_ERR_UNK_CONN_ID;
        goto done;
    }

    /* Make sure connection parameters are valid if this is a positive reply */
    rc = BLE_ERR_SUCCESS;
    ble_err = cmdbuf[2];
    if (positive_reply) {
        rc = ble_ll_conn_process_conn_params(cmdbuf, connsm);
        if (rc) {
            ble_err = BLE_ERR_CONN_PARMS;
        }
    }

    /* The connection should be awaiting a reply. If not, just discard */
    if (connsm->csmflags.cfbit.awaiting_host_reply) {
        /* Get a control packet buffer */
        if (positive_reply && (rc == BLE_ERR_SUCCESS)) {
            om = os_msys_get_pkthdr(BLE_LL_CTRL_MAX_PAYLOAD + 1,
                                    sizeof(struct ble_mbuf_hdr));
            if (om) {
                dptr = om->om_data;
                rsp_opcode = ble_ll_ctrl_conn_param_reply(connsm, dptr,
                                                          &connsm->conn_cp);
                dptr[0] = rsp_opcode;
                len = g_ble_ll_ctrl_pkt_lengths[rsp_opcode] + 1;
                ble_ll_conn_enqueue_pkt(connsm, om, BLE_LL_LLID_CTRL, len);
            }
        } else {
            /* XXX: check return code and deal */
            ble_ll_ctrl_reject_ind_send(connsm, connsm->host_reply_opcode,
                                        ble_err);
        }
        connsm->csmflags.cfbit.awaiting_host_reply = 0;

        /* XXX: if we cant get a buffer, what do we do? We need to remember
         * reason if it was a negative reply. We also would need to have
           some state to tell us this happened */
    }

done:
    put_le16(rspbuf, handle);
    *rsplen = sizeof(uint16_t);
    return rc;
}

/* this is called from same context after cmd complete is send so it is
 * safe to use g_ble_ll_conn_comp_ev
 */
static void
ble_ll_conn_hci_cancel_conn_complete_event(void)
{
    BLE_LL_ASSERT(g_ble_ll_conn_comp_ev);

    ble_ll_conn_comp_event_send(NULL, BLE_ERR_UNK_CONN_ID,
                                g_ble_ll_conn_comp_ev, NULL);
    g_ble_ll_conn_comp_ev = NULL;
}

/**
 * Called when HCI command to cancel a create connection command has been
 * received.
 *
 * Context: Link Layer (HCI command parser)
 *
 * @return int
 */
int
ble_ll_conn_create_cancel(ble_ll_hci_post_cmd_complete_cb *post_cmd_cb)
{
    int rc;
    struct ble_ll_conn_sm *connsm;
    os_sr_t sr;

    /*
     * If we receive this command and we have not got a connection
     * create command, we have to return disallowed. The spec does not say
     * what happens if the connection has already been established. We
     * return disallowed as well
     */
    OS_ENTER_CRITICAL(sr);
    connsm = g_ble_ll_conn_create_sm;
    if (connsm && (connsm->conn_state == BLE_LL_CONN_STATE_IDLE)) {
        /* stop scanning and end the connection event */
        g_ble_ll_conn_create_sm = NULL;
        ble_ll_scan_sm_stop(1);
        ble_ll_conn_end(connsm, BLE_ERR_UNK_CONN_ID);

        *post_cmd_cb = ble_ll_conn_hci_cancel_conn_complete_event;

        rc = BLE_ERR_SUCCESS;
    } else {
        /* If we are not attempting to create a connection*/
        rc = BLE_ERR_CMD_DISALLOWED;
    }
    OS_EXIT_CRITICAL(sr);

    return rc;
}

/**
 * Called to process a HCI disconnect command
 *
 * Context: Link Layer task (HCI command parser).
 *
 * @param cmdbuf
 *
 * @return int
 */
int
ble_ll_conn_hci_disconnect_cmd(uint8_t *cmdbuf)
{
    int rc;
    uint8_t reason;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    /* Check for valid parameters */
    handle = get_le16(cmdbuf);
    reason = cmdbuf[2];

    rc = BLE_ERR_INV_HCI_CMD_PARMS;
    if (handle <= BLE_LL_CONN_MAX_CONN_HANDLE) {
        /* Make sure reason is valid */
        switch (reason) {
        case BLE_ERR_AUTH_FAIL:
        case BLE_ERR_REM_USER_CONN_TERM:
        case BLE_ERR_RD_CONN_TERM_RESRCS:
        case BLE_ERR_RD_CONN_TERM_PWROFF:
        case BLE_ERR_UNSUPP_REM_FEATURE:
        case BLE_ERR_UNIT_KEY_PAIRING:
        case BLE_ERR_CONN_PARMS:
            connsm = ble_ll_conn_find_active_conn(handle);
            if (connsm) {
                /* Do not allow command if we are in process of disconnecting */
                if (connsm->disconnect_reason) {
                    rc = BLE_ERR_CMD_DISALLOWED;
                } else {
                    /* This control procedure better not be pending! */
                    BLE_LL_ASSERT(CONN_F_TERMINATE_STARTED(connsm) == 0);

                    /* Record the disconnect reason */
                    connsm->disconnect_reason = reason;

                    /* Start this control procedure */
                    ble_ll_ctrl_terminate_start(connsm);

                    rc = BLE_ERR_SUCCESS;
                }
            } else {
                rc = BLE_ERR_UNK_CONN_ID;
            }
            break;
        default:
            break;
        }
    }

    return rc;
}

/**
 * Called to process a HCI disconnect command
 *
 * Context: Link Layer task (HCI command parser).
 *
 * @param cmdbuf
 *
 * @return int
 */
int
ble_ll_conn_hci_rd_rem_ver_cmd(uint8_t *cmdbuf)
{
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    /* Check for valid parameters */
    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        return BLE_ERR_UNK_CONN_ID;
    }

    /* Return error if in progress */
    if (IS_PENDING_CTRL_PROC(connsm, BLE_LL_CTRL_PROC_VERSION_XCHG)) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    /*
     * Start this control procedure. If we have already done this control
     * procedure we set the pending bit so that the host gets an event because
     * it is obviously expecting one (or would not have sent the command).
     * NOTE: we cant just send the event here. That would cause the event to
     * be queued before the command status.
     */
    if (!connsm->csmflags.cfbit.version_ind_sent) {
        ble_ll_ctrl_proc_start(connsm, BLE_LL_CTRL_PROC_VERSION_XCHG);
    } else {
        connsm->pending_ctrl_procs |= (1 << BLE_LL_CTRL_PROC_VERSION_XCHG);
    }

    return BLE_ERR_SUCCESS;
}

/**
 * Called to read the RSSI for a given connection handle
 *
 * @param cmdbuf
 * @param rspbuf
 * @param rsplen
 *
 * @return int
 */
int
ble_ll_conn_hci_rd_rssi(uint8_t *cmdbuf, uint8_t *rspbuf, uint8_t *rsplen)
{
    int rc;
    int8_t rssi;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rssi = 127;
        rc = BLE_ERR_UNK_CONN_ID;
    } else {
        rssi = connsm->conn_rssi;
        rc = BLE_ERR_SUCCESS;
    }

    put_le16(rspbuf, handle);
    rspbuf[2] = (uint8_t)rssi;
    *rsplen = 3;

    /* Place the RSSI of the connection into the response buffer */
    return rc;
}

/**
 * Called to read the current channel map of a connection
 *
 * @param cmdbuf
 * @param rspbuf
 * @param rsplen
 *
 * @return int
 */
int
ble_ll_conn_hci_rd_chan_map(uint8_t *cmdbuf, uint8_t *rspbuf, uint8_t *rsplen)
{
    int rc;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rc = BLE_ERR_UNK_CONN_ID;
    } else {
        if (connsm->csmflags.cfbit.chanmap_update_scheduled) {
            memcpy(rspbuf + 2, &connsm->req_chanmap[0], BLE_LL_CONN_CHMAP_LEN);
        } else {
            memcpy(rspbuf + 2, &connsm->chanmap[0], BLE_LL_CONN_CHMAP_LEN);
        }
        rc = BLE_ERR_SUCCESS;
    }

    put_le16(rspbuf, handle);
    *rsplen = sizeof(uint16_t) + BLE_LL_CONN_CHMAP_LEN;
    return rc;
}

/**
 * Called when the host issues the LE command "set host channel classification"
 *
 * @param cmdbuf
 *
 * @return int
 */
int
ble_ll_conn_hci_set_chan_class(uint8_t *cmdbuf)
{
    int rc;
    uint8_t num_used_chans;

    /*
     * The HCI command states that the host is allowed to mask in just one
     * channel but the Link Layer needs minimum two channels to operate. So
     * I will not allow this command if there are less than 2 channels masked.
     */
    rc = BLE_ERR_SUCCESS;
    num_used_chans = ble_ll_utils_calc_num_used_chans(cmdbuf);
    if ((num_used_chans < 2) || ((cmdbuf[4] & 0xe0) != 0)) {
        rc = BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Set the host channel mask */
    ble_ll_conn_set_global_chanmap(num_used_chans, cmdbuf);
    return rc;
}

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_DATA_LEN_EXT)
int
ble_ll_conn_hci_set_data_len(uint8_t *cmdbuf, uint8_t *rspbuf, uint8_t *rsplen)
{
    int rc;
    uint16_t handle;
    uint16_t txoctets;
    uint16_t txtime;
    struct ble_ll_conn_sm *connsm;

    /* Find connection */
    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rc = BLE_ERR_UNK_CONN_ID;
        goto done;
    }

    txoctets = get_le16(cmdbuf + 2);
    txtime = get_le16(cmdbuf + 4);

    /* Make sure it is valid */
    if (!ble_ll_chk_txrx_octets(txoctets) ||
        !ble_ll_chk_txrx_time(txtime)) {
        rc = BLE_ERR_INV_HCI_CMD_PARMS;
        goto done;
    }

    rc = BLE_ERR_SUCCESS;
    if (connsm->max_tx_time != txtime ||
        connsm->max_tx_octets != txoctets) {

        connsm->max_tx_time = txtime;
        connsm->max_tx_octets = txoctets;

        ble_ll_ctrl_initiate_dle(connsm);
    }

done:
    put_le16(rspbuf, handle);
    *rsplen = sizeof(uint16_t);
    return rc;
}
#endif

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION)
/**
 * LE start encrypt command
 *
 * @param cmdbuf
 *
 * @return int
 */
int
ble_ll_conn_hci_le_start_encrypt(uint8_t *cmdbuf)
{
    int rc;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rc = BLE_ERR_UNK_CONN_ID;
    } else if (connsm->conn_role == BLE_LL_CONN_ROLE_SLAVE) {
        rc = BLE_ERR_UNSPECIFIED;
    } else if (connsm->cur_ctrl_proc == BLE_LL_CTRL_PROC_ENCRYPT) {
        /*
         * The specification does not say what to do here but the host should
         * not be telling us to start encryption while we are in the process
         * of honoring a previous start encrypt.
         */
        rc = BLE_ERR_CMD_DISALLOWED;
    } else {
        /* Start the control procedure */
        connsm->enc_data.host_rand_num = get_le64(cmdbuf + 2);
        connsm->enc_data.enc_div = get_le16(cmdbuf + 10);
        swap_buf(connsm->enc_data.enc_block.key, cmdbuf + 12, 16);
        ble_ll_ctrl_proc_start(connsm, BLE_LL_CTRL_PROC_ENCRYPT);
        rc = BLE_ERR_SUCCESS;
    }

    return rc;
}

/**
 * Called to process the LE long term key reply.
 *
 * Context: Link Layer Task.
 *
 * @param cmdbuf
 * @param rspbuf
 * @param ocf
 *
 * @return int
 */
int
ble_ll_conn_hci_le_ltk_reply(uint8_t *cmdbuf, uint8_t *rspbuf, uint8_t *rsplen)
{
    int rc;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    /* Find connection handle */
    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rc = BLE_ERR_UNK_CONN_ID;
        goto ltk_key_cmd_complete;
    }

    /* Should never get this if we are a master! */
    if (connsm->conn_role == BLE_LL_CONN_ROLE_MASTER) {
        rc = BLE_ERR_UNSPECIFIED;
        goto ltk_key_cmd_complete;
    }

    /* The connection should be awaiting a reply. If not, just discard */
    if (connsm->enc_data.enc_state != CONN_ENC_S_LTK_REQ_WAIT) {
        rc = BLE_ERR_CMD_DISALLOWED;
        goto ltk_key_cmd_complete;
    }

    swap_buf(connsm->enc_data.enc_block.key, cmdbuf + 2, 16);
    ble_ll_calc_session_key(connsm);
    ble_ll_ctrl_start_enc_send(connsm);
    rc = BLE_ERR_SUCCESS;

ltk_key_cmd_complete:
    put_le16(rspbuf, handle);
    *rsplen = sizeof(uint16_t);
    return rc;
}

/**
 * Called to process the LE long term key negative reply.
 *
 * Context: Link Layer Task.
 *
 * @param cmdbuf
 * @param rspbuf
 * @param ocf
 *
 * @return int
 */
int
ble_ll_conn_hci_le_ltk_neg_reply(uint8_t *cmdbuf, uint8_t *rspbuf,
                                 uint8_t *rsplen)
{
    int rc;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    /* Find connection handle */
    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rc = BLE_ERR_UNK_CONN_ID;
        goto ltk_key_cmd_complete;
    }

    /* Should never get this if we are a master! */
    if (connsm->conn_role == BLE_LL_CONN_ROLE_MASTER) {
        rc = BLE_ERR_UNSPECIFIED;
        goto ltk_key_cmd_complete;
    }

    /* The connection should be awaiting a reply. If not, just discard */
    if (connsm->enc_data.enc_state != CONN_ENC_S_LTK_REQ_WAIT) {
        rc = BLE_ERR_CMD_DISALLOWED;
        goto ltk_key_cmd_complete;
    }

    /* We received a negative reply! Send REJECT_IND */
    ble_ll_ctrl_reject_ind_send(connsm, BLE_LL_CTRL_ENC_REQ,
                                BLE_ERR_PINKEY_MISSING);
    connsm->enc_data.enc_state = CONN_ENC_S_LTK_NEG_REPLY;

    rc = BLE_ERR_SUCCESS;

ltk_key_cmd_complete:
    put_le16(rspbuf, handle);
    *rsplen = sizeof(uint16_t);
    return rc;
}
#endif

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_PING)
/**
 * Read authenticated payload timeout (OGF=3, OCF==0x007B)
 *
 * @param cmdbuf
 * @param rsplen
 *
 * @return int
 */
int
ble_ll_conn_hci_rd_auth_pyld_tmo(uint8_t *cmdbuf, uint8_t *rsp, uint8_t *rsplen)
{
    int rc;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rc = BLE_ERR_UNK_CONN_ID;
    } else {
        put_le16(rsp + 2, connsm->auth_pyld_tmo);
        rc = BLE_ERR_SUCCESS;
    }

    put_le16(rsp, handle);
    *rsplen = BLE_HCI_RD_AUTH_PYLD_TMO_LEN;
    return rc;
}

/**
 * Write authenticated payload timeout (OGF=3, OCF=00x7C)
 *
 * @param cmdbuf
 * @param rsplen
 *
 * @return int
 */
int
ble_ll_conn_hci_wr_auth_pyld_tmo(uint8_t *cmdbuf, uint8_t *rsp, uint8_t *rsplen)
{
    int rc;
    uint16_t handle;
    uint16_t tmo;
    uint32_t min_tmo;
    struct ble_ll_conn_sm *connsm;

    rc = BLE_ERR_SUCCESS;

    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rc = BLE_ERR_UNK_CONN_ID;
        goto wr_auth_exit;
    }

    /*
     * The timeout is in units of 10 msecs. We need to make sure that the
     * timeout is greater than or equal to connItvl * (1 + slaveLatency)
     */
    tmo = get_le16(cmdbuf + 2);
    min_tmo = (uint32_t)connsm->conn_itvl * BLE_LL_CONN_ITVL_USECS;
    min_tmo *= (connsm->slave_latency + 1);
    min_tmo /= 10000;

    if (tmo < min_tmo) {
        rc = BLE_ERR_INV_HCI_CMD_PARMS;
    } else {
        connsm->auth_pyld_tmo = tmo;
        if (ble_npl_callout_is_active(&connsm->auth_pyld_timer)) {
            ble_ll_conn_auth_pyld_timer_start(connsm);
        }
    }

wr_auth_exit:
    put_le16(rsp, handle);
    *rsplen = BLE_HCI_WR_AUTH_PYLD_TMO_LEN;
    return rc;
}
#endif

#if (BLE_LL_BT5_PHY_SUPPORTED == 1)
/**
 * Read current phy for connection (OGF=8, OCF==0x0030)
 *
 * @param cmdbuf
 * @param rsplen
 *
 * @return int
 */
int
ble_ll_conn_hci_le_rd_phy(uint8_t *cmdbuf, uint8_t *rsp, uint8_t *rsplen)
{
    int rc;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        rc = BLE_ERR_UNK_CONN_ID;
    } else {
        rsp[2] = connsm->phy_data.cur_tx_phy;
        rsp[3] = connsm->phy_data.cur_rx_phy;
        rc = BLE_ERR_SUCCESS;
    }

    put_le16(rsp, handle);
    *rsplen = BLE_HCI_LE_RD_PHY_RSPLEN;
    return rc;
}

/**
 * Set PHY preferences for connection
 *
 * @param cmdbuf
 *
 * @return int
 */
int
ble_ll_conn_hci_le_set_phy(uint8_t *cmdbuf)
{
    int rc;
    uint16_t phy_options;
    uint8_t tx_phys;
    uint8_t rx_phys;
    uint16_t handle;
    struct ble_ll_conn_sm *connsm;

    handle = get_le16(cmdbuf);
    connsm = ble_ll_conn_find_active_conn(handle);
    if (!connsm) {
        return BLE_ERR_UNK_CONN_ID;
    }

    /*
     * If host has requested a PHY update and we are not finished do
     * not allow another one
     */
    if (CONN_F_HOST_PHY_UPDATE(connsm)) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    phy_options = get_le16(cmdbuf + 5);
    if (phy_options > BLE_HCI_LE_PHY_CODED_S8_PREF) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* Check valid parameters */
    rc = ble_ll_hci_chk_phy_masks(cmdbuf + 2, &tx_phys, &rx_phys);
    if (rc) {
        goto phy_cmd_param_err;
    }

    connsm->phy_data.phy_options = phy_options & 0x03;
    connsm->phy_data.host_pref_tx_phys_mask = tx_phys,
    connsm->phy_data.host_pref_rx_phys_mask = rx_phys;

    /*
     * The host preferences override the default phy preferences. Currently,
     * the only reason the controller will initiate a procedure on its own
     * is due to the fact that the host set default preferences. So if there
     * is a pending control procedure and it has not yet started, we do not
     * need to perform the default controller procedure.
     */
    if (IS_PENDING_CTRL_PROC(connsm, BLE_LL_CTRL_PROC_PHY_UPDATE)) {
        if (connsm->cur_ctrl_proc != BLE_LL_CTRL_PROC_PHY_UPDATE) {
            CONN_F_CTRLR_PHY_UPDATE(connsm) = 0;
        }
        CONN_F_HOST_PHY_UPDATE(connsm) = 1;
    } else {
        /*
         * We could be doing a peer-initiated PHY update procedure. If this
         * is the case the requested phy preferences will not both be 0. If
         * we are not done with a peer-initiated procedure we just set the
         * pending bit but do not start the control procedure.
         */
        if (CONN_F_PEER_PHY_UPDATE(connsm)) {
            connsm->pending_ctrl_procs |= (1 << BLE_LL_CTRL_PROC_PHY_UPDATE);
            CONN_F_HOST_PHY_UPDATE(connsm) = 1;
        } else {
            /* Check if we should start phy update procedure */
            if (!ble_ll_conn_chk_phy_upd_start(connsm)) {
                CONN_F_HOST_PHY_UPDATE(connsm) = 1;
            } else {
                /*
                 * Set flag to send a PHY update complete event. We set flag
                 * even if we do not do an update procedure since we have to
                 * inform the host even if we decide not to change anything.
                 */
                CONN_F_PHY_UPDATE_EVENT(connsm) = 1;
            }
        }
    }

phy_cmd_param_err:
    return rc;
}
#endif
