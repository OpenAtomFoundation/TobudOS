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

#include <string.h>
#include <errno.h>
#include "sysinit/sysinit.h"
#include "stats/stats.h"
#include "testutil/testutil.h"
#include "nimble/ble.h"
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"
#include "host/ble_hs_adv.h"
#include "host/ble_hs_id.h"
#include "store/config/ble_store_config.h"
#include "transport/ram/ble_hci_ram.h"
#include "ble_hs_test_util.h"

/* Our global device address. */
uint8_t g_dev_addr[BLE_DEV_ADDR_LEN];

static STAILQ_HEAD(, os_mbuf_pkthdr) ble_hs_test_util_prev_tx_queue;
struct os_mbuf *ble_hs_test_util_prev_tx_cur;

int ble_sm_test_store_obj_type;
union ble_store_key ble_sm_test_store_key;
union ble_store_value ble_sm_test_store_value;

const struct ble_gap_adv_params ble_hs_test_util_adv_params = {
    .conn_mode = BLE_GAP_CONN_MODE_UND,
    .disc_mode = BLE_GAP_DISC_MODE_GEN,

    .itvl_min = 0,
    .itvl_max = 0,
    .channel_map = 0,
    .filter_policy = 0,
    .high_duty_cycle = 0,
};

void
ble_hs_test_util_prev_tx_enqueue(struct os_mbuf *om)
{
    struct os_mbuf_pkthdr *omp;

    assert(OS_MBUF_IS_PKTHDR(om));

    omp = OS_MBUF_PKTHDR(om);
    if (STAILQ_EMPTY(&ble_hs_test_util_prev_tx_queue)) {
        STAILQ_INSERT_HEAD(&ble_hs_test_util_prev_tx_queue, omp, omp_next);
    } else {
        STAILQ_INSERT_TAIL(&ble_hs_test_util_prev_tx_queue, omp, omp_next);
    }
}

static struct os_mbuf *
ble_hs_test_util_prev_tx_dequeue_once(struct hci_data_hdr *out_hci_hdr)
{
    struct os_mbuf_pkthdr *omp;
    struct os_mbuf *om;
    int rc;

    omp = STAILQ_FIRST(&ble_hs_test_util_prev_tx_queue);
    if (omp == NULL) {
        return NULL;
    }
    STAILQ_REMOVE_HEAD(&ble_hs_test_util_prev_tx_queue, omp_next);

    om = OS_MBUF_PKTHDR_TO_MBUF(omp);

    rc = ble_hs_hci_util_data_hdr_strip(om, out_hci_hdr);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT_FATAL(out_hci_hdr->hdh_len == OS_MBUF_PKTLEN(om));

    return om;
}

struct os_mbuf *
ble_hs_test_util_prev_tx_dequeue(void)
{
    struct ble_l2cap_hdr l2cap_hdr;
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    uint8_t pb;
    int rc;

    rc = os_mbuf_free_chain(ble_hs_test_util_prev_tx_cur);
    TEST_ASSERT_FATAL(rc == 0);

    om = ble_hs_test_util_prev_tx_dequeue_once(&hci_hdr);
    if (om != NULL) {
        pb = BLE_HCI_DATA_PB(hci_hdr.hdh_handle_pb_bc);
        TEST_ASSERT_FATAL(pb == BLE_HCI_PB_FIRST_NON_FLUSH);

        rc = ble_l2cap_parse_hdr(om, 0, &l2cap_hdr);
        TEST_ASSERT_FATAL(rc == 0);

        os_mbuf_adj(om, BLE_L2CAP_HDR_SZ);

        ble_hs_test_util_prev_tx_cur = om;
        while (OS_MBUF_PKTLEN(ble_hs_test_util_prev_tx_cur) <
               l2cap_hdr.len) {

            om = ble_hs_test_util_prev_tx_dequeue_once(&hci_hdr);
            TEST_ASSERT_FATAL(om != NULL);

            pb = BLE_HCI_DATA_PB(hci_hdr.hdh_handle_pb_bc);
            TEST_ASSERT_FATAL(pb == BLE_HCI_PB_MIDDLE);

            os_mbuf_concat(ble_hs_test_util_prev_tx_cur, om);
        }
    } else {
        ble_hs_test_util_prev_tx_cur = NULL;
    }

    return ble_hs_test_util_prev_tx_cur;
}

struct os_mbuf *
ble_hs_test_util_prev_tx_dequeue_pullup(void)
{
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue();
    if (om != NULL) {
        om = os_mbuf_pullup(om, OS_MBUF_PKTLEN(om));
        TEST_ASSERT_FATAL(om != NULL);
        ble_hs_test_util_prev_tx_cur = om;
    }

    return om;
}

int
ble_hs_test_util_prev_tx_queue_sz(void)
{
    struct os_mbuf_pkthdr *omp;
    int cnt;

    cnt = 0;
    STAILQ_FOREACH(omp, &ble_hs_test_util_prev_tx_queue, omp_next) {
        cnt++;
    }

    return cnt;
}

void
ble_hs_test_util_prev_tx_queue_clear(void)
{
    while (!STAILQ_EMPTY(&ble_hs_test_util_prev_tx_queue)) {
        ble_hs_test_util_prev_tx_dequeue();
    }
}

static void
ble_hs_test_util_conn_params_dflt(struct ble_gap_conn_params *conn_params)
{
    conn_params->scan_itvl = 0x0010;
    conn_params->scan_window = 0x0010;
    conn_params->itvl_min = BLE_GAP_INITIAL_CONN_ITVL_MIN;
    conn_params->itvl_max = BLE_GAP_INITIAL_CONN_ITVL_MAX;
    conn_params->latency = BLE_GAP_INITIAL_CONN_LATENCY;
    conn_params->supervision_timeout = BLE_GAP_INITIAL_SUPERVISION_TIMEOUT;
    conn_params->min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN;
    conn_params->max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN;
}

static void
ble_hs_test_util_hcc_from_conn_params(
    struct hci_create_conn *hcc, uint8_t own_addr_type,
    const ble_addr_t *peer_addr, const struct ble_gap_conn_params *conn_params)
{
    hcc->scan_itvl = conn_params->scan_itvl;
    hcc->scan_window = conn_params->scan_window;

    if (peer_addr == NULL) {
        hcc->filter_policy = BLE_HCI_CONN_FILT_USE_WL;
        hcc->peer_addr_type = 0;
        memset(hcc->peer_addr, 0, 6);
    } else {
        hcc->filter_policy = BLE_HCI_CONN_FILT_NO_WL;
        hcc->peer_addr_type = peer_addr->type;
        memcpy(hcc->peer_addr, peer_addr->val, 6);
    }
    hcc->own_addr_type = own_addr_type;
    hcc->conn_itvl_min = conn_params->itvl_min;
    hcc->conn_itvl_max = conn_params->itvl_max;
    hcc->conn_latency = conn_params->latency;
    hcc->supervision_timeout = conn_params->supervision_timeout;
    hcc->min_ce_len = conn_params->min_ce_len;
    hcc->max_ce_len = conn_params->max_ce_len;
}

