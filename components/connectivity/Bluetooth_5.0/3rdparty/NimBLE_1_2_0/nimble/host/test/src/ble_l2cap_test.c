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
#include <errno.h>
#include "testutil/testutil.h"
#include "nimble/hci_common.h"
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

#define BLE_L2CAP_TEST_PSM                   (90)
#define BLE_L2CAP_TEST_CID                   (99)
#define BLE_L2CAP_TEST_COC_MTU               (256)
/* We use same pool for incoming and outgoing sdu */
#define BLE_L2CAP_TEST_COC_BUF_COUNT         (6 * MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM))

static uint16_t ble_l2cap_test_update_conn_handle;
static int ble_l2cap_test_update_status;
static void *ble_l2cap_test_update_arg;

static void *test_sdu_coc_mem;
struct os_mbuf_pool sdu_os_mbuf_pool;
static struct os_mempool sdu_coc_mbuf_mempool;
static uint16_t current_cid  = 0x0040;
/*****************************************************************************
 * $util                                                                     *
 *****************************************************************************/

static void
ble_l2cap_test_util_init(void)
{
    ble_hs_test_util_init();
    ble_l2cap_test_update_conn_handle = BLE_HS_CONN_HANDLE_NONE;
    ble_l2cap_test_update_status = -1;
    ble_l2cap_test_update_arg = (void *)(uintptr_t)-1;
    int rc;

    if (test_sdu_coc_mem) {
        free(test_sdu_coc_mem);
    }

    /* For testing we want to support all the available channels */
    test_sdu_coc_mem = malloc(
        OS_MEMPOOL_BYTES(BLE_L2CAP_TEST_COC_BUF_COUNT,BLE_L2CAP_TEST_COC_MTU));
    assert(test_sdu_coc_mem != NULL);

    rc = os_mempool_init(&sdu_coc_mbuf_mempool, BLE_L2CAP_TEST_COC_BUF_COUNT,
                         BLE_L2CAP_TEST_COC_MTU, test_sdu_coc_mem,
                         "test_coc_sdu_pool");
    assert(rc == 0);

    rc = os_mbuf_pool_init(&sdu_os_mbuf_pool, &sdu_coc_mbuf_mempool,
                           BLE_L2CAP_TEST_COC_MTU, BLE_L2CAP_TEST_COC_BUF_COUNT);
    assert(rc == 0);

}

static void
ble_l2cap_test_util_rx_update_req(uint16_t conn_handle, uint8_t id,
                                  struct ble_l2cap_sig_update_params *params)
{
    struct ble_l2cap_sig_update_req *req;
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    int rc;

    hci_hdr = BLE_HS_TEST_UTIL_L2CAP_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + BLE_L2CAP_SIG_HDR_SZ + BLE_L2CAP_SIG_UPDATE_REQ_SZ);

    req = ble_l2cap_sig_cmd_get(BLE_L2CAP_SIG_OP_UPDATE_REQ, id,
                                BLE_L2CAP_SIG_UPDATE_REQ_SZ, &om);
    TEST_ASSERT_FATAL(req != NULL);

    req->itvl_min = htole16(params->itvl_min);
    req->itvl_max = htole16(params->itvl_max);
    req->slave_latency = htole16(params->slave_latency);
    req->timeout_multiplier = htole16(params->timeout_multiplier);

    ble_hs_test_util_hci_ack_set(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_CONN_UPDATE), 0);
    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SIG,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == 0);
}

static void
ble_l2cap_test_util_verify_tx_update_conn(
    struct ble_gap_upd_params *params)
{
    uint8_t param_len;
    uint8_t *param;

    param = ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                           BLE_HCI_OCF_LE_CONN_UPDATE,
                                           &param_len);
    TEST_ASSERT(param_len == BLE_HCI_CONN_UPDATE_LEN);
    TEST_ASSERT(get_le16(param + 0) == 2);
    TEST_ASSERT(get_le16(param + 2) == params->itvl_min);
    TEST_ASSERT(get_le16(param + 4) == params->itvl_max);
    TEST_ASSERT(get_le16(param + 6) == params->latency);
    TEST_ASSERT(get_le16(param + 8) == params->supervision_timeout);
    TEST_ASSERT(get_le16(param + 10) == params->min_ce_len);
    TEST_ASSERT(get_le16(param + 12) == params->max_ce_len);
}

static int
ble_l2cap_test_util_dummy_rx(struct ble_l2cap_chan *chan)
{
    return 0;
}

static void
ble_l2cap_test_util_create_conn(uint16_t conn_handle, uint8_t *addr,
                                ble_gap_event_fn *cb, void *cb_arg)
{
    struct ble_l2cap_chan *chan;
    struct ble_hs_conn *conn;

    ble_hs_test_util_create_conn(conn_handle, addr, cb, cb_arg);

    ble_hs_lock();

    conn = ble_hs_conn_find(conn_handle);
    TEST_ASSERT_FATAL(conn != NULL);

    chan = ble_l2cap_chan_alloc(conn_handle);
    TEST_ASSERT_FATAL(chan != NULL);

    chan->scid = BLE_L2CAP_TEST_CID;
    chan->my_mtu = 240;
    chan->rx_fn = ble_l2cap_test_util_dummy_rx;

    ble_hs_conn_chan_insert(conn, chan);

    ble_hs_test_util_hci_out_clear();

    ble_hs_unlock();
}

