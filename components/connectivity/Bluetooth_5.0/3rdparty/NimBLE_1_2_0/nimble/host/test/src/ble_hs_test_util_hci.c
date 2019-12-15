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

#include "testutil/testutil.h"
#include "nimble/ble.h"
#include "nimble/hci_common.h"
#include "transport/ram/ble_hci_ram.h"
#include "ble_hs_test_util.h"

#define BLE_HS_TEST_UTIL_PREV_HCI_TX_CNT      64

static uint8_t
ble_hs_test_util_hci_out_queue[BLE_HS_TEST_UTIL_PREV_HCI_TX_CNT][260];
static int ble_hs_test_util_hci_out_queue_sz;
static uint8_t ble_hs_test_util_hci_out_cur[260];

static struct ble_hs_test_util_hci_ack
ble_hs_test_util_hci_acks[BLE_HS_TEST_UTIL_PHONY_ACK_MAX];
static int ble_hs_test_util_hci_num_acks;

/*****************************************************************************
 * $tx queue                                                                 *
 *****************************************************************************/

void
ble_hs_test_util_hci_out_adj(int count)
{
    if (count >= 0) {
        TEST_ASSERT(ble_hs_test_util_hci_out_queue_sz >= count);

        ble_hs_test_util_hci_out_queue_sz -= count;
        if (ble_hs_test_util_hci_out_queue_sz > 0) {
            memmove(
                ble_hs_test_util_hci_out_queue,
                ble_hs_test_util_hci_out_queue + count,
                sizeof ble_hs_test_util_hci_out_queue[0] *
                ble_hs_test_util_hci_out_queue_sz);
        }
    } else {
        TEST_ASSERT(ble_hs_test_util_hci_out_queue_sz >= -count);

        ble_hs_test_util_hci_out_adj(
            ble_hs_test_util_hci_out_queue_sz + count);
    }
}

void *
ble_hs_test_util_hci_out_first(void)
{
    if (ble_hs_test_util_hci_out_queue_sz == 0) {
        return NULL;
    }

    memcpy(ble_hs_test_util_hci_out_cur, ble_hs_test_util_hci_out_queue[0],
           sizeof ble_hs_test_util_hci_out_cur);

    ble_hs_test_util_hci_out_adj(1);

    return ble_hs_test_util_hci_out_cur;
}

void *
ble_hs_test_util_hci_out_last(void)
{
    if (ble_hs_test_util_hci_out_queue_sz == 0) {
        return NULL;
    }

    ble_hs_test_util_hci_out_queue_sz--;
    memcpy(ble_hs_test_util_hci_out_cur,
           ble_hs_test_util_hci_out_queue + ble_hs_test_util_hci_out_queue_sz,
           sizeof ble_hs_test_util_hci_out_cur);

    return ble_hs_test_util_hci_out_cur;
}

void
ble_hs_test_util_hci_out_enqueue(void *cmd)
{
    TEST_ASSERT_FATAL(ble_hs_test_util_hci_out_queue_sz <
                      BLE_HS_TEST_UTIL_PREV_HCI_TX_CNT);
    memcpy(ble_hs_test_util_hci_out_queue + ble_hs_test_util_hci_out_queue_sz,
           cmd, 260);

    ble_hs_test_util_hci_out_queue_sz++;
}

void
ble_hs_test_util_hci_out_clear(void)
{
    ble_hs_test_util_hci_out_queue_sz = 0;
}

/*****************************************************************************
 * $build                                                                    *
 *****************************************************************************/

void
ble_hs_test_util_hci_build_cmd_complete(uint8_t *dst, int len,
                                    uint8_t param_len, uint8_t num_pkts,
                                    uint16_t opcode)
{
    TEST_ASSERT(len >= BLE_HCI_EVENT_CMD_COMPLETE_HDR_LEN);

    dst[0] = BLE_HCI_EVCODE_COMMAND_COMPLETE;
    dst[1] = 3 + param_len;
    dst[2] = num_pkts;
    put_le16(dst + 3, opcode);
}