void
ble_hs_test_util_create_rpa_conn(uint16_t handle, uint8_t own_addr_type,
                                 const uint8_t *our_rpa,
                                 uint8_t peer_addr_type,
                                 const uint8_t *peer_id_addr,
                                 const uint8_t *peer_rpa,
                                 uint8_t conn_features,
                                 ble_gap_event_fn *cb, void *cb_arg)
{
    ble_addr_t addr;
    struct hci_le_conn_complete evt;
    struct hci_le_rd_rem_supp_feat_complete evt2;
    int rc;

    addr.type = peer_addr_type;
    memcpy(addr.val, peer_id_addr, 6);

    rc = ble_hs_test_util_connect(own_addr_type, &addr, 0, NULL, cb, cb_arg,
                                  0);
    TEST_ASSERT_FATAL(rc == 0);

    /* ble_gap_rx_conn_complete() will send extra HCI command, need phony ack */
    ble_hs_test_util_hci_ack_set(ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                             BLE_HCI_OCF_LE_RD_REM_FEAT), 0);

    memset(&evt, 0, sizeof evt);
    evt.subevent_code = BLE_HCI_LE_SUBEV_CONN_COMPLETE;
    evt.status = BLE_ERR_SUCCESS;
    evt.connection_handle = handle;
    evt.role = BLE_HCI_LE_CONN_COMPLETE_ROLE_MASTER;
    evt.peer_addr_type = peer_addr_type;
    memcpy(evt.peer_addr, peer_id_addr, 6);
    evt.conn_itvl = BLE_GAP_INITIAL_CONN_ITVL_MAX;
    evt.conn_latency = BLE_GAP_INITIAL_CONN_LATENCY;
    evt.supervision_timeout = BLE_GAP_INITIAL_SUPERVISION_TIMEOUT;
    memcpy(evt.local_rpa, our_rpa, 6);
    memcpy(evt.peer_rpa, peer_rpa, 6);

    rc = ble_gap_rx_conn_complete(&evt, 0);
    TEST_ASSERT(rc == 0);

    evt2.subevent_code = BLE_HCI_LE_SUBEV_RD_REM_USED_FEAT;
    evt2.status = BLE_ERR_SUCCESS;
    evt2.connection_handle = handle;
    memcpy(evt2.features, ((uint8_t[]){ conn_features, 0, 0, 0, 0, 0, 0, 0 }),
           8);

    ble_gap_rx_rd_rem_sup_feat_complete(&evt2);

    ble_hs_test_util_hci_out_clear();
}

void
ble_hs_test_util_create_conn(uint16_t handle, const uint8_t *peer_id_addr,
                             ble_gap_event_fn *cb, void *cb_arg)
{
    static uint8_t null_addr[6];

    ble_hs_test_util_create_rpa_conn(handle, BLE_OWN_ADDR_PUBLIC, null_addr,
                                     BLE_ADDR_PUBLIC, peer_id_addr,
                                     null_addr, BLE_HS_TEST_CONN_FEAT_ALL,
                                     cb, cb_arg);
}

void
ble_hs_test_util_create_conn_feat(uint16_t handle, const uint8_t *peer_id_addr,
                                  uint8_t conn_features, ble_gap_event_fn *cb,
                                  void *cb_arg)
{
    static uint8_t null_addr[6];

    ble_hs_test_util_create_rpa_conn(handle, BLE_OWN_ADDR_PUBLIC, null_addr,
                                     BLE_ADDR_PUBLIC, peer_id_addr,
                                     null_addr, conn_features, cb, cb_arg);
}

int
ble_hs_test_util_connect(uint8_t own_addr_type, const ble_addr_t *peer_addr,
                         int32_t duration_ms,
                         const struct ble_gap_conn_params *params,
                         ble_gap_event_fn *cb, void *cb_arg,
                         uint8_t ack_status)
{
    struct ble_gap_conn_params dflt_params;
    struct hci_create_conn hcc;
    int rc;

    /* This function ensures the most recently sent HCI command is the expected
     * create connection command.  If the current test case has unverified HCI
     * commands, assume we are not interested in them and clear the queue.
     */
    ble_hs_test_util_hci_out_clear();

    ble_hs_test_util_hci_ack_set(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_CREATE_CONN),
        ack_status);

    rc = ble_gap_connect(own_addr_type, peer_addr, duration_ms, params, cb,
                         cb_arg);
    if (ack_status != 0) {
        TEST_ASSERT(rc == BLE_HS_HCI_ERR(ack_status));
    } else if (rc != 0) {
        return rc;
    }

    if (params == NULL) {
        ble_hs_test_util_conn_params_dflt(&dflt_params);
        params = &dflt_params;
    }

    ble_hs_test_util_hcc_from_conn_params(&hcc, own_addr_type, peer_addr,
                                          params);
    ble_hs_test_util_hci_verify_tx_create_conn(&hcc);

    return rc;
}

int
ble_hs_test_util_conn_cancel(uint8_t ack_status)
{
    int rc;

    ble_hs_test_util_hci_ack_set(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_CREATE_CONN_CANCEL),
        ack_status);

    rc = ble_gap_conn_cancel();
    return rc;
}

void
ble_hs_test_util_rx_conn_cancel_evt(void)
{
    ble_hs_test_util_conn_cancel(0);
    ble_hs_test_util_hci_rx_conn_cancel_evt();
}

void
ble_hs_test_util_conn_cancel_full(void)
{
    ble_hs_test_util_conn_cancel(0);
    ble_hs_test_util_rx_conn_cancel_evt();
}

int
ble_hs_test_util_conn_terminate(uint16_t conn_handle, uint8_t hci_status)
{
    int rc;

    ble_hs_test_util_hci_ack_set_disconnect(hci_status);
    rc = ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    return rc;
}

void
ble_hs_test_util_conn_disconnect(uint16_t conn_handle)
{
    struct hci_disconn_complete evt;
    int rc;

    rc = ble_hs_test_util_conn_terminate(conn_handle, 0);
    TEST_ASSERT_FATAL(rc == 0);

    /* Receive disconnection complete event. */
    evt.connection_handle = conn_handle;
    evt.status = 0;
    evt.reason = BLE_ERR_CONN_TERM_LOCAL;
    ble_hs_test_util_hci_rx_disconn_complete_event(&evt);
}

int
ble_hs_test_util_disc(uint8_t own_addr_type, int32_t duration_ms,
                      const struct ble_gap_disc_params *disc_params,
                      ble_gap_event_fn *cb, void *cb_arg, int fail_idx,
                      uint8_t fail_status)
{
    int rc;

    ble_hs_test_util_hci_ack_set_disc(own_addr_type, fail_idx, fail_status);
    rc = ble_gap_disc(own_addr_type, duration_ms, disc_params,
                      cb, cb_arg);
    return rc;
}

int
ble_hs_test_util_disc_cancel(uint8_t ack_status)
{
    int rc;

    ble_hs_test_util_hci_ack_set(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_SET_SCAN_ENABLE),
        ack_status);

    rc = ble_gap_disc_cancel();
    return rc;
}

static void
ble_hs_test_util_verify_tx_rd_pwr(void)
{
    uint8_t param_len;

    ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                   BLE_HCI_OCF_LE_RD_ADV_CHAN_TXPWR,
                                   &param_len);
    TEST_ASSERT(param_len == 0);
}

int
ble_hs_test_util_adv_set_fields(const struct ble_hs_adv_fields *adv_fields,
                                int cmd_fail_idx, uint8_t hci_status)
{
    struct ble_hs_test_util_hci_ack acks[3];
    int auto_pwr;
    int rc;
    int i;

    auto_pwr = adv_fields->tx_pwr_lvl_is_present &&
               adv_fields->tx_pwr_lvl == BLE_HS_ADV_TX_PWR_LVL_AUTO;

    i = 0;
    if (auto_pwr) {
        acks[i] = (struct ble_hs_test_util_hci_ack) {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_RD_ADV_CHAN_TXPWR),
            ble_hs_test_util_hci_misc_exp_status(i, cmd_fail_idx, hci_status),
            {0},
            1,
        };
        i++;
    }

    acks[i] = (struct ble_hs_test_util_hci_ack) {
        BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_ADV_DATA),
        ble_hs_test_util_hci_misc_exp_status(i, cmd_fail_idx, hci_status),
    };
    i++;

    memset(acks + i, 0, sizeof acks[i]);
    ble_hs_test_util_hci_ack_set_seq(acks);

    rc = ble_gap_adv_set_fields(adv_fields);
    if (rc == 0 && auto_pwr) {
        /* Verify tx of set advertising params command. */
        ble_hs_test_util_verify_tx_rd_pwr();
    }

    return rc;
}

int
ble_hs_test_util_adv_rsp_set_fields(const struct ble_hs_adv_fields *adv_fields,
                                    int cmd_fail_idx, uint8_t hci_status)
{
    struct ble_hs_test_util_hci_ack acks[3];
    int auto_pwr;
    int rc;
    int i;

    auto_pwr = adv_fields->tx_pwr_lvl_is_present &&
               adv_fields->tx_pwr_lvl == BLE_HS_ADV_TX_PWR_LVL_AUTO;

    i = 0;
    if (auto_pwr) {
        acks[i] = (struct ble_hs_test_util_hci_ack) {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_RD_ADV_CHAN_TXPWR),
            ble_hs_test_util_hci_misc_exp_status(i, cmd_fail_idx, hci_status),
            {0},
            1,
        };
        i++;
    }

    acks[i] = (struct ble_hs_test_util_hci_ack) {
        BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_SCAN_RSP_DATA),
        ble_hs_test_util_hci_misc_exp_status(i, cmd_fail_idx, hci_status),
    };
    i++;

    memset(acks + i, 0, sizeof acks[i]);
    ble_hs_test_util_hci_ack_set_seq(acks);

    rc = ble_gap_adv_rsp_set_fields(adv_fields);
    if (rc == 0 && auto_pwr) {
        /* Verify tx of set advertising params command. */
        ble_hs_test_util_verify_tx_rd_pwr();
    }

    return rc;
}

