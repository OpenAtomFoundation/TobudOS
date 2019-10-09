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
#include <string.h>
#include <errno.h>
#include "testutil/testutil.h"
#include "nimble/hci_common.h"
#include "nimble/nimble_opt.h"
#include "host/ble_sm.h"
#include "ble_hs_test.h"
#include "host/ble_hs_id.h"
#include "ble_hs_test_util.h"
#include "ble_sm_test_util.h"

int ble_sm_test_gap_event_type;
int ble_sm_test_gap_status;
struct ble_gap_sec_state ble_sm_test_sec_state;
static struct ble_gap_passkey_params ble_sm_test_ioact;

static struct {
    /** Handle reported in previous repeat pairing event. */
    struct ble_gap_repeat_pairing rp;

    struct ble_sm_test_params params;

    /** What the callback should return this time. */
    int rc;

    /** What the callback should return next time. */
    int next_rc;

    /**
     * Whether the callback should erase the conflicting entry before retrying.
     */
    int erase_on_retry;

    /** The number of times the event got reported. */
    int num_calls;
} ble_sm_test_repeat_pairing;

struct ble_sm_test_util_entity {
    uint8_t addr_type;
    uint8_t id_addr_type;
    uint8_t *id_addr;
    uint8_t *rpa;

    struct ble_sm_pair_cmd *pair_cmd;
    struct ble_sm_pair_confirm *confirms;
    struct ble_sm_pair_random *randoms;
    struct ble_sm_id_info *id_info;
    struct ble_sm_id_addr_info *id_addr_info;
    struct ble_sm_sign_info *sign_info;
    uint8_t *ltk;

    uint8_t key_dist;

    /*** Secure connections fields. */
    struct ble_sm_public_key *public_key;
    struct ble_sm_dhkey_check *dhkey_check;

    /*** Legacy fields. */
    struct ble_sm_enc_info *enc_info;
    struct ble_sm_master_id *master_id;
    uint64_t rand_num;
    uint16_t ediv;
};

static void ble_sm_test_util_repeat_pairing(struct ble_sm_test_params *params,
                                            int sc);

#define BLE_SM_TEST_UTIL_HCI_HDR(handle, pb, len) \
    ((struct hci_data_hdr) {                            \
        .hdh_handle_pb_bc = ((handle)  << 0) |          \
                            ((pb)      << 12),          \
        .hdh_len = (len)                                \
    })

static void
ble_sm_pair_cmd_parse(void *payload, int len, struct ble_sm_pair_cmd *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_pair_cmd));

    u8ptr = payload;
    cmd->io_cap = u8ptr[0];
    cmd->oob_data_flag = u8ptr[1];
    cmd->authreq = u8ptr[2];
    cmd->max_enc_key_size = u8ptr[3];
    cmd->init_key_dist = u8ptr[4];
    cmd->resp_key_dist = u8ptr[5];
}

static void
ble_sm_pair_cmd_write(void *payload, int len, int is_req,
                      struct ble_sm_pair_cmd *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_pair_cmd));

    u8ptr = payload;
    u8ptr[0] = is_req ? BLE_SM_OP_PAIR_REQ : BLE_SM_OP_PAIR_RSP;
    u8ptr[1] = cmd->io_cap;
    u8ptr[2] = cmd->oob_data_flag;
    u8ptr[3] = cmd->authreq;
    u8ptr[4] = cmd->max_enc_key_size;
    u8ptr[5] = cmd->init_key_dist;
    u8ptr[6] = cmd->resp_key_dist;
}

static void
ble_sm_pair_confirm_parse(void *payload, int len,
                          struct ble_sm_pair_confirm *cmd)
{
    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_pair_confirm));
    memcpy(cmd->value, payload, sizeof cmd->value);
}

static void
ble_sm_pair_confirm_write(void *payload, int len,
                          struct ble_sm_pair_confirm *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_pair_confirm));

    u8ptr = payload;

    u8ptr[0] = BLE_SM_OP_PAIR_CONFIRM;
    memcpy(u8ptr + sizeof(struct ble_sm_hdr), cmd->value, sizeof cmd->value);
}

static void
ble_sm_pair_random_parse(void *payload, int len,
                         struct ble_sm_pair_random *cmd)
{
    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_pair_random));
    memcpy(cmd->value, payload, sizeof cmd->value);
}

static void
ble_sm_pair_random_write(void *payload, int len,
                         struct ble_sm_pair_random *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_pair_random));

    u8ptr = payload;

    u8ptr[0] = BLE_SM_OP_PAIR_RANDOM;
    memcpy(u8ptr + sizeof(struct ble_sm_hdr), cmd->value, sizeof cmd->value);
}

static void
ble_sm_pair_fail_parse(void *payload, int len, struct ble_sm_pair_fail *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_pair_fail));

    u8ptr = payload;
    cmd->reason = u8ptr[0];
}

static void
ble_sm_enc_info_parse(void *payload, int len, struct ble_sm_enc_info *cmd)
{
    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_enc_info));

    memcpy(cmd->ltk, payload, sizeof cmd->ltk);
}

static void
ble_sm_enc_info_write(void *payload, int len, struct ble_sm_enc_info *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_enc_info));

    u8ptr = payload;

    u8ptr[0] = BLE_SM_OP_ENC_INFO;
    memcpy(u8ptr + 1, cmd->ltk, sizeof cmd->ltk);
}

static void
ble_sm_master_id_parse(void *payload, int len, struct ble_sm_master_id *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_master_id));

    u8ptr = payload;

    cmd->ediv = get_le16(u8ptr);
    cmd->rand_val = get_le64(u8ptr + 2);
}

static void
ble_sm_master_id_write(void *payload, int len, struct ble_sm_master_id *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_master_id));

    u8ptr = payload;

    u8ptr[0] = BLE_SM_OP_MASTER_ID;
    put_le16(u8ptr + 1, cmd->ediv);
    put_le64(u8ptr + 3, cmd->rand_val);
}

static void
ble_sm_id_info_parse(void *payload, int len, struct ble_sm_id_info *cmd)
{
    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_id_info));

    memcpy(cmd->irk, payload, 16);
}

static void
ble_sm_id_info_write(void *payload, int len, struct ble_sm_id_info *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_id_info));

    u8ptr = payload;

    u8ptr[0] = BLE_SM_OP_IDENTITY_INFO;
    memcpy(u8ptr + sizeof(struct ble_sm_hdr), cmd->irk, sizeof cmd->irk);
}

static void
ble_sm_id_addr_info_parse(void *payload, int len,
                          struct ble_sm_id_addr_info *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_id_addr_info));

    u8ptr = payload;

    cmd->addr_type = *u8ptr;
    memcpy(cmd->bd_addr, u8ptr + 1, 6);
}

static void
ble_sm_id_addr_info_write(void *payload, int len,
                          struct ble_sm_id_addr_info *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_id_addr_info));

    u8ptr = payload;

    u8ptr[0] = BLE_SM_OP_IDENTITY_ADDR_INFO;
    u8ptr[1] = cmd->addr_type;
    memcpy(u8ptr + 2, cmd->bd_addr, sizeof cmd->bd_addr);
}

static void
ble_sm_sign_info_parse(void *payload, int len, struct ble_sm_sign_info *cmd)
{
    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_sign_info));

    memcpy(cmd->sig_key, payload, 16);
}

static void
ble_sm_sign_info_write(void *payload, int len, struct ble_sm_sign_info *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_sign_info));

    u8ptr = payload;

    u8ptr[0] = BLE_SM_OP_SIGN_INFO;
    memcpy(u8ptr + sizeof(struct ble_sm_hdr),
           cmd->sig_key, sizeof cmd->sig_key);
}

static void
ble_sm_sec_req_parse(void *payload, int len, struct ble_sm_sec_req *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_sec_req));

    u8ptr = payload;
    cmd->authreq = *u8ptr;
}

static void
ble_sm_sec_req_write(void *payload, int len, struct ble_sm_sec_req *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_sec_req));

    u8ptr = payload;

    u8ptr[0] = BLE_SM_OP_SEC_REQ;
    u8ptr[1] = cmd->authreq;
}

static void
ble_sm_public_key_parse(void *payload, int len, struct ble_sm_public_key *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_public_key));

    u8ptr = payload;

    memcpy(cmd->x, u8ptr, sizeof cmd->x);
    u8ptr += sizeof cmd->x;

    memcpy(cmd->y, u8ptr, sizeof cmd->y);
    u8ptr += sizeof cmd->y;
}

static void
ble_sm_public_key_write(void *payload, int len, struct ble_sm_public_key *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_public_key));

    u8ptr = payload;

    *u8ptr = BLE_SM_OP_PAIR_PUBLIC_KEY;
    u8ptr++;

    memcpy(u8ptr, cmd->x, sizeof cmd->x);
    memcpy(u8ptr + 32, cmd->y, sizeof cmd->y);
}

static void
ble_sm_dhkey_check_parse(void *payload, int len,
                         struct ble_sm_dhkey_check *cmd)
{
    BLE_HS_DBG_ASSERT(len >= sizeof(struct ble_sm_dhkey_check));

    memcpy(cmd->value, payload, sizeof cmd->value);
}

static void
ble_sm_dhkey_check_write(void *payload, int len,
                         struct ble_sm_dhkey_check *cmd)
{
    uint8_t *u8ptr;

    BLE_HS_DBG_ASSERT(
        len >= sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_dhkey_check));

    u8ptr = payload;

    *u8ptr = BLE_SM_OP_PAIR_DHKEY_CHECK;
    u8ptr++;

    memcpy(u8ptr, cmd->value, sizeof cmd->value);
}

void
ble_sm_test_util_init(void)
{
    ble_hs_test_util_init();

    ble_sm_test_gap_event_type = -1;
    ble_sm_test_gap_status = -1;
    memset(&ble_sm_test_repeat_pairing, 0, sizeof ble_sm_test_repeat_pairing);
    ble_sm_test_repeat_pairing.rp.conn_handle = BLE_HS_CONN_HANDLE_NONE;

    memset(&ble_sm_test_ioact, 0, sizeof ble_sm_test_ioact);
    memset(&ble_sm_test_sec_state, 0xff, sizeof ble_sm_test_sec_state);
}

static void
ble_sm_test_util_params_to_entity(struct ble_sm_test_params *params,
                                  int initiator,
                                  struct ble_sm_test_util_entity *out_entity)
{
    int sc;

    memset(out_entity, 0, sizeof *out_entity);

    sc = params->pair_req.authreq & BLE_SM_PAIR_AUTHREQ_SC &&
         params->pair_rsp.authreq & BLE_SM_PAIR_AUTHREQ_SC;