static int
ble_l2cap_test_util_rx_first_frag(uint16_t conn_handle,
                                  uint16_t l2cap_frag_len,
                                  uint16_t cid, uint16_t l2cap_len)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    uint16_t hci_len;
    void *v;
    int rc;

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    v = os_mbuf_extend(om, l2cap_frag_len);
    TEST_ASSERT_FATAL(v != NULL);

    om = ble_l2cap_prepend_hdr(om, cid, l2cap_len);
    TEST_ASSERT_FATAL(om != NULL);

    hci_len = sizeof hci_hdr + l2cap_frag_len;
    hci_hdr = BLE_HS_TEST_UTIL_L2CAP_HCI_HDR(conn_handle,
                                          BLE_HCI_PB_FIRST_FLUSH, hci_len);
    rc = ble_hs_test_util_l2cap_rx(conn_handle, &hci_hdr, om);
    return rc;
}

static int
ble_l2cap_test_util_rx_next_frag(uint16_t conn_handle, uint16_t hci_len)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int rc;

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    v = os_mbuf_extend(om, hci_len);
    TEST_ASSERT_FATAL(v != NULL);

    hci_hdr = BLE_HS_TEST_UTIL_L2CAP_HCI_HDR(conn_handle,
                                          BLE_HCI_PB_MIDDLE, hci_len);
    rc = ble_hs_test_util_l2cap_rx(conn_handle, &hci_hdr, om);
    return rc;
}

static void
ble_l2cap_test_util_verify_first_frag(uint16_t conn_handle,
                                      uint16_t l2cap_frag_len,
                                      uint16_t l2cap_len)
{
    struct ble_hs_conn *conn;
    int rc;

    rc = ble_l2cap_test_util_rx_first_frag(conn_handle, l2cap_frag_len,
                                           BLE_L2CAP_TEST_CID, l2cap_len);
    TEST_ASSERT(rc == 0);

    ble_hs_lock();

    conn = ble_hs_conn_find(conn_handle);
    TEST_ASSERT_FATAL(conn != NULL);
    TEST_ASSERT(conn->bhc_rx_chan != NULL &&
                conn->bhc_rx_chan->scid == BLE_L2CAP_TEST_CID);

    ble_hs_unlock();
}

static void
ble_l2cap_test_util_verify_middle_frag(uint16_t conn_handle,
                                       uint16_t hci_len)
{
    struct ble_hs_conn *conn;
    int rc;

    rc = ble_l2cap_test_util_rx_next_frag(conn_handle, hci_len);
    TEST_ASSERT(rc == 0);

    ble_hs_lock();

    conn = ble_hs_conn_find(conn_handle);
    TEST_ASSERT_FATAL(conn != NULL);
    TEST_ASSERT(conn->bhc_rx_chan != NULL &&
                conn->bhc_rx_chan->scid == BLE_L2CAP_TEST_CID);

    ble_hs_unlock();
}

static void
ble_l2cap_test_util_verify_last_frag(uint16_t conn_handle,
                                     uint16_t hci_len)
{
    struct ble_hs_conn *conn;
    int rc;

    rc = ble_l2cap_test_util_rx_next_frag(conn_handle, hci_len);
    TEST_ASSERT(rc == 0);

    ble_hs_lock();

    conn = ble_hs_conn_find(conn_handle);
    TEST_ASSERT_FATAL(conn != NULL);
    TEST_ASSERT(conn->bhc_rx_chan == NULL);

    ble_hs_unlock();
}

/*****************************************************************************
 * $rx                                                                       *
 *****************************************************************************/

TEST_CASE_SELF(ble_l2cap_test_case_bad_header)
{
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    NULL, NULL);

    rc = ble_l2cap_test_util_rx_first_frag(2, 14, 1234, 10);
    TEST_ASSERT(rc == BLE_HS_ENOENT);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_bad_handle)
{
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    NULL, NULL);

    rc = ble_l2cap_test_util_rx_first_frag(1234, 14, 1234, 10);
    TEST_ASSERT(rc == BLE_HS_ENOTCONN);

    /* Ensure we did not send anything in return. */
    TEST_ASSERT_FATAL(ble_hs_test_util_prev_tx_dequeue() == NULL);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*****************************************************************************
 * $fragmentation                                                            *
 *****************************************************************************/