int
ble_hs_test_util_adv_start(uint8_t own_addr_type, const ble_addr_t *peer_addr,
                           const struct ble_gap_adv_params *adv_params,
                           int32_t duration_ms,
                           ble_gap_event_fn *cb, void *cb_arg,
                           int fail_idx, uint8_t fail_status)
{
    struct ble_hs_test_util_hci_ack acks[6];
    int rc;
    int i;

    i = 0;

    acks[i] = (struct ble_hs_test_util_hci_ack) {
        BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_ADV_PARAMS),
        fail_idx == i ? fail_status : 0,
    };
    i++;

    acks[i] = (struct ble_hs_test_util_hci_ack) {
        BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_ADV_ENABLE),
        ble_hs_test_util_hci_misc_exp_status(i, fail_idx, fail_status),
    };
    i++;

    memset(acks + i, 0, sizeof acks[i]);

    ble_hs_test_util_hci_ack_set_seq(acks);

    rc = ble_gap_adv_start(own_addr_type, peer_addr,
                           duration_ms, adv_params, cb, cb_arg);

    return rc;
}

int
ble_hs_test_util_adv_stop(uint8_t hci_status)
{
    int rc;

    ble_hs_test_util_hci_ack_set(
        BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_ADV_ENABLE),
        hci_status);

    rc = ble_gap_adv_stop();
    return rc;
}

int
ble_hs_test_util_wl_set(ble_addr_t *addrs, uint8_t addrs_count,
                        int fail_idx, uint8_t fail_status)
{
    struct ble_hs_test_util_hci_ack acks[64];
    int cmd_idx;
    int rc;
    int i;

    TEST_ASSERT_FATAL(addrs_count < 63);

    cmd_idx = 0;
    acks[cmd_idx] = (struct ble_hs_test_util_hci_ack) {
        BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_CLEAR_WHITE_LIST),
        ble_hs_test_util_hci_misc_exp_status(cmd_idx, fail_idx, fail_status),
    };
    cmd_idx++;

    for (i = 0; i < addrs_count; i++) {
        acks[cmd_idx] = (struct ble_hs_test_util_hci_ack) {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_ADD_WHITE_LIST),
            ble_hs_test_util_hci_misc_exp_status(cmd_idx, fail_idx, fail_status),
        };

        cmd_idx++;
    }
    memset(acks + cmd_idx, 0, sizeof acks[cmd_idx]);

    ble_hs_test_util_hci_ack_set_seq(acks);
    rc = ble_gap_wl_set(addrs, addrs_count);
    return rc;
}

int
ble_hs_test_util_conn_update(uint16_t conn_handle,
                             struct ble_gap_upd_params *params,
                             uint8_t hci_status)
{
    int rc;

    /*
     * 0xFF is magic value used for cases where we expect update over L2CAP to
     * be triggered - in this case we don't need phony ack.
     */
    if (hci_status != 0xFF) {
        ble_hs_test_util_hci_ack_set(
                BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_CONN_UPDATE),
                hci_status);
    }

    rc = ble_gap_update_params(conn_handle, params);
    return rc;
}

int
ble_hs_test_util_set_our_irk(const uint8_t *irk, int fail_idx,
                             uint8_t hci_status)
{
    int rc;

    ble_hs_test_util_hci_ack_set_seq(((struct ble_hs_test_util_hci_ack[]) {
        {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_ADDR_RES_EN),
            ble_hs_test_util_hci_misc_exp_status(0, fail_idx, hci_status),
        },
        {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_CLR_RESOLV_LIST),
            ble_hs_test_util_hci_misc_exp_status(1, fail_idx, hci_status),
        },
        {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_ADDR_RES_EN),
            ble_hs_test_util_hci_misc_exp_status(2, fail_idx, hci_status),
        },
        {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_ADV_ENABLE),
            ble_hs_test_util_hci_misc_exp_status(3, fail_idx, hci_status),
        },
        {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_ADD_RESOLV_LIST),
            ble_hs_test_util_hci_misc_exp_status(4, fail_idx, hci_status),
        },
        {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_PRIVACY_MODE),
            ble_hs_test_util_hci_misc_exp_status(5, fail_idx, hci_status),
        },
        {
            BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_PRIVACY_MODE),
            ble_hs_test_util_hci_misc_exp_status(6, fail_idx, hci_status),
        },
        {
            0
        }
    }));

    rc = ble_hs_pvcy_set_our_irk(irk);
    return rc;
}

int
ble_hs_test_util_security_initiate(uint16_t conn_handle, uint8_t hci_status)
{
    int rc;

    ble_hs_test_util_hci_ack_set(
        BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_START_ENCRYPT), hci_status);

    rc = ble_gap_security_initiate(conn_handle);
    return rc;
}

int
ble_hs_test_util_l2cap_rx_first_frag(uint16_t conn_handle, uint16_t cid,
                                     struct hci_data_hdr *hci_hdr,
                                     struct os_mbuf *om)
{
    int rc;

    om = ble_l2cap_prepend_hdr(om, cid, OS_MBUF_PKTLEN(om));
    TEST_ASSERT_FATAL(om != NULL);

    rc = ble_hs_test_util_l2cap_rx(conn_handle, hci_hdr, om);
    return rc;
}

int
ble_hs_test_util_l2cap_rx(uint16_t conn_handle,
                          struct hci_data_hdr *hci_hdr,
                          struct os_mbuf *om)
{
    struct ble_hs_conn *conn;
    ble_l2cap_rx_fn *rx_cb;
    int reject_cid;
    int rc;

    ble_hs_lock();

    conn = ble_hs_conn_find(conn_handle);
    if (conn != NULL) {
        rc = ble_l2cap_rx(conn, hci_hdr, om, &rx_cb, &reject_cid);
    } else {
        rc = os_mbuf_free_chain(om);
        TEST_ASSERT_FATAL(rc == 0);
    }

    ble_hs_unlock();

    if (conn == NULL) {
        rc = BLE_HS_ENOTCONN;
    } else if (rc == 0) {
        TEST_ASSERT_FATAL(rx_cb != NULL);
        rc = rx_cb(conn->bhc_rx_chan);
        ble_l2cap_remove_rx(conn, conn->bhc_rx_chan);
    } else if (rc == BLE_HS_EAGAIN) {
        /* More fragments on the way. */
        rc = 0;
    } else {
        if (reject_cid != -1) {
            ble_l2cap_sig_reject_invalid_cid_tx(conn_handle, 0, 0, reject_cid);
        }
    }

    return rc;
}

int
ble_hs_test_util_l2cap_rx_payload_flat(uint16_t conn_handle, uint16_t cid,
                                       const void *data, int len)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    int rc;

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    rc = os_mbuf_append(om, data, len);
    TEST_ASSERT_FATAL(rc == 0);

    hci_hdr.hdh_handle_pb_bc =
        ble_hs_hci_util_handle_pb_bc_join(conn_handle,
                                          BLE_HCI_PB_FIRST_FLUSH, 0);
    hci_hdr.hdh_len = OS_MBUF_PKTHDR(om)->omp_len;

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, cid, &hci_hdr, om);
    return rc;
}

void
ble_hs_test_util_set_att_mtu(uint16_t conn_handle, uint16_t mtu)
{
    struct ble_l2cap_chan *chan;
    struct ble_hs_conn *conn;
    int rc;

    if (mtu <= BLE_ATT_MTU_DFLT) {
        return;
    }

    ble_hs_lock();

    rc = ble_att_conn_chan_find(conn_handle, &conn, &chan);
    assert(rc == 0);
    chan->my_mtu = mtu;
    chan->peer_mtu = mtu;
    chan->flags |= BLE_L2CAP_CHAN_F_TXED_MTU;

    ble_hs_unlock();
}

