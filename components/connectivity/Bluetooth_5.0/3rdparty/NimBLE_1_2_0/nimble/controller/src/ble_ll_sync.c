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

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "syscfg/syscfg.h"

#include "controller/ble_ll.h"
#include "controller/ble_ll_hci.h"
#include "controller/ble_ll_sync.h"
#include "controller/ble_ll_utils.h"
#include "controller/ble_ll_sched.h"
#include "controller/ble_ll_whitelist.h"
#include "controller/ble_ll_scan.h"

#include "nimble/ble.h"
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"

#include "stats/stats.h"

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PERIODIC_ADV)

/* defines number of events that can be lost during sync establishment
 * before failed to be established error is reported
 */
#define BLE_LL_SYNC_ESTABLISH_CNT 6

#define BLE_LL_SYNC_CNT MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PERIODIC_ADV_SYNC_CNT)

#define BLE_LL_SYNC_SM_FLAG_ON_LIST         0x01
#define BLE_LL_SYNC_SM_FLAG_PENDING         0x02
#define BLE_LL_SYNC_SM_FLAG_ESTABLISHING    0x04
#define BLE_LL_SYNC_SM_FLAG_ESTABLISHED     0x08
#define BLE_LL_SYNC_SM_FLAG_SET_ANCHOR      0x10
#define BLE_LL_SYNC_SM_FLAG_OFFSET_300      0x20
#define BLE_LL_SYNC_SM_FLAG_SYNC_INFO       0x40

#define BLE_LL_SYNC_CHMAP_LEN               5
#define BLE_LL_SYNC_ITVL_USECS              1250

struct ble_ll_sync_sm {
    uint8_t flags;

    uint8_t adv_sid;
    uint8_t adv_addr[BLE_DEV_ADDR_LEN];
    uint8_t adv_addr_type;

    uint8_t sca;
    uint8_t chanmap[BLE_LL_SYNC_CHMAP_LEN];
    uint8_t num_used_chans;

    uint8_t chan_index;
    uint8_t chan_chain;

    uint8_t phy_mode;

    uint32_t timeout;
    uint16_t skip;

    uint16_t itvl;
    uint8_t  itvl_usecs;
    uint32_t itvl_ticks;

    uint32_t crcinit; /* only 3 bytes are used */
    uint32_t access_addr;
    uint16_t event_cntr;
    uint16_t channel_id;

    uint32_t window_widening;
    uint32_t last_anchor_point;
    uint32_t anchor_point;
    uint8_t anchor_point_usecs;

    struct ble_ll_sched_item sch;

    struct ble_npl_event sync_ev_end;

    uint8_t *next_report;
};

static struct ble_ll_sync_sm g_ble_ll_sync_sm[BLE_LL_SYNC_CNT];

static unsigned int g_ble_ll_sync_pending;

static struct ble_ll_sync_sm *g_ble_ll_sync_sm_establishing;
static struct ble_ll_sync_sm *g_ble_ll_sync_sm_current;
static uint8_t *g_ble_ll_sync_comp_ev;

static uint8_t
ble_ll_sync_get_handle(struct ble_ll_sync_sm *sm)
{
    /* handle number is offset in global array */
    return sm - g_ble_ll_sync_sm;
}

static void
ble_ll_sync_sm_clear(struct ble_ll_sync_sm *sm)
{
    if (sm->flags & (BLE_LL_SYNC_SM_FLAG_ESTABLISHING |
                     BLE_LL_SYNC_SM_FLAG_ESTABLISHED)) {
        ble_ll_sched_rmv_elem(&sm->sch);
        ble_npl_eventq_remove(&g_ble_ll_data.ll_evq, &sm->sync_ev_end);
    }

    if (sm->next_report) {
        ble_hci_trans_buf_free(sm->next_report);
    }

    memset(sm, 0, sizeof(*sm));
}

static uint8_t
ble_ll_sync_phy_mode_to_hci(int8_t phy_mode)
{
#if (BLE_LL_BT5_PHY_SUPPORTED == 1)
    switch (phy_mode) {
    case BLE_PHY_MODE_1M:
        return BLE_HCI_LE_PHY_1M;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_2M_PHY)
    case BLE_PHY_MODE_2M:
        return BLE_HCI_LE_PHY_2M;
#endif
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
    case BLE_PHY_MODE_CODED_125KBPS:
    case BLE_PHY_MODE_CODED_500KBPS:
        return BLE_HCI_LE_PHY_CODED;
#endif
    default:
        BLE_LL_ASSERT(false);
        return BLE_PHY_MODE_1M;
    }
#else
    return BLE_PHY_MODE_1M;
#endif
}

static void
ble_ll_sync_est_event_success(struct ble_ll_sync_sm *sm)
{
    struct hci_le_subev_periodic_adv_sync_estab *evt;
    uint8_t *evbuf;

    BLE_LL_ASSERT(g_ble_ll_sync_comp_ev);

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_PERIODIC_ADV_SYNC_ESTAB)) {
        evbuf = g_ble_ll_sync_comp_ev;
        evbuf[0] = BLE_HCI_EVCODE_LE_META;
        evbuf[1] = BLE_HCI_LE_PERIODIC_ADV_SYNC_ESTAB_LEN;
        evbuf[2] = BLE_HCI_LE_SUBEV_PERIODIC_ADV_SYNC_ESTAB;

        evt = (void *) evbuf + 3;

        evt->status = BLE_ERR_SUCCESS;
        evt->sync_handle = htole16(ble_ll_sync_get_handle(sm));
        evt->sid = sm->adv_sid;
        evt->adv_addr_type = sm->adv_addr_type;
        memcpy(evt->adv_addr, sm->adv_addr, BLE_DEV_ADDR_LEN);
        evt->adv_phy = ble_ll_sync_phy_mode_to_hci(sm->phy_mode);
        evt->per_adv_ival = htole16(sm->itvl);
        evt->adv_clk_accuracy = sm->sca;

        ble_ll_hci_event_send(evbuf);
    } else {
        ble_hci_trans_buf_free(g_ble_ll_sync_comp_ev);
    }

    g_ble_ll_sync_comp_ev = NULL;
}