    if (initiator) {
        out_entity->key_dist = params->pair_rsp.init_key_dist;

        out_entity->addr_type = params->init_addr_type;
        out_entity->id_addr = params->init_id_addr;
        out_entity->rpa = params->init_rpa;

        out_entity->pair_cmd = &params->pair_req;
        out_entity->confirms = params->confirm_req;
        out_entity->randoms = params->random_req;
        out_entity->id_info = &params->id_info_rsp;
        out_entity->id_addr_info = &params->id_addr_info_rsp;
        out_entity->sign_info = &params->sign_info_rsp;

        if (sc) {
            out_entity->ltk = params->ltk;
            out_entity->public_key = &params->public_key_req;
            out_entity->dhkey_check = &params->dhkey_check_req;
        } else {
            out_entity->enc_info = &params->enc_info_rsp;
            out_entity->master_id = &params->master_id_rsp;
            if (out_entity->key_dist & BLE_SM_PAIR_KEY_DIST_ENC) {
                out_entity->rand_num = params->master_id_rsp.rand_val;
                out_entity->ediv = params->master_id_rsp.ediv;
                out_entity->ltk = params->enc_info_rsp.ltk;
            }
        }
    } else {
        out_entity->key_dist = params->pair_rsp.resp_key_dist;

        out_entity->addr_type = params->resp_addr_type;
        out_entity->id_addr = params->resp_id_addr;
        out_entity->rpa = params->resp_rpa;

        out_entity->pair_cmd = &params->pair_rsp;
        out_entity->confirms = params->confirm_rsp;
        out_entity->randoms = params->random_rsp;
        out_entity->id_info = &params->id_info_req;
        out_entity->id_addr_info = &params->id_addr_info_req;
        out_entity->sign_info = &params->sign_info_req;

        if (sc) {
            out_entity->ltk = params->ltk;
            out_entity->public_key = &params->public_key_rsp;
            out_entity->dhkey_check = &params->dhkey_check_rsp;
        } else {
            out_entity->enc_info = &params->enc_info_req;
            out_entity->master_id = &params->master_id_req;
            if (out_entity->key_dist & BLE_SM_PAIR_KEY_DIST_ENC) {
                out_entity->rand_num = params->master_id_req.rand_val;
                out_entity->ediv = params->master_id_req.ediv;
                out_entity->ltk = params->enc_info_req.ltk;
            }
        }
    }

    out_entity->id_addr_type =
        ble_hs_misc_addr_type_to_id(out_entity->addr_type);
}

static void
ble_sm_test_util_params_to_entities(struct ble_sm_test_params *params,
                                    int we_are_initiator,
                                    struct ble_sm_test_util_entity *out_us,
                                    struct ble_sm_test_util_entity *out_peer)
{
    ble_sm_test_util_params_to_entity(params, we_are_initiator, out_us);
    ble_sm_test_util_params_to_entity(params, !we_are_initiator, out_peer);
}

static void
ble_sm_test_util_init_good(struct ble_sm_test_params *params,
                           int we_are_initiator,
                           struct ble_hs_conn **out_conn,
                           struct ble_sm_test_util_entity *out_us,
                           struct ble_sm_test_util_entity *out_peer)
{
    struct ble_hs_conn *conn;

    ble_sm_test_util_init();

    ble_sm_test_util_params_to_entities(params, we_are_initiator,
                                        out_us, out_peer);

    ble_hs_cfg.sm_io_cap = out_us->pair_cmd->io_cap;
    ble_hs_cfg.sm_oob_data_flag = out_us->pair_cmd->oob_data_flag;
    ble_hs_cfg.sm_bonding = !!(out_us->pair_cmd->authreq &
                               BLE_SM_PAIR_AUTHREQ_BOND);
    ble_hs_cfg.sm_mitm = !!(out_us->pair_cmd->authreq &
                            BLE_SM_PAIR_AUTHREQ_MITM);
    ble_hs_cfg.sm_sc = !!(out_us->pair_cmd->authreq &
                          BLE_SM_PAIR_AUTHREQ_SC);
    ble_hs_cfg.sm_keypress = !!(out_us->pair_cmd->authreq &
                                BLE_SM_PAIR_AUTHREQ_KEYPRESS);

    if (we_are_initiator) {
        ble_hs_cfg.sm_our_key_dist = out_us->pair_cmd->init_key_dist;
        ble_hs_cfg.sm_their_key_dist = out_us->pair_cmd->resp_key_dist;
    } else {
        ble_hs_cfg.sm_our_key_dist = out_us->pair_cmd->resp_key_dist;
        ble_hs_cfg.sm_their_key_dist = out_us->pair_cmd->init_key_dist;
    }

    ble_hs_id_set_pub(out_us->id_addr);
    ble_sm_dbg_set_next_pair_rand(out_us->randoms[0].value);
    ble_sm_dbg_set_next_ediv(out_us->ediv);
    ble_sm_dbg_set_next_master_id_rand(out_us->rand_num);
    ble_sm_dbg_set_next_ltk(out_us->ltk);
    ble_hs_test_util_set_our_irk(out_us->id_info->irk, 0, 0);
    ble_sm_dbg_set_next_csrk(out_us->sign_info->sig_key);

    if (out_us->public_key != NULL) {
        ble_sm_dbg_set_sc_keys((uint8_t *)out_us->public_key,
                               params->our_priv_key);
    }

    ble_hs_test_util_create_rpa_conn(2, out_us->addr_type, out_us->rpa,
                                     out_peer->addr_type,
                                     out_peer->id_addr, out_peer->rpa,
                                     BLE_HS_TEST_CONN_FEAT_ALL,
                                     ble_sm_test_util_conn_cb,
                                     NULL);

    /* This test code and modifies the connection object after unlocking
     * the host mutex.  It is not OK for real code to do this, but this test
     * can assume the connection list is unchanging.
     */
    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    ble_hs_unlock();

    if (!we_are_initiator) {
        /* Peer is the initiator so we must be the slave. */
        conn->bhc_flags &= ~BLE_HS_CONN_F_MASTER;
    }

    if (out_conn != NULL) {
        *out_conn = conn;
    }
}

static int
ble_sm_test_util_repeat_pairing_cb(const struct ble_gap_repeat_pairing *rp)
{
    struct ble_store_value_sec value_sec;
    struct ble_store_key_sec key_sec;
    struct ble_gap_conn_desc desc;
    int rc;

    TEST_ASSERT_FATAL(rp->conn_handle != BLE_HS_CONN_HANDLE_NONE);

    ble_sm_test_repeat_pairing.num_calls++;

    rc = ble_gap_conn_find(rp->conn_handle, &desc);
    TEST_ASSERT_FATAL(rc == 0);

    memset(&key_sec, 0, sizeof key_sec);
    key_sec.peer_addr = desc.peer_id_addr;
    rc = ble_store_read_peer_sec(&key_sec, &value_sec);
    TEST_ASSERT_FATAL(rc == 0);

    /* Verify current bond is reported correctly. */
    TEST_ASSERT(rp->cur_key_size == value_sec.key_size);
    TEST_ASSERT(rp->cur_authenticated == value_sec.authenticated);
    TEST_ASSERT(rp->cur_sc == value_sec.sc);

    /* Verify new pairing request is reported correctly. */
    TEST_ASSERT(
        rp->new_key_size ==
        min(ble_sm_test_repeat_pairing.params.pair_req.max_enc_key_size,
            ble_sm_test_repeat_pairing.params.pair_rsp.max_enc_key_size));
    TEST_ASSERT(
        rp->new_authenticated ==
            !!(ble_sm_test_repeat_pairing.params.passkey_info.passkey.action));
    TEST_ASSERT(
        rp->new_sc ==
            ((ble_sm_test_repeat_pairing.params.pair_req.authreq &
              BLE_SM_PAIR_AUTHREQ_SC)
                &&
             (ble_sm_test_repeat_pairing.params.pair_rsp.authreq &
              BLE_SM_PAIR_AUTHREQ_SC)));
    TEST_ASSERT(
        rp->new_bonding ==
            ((ble_sm_test_repeat_pairing.params.pair_req.authreq &
              BLE_SM_PAIR_AUTHREQ_BOND)
                &&
             (ble_sm_test_repeat_pairing.params.pair_rsp.authreq &
              BLE_SM_PAIR_AUTHREQ_BOND)));

    if (ble_sm_test_repeat_pairing.rp.conn_handle ==
        BLE_HS_CONN_HANDLE_NONE) {

        ble_sm_test_repeat_pairing.rp.conn_handle = rp->conn_handle;
    } else {
        /* Ensure the correct connection handle gets reported each time. */
        TEST_ASSERT(rp->conn_handle ==
                    ble_sm_test_repeat_pairing.rp.conn_handle);
    }

    ble_sm_test_repeat_pairing.rp = *rp;

    if (ble_sm_test_repeat_pairing.rc == BLE_GAP_REPEAT_PAIRING_RETRY &&
        ble_sm_test_repeat_pairing.erase_on_retry) {

        rc = ble_gap_conn_find(rp->conn_handle, &desc);
        TEST_ASSERT_FATAL(rc == 0);

        rc = ble_store_util_delete_peer(&desc.peer_id_addr);
        TEST_ASSERT_FATAL(rc == 0);
    }

    rc = ble_sm_test_repeat_pairing.rc;
    ble_sm_test_repeat_pairing.rc = ble_sm_test_repeat_pairing.next_rc;

    return rc;
}

int
ble_sm_test_util_conn_cb(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_ENC_CHANGE:
        ble_sm_test_gap_status = event->enc_change.status;

        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        TEST_ASSERT_FATAL(rc == 0);
        ble_sm_test_sec_state = desc.sec_state;
        rc = 0;
        break;

    case BLE_GAP_EVENT_PASSKEY_ACTION:
        ble_sm_test_ioact = event->passkey.params;
        rc = 0;
        break;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        rc = ble_sm_test_util_repeat_pairing_cb(&event->repeat_pairing);
        break;

    default:
        return 0;
    }

    ble_sm_test_gap_event_type = event->type;
    return rc;
}

static void
ble_sm_test_util_rx_pair_cmd(uint16_t conn_handle, uint8_t op,
                             struct ble_sm_pair_cmd *cmd,
                             int rx_status)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ +
        sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_pair_cmd));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_pair_cmd);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_pair_cmd_write(v, payload_len, op == BLE_SM_OP_PAIR_REQ, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT(rc == rx_status);
}

static void
ble_sm_test_util_rx_pair_req(uint16_t conn_handle,
                             struct ble_sm_pair_cmd *req,
                             int rx_status)
{
    ble_sm_test_util_rx_pair_cmd(conn_handle, BLE_SM_OP_PAIR_REQ,
                                 req, rx_status);
}

static void
ble_sm_test_util_rx_pair_rsp(uint16_t conn_handle, struct ble_sm_pair_cmd *rsp,
                             int rx_status)
{
    ble_sm_test_util_rx_pair_cmd(conn_handle, BLE_SM_OP_PAIR_RSP,
                                 rsp, rx_status);
}

static void
ble_sm_test_util_rx_confirm(uint16_t conn_handle,
                            struct ble_sm_pair_confirm *cmd)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ +
        sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_pair_confirm));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len =
        sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_pair_confirm);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_pair_confirm_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == 0);
}

static void
ble_sm_test_util_rx_random(uint16_t conn_handle,
                           struct ble_sm_pair_random *cmd,
                           int exp_status)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_pair_random));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_pair_random);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_pair_random_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == exp_status);
}

void
ble_sm_test_util_rx_sec_req(uint16_t conn_handle, struct ble_sm_sec_req *cmd,
                            int exp_status)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_sec_req));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_sec_req);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_sec_req_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == exp_status);
}

static void
ble_sm_test_util_rx_public_key(uint16_t conn_handle,
                               struct ble_sm_public_key *cmd)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_public_key));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_public_key);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_public_key_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == 0);
}

static void
ble_sm_test_util_rx_dhkey_check(uint16_t conn_handle,
                                struct ble_sm_dhkey_check *cmd,
                                int exp_status)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_dhkey_check));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_dhkey_check);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_dhkey_check_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == exp_status);
}