int
ble_hs_test_util_rx_att_mtu_cmd(uint16_t conn_handle, int is_req, uint16_t mtu)
{
    struct ble_att_mtu_cmd cmd;
    uint8_t buf[BLE_ATT_MTU_CMD_SZ];
    int rc;

    cmd.bamc_mtu = mtu;

    if (is_req) {
        ble_att_mtu_req_write(buf, sizeof buf, &cmd);
    } else {
        ble_att_mtu_rsp_write(buf, sizeof buf, &cmd);
    }

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    return rc;
}

int
ble_hs_test_util_rx_att_find_info_req(uint16_t conn_handle,
                                      uint16_t start_handle,
                                      uint16_t end_handle)
{
    struct ble_att_find_info_req req;
    uint8_t buf[BLE_ATT_FIND_INFO_REQ_SZ];
    int rc;

    req.bafq_start_handle = start_handle;
    req.bafq_end_handle = end_handle;

    ble_att_find_info_req_write(buf, sizeof buf, &req);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);

    return rc;
}

int
ble_hs_test_util_rx_att_find_type_value_req(uint16_t conn_handle,
                                            uint16_t start_handle,
                                            uint16_t end_handle,
                                            uint16_t attr_type,
                                            const void *attr_val,
                                            uint16_t attr_len)
{
    struct ble_att_find_type_value_req req;
    uint8_t buf[BLE_ATT_FIND_TYPE_VALUE_REQ_BASE_SZ + 16];
    int rc;

    TEST_ASSERT(attr_len <= 16);

    req.bavq_start_handle = start_handle;
    req.bavq_end_handle = end_handle;
    req.bavq_attr_type = attr_type;

    ble_att_find_type_value_req_write(buf, sizeof buf, &req);
    memcpy(buf + BLE_ATT_FIND_TYPE_VALUE_REQ_BASE_SZ, attr_val, attr_len);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_FIND_TYPE_VALUE_REQ_BASE_SZ + attr_len);

    return rc;
}

int
ble_hs_test_util_rx_att_read_type_req(uint16_t conn_handle,
                                      uint16_t start_handle,
                                      uint16_t end_handle,
                                      const ble_uuid_t *uuid)
{
    struct ble_att_read_type_req req;
    uint8_t buf[BLE_ATT_READ_TYPE_REQ_SZ_128];
    int req_len;
    int rc;

    req.batq_start_handle = start_handle;
    req.batq_end_handle = end_handle;

    ble_att_read_type_req_write(buf, sizeof buf, &req);

    ble_uuid_flat(uuid, buf + BLE_ATT_READ_TYPE_REQ_BASE_SZ);
    req_len = BLE_ATT_READ_TYPE_REQ_BASE_SZ + ble_uuid_length(uuid);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, req_len);
    return rc;
}

int
ble_hs_test_util_rx_att_read_type_req16(uint16_t conn_handle,
                                        uint16_t start_handle,
                                        uint16_t end_handle,
                                        uint16_t uuid16)
{
    int rc;

    rc = ble_hs_test_util_rx_att_read_type_req(conn_handle, start_handle,
                                               end_handle,
                                               BLE_UUID16_DECLARE(uuid16));
    return rc;
}

int
ble_hs_test_util_rx_att_read_req(uint16_t conn_handle, uint16_t attr_handle)
{
    struct ble_att_read_req req;
    uint8_t buf[BLE_ATT_READ_REQ_SZ];
    int rc;

    req.barq_handle = attr_handle;
    ble_att_read_req_write(buf, sizeof buf, &req);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    return rc;
}

int
ble_hs_test_util_rx_att_read_blob_req(uint16_t conn_handle,
                                      uint16_t attr_handle,
                                      uint16_t offset)
{
    struct ble_att_read_blob_req req;
    uint8_t buf[BLE_ATT_READ_BLOB_REQ_SZ];
    int rc;

    req.babq_handle = attr_handle;
    req.babq_offset = offset;
    ble_att_read_blob_req_write(buf, sizeof buf, &req);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    return rc;
}

int
ble_hs_test_util_rx_att_read_mult_req(uint16_t conn_handle,
                                      const uint16_t *handles,
                                      int num_handles)
{
    uint8_t buf[256];
    int off;
    int rc;
    int i;

    ble_att_read_mult_req_write(buf, sizeof buf);

    off = BLE_ATT_READ_MULT_REQ_BASE_SZ;
    for (i = 0; i < num_handles; i++) {
        put_le16(buf + off, handles[i]);
        off += 2;
    }

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, off);
    return rc;
}

int
ble_hs_test_util_rx_att_read_group_type_req(uint16_t conn_handle,
                                            uint16_t start_handle,
                                            uint16_t end_handle,
                                            const ble_uuid_t *uuid)
{
    struct ble_att_read_group_type_req req;
    uint8_t buf[BLE_ATT_READ_GROUP_TYPE_REQ_SZ_128];
    int req_len;
    int rc;

    req.bagq_start_handle = start_handle;
    req.bagq_end_handle = end_handle;

    ble_uuid_flat(uuid, buf + BLE_ATT_READ_TYPE_REQ_BASE_SZ);
    req_len = BLE_ATT_READ_GROUP_TYPE_REQ_BASE_SZ + ble_uuid_length(uuid);

    ble_att_read_group_type_req_write(buf, sizeof buf, &req);
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, req_len);
    return rc;
}

int
ble_hs_test_util_rx_att_read_group_type_req16(uint16_t conn_handle,
                                              uint16_t start_handle,
                                              uint16_t end_handle,
                                              uint16_t uuid16)
{
    int rc;

    rc = ble_hs_test_util_rx_att_read_group_type_req(conn_handle, start_handle,
                                                     end_handle,
                                                     BLE_UUID16_DECLARE(uuid16));
    return rc;
}

int
ble_hs_test_util_rx_att_write_req(uint16_t conn_handle, uint16_t attr_handle,
                                  const void *attr_val, uint16_t attr_len)
{
    struct ble_att_write_req req;
    uint8_t buf[BLE_ATT_WRITE_REQ_BASE_SZ + BLE_ATT_ATTR_MAX_LEN];
    int rc;

    req.bawq_handle = attr_handle;
    ble_att_write_req_write(buf, sizeof buf, &req);

    memcpy(buf + BLE_ATT_WRITE_REQ_BASE_SZ, attr_val, attr_len);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_WRITE_REQ_BASE_SZ + attr_len);

    return rc;
}

int
ble_hs_test_util_rx_att_write_cmd(uint16_t conn_handle, uint16_t attr_handle,
                                  const void *attr_val, uint16_t attr_len)
{
    struct ble_att_write_req req;
    uint8_t buf[BLE_ATT_WRITE_REQ_BASE_SZ + BLE_ATT_ATTR_MAX_LEN];
    int rc;

    req.bawq_handle = attr_handle;
    ble_att_write_cmd_write(buf, sizeof buf, &req);

    memcpy(buf + BLE_ATT_WRITE_REQ_BASE_SZ, attr_val, attr_len);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_WRITE_REQ_BASE_SZ + attr_len);

    return rc;
}

int
ble_hs_test_util_rx_att_prep_write_req(uint16_t conn_handle,
                                       uint16_t attr_handle,
                                       uint16_t offset,
                                       const void *attr_val,
                                       uint16_t attr_len)
{
    struct ble_att_prep_write_cmd prep_req;
    uint8_t buf[BLE_ATT_PREP_WRITE_CMD_BASE_SZ + BLE_ATT_ATTR_MAX_LEN];
    int rc;

    prep_req.bapc_handle = attr_handle;
    prep_req.bapc_offset = offset;
    ble_att_prep_write_req_write(buf, sizeof buf, &prep_req);
    memcpy(buf + BLE_ATT_PREP_WRITE_CMD_BASE_SZ, attr_val, attr_len);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_PREP_WRITE_CMD_BASE_SZ + attr_len);

    return rc;
}