void
ble_hs_test_util_hci_build_cmd_status(uint8_t *dst, int len,
                                  uint8_t status, uint8_t num_pkts,
                                  uint16_t opcode)
{
    TEST_ASSERT(len >= BLE_HCI_EVENT_CMD_STATUS_LEN);

    dst[0] = BLE_HCI_EVCODE_COMMAND_STATUS;
    dst[1] = BLE_HCI_EVENT_CMD_STATUS_LEN;
    dst[2] = status;
    dst[3] = num_pkts;
    put_le16(dst + 4, opcode);
}

static void
ble_hs_test_util_hci_build_ack_params(
    struct ble_hs_test_util_hci_ack *ack,
    uint16_t opcode, uint8_t status, void *params, uint8_t params_len)
{
    ack->opcode = opcode;
    ack->status = status;

    if (params == NULL || params_len == 0) {
        ack->evt_params_len = 0;
    } else {
        memcpy(ack->evt_params, params, params_len);
        ack->evt_params_len = params_len;
    }
}

/*****************************************************************************
 * $ack                                                                      *
 *****************************************************************************/

static int
ble_hs_test_util_hci_ack_cb(uint8_t *ack, int ack_buf_len)
{
    struct ble_hs_test_util_hci_ack *entry;

    if (ble_hs_test_util_hci_num_acks == 0) {
        return BLE_HS_ETIMEOUT_HCI;
    }

    entry = ble_hs_test_util_hci_acks;

    ble_hs_test_util_hci_build_cmd_complete(ack, 256,
                                        entry->evt_params_len + 1, 1,
                                        entry->opcode);
    ack[BLE_HCI_EVENT_CMD_COMPLETE_HDR_LEN] = entry->status;
    memcpy(ack + BLE_HCI_EVENT_CMD_COMPLETE_HDR_LEN + 1, entry->evt_params,
           entry->evt_params_len);

    ble_hs_test_util_hci_num_acks--;
    if (ble_hs_test_util_hci_num_acks > 0) {
        memmove(ble_hs_test_util_hci_acks,
                ble_hs_test_util_hci_acks + 1,
                sizeof *entry * ble_hs_test_util_hci_num_acks);
    }

    return 0;
}

void
ble_hs_test_util_hci_ack_set_params(uint16_t opcode, uint8_t status,
                                    void *params, uint8_t params_len)
{
    struct ble_hs_test_util_hci_ack *ack;

    ack = ble_hs_test_util_hci_acks + 0;
    ble_hs_test_util_hci_build_ack_params(ack, opcode, status, params,
                                          params_len);
    ble_hs_test_util_hci_num_acks = 1;

    ble_hs_hci_set_phony_ack_cb(ble_hs_test_util_hci_ack_cb);
}

void
ble_hs_test_util_hci_ack_set(uint16_t opcode, uint8_t status)
{
    ble_hs_test_util_hci_ack_set_params(opcode, status, NULL, 0);
}

void
ble_hs_test_util_hci_ack_append_params(uint16_t opcode, uint8_t status,
                                       void *params, uint8_t params_len)
{
    struct ble_hs_test_util_hci_ack *ack;

    ack = ble_hs_test_util_hci_acks + ble_hs_test_util_hci_num_acks;
    ble_hs_test_util_hci_build_ack_params(ack, opcode, status, params,
                                          params_len);
    ble_hs_test_util_hci_num_acks++;

    ble_hs_hci_set_phony_ack_cb(ble_hs_test_util_hci_ack_cb);
}

void
ble_hs_test_util_hci_ack_append(uint16_t opcode, uint8_t status)
{
    ble_hs_test_util_hci_ack_append_params(opcode, status, NULL, 0);
}