static void
ble_ll_sync_est_event_failed(uint8_t status)
{
    struct hci_le_subev_periodic_adv_sync_estab *evt;
    uint8_t *evbuf;

    BLE_LL_ASSERT(g_ble_ll_sync_comp_ev);

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_PERIODIC_ADV_SYNC_ESTAB)) {
        evbuf = g_ble_ll_sync_comp_ev;
        evbuf[0] = BLE_HCI_EVCODE_LE_META;
        evbuf[1] = BLE_HCI_LE_PERIODIC_ADV_SYNC_ESTAB_LEN;
        evbuf[2] = BLE_HCI_LE_SUBEV_PERIODIC_ADV_SYNC_ESTAB;

        evt = (void *) evbuf + 3;

        memset(evt, 0, sizeof(*evt));
        evt->status = status;

        ble_ll_hci_event_send(evbuf);
    } else {
        ble_hci_trans_buf_free(g_ble_ll_sync_comp_ev);
    }

    g_ble_ll_sync_comp_ev = NULL;
}

static void
ble_ll_sync_lost_event(struct ble_ll_sync_sm *sm)
{
    struct hci_le_subev_periodic_adv_sync_lost *evt;
    uint8_t *evbuf;

    if (ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_PERIODIC_ADV_SYNC_LOST)) {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        if (evbuf) {
            evbuf[0] = BLE_HCI_EVCODE_LE_META;
            evbuf[1] = BLE_HCI_LE_PERIODIC_ADV_SYNC_LOST_LEN;
            evbuf[2] = BLE_HCI_LE_SUBEV_PERIODIC_ADV_SYNC_LOST;

            evt = (void *) evbuf + 3;
            evt->sync_handle = htole16(ble_ll_sync_get_handle(sm));

            ble_ll_hci_event_send(evbuf);
        }
    }

    ble_ll_sync_sm_clear(sm);
}

static struct ble_ll_sync_sm *
ble_ll_sync_find(const uint8_t *addr, uint8_t addr_type, uint8_t sid)
{
    struct ble_ll_sync_sm *sm;
    int i;

    for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
        sm = &g_ble_ll_sync_sm[i];

        if (!sm->flags) {
            continue;
        }
        if ((sm->adv_sid == sid) && (sm->adv_addr_type == addr_type) &&
                !memcmp(&sm->adv_addr, addr, BLE_DEV_ADDR_LEN)) {
            return sm;
        }
    }

    return NULL;
}

static struct ble_ll_sync_sm *
ble_ll_sync_get(const uint8_t *addr, uint8_t addr_type, uint8_t sid)
{
    struct ble_ll_sync_sm *sm;
    int i;

    sm = ble_ll_sync_find(addr, addr_type, sid);
    if (sm) {
        return sm;
    }

    for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
        sm = &g_ble_ll_sync_sm[i];

        if (!sm->flags) {
            sm->adv_sid = sid;
            sm->adv_addr_type = addr_type;
            memcpy(&sm->adv_addr, addr, BLE_DEV_ADDR_LEN);
            return sm;
        }
    }

    return NULL;
}

static void
ble_ll_sync_current_sm_over(void)
{
    /* Disable the PHY */
    ble_phy_disable();

    /* Disable the wfr timer */
    ble_ll_wfr_disable();

    /* Link-layer is in standby state now */
    ble_ll_state_set(BLE_LL_STATE_STANDBY);

    /* Set current LL sync to NULL */
    g_ble_ll_sync_sm_current = NULL;
}

static int
ble_ll_sync_event_start_cb(struct ble_ll_sched_item *sch)
{
    struct ble_ll_sync_sm *sm;
    uint32_t wfr_usecs;
    uint32_t start;
    int rc;

    /* Set current connection state machine */
    sm = sch->cb_arg;
    g_ble_ll_sync_sm_current = sm;
    BLE_LL_ASSERT(sm);

    /* Disable whitelisting */
    ble_ll_whitelist_disable();

    /* Set LL state */
    ble_ll_state_set(BLE_LL_STATE_SYNC);

    /* Set channel */
    ble_phy_setchan(sm->chan_index, sm->access_addr, sm->crcinit);

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY)
    ble_phy_resolv_list_disable();
#endif

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION)
    ble_phy_encrypt_disable();
#endif

#if (BLE_LL_BT5_PHY_SUPPORTED == 1)
    ble_phy_mode_set(sm->phy_mode, sm->phy_mode);
#endif

    start = sch->start_time + g_ble_ll_sched_offset_ticks;
    rc = ble_phy_rx_set_start_time(start, sch->remainder);
    if (rc && rc != BLE_PHY_ERR_RX_LATE) {
        STATS_INC(ble_ll_stats, sync_event_failed);
        rc = BLE_LL_SCHED_STATE_DONE;
        ble_ll_event_send(&sm->sync_ev_end);
        ble_ll_sync_current_sm_over();
    } else {
        /*
         * Set flag that tells to set last anchor point if a packet
         * has been received.
         */
        sm->flags |= BLE_LL_SYNC_SM_FLAG_SET_ANCHOR;

        /* set wfr timer, for first PDU pointed with AuxPtr we use offset
         * unit, for other PDU we use current window widening
         */
        if (sm->flags & BLE_LL_SYNC_SM_FLAG_SYNC_INFO) {
            wfr_usecs = (sm->flags & BLE_LL_SYNC_SM_FLAG_OFFSET_300) ? 300 : 30;
        } else {
            wfr_usecs = 2 * sm->window_widening;
        }
        ble_phy_wfr_enable(BLE_PHY_WFR_ENABLE_RX, 0, wfr_usecs);

        rc = BLE_LL_SCHED_STATE_RUNNING;
    }

    sm->flags &= ~BLE_LL_SYNC_SM_FLAG_SYNC_INFO;

    return rc;
}

/**
 * Called when a receive PDU has started.
 *
 * Context: interrupt
 *
 * @return int
 *   < 0: A frame we dont want to receive.
 *   = 0: Continue to receive frame. Dont go from rx to tx
 */