int
ble_hs_test_util_rx_att_exec_write_req(uint16_t conn_handle, uint8_t flags)
{
    struct ble_att_exec_write_req exec_req;
    uint8_t buf[BLE_ATT_EXEC_WRITE_REQ_SZ];
    int rc;

    exec_req.baeq_flags = flags;
    ble_att_exec_write_req_write(buf, sizeof buf, &exec_req);
    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf,
                                                BLE_ATT_EXEC_WRITE_REQ_SZ);
    return rc;
}

int
ble_hs_test_util_rx_att_notify_req(uint16_t conn_handle,
                                   uint16_t attr_handle,
                                   void *attr_val,
                                   uint16_t attr_len)
{
    struct ble_att_notify_req req;
    uint8_t buf[BLE_ATT_NOTIFY_REQ_BASE_SZ + BLE_ATT_ATTR_MAX_LEN];
    int rc;

    req.banq_handle = attr_handle;
    ble_att_notify_req_write(buf, sizeof buf, &req);
    memcpy(buf + BLE_ATT_NOTIFY_REQ_BASE_SZ, attr_val, attr_len);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_NOTIFY_REQ_BASE_SZ + attr_len);

    return rc;
}

int
ble_hs_test_util_rx_att_indicate_req(uint16_t conn_handle,
                                     uint16_t attr_handle,
                                     void *attr_val,
                                     uint16_t attr_len)
{
    struct ble_att_indicate_req req;
    uint8_t buf[BLE_ATT_INDICATE_REQ_BASE_SZ + BLE_ATT_ATTR_MAX_LEN];
    int rc;

    req.baiq_handle = attr_handle;
    ble_att_indicate_req_write(buf, sizeof buf, &req);
    memcpy(buf + BLE_ATT_INDICATE_REQ_BASE_SZ, attr_val, attr_len);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(
        conn_handle, BLE_L2CAP_CID_ATT, buf,
        BLE_ATT_INDICATE_REQ_BASE_SZ + attr_len);

    return rc;
}

void
ble_hs_test_util_rx_att_err_rsp(uint16_t conn_handle, uint8_t req_op,
                                uint8_t error_code, uint16_t err_handle)
{
    struct ble_att_error_rsp rsp;
    uint8_t buf[BLE_ATT_ERROR_RSP_SZ];
    int rc;

    rsp.baep_req_op = req_op;
    rsp.baep_handle = err_handle;
    rsp.baep_error_code = error_code;

    ble_att_error_rsp_write(buf, sizeof buf, &rsp);

    rc = ble_hs_test_util_l2cap_rx_payload_flat(conn_handle, BLE_L2CAP_CID_ATT,
                                                buf, sizeof buf);
    TEST_ASSERT(rc == 0);
}

void
ble_hs_test_util_verify_tx_prep_write(uint16_t attr_handle, uint16_t offset,
                                      const void *data, int data_len)
{
    struct ble_att_prep_write_cmd req;
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue();
    TEST_ASSERT_FATAL(om != NULL);
    TEST_ASSERT(OS_MBUF_PKTLEN(om) ==
                BLE_ATT_PREP_WRITE_CMD_BASE_SZ + data_len);

    om = os_mbuf_pullup(om, BLE_ATT_PREP_WRITE_CMD_BASE_SZ);
    TEST_ASSERT_FATAL(om != NULL);

    ble_att_prep_write_req_parse(om->om_data, om->om_len, &req);
    TEST_ASSERT(req.bapc_handle == attr_handle);
    TEST_ASSERT(req.bapc_offset == offset);
    TEST_ASSERT(os_mbuf_cmpf(om, BLE_ATT_PREP_WRITE_CMD_BASE_SZ,
                             data, data_len) == 0);
}

void
ble_hs_test_util_verify_tx_exec_write(uint8_t expected_flags)
{
    struct ble_att_exec_write_req req;
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);
    TEST_ASSERT(om->om_len == BLE_ATT_EXEC_WRITE_REQ_SZ);

    ble_att_exec_write_req_parse(om->om_data, om->om_len, &req);
    TEST_ASSERT(req.baeq_flags == expected_flags);
}

void
ble_hs_test_util_verify_tx_find_type_value(uint16_t start_handle,
                                           uint16_t end_handle,
                                           uint16_t attr_type,
                                           const void *value,
                                           uint16_t value_len)
{
    struct ble_att_find_type_value_req req;
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);
    TEST_ASSERT(om->om_len == BLE_ATT_FIND_TYPE_VALUE_REQ_BASE_SZ + value_len);

    ble_att_find_type_value_req_parse(om->om_data, om->om_len, &req);
    TEST_ASSERT(req.bavq_start_handle == start_handle);
    TEST_ASSERT(req.bavq_end_handle == end_handle);
    TEST_ASSERT(req.bavq_attr_type == attr_type);
    TEST_ASSERT(memcmp(om->om_data + BLE_ATT_FIND_TYPE_VALUE_REQ_BASE_SZ,
                       value,
                       value_len) == 0);
}

void
ble_hs_test_util_verify_tx_disc_svc_uuid(const ble_uuid_t *uuid)
{
    uint8_t uuid_buf[16];

    ble_uuid_flat(uuid, uuid_buf);
    ble_hs_test_util_verify_tx_find_type_value(
        1, 0xffff, BLE_ATT_UUID_PRIMARY_SERVICE,
        uuid_buf, ble_uuid_length(uuid));
}

void
ble_hs_test_util_verify_tx_read_rsp_gen(uint8_t att_op,
                                        uint8_t *attr_data, int attr_len)
{
    struct os_mbuf *om;
    uint8_t u8;
    int rc;
    int i;

    om = ble_hs_test_util_prev_tx_dequeue();

    rc = os_mbuf_copydata(om, 0, 1, &u8);
    TEST_ASSERT(rc == 0);
    TEST_ASSERT(u8 == att_op);

    for (i = 0; i < attr_len; i++) {
        rc = os_mbuf_copydata(om, i + 1, 1, &u8);
        TEST_ASSERT(rc == 0);
        TEST_ASSERT(u8 == attr_data[i]);
    }

    rc = os_mbuf_copydata(om, i + 1, 1, &u8);
    TEST_ASSERT(rc != 0);
}

void
ble_hs_test_util_verify_tx_read_rsp(uint8_t *attr_data, int attr_len)
{
    ble_hs_test_util_verify_tx_read_rsp_gen(BLE_ATT_OP_READ_RSP,
                                            attr_data, attr_len);
}

void
ble_hs_test_util_verify_tx_read_blob_rsp(uint8_t *attr_data, int attr_len)
{
    ble_hs_test_util_verify_tx_read_rsp_gen(BLE_ATT_OP_READ_BLOB_RSP,
                                            attr_data, attr_len);
}

void
ble_hs_test_util_verify_tx_write_rsp(void)
{
    struct os_mbuf *om;
    uint8_t u8;
    int rc;

    om = ble_hs_test_util_prev_tx_dequeue();

    rc = os_mbuf_copydata(om, 0, 1, &u8);
    TEST_ASSERT(rc == 0);
    TEST_ASSERT(u8 == BLE_ATT_OP_WRITE_RSP);
}

void
ble_hs_test_util_verify_tx_mtu_cmd(int is_req, uint16_t mtu)
{
    struct ble_att_mtu_cmd cmd;
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);

    if (is_req) {
        ble_att_mtu_req_parse(om->om_data, om->om_len, &cmd);
    } else {
        ble_att_mtu_rsp_parse(om->om_data, om->om_len, &cmd);
    }

    TEST_ASSERT(cmd.bamc_mtu == mtu);
}

void
ble_hs_test_util_verify_tx_find_info_rsp(
    struct ble_hs_test_util_att_info_entry *entries)
{
    struct ble_hs_test_util_att_info_entry *entry;
    struct ble_att_find_info_rsp rsp;
    struct os_mbuf *om;
    uint16_t handle;
    uint8_t buf[BLE_ATT_FIND_INFO_RSP_BASE_SZ];
    ble_uuid_any_t uuid;
    int off;
    int rc;