TEST_CASE_SELF(ble_l2cap_test_case_frag_single)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    NULL, NULL);

    /*** HCI header specifies middle fragment without start. */
    hci_hdr = BLE_HS_TEST_UTIL_L2CAP_HCI_HDR(2, BLE_HCI_PB_MIDDLE, 10);

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    om = ble_l2cap_prepend_hdr(om, 0, 5);
    TEST_ASSERT_FATAL(om != NULL);

    rc = ble_hs_test_util_l2cap_rx(2, &hci_hdr, om);
    TEST_ASSERT(rc == BLE_HS_EBADDATA);

    /*** Packet consisting of three fragments. */
    ble_l2cap_test_util_verify_first_frag(2, 10, 30);
    ble_l2cap_test_util_verify_middle_frag(2, 10);
    ble_l2cap_test_util_verify_last_frag(2, 10);

    /*** Packet consisting of five fragments. */
    ble_l2cap_test_util_verify_first_frag(2, 8, 49);
    ble_l2cap_test_util_verify_middle_frag(2, 13);
    ble_l2cap_test_util_verify_middle_frag(2, 2);
    ble_l2cap_test_util_verify_middle_frag(2, 21);
    ble_l2cap_test_util_verify_last_frag(2, 5);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_frag_multiple)
{
    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    NULL, NULL);
    ble_l2cap_test_util_create_conn(3, ((uint8_t[]){2,3,4,5,6,7}),
                                    NULL, NULL);
    ble_l2cap_test_util_create_conn(4, ((uint8_t[]){3,4,5,6,7,8}),
                                    NULL, NULL);

    ble_l2cap_test_util_verify_first_frag(2, 3, 10);
    ble_l2cap_test_util_verify_first_frag(3, 2, 5);
    ble_l2cap_test_util_verify_middle_frag(2, 6);
    ble_l2cap_test_util_verify_first_frag(4, 1, 4);
    ble_l2cap_test_util_verify_middle_frag(3, 2);
    ble_l2cap_test_util_verify_last_frag(3, 1);
    ble_l2cap_test_util_verify_middle_frag(4, 2);
    ble_l2cap_test_util_verify_last_frag(4, 1);
    ble_l2cap_test_util_verify_last_frag(2, 1);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_frag_channels)
{
    struct ble_hs_conn *conn;
    int rc;
    uint16_t data_len = 30;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    NULL, NULL);

    /* Receive a starting fragment on the first channel. */
    rc = ble_l2cap_test_util_rx_first_frag(2, 14, BLE_L2CAP_TEST_CID, data_len);
    TEST_ASSERT(rc == 0);

    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    TEST_ASSERT(conn->bhc_rx_chan != NULL &&
                conn->bhc_rx_chan->scid == BLE_L2CAP_TEST_CID);
    ble_hs_unlock();

    /* Receive a starting fragment on a different channel.  The first fragment
     * should get discarded.
     */
    ble_hs_test_util_set_att_mtu(conn->bhc_handle, data_len);
    rc = ble_l2cap_test_util_rx_first_frag(2, 14, BLE_L2CAP_CID_ATT, data_len);
    TEST_ASSERT(rc == 0);

    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    TEST_ASSERT(conn->bhc_rx_chan != NULL &&
                conn->bhc_rx_chan->scid == BLE_L2CAP_CID_ATT);
    ble_hs_unlock();

    /* Terminate the connection.  The received fragments should get freed.
     * Mbuf leaks are tested in the post-test-case callback.
     */
    ble_hs_test_util_conn_disconnect(2);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_frag_timeout)
{
    int32_t ticks_from_now;
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    NULL, NULL);

    /* Ensure timer is not set. */
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT_FATAL(ticks_from_now == BLE_HS_FOREVER);

    /* Receive the first fragment of a multipart ACL data packet. */
    rc = ble_l2cap_test_util_rx_first_frag(2, 14, BLE_L2CAP_TEST_CID, 30);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure timer will expire in 30 seconds. */
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT(ticks_from_now == MYNEWT_VAL(BLE_L2CAP_RX_FRAG_TIMEOUT));

    /* Almost let the timer expire. */
    os_time_advance(MYNEWT_VAL(BLE_L2CAP_RX_FRAG_TIMEOUT) - 1);
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT(ticks_from_now == 1);

    /* Receive a second fragment. */
    rc = ble_l2cap_test_util_rx_next_frag(2, 14);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure timer got reset. */
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT(ticks_from_now == MYNEWT_VAL(BLE_L2CAP_RX_FRAG_TIMEOUT));

    /* Allow the timer to expire. */
    ble_hs_test_util_hci_ack_set_disconnect(0);
    os_time_advance(MYNEWT_VAL(BLE_L2CAP_RX_FRAG_TIMEOUT));
    ticks_from_now = ble_hs_conn_timer();
    TEST_ASSERT(ticks_from_now == BLE_HS_FOREVER);

    /* Ensure connection was terminated. */
    ble_hs_test_util_hci_verify_tx_disconnect(2, BLE_ERR_REM_USER_CONN_TERM);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*****************************************************************************
 * $unsolicited response                                                     *
 *****************************************************************************/

TEST_CASE_SELF(ble_l2cap_test_case_sig_unsol_rsp)
{
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    NULL, NULL);

    /* Receive an unsolicited response. */
    rc = ble_hs_test_util_rx_l2cap_update_rsp(2, 100, 0);
    TEST_ASSERT(rc == 0);

    /* Ensure we did not send anything in return. */
    TEST_ASSERT_FATAL(ble_hs_test_util_prev_tx_dequeue() == NULL);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/*****************************************************************************
 * $update                                                                   *
 *****************************************************************************/

static int
ble_l2cap_test_util_conn_cb(struct ble_gap_event *event, void *arg)
{
    int *accept;

    switch (event->type) {
    case BLE_GAP_EVENT_L2CAP_UPDATE_REQ:
        accept = arg;
        return !*accept;

    default:
        return 0;
    }
}