static const struct ble_hs_test_util_hci_ack hci_startup_seq[] = {
    {
        .opcode = ble_hs_hci_util_opcode_join(BLE_HCI_OGF_CTLR_BASEBAND,
                                              BLE_HCI_OCF_CB_RESET),
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
         BLE_HCI_OGF_INFO_PARAMS, BLE_HCI_OCF_IP_RD_LOCAL_VER),
        .evt_params = { 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        .evt_params_len = 8,
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
         BLE_HCI_OGF_INFO_PARAMS, BLE_HCI_OCF_IP_RD_LOC_SUPP_FEAT),
        .evt_params = { 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00},
        .evt_params_len = 8,
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_CTLR_BASEBAND, BLE_HCI_OCF_CB_SET_EVENT_MASK),
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_CTLR_BASEBAND, BLE_HCI_OCF_CB_SET_EVENT_MASK2),
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_SET_EVENT_MASK),
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_RD_BUF_SIZE),
        /* Use a very low buffer size (20) to test fragmentation.
         * Use a large num-pkts (200) to prevent controller buf exhaustion.
         */
        .evt_params = { 0x14, 0x00, 200 },
        .evt_params_len = 3,
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_RD_LOC_SUPP_FEAT),
        .evt_params = { 0 },
        .evt_params_len = 8,
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_INFO_PARAMS, BLE_HCI_OCF_IP_RD_BD_ADDR),
        .evt_params = BLE_HS_TEST_UTIL_PUB_ADDR_VAL,
        .evt_params_len = 6,
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_SET_ADDR_RES_EN),
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_CLR_RESOLV_LIST),
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_SET_ADDR_RES_EN),
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_SET_ADV_ENABLE),
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_ADD_RESOLV_LIST),
    },
    {
        .opcode = ble_hs_hci_util_opcode_join(
            BLE_HCI_OGF_LE, BLE_HCI_OCF_LE_SET_PRIVACY_MODE),
    },
    { 0 }
};

void
ble_hs_test_util_hci_ack_set_seq(const struct ble_hs_test_util_hci_ack *acks)
{
    int i;

    for (i = 0; acks[i].opcode != 0; i++) {
        ble_hs_test_util_hci_acks[i] = acks[i];
    }
    ble_hs_test_util_hci_num_acks = i;

    ble_hs_hci_set_phony_ack_cb(ble_hs_test_util_hci_ack_cb);
}

int
ble_hs_test_util_hci_startup_seq_cnt(void)
{
    /* last element is terminator, don't count it*/
    return sizeof(hci_startup_seq)/sizeof(hci_startup_seq[0]) - 1;
}

void
ble_hs_test_util_hci_ack_set_startup(void)
{
    /* Receive acknowledgements for the startup sequence.  We sent the
     * corresponding requests when the host task was started.
     */
    ble_hs_test_util_hci_ack_set_seq(hci_startup_seq);
}

void
ble_hs_test_util_hci_ack_set_disc(uint8_t own_addr_type,
                                  int fail_idx, uint8_t fail_status)
{
    static bool privacy_enabled;

    /*
     * SET_RPA_TMO should be expected only when test uses RPA and privacy has
     * not yet been enabled. The Bluetooth stack remembers that privacy is
     * enabled and does not send SET_RPA_TMO every time. For test purpose
     * let's track privacy state in here.
     */
    if ((own_addr_type == BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT ||
         own_addr_type == BLE_OWN_ADDR_RPA_RANDOM_DEFAULT) &&
        !privacy_enabled) {

        privacy_enabled = true;
        ble_hs_test_util_hci_ack_set_seq(((struct ble_hs_test_util_hci_ack[]) {
            {
                BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_RPA_TMO),
                ble_hs_test_util_hci_misc_exp_status(0, fail_idx, fail_status),
            },
            {
                BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_SCAN_PARAMS),
                ble_hs_test_util_hci_misc_exp_status(1, fail_idx, fail_status),
            },
            {
                BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_SCAN_ENABLE),
                ble_hs_test_util_hci_misc_exp_status(2, fail_idx, fail_status),
            },

            { 0 }
        }));
    } else {
        ble_hs_test_util_hci_ack_set_seq(((struct ble_hs_test_util_hci_ack[]) {
            {
                BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_SCAN_PARAMS),
                ble_hs_test_util_hci_misc_exp_status(0, fail_idx, fail_status),
            },
            {
                BLE_HS_TEST_UTIL_LE_OPCODE(BLE_HCI_OCF_LE_SET_SCAN_ENABLE),
                ble_hs_test_util_hci_misc_exp_status(1, fail_idx, fail_status),
            },

            { 0 }
        }));
    }
}