    off = 0;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om);

    rc = os_mbuf_copydata(om, off, sizeof buf, buf);
    TEST_ASSERT(rc == 0);
    off += sizeof buf;

    ble_att_find_info_rsp_parse(buf, sizeof buf, &rsp);

    for (entry = entries; entry->handle != 0; entry++) {
        rc = os_mbuf_copydata(om, off, 2, &handle);
        TEST_ASSERT(rc == 0);
        off += 2;

        handle = get_le16((void *)&handle);
        TEST_ASSERT(handle == entry->handle);

        if (entry->uuid->type == BLE_UUID_TYPE_16) {
            TEST_ASSERT(rsp.bafp_format ==
                        BLE_ATT_FIND_INFO_RSP_FORMAT_16BIT);

            ble_uuid_init_from_att_mbuf(&uuid, om, off, 2);
            TEST_ASSERT(rc == 0);
            off += 2;
        } else {
            TEST_ASSERT(rsp.bafp_format ==
                        BLE_ATT_FIND_INFO_RSP_FORMAT_128BIT);

            rc = ble_uuid_init_from_att_mbuf(&uuid, om, off, 16);
            TEST_ASSERT(rc == 0);
            off += 16;
        }

        TEST_ASSERT(ble_uuid_cmp(entry->uuid, &uuid.u) == 0);
    }

    /* Ensure there is no extra data in the response. */
    TEST_ASSERT(off == OS_MBUF_PKTHDR(om)->omp_len);
}

void
ble_hs_test_util_verify_tx_read_group_type_rsp(
    struct ble_hs_test_util_att_group_type_entry *entries)
{
    struct ble_hs_test_util_att_group_type_entry *entry;
    struct ble_att_read_group_type_rsp rsp;
    struct os_mbuf *om;
    uint16_t u16;
    ble_uuid_any_t uuid;
    int off;
    int rc;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om);

    ble_att_read_group_type_rsp_parse(om->om_data, om->om_len, &rsp);

    off = BLE_ATT_READ_GROUP_TYPE_RSP_BASE_SZ;
    for (entry = entries; entry->start_handle != 0; entry++) {
        if (entry->uuid->type == BLE_UUID_TYPE_16) {
            TEST_ASSERT(rsp.bagp_length ==
                        BLE_ATT_READ_GROUP_TYPE_ADATA_SZ_16);
        } else {
            TEST_ASSERT(rsp.bagp_length ==
                        BLE_ATT_READ_GROUP_TYPE_ADATA_SZ_128);
        }

        rc = os_mbuf_copydata(om, off, 2, &u16);
        TEST_ASSERT(rc == 0);
        put_le16(&u16, u16);
        TEST_ASSERT(u16 == entry->start_handle);
        off += 2;

        rc = os_mbuf_copydata(om, off, 2, &u16);
        TEST_ASSERT(rc == 0);
        put_le16(&u16, u16);
        TEST_ASSERT(u16 == entry->end_handle);
        off += 2;

        if (entry->uuid->type == BLE_UUID_TYPE_16) {
            rc = ble_uuid_init_from_att_mbuf(&uuid, om, off, 2);
            TEST_ASSERT(rc == 0);
        } else {
            rc = ble_uuid_init_from_att_mbuf(&uuid, om, off, 16);
            TEST_ASSERT(rc == 0);
        }

        TEST_ASSERT(ble_uuid_cmp(&uuid.u, entry->uuid) == 0);
        off += ble_uuid_length(&uuid.u);
    }

    /* Ensure there is no extra data in the response. */
    TEST_ASSERT(off == OS_MBUF_PKTLEN(om));
}

void
ble_hs_test_util_verify_tx_err_rsp(uint8_t req_op, uint16_t handle,
                                   uint8_t error_code)
{
    struct ble_att_error_rsp rsp;
    struct os_mbuf *om;
    uint8_t buf[BLE_ATT_ERROR_RSP_SZ];
    int rc;

    om = ble_hs_test_util_prev_tx_dequeue();

    rc = os_mbuf_copydata(om, 0, sizeof buf, buf);
    TEST_ASSERT(rc == 0);

    ble_att_error_rsp_parse(buf, sizeof buf, &rsp);

    TEST_ASSERT(rsp.baep_req_op == req_op);
    TEST_ASSERT(rsp.baep_handle == handle);
    TEST_ASSERT(rsp.baep_error_code == error_code);
}

void
ble_hs_test_util_verify_tx_write_cmd(uint16_t handle, const void *data,
                                     uint16_t data_len)
{
    struct ble_att_write_req req;
    struct os_mbuf *om;
    uint8_t buf[BLE_ATT_WRITE_CMD_BASE_SZ];
    int rc;

    om = ble_hs_test_util_prev_tx_dequeue();

    rc = os_mbuf_copydata(om, 0, sizeof buf, buf);
    TEST_ASSERT(rc == 0);

    ble_att_write_cmd_parse(buf, sizeof buf, &req);

    TEST_ASSERT(req.bawq_handle == handle);

    os_mbuf_adj(om, sizeof buf);
    TEST_ASSERT(OS_MBUF_PKTLEN(om) == data_len);
    TEST_ASSERT(os_mbuf_cmpf(om, 0, data, data_len) == 0);
}

static struct os_mbuf *
ble_hs_test_util_verify_tx_l2cap_sig_hdr(uint8_t op, uint8_t id,
                                   uint16_t payload_len,
                                   struct ble_l2cap_sig_hdr *out_hdr)
{
    struct ble_l2cap_sig_hdr hdr;
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue();
    TEST_ASSERT_FATAL(om != NULL);

    TEST_ASSERT(OS_MBUF_PKTLEN(om) == BLE_L2CAP_SIG_HDR_SZ + payload_len);
    ble_l2cap_sig_hdr_parse(om->om_data, om->om_len, &hdr);
    TEST_ASSERT(hdr.op == op);
    if (id != 0) {
        TEST_ASSERT(hdr.identifier == id);
    }
    TEST_ASSERT(hdr.length == payload_len);

    om->om_data += BLE_L2CAP_SIG_HDR_SZ;
    om->om_len -= BLE_L2CAP_SIG_HDR_SZ;

    if (out_hdr != NULL) {
        *out_hdr = hdr;
    }

    return om;
}

int
ble_hs_test_util_inject_rx_l2cap_sig(uint16_t conn_handle, uint8_t opcode,
                              uint8_t id, void *cmd, uint16_t cmd_size)
{
    void *r;
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    int rc;

    hci_hdr = BLE_HS_TEST_UTIL_L2CAP_HCI_HDR(2, BLE_HCI_PB_FIRST_FLUSH,
                         BLE_L2CAP_HDR_SZ + BLE_L2CAP_SIG_HDR_SZ + cmd_size);

    r = ble_l2cap_sig_cmd_get(opcode, id, cmd_size, &om);
    TEST_ASSERT_FATAL(r != NULL);

    memcpy(r, cmd, cmd_size);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SIG,
                                              &hci_hdr, om);
    return rc;
}

/**
 * @return  The L2CAP sig identifier in the request/response.
 */
uint8_t
ble_hs_test_util_verify_tx_l2cap_sig(uint16_t opcode, void *cmd,
                                     uint16_t cmd_size)
{
    struct ble_l2cap_sig_hdr hdr;
    struct os_mbuf *om;

    om = ble_hs_test_util_verify_tx_l2cap_sig_hdr(opcode, 0, cmd_size, &hdr);
    om = os_mbuf_pullup(om, cmd_size);

    /* Verify payload. */
    TEST_ASSERT(memcmp(om->om_data, cmd, cmd_size) == 0);

    return hdr.identifier;
}

void
ble_hs_test_util_verify_tx_l2cap(struct os_mbuf *txom)
{
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue();
    TEST_ASSERT_FATAL(om != NULL);

    /* TODO Handle fragmentation */
    TEST_ASSERT_FATAL(os_mbuf_cmpm(om, 0, txom, 0, OS_MBUF_PKTLEN(om)) == 0);
}

void
ble_hs_test_util_inject_rx_l2cap(uint16_t conn_handle, uint16_t cid,
                                 struct os_mbuf *rxom)
{
    struct hci_data_hdr hci_hdr;
    int rc;

    hci_hdr = BLE_HS_TEST_UTIL_L2CAP_HCI_HDR(2, BLE_HCI_PB_FIRST_FLUSH,
                                             BLE_L2CAP_HDR_SZ +
                                             BLE_L2CAP_SIG_HDR_SZ +
                                             OS_MBUF_PKTLEN(rxom));

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, cid, &hci_hdr, rxom);
    TEST_ASSERT(rc == 0);
}