static void
ble_sm_test_util_rx_enc_info(uint16_t conn_handle,
                             struct ble_sm_enc_info *cmd,
                             int exp_status)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_enc_info));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_enc_info);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_enc_info_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == exp_status);
}

static void
ble_sm_test_util_rx_master_id(uint16_t conn_handle,
                             struct ble_sm_master_id *cmd,
                             int exp_status)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_master_id));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_master_id);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_master_id_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == exp_status);
}

static void
ble_sm_test_util_rx_id_info(uint16_t conn_handle,
                            struct ble_sm_id_info *cmd,
                            int exp_status)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_id_info));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_id_info);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_id_info_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == exp_status);
}

static void
ble_sm_test_util_rx_id_addr_info(uint16_t conn_handle,
                                 struct ble_sm_id_addr_info *cmd,
                                 int exp_status)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_id_addr_info));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_id_addr_info);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_id_addr_info_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == exp_status);
}

static void
ble_sm_test_util_rx_sign_info(uint16_t conn_handle,
                              struct ble_sm_sign_info *cmd,
                              int exp_status)
{
    struct hci_data_hdr hci_hdr;
    struct os_mbuf *om;
    void *v;
    int payload_len;
    int rc;

    hci_hdr = BLE_SM_TEST_UTIL_HCI_HDR(
        2, BLE_HCI_PB_FIRST_FLUSH,
        BLE_L2CAP_HDR_SZ + sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_sign_info));

    om = ble_hs_mbuf_l2cap_pkt();
    TEST_ASSERT_FATAL(om != NULL);

    payload_len = sizeof(struct ble_sm_hdr) + sizeof(struct ble_sm_sign_info);

    v = os_mbuf_extend(om, payload_len);
    TEST_ASSERT_FATAL(v != NULL);

    ble_sm_sign_info_write(v, payload_len, cmd);

    rc = ble_hs_test_util_l2cap_rx_first_frag(conn_handle, BLE_L2CAP_CID_SM,
                                              &hci_hdr, om);
    TEST_ASSERT_FATAL(rc == exp_status);
}

static struct os_mbuf *
ble_sm_test_util_verify_tx_hdr(uint8_t sm_op, uint16_t payload_len)
{
    struct os_mbuf *om;

    om = ble_hs_test_util_prev_tx_dequeue_pullup();
    TEST_ASSERT_FATAL(om != NULL);

    TEST_ASSERT(OS_MBUF_PKTLEN(om) == sizeof(struct ble_sm_hdr) + payload_len);
    TEST_ASSERT_FATAL(om->om_data[0] == sm_op);

    om->om_data += sizeof(struct ble_sm_hdr);
    om->om_len -= sizeof(struct ble_sm_hdr);

    return om;
}

static void
ble_sm_test_util_verify_tx_pair_cmd(
    uint8_t op,
    struct ble_sm_pair_cmd *exp_cmd)
{
    struct ble_sm_pair_cmd cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(op, sizeof(struct ble_sm_pair_cmd));
    ble_sm_pair_cmd_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(cmd.io_cap == exp_cmd->io_cap);
    TEST_ASSERT(cmd.oob_data_flag == exp_cmd->oob_data_flag);
    TEST_ASSERT(cmd.authreq == exp_cmd->authreq);
    TEST_ASSERT(cmd.max_enc_key_size == exp_cmd->max_enc_key_size);
    TEST_ASSERT(cmd.init_key_dist == exp_cmd->init_key_dist);
    TEST_ASSERT(cmd.resp_key_dist == exp_cmd->resp_key_dist);
}

static void
ble_sm_test_util_verify_tx_pair_req(
    struct ble_sm_pair_cmd *exp_req)
{
    ble_sm_test_util_verify_tx_pair_cmd(BLE_SM_OP_PAIR_REQ,
                                              exp_req);
}

static void
ble_sm_test_util_verify_tx_pair_rsp(
    struct ble_sm_pair_cmd *exp_rsp)
{
    ble_sm_test_util_verify_tx_pair_cmd(BLE_SM_OP_PAIR_RSP,
                                              exp_rsp);
}

static void
ble_sm_test_util_verify_tx_pair_confirm(
    struct ble_sm_pair_confirm *exp_cmd)
{
    struct ble_sm_pair_confirm cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_PAIR_CONFIRM, sizeof(cmd));
    ble_sm_pair_confirm_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(memcmp(cmd.value, exp_cmd->value, 16) == 0);
}

static void
ble_sm_test_util_verify_tx_pair_random(
    struct ble_sm_pair_random *exp_cmd)
{
    struct ble_sm_pair_random cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_PAIR_RANDOM,
                                        sizeof(struct ble_sm_pair_random));
    ble_sm_pair_random_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(memcmp(cmd.value, exp_cmd->value, 16) == 0);
}

static void
ble_sm_test_util_verify_tx_public_key(
    struct ble_sm_public_key *exp_cmd)
{
    struct ble_sm_public_key cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_PAIR_PUBLIC_KEY,
                                        sizeof(struct ble_sm_public_key));
    ble_sm_public_key_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(memcmp(cmd.x, exp_cmd->x, sizeof cmd.x) == 0);
    TEST_ASSERT(memcmp(cmd.y, exp_cmd->y, sizeof cmd.y) == 0);
}

static void
ble_sm_test_util_verify_tx_dhkey_check(
    struct ble_sm_dhkey_check *exp_cmd)
{
    struct ble_sm_dhkey_check cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_PAIR_DHKEY_CHECK,
                                        sizeof(struct ble_sm_dhkey_check));
    ble_sm_dhkey_check_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(memcmp(cmd.value, exp_cmd->value, 16) == 0);
}

static void
ble_sm_test_util_verify_tx_enc_info(struct ble_sm_enc_info *exp_cmd)
{
    struct ble_sm_enc_info cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_ENC_INFO,
                                        sizeof(struct ble_sm_enc_info));
    ble_sm_enc_info_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(memcmp(cmd.ltk, exp_cmd->ltk, 16) == 0);

    /* Ensure LTK is sent in little endian. */
    TEST_ASSERT(memcmp(om->om_data, cmd.ltk, 16) == 0);
}

static void
ble_sm_test_util_verify_tx_master_id(struct ble_sm_master_id *exp_cmd)
{
    struct ble_sm_master_id cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_MASTER_ID,
                                        sizeof(struct ble_sm_master_id));
    ble_sm_master_id_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(cmd.ediv == exp_cmd->ediv);
    TEST_ASSERT(cmd.rand_val == exp_cmd->rand_val);
}

static void
ble_sm_test_util_verify_tx_id_info(struct ble_sm_id_info *exp_cmd)
{
    struct ble_sm_id_info cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_IDENTITY_INFO,
                                        sizeof(struct ble_sm_id_info));
    ble_sm_id_info_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(memcmp(cmd.irk, exp_cmd->irk, 16) == 0);

    /* Ensure IRK is sent in little endian. */
    TEST_ASSERT(memcmp(om->om_data, cmd.irk, 16) == 0);
}

static void
ble_sm_test_util_verify_tx_id_addr_info(struct ble_sm_id_addr_info *exp_cmd)
{
    struct ble_sm_id_addr_info cmd;
    struct os_mbuf *om;
    const uint8_t *our_id_addr;
    int rc;

    ble_hs_lock();
    rc = ble_hs_id_addr(exp_cmd->addr_type, &our_id_addr, NULL);
    ble_hs_unlock();

    TEST_ASSERT_FATAL(rc == 0);

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_IDENTITY_ADDR_INFO,
                                        sizeof(struct ble_sm_id_addr_info));
    ble_sm_id_addr_info_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(cmd.addr_type == exp_cmd->addr_type);
    TEST_ASSERT(memcmp(cmd.bd_addr, exp_cmd->bd_addr, 6) == 0);
    TEST_ASSERT(memcmp(cmd.bd_addr, our_id_addr, 6) == 0);
}

static void
ble_sm_test_util_verify_tx_sign_info(struct ble_sm_sign_info *exp_cmd)
{
    struct ble_sm_sign_info cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_SIGN_INFO,
                                        sizeof(struct ble_sm_sign_info));
    ble_sm_sign_info_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(memcmp(cmd.sig_key, exp_cmd->sig_key, 16) == 0);

    /* Ensure CSRK is sent in little endian. */
    TEST_ASSERT(memcmp(om->om_data, cmd.sig_key, 16) == 0);
}

static void
ble_sm_test_util_verify_tx_sec_req(struct ble_sm_sec_req *exp_cmd)
{
    struct ble_sm_sec_req cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_SEC_REQ, sizeof(struct ble_sm_sec_req));
    ble_sm_sec_req_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(cmd.authreq == exp_cmd->authreq);
}

void
ble_sm_test_util_verify_tx_pair_fail(
    struct ble_sm_pair_fail *exp_cmd)
{
    struct ble_sm_pair_fail cmd;
    struct os_mbuf *om;

    om = ble_sm_test_util_verify_tx_hdr(BLE_SM_OP_PAIR_FAIL,
                                        sizeof(struct ble_sm_pair_fail));
    ble_sm_pair_fail_parse(om->om_data, om->om_len, &cmd);

    TEST_ASSERT(cmd.reason == exp_cmd->reason);
}

static void
ble_sm_test_util_rx_lt_key_req(uint16_t conn_handle, uint64_t r, uint16_t ediv)
{
    struct hci_le_lt_key_req evt;
    int rc;

    evt.subevent_code = BLE_HCI_LE_SUBEV_LT_KEY_REQ;
    evt.connection_handle = conn_handle;
    evt.random_number = r;
    evt.encrypted_diversifier = ediv;

    rc = ble_sm_ltk_req_rx(&evt);
    TEST_ASSERT_FATAL(rc == 0);
}

static void
ble_sm_test_util_verify_tx_lt_key_req_reply(uint16_t conn_handle, uint8_t *stk)
{
    uint8_t param_len;
    uint8_t *param;

    param = ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                           BLE_HCI_OCF_LE_LT_KEY_REQ_REPLY,
                                           &param_len);
    TEST_ASSERT(param_len == BLE_HCI_LT_KEY_REQ_REPLY_LEN);
    TEST_ASSERT(get_le16(param + 0) == conn_handle);
    TEST_ASSERT(memcmp(param + 2, stk, 16) == 0);
}

static void
ble_sm_test_util_verify_tx_lt_key_req_neg_reply(uint16_t conn_handle)
{
    uint8_t param_len;
    uint8_t *param;

    param = ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                           BLE_HCI_OCF_LE_LT_KEY_REQ_NEG_REPLY,
                                           &param_len);
    TEST_ASSERT(param_len == BLE_HCI_LT_KEY_REQ_NEG_REPLY_LEN);
    TEST_ASSERT(get_le16(param + 0) == conn_handle);
}

static void
ble_sm_test_util_set_lt_key_req_neg_reply_ack(uint8_t status,
                                              uint16_t conn_handle)
{
    static uint8_t params[BLE_HCI_LT_KEY_REQ_NEG_REPLY_ACK_PARAM_LEN];

    put_le16(params, conn_handle);
    ble_hs_test_util_hci_ack_set_params(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_LT_KEY_REQ_NEG_REPLY),
        status, params, sizeof params);
}