static void
ble_l2cap_test_util_peer_updates(int accept)
{
    struct ble_l2cap_sig_update_params l2cap_params;
    struct ble_gap_upd_params params;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    ble_l2cap_test_util_conn_cb,
                                    &accept);

    l2cap_params.itvl_min = 0x200;
    l2cap_params.itvl_max = 0x300;
    l2cap_params.slave_latency = 0;
    l2cap_params.timeout_multiplier = 0x500;
    ble_l2cap_test_util_rx_update_req(2, 1, &l2cap_params);

    /* Ensure an update response command got sent. */
    ble_hs_test_util_verify_tx_l2cap_update_rsp(1, !accept);

    if (accept) {
        params.itvl_min = 0x200;
        params.itvl_max = 0x300;
        params.latency = 0;
        params.supervision_timeout = 0x500;
        params.min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN;
        params.max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN;
        ble_l2cap_test_util_verify_tx_update_conn(&params);
    } else {
        /* Ensure no update got scheduled. */
        TEST_ASSERT(!ble_gap_dbg_update_active(2));
    }
}

static void
ble_l2cap_test_util_update_cb(uint16_t conn_handle, int status, void *arg)
{
    ble_l2cap_test_update_conn_handle = conn_handle;
    ble_l2cap_test_update_status = status;
    ble_l2cap_test_update_arg = arg;
}

static void
ble_l2cap_test_util_we_update(int peer_accepts)
{
    struct ble_l2cap_sig_update_params params;
    uint8_t id;
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    ble_l2cap_test_util_conn_cb, NULL);

    /* Only the slave can initiate the L2CAP connection update procedure. */
    ble_hs_atomic_conn_set_flags(2, BLE_HS_CONN_F_MASTER, 0);

    params.itvl_min = 0x200;
    params.itvl_max = 0x300;
    params.slave_latency = 0;
    params.timeout_multiplier = 0x100;
    rc = ble_l2cap_sig_update(2, &params, ble_l2cap_test_util_update_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure an update request got sent. */
    id = ble_hs_test_util_verify_tx_l2cap_update_req(&params);

    /* Receive response from peer. */
    rc = ble_hs_test_util_rx_l2cap_update_rsp(2, id, !peer_accepts);
    TEST_ASSERT(rc == 0);

    /* Ensure callback got called. */
    if (peer_accepts) {
        TEST_ASSERT(ble_l2cap_test_update_status == 0);
    } else {
        TEST_ASSERT(ble_l2cap_test_update_status == BLE_HS_EREJECT);
    }
    TEST_ASSERT(ble_l2cap_test_update_arg == NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_update_accept)
{
    ble_l2cap_test_util_peer_updates(1);
    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_update_reject)
{
    ble_l2cap_test_util_peer_updates(0);
    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_update_init_accept)
{
    ble_l2cap_test_util_we_update(1);
    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_update_init_reject)
{
    ble_l2cap_test_util_we_update(0);
    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_update_init_fail_master)
{
    struct ble_l2cap_sig_update_params params;
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    ble_l2cap_test_util_conn_cb, NULL);

    params.itvl_min = 0x200;
    params.itvl_max = 0x300;
    params.slave_latency = 0;
    params.timeout_multiplier = 0x100;
    rc = ble_l2cap_sig_update(2, &params, ble_l2cap_test_util_update_cb, NULL);
    TEST_ASSERT_FATAL(rc == BLE_HS_EINVAL);

    /* Ensure callback never called. */
    TEST_ASSERT(ble_l2cap_test_update_status == -1);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_update_init_fail_bad_id)
{
    struct ble_l2cap_sig_update_params params;
    uint8_t id;
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    ble_l2cap_test_util_conn_cb, NULL);

    /* Only the slave can initiate the L2CAP connection update procedure. */
    ble_hs_atomic_conn_set_flags(2, BLE_HS_CONN_F_MASTER, 0);

    params.itvl_min = 0x200;
    params.itvl_max = 0x300;
    params.slave_latency = 0;
    params.timeout_multiplier = 0x100;
    rc = ble_l2cap_sig_update(2, &params, ble_l2cap_test_util_update_cb, NULL);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure an update request got sent. */
    id = ble_hs_test_util_verify_tx_l2cap_update_req(&params);

    /* Receive response from peer with incorrect ID. */
    rc = ble_hs_test_util_rx_l2cap_update_rsp(2, id + 1, 0);
    TEST_ASSERT(rc == 0);

    /* Ensure callback did not get called. */
    TEST_ASSERT(ble_l2cap_test_update_status == -1);

    /* Receive response from peer with correct ID. */
    rc = ble_hs_test_util_rx_l2cap_update_rsp(2, id, 0);
    TEST_ASSERT(rc == 0);

    /* Ensure callback got called. */
    TEST_ASSERT(ble_l2cap_test_update_status == 0);
    TEST_ASSERT(ble_l2cap_test_update_arg == NULL);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

/* Test enum but first four events matches to events which L2CAP sends to
 * application. We need this in order to add additional SEND_DATA event for
 * testing
 */