int
ble_ll_sync_rx_isr_start(uint8_t pdu_type, struct ble_mbuf_hdr *rxhdr)
{
    struct ble_ll_sync_sm *sm = g_ble_ll_sync_sm_current;

    BLE_LL_ASSERT(g_ble_ll_sync_sm_current);

    /* this also handles chains as those have same PDU type */
    if (pdu_type != BLE_ADV_PDU_TYPE_AUX_SYNC_IND) {
        ble_ll_event_send(&sm->sync_ev_end);
        ble_ll_sync_current_sm_over();
        STATS_INC(ble_ll_stats, sched_invalid_pdu);
        return -1;
    }

    /* Set anchor point (and last) if 1st rxd frame in sync event.
     * According to spec this should be done even if CRC is not valid so we
     * can store it here
     */
    if (sm->flags & BLE_LL_SYNC_SM_FLAG_SET_ANCHOR) {
        sm->flags &= ~BLE_LL_SYNC_SM_FLAG_SET_ANCHOR;

        sm->anchor_point = rxhdr->beg_cputime;
        sm->anchor_point_usecs = rxhdr->rem_usecs;
        sm->last_anchor_point = sm->anchor_point;
    }

    STATS_INC(ble_ll_stats, sync_received);
    return 0;
}

int
ble_ll_sync_parse_ext_hdr(struct os_mbuf *om, uint8_t **aux, int8_t *tx_power)
{
    uint8_t *rxbuf = om->om_data;
    uint8_t ext_hdr_flags;
    uint8_t ext_hdr_len;
    uint8_t *ext_hdr;
    uint8_t pdu_len;
    int i;

    pdu_len = rxbuf[1];
    if (pdu_len == 0) {
        return -1;
    }
    ext_hdr_len = rxbuf[2] & 0x3F;
    if (ext_hdr_len > pdu_len) {
        return -1;
    }

    os_mbuf_adj(om, 3);

    if (ext_hdr_len) {
        ext_hdr_flags = rxbuf[3];
        ext_hdr = &rxbuf[4];

        i = 0;

        /* TODO should we ignore packets with extra data instead of just
         * skipping fields?
         */

        /* there should be no AdvA in Sync or chain, skip it */
        if (ext_hdr_flags & (1 << BLE_LL_EXT_ADV_ADVA_BIT)) {
            i += BLE_LL_EXT_ADV_ADVA_SIZE;
        }

        /* there should be no TargetA in Sync or chain, skip it */
        if (ext_hdr_flags & (1 << BLE_LL_EXT_ADV_TARGETA_BIT)) {
            i += BLE_LL_EXT_ADV_TARGETA_SIZE;
        }

        /* there should be no RFU in Sync or chain, skip it */
        if (ext_hdr_flags & (1 << BLE_LL_EXT_ADV_RFU_BIT)) {
            i += 1;
        }

        /* there should be no ADI in Sync or chain, skip it */
        if (ext_hdr_flags & (1 << BLE_LL_EXT_ADV_DATA_INFO_BIT)) {
            i += BLE_LL_EXT_ADV_DATA_INFO_SIZE;
        }

        /* get AuXPTR if present */
        if (ext_hdr_flags & (1 << BLE_LL_EXT_ADV_AUX_PTR_BIT)) {
            *aux = ext_hdr + i;
            i += BLE_LL_EXT_ADV_AUX_PTR_SIZE;
        } else {
            *aux = NULL;
        }

        /* there should be no SyncInfo in Sync or chain, skip it */
        if (ext_hdr_flags & (1 << BLE_LL_EXT_ADV_SYNC_INFO_BIT)) {
            i += BLE_LL_EXT_ADV_SYNC_INFO_SIZE;
        }

        if (ext_hdr_flags & (1 << BLE_LL_EXT_ADV_TX_POWER_BIT)) {
            *tx_power = *(ext_hdr + i);
            i += BLE_LL_EXT_ADV_TX_POWER_SIZE;
        } else {
            *tx_power = 127; /* not available */
        }

        /* TODO Handle ACAD if needed */
    }

    if (ext_hdr_len) {
        /* Adjust mbuf to contain advertising data only */
        os_mbuf_adj(om, ext_hdr_len);
    }

    return pdu_len - ext_hdr_len - 1;
}

static void
ble_ll_sync_send_truncated_per_adv_rpt(struct ble_ll_sync_sm *sm, uint8_t *evbuf)
{
    struct hci_le_subev_periodic_adv_rpt *evt;

    if (!ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_PERIODIC_ADV_RPT)) {
        ble_hci_trans_buf_free(evbuf);
        return;
    }

    evbuf[0] = BLE_HCI_EVCODE_LE_META;
    evbuf[1] = BLE_HCI_LE_PERIODIC_ADV_RPT_LEN;
    evbuf[2] = BLE_HCI_LE_SUBEV_PERIODIC_ADV_RPT;

    evt = (void *) evbuf + 3;
    evt->sync_handle = htole16(ble_ll_sync_get_handle(sm));
    evt->tx_power = 127; /* not available */
    evt->rssi = 127; /* not available */
    evt->unused = 0xff;
    evt->data_status = BLE_HCI_PERIODIC_DATA_STATUS_TRUNCATED;
    evt->data_length = 0;
    ble_ll_hci_event_send(evbuf);
}

static int
ble_ll_sync_send_per_adv_rpt(struct ble_ll_sync_sm *sm, struct os_mbuf *rxpdu,
                             struct ble_mbuf_hdr *hdr, uint8_t **aux)
{
    struct hci_le_subev_periodic_adv_rpt *evt;
    uint8_t *evbuf_next = NULL;
    uint16_t max_event_len;
    int8_t tx_power;
    uint8_t *evbuf;
    int datalen;
    int offset;
    int rc;

    if (!ble_ll_hci_is_le_event_enabled(BLE_HCI_LE_SUBEV_PERIODIC_ADV_RPT)) {
        return -1;
    }

    datalen = ble_ll_sync_parse_ext_hdr(rxpdu, aux, &tx_power);
    if (datalen < 0) {
        /* we got bad packet but were chaining, send truncated report */
        if (sm->next_report) {
            ble_ll_sync_send_truncated_per_adv_rpt(sm, sm->next_report);
            sm->next_report = NULL;
        }
        return -1;
    }