void
ble_hs_test_util_hci_ack_set_disconnect(uint8_t hci_status)
{
    ble_hs_test_util_hci_ack_set(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LINK_CTRL,
                                    BLE_HCI_OCF_DISCONNECT_CMD),
        hci_status);
}

/*****************************************************************************
 * $verify tx                                                                *
 *****************************************************************************/

void
ble_hs_test_util_hci_verify_tx_add_irk(uint8_t addr_type,
                                       const uint8_t *addr,
                                       const uint8_t *peer_irk,
                                       const uint8_t *local_irk)
{
    uint8_t param_len;
    uint8_t *param;


    param = ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                           BLE_HCI_OCF_LE_ADD_RESOLV_LIST,
                                           &param_len);
    TEST_ASSERT(param_len == BLE_HCI_ADD_TO_RESOLV_LIST_LEN);

    TEST_ASSERT(param[0] == addr_type);
    TEST_ASSERT(memcmp(param + 1, addr, 6) == 0);
    TEST_ASSERT(memcmp(param + 7, peer_irk, 16) == 0);
    TEST_ASSERT(memcmp(param + 23, local_irk, 16) == 0);
}

void
ble_hs_test_util_hci_verify_tx_set_priv_mode(uint8_t addr_type,
                                             const uint8_t *addr,
                                             uint8_t priv_mode)
{
    uint8_t param_len;
    uint8_t *param;

    param = ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                           BLE_HCI_OCF_LE_SET_PRIVACY_MODE,
                                           &param_len);
    TEST_ASSERT(param_len == BLE_HCI_LE_SET_PRIVACY_MODE_LEN);

    TEST_ASSERT(param[0] == addr_type);
    TEST_ASSERT(memcmp(param + 1, addr, 6) == 0);
    TEST_ASSERT(param[7] == priv_mode);
}

void
ble_hs_test_util_hci_verify_tx_disconnect(uint16_t handle, uint8_t reason)
{
    uint8_t param_len;
    uint8_t *param;

    param = ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LINK_CTRL,
                                           BLE_HCI_OCF_DISCONNECT_CMD,
                                           &param_len);
    TEST_ASSERT(param_len == BLE_HCI_DISCONNECT_CMD_LEN);

    TEST_ASSERT(get_le16(param + 0) == handle);
    TEST_ASSERT(param[2] == reason);
}

void
ble_hs_test_util_hci_verify_tx_create_conn(const struct hci_create_conn *exp)
{
    uint8_t param_len;
    uint8_t *param;

    param = ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                           BLE_HCI_OCF_LE_CREATE_CONN,
                                           &param_len);
    TEST_ASSERT(param_len == BLE_HCI_CREATE_CONN_LEN);

    TEST_ASSERT(get_le16(param + 0) == exp->scan_itvl);
    TEST_ASSERT(get_le16(param + 2) == exp->scan_window);
    TEST_ASSERT(param[4] == exp->filter_policy);
    TEST_ASSERT(param[5] == exp->peer_addr_type);
    TEST_ASSERT(memcmp(param + 6, exp->peer_addr, 6) == 0);
    TEST_ASSERT(param[12] == exp->own_addr_type);
    TEST_ASSERT(get_le16(param + 13) == exp->conn_itvl_min);
    TEST_ASSERT(get_le16(param + 15) == exp->conn_itvl_max);
    TEST_ASSERT(get_le16(param + 17) == exp->conn_latency);
    TEST_ASSERT(get_le16(param + 19) == exp->supervision_timeout);
    TEST_ASSERT(get_le16(param + 21) == exp->min_ce_len);
    TEST_ASSERT(get_le16(param + 23) == exp->max_ce_len);
}