enum {
    BLE_L2CAP_TEST_EVENT_COC_CONNECT = 0,
    BLE_L2CAP_TEST_EVENT_COC_DISCONNECT,
    BLE_L2CAP_TEST_EVENT_COC_ACCEPT,
    BLE_L2CAP_TEST_EVENT_COC_RECV_DATA,
    BLE_L2CAP_TEST_EVENT_COC_SEND_DATA,
};

struct event {
    uint8_t type;
    uint16_t early_error;
    uint16_t l2cap_status;
    uint16_t app_status;
    uint8_t handled;
    uint8_t *data;
    uint16_t data_len;
};

struct test_data {
    struct event event[3];
    uint16_t expected_num_of_ev;
    /* This we use to track number of events sent to application*/
    uint16_t event_cnt;
    /* This we use to track verified events (received or not) */
    uint16_t event_iter;
    uint16_t psm;
    uint16_t mtu;
    struct ble_l2cap_chan *chan;
};

static int
ble_l2cap_test_event(struct ble_l2cap_event *event, void *arg)
{
    struct test_data *t = arg;
    struct event *ev = &t->event[t->event_cnt++];
    struct os_mbuf *sdu_rx;

    assert(ev->type == event->type);
    ev->handled = 1;
    switch(event->type) {
    case BLE_L2CAP_EVENT_COC_CONNECTED:
        assert(ev->app_status == event->connect.status);
        t->chan = event->connect.chan;
        return 0;
    case BLE_L2CAP_EVENT_COC_DISCONNECTED:
        return 0;
    case BLE_L2CAP_EVENT_COC_ACCEPT:
        if (ev->app_status != 0) {
            return ev->app_status;
        }

        sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
        assert(sdu_rx != NULL);
        ble_l2cap_recv_ready(event->accept.chan, sdu_rx);

        return 0;

    case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
        sdu_rx = os_mbuf_pullup(event->receive.sdu_rx,
                                    OS_MBUF_PKTLEN(event->receive.sdu_rx));
        TEST_ASSERT(memcmp(sdu_rx->om_data, ev->data, ev->data_len) == 0);
        return 0;
    case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
        /* TODO Add tests for this */
        return 0;
    default:
        return 0;
    }
}

static uint16_t ble_l2cap_calculate_credits(uint16_t mtu, uint16_t mps)
{
    int credits;

    credits = mtu / mps;
    if (mtu % mps) {
        credits++;
    }

    return credits;
}

static void
ble_l2cap_test_coc_connect(struct test_data *t)
{
    struct ble_l2cap_sig_le_con_req req = {};
    struct ble_l2cap_sig_le_con_rsp rsp = {};
    struct os_mbuf *sdu_rx;
    struct event *ev = &t->event[t->event_iter++];
    uint8_t id;
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    ble_l2cap_test_util_conn_cb, NULL);

    sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    assert(sdu_rx != NULL);

    rc = ble_l2cap_sig_coc_connect(2, t->psm, t->mtu, sdu_rx,
                                   ble_l2cap_test_event, t);
    TEST_ASSERT_FATAL(rc == ev->early_error);

    if (rc != 0) {
        rc = os_mbuf_free_chain(sdu_rx);
        TEST_ASSERT_FATAL(rc == 0);
        return;
    }

    req.credits = htole16(
                        ble_l2cap_calculate_credits(t->mtu,
                                                    MYNEWT_VAL(BLE_L2CAP_COC_MPS)));
    req.mps = htole16(MYNEWT_VAL(BLE_L2CAP_COC_MPS));
    req.mtu = htole16(t->mtu);
    req.psm = htole16(t->psm);
    req.scid = htole16(current_cid++);

    /* Ensure an update request got sent. */
    id = ble_hs_test_util_verify_tx_l2cap_sig(
                                            BLE_L2CAP_SIG_OP_CREDIT_CONNECT_REQ,
                                            &req, sizeof(req));

    /* Use some different parameters for peer. Just keep mtu same for testing
     * only*/
    rsp.credits = htole16(10);
    rsp.dcid = htole16(current_cid);
    rsp.mps = htole16(MYNEWT_VAL(BLE_L2CAP_COC_MPS) + 16);
    rsp.mtu = htole16(t->mtu);
    rsp.result = htole16(ev->l2cap_status);

    rc = ble_hs_test_util_inject_rx_l2cap_sig(2,
                                              BLE_L2CAP_SIG_OP_CREDIT_CONNECT_RSP,
                                              id, &rsp, sizeof(rsp));
    TEST_ASSERT(rc == 0);

    /* Ensure callback got called. */
    TEST_ASSERT(ev->handled);
}