static void
ble_l2cap_test_update_req_swap(struct ble_l2cap_sig_update_req *dst,
                               struct ble_l2cap_sig_update_req *src)
{
    dst->itvl_min = le16toh(src->itvl_min);
    dst->itvl_max = le16toh(src->itvl_max);
    dst->slave_latency = le16toh(src->slave_latency);
    dst->timeout_multiplier = le16toh(src->timeout_multiplier);
}

static void
ble_l2cap_test_update_req_parse(void *payload, int len,
                               struct ble_l2cap_sig_update_req *dst)
{
    BLE_HS_DBG_ASSERT(len >= BLE_L2CAP_SIG_UPDATE_REQ_SZ);
    ble_l2cap_test_update_req_swap(dst, payload);
}

/**
 * @return                      The L2CAP sig identifier in the request.
 */
uint8_t
ble_hs_test_util_verify_tx_l2cap_update_req(
    struct ble_l2cap_sig_update_params *params)
{
    struct ble_l2cap_sig_update_req req;
    struct ble_l2cap_sig_hdr hdr;
    struct os_mbuf *om;

    om = ble_hs_test_util_verify_tx_l2cap_sig_hdr(BLE_L2CAP_SIG_OP_UPDATE_REQ,
                                                  0,
                                                  BLE_L2CAP_SIG_UPDATE_REQ_SZ,
                                                  &hdr);

    /* Verify payload. */
    ble_l2cap_test_update_req_parse(om->om_data, om->om_len, &req);
    TEST_ASSERT(req.itvl_min == params->itvl_min);
    TEST_ASSERT(req.itvl_max == params->itvl_max);
    TEST_ASSERT(req.slave_latency == params->slave_latency);
    TEST_ASSERT(req.timeout_multiplier == params->timeout_multiplier);

    return hdr.identifier;
}

static void
ble_l2cap_sig_update_rsp_parse(void *payload, int len,
                               struct ble_l2cap_sig_update_rsp *dst)
{
    struct ble_l2cap_sig_update_rsp *src = payload;

    BLE_HS_DBG_ASSERT(len >= BLE_L2CAP_SIG_UPDATE_RSP_SZ);
    dst->result = le16toh(src->result);
}

int
ble_hs_test_util_rx_l2cap_update_rsp(uint16_t conn_handle,
                                     uint8_t id, uint16_t result)
{
    struct ble_l2cap_sig_update_rsp *rsp;
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    int rc;

    hci_hdr = BLE_HS_TEST_UTIL_L2CAP_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + BLE_L2CAP_SIG_HDR_SZ + BLE_L2CAP_SIG_UPDATE_RSP_SZ);

    rsp = ble_l2cap_sig_cmd_get(BLE_L2CAP_SIG_OP_UPDATE_RSP, id,
                                BLE_L2CAP_SIG_UPDATE_RSP_SZ, &om);
    TEST_ASSERT_FATAL(rsp != NULL);

    rsp->result = htole16(result);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SIG,
                                              &hci_hdr, om);
    return rc;
}

void
ble_hs_test_util_verify_tx_l2cap_update_rsp(uint8_t exp_id,
                                            uint16_t exp_result)
{
    struct ble_l2cap_sig_update_rsp rsp;
    struct os_mbuf *om;

    om = ble_hs_test_util_verify_tx_l2cap_sig_hdr(BLE_L2CAP_SIG_OP_UPDATE_RSP,
                                            exp_id,
                                            BLE_L2CAP_SIG_UPDATE_RSP_SZ,
                                            NULL);

    ble_l2cap_sig_update_rsp_parse(om->om_data, om->om_len, &rsp);
    TEST_ASSERT(rsp.result == exp_result);
}

void
ble_hs_test_util_set_static_rnd_addr(const uint8_t *addr)
{
    int rc;

    ble_hs_test_util_hci_ack_set(
        BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_RAND_ADDR), 0);

    rc = ble_hs_id_set_rnd(addr);
    TEST_ASSERT_FATAL(rc == 0);

    ble_hs_test_util_hci_out_first();
}

struct os_mbuf *
ble_hs_test_util_om_from_flat(const void *buf, uint16_t len)
{
    struct os_mbuf *om;

    om = ble_hs_mbuf_from_flat(buf, len);
    TEST_ASSERT_FATAL(om != NULL);

    return om;
}

int
ble_hs_test_util_flat_attr_cmp(const struct ble_hs_test_util_flat_attr *a,
                               const struct ble_hs_test_util_flat_attr *b)
{
    if (a->handle != b->handle) {
        return -1;
    }
    if (a->offset != b->offset) {
        return -1;
    }
    if (a->value_len != b->value_len) {
        return -1;
    }
    return memcmp(a->value, b->value, a->value_len);
}

void
ble_hs_test_util_attr_to_flat(struct ble_hs_test_util_flat_attr *flat,
                              const struct ble_gatt_attr *attr)
{
    int rc;

    flat->handle = attr->handle;
    flat->offset = attr->offset;
    rc = ble_hs_mbuf_to_flat(attr->om, flat->value, sizeof flat->value,
                           &flat->value_len);
    TEST_ASSERT_FATAL(rc == 0);
}

void
ble_hs_test_util_attr_from_flat(struct ble_gatt_attr *attr,
                                const struct ble_hs_test_util_flat_attr *flat)
{
    attr->handle = flat->handle;
    attr->offset = flat->offset;
    attr->om = ble_hs_test_util_om_from_flat(flat->value, flat->value_len);
}

int
ble_hs_test_util_read_local_flat(uint16_t attr_handle, uint16_t max_len,
                                 void *buf, uint16_t *out_len)
{
    struct os_mbuf *om;
    int rc;

    rc = ble_att_svr_read_local(attr_handle, &om);
    if (rc != 0) {
        return rc;
    }

    TEST_ASSERT_FATAL(OS_MBUF_PKTLEN(om) <= max_len);

    rc = os_mbuf_copydata(om, 0, OS_MBUF_PKTLEN(om), buf);
    TEST_ASSERT_FATAL(rc == 0);

    *out_len = OS_MBUF_PKTLEN(om);

    rc = os_mbuf_free_chain(om);
    TEST_ASSERT_FATAL(rc == 0);
    return 0;
}

int
ble_hs_test_util_write_local_flat(uint16_t attr_handle,
                                  const void *buf, uint16_t buf_len)
{
    struct os_mbuf *om;
    int rc;

    om = ble_hs_test_util_om_from_flat(buf, buf_len);
    rc = ble_att_svr_write_local(attr_handle, om);
    return rc;
}

int
ble_hs_test_util_gatt_write_flat(uint16_t conn_handle, uint16_t attr_handle,
                                 const void *data, uint16_t data_len,
                                 ble_gatt_attr_fn *cb, void *cb_arg)
{
    struct os_mbuf *om;
    int rc;

    om = ble_hs_test_util_om_from_flat(data, data_len);
    rc = ble_gattc_write(conn_handle, attr_handle, om, cb, cb_arg);

    return rc;
}

int
ble_hs_test_util_gatt_write_no_rsp_flat(uint16_t conn_handle,
                                        uint16_t attr_handle,
                                        const void *data, uint16_t data_len)
{
    struct os_mbuf *om;
    int rc;

    om = ble_hs_test_util_om_from_flat(data, data_len);
    rc = ble_gattc_write_no_rsp(conn_handle, attr_handle, om);

    return rc;
}

int
ble_hs_test_util_gatt_write_long_flat(uint16_t conn_handle,
                                      uint16_t attr_handle,
                                      const void *data, uint16_t data_len,
                                      ble_gatt_attr_fn *cb, void *cb_arg)
{
    struct os_mbuf *om;
    uint16_t offset = 0;
    int rc;

    om = ble_hs_test_util_om_from_flat(data, data_len);
    rc = ble_gattc_write_long(conn_handle, attr_handle, offset, om, cb, cb_arg);

    return rc;
}

static int
ble_hs_test_util_mbuf_chain_len(const struct os_mbuf *om)
{
    int count;

    count = 0;
    while (om != NULL) {
        count++;
        om = SLIST_NEXT(om, om_next);
    }

    return count;
}