static void
ble_sm_test_util_set_lt_key_req_reply_ack(uint8_t status, uint16_t conn_handle)
{
    static uint8_t params[BLE_HCI_LT_KEY_REQ_REPLY_ACK_PARAM_LEN];

    put_le16(params, conn_handle);
    ble_hs_test_util_hci_ack_set_params(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_LT_KEY_REQ_REPLY),
        status, params, sizeof params);
}

static void
ble_sm_test_util_rx_enc_change(uint16_t conn_handle, uint8_t status,
                                     uint8_t encryption_enabled)
{
    struct hci_encrypt_change evt;

    evt.status = status;
    evt.encryption_enabled = encryption_enabled;
    evt.connection_handle = conn_handle;

    ble_sm_enc_change_rx(&evt);
}

static void
ble_sm_test_util_verify_tx_start_enc(uint16_t conn_handle,
                                     uint64_t random_number,
                                     uint16_t ediv,
                                     uint8_t *ltk)
{
    uint8_t param_len;
    uint8_t *param;

    param = ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                           BLE_HCI_OCF_LE_START_ENCRYPT,
                                           &param_len);
    TEST_ASSERT(param_len == BLE_HCI_LE_START_ENCRYPT_LEN);
    TEST_ASSERT(get_le16(param + 0) == conn_handle);
    TEST_ASSERT(get_le64(param + 2) == random_number);
    TEST_ASSERT(get_le16(param + 10) == ediv);
    TEST_ASSERT(memcmp(param + 12, ltk, 16) == 0);
}

static void
ble_sm_test_util_verify_tx_add_resolve_list(uint8_t peer_id_addr_type,
                                            uint8_t *peer_id_addr,
                                            uint8_t *peer_irk,
                                            uint8_t *our_irk)
{
    uint8_t param_len;
    uint8_t *param;

    ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                   BLE_HCI_OCF_LE_SET_ADV_ENABLE,
                                   NULL);

    param = ble_hs_test_util_hci_verify_tx(BLE_HCI_OGF_LE,
                                           BLE_HCI_OCF_LE_ADD_RESOLV_LIST,
                                           &param_len);
    TEST_ASSERT(param_len == BLE_HCI_ADD_TO_RESOLV_LIST_LEN);
    TEST_ASSERT(param[0] == peer_id_addr_type);
    TEST_ASSERT(memcmp(param + 1, peer_id_addr, 6) == 0);

    /* Ensure IRKs are sent in little endian. */
    TEST_ASSERT(memcmp(param + 7, peer_irk, 16) == 0);
    TEST_ASSERT(memcmp(param + 23, our_irk, 16) == 0);
}

void
ble_sm_test_util_io_inject(struct ble_sm_test_passkey_info *passkey_info,
                           uint8_t cur_sm_state)
{
    uint8_t io_sm_state;
    int rc;

    io_sm_state = ble_sm_ioact_state(passkey_info->passkey.action);
    if (io_sm_state != cur_sm_state) {
        TEST_ASSERT(ble_sm_test_ioact.action == BLE_SM_IOACT_NONE);
        return;
    }

    TEST_ASSERT(ble_sm_test_ioact.action == passkey_info->passkey.action);

    if (passkey_info->passkey.action == BLE_SM_IOACT_NUMCMP) {
        TEST_ASSERT(ble_sm_test_ioact.numcmp == passkey_info->exp_numcmp);
    }

    rc = ble_sm_inject_io(2, &passkey_info->passkey);
    TEST_ASSERT(rc == 0);

    ble_sm_test_ioact.action = BLE_SM_IOACT_NONE;
}

void
ble_sm_test_util_io_inject_bad(uint16_t conn_handle, uint8_t correct_io_act)
{
    struct ble_sm_proc *proc;
    struct ble_sm_io io;
    uint8_t io_sm_state;
    int already_injected;
    int rc;
    int i;

    /* Lock mutex to prevent thread-safety assert from failing. */
    ble_hs_lock();
    proc = ble_sm_proc_find(conn_handle, BLE_SM_PROC_STATE_NONE, -1, NULL);
    ble_hs_unlock();

    TEST_ASSERT_FATAL(proc != NULL);

    io_sm_state = ble_sm_ioact_state(correct_io_act);

    for (i = 1; i < BLE_SM_IOACT_MAX_PLUS_ONE; i++) {
        if (io_sm_state != proc->state  ||
            i != correct_io_act         ||
            proc->flags & BLE_SM_PROC_F_IO_INJECTED) {

            already_injected = proc->flags & BLE_SM_PROC_F_IO_INJECTED;

            io.action = i;
            rc = ble_sm_inject_io(conn_handle, &io);

            if (already_injected) {
                TEST_ASSERT(rc == BLE_HS_EALREADY);
            } else {
                TEST_ASSERT(rc == BLE_HS_EINVAL);
            }
        }
    }
}

void
ble_sm_test_util_io_check_pre(struct ble_sm_test_passkey_info *passkey_info,
                              uint8_t cur_sm_state)
{
    uint8_t io_sm_state;
    int rc;

    io_sm_state = ble_sm_ioact_state(passkey_info->passkey.action);
    if (io_sm_state != cur_sm_state) {
        return;
    }

    if (!passkey_info->io_before_rx) {
        return;
    }

    if (passkey_info->passkey.action == BLE_SM_IOACT_NUMCMP) {
        TEST_ASSERT(ble_sm_test_ioact.numcmp == passkey_info->exp_numcmp);
    }

    rc = ble_sm_inject_io(2, &passkey_info->passkey);
    TEST_ASSERT(rc == 0);
}

void
ble_sm_test_util_io_check_post(struct ble_sm_test_passkey_info *passkey_info,
                               uint8_t cur_sm_state)
{
    uint8_t io_sm_state;
    int rc;

    io_sm_state = ble_sm_ioact_state(passkey_info->passkey.action);
    if (io_sm_state != cur_sm_state) {
        return;
    }

    if (passkey_info->io_before_rx) {
        return;
    }

    if (passkey_info->passkey.action == BLE_SM_IOACT_NUMCMP) {
        TEST_ASSERT(ble_sm_test_ioact.numcmp == passkey_info->exp_numcmp);
    }

    /* Ensure response not sent until user performs IO. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_queue_sz() == 0);

    rc = ble_sm_inject_io(2, &passkey_info->passkey);
    TEST_ASSERT_FATAL(rc == 0);
}

static void
ble_sm_test_util_verify_persist(struct ble_sm_test_params *params,
                                int we_are_initiator)
{
    struct ble_sm_test_util_entity peer_entity;
    struct ble_sm_test_util_entity our_entity;
    struct ble_store_value_sec value_sec;
    struct ble_store_key_sec key_sec;
    int csrk_expected;
    int ltk_expected;
    int peer_irk_expected;
    int our_irk_expected;
    int bonding;
    int sc;
    int rc;

    ble_sm_test_util_params_to_entities(params, we_are_initiator,
                                        &our_entity, &peer_entity);

    sc = params->pair_req.authreq & BLE_SM_PAIR_AUTHREQ_SC &&
         params->pair_rsp.authreq & BLE_SM_PAIR_AUTHREQ_SC;

    bonding = params->pair_req.authreq & BLE_SM_PAIR_AUTHREQ_BOND &&
              params->pair_rsp.authreq & BLE_SM_PAIR_AUTHREQ_BOND;

    memset(&key_sec, 0, sizeof key_sec);
    key_sec.peer_addr = *BLE_ADDR_ANY;

    rc = ble_store_read_peer_sec(&key_sec, &value_sec);
    if (!bonding) {
        TEST_ASSERT(rc == BLE_HS_ENOENT);
        peer_irk_expected = 0;
    } else {
        TEST_ASSERT_FATAL(rc == 0);

        ltk_expected =
            sc || !!(peer_entity.key_dist & BLE_SM_PAIR_KEY_DIST_ENC);
        peer_irk_expected =
            !!(peer_entity.key_dist & BLE_SM_PAIR_KEY_DIST_ID);
        csrk_expected =
            !!(peer_entity.key_dist & BLE_SM_PAIR_KEY_DIST_SIGN);

        TEST_ASSERT(value_sec.peer_addr.type == peer_entity.id_addr_type);
        TEST_ASSERT(
            memcmp(value_sec.peer_addr.val, peer_entity.id_addr, 6) == 0);
        TEST_ASSERT(value_sec.ediv == peer_entity.ediv);
        TEST_ASSERT(value_sec.rand_num == peer_entity.rand_num);
        TEST_ASSERT(value_sec.authenticated == params->authenticated);

        TEST_ASSERT(value_sec.ltk_present == ltk_expected);
        TEST_ASSERT(memcmp(value_sec.ltk, peer_entity.ltk, 16) == 0);

        TEST_ASSERT(value_sec.irk_present == peer_irk_expected);
        if (peer_irk_expected) {
            TEST_ASSERT(memcmp(value_sec.irk,
                               peer_entity.id_info->irk, 16) == 0);
        }

        TEST_ASSERT(value_sec.csrk_present == csrk_expected);
        if (csrk_expected) {
            TEST_ASSERT(memcmp(value_sec.csrk,
                               peer_entity.sign_info->sig_key, 16) == 0);
        }
    }

    rc = ble_store_read_our_sec(&key_sec, &value_sec);
    if (!bonding) {
        TEST_ASSERT(rc == BLE_HS_ENOENT);
    } else {
        TEST_ASSERT_FATAL(rc == 0);

        ltk_expected =
            sc || !!(our_entity.key_dist & BLE_SM_PAIR_KEY_DIST_ENC);
        our_irk_expected =
            !!(our_entity.key_dist & BLE_SM_PAIR_KEY_DIST_ID);
        csrk_expected =
            !!(our_entity.key_dist & BLE_SM_PAIR_KEY_DIST_SIGN);

        TEST_ASSERT(value_sec.peer_addr.type == peer_entity.id_addr_type);
        TEST_ASSERT(memcmp(value_sec.peer_addr.val, peer_entity.id_addr, 6) == 0);
        TEST_ASSERT(value_sec.ediv == our_entity.ediv);
        TEST_ASSERT(value_sec.rand_num == our_entity.rand_num);
        TEST_ASSERT(value_sec.authenticated == params->authenticated);

        TEST_ASSERT(value_sec.ltk_present == ltk_expected);
        TEST_ASSERT(memcmp(value_sec.ltk, our_entity.ltk, 16) == 0);

        TEST_ASSERT(value_sec.irk_present == our_irk_expected);
        if (our_irk_expected) {
            TEST_ASSERT(memcmp(value_sec.irk,
                               our_entity.id_info->irk, 16) == 0);
        }

        TEST_ASSERT(value_sec.csrk_present == csrk_expected);
        if (csrk_expected) {
            TEST_ASSERT(memcmp(value_sec.csrk,
                               our_entity.sign_info->sig_key, 16) == 0);
        }
    }

    /* Verify no other keys were persisted. */
    key_sec.idx++;
    rc = ble_store_read_our_sec(&key_sec, &value_sec);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOENT);
    rc = ble_store_read_peer_sec(&key_sec, &value_sec);
    TEST_ASSERT_FATAL(rc == BLE_HS_ENOENT);

    /* Verify we sent the peer's IRK to the controller. */
    if (peer_irk_expected) {
        ble_sm_test_util_verify_tx_add_resolve_list(peer_entity.id_addr_type,
                                                    peer_entity.id_addr,
                                                    peer_entity.id_info->irk,
                                                    our_entity.id_info->irk);
    }
}