static void
ble_l2cap_test_coc_connect_by_peer(struct test_data *t)
{
    struct ble_l2cap_sig_le_con_req req = {};
    struct ble_l2cap_sig_le_con_rsp rsp = {};
    uint8_t id = 10;
    int rc;
    struct event *ev = &t->event[t->event_iter++];

    ble_l2cap_test_util_create_conn(2, ((uint8_t[]){1,2,3,4,5,6}),
                                    ble_l2cap_test_util_conn_cb, NULL);

    /* Use some different parameters for peer */
    req.credits = htole16(30);
    req.mps = htole16(MYNEWT_VAL(BLE_L2CAP_COC_MPS) + 16);
    req.mtu = htole16(t->mtu);
    req.psm = htole16(t->psm);
    req.scid = htole16(0x0040);

    /* Receive remote request*/
    rc = ble_hs_test_util_inject_rx_l2cap_sig(2,
                                              BLE_L2CAP_SIG_OP_CREDIT_CONNECT_REQ,
                                              id, &req, sizeof(req));
    TEST_ASSERT_FATAL(rc == 0);

    if (ev->type == BLE_L2CAP_EVENT_COC_ACCEPT) {
        /* Lets check if there is accept event */
        TEST_ASSERT(ev->handled);
        /* Ensure callback got called. */
        ev = &t->event[t->event_iter++];
    }

    if (ev->l2cap_status != 0) {
        rsp.result = htole16(ev->l2cap_status);
    } else {
        /* Receive response from peer.*/
        rsp.credits = htole16(
                            ble_l2cap_calculate_credits(t->mtu,
                                                        MYNEWT_VAL(BLE_L2CAP_COC_MPS)));
        rsp.dcid = current_cid++;
        rsp.mps = htole16(MYNEWT_VAL(BLE_L2CAP_COC_MPS));
        rsp.mtu = htole16(t->mtu);
    }

    /* Ensure we sent response. */
    TEST_ASSERT(id == ble_hs_test_util_verify_tx_l2cap_sig(
                                            BLE_L2CAP_SIG_OP_CREDIT_CONNECT_RSP,
                                            &rsp, sizeof(rsp)));

    if (ev->l2cap_status == 0) {
        TEST_ASSERT(ev->handled);
    } else {
        TEST_ASSERT(!ev->handled);
    }
}

static void
ble_l2cap_test_coc_disc(struct test_data *t)
{
    struct ble_l2cap_sig_disc_req req;
    struct event *ev = &t->event[t->event_iter++];
    uint8_t id;
    int rc;

    rc = ble_l2cap_sig_disconnect(t->chan);
    TEST_ASSERT_FATAL(rc == 0);

    req.dcid = htole16(t->chan->dcid);
    req.scid = htole16(t->chan->scid);

    /* Ensure an update request got sent. */
    id = ble_hs_test_util_verify_tx_l2cap_sig(BLE_L2CAP_SIG_OP_DISCONN_REQ,
                                                   &req, sizeof(req));

    /* Receive response from peer. Note it shall be same as request */
    rc = ble_hs_test_util_inject_rx_l2cap_sig(2, BLE_L2CAP_SIG_OP_DISCONN_RSP,
                                           id, &req, sizeof(req));
    TEST_ASSERT(rc == 0);

    /* Ensure callback got called. */
    TEST_ASSERT(ev->handled);
}

static void
ble_l2cap_test_coc_disc_by_peer(struct test_data *t)
{
    struct ble_l2cap_sig_disc_req req;
    struct event *ev = &t->event[t->event_iter++];
    uint8_t id = 10;
    int rc;

    /* Receive disconnect request from peer. Note that source cid
     * and destination cid are from peer perspective */
    req.dcid = htole16(t->chan->scid);
    req.scid = htole16(t->chan->dcid);

    rc = ble_hs_test_util_inject_rx_l2cap_sig(2, BLE_L2CAP_SIG_OP_DISCONN_REQ,
                                       id, &req, sizeof(req));
    TEST_ASSERT(rc == 0);

    /* Ensure callback got called. */
    TEST_ASSERT(ev->handled);

    /* Ensure an we sent back response. Note that payload is same as request,
     * lets reuse it */
    TEST_ASSERT(ble_hs_test_util_verify_tx_l2cap_sig(
                                        BLE_L2CAP_SIG_OP_DISCONN_RSP,
                                        &req, sizeof(req)) == id);
}

static void
ble_l2cap_test_coc_invalid_disc_by_peer(struct test_data *t)
{
    struct ble_l2cap_sig_disc_req req;
    uint8_t id = 10;
    int rc;
    struct event *ev = &t->event[t->event_iter++];

    /* Receive disconnect request from peer. Note that source cid
     * and destination cid are from peer perspective */
    req.dcid = htole16(t->chan->scid);
    req.scid = htole16(0);

    rc = ble_hs_test_util_inject_rx_l2cap_sig(2, BLE_L2CAP_SIG_OP_DISCONN_REQ,
                                       id, &req, sizeof(req));
    TEST_ASSERT(rc == 0);

    /* Ensure callback HAS NOT BEEN*/
    TEST_ASSERT(!ev->handled);
}