    /* use next report buffer if present, this means we are chaining */
    if (sm->next_report) {
        evbuf = sm->next_report;
        sm->next_report = NULL;
    } else {
        evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_LO);
        if (!evbuf) {
            return -1;
        }
    }

    /* Max_event_len contains advertising data and BLE_HCI_EVENT_HDR_LEN as this
     * is related to the buffer available for the event. The maximum is 255 + 2
     */
    offset = 0;
    max_event_len = min(UINT8_MAX + BLE_HCI_EVENT_HDR_LEN, BLE_LL_MAX_EVT_LEN);

    do {
        if (evbuf_next) {
            evbuf = evbuf_next;
            evbuf_next = NULL;
        }

        evbuf[0] = BLE_HCI_EVCODE_LE_META;
        evbuf[1] = BLE_HCI_LE_PERIODIC_ADV_RPT_LEN;
        evbuf[2] = BLE_HCI_LE_SUBEV_PERIODIC_ADV_RPT;

        evt = (void *) evbuf + 3;
        evt->sync_handle = htole16(ble_ll_sync_get_handle(sm));
        evt->tx_power = tx_power;
        evt->rssi = hdr->rxinfo.rssi;
        evt->unused = 0xff;

        evt->data_length = min(max_event_len - sizeof(*evt) - 3, datalen - offset);
        /* adjust event length */
        evbuf[1] += evt->data_length;

        os_mbuf_copydata(rxpdu, offset, evt->data_length, evt->data);
        offset += evt->data_length;

        /* Need another event for next fragment of this PDU */
        if ((offset < datalen) || *aux) {
            evbuf_next = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_LO);
            if (evbuf_next) {
                evt->data_status = BLE_HCI_PERIODIC_DATA_STATUS_INCOMPLETE;
                rc = 0;
            } else {
                evt->data_status = BLE_HCI_PERIODIC_DATA_STATUS_TRUNCATED;
                rc = -1;
            }
        } else {
            evt->data_status = BLE_HCI_PERIODIC_DATA_STATUS_COMPLETE;
            rc = 0;
        }

        ble_ll_hci_event_send(evbuf);
    } while ((offset < datalen) && evbuf_next);

    /* store for chain */
    sm->next_report = evbuf_next;

    return rc;
}

/**
 * Called when a receive PDU has ended.
 *
 * Context: Interrupt
 *
 * @param rxpdu
 *
 * @return int
 *       < 0: Disable the phy after reception.
 *      == 0: Success. Do not disable the PHY.
 *       > 0: Do not disable PHY as that has already been done.
 */
int
ble_ll_sync_rx_isr_end(uint8_t *rxbuf, struct ble_mbuf_hdr *rxhdr)
{
    struct ble_ll_sync_sm *sm = g_ble_ll_sync_sm_current;
    struct ble_mbuf_hdr *ble_hdr;
    struct os_mbuf *rxpdu;

    BLE_LL_ASSERT(sm);

    ble_ll_sync_current_sm_over();

    /* type was verified in isr_start */

    rxpdu = ble_ll_rxpdu_alloc(rxbuf[1] + BLE_LL_PDU_HDR_LEN);
    if (rxpdu) {
        ble_phy_rxpdu_copy(rxbuf, rxpdu);

        ble_hdr = BLE_MBUF_HDR_PTR(rxpdu);
        ble_hdr->rxinfo.user_data = sm;

        ble_ll_rx_pdu_in(rxpdu);
    } else {
        STATS_INC(ble_ll_stats, sync_rx_buf_err);
        ble_ll_event_send(&sm->sync_ev_end);
    }

    return -1;
}

/**
 * Called when the wait for response timer expires while in the sync state.
 *
 * Context: Interrupt.
 */
void
ble_ll_sync_wfr_timer_exp(void)
{
    struct ble_ll_sync_sm *sm = g_ble_ll_sync_sm_current;

    BLE_LL_ASSERT(g_ble_ll_sync_sm_current);
    STATS_INC(ble_ll_stats, sync_missed_err);

    ble_ll_sync_current_sm_over();
    ble_ll_event_send(&sm->sync_ev_end);
}

/**
 * Called when sync event needs to be halted. This normally should not be called
 * and is only called when a scheduled item executes but scanning for sync/chain
 * is stil ongoing
 * Context: Interrupt
 */
void
ble_ll_sync_halt(void)
{
    struct ble_ll_sync_sm *sm = g_ble_ll_sync_sm_current;

    ble_ll_sync_current_sm_over();

    if (sm) {
        ble_ll_event_send(&sm->sync_ev_end);
    }
}

uint32_t
ble_ll_sync_get_event_end_time(void)
{
    uint32_t end_time;

    if (g_ble_ll_sync_sm_current) {
        end_time = g_ble_ll_sync_sm_current->sch.end_time;
    } else {
        end_time = os_cputime_get32();
    }
    return end_time;
}

static uint8_t
ble_ll_sync_phy_mode_to_aux_phy(uint8_t phy_mode)
{
    switch (phy_mode) {
    case BLE_PHY_MODE_1M:
        return 0x00;
    case BLE_PHY_MODE_2M:
        return 0x01;
    case BLE_PHY_MODE_CODED_125KBPS:
    case BLE_PHY_MODE_CODED_500KBPS:
        return 0x02;
    default:
        BLE_LL_ASSERT(false);
        return 0x00;
    }
}

static void
ble_ll_sync_parse_aux_ptr(const uint8_t *buf, uint8_t *chan, uint32_t *offset,
                          uint8_t *offset_units, uint8_t *phy)
{
    uint32_t aux_ptr_field = get_le32(buf) & 0x00FFFFFF;

    *chan = aux_ptr_field & 0x3F;

    /* TODO use CA aux_ptr_field >> 6 */

    if ((aux_ptr_field >> 7) & 0x01) {
        *offset = 300 * ((aux_ptr_field >> 8) & 0x1FFF);
        *offset_units = 1;
    } else {
        *offset = 30 * ((aux_ptr_field >> 8) & 0x1FFF);
        *offset_units = 0;
    }

    *phy = (aux_ptr_field >> 21) & 0x07;
}

static int
ble_ll_sync_chain_start_cb(struct ble_ll_sched_item *sch)
{
    struct ble_ll_sync_sm *sm;
    uint32_t wfr_usecs;
    uint32_t start;
    int rc;

    /* Set current connection state machine */
    sm = sch->cb_arg;
    g_ble_ll_sync_sm_current = sm;
    BLE_LL_ASSERT(sm);
    BLE_LL_ASSERT(sm->next_report);

    /* Disable whitelisting */
    ble_ll_whitelist_disable();

    /* Set LL state */
    ble_ll_state_set(BLE_LL_STATE_SYNC);

    /* Set channel */
    ble_phy_setchan(sm->chan_chain, sm->access_addr, sm->crcinit);

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY)
    ble_phy_resolv_list_disable();
#endif

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION)
    ble_phy_encrypt_disable();
#endif