static int
ble_hs_test_util_num_mbufs(void)
{
    return os_msys_count() + ble_hs_hci_frag_num_mbufs();
}

static int
ble_hs_test_util_num_mbufs_free(void)
{
    return os_msys_num_free() + ble_hs_hci_frag_num_mbufs_free();
}

struct os_mbuf *
ble_hs_test_util_mbuf_alloc_all_but(int count)
{
    struct os_mbuf *prev;
    struct os_mbuf *om;
    int rc;
    int i;

    /* Allocate all available mbufs and put them in a single chain. */
    prev = NULL;
    while (1) {
        om = os_msys_get(0, 0);
        if (om == NULL) {
            break;
        }

        if (prev != NULL) {
            SLIST_NEXT(om, om_next) = prev;
        }

        prev = om;
    }

    /* Now free 'count' mbufs. */
    for (i = 0; i < count; i++) {
        TEST_ASSERT_FATAL(prev != NULL);
        om = SLIST_NEXT(prev, om_next);
        rc = os_mbuf_free(prev);
        TEST_ASSERT_FATAL(rc == 0);

        prev = om;
    }

    return prev;
}

int
ble_hs_test_util_mbuf_count(const struct ble_hs_test_util_mbuf_params *params)
{
    const struct ble_att_prep_entry *prep;
    const struct os_mbuf_pkthdr *omp;
    const struct ble_l2cap_chan *chan;
    const struct ble_hs_conn *conn;
    const struct os_mbuf *om;
    int count;
    int i;

    ble_hs_process_rx_data_queue();

    count = ble_hs_test_util_num_mbufs_free();

    if (params->prev_tx) {
        count += ble_hs_test_util_mbuf_chain_len(ble_hs_test_util_prev_tx_cur);
        STAILQ_FOREACH(omp, &ble_hs_test_util_prev_tx_queue, omp_next) {
            om = OS_MBUF_PKTHDR_TO_MBUF(omp);
            count += ble_hs_test_util_mbuf_chain_len(om);
        }
    }

    ble_hs_lock();
    for (i = 0; ; i++) {
        conn = ble_hs_conn_find_by_idx(i);
        if (conn == NULL) {
            break;
        }

        if (params->rx_queue) {
            SLIST_FOREACH(chan, &conn->bhc_channels, next) {
                count += ble_hs_test_util_mbuf_chain_len(chan->rx_buf);
            }
        }

        if (params->prep_list) {
            SLIST_FOREACH(prep, &conn->bhc_att_svr.basc_prep_list, bape_next) {
                count += ble_hs_test_util_mbuf_chain_len(prep->bape_value);
            }
        }
    }
    ble_hs_unlock();

    return count;
}

void
ble_hs_test_util_assert_mbufs_freed(
    const struct ble_hs_test_util_mbuf_params *params)
{
    static const struct ble_hs_test_util_mbuf_params dflt = {
        .prev_tx = 1,
        .rx_queue = 1,
        .prep_list = 1,
    };

    int count;

    if (params == NULL) {
        params = &dflt;
    }

    count = ble_hs_test_util_mbuf_count(params);
    TEST_ASSERT(count == ble_hs_test_util_num_mbufs());
}

static int
ble_hs_test_util_pkt_txed(struct os_mbuf *om, void *arg)
{
    ble_hs_test_util_prev_tx_enqueue(om);
    return 0;
}

static int
ble_hs_test_util_hci_txed(uint8_t *cmdbuf, void *arg)
{
    ble_hs_test_util_hci_out_enqueue(cmdbuf);
    ble_hci_trans_buf_free(cmdbuf);
    return 0;
}

int
ble_hs_test_util_num_cccds(void)
{
    struct ble_store_value_cccd val;
    struct ble_store_key_cccd key = { };
    int rc;

    key.peer_addr = *BLE_ADDR_ANY;
    for (key.idx = 0; ; key.idx++) {
        rc = ble_store_read_cccd(&key, &val);
        switch (rc) {
        case 0:
            break;

        case BLE_HS_ENOENT:
            return key.idx;

        default:
            TEST_ASSERT_FATAL(0);
        }
    }
}

int
ble_hs_test_util_num_our_secs(void)
{
    struct ble_store_value_sec val;
    struct ble_store_key_sec key = { };
    int rc;

    key.peer_addr = *BLE_ADDR_ANY;
    for (key.idx = 0; ; key.idx++) {
        rc = ble_store_read_our_sec(&key, &val);
        switch (rc) {
        case 0:
            break;

        case BLE_HS_ENOENT:
            return key.idx;

        default:
            TEST_ASSERT_FATAL(0);
        }
    }
}

int
ble_hs_test_util_num_peer_secs(void)
{
    struct ble_store_value_sec val;
    struct ble_store_key_sec key = { };
    int rc;

    key.peer_addr = *BLE_ADDR_ANY;
    for (key.idx = 0; ; key.idx++) {
        rc = ble_store_read_peer_sec(&key, &val);
        switch (rc) {
        case 0:
            break;

        case BLE_HS_ENOENT:
            return key.idx;

        default:
            TEST_ASSERT_FATAL(0);
        }
    }
}

static int
ble_hs_test_util_store_read(int obj_type, const union ble_store_key *key,
                            union ble_store_value *value)
{
    int rc;

    ble_sm_test_store_obj_type = obj_type;
    ble_sm_test_store_key = *key;

    rc = ble_store_config_read(obj_type, key, value);
    ble_sm_test_store_value = *value;

    return rc;
}

static int
ble_hs_test_util_store_write(int obj_type, const union ble_store_value *value)
{
    int rc;

    ble_sm_test_store_obj_type = obj_type;

    rc = ble_store_config_write(obj_type, value);
    ble_sm_test_store_value = *value;

    return rc;
}

static int
ble_hs_test_util_store_delete(int obj_type, const union ble_store_key *key)
{
    int rc;

    ble_sm_test_store_obj_type = obj_type;
    ble_sm_test_store_key = *key;

    rc = ble_store_config_delete(obj_type, key);
    return rc;
}

void
ble_hs_test_util_reg_svcs(const struct ble_gatt_svc_def *svcs,
                          ble_gatt_register_fn *reg_cb,
                          void *cb_arg)
{
    int rc;

    ble_hs_cfg.gatts_register_cb = reg_cb;
    ble_hs_cfg.gatts_register_arg = cb_arg;

    rc = ble_gatts_reset();
    TEST_ASSERT_FATAL(rc == 0);

    rc = ble_gatts_add_svcs(svcs);
    TEST_ASSERT_FATAL(rc == 0);

    rc = ble_gatts_start();
    TEST_ASSERT_FATAL(rc == 0);
}


void
ble_hs_test_util_init_no_sysinit_no_start(void)
{
    STAILQ_INIT(&ble_hs_test_util_prev_tx_queue);
    ble_hs_test_util_prev_tx_cur = NULL;

    ble_hs_hci_set_phony_ack_cb(NULL);

    ble_hci_trans_cfg_ll(ble_hs_test_util_hci_txed, NULL,
                         ble_hs_test_util_pkt_txed, NULL);

    ble_hs_test_util_hci_ack_set_startup();

    ble_hs_enabled_state = BLE_HS_ENABLED_STATE_OFF;

    ble_hs_max_services = 16;
    ble_hs_max_client_configs = 32;
    ble_hs_max_attrs = 64;

    ble_hs_test_util_hci_out_clear();

    ble_hs_cfg.store_read_cb = ble_hs_test_util_store_read;
    ble_hs_cfg.store_write_cb = ble_hs_test_util_store_write;
    ble_hs_cfg.store_delete_cb = ble_hs_test_util_store_delete;

    ble_store_clear();
}

void
ble_hs_test_util_init_no_start(void)
{
    sysinit();
    ble_hs_test_util_init_no_sysinit_no_start();
}

void
ble_hs_test_util_init(void)
{
    int rc;

    ble_hs_test_util_init_no_start();

    rc = ble_hs_start();
    TEST_ASSERT_FATAL(rc == 0);

    ble_hs_test_util_hci_out_clear();

    /* Clear random address. */
    ble_hs_test_util_set_static_rnd_addr((uint8_t[6]){ 0, 0, 0, 0, 0, 0 });
}