static void
ble_l2cap_test_coc_send_data(struct test_data *t)
{
    struct os_mbuf *sdu;
    struct os_mbuf *sdu_copy;
    struct event *ev = &t->event[t->event_iter++];
    int rc;

    /* Send data event is created only for testing.
     * Since application callback do caching of real stack event
     * and checks the type of the event, lets increase event counter here and
     * fake that this event is handled*/
    t->event_cnt++;

    sdu = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    assert(sdu != NULL);

    sdu_copy = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    assert(sdu_copy != NULL);

    rc = os_mbuf_append(sdu, ev->data, ev->data_len);
    TEST_ASSERT(rc == 0);

    rc = os_mbuf_append(sdu_copy, ev->data, ev->data_len);
    TEST_ASSERT(rc == 0);

    rc = ble_l2cap_send(t->chan, sdu);
    TEST_ASSERT(rc == ev->early_error);

    if (rc) {
        rc = os_mbuf_free(sdu);
        TEST_ASSERT_FATAL(rc == 0);

        rc = os_mbuf_free(sdu_copy);
        TEST_ASSERT_FATAL(rc == 0);
        return;
    }

    /* Add place for SDU len */
    sdu_copy = os_mbuf_prepend_pullup(sdu_copy, 2);
    assert(sdu_copy != NULL);
    put_le16(sdu_copy->om_data, ev->data_len);

    ble_hs_test_util_verify_tx_l2cap(sdu);

    rc = os_mbuf_free_chain(sdu_copy);
    TEST_ASSERT_FATAL(rc == 0);
}

static void
ble_l2cap_test_coc_recv_data(struct test_data *t)
{
    struct os_mbuf *sdu;
    int rc;
    struct event *ev = &t->event[t->event_iter++];

    sdu = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    assert(sdu != NULL);

    rc = os_mbuf_append(sdu, ev->data, ev->data_len);
    TEST_ASSERT(rc == 0);

    /* TODO  handle fragmentation */

    /* Add place for SDU len */
    sdu = os_mbuf_prepend_pullup(sdu, 2);
    assert(sdu != NULL);
    put_le16(sdu->om_data, ev->data_len);

    ble_hs_test_util_inject_rx_l2cap(2, t->chan->scid, sdu);
}

static void
ble_l2cap_test_set_chan_test_conf(uint16_t psm, uint16_t mtu,
                                  struct test_data *t)
{
    memset(t, 0, sizeof(*t));