#if (BLE_LL_BT5_PHY_SUPPORTED == 1)
    ble_phy_mode_set(sm->phy_mode, sm->phy_mode);
#endif

    start = sch->start_time + g_ble_ll_sched_offset_ticks;
    rc = ble_phy_rx_set_start_time(start, sch->remainder);
    if (rc && rc != BLE_PHY_ERR_RX_LATE) {
        STATS_INC(ble_ll_stats, sync_chain_failed);
        rc = BLE_LL_SCHED_STATE_DONE;
        ble_ll_event_send(&sm->sync_ev_end);
        ble_ll_sync_current_sm_over();
    } else {
        /*
         * Clear flag that tells to set last anchor point if a packet
         * has been received, this is chain and we don't need it.
         */
        sm->flags &= ~BLE_LL_SYNC_SM_FLAG_SET_ANCHOR;

        wfr_usecs = (sm->flags & BLE_LL_SYNC_SM_FLAG_OFFSET_300) ? 300 : 30;

        ble_phy_wfr_enable(BLE_PHY_WFR_ENABLE_RX, 0, wfr_usecs);
        rc = BLE_LL_SCHED_STATE_RUNNING;
    }

    return rc;
}

static int
ble_ll_sync_schedule_chain(struct ble_ll_sync_sm *sm, struct ble_mbuf_hdr *hdr,
                           const uint8_t *aux)
{
    uint8_t offset_units;
    uint32_t offset;
    uint8_t chan;
    uint8_t phy;

    ble_ll_sync_parse_aux_ptr(aux, &chan, &offset, &offset_units, &phy);

    if (chan >= BLE_PHY_NUM_DATA_CHANS) {
        return -1;
    }

    if (offset < BLE_LL_MAFS) {
        return -1;
    }

    /* chain should use same PHY as master PDU */
    if (phy != ble_ll_sync_phy_mode_to_aux_phy(sm->phy_mode)) {
        return -1;
    }

    if (offset_units) {
        sm->flags |= BLE_LL_SYNC_SM_FLAG_OFFSET_300;
    } else {
        sm->flags &= ~BLE_LL_SYNC_SM_FLAG_OFFSET_300;
    }

    sm->chan_chain = chan;

    sm->sch.sched_cb = ble_ll_sync_chain_start_cb;
    sm->sch.cb_arg = sm;
    sm->sch.sched_type = BLE_LL_SCHED_TYPE_SYNC;

    return ble_ll_sched_sync(&sm->sch, hdr, offset, sm->phy_mode);
}

static void
ble_ll_sync_established(struct ble_ll_sync_sm *sm)
{
    int i;

    BLE_LL_ASSERT(sm == g_ble_ll_sync_sm_establishing);
    BLE_LL_ASSERT(g_ble_ll_sync_pending);

    /* mark as established */
    ble_ll_sync_est_event_success(sm);
    sm->flags |= BLE_LL_SYNC_SM_FLAG_ESTABLISHED;
    sm->flags &= ~BLE_LL_SYNC_SM_FLAG_ESTABLISHING;

    /* clear as we are not longer pending sync here */
    for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
        g_ble_ll_sync_sm[i].flags &= ~BLE_LL_SYNC_SM_FLAG_PENDING;
    }

    g_ble_ll_sync_sm_establishing = NULL;
    g_ble_ll_sync_pending = 0;
}

static void
ble_ll_sync_check_failed(struct ble_ll_sync_sm *sm)
{
    int i;

    BLE_LL_ASSERT(sm == g_ble_ll_sync_sm_establishing);
    BLE_LL_ASSERT(g_ble_ll_sync_pending);

    /* if we can retry on next event */
    if (--g_ble_ll_sync_pending) {
        return;
    }

    ble_ll_sync_est_event_failed(BLE_ERR_CONN_ESTABLISHMENT);

    sm->flags &= ~BLE_LL_SYNC_SM_FLAG_ESTABLISHING;

    /* clear as we are not longer pending sync here */
    for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
        g_ble_ll_sync_sm[i].flags &= ~BLE_LL_SYNC_SM_FLAG_PENDING;
    }

    g_ble_ll_sync_sm_establishing = NULL;
    g_ble_ll_sync_pending = 0;
}

void
ble_ll_sync_rx_pkt_in(struct os_mbuf *rxpdu, struct ble_mbuf_hdr *hdr)
{
    struct ble_ll_sync_sm *sm = hdr->rxinfo.user_data;
    uint8_t *aux = NULL;

    BLE_LL_ASSERT(sm);

    /* this could happen if sync was terminated while pkt_in was already
     * in LL queue, just drop in that case
     */
    if (!sm->flags) {
        ble_ll_scan_chk_resume();
        return;
    }

    /* CRC error, end event */
    if (!BLE_MBUF_HDR_CRC_OK(hdr)) {
        STATS_INC(ble_ll_stats, sync_crc_err);
        goto end_event;
    }

    /* if packet is good we send sync established here */
    if (sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHING) {
        ble_ll_sync_established(sm);
    }

    /* send report to host, if this fails we end event */
    if (ble_ll_sync_send_per_adv_rpt(sm, rxpdu, hdr, &aux) < 0) {
        goto end_event;
    }

    /* schedule for chain packet if AUX pointer was present */
    if (sm->next_report && aux) {
        if (ble_ll_sync_schedule_chain(sm, hdr, aux) < 0) {
            goto end_event;
        }

        /* if chain was scheduled we don't end event yet */
        /* TODO should we check resume only if offset is high? */
        ble_ll_scan_chk_resume();
        return;
    }

end_event:
    ble_ll_event_send(&sm->sync_ev_end);
}