static void
ble_sm_test_util_peer_bonding_good(int send_enc_req,
                                   uint8_t our_addr_type,
                                   uint8_t *our_rpa,
                                   uint8_t peer_addr_type,
                                   uint8_t *peer_id_addr,
                                   uint8_t *peer_rpa,
                                   uint8_t *ltk, int authenticated,
                                   uint16_t ediv, uint64_t rand_num)
{
    struct ble_hs_conn *conn;
    int rc;

    ble_hs_test_util_create_rpa_conn(2, our_addr_type, our_rpa, peer_addr_type,
                                     peer_id_addr, peer_rpa,
                                     BLE_HS_TEST_CONN_FEAT_ALL,
                                     ble_sm_test_util_conn_cb, NULL);

    /* This test inspects and modifies the connection object after unlocking
     * the host mutex.  It is not OK for real code to do this, but this test
     * can assume the connection list is unchanging.
     */
    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    conn->bhc_flags &= ~BLE_HS_CONN_F_MASTER;
    ble_hs_unlock();

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    if (send_enc_req) {
        rc = ble_sm_slave_initiate(2);
        TEST_ASSERT(rc == 0);
    }

    /* Receive a long term key request from the controller. */
    ble_sm_test_util_set_lt_key_req_reply_ack(0, 2);
    ble_sm_test_util_rx_lt_key_req(2, rand_num, ediv);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);

    /* Ensure the LTK request event got sent to the application. */
    TEST_ASSERT(ble_sm_test_store_obj_type == BLE_STORE_OBJ_TYPE_OUR_SEC);
    TEST_ASSERT(ble_sm_test_store_key.sec.peer_addr.type ==
                ble_hs_misc_addr_type_to_id(peer_addr_type));
    TEST_ASSERT(ble_sm_test_store_key.sec.ediv_rand_present);
    TEST_ASSERT(ble_sm_test_store_key.sec.ediv == ediv);
    TEST_ASSERT(ble_sm_test_store_key.sec.rand_num == rand_num);

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, BLE_SM_IOACT_NONE);

    /* Ensure we sent the expected long term key request reply command. */
    ble_sm_test_util_verify_tx_lt_key_req_reply(2, ltk);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, BLE_SM_IOACT_NONE);

    /* Receive an encryption changed event. */
    ble_sm_test_util_rx_enc_change(2, 0, 1);

    /* Pairing should now be complete. */
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify that security callback was executed. */
    TEST_ASSERT(ble_sm_test_gap_event_type == BLE_GAP_EVENT_ENC_CHANGE);
    TEST_ASSERT(ble_sm_test_gap_status == 0);
    TEST_ASSERT(ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                authenticated);

    /* Verify that connection has correct security state. */
    TEST_ASSERT(ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                authenticated);

    ble_hs_test_util_conn_disconnect(2);
}

void
ble_sm_test_util_peer_bonding_bad(uint16_t ediv, uint64_t rand_num)
{
    struct ble_hs_conn *conn;

    ble_sm_test_util_init();

    ble_hs_test_util_create_conn(2, ((uint8_t[6]){1,2,3,4,5,6}),
                                 ble_sm_test_util_conn_cb,
                                 NULL);

    /* This test inspects and modifies the connection object after unlocking
     * the host mutex.  It is not OK for real code to do this, but this test
     * can assume the connection list is unchanging.
     */
    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    conn->bhc_flags &= ~BLE_HS_CONN_F_MASTER;
    ble_hs_unlock();

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Receive a long term key request from the controller. */
    ble_sm_test_util_set_lt_key_req_neg_reply_ack(0, 2);
    ble_sm_test_util_rx_lt_key_req(2, rand_num, ediv);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);

    /* Ensure the LTK request event got sent to the application. */
    TEST_ASSERT(ble_sm_test_store_obj_type ==
                BLE_STORE_OBJ_TYPE_OUR_SEC);
    TEST_ASSERT(ble_sm_test_store_key.sec.ediv_rand_present);
    TEST_ASSERT(ble_sm_test_store_key.sec.ediv == ediv);
    TEST_ASSERT(ble_sm_test_store_key.sec.rand_num == rand_num);

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);

    /* Ensure we sent the expected long term key request neg reply command. */
    ble_sm_test_util_verify_tx_lt_key_req_neg_reply(2);

    /* Ensure the security procedure was aborted. */
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(!conn->bhc_sec_state.authenticated);
    TEST_ASSERT(ble_sm_num_procs() == 0);
}

/**
 * @param send_enc_req          Whether this procedure is initiated by a slave
 *                                  security request;
 *                                  1: Peer sends a security request at start.
 *                                  0: No security request; we initiate.
 */
static void
ble_sm_test_util_us_bonding_good(int send_enc_req, uint8_t our_addr_type,
                                 uint8_t *our_rpa,
                                 uint8_t peer_addr_type,
                                 uint8_t *peer_id_addr, uint8_t *peer_rpa,
                                 uint8_t *ltk, int authenticated,
                                 uint16_t ediv, uint64_t rand_num)
{
    struct ble_sm_sec_req sec_req;
    struct ble_hs_conn *conn;

    ble_hs_test_util_create_rpa_conn(2, our_addr_type, our_rpa,
                                     peer_addr_type, peer_id_addr,
                                     peer_rpa, BLE_HS_TEST_CONN_FEAT_ALL,
                                     ble_sm_test_util_conn_cb, NULL);

    /* This test inspects and modifies the connection object after unlocking
     * the host mutex.  It is not OK for real code to do this, but this test
     * can assume the connection list is unchanging.
     */
    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    ble_hs_unlock();

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    ble_hs_test_util_hci_ack_set(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_START_ENCRYPT),
        0);

    if (send_enc_req) {
        sec_req.authreq = 0;
        sec_req.authreq |= BLE_SM_PAIR_AUTHREQ_BOND;
        if (authenticated) {
            sec_req.authreq |= BLE_SM_PAIR_AUTHREQ_MITM;
        }
        ble_sm_test_util_rx_sec_req(2, &sec_req, 0);
    } else {
        ble_gap_security_initiate(2);
    }

    /* Ensure we sent the expected start encryption command. */
    ble_sm_test_util_verify_tx_start_enc(2, rand_num, ediv, ltk);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, BLE_SM_IOACT_NONE);

    /* Receive an encryption changed event. */
    ble_sm_test_util_rx_enc_change(2, 0, 1);

    /* Pairing should now be complete. */
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify that security callback was executed. */
    TEST_ASSERT(ble_sm_test_gap_event_type == BLE_GAP_EVENT_ENC_CHANGE);
    TEST_ASSERT(ble_sm_test_gap_status == 0);
    TEST_ASSERT(ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                authenticated);

    /* Verify that connection has correct security state. */
    TEST_ASSERT(ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                authenticated);

    ble_hs_test_util_conn_disconnect(2);
}

void
ble_sm_test_util_peer_fail_inval(
    int we_are_master,
    uint8_t *init_id_addr,
    uint8_t *resp_addr,
    struct ble_sm_pair_cmd *pair_req,
    struct ble_sm_pair_fail *pair_fail)
{
    struct ble_hs_conn *conn;

    ble_sm_test_util_init();
    ble_hs_id_set_pub(resp_addr);

    ble_hs_test_util_create_conn(2, init_id_addr, ble_sm_test_util_conn_cb,
                                 NULL);

    /* This test inspects and modifies the connection object after unlocking
     * the host mutex.  It is not OK for real code to do this, but this test
     * can assume the connection list is unchanging.
     */
    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    ble_hs_unlock();

    if (!we_are_master) {
        conn->bhc_flags &= ~BLE_HS_CONN_F_MASTER;
    }

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Receive a pair request from the peer. */
    ble_sm_test_util_rx_pair_req(2, pair_req,
                                 BLE_HS_SM_US_ERR(pair_fail->reason));
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Ensure we sent the expected pair fail. */
    ble_sm_test_util_verify_tx_pair_fail(pair_fail);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify that security callback was not executed. */
    TEST_ASSERT(ble_sm_test_gap_event_type == -1);
    TEST_ASSERT(ble_sm_test_gap_status == -1);

    /* Verify that connection has correct security state. */
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(!conn->bhc_sec_state.authenticated);
}

void
ble_sm_test_util_peer_lgcy_fail_confirm(
    uint8_t *init_id_addr,
    uint8_t *resp_addr,
    struct ble_sm_pair_cmd *pair_req,
    struct ble_sm_pair_cmd *pair_rsp,
    struct ble_sm_pair_confirm *confirm_req,
    struct ble_sm_pair_confirm *confirm_rsp,
    struct ble_sm_pair_random *random_req,
    struct ble_sm_pair_random *random_rsp,
    struct ble_sm_pair_fail *fail_rsp)
{
    struct ble_hs_conn *conn;

    ble_sm_test_util_init();
    ble_hs_id_set_pub(resp_addr);
    ble_sm_dbg_set_next_pair_rand(random_rsp->value);

    if (pair_rsp->authreq & BLE_SM_PAIR_AUTHREQ_SC) {
        ble_hs_cfg.sm_sc = 1;
    } else {
        ble_hs_cfg.sm_sc = 0;
    }

    ble_hs_test_util_create_conn(2, init_id_addr, ble_sm_test_util_conn_cb,
                                 NULL);

    /* This test inspects and modifies the connection object after unlocking
     * the host mutex.  It is not OK for real code to do this, but this test
     * can assume the connection list is unchanging.
     */
    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    ble_hs_unlock();

    /* Peer is the initiator so we must be the slave. */
    conn->bhc_flags &= ~BLE_HS_CONN_F_MASTER;

    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Receive a pair request from the peer. */
    ble_sm_test_util_rx_pair_req(2, pair_req, 0);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, BLE_SM_IOACT_NONE);

    /* Ensure we sent the expected pair response. */
    ble_sm_test_util_verify_tx_pair_rsp(pair_rsp);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, BLE_SM_IOACT_NONE);

    /* Receive a pair confirm from the peer. */
    ble_sm_test_util_rx_confirm(2, confirm_req);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, BLE_SM_IOACT_NONE);

    /* Ensure we sent the expected pair confirm. */
    ble_sm_test_util_verify_tx_pair_confirm(confirm_rsp);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, BLE_SM_IOACT_NONE);

    /* Receive a pair random from the peer. */
    ble_sm_test_util_rx_random(
        2, random_req, BLE_HS_SM_US_ERR(BLE_SM_ERR_CONFIRM_MISMATCH));

    /* Ensure we sent the expected pair fail. */
    ble_sm_test_util_verify_tx_pair_fail(fail_rsp);

    /* The proc should now be freed. */
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify that security callback was executed. */
    TEST_ASSERT(ble_sm_test_gap_event_type == BLE_GAP_EVENT_ENC_CHANGE);
    TEST_ASSERT(ble_sm_test_gap_status ==
                BLE_HS_SM_US_ERR(BLE_SM_ERR_CONFIRM_MISMATCH));
    TEST_ASSERT(!ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(!ble_sm_test_sec_state.authenticated);

    /* Verify that connection has correct security state. */
    TEST_ASSERT(ble_sm_test_sec_state.encrypted ==
                conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                conn->bhc_sec_state.authenticated);
}