    t->psm = psm;
    t->mtu = mtu;
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_conn_invalid_psm)
{
    struct test_data t;

    ble_l2cap_test_util_init();
    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 1;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[0].app_status = BLE_HS_ENOTSUP;
    t.event[0].l2cap_status = BLE_L2CAP_COC_ERR_UNKNOWN_LE_PSM;

    ble_l2cap_test_coc_connect(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_conn_out_of_resource)
{
    struct test_data t;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 1;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[0].app_status = BLE_HS_ENOMEM;
    t.event[0].l2cap_status = BLE_L2CAP_COC_ERR_NO_RESOURCES;

    ble_l2cap_test_coc_connect(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_conn_invalid_cid)
{
    struct test_data t;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 1;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[0].app_status = BLE_HS_EREJECT;
    t.event[0].l2cap_status = BLE_L2CAP_COC_ERR_INVALID_SOURCE_CID;

    ble_l2cap_test_coc_connect(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_conn_insuff_authen)
{
    struct test_data t;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 1;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[0].app_status = BLE_HS_EAUTHEN;
    t.event[0].l2cap_status = BLE_L2CAP_COC_ERR_INSUFFICIENT_AUTHEN;

    ble_l2cap_test_coc_connect(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_conn_insuff_author)
{
    struct test_data t;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 1;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[0].app_status = BLE_HS_EAUTHOR;
    t.event[0].l2cap_status = BLE_L2CAP_COC_ERR_INSUFFICIENT_AUTHOR;

    ble_l2cap_test_coc_connect(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_incoming_conn_invalid_psm)
{
    struct test_data t;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 1;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[0].l2cap_status = BLE_L2CAP_COC_ERR_UNKNOWN_LE_PSM;

    ble_l2cap_test_coc_connect_by_peer(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_incoming_conn_rejected_by_app)
{
    struct test_data t;
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 2;

    t.event[0].type = BLE_L2CAP_EVENT_COC_ACCEPT;
    t.event[0].app_status = BLE_HS_ENOMEM;

    /* This event will not be called and test is going to verify it*/
    t.event[1].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[1].l2cap_status = BLE_L2CAP_COC_ERR_NO_RESOURCES;

    /* Register server */
    rc = ble_l2cap_create_server(t.psm, BLE_L2CAP_TEST_COC_MTU,
                                 ble_l2cap_test_event, &t);
    TEST_ASSERT(rc == 0);

    ble_l2cap_test_coc_connect_by_peer(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    /* In this test case, L2CAP channel is created and once application rejects
     * connection, channel is destroyed. In such case CID for channel has been
     * used and we need to increase current_cid. */
    current_cid++;

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_incoming_conn_success)
{
    struct test_data t;
    int rc;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 2;

    t.event[0].type = BLE_L2CAP_EVENT_COC_ACCEPT;
    t.event[1].type = BLE_L2CAP_EVENT_COC_CONNECTED;

    /* Register server */
    rc = ble_l2cap_create_server(t.psm, BLE_L2CAP_TEST_COC_MTU,
                                 ble_l2cap_test_event, &t);
    TEST_ASSERT(rc == 0);

    ble_l2cap_test_coc_connect_by_peer(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_disconnect_succeed)
{
    struct test_data t;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t. expected_num_of_ev = 2;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[0].app_status = 0;
    t.event[0].l2cap_status = BLE_L2CAP_COC_ERR_CONNECTION_SUCCESS;
    t.event[1].type = BLE_L2CAP_EVENT_COC_DISCONNECTED;

    ble_l2cap_test_coc_connect(&t);
    ble_l2cap_test_coc_disc(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_incoming_disconnect_succeed)
{
    struct test_data t;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 2;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[0].app_status = 0;
    t.event[0].l2cap_status = BLE_L2CAP_COC_ERR_CONNECTION_SUCCESS;
    t.event[1].type = BLE_L2CAP_EVENT_COC_DISCONNECTED;

    ble_l2cap_test_coc_connect(&t);
    ble_l2cap_test_coc_disc_by_peer(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_sig_coc_incoming_disconnect_failed)
{
    struct test_data t;

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 2;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[0].app_status = 0;
    t.event[0].l2cap_status = BLE_L2CAP_COC_ERR_CONNECTION_SUCCESS;
    t.event[1].type = BLE_L2CAP_EVENT_COC_DISCONNECTED;

    ble_l2cap_test_coc_connect(&t);
    ble_l2cap_test_coc_invalid_disc_by_peer(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_coc_send_data_succeed)
{
    struct test_data t;
    uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 3;

    t.event[0].type = BLE_L2CAP_TEST_EVENT_COC_CONNECT;
    t.event[1].type = BLE_L2CAP_TEST_EVENT_COC_SEND_DATA;
    t.event[1].data = buf;
    t.event[1].data_len = sizeof(buf);
    t.event[2].type = BLE_L2CAP_TEST_EVENT_COC_DISCONNECT;

    ble_l2cap_test_coc_connect(&t);
    ble_l2cap_test_coc_send_data(&t);
    ble_l2cap_test_coc_disc(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_coc_send_data_failed_too_big_sdu)
{
    struct test_data t = {};
    uint16_t small_mtu = 27;
    uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM, small_mtu, &t);
    t.expected_num_of_ev = 3;

    t.event[0].type = BLE_L2CAP_TEST_EVENT_COC_CONNECT;
    t.event[1].type = BLE_L2CAP_TEST_EVENT_COC_SEND_DATA;
    t.event[1].data = buf;
    t.event[1].data_len = sizeof(buf);
    t.event[1].early_error = BLE_HS_EBADDATA;
    t.event[2].type = BLE_L2CAP_TEST_EVENT_COC_DISCONNECT;

    ble_l2cap_test_coc_connect(&t);
    ble_l2cap_test_coc_send_data(&t);
    ble_l2cap_test_coc_disc(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_l2cap_test_case_coc_recv_data_succeed)
{
    struct test_data t = {};
    uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    ble_l2cap_test_util_init();

    ble_l2cap_test_set_chan_test_conf(BLE_L2CAP_TEST_PSM,
                                      BLE_L2CAP_TEST_COC_MTU, &t);
    t.expected_num_of_ev = 3;

    t.event[0].type = BLE_L2CAP_EVENT_COC_CONNECTED;
    t.event[1].type = BLE_L2CAP_EVENT_COC_DATA_RECEIVED;
    t.event[1].data = buf;
    t.event[1].data_len = sizeof(buf);
    t.event[2].type = BLE_L2CAP_EVENT_COC_DISCONNECTED;

    ble_l2cap_test_coc_connect(&t);
    ble_l2cap_test_coc_recv_data(&t);
    ble_l2cap_test_coc_disc(&t);

    TEST_ASSERT(t.expected_num_of_ev == t.event_iter);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_l2cap_test_suite)
{
    ble_l2cap_test_case_bad_header();
    ble_l2cap_test_case_bad_handle();
    ble_l2cap_test_case_frag_single();
    ble_l2cap_test_case_frag_multiple();
    ble_l2cap_test_case_frag_channels();
    ble_l2cap_test_case_frag_timeout();
    ble_l2cap_test_case_sig_unsol_rsp();
    ble_l2cap_test_case_sig_update_accept();
    ble_l2cap_test_case_sig_update_reject();
    ble_l2cap_test_case_sig_update_init_accept();
    ble_l2cap_test_case_sig_update_init_reject();
    ble_l2cap_test_case_sig_update_init_fail_master();
    ble_l2cap_test_case_sig_update_init_fail_bad_id();
    ble_l2cap_test_case_sig_coc_conn_invalid_psm();
    ble_l2cap_test_case_sig_coc_conn_out_of_resource();
    ble_l2cap_test_case_sig_coc_conn_invalid_cid();
    ble_l2cap_test_case_sig_coc_conn_insuff_authen();
    ble_l2cap_test_case_sig_coc_conn_insuff_author();
    ble_l2cap_test_case_sig_coc_incoming_conn_invalid_psm();
    ble_l2cap_test_case_sig_coc_incoming_conn_rejected_by_app();
    ble_l2cap_test_case_sig_coc_incoming_conn_success();
    ble_l2cap_test_case_sig_coc_disconnect_succeed();
    ble_l2cap_test_case_sig_coc_incoming_disconnect_succeed();
    ble_l2cap_test_case_sig_coc_incoming_disconnect_failed();
    ble_l2cap_test_case_coc_send_data_succeed();
    ble_l2cap_test_case_coc_send_data_failed_too_big_sdu();
    ble_l2cap_test_case_coc_recv_data_succeed();
}