uint8_t *
ble_hs_test_util_hci_verify_tx(uint8_t ogf, uint16_t ocf,
                               uint8_t *out_param_len)
{
    uint16_t opcode;
    uint8_t *cmd;

    cmd = ble_hs_test_util_hci_out_first();
    TEST_ASSERT_FATAL(cmd != NULL);

    opcode = get_le16(cmd);
    TEST_ASSERT(BLE_HCI_OGF(opcode) == ogf);
    TEST_ASSERT(BLE_HCI_OCF(opcode) == ocf);

    if (out_param_len != NULL) {
        *out_param_len = cmd[2];
    }

    return cmd + 3;
}

/*****************************************************************************
 * $rx                                                                       *
 *****************************************************************************/

static void
ble_hs_test_util_hci_rx_evt(uint8_t *evt)
{
    uint8_t *evbuf;
    int totlen;
    int rc;

    totlen = BLE_HCI_EVENT_HDR_LEN + evt[1];
    TEST_ASSERT_FATAL(totlen <= UINT8_MAX + BLE_HCI_EVENT_HDR_LEN);

    evbuf = ble_hci_trans_buf_alloc(
        BLE_HCI_TRANS_BUF_EVT_LO);
    TEST_ASSERT_FATAL(evbuf != NULL);

    memcpy(evbuf, evt, totlen);

    if (os_started()) {
        rc = ble_hci_trans_ll_evt_tx(evbuf);
    } else {
        rc = ble_hs_hci_evt_process(evbuf);
    }

    TEST_ASSERT_FATAL(rc == 0);
}

void
ble_hs_test_util_hci_rx_num_completed_pkts_event(
    struct ble_hs_test_util_hci_num_completed_pkts_entry *entries)
{
    struct ble_hs_test_util_hci_num_completed_pkts_entry *entry;
    uint8_t buf[1024];
    int num_entries;
    int off;
    int i;

    /* Count number of entries. */
    num_entries = 0;
    for (entry = entries; entry->handle_id != 0; entry++) {
        num_entries++;
    }
    TEST_ASSERT_FATAL(num_entries <= UINT8_MAX);

    buf[0] = BLE_HCI_EVCODE_NUM_COMP_PKTS;
    buf[2] = num_entries;

    off = 3;
    for (i = 0; i < num_entries; i++) {
        put_le16(buf + off, entries[i].handle_id);
        off += 2;
        put_le16(buf + off, entries[i].num_pkts);
        off += 2;
    }

    buf[1] = off - 2;

    ble_hs_test_util_hci_rx_evt(buf);
}

void
ble_hs_test_util_hci_rx_disconn_complete_event(
    struct hci_disconn_complete *evt)
{
    uint8_t buf[BLE_HCI_EVENT_HDR_LEN + BLE_HCI_EVENT_DISCONN_COMPLETE_LEN];

    buf[0] = BLE_HCI_EVCODE_DISCONN_CMP;
    buf[1] = BLE_HCI_EVENT_DISCONN_COMPLETE_LEN;
    buf[2] = evt->status;
    put_le16(buf + 3, evt->connection_handle);
    buf[5] = evt->reason;

    ble_hs_test_util_hci_rx_evt(buf);
}

void
ble_hs_test_util_hci_rx_conn_cancel_evt(void)
{
    struct hci_le_conn_complete evt;
    int rc;

    memset(&evt, 0, sizeof evt);
    evt.subevent_code = BLE_HCI_LE_SUBEV_CONN_COMPLETE;
    evt.status = BLE_ERR_UNK_CONN_ID;
    /* test if host correctly ignores other fields if status is error */
    evt.connection_handle = 0x0fff;

    rc = ble_gap_rx_conn_complete(&evt, 0);
    TEST_ASSERT_FATAL(rc == 0);
}

/*****************************************************************************
 * $misc                                                                     *
 *****************************************************************************/

int
ble_hs_test_util_hci_misc_exp_status(int cmd_idx, int fail_idx,
                                     uint8_t fail_status)
{
    if (cmd_idx == fail_idx) {
        return BLE_HS_HCI_ERR(fail_status);
    } else {
        return 0;
    }
}