static void
ble_sm_test_util_bonding_all(struct ble_sm_test_params *params,
                             int we_are_original_initiator)
{
    struct ble_sm_test_util_entity peer_entity;
    struct ble_sm_test_util_entity our_entity;
    int sc;

    if (!(params->pair_req.authreq & BLE_SM_PAIR_AUTHREQ_BOND) ||
        !(params->pair_rsp.authreq & BLE_SM_PAIR_AUTHREQ_BOND)) {

        /* Bonding not performed. */
        return;
    }

    sc = params->pair_req.authreq & BLE_SM_PAIR_AUTHREQ_SC &&
         params->pair_rsp.authreq & BLE_SM_PAIR_AUTHREQ_SC;

    ble_sm_test_util_params_to_entities(params, we_are_original_initiator,
                                        &our_entity, &peer_entity);

    if (sc || peer_entity.key_dist & BLE_SM_PAIR_KEY_DIST_ENC) {
        /* We are master; we initiate procedure. */
        ble_sm_test_util_us_bonding_good(0, our_entity.addr_type,
                                         our_entity.rpa,
                                         peer_entity.addr_type,
                                         peer_entity.id_addr,
                                         peer_entity.rpa,
                                         peer_entity.ltk,
                                         params->authenticated,
                                         peer_entity.ediv,
                                         peer_entity.rand_num);

        /* We are master; peer initiates procedure via security request. */
        ble_sm_test_util_us_bonding_good(1, our_entity.addr_type,
                                         our_entity.rpa,
                                         peer_entity.addr_type,
                                         peer_entity.id_addr,
                                         peer_entity.rpa,
                                         peer_entity.ltk,
                                         params->authenticated,
                                         peer_entity.ediv,
                                         peer_entity.rand_num);
    }

    if (sc || our_entity.key_dist & BLE_SM_PAIR_KEY_DIST_ENC) {
        /* Peer is master; peer initiates procedure. */
        ble_sm_test_util_peer_bonding_good(0, our_entity.addr_type,
                                           our_entity.rpa,
                                           peer_entity.addr_type,
                                           peer_entity.id_addr,
                                           peer_entity.rpa,
                                           our_entity.ltk,
                                           params->authenticated,
                                           our_entity.ediv,
                                           our_entity.rand_num);

        /* Peer is master; we initiate procedure via security request. */
        ble_sm_test_util_peer_bonding_good(1, our_entity.addr_type,
                                           our_entity.rpa,
                                           peer_entity.addr_type,
                                           peer_entity.id_addr,
                                           peer_entity.rpa,
                                           our_entity.ltk,
                                           params->authenticated,
                                           our_entity.ediv,
                                           our_entity.rand_num);
    }
}

static void
ble_sm_test_util_rx_keys(struct ble_sm_test_params *params,
                         int we_are_initiator)
{
    struct ble_sm_id_addr_info *peer_id_addr_info;
    struct ble_sm_sign_info *peer_sign_info;
    struct ble_sm_master_id *peer_master_id;
    struct ble_sm_enc_info *peer_enc_info;
    struct ble_sm_id_info *peer_id_info;
    uint8_t peer_key_dist;
    int sc;

    if (we_are_initiator) {
        peer_key_dist = params->pair_rsp.resp_key_dist;
        peer_id_addr_info = &params->id_addr_info_req;
        peer_sign_info = &params->sign_info_req;
        peer_master_id = &params->master_id_req;
        peer_enc_info = &params->enc_info_req;
        peer_id_info = &params->id_info_req;
    } else {
        peer_key_dist = params->pair_rsp.init_key_dist;
        peer_id_addr_info = &params->id_addr_info_rsp;
        peer_sign_info = &params->sign_info_rsp;
        peer_master_id = &params->master_id_rsp;
        peer_enc_info = &params->enc_info_rsp;
        peer_id_info = &params->id_info_rsp;
    }

    sc = params->pair_req.authreq & BLE_SM_PAIR_AUTHREQ_SC &&
         params->pair_rsp.authreq & BLE_SM_PAIR_AUTHREQ_SC;

    /* Receive key material from peer. */
    if (!sc && (peer_key_dist & BLE_SM_PAIR_KEY_DIST_ENC)) {
        ble_sm_test_util_rx_enc_info(2, peer_enc_info, 0);
        ble_sm_test_util_rx_master_id(2, peer_master_id, 0);
    }
    if (peer_key_dist & BLE_SM_PAIR_KEY_DIST_ID) {

        ble_hs_test_util_hci_ack_set_seq(((struct ble_hs_test_util_hci_ack[]) {
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
        }));

        ble_sm_test_util_rx_id_info(2, peer_id_info, 0);
        ble_sm_test_util_rx_id_addr_info(2, peer_id_addr_info, 0);
    }
    if (peer_key_dist & BLE_SM_PAIR_KEY_DIST_SIGN) {
        ble_sm_test_util_rx_sign_info(2, peer_sign_info, 0);
    }
}

static void
ble_sm_test_util_verify_tx_keys(struct ble_sm_test_params *params,
                                int we_are_initiator)
{
    struct ble_sm_id_addr_info *our_id_addr_info;
    struct ble_sm_sign_info *our_sign_info;
    struct ble_sm_master_id *our_master_id;
    struct ble_sm_enc_info *our_enc_info;
    struct ble_sm_id_info *our_id_info;
    uint8_t our_key_dist;
    int sc;

    if (we_are_initiator) {
        our_key_dist = params->pair_rsp.init_key_dist;
        our_id_addr_info = &params->id_addr_info_rsp;
        our_sign_info = &params->sign_info_rsp;
        our_master_id = &params->master_id_rsp;
        our_enc_info = &params->enc_info_rsp;
        our_id_info = &params->id_info_rsp;
    } else {
        our_key_dist = params->pair_rsp.resp_key_dist;
        our_id_addr_info = &params->id_addr_info_req;
        our_sign_info = &params->sign_info_req;
        our_master_id = &params->master_id_req;
        our_enc_info = &params->enc_info_req;
        our_id_info = &params->id_info_req;
    }

    sc = params->pair_req.authreq & BLE_SM_PAIR_AUTHREQ_SC &&
         params->pair_rsp.authreq & BLE_SM_PAIR_AUTHREQ_SC;

    if (!sc && our_key_dist & BLE_SM_PAIR_KEY_DIST_ENC) {
        ble_sm_test_util_verify_tx_enc_info(our_enc_info);
        ble_sm_test_util_verify_tx_master_id(our_master_id);
    }
    if (our_key_dist & BLE_SM_PAIR_KEY_DIST_ID) {
        ble_sm_test_util_verify_tx_id_info(our_id_info);
        ble_sm_test_util_verify_tx_id_addr_info(our_id_addr_info);
    }
    if (our_key_dist & BLE_SM_PAIR_KEY_DIST_SIGN) {
        ble_sm_test_util_verify_tx_sign_info(our_sign_info);
    }
}