static int
ble_ll_sync_next_event(struct ble_ll_sync_sm *sm)
{
    uint32_t cur_ww;
    uint32_t max_ww;
    uint32_t ticks;
    uint32_t itvl;
    uint8_t usecs;
    uint16_t skip = sm->skip;

    /* don't skip if are establishing sync or we missed last event */
    if (skip && ((sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHING) ||
                  CPUTIME_LT(sm->last_anchor_point, sm->anchor_point))) {
        skip = 0;
    }

    /* Set next event start time, we can use pre-calculated values for one
     * interval if not skipping
     */
    if (skip == 0) {
        ticks = sm->itvl_ticks;
        usecs = sm->itvl_usecs;
    } else {
        itvl = sm->itvl * BLE_LL_SYNC_ITVL_USECS * (1 + skip);
        ticks = os_cputime_usecs_to_ticks(itvl);
        usecs = itvl - os_cputime_ticks_to_usecs(ticks);
    }

    sm->anchor_point += ticks;
    sm->anchor_point_usecs += usecs;
    if (sm->anchor_point_usecs >= 31) {
        sm->anchor_point++;
        sm->anchor_point_usecs -= 31;
    }

    /* Set event counter to the next event */
    sm->event_cntr += 1 + skip;

    /* Calculate channel index of next event */
    sm->chan_index = ble_ll_utils_calc_dci_csa2(sm->event_cntr, sm->channel_id,
                                                sm->num_used_chans, sm->chanmap);

    cur_ww = ble_ll_utils_calc_window_widening(sm->anchor_point,
                                               sm->last_anchor_point,
                                               sm->sca);

    max_ww = (sm->itvl * (BLE_LL_SYNC_ITVL_USECS / 2)) - BLE_LL_IFS;
    if (cur_ww >= max_ww) {
        return -1;
    }

    /* if we are establishing, adjust window widening with offset unit
     * uncertainty, since we set anchor point in the middle of offset window
     * it is enough to adjust for half of offset
     */
    if (sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHING) {
        if (sm->flags & BLE_LL_SYNC_SM_FLAG_OFFSET_300) {
            cur_ww += 150;
        } else {
            cur_ww += 15;
        }
    }

    cur_ww += BLE_LL_JITTER_USECS;

    /* if updated anchor is pass last anchor + timeout it means we will not be
     * able to get it in time and hit sync timeout
     *
     * note that this may result in sync timeout being sent before real
     * timeout but we won't be able to fit in time anyway..
     *
     * We don't do that when establishing since we try up to
     * BLE_LL_SYNC_ESTABLISH_CNT events before failing regardless of timeout
     */
    if (!(sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHING)) {
        if (CPUTIME_GT(sm->anchor_point - os_cputime_usecs_to_ticks(cur_ww),
                       sm->last_anchor_point + sm->timeout )) {
            return -1;
        }
    }

    sm->window_widening = cur_ww;

    return 0;
}

static void
ble_ll_sync_event_end(struct ble_npl_event *ev)
{
    struct ble_ll_sync_sm *sm;

    /* Better be a connection state machine! */
    sm = ble_npl_event_get_arg(ev);
    BLE_LL_ASSERT(sm);

    if (sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHING) {
        ble_ll_sync_check_failed(sm);
    }

    /* Check if we need to resume scanning */
    ble_ll_scan_chk_resume();

#ifdef BLE_XCVR_RFCLK
    ble_ll_sched_rfclk_chk_restart();
#endif

    /* Remove any end events that might be enqueued */
    ble_npl_eventq_remove(&g_ble_ll_data.ll_evq, &sm->sync_ev_end);

    /* don't schedule next event if sync is not established nor establishing */
    if (!(sm->flags & (BLE_LL_SYNC_SM_FLAG_ESTABLISHED |
                       BLE_LL_SYNC_SM_FLAG_ESTABLISHING))) {
        return;
    }

    /* if we had prepared buffer for next even it means we were chaining and
     * must send truncated report to host
     */
    if (sm->next_report) {
        ble_ll_sync_send_truncated_per_adv_rpt(sm, sm->next_report);
        sm->next_report = NULL;
    }

    sm->sch.sched_cb = ble_ll_sync_event_start_cb;
    sm->sch.cb_arg = sm;
    sm->sch.sched_type = BLE_LL_SCHED_TYPE_SYNC;

    do {
        if (ble_ll_sync_next_event(sm) < 0) {
            if (sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHING) {
                /* don't allow any retry if this failed */
                g_ble_ll_sync_pending = 1;
                ble_ll_sync_check_failed(sm);
            } else {
                ble_ll_sync_lost_event(sm);
            }
            return;
        }
    } while (ble_ll_sched_sync_reschedule(&sm->sch, sm->anchor_point,
                                          sm->anchor_point_usecs,
                                          sm->window_widening, sm->phy_mode));
}

static uint16_t
get_max_skip(uint32_t interval_us, uint32_t timeout_us)
{
    BLE_LL_ASSERT(interval_us);
    BLE_LL_ASSERT(timeout_us);

    if (timeout_us <= interval_us) {
        return 0;
    }

    return (timeout_us / interval_us) - 1;
}

void
ble_ll_sync_info_event(const uint8_t *addr, uint8_t addr_type, uint8_t sid,
                       struct ble_mbuf_hdr *rxhdr, const uint8_t *syncinfo)
{
    struct ble_ll_sync_sm *sm;
    uint16_t max_skip;
    uint32_t offset;
    uint32_t usecs;

    if (!g_ble_ll_sync_pending || g_ble_ll_sync_sm_establishing) {
        return;
    }

    sm = ble_ll_sync_find(addr, addr_type, sid);
    if (!sm) {
        return;
    }

    /* don't attempt to synchronize again if already synchronized */
    if (sm->flags & (BLE_LL_SYNC_SM_FLAG_ESTABLISHING |
                     BLE_LL_SYNC_SM_FLAG_ESTABLISHED)) {
            return;
    }

    /* check if this SM is allowed to establish new sync */
    if (!(sm->flags & BLE_LL_SYNC_SM_FLAG_PENDING)) {
            return;
    }

    /* Sync Packet Offset (13 bits), Offset Units (1 bit), RFU (2 bits) */
    offset = syncinfo[0];
    offset |= (uint16_t)(syncinfo[1] & 0x1f) << 8;

    /* ignore if offset is not valid */
    if (!offset) {
        return;
    }

    if (syncinfo[1] & 0x20) {
        offset *= 300;
        sm->flags |= BLE_LL_SYNC_SM_FLAG_OFFSET_300;
    } else {
        offset *= 30;
        sm->flags &= ~BLE_LL_SYNC_SM_FLAG_OFFSET_300;
    }

    /* sync end event */
    ble_npl_event_init(&sm->sync_ev_end, ble_ll_sync_event_end, sm);

    /* Interval (2 bytes) */
    sm->itvl = get_le16(&syncinfo[2]);

    /* ignore if interval is invalid */
    if (sm->itvl < 6) {
        return;
    }