static void
ble_sm_test_util_us_lgcy_good_once_no_init(
    struct ble_sm_test_params *params,
    struct ble_hs_conn *conn,
    struct ble_sm_test_util_entity *our_entity,
    struct ble_sm_test_util_entity *peer_entity)
{
    int rc;

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    ble_hs_test_util_hci_ack_set(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_START_ENCRYPT), 0);
    if (params->sec_req.authreq != 0) {
        ble_sm_test_util_rx_sec_req(2, &params->sec_req, 0);
    } else {
        /* Initiate the pairing procedure. */
        rc = ble_gap_security_initiate(2);
        TEST_ASSERT_FATAL(rc == 0);
    }

    /* Ensure we sent the expected pair request. */
    ble_sm_test_util_verify_tx_pair_req(our_entity->pair_cmd);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a pair response from the peer. */
    ble_sm_test_util_rx_pair_rsp(2, peer_entity->pair_cmd, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    ble_sm_test_util_io_inject(&params->passkey_info,
                               BLE_SM_PROC_STATE_CONFIRM);

    /* Ensure we sent the expected pair confirm. */
    ble_sm_test_util_verify_tx_pair_confirm(our_entity->confirms);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a pair confirm from the peer. */
    ble_sm_test_util_rx_confirm(2, peer_entity->confirms);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected pair random. */
    ble_sm_test_util_verify_tx_pair_random(our_entity->randoms);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a pair random from the peer. */
    ble_sm_test_util_rx_random(2, peer_entity->randoms, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected start encryption command. */
    ble_sm_test_util_verify_tx_start_enc(2, 0, 0, params->stk);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive an encryption changed event. */
    ble_sm_test_util_rx_enc_change(2, 0, 1);

    /* Receive key material from peer. */
    ble_sm_test_util_rx_keys(params, 1);

    /* Verify key material gets sent to peer. */
    ble_sm_test_util_verify_tx_keys(params, 1);

    /* Pairing should now be complete. */
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify that security callback was executed. */
    TEST_ASSERT(ble_sm_test_gap_event_type == BLE_GAP_EVENT_ENC_CHANGE);
    TEST_ASSERT(ble_sm_test_gap_status == 0);
    TEST_ASSERT(ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated == params->authenticated);

    /* Verify that connection has correct security state. */
    TEST_ASSERT(ble_sm_test_sec_state.encrypted ==
                conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                conn->bhc_sec_state.authenticated);

    /* Verify the appropriate security material was persisted. */
    ble_sm_test_util_verify_persist(params, 1);

    ble_hs_test_util_conn_disconnect(2);
}

static void
ble_sm_test_util_us_lgcy_good_once(struct ble_sm_test_params *params)
{
    struct ble_sm_test_util_entity peer_entity;
    struct ble_sm_test_util_entity our_entity;
    struct ble_hs_conn *conn;

    ble_sm_test_util_init_good(params, 1, &conn, &our_entity, &peer_entity);
    ble_sm_test_util_us_lgcy_good_once_no_init(
        params, conn, &our_entity, &peer_entity);
}

void
ble_sm_test_util_us_lgcy_good(struct ble_sm_test_params *params)
{
    ble_addr_t peer_addr;
    int rc;

    /*** We are master. */

    /* We initiate pairing. */
    params->sec_req.authreq = 0;
    ble_sm_test_util_us_lgcy_good_once(params);

    /* Peer initiates with security request. */
    params->sec_req.authreq = params->pair_rsp.authreq;
    ble_sm_test_util_us_lgcy_good_once(params);

    /* Verify link can be restored via the encryption procedure. */
    ble_sm_test_util_bonding_all(params, 1);

    /* Verify programmatic unbonding. */
    peer_addr.type = ble_hs_misc_addr_type_to_id(params->resp_addr_type);
    memcpy(peer_addr.val, params->resp_id_addr, sizeof peer_addr.val);
    rc = ble_store_util_delete_peer(&peer_addr);
    TEST_ASSERT(rc == 0);

    TEST_ASSERT(ble_hs_test_util_num_our_secs() == 0);
    TEST_ASSERT(ble_hs_test_util_num_peer_secs() == 0);
}

void
ble_sm_test_util_peer_lgcy_good_once_no_init(
    struct ble_sm_test_params *params,
    struct ble_hs_conn *conn,
    struct ble_sm_test_util_entity *our_entity,
    struct ble_sm_test_util_entity *peer_entity)
{
    int rc;

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    if (params->sec_req.authreq != 0) {
        rc = ble_sm_slave_initiate(2);
        TEST_ASSERT(rc == 0);

        /* Ensure we sent the expected security request. */
        ble_sm_test_util_verify_tx_sec_req(&params->sec_req);
    }

    /* Receive a pair request from the peer. */
    ble_sm_test_util_rx_pair_req(2, peer_entity->pair_cmd, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected pair response. */
    ble_sm_test_util_verify_tx_pair_rsp(our_entity->pair_cmd);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    ble_sm_test_util_io_check_pre(&params->passkey_info,
                                  BLE_SM_PROC_STATE_CONFIRM);

    /* Receive a pair confirm from the peer. */
    ble_sm_test_util_rx_confirm(2, peer_entity->confirms);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    ble_sm_test_util_io_check_post(&params->passkey_info,
                                   BLE_SM_PROC_STATE_CONFIRM);

    /* Ensure we sent the expected pair confirm. */
    ble_sm_test_util_verify_tx_pair_confirm(our_entity->confirms);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a pair random from the peer. */
    ble_sm_test_util_rx_random(2, peer_entity->randoms, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected pair random. */
    ble_sm_test_util_verify_tx_pair_random(our_entity->randoms);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a long term key request from the controller. */
    ble_sm_test_util_set_lt_key_req_reply_ack(0, 2);
    ble_sm_test_util_rx_lt_key_req(2, 0, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected long term key request reply command. */
    ble_sm_test_util_verify_tx_lt_key_req_reply(2, params->stk);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive an encryption changed event. */
    ble_sm_test_util_rx_enc_change(2, 0, 1);

    /* Verify key material gets sent to peer. */
    ble_sm_test_util_verify_tx_keys(params, 0);

    /* Receive key material from peer. */
    ble_sm_test_util_rx_keys(params, 0);

    /* Pairing should now be complete. */
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify that security callback was executed. */
    TEST_ASSERT(ble_sm_test_gap_event_type == BLE_GAP_EVENT_ENC_CHANGE);
    TEST_ASSERT(ble_sm_test_gap_status == 0);
    TEST_ASSERT(ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                params->authenticated);

    /* Verify that connection has correct security state. */
    TEST_ASSERT(ble_sm_test_sec_state.encrypted ==
                conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                conn->bhc_sec_state.authenticated);

    /* Verify the appropriate security material was persisted. */
    ble_sm_test_util_verify_persist(params, 0);

    ble_hs_test_util_conn_disconnect(2);
}

void
ble_sm_test_util_peer_lgcy_good_once(struct ble_sm_test_params *params)
{
    struct ble_sm_test_util_entity peer_entity;
    struct ble_sm_test_util_entity our_entity;
    struct ble_hs_conn *conn;

    ble_sm_test_util_init_good(params, 0, &conn, &our_entity, &peer_entity);
    ble_sm_test_util_peer_lgcy_good_once_no_init(
        params, conn, &our_entity, &peer_entity);
}

void
ble_sm_test_util_peer_lgcy_good(struct ble_sm_test_params *params)
{
    ble_addr_t peer_addr;
    int rc;

    /*** Peer is master. */

    /* Peer performs IO first; peer initiates pairing. */
    params->passkey_info.io_before_rx = 0;
    params->sec_req.authreq = 0;
    ble_sm_test_util_peer_lgcy_good_once(params);

    /* Peer performs IO first; we initiate with security request. */
    params->passkey_info.io_before_rx = 0;
    params->sec_req.authreq = params->pair_rsp.authreq;
    ble_sm_test_util_peer_lgcy_good_once(params);

    /* We perform IO first; peer initiates pairing. */
    params->passkey_info.io_before_rx = 1;
    params->sec_req.authreq = 0;
    ble_sm_test_util_peer_lgcy_good_once(params);

    /* We perform IO first; we initiate with security request. */
    params->passkey_info.io_before_rx = 1;
    params->sec_req.authreq = params->pair_rsp.authreq;
    ble_sm_test_util_peer_lgcy_good_once(params);

    /* Verify link can be restored via the encryption procedure. */
    ble_sm_test_util_bonding_all(params, 0);

    /* Verify repeating pairing event generated when peer attempts to pair
     * while bonded.
     */
    ble_sm_test_util_repeat_pairing(params, 0);

    /* Verify programmatic unbonding. */
    peer_addr.type = ble_hs_misc_addr_type_to_id(params->init_addr_type);
    memcpy(peer_addr.val, params->init_id_addr, sizeof peer_addr.val);
    rc = ble_store_util_delete_peer(&peer_addr);
    TEST_ASSERT(rc == 0);

    TEST_ASSERT(ble_hs_test_util_num_our_secs() == 0);
    TEST_ASSERT(ble_hs_test_util_num_peer_secs() == 0);
}

static void
ble_sm_test_util_us_sc_good_once_no_init(
    struct ble_sm_test_params *params,
    struct ble_hs_conn *conn,
    struct ble_sm_test_util_entity *our_entity,
    struct ble_sm_test_util_entity *peer_entity)
{
    int num_iters;
    int rc;
    int i;

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    ble_hs_test_util_hci_ack_set(
        ble_hs_hci_util_opcode_join(BLE_HCI_OGF_LE,
                                    BLE_HCI_OCF_LE_START_ENCRYPT), 0);
    if (params->sec_req.authreq != 0) {
        ble_sm_test_util_rx_sec_req(2, &params->sec_req, 0);
    } else {
        /* Initiate the pairing procedure. */
        rc = ble_gap_security_initiate(2);
        TEST_ASSERT_FATAL(rc == 0);
    }

    /* Ensure we sent the expected pair request. */
    ble_sm_test_util_verify_tx_pair_req(our_entity->pair_cmd);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a pair response from the peer. */
    ble_sm_test_util_rx_pair_rsp(2, peer_entity->pair_cmd, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected public key. */
    ble_sm_test_util_verify_tx_public_key(our_entity->public_key);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a public key from the peer. */
    ble_sm_test_util_rx_public_key(2, peer_entity->public_key);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    switch (params->pair_alg) {
    case BLE_SM_PAIR_ALG_PASSKEY:
        num_iters = 20;
        break;

    default:
        num_iters = 1;
        break;
    }

    ble_sm_test_util_io_inject(&params->passkey_info,
                               BLE_SM_PROC_STATE_CONFIRM);

    for (i = 0; i < num_iters; i++) {
        if (params->pair_alg != BLE_SM_PAIR_ALG_JW      &&
            params->pair_alg != BLE_SM_PAIR_ALG_NUMCMP) {

            if (i < num_iters - 1) {
                ble_sm_dbg_set_next_pair_rand(
                    our_entity->randoms[i + 1].value);
            }

            /* Ensure we sent the expected pair confirm. */
            ble_sm_test_util_verify_tx_pair_confirm(our_entity->confirms + i);
            TEST_ASSERT(!conn->bhc_sec_state.encrypted);
            TEST_ASSERT(ble_sm_num_procs() == 1);
            ble_sm_test_util_io_inject_bad(
                2, params->passkey_info.passkey.action);
        }

        /* Receive a pair confirm from the peer. */
        ble_sm_test_util_rx_confirm(2, peer_entity->confirms + i);
        TEST_ASSERT(!conn->bhc_sec_state.encrypted);
        TEST_ASSERT(ble_sm_num_procs() == 1);
        ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

        /* Ensure we sent the expected pair random. */
        ble_sm_test_util_verify_tx_pair_random(our_entity->randoms + i);
        TEST_ASSERT(!conn->bhc_sec_state.encrypted);
        TEST_ASSERT(ble_sm_num_procs() == 1);
        ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

        /* Receive a pair random from the peer. */
        ble_sm_test_util_rx_random(2, peer_entity->randoms + i, 0);
        TEST_ASSERT(!conn->bhc_sec_state.encrypted);
        TEST_ASSERT(ble_sm_num_procs() == 1);
        ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);
    }

    ble_sm_test_util_io_inject(&params->passkey_info,
                               BLE_SM_PROC_STATE_DHKEY_CHECK);

    /* Ensure we sent the expected dhkey check. */
    ble_sm_test_util_verify_tx_dhkey_check(our_entity->dhkey_check);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a dhkey check from the peer. */
    ble_sm_test_util_rx_dhkey_check(2, peer_entity->dhkey_check, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected start encryption command. */
    ble_sm_test_util_verify_tx_start_enc(2, 0, 0, params->ltk);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive an encryption changed event. */
    ble_sm_test_util_rx_enc_change(2, 0, 1);

    /* Receive key material from peer. */
    ble_sm_test_util_rx_keys(params, 1);

    /* Verify key material gets sent to peer. */
    ble_sm_test_util_verify_tx_keys(params, 1);

    /* Pairing should now be complete. */
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify that security callback was executed. */
    TEST_ASSERT(ble_sm_test_gap_event_type == BLE_GAP_EVENT_ENC_CHANGE);
    TEST_ASSERT(ble_sm_test_gap_status == 0);
    TEST_ASSERT(ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                params->authenticated);

    /* Verify that connection has correct security state. */
    TEST_ASSERT(ble_sm_test_sec_state.encrypted ==
                conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                conn->bhc_sec_state.authenticated);

    /* Verify the appropriate security material was persisted. */
    ble_sm_test_util_verify_persist(params, 1);

    ble_hs_test_util_conn_disconnect(2);
}

static void
ble_sm_test_util_us_sc_good_once(struct ble_sm_test_params *params)
{
    struct ble_sm_test_util_entity peer_entity;
    struct ble_sm_test_util_entity our_entity;
    struct ble_hs_conn *conn;

    ble_sm_test_util_init_good(params, 1, &conn, &our_entity, &peer_entity);
    ble_sm_test_util_us_sc_good_once_no_init(
        params, conn, &our_entity, &peer_entity);
}

void
ble_sm_test_util_us_sc_good(struct ble_sm_test_params *params)
{
    ble_addr_t peer_addr;
    int rc;

    /*** We are master. */

    /* We initiate pairing. */
    params->passkey_info.io_before_rx = 0;
    params->sec_req.authreq = 0;
    ble_sm_test_util_us_sc_good_once(params);

    /* Peer initiates with security request. */
    params->passkey_info.io_before_rx = 0;
    params->sec_req.authreq = params->pair_rsp.authreq;
    ble_sm_test_util_us_sc_good_once(params);

    /* Verify link can be restored via the encryption procedure. */
    ble_sm_test_util_bonding_all(params, 1);

    /* Verify programmatic unbonding. */
    peer_addr.type = ble_hs_misc_addr_type_to_id(params->resp_addr_type);
    memcpy(peer_addr.val, params->resp_id_addr, sizeof peer_addr.val);
    rc = ble_store_util_delete_peer(&peer_addr);
    TEST_ASSERT(rc == 0);

    TEST_ASSERT(ble_hs_test_util_num_our_secs() == 0);
    TEST_ASSERT(ble_hs_test_util_num_peer_secs() == 0);
}

static void
ble_sm_test_util_peer_sc_good_once_no_init(
    struct ble_sm_test_params *params,
    struct ble_hs_conn *conn,
    struct ble_sm_test_util_entity *our_entity,
    struct ble_sm_test_util_entity *peer_entity)
{
    int num_iters;
    int rc;
    int i;

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    if (params->sec_req.authreq != 0) {
        rc = ble_sm_slave_initiate(2);
        TEST_ASSERT(rc == 0);

        /* Ensure we sent the expected security request. */
        ble_sm_test_util_verify_tx_sec_req(&params->sec_req);
    }

    /* Receive a pair request from the peer. */
    ble_sm_test_util_rx_pair_req(2, peer_entity->pair_cmd, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected pair response. */
    ble_sm_test_util_verify_tx_pair_rsp(our_entity->pair_cmd);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a public key from the peer. */
    ble_sm_test_util_rx_public_key(2, peer_entity->public_key);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected public key. */
    ble_sm_test_util_verify_tx_public_key(our_entity->public_key);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    switch (params->pair_alg) {
    case BLE_SM_PAIR_ALG_PASSKEY:
        num_iters = 20;
        break;

    default:
        num_iters = 1;
        break;
    }

    ble_sm_test_util_io_check_pre(&params->passkey_info,
                                  BLE_SM_PROC_STATE_CONFIRM);

    for (i = 0; i < num_iters; i++) {
        if (params->pair_alg != BLE_SM_PAIR_ALG_JW      &&
            params->pair_alg != BLE_SM_PAIR_ALG_NUMCMP) {

            /* Receive a pair confirm from the peer. */
            ble_sm_test_util_rx_confirm(2, peer_entity->confirms + i);
            TEST_ASSERT(!conn->bhc_sec_state.encrypted);
            TEST_ASSERT(ble_sm_num_procs() == 1);
            ble_sm_test_util_io_inject_bad(
                2, params->passkey_info.passkey.action);

            if (i < num_iters - 1) {
                ble_sm_dbg_set_next_pair_rand(
                    our_entity->randoms[i + 1].value);
            }
        }

        if (i == 0) {
            ble_sm_test_util_io_check_post(&params->passkey_info,
                                           BLE_SM_PROC_STATE_CONFIRM);
        }

        /* Ensure we sent the expected pair confirm. */
        ble_sm_test_util_verify_tx_pair_confirm(our_entity->confirms + i);
        TEST_ASSERT(!conn->bhc_sec_state.encrypted);
        TEST_ASSERT(ble_sm_num_procs() == 1);
        ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

        /* Receive a pair random from the peer. */
        ble_sm_test_util_rx_random(2, peer_entity->randoms + i, 0);
        TEST_ASSERT(!conn->bhc_sec_state.encrypted);
        TEST_ASSERT(ble_sm_num_procs() == 1);
        ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

        /* Ensure we sent the expected pair random. */
        ble_sm_test_util_verify_tx_pair_random(our_entity->randoms + i);
        TEST_ASSERT(!conn->bhc_sec_state.encrypted);
        TEST_ASSERT(ble_sm_num_procs() == 1);
        ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    }

    ble_sm_test_util_io_check_pre(&params->passkey_info,
                                  BLE_SM_PROC_STATE_DHKEY_CHECK);

    /* Receive a dhkey check from the peer. */
    ble_sm_test_util_rx_dhkey_check(2, peer_entity->dhkey_check, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    ble_sm_test_util_io_check_post(&params->passkey_info,
                                   BLE_SM_PROC_STATE_DHKEY_CHECK);

    /* Ensure we sent the expected dhkey check. */
    ble_sm_test_util_verify_tx_dhkey_check(our_entity->dhkey_check);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a long term key request from the controller. */
    ble_sm_test_util_set_lt_key_req_reply_ack(0, 2);
    ble_sm_test_util_rx_lt_key_req(2, 0, 0);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Ensure we sent the expected long term key request reply command. */
    ble_sm_test_util_verify_tx_lt_key_req_reply(2, params->ltk);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive an encryption changed event. */
    ble_sm_test_util_rx_enc_change(2, 0, 1);

    /* Verify key material gets sent to peer. */
    ble_sm_test_util_verify_tx_keys(params, 0);

    /* Receive key material from peer. */
    ble_sm_test_util_rx_keys(params, 0);

    /* Pairing should now be complete. */
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify that security callback was executed. */
    TEST_ASSERT(ble_sm_test_gap_event_type == BLE_GAP_EVENT_ENC_CHANGE);
    TEST_ASSERT(ble_sm_test_gap_status == 0);
    TEST_ASSERT(ble_sm_test_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                params->authenticated);

    /* Verify that connection has correct security state. */
    TEST_ASSERT(ble_sm_test_sec_state.encrypted ==
                conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_test_sec_state.authenticated ==
                conn->bhc_sec_state.authenticated);

    /* Verify the appropriate security material was persisted. */
    ble_sm_test_util_verify_persist(params, 0);

    ble_hs_test_util_conn_disconnect(2);
}

static void
ble_sm_test_util_peer_sc_good_once(struct ble_sm_test_params *params)
{
    struct ble_sm_test_util_entity peer_entity;
    struct ble_sm_test_util_entity our_entity;
    struct ble_hs_conn *conn;

    ble_sm_test_util_init_good(params, 0, &conn, &our_entity, &peer_entity);
    ble_sm_test_util_peer_sc_good_once_no_init(
        params, conn, &our_entity, &peer_entity);
}

void
ble_sm_test_util_peer_sc_good(struct ble_sm_test_params *params)
{
    ble_addr_t peer_addr;
    int rc;

    /*** Peer is master. */

    /* Peer performs IO first; peer initiates pairing. */
    params->passkey_info.io_before_rx = 0;
    params->sec_req.authreq = 0;
    ble_sm_test_util_peer_sc_good_once(params);

    /* Peer performs IO first; we initiate with security request. */
    params->passkey_info.io_before_rx = 0;
    params->sec_req.authreq = params->pair_req.authreq;
    ble_sm_test_util_peer_sc_good_once(params);

    /* We perform IO first; peer initiates pairing. */
    params->passkey_info.io_before_rx = 1;
    params->sec_req.authreq = 0;
    ble_sm_test_util_peer_sc_good_once(params);

    /* We perform IO first; we initiate with security request. */
    params->passkey_info.io_before_rx = 1;
    params->sec_req.authreq = params->pair_req.authreq;
    ble_sm_test_util_peer_sc_good_once(params);

    /* Verify link can be restored via the encryption procedure. */
    ble_sm_test_util_bonding_all(params, 0);

    /* Verify repeating pairing event generated when peer attempts to pair
     * while bonded.
     */
    ble_sm_test_util_repeat_pairing(params, 1);

    /* Verify programmatic unbonding. */
    peer_addr.type = ble_hs_misc_addr_type_to_id(params->init_addr_type);
    memcpy(peer_addr.val, params->init_id_addr, sizeof peer_addr.val);
    rc = ble_store_util_delete_peer(&peer_addr);
    TEST_ASSERT(rc == 0);

    TEST_ASSERT(ble_hs_test_util_num_our_secs() == 0);
    TEST_ASSERT(ble_hs_test_util_num_peer_secs() == 0);
}

void
ble_sm_test_util_us_fail_inval(struct ble_sm_test_params *params)
{
    struct ble_hs_conn *conn;
    int rc;

    ble_sm_test_util_init();
    ble_hs_id_set_pub(params->resp_id_addr);

    ble_sm_dbg_set_next_pair_rand(((uint8_t[16]){0}));

    ble_hs_test_util_create_conn(2, params->init_id_addr,
                                 ble_sm_test_util_conn_cb,
                                 NULL);

    /* This test inspects and modifies the connection object after unlocking
     * the host mutex.  It is not OK for real code to do this, but this test
     * can assume the connection list is unchanging.
     */
    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    ble_hs_unlock();

    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Initiate the pairing procedure. */
    rc = ble_hs_test_util_security_initiate(2, 0);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure we sent the expected pair request. */
    ble_sm_test_util_verify_tx_pair_req(&params->pair_req);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 1);
    ble_sm_test_util_io_inject_bad(2, params->passkey_info.passkey.action);

    /* Receive a pair response from the peer. */
    ble_sm_test_util_rx_pair_rsp(
        2, &params->pair_rsp, BLE_HS_SM_US_ERR(params->pair_fail.reason));
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Ensure we sent the expected pair fail. */
    ble_sm_test_util_verify_tx_pair_fail(&params->pair_fail);
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify that security callback was not executed. */
    TEST_ASSERT(ble_sm_test_gap_event_type == -1);
    TEST_ASSERT(ble_sm_test_gap_status == -1);

    /* Verify that connection has correct security state. */
    TEST_ASSERT(!conn->bhc_sec_state.encrypted);
    TEST_ASSERT(!conn->bhc_sec_state.authenticated);
}

static void
ble_sm_test_util_repeat_pairing(struct ble_sm_test_params *params, int sc)
{
    struct ble_sm_test_util_entity peer_entity;
    struct ble_sm_test_util_entity our_entity;
    struct ble_hs_conn *conn;

    ble_sm_test_util_params_to_entities(params, 0, &our_entity, &peer_entity);

    ble_sm_test_repeat_pairing.params = *params;
    ble_hs_id_set_pub(our_entity.id_addr);
    ble_sm_dbg_set_next_pair_rand(our_entity.randoms[0].value);
    ble_sm_dbg_set_next_ediv(our_entity.ediv);
    ble_sm_dbg_set_next_master_id_rand(our_entity.rand_num);
    ble_sm_dbg_set_next_ltk(our_entity.ltk);
    ble_hs_test_util_set_our_irk(our_entity.id_info->irk, 0, 0);
    ble_sm_dbg_set_next_csrk(our_entity.sign_info->sig_key);

    ble_hs_test_util_create_rpa_conn(2, our_entity.addr_type, our_entity.rpa,
                                     peer_entity.addr_type,
                                     peer_entity.id_addr, peer_entity.rpa,
                                     BLE_HS_TEST_CONN_FEAT_ALL,
                                     ble_sm_test_util_conn_cb,
                                     NULL);
    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    conn->bhc_flags &= ~BLE_HS_CONN_F_MASTER;
    ble_hs_unlock();

    ble_hs_test_util_prev_tx_queue_clear();

    /* First repeat pairing event:  retry;
     * Second repeat pairing event: ignore.
     */
    ble_sm_test_repeat_pairing.rc = BLE_GAP_REPEAT_PAIRING_RETRY;
    ble_sm_test_repeat_pairing.next_rc = BLE_GAP_REPEAT_PAIRING_IGNORE;

    /* Receive a pair request from the peer. */
    ble_sm_test_util_rx_pair_req(2, peer_entity.pair_cmd, BLE_HS_EALREADY);

    /* Verify repeat pairing event got reported twice. */
    TEST_ASSERT(ble_sm_test_repeat_pairing.num_calls == 2);

    /* Verify no pairing procedures in progress. */
    TEST_ASSERT(ble_sm_num_procs() == 0);

    /* Verify no SM messages were sent. */
    TEST_ASSERT(ble_hs_test_util_prev_tx_dequeue() == NULL);

    /*** Receive another pairing request. */

    ble_sm_test_repeat_pairing.num_calls = 0;

    /* First repeat pairing event: erase and retry. */
    ble_sm_test_repeat_pairing.rc = BLE_GAP_REPEAT_PAIRING_RETRY;
    ble_sm_test_repeat_pairing.erase_on_retry = 1;

    ble_hs_lock();
    conn = ble_hs_conn_find(2);
    TEST_ASSERT_FATAL(conn != NULL);
    ble_hs_unlock();

    /* Receive a pair request from the peer; verify pairing procedure completes
     * successfully.
     */
    if (!sc) {
        ble_sm_test_util_peer_lgcy_good_once_no_init(
            params, conn, &our_entity, &peer_entity);
    } else {
        ble_sm_test_util_peer_sc_good_once_no_init(
            params, conn, &our_entity, &peer_entity);
    }

    /* Verify repeat pairing event got reported once. */
    TEST_ASSERT(ble_sm_test_repeat_pairing.num_calls == 1);
}