    /* precalculate interval ticks and usecs */
    usecs = sm->itvl * BLE_LL_SYNC_ITVL_USECS;
    sm->itvl_ticks = os_cputime_usecs_to_ticks(usecs);
    sm->itvl_usecs = (uint8_t)(usecs -
                               os_cputime_ticks_to_usecs(sm->itvl_ticks));
    if (sm->itvl_usecs == 31) {
        sm->itvl_usecs = 0;
        sm->itvl_ticks++;
    }

    /* Channels Mask (37 bits) */
    sm->chanmap[0] = syncinfo[4];
    sm->chanmap[1] = syncinfo[5];
    sm->chanmap[2] = syncinfo[6];
    sm->chanmap[3] = syncinfo[7];
    sm->chanmap[4] = syncinfo[8] & 0x1f;
    sm->num_used_chans = ble_ll_utils_calc_num_used_chans(sm->chanmap);

    /* SCA (3 bits) */
    sm->sca = syncinfo[8] >> 5;

    /* AA (4 bytes) */
    sm->access_addr = get_le32(&syncinfo[9]);
    sm->channel_id = ((sm->access_addr & 0xffff0000) >> 16) ^
                      (sm->access_addr & 0x0000ffff);

    /* CRCInit (3 bytes) */
    sm->crcinit = syncinfo[15];
    sm->crcinit = (sm->crcinit << 8) | syncinfo[14];
    sm->crcinit = (sm->crcinit << 8) | syncinfo[13];

    /* Event Counter (2 bytes) */
    sm->event_cntr = get_le16(&syncinfo[16]);

    /* adjust skip if pass timeout */
    max_skip = get_max_skip(sm->itvl * BLE_LL_SYNC_ITVL_USECS, sm->timeout);
    if (sm->skip > max_skip) {
        sm->skip = max_skip;
    }

    /* from now on we only need timeout in ticks */
    sm->timeout = os_cputime_usecs_to_ticks(sm->timeout);

    sm->phy_mode = rxhdr->rxinfo.phy_mode;
    sm->window_widening = BLE_LL_JITTER_USECS;

    /* Calculate channel index of first event */
    sm->chan_index = ble_ll_utils_calc_dci_csa2(sm->event_cntr, sm->channel_id,
                                                sm->num_used_chans, sm->chanmap);

    sm->sch.sched_cb = ble_ll_sync_event_start_cb;
    sm->sch.cb_arg = sm;
    sm->sch.sched_type = BLE_LL_SCHED_TYPE_SYNC;

    if (ble_ll_sched_sync(&sm->sch, rxhdr, offset, sm->phy_mode)) {
        return;
    }

    sm->anchor_point = sm->sch.start_time + g_ble_ll_sched_offset_ticks;
    sm->anchor_point_usecs = sm->sch.remainder;

    /* set anchor point in middle of offset window */
    if (sm->flags & BLE_LL_SYNC_SM_FLAG_OFFSET_300) {
        sm->anchor_point_usecs += 150;
    } else {
        sm->anchor_point_usecs += 15;
    }

    while (sm->anchor_point_usecs >= 31) {
        sm->anchor_point++;
        sm->anchor_point_usecs -= 31;
    }

    sm->last_anchor_point = sm->anchor_point;

    /* keep sm for which we want to establish new sync */
    sm->flags |= BLE_LL_SYNC_SM_FLAG_ESTABLISHING;
    sm->flags |= BLE_LL_SYNC_SM_FLAG_SYNC_INFO;
    g_ble_ll_sync_sm_establishing = sm;
}

int
ble_ll_sync_create(uint8_t *cmdbuf)
{
    struct ble_ll_sync_sm *sm;
    uint8_t filter_policy;
    uint8_t addr_type;
    uint16_t timeout;
    uint8_t *addr;
    uint16_t skip;
    uint8_t sid;
    os_sr_t sr;
    int cnt;
    int i;

    if (g_ble_ll_sync_pending) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    filter_policy = cmdbuf[0];
    if (filter_policy > 0x01) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    skip = get_le16(cmdbuf + 9);
    if (skip > 0x01f3) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    timeout = get_le16(cmdbuf + 11);
    if (timeout < 0x000a && timeout > 0x4000) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* check if list is sane */
    if (filter_policy) {
        cnt = 0;

        OS_ENTER_CRITICAL(sr);

        for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
            sm = &g_ble_ll_sync_sm[i];

            if (!(sm->flags & BLE_LL_SYNC_SM_FLAG_ON_LIST)) {
                continue;
            }

            /* skip if already synchronized
             * TODO should we return 0x0B if list is to be used?
             */
            if (sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHED) {
                continue;
            }

            /* mark as pending */
            sm->flags |= BLE_LL_SYNC_SM_FLAG_PENDING;
            sm->skip = skip;
            sm->timeout = timeout * 10000; /* 10ms units, store in us */
            cnt++;
        }

        OS_EXIT_CRITICAL(sr);

        /* if nothing on list return error
         * TODO is this valid behavior?
         */
        if (!cnt) {
            return BLE_ERR_CMD_DISALLOWED;
        }
    } else {
        sid = cmdbuf[1];
        if (sid > 0x0f) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        addr_type = cmdbuf[2];
        if (addr_type > BLE_HCI_ADV_PEER_ADDR_MAX) {
            return BLE_ERR_INV_HCI_CMD_PARMS;
        }

        addr = &cmdbuf[3];

        sm = ble_ll_sync_get(addr, addr_type, sid);
        if (!sm) {
            return BLE_ERR_MEM_CAPACITY;
        }

        OS_ENTER_CRITICAL(sr);

        /* if we already have link established return error as per spec */
        if (sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHED) {
            OS_EXIT_CRITICAL(sr);
            return BLE_ERR_ACL_CONN_EXISTS;
        }

        /* mark as pending */
        sm->flags |= BLE_LL_SYNC_SM_FLAG_PENDING;
        sm->timeout = timeout * 10000; /* 10ms units, store in us */
        sm->skip = skip;

        OS_EXIT_CRITICAL(sr);
    }

    g_ble_ll_sync_comp_ev = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
    if (!g_ble_ll_sync_comp_ev) {
        OS_ENTER_CRITICAL(sr);

        for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
            g_ble_ll_sync_sm[i].flags &= ~BLE_LL_SYNC_SM_FLAG_PENDING;
            sm->timeout = 0;
            sm->skip = 0;
        }

        OS_EXIT_CRITICAL(sr);

        return BLE_ERR_MEM_CAPACITY;
    }

    g_ble_ll_sync_pending = BLE_LL_SYNC_ESTABLISH_CNT;

    return BLE_ERR_SUCCESS;
}

static void
ble_ll_sync_cancel_complete_event(void)
{
    ble_ll_sync_est_event_failed(BLE_ERR_OPERATION_CANCELLED);
}

int
ble_ll_sync_cancel(ble_ll_hci_post_cmd_complete_cb *post_cmd_cb)
{
    struct ble_ll_sync_sm *sm;
    os_sr_t sr;
    int i;

    if (!g_ble_ll_sync_pending) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    OS_ENTER_CRITICAL(sr);

    for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
        sm = &g_ble_ll_sync_sm[i];

        sm->flags &= ~BLE_LL_SYNC_SM_FLAG_PENDING;

        if (sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHING) {
            sm->flags &= ~BLE_LL_SYNC_SM_FLAG_ESTABLISHING;
            ble_ll_sched_rmv_elem(&sm->sch);
            ble_npl_eventq_remove(&g_ble_ll_data.ll_evq, &sm->sync_ev_end);
        }
    }

    OS_EXIT_CRITICAL(sr);

    g_ble_ll_sync_pending = 0;
    g_ble_ll_sync_sm_establishing = NULL;

    *post_cmd_cb = ble_ll_sync_cancel_complete_event;

    return BLE_ERR_SUCCESS;
}

int
ble_ll_sync_terminate(uint8_t *cmdbuf)
{
    struct ble_ll_sync_sm *sm;
    uint16_t handle;
    os_sr_t sr;

    if (g_ble_ll_sync_pending) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    handle  = get_le16(cmdbuf);
    if (handle > 0xeff) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    if (handle >= BLE_LL_SYNC_CNT) {
        return BLE_ERR_UNK_ADV_INDENT;
    }

    sm = &g_ble_ll_sync_sm[handle];

    OS_ENTER_CRITICAL(sr);

    if (!(sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHED)) {
        OS_EXIT_CRITICAL(sr);
        return BLE_ERR_UNK_ADV_INDENT;
    }

    ble_ll_sync_sm_clear(sm);

    OS_EXIT_CRITICAL(sr);

    return BLE_ERR_SUCCESS;
}

int
ble_ll_sync_list_add(uint8_t *cmdbuf)
{
    struct ble_ll_sync_sm *sm;
    uint8_t addr_type;
    uint8_t *addr;
    uint8_t sid;
    os_sr_t sr;

    if (g_ble_ll_sync_pending) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    addr_type = cmdbuf[0];
    if (addr_type > BLE_HCI_ADV_PEER_ADDR_MAX) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    addr = &cmdbuf[1];

    sid = cmdbuf[7];
    if (sid > 0x0f) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    OS_ENTER_CRITICAL(sr);

    sm = ble_ll_sync_get(addr, addr_type, sid);
    if (!sm) {
        OS_EXIT_CRITICAL(sr);
        return BLE_ERR_MEM_CAPACITY;
    }

    sm->flags |= BLE_LL_SYNC_SM_FLAG_ON_LIST;

    OS_EXIT_CRITICAL(sr);

    return BLE_ERR_SUCCESS;
}

int
ble_ll_sync_list_remove(uint8_t *cmdbuf)
{
    struct ble_ll_sync_sm *sm;
    uint8_t addr_type;
    uint8_t *addr;
    uint8_t sid;
    os_sr_t sr;

    if (g_ble_ll_sync_pending) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    addr_type = cmdbuf[0];
    if (addr_type > BLE_HCI_ADV_PEER_ADDR_MAX) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    addr = &cmdbuf[1];

    sid = cmdbuf[7];
    if (sid > 0x0f) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    OS_ENTER_CRITICAL(sr);

    sm = ble_ll_sync_find(addr, addr_type, sid);
    if (!sm) {
        OS_EXIT_CRITICAL(sr);
        return BLE_ERR_UNK_ADV_INDENT;
    }

    /* if sync is established only mark entry as removed from list */
    if (sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHED) {
        sm->flags &= ~BLE_LL_SYNC_SM_FLAG_ON_LIST;
    } else {
        ble_ll_sync_sm_clear(sm);
    }

    OS_EXIT_CRITICAL(sr);

    return BLE_ERR_SUCCESS;
}

int
ble_ll_sync_list_clear(void)
{
    struct ble_ll_sync_sm *sm;
    os_sr_t sr;
    int i;

    if (g_ble_ll_sync_pending) {
        return BLE_ERR_CMD_DISALLOWED;
    }

    OS_ENTER_CRITICAL(sr);

    for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
        sm = &g_ble_ll_sync_sm[i];

        /* if sync is establish only mark entry as removed from list */
        if (sm->flags & BLE_LL_SYNC_SM_FLAG_ESTABLISHED) {
            sm->flags &= ~BLE_LL_SYNC_SM_FLAG_ON_LIST;
            continue;
        }

        ble_ll_sync_sm_clear(sm);
    }

    OS_EXIT_CRITICAL(sr);

    return BLE_ERR_SUCCESS;
}

int
ble_ll_sync_list_size(uint8_t *rspbuf, uint8_t *rsplen)
{
    os_sr_t sr;
    int i;

    rspbuf[0] = 0;

    OS_ENTER_CRITICAL(sr);

    for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
        /* only established syncs 'consume' state machine entry */
        if (g_ble_ll_sync_sm[i].flags & (BLE_LL_SYNC_SM_FLAG_ESTABLISHING |
                                         BLE_LL_SYNC_SM_FLAG_ESTABLISHED)) {
            continue;
        }

        rspbuf[0]++;
    }

    OS_EXIT_CRITICAL(sr);

    *rsplen = 1;
    return BLE_ERR_SUCCESS;
}

void
ble_ll_sync_reset(void)
{
    int i;

    for (i = 0; i < BLE_LL_SYNC_CNT; i++) {
        ble_ll_sync_sm_clear(&g_ble_ll_sync_sm[i]);
    }

    g_ble_ll_sync_pending = 0;

    g_ble_ll_sync_sm_establishing = NULL;
    g_ble_ll_sync_sm_current = NULL;

    if (g_ble_ll_sync_comp_ev) {
        ble_hci_trans_buf_free(g_ble_ll_sync_comp_ev);
        g_ble_ll_sync_comp_ev = NULL;
    }
}
#endif
