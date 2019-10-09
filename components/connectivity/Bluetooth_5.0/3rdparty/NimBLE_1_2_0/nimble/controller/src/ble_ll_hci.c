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
#include "os/os.h"
#include "nimble/ble.h"
#include "nimble/nimble_opt.h"
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"
#include "controller/ble_hw.h"
#include "controller/ble_ll_adv.h"
#include "controller/ble_ll_scan.h"
#include "controller/ble_ll.h"
#include "controller/ble_ll_hci.h"
#include "controller/ble_ll_whitelist.h"
#include "controller/ble_ll_resolv.h"
#include "controller/ble_ll_sync.h"
#include "ble_ll_conn_priv.h"
#if MYNEWT_VAL(BLE_LL_DBG_HCI_CMD_PIN) >= 0 || MYNEWT_VAL(BLE_LL_DBG_HCI_EV_PIN) >= 0
#include "hal/hal_gpio.h"
#endif

#if MYNEWT_VAL(BLE_LL_DIRECT_TEST_MODE)
#include "ble_ll_dtm_priv.h"
#endif

static void ble_ll_hci_cmd_proc(struct ble_npl_event *ev);

/* OS event to enqueue command */
static struct ble_npl_event g_ble_ll_hci_cmd_ev;

/* LE event mask */
static uint8_t g_ble_ll_hci_le_event_mask[BLE_HCI_SET_LE_EVENT_MASK_LEN];
static uint8_t g_ble_ll_hci_event_mask[BLE_HCI_SET_EVENT_MASK_LEN];
static uint8_t g_ble_ll_hci_event_mask2[BLE_HCI_SET_EVENT_MASK_LEN];

static int16_t rx_path_pwr_compensation;
static int16_t tx_path_pwr_compensation;

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
static enum {
    ADV_MODE_ANY,
    ADV_MODE_LEGACY,
    ADV_MODE_EXT,
} hci_adv_mode;

bool ble_ll_hci_adv_mode_ext(void)
{
    return hci_adv_mode == ADV_MODE_EXT;
}
#else
bool
ble_ll_hci_adv_mode_ext(void)
{
    return false;
}
#endif

/**
 * ll hci get num cmd pkts
 *
 * Returns the number of command packets that the host is allowed to send
 * to the controller.
 *
 * @return uint8_t
 */
static uint8_t
ble_ll_hci_get_num_cmd_pkts(void)
{
    return BLE_LL_CFG_NUM_HCI_CMD_PKTS;
}

/**
 * Send an event to the host.
 *
 * @param evbuf Pointer to event buffer to send
 *
 * @return int 0: success; -1 otherwise.
 */
int
ble_ll_hci_event_send(uint8_t *evbuf)
{
    int rc;

#if MYNEWT_VAL(BLE_LL_DBG_HCI_EV_PIN) >= 0
    hal_gpio_write(MYNEWT_VAL(BLE_LL_DBG_HCI_EV_PIN), 1);
#endif

    BLE_LL_ASSERT(BLE_HCI_EVENT_HDR_LEN + evbuf[1] <= BLE_LL_MAX_EVT_LEN);

    /* Count number of events sent */
    STATS_INC(ble_ll_stats, hci_events_sent);

    /* Send the event to the host */
    rc = ble_hci_trans_ll_evt_tx(evbuf);

#if MYNEWT_VAL(BLE_LL_DBG_HCI_EV_PIN) >= 0
    hal_gpio_write(MYNEWT_VAL(BLE_LL_DBG_HCI_EV_PIN), 0);
#endif

    return rc;
}

/**
 * Created and sends a command complete event with the no-op opcode to the
 * host.
 *
 * @return int 0: ok, ble error code otherwise.
 */
int
ble_ll_hci_send_noop(void)
{
    int rc;
    uint8_t *evbuf;
    uint16_t opcode;

    evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
    if (evbuf) {
        /* Create a command complete event with a NO-OP opcode */
        opcode = 0;
        evbuf[0] = BLE_HCI_EVCODE_COMMAND_COMPLETE;
        evbuf[1] = 3;
        evbuf[2] = ble_ll_hci_get_num_cmd_pkts();
        put_le16(evbuf + 3, opcode);
        ble_ll_hci_event_send(evbuf);
        rc = BLE_ERR_SUCCESS;
    } else {
        rc = BLE_ERR_MEM_CAPACITY;
    }

    return rc;
}

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION)
/**
 * LE encrypt command
 *
 * @param cmdbuf
 * @param rspbuf
 * @param rsplen
 *
 * @return int
 */
static int
ble_ll_hci_le_encrypt(uint8_t *cmdbuf, uint8_t *rspbuf, uint8_t *rsplen)
{
    int rc;
    struct ble_encryption_block ecb;

    /* Call the link layer to encrypt the data */
    swap_buf(ecb.key, cmdbuf, BLE_ENC_BLOCK_SIZE);
    swap_buf(ecb.plain_text, cmdbuf + BLE_ENC_BLOCK_SIZE, BLE_ENC_BLOCK_SIZE);
    rc = ble_hw_encrypt_block(&ecb);
    if (!rc) {
        swap_buf(rspbuf, ecb.cipher_text, BLE_ENC_BLOCK_SIZE);
        *rsplen = BLE_ENC_BLOCK_SIZE;
        rc = BLE_ERR_SUCCESS;
    } else {
        *rsplen = 0;
        rc = BLE_ERR_CTLR_BUSY;
    }
    return rc;
}
#endif

/**
 * LE rand command
 *
 * @param cmdbuf
 * @param rspbuf
 * @param rsplen
 *
 * @return int
 */
static int
ble_ll_hci_le_rand(uint8_t *rspbuf, uint8_t *rsplen)
{
    int rc;

    rc = ble_ll_rand_data_get(rspbuf, BLE_HCI_LE_RAND_LEN);
    *rsplen = BLE_HCI_LE_RAND_LEN;
    return rc;
}

/**
 * Read local version
 *
 * @param rspbuf
 * @param rsplen
 *
 * @return int
 */
static int
ble_ll_hci_rd_local_version(uint8_t *rspbuf, uint8_t *rsplen)
{
    uint16_t hci_rev;
    uint16_t lmp_subver;
    uint16_t mfrg;

    hci_rev = 0;
    lmp_subver = 0;
    mfrg = MYNEWT_VAL(BLE_LL_MFRG_ID);

    /* Place the data packet length and number of packets in the buffer */
    rspbuf[0] = BLE_HCI_VER_BCS_5_0;
    put_le16(rspbuf + 1, hci_rev);
    rspbuf[3] = BLE_LMP_VER_BCS_5_0;
    put_le16(rspbuf + 4, mfrg);
    put_le16(rspbuf + 6, lmp_subver);
    *rsplen = BLE_HCI_RD_LOC_VER_INFO_RSPLEN;
    return BLE_ERR_SUCCESS;
}

/**
 * Read local supported features
 *
 * @param rspbuf
 * @param rsplen
 *
 * @return int
 */
static int
ble_ll_hci_rd_local_supp_feat(uint8_t *rspbuf, uint8_t *rsplen)
{
    /*
     * The only two bits we set here currently are:
     *      BR/EDR not supported        (bit 5)
     *      LE supported (controller)   (bit 6)
     */
    memset(rspbuf, 0, BLE_HCI_RD_LOC_SUPP_FEAT_RSPLEN);
    rspbuf[4] = 0x60;
    *rsplen = BLE_HCI_RD_LOC_SUPP_FEAT_RSPLEN;
    return BLE_ERR_SUCCESS;
}

/**
 * Read local supported commands
 *
 * @param rspbuf
 * @param rsplen
 *
 * @return int
 */
static int
ble_ll_hci_rd_local_supp_cmd(uint8_t *rspbuf, uint8_t *rsplen)
{
    memset(rspbuf, 0, BLE_HCI_RD_LOC_SUPP_CMD_RSPLEN);
    memcpy(rspbuf, g_ble_ll_supp_cmds, sizeof(g_ble_ll_supp_cmds));
    *rsplen = BLE_HCI_RD_LOC_SUPP_CMD_RSPLEN;
    return BLE_ERR_SUCCESS;
}

/**
 * Called to read the public device address of the device
 *
 *
 * @param rspbuf
 * @param rsplen
 *
 * @return int
 */
static int
ble_ll_hci_rd_bd_addr(uint8_t *rspbuf, uint8_t *rsplen)
{
    /*
     * XXX: for now, assume we always have a public device address. If we
     * dont, we should set this to zero
     */
    memcpy(rspbuf, g_dev_addr, BLE_DEV_ADDR_LEN);
    *rsplen = BLE_DEV_ADDR_LEN;
    return BLE_ERR_SUCCESS;
}

/**
 * ll hci set le event mask
 *
 * Called when the LL controller receives a set LE event mask command.
 *
 * Context: Link Layer task (HCI command parser)
 *
 * @param cmdbuf Pointer to command buf.
 *
 * @return int BLE_ERR_SUCCESS. Does not return any errors.
 */
static int
ble_ll_hci_set_le_event_mask(uint8_t *cmdbuf)
{
    /* Copy the data into the event mask */
    memcpy(g_ble_ll_hci_le_event_mask, cmdbuf, BLE_HCI_SET_LE_EVENT_MASK_LEN);
    return BLE_ERR_SUCCESS;
}

/**
 * HCI read buffer size command. Returns the ACL data packet length and
 * num data packets.
 *
 * @param rspbuf Pointer to response buffer
 * @param rsplen Length of response buffer
 *
 * @return int BLE error code
 */
static int
ble_ll_hci_le_read_bufsize(uint8_t *rspbuf, uint8_t *rsplen)
{
    /* Place the data packet length and number of packets in the buffer */
    put_le16(rspbuf, g_ble_ll_data.ll_acl_pkt_size);
    rspbuf[2] = g_ble_ll_data.ll_num_acl_pkts;
    *rsplen = BLE_HCI_RD_BUF_SIZE_RSPLEN;
    return BLE_ERR_SUCCESS;
}

#if (BLE_LL_BT5_PHY_SUPPORTED == 1)
/**
 * Checks the preferred phy masks for validity and places the preferred masks
 * in the input phy masks
 *
 * @param cmdbuf Pointer to command buffer where phy masks are located
 * @param txphy Pointer to output tx phy mask
 * @param rxphy Pointer to output rx phy mask
 *
 * @return int BLE_ERR_SUCCESS or BLE_ERR_INV_HCI_CMD_PARMS or BLE_ERR_UNSUPPORTED
 */
int
ble_ll_hci_chk_phy_masks(uint8_t *cmdbuf, uint8_t *txphy, uint8_t *rxphy)
{
    uint8_t all_phys;
    uint8_t rx_phys;
    uint8_t tx_phys;

    /* Check for RFU */
    if ((cmdbuf[1] & ~BLE_HCI_LE_PHY_PREF_MASK_ALL) ||
                    (cmdbuf[2] & ~BLE_HCI_LE_PHY_PREF_MASK_ALL)) {
        return BLE_ERR_UNSUPPORTED;
    }

    /* Check for valid values */
    all_phys = cmdbuf[0];
    tx_phys = cmdbuf[1] & BLE_HCI_LE_PHY_PREF_MASK_ALL;
    rx_phys = cmdbuf[2] & BLE_HCI_LE_PHY_PREF_MASK_ALL;

    if ((!(all_phys & BLE_HCI_LE_PHY_NO_TX_PREF_MASK) && (tx_phys == 0)) ||
        (!(all_phys & BLE_HCI_LE_PHY_NO_RX_PREF_MASK) && (rx_phys == 0))) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    /* If phy not supported, return error */
#if !MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_2M_PHY)
    if((tx_phys & BLE_HCI_LE_PHY_2M_PREF_MASK) ||
                    (rx_phys & BLE_HCI_LE_PHY_2M_PREF_MASK)) {
        return BLE_ERR_UNSUPPORTED;
    }
#endif
#if !MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
    if ((tx_phys & BLE_HCI_LE_PHY_CODED_PREF_MASK) ||
                    (rx_phys & BLE_HCI_LE_PHY_CODED_PREF_MASK)) {
        return BLE_ERR_UNSUPPORTED;
    }
#endif
    /* Set the default PHY preferences */
    if (all_phys & BLE_HCI_LE_PHY_NO_TX_PREF_MASK) {
        tx_phys = BLE_HCI_LE_PHY_PREF_MASK_ALL;
    }
    *txphy = tx_phys;

    if (all_phys & BLE_HCI_LE_PHY_NO_RX_PREF_MASK) {
        rx_phys = BLE_HCI_LE_PHY_PREF_MASK_ALL;
    }
    *rxphy = rx_phys;

    return BLE_ERR_SUCCESS;
}

/**
 * Set PHY preferences for connection
 *
 * @param cmdbuf
 *
 * @return int
 */
static int
ble_ll_hci_le_set_def_phy(uint8_t *cmdbuf)
{
    int rc;

    rc = ble_ll_hci_chk_phy_masks(cmdbuf, &g_ble_ll_data.ll_pref_tx_phys,
                                  &g_ble_ll_data.ll_pref_rx_phys);
    return rc;
}
#endif

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_DATA_LEN_EXT)
/**
 * HCI write suggested default data length command.
 *
 * This command is used by the host to change the initial max tx octets/time
 * for all connections. Note that if the controller does not support the
 * requested times no error is returned; the controller simply ignores the
 * request (but remembers what the host requested for the read suggested
 * default data length command). The spec allows for the controller to
 * disregard the host.
 *
 * @param rspbuf Pointer to response buffer
 * @param rsplen Length of response buffer
 *
 * @return int BLE error code
 */
static int
ble_ll_hci_le_wr_sugg_data_len(uint8_t *cmdbuf)
{
    int rc;
    uint16_t tx_oct;
    uint16_t tx_time;

    /* Get suggested octets and time */
    tx_oct = get_le16(cmdbuf);
    tx_time = get_le16(cmdbuf + 2);

    /* If valid, write into suggested and change connection initial times */
    if (ble_ll_chk_txrx_octets(tx_oct) && ble_ll_chk_txrx_time(tx_time)) {
        g_ble_ll_conn_params.sugg_tx_octets = (uint8_t)tx_oct;
        g_ble_ll_conn_params.sugg_tx_time = tx_time;

        /*
         * We can disregard host suggestion, but we are a nice controller so
         * let's use host suggestion, unless they exceed max supported values
         * in which case we just use our max.
         */
        g_ble_ll_conn_params.conn_init_max_tx_octets =
                        min(tx_oct, g_ble_ll_conn_params.supp_max_tx_octets);
        g_ble_ll_conn_params.conn_init_max_tx_time =
                        min(tx_time, g_ble_ll_conn_params.supp_max_tx_time);

        rc = BLE_ERR_SUCCESS;
    } else {
        rc = BLE_ERR_INV_HCI_CMD_PARMS;
    }

    return rc;
}

/**
 * HCI read suggested default data length command. Returns the controllers
 * initial max tx octet/time.
 *
 * @param rspbuf Pointer to response buffer
 * @param rsplen Length of response buffer
 *
 * @return int BLE error code
 */
static int
ble_ll_hci_le_rd_sugg_data_len(uint8_t *rspbuf, uint8_t *rsplen)
{
    /* Place the data packet length and number of packets in the buffer */
    put_le16(rspbuf, g_ble_ll_conn_params.sugg_tx_octets);
    put_le16(rspbuf + 2, g_ble_ll_conn_params.sugg_tx_time);
    *rsplen = BLE_HCI_RD_SUGG_DATALEN_RSPLEN;
    return BLE_ERR_SUCCESS;
}

/**
 * HCI read maximum data length command. Returns the controllers max supported
 * rx/tx octets/times.
 *
 * @param rspbuf Pointer to response buffer
 * @param rsplen Length of response buffer
 *
 * @return int BLE error code
 */
static int
ble_ll_hci_le_rd_max_data_len(uint8_t *rspbuf, uint8_t *rsplen)
{
    /* Place the data packet length and number of packets in the buffer */
    put_le16(rspbuf, g_ble_ll_conn_params.supp_max_tx_octets);
    put_le16(rspbuf + 2, g_ble_ll_conn_params.supp_max_tx_time);
    put_le16(rspbuf + 4, g_ble_ll_conn_params.supp_max_rx_octets);
    put_le16(rspbuf + 6, g_ble_ll_conn_params.supp_max_rx_time);
    *rsplen = BLE_HCI_RD_MAX_DATALEN_RSPLEN;
    return BLE_ERR_SUCCESS;
}
#endif

/**
 * HCI read local supported features command. Returns the features
 * supported by the controller.
 *
 * @param rspbuf Pointer to response buffer
 * @param rsplen Length of response buffer
 *
 * @return int BLE error code
 */
static int
ble_ll_hci_le_read_local_features(uint8_t *rspbuf, uint8_t *rsplen)
{
    /* Add list of supported features. */
    memset(rspbuf, 0, BLE_HCI_RD_LOC_SUPP_FEAT_RSPLEN);
    put_le32(rspbuf, ble_ll_read_supp_features());

    *rsplen = BLE_HCI_RD_LOC_SUPP_FEAT_RSPLEN;
    return BLE_ERR_SUCCESS;
}

/**
 * HCI read local supported states command. Returns the states
 * supported by the controller.
 *
 * @param rspbuf Pointer to response buffer
 * @param rsplen Length of response buffer
 *
 * @return int BLE error code
 */
static int
ble_ll_hci_le_read_supp_states(uint8_t *rspbuf, uint8_t *rsplen)
{
    uint64_t supp_states;

    /* Add list of supported states. */
    supp_states = ble_ll_read_supp_states();
    put_le64(rspbuf, supp_states);
    *rsplen = BLE_HCI_RD_SUPP_STATES_RSPLEN;
    return BLE_ERR_SUCCESS;
}

/**
 * Checks to see if a LE event has been disabled by the host.
 *
 * @param subev Sub-event code of the LE Meta event. Note that this can
 * be a value from 0 to 63, inclusive.
 *
 * @return uint8_t 0: event is not enabled; otherwise event is enabled.
 */
uint8_t
ble_ll_hci_is_le_event_enabled(int subev)
{
    uint8_t enabled;
    uint8_t bytenum;
    uint8_t bitmask;
    int bitpos;

    /* The LE meta event must be enabled for any LE event to be enabled */
    enabled = 0;
    bitpos = subev - 1;
    if (g_ble_ll_hci_event_mask[7] & 0x20) {
        bytenum = bitpos / 8;
        bitmask = 1 << (bitpos & 0x7);
        enabled = g_ble_ll_hci_le_event_mask[bytenum] & bitmask;
    }

    return enabled;
}

/**
 * Checks to see if an event has been disabled by the host.
 *
 * NOTE: there are two "pages" of event masks; the first page is for event
 * codes between 0 and 63 and the second page is for event codes 64 and
 * greater.
 *
 * @param evcode This is the event code for the event.
 *
 * @return uint8_t 0: event is not enabled; otherwise event is enabled.
 */
uint8_t
ble_ll_hci_is_event_enabled(int evcode)
{
    uint8_t enabled;
    uint8_t bytenum;
    uint8_t bitmask;
    uint8_t *evptr;
    int bitpos;

    if (evcode >= 64) {
        evptr = &g_ble_ll_hci_event_mask2[0];
        bitpos = evcode - 64;
    } else {
        evptr = &g_ble_ll_hci_event_mask[0];
        bitpos = evcode - 1;
    }

    bytenum = bitpos / 8;
    bitmask = 1 << (bitpos & 0x7);
    enabled = evptr[bytenum] & bitmask;

    return enabled;
}

/**
 * Called to determine if the reply to the command should be a command complete
 * event or a command status event.
 *
 * @param ocf
 *
 * @return int 0: return command complete; 1: return command status event
 */
static int
ble_ll_hci_le_cmd_send_cmd_status(uint16_t ocf)
{
    int rc;

    switch (ocf) {
    case BLE_HCI_OCF_LE_RD_REM_FEAT:
    case BLE_HCI_OCF_LE_CREATE_CONN:
    case BLE_HCI_OCF_LE_EXT_CREATE_CONN:
    case BLE_HCI_OCF_LE_CONN_UPDATE:
    case BLE_HCI_OCF_LE_START_ENCRYPT:
    case BLE_HCI_OCF_LE_RD_P256_PUBKEY:
    case BLE_HCI_OCF_LE_GEN_DHKEY:
    case BLE_HCI_OCF_LE_SET_PHY:
    case BLE_HCI_OCF_LE_PERIODIC_ADV_CREATE_SYNC:
        rc = 1;
        break;
    default:
        rc = 0;
        break;
    }
    return rc;
}

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
/** HCI LE read maximum advertising data length command. Returns the controllers
* max supported advertising data length;
*
* @param rspbuf Pointer to response buffer
* @param rsplen Length of response buffer
*
* @return int BLE error code
*/
static int
ble_ll_adv_rd_max_adv_data_len(uint8_t *rspbuf, uint8_t *rsplen)
{
    put_le16(rspbuf, BLE_ADV_DATA_MAX_LEN);
    *rsplen = BLE_HCI_RD_MAX_ADV_DATA_LEN;
    return BLE_ERR_SUCCESS;
}

/**
 * HCI LE read number of supported advertising sets
 *
 * @param rspbuf Pointer to response buffer
 * @param rsplen Length of response buffer
 *
 * @return int BLE error code
 */
static int
ble_ll_adv_rd_sup_adv_sets(uint8_t *rspbuf, uint8_t *rsplen)
{
    rspbuf[0] = BLE_ADV_INSTANCES;
    *rsplen = BLE_HCI_RD_NR_SUP_ADV_SETS;
    return BLE_ERR_SUCCESS;
}

static int
ble_ll_ext_adv_set_remove(uint8_t *cmd)
{
    return ble_ll_adv_remove(cmd[0]);
}

static bool
ble_ll_is_valid_adv_mode(uint8_t ocf)
{
    /*
     * If, since the last power-on or reset, the Host has ever issued a legacy
     * advertising command and then issues an extended advertising command, or
     * has ever issued an extended advertising command and then issues a legacy
     * advertising command, the Controller shall return the error code Command
     * Disallowed (0x0C).
    */

    switch(ocf) {
    case BLE_HCI_OCF_LE_CREATE_CONN:
    case BLE_HCI_OCF_LE_SET_ADV_PARAMS:
    case BLE_HCI_OCF_LE_SET_ADV_ENABLE:
    case BLE_HCI_OCF_LE_SET_ADV_DATA:
    case BLE_HCI_OCF_LE_SET_SCAN_PARAMS:
    case BLE_HCI_OCF_LE_SET_SCAN_ENABLE:
    case BLE_HCI_OCF_LE_SET_SCAN_RSP_DATA:
    case BLE_HCI_OCF_LE_RD_ADV_CHAN_TXPWR:
        if (hci_adv_mode == ADV_MODE_EXT) {
            return false;
        }

        hci_adv_mode = ADV_MODE_LEGACY;
        break;
    case BLE_HCI_OCF_LE_EXT_CREATE_CONN:
    case BLE_HCI_OCF_LE_SET_EXT_ADV_DATA:
    case BLE_HCI_OCF_LE_SET_EXT_ADV_ENABLE:
    case BLE_HCI_OCF_LE_SET_EXT_ADV_PARAM:
    case BLE_HCI_OCF_LE_SET_EXT_SCAN_ENABLE:
    case BLE_HCI_OCF_LE_SET_EXT_SCAN_PARAM:
    case BLE_HCI_OCF_LE_SET_EXT_SCAN_RSP_DATA:
    case BLE_HCI_OCF_LE_RD_MAX_ADV_DATA_LEN:
    case BLE_HCI_OCF_LE_RD_NUM_OF_ADV_SETS:
    case BLE_HCI_OCF_LE_REMOVE_ADV_SET:
    case BLE_HCI_OCF_LE_CLEAR_ADV_SETS:
    case BLE_HCI_OCF_LE_SET_PERIODIC_ADV_PARAMS:
    case BLE_HCI_OCF_LE_SET_PERIODIC_ADV_DATA:
    case BLE_HCI_OCF_LE_SET_PERIODIC_ADV_ENABLE:
    case BLE_HCI_OCF_LE_PERIODIC_ADV_CREATE_SYNC:
    case BLE_HCI_OCF_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL:
    case BLE_HCI_OCF_LE_PERIODIC_ADV_TERM_SYNC:
    case BLE_HCI_OCF_LE_ADD_DEV_TO_PERIODIC_ADV_LIST:
    case BLE_HCI_OCF_LE_REM_DEV_FROM_PERIODIC_ADV_LIST:
    case BLE_HCI_OCF_LE_CLEAR_PERIODIC_ADV_LIST:
    case BLE_HCI_OCF_LE_RD_PERIODIC_ADV_LIST_SIZE:
        if (hci_adv_mode == ADV_MODE_LEGACY) {
            return false;
        }

        hci_adv_mode = ADV_MODE_EXT;
        break;
    default:
        break;
    }

    return true;
}
#endif

static int
ble_ll_read_tx_power(uint8_t *rspbuf, uint8_t *rsplen)
{
    int8_t min;
    int8_t max;

    min = ble_phy_txpower_round(-127);
    max = ble_phy_txpower_round(126);

    rspbuf[0] = min;
    rspbuf[1] = max;

    *rsplen = 2;

    return BLE_ERR_SUCCESS;
}

static int
ble_ll_read_rf_path_compensation(uint8_t *rspbuf, uint8_t *rsplen)
{
    put_le16(rspbuf, tx_path_pwr_compensation);
    *rsplen = sizeof(int16_t);

    put_le16(rspbuf + 2, rx_path_pwr_compensation);
    *rsplen += sizeof(int16_t);

    return BLE_ERR_SUCCESS;
}

static int
ble_ll_write_rf_path_compensation(const uint8_t *cmdbuf)
{
    int16_t rx;
    int16_t tx;

    tx = get_le16(cmdbuf);
    rx = get_le16(cmdbuf + 2);

    if ((tx < -1280) || (tx > 1280) || (rx < -1280) || (rx > 1280)) {
        return BLE_ERR_INV_HCI_CMD_PARMS;
    }

    tx_path_pwr_compensation = tx;
    rx_path_pwr_compensation = rx;

    ble_phy_set_rx_pwr_compensation(rx_path_pwr_compensation / 10);

    return BLE_ERR_SUCCESS;
}

int8_t
ble_ll_get_tx_pwr_compensation(void)
{
    return tx_path_pwr_compensation / 10;
}

/**
 * Process a LE command sent from the host to the controller. The HCI command
 * has a 3 byte command header followed by data. The header is:
 *  -> opcode (2 bytes)
 *  -> Length of parameters (1 byte; does include command header bytes).
 *
 * @param cmdbuf Pointer to command buffer. Points to start of command header.
 * @param ocf    Opcode command field.
 * @param *rsplen Pointer to length of response
 *
 * @return int  This function returns a BLE error code. If a command status
 *              event should be returned as opposed to command complete,
 *              256 gets added to the return value.
 */
static int
ble_ll_hci_le_cmd_proc(uint8_t *cmdbuf, uint16_t ocf, uint8_t *rsplen,
                                            ble_ll_hci_post_cmd_complete_cb *cb)
{
    int rc;
    uint8_t len;
    uint8_t *rspbuf;

    /* Assume error; if all pass rc gets set to 0 */
    rc = BLE_ERR_INV_HCI_CMD_PARMS;

    /* Get length from command */
    len = cmdbuf[sizeof(uint16_t)];

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
    if (!ble_ll_is_valid_adv_mode(ocf)) {
        rc = BLE_ERR_CMD_DISALLOWED;
        goto ll_hci_le_cmd_exit;
    }
#endif

    /*
     * The command response pointer points into the same buffer as the
     * command data itself. That is fine, as each command reads all the data
     * before crafting a response.
     */
    rspbuf = cmdbuf + BLE_HCI_EVENT_CMD_COMPLETE_MIN_LEN;

    /* Move past HCI command header */
    cmdbuf += BLE_HCI_CMD_HDR_LEN;

    switch (ocf) {
    case BLE_HCI_OCF_LE_SET_EVENT_MASK:
        if (len == BLE_HCI_SET_LE_EVENT_MASK_LEN) {
            rc = ble_ll_hci_set_le_event_mask(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_RD_BUF_SIZE:
        if (len == BLE_HCI_RD_BUF_SIZE_LEN) {
            rc = ble_ll_hci_le_read_bufsize(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_RD_LOC_SUPP_FEAT:
        if (len == 0) {
            rc = ble_ll_hci_le_read_local_features(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_SET_RAND_ADDR:
        if (len == BLE_HCI_SET_RAND_ADDR_LEN) {
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
            rc = ble_ll_set_random_addr(cmdbuf, hci_adv_mode == ADV_MODE_EXT);
#else
            rc = ble_ll_set_random_addr(cmdbuf, false);
#endif
        }
        break;
    case BLE_HCI_OCF_LE_SET_ADV_PARAMS:
        if (len == BLE_HCI_SET_ADV_PARAM_LEN) {
            rc = ble_ll_adv_set_adv_params(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_RD_ADV_CHAN_TXPWR:
        if (len == 0) {
            rc = ble_ll_adv_read_txpwr(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_SET_ADV_DATA:
        if (len == BLE_HCI_SET_ADV_DATA_LEN) {
            rc = ble_ll_adv_set_adv_data(cmdbuf, len, 0,
                                     BLE_HCI_LE_SET_DATA_OPER_COMPLETE);
        }
        break;
    case BLE_HCI_OCF_LE_SET_SCAN_RSP_DATA:
        if (len == BLE_HCI_SET_SCAN_RSP_DATA_LEN) {
            rc = ble_ll_adv_set_scan_rsp_data(cmdbuf, len, 0,
                                BLE_HCI_LE_SET_DATA_OPER_COMPLETE);
        }
        break;
    case BLE_HCI_OCF_LE_SET_ADV_ENABLE:
        if (len == BLE_HCI_SET_ADV_ENABLE_LEN) {
            rc = ble_ll_adv_set_enable(0, cmdbuf[0], -1, 0);
        }
        break;
    case BLE_HCI_OCF_LE_SET_SCAN_PARAMS:
        if (len == BLE_HCI_SET_SCAN_PARAM_LEN) {
            rc = ble_ll_scan_set_scan_params(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_SET_SCAN_ENABLE:
        if (len == BLE_HCI_SET_SCAN_ENABLE_LEN) {
            rc = ble_ll_scan_set_enable(cmdbuf, 0);
        }
        break;
    case BLE_HCI_OCF_LE_CREATE_CONN:
        if (len == BLE_HCI_CREATE_CONN_LEN) {
            rc = ble_ll_conn_create(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_CREATE_CONN_CANCEL:
        if (len == 0) {
            rc = ble_ll_conn_create_cancel(cb);
        }
        break;
    case BLE_HCI_OCF_LE_RD_WHITE_LIST_SIZE:
        if (len == 0) {
            rc = ble_ll_whitelist_read_size(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_CLEAR_WHITE_LIST:
        if (len == 0) {
            rc = ble_ll_whitelist_clear();
        }
        break;
    case BLE_HCI_OCF_LE_ADD_WHITE_LIST:
        if (len == BLE_HCI_ADD_WHITE_LIST_LEN) {
            rc = ble_ll_whitelist_add(cmdbuf + 1, cmdbuf[0]);
        }
        break;
    case BLE_HCI_OCF_LE_RMV_WHITE_LIST:
        if (len == BLE_HCI_RMV_WHITE_LIST_LEN) {
            rc = ble_ll_whitelist_rmv(cmdbuf + 1, cmdbuf[0]);
        }
        break;
    case BLE_HCI_OCF_LE_CONN_UPDATE:
        if (len == BLE_HCI_CONN_UPDATE_LEN) {
            rc = ble_ll_conn_hci_update(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_SET_HOST_CHAN_CLASS:
        if (BLE_HCI_SET_HOST_CHAN_CLASS_LEN) {
            rc = ble_ll_conn_hci_set_chan_class(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_RD_CHAN_MAP:
        if (len == BLE_HCI_RD_CHANMAP_LEN) {
            rc = ble_ll_conn_hci_rd_chan_map(cmdbuf, rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_RD_REM_FEAT:
        if (len == BLE_HCI_CONN_RD_REM_FEAT_LEN) {
            rc = ble_ll_conn_hci_read_rem_features(cmdbuf);
        }
        break;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION)
    case BLE_HCI_OCF_LE_ENCRYPT:
        if (len == BLE_HCI_LE_ENCRYPT_LEN) {
            rc = ble_ll_hci_le_encrypt(cmdbuf, rspbuf, rsplen);
        }
        break;
#endif
    case BLE_HCI_OCF_LE_RAND:
        if (len == 0) {
            rc = ble_ll_hci_le_rand(rspbuf, rsplen);
        }
        break;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION)
    case BLE_HCI_OCF_LE_START_ENCRYPT:
        if (len == BLE_HCI_LE_START_ENCRYPT_LEN) {
            rc = ble_ll_conn_hci_le_start_encrypt(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_LT_KEY_REQ_REPLY:
        if (len == BLE_HCI_LT_KEY_REQ_REPLY_LEN) {
            rc = ble_ll_conn_hci_le_ltk_reply(cmdbuf, rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_LT_KEY_REQ_NEG_REPLY:
        if (len == BLE_HCI_LT_KEY_REQ_NEG_REPLY_LEN) {
            rc = ble_ll_conn_hci_le_ltk_neg_reply(cmdbuf, rspbuf, rsplen);
        }
        break;
#endif
    case BLE_HCI_OCF_LE_RD_SUPP_STATES :
        if (len == 0) {
            rc = ble_ll_hci_le_read_supp_states(rspbuf, rsplen);
        }
        break;
#if MYNEWT_VAL(BLE_LL_DIRECT_TEST_MODE)
    case BLE_HCI_OCF_LE_TX_TEST:
        if (len == BLE_HCI_TX_TEST_LEN) {
            rc = ble_ll_dtm_tx_test(cmdbuf, false);
        }
        break;
    case BLE_HCI_OCF_LE_RX_TEST:
        if (len == BLE_HCI_RX_TEST_LEN) {
            rc = ble_ll_dtm_rx_test(cmdbuf, false);
        }
        break;
    case BLE_HCI_OCF_LE_TEST_END:
        if (len == 0) {
            rc = ble_ll_dtm_end_test(rspbuf, rsplen);
        }
        break;
#endif
    case BLE_HCI_OCF_LE_REM_CONN_PARAM_RR:
        if (len == BLE_HCI_CONN_PARAM_REPLY_LEN) {
            rc = ble_ll_conn_hci_param_reply(cmdbuf, 1, rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_REM_CONN_PARAM_NRR:
        if (len == BLE_HCI_CONN_PARAM_NEG_REPLY_LEN) {
            rc = ble_ll_conn_hci_param_reply(cmdbuf, 0, rspbuf, rsplen);
        }
        break;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_DATA_LEN_EXT)
    case BLE_HCI_OCF_LE_SET_DATA_LEN:
        if (len == BLE_HCI_SET_DATALEN_LEN) {
            rc = ble_ll_conn_hci_set_data_len(cmdbuf, rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_RD_SUGG_DEF_DATA_LEN:
        if (len == 0) {
            rc = ble_ll_hci_le_rd_sugg_data_len(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_WR_SUGG_DEF_DATA_LEN:
        if (len == BLE_HCI_WR_SUGG_DATALEN_LEN) {
            rc = ble_ll_hci_le_wr_sugg_data_len(cmdbuf);
        }
        break;
#endif
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY)
    case BLE_HCI_OCF_LE_ADD_RESOLV_LIST :
        if (len == BLE_HCI_ADD_TO_RESOLV_LIST_LEN) {
            rc = ble_ll_resolv_list_add(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_RMV_RESOLV_LIST:
        if (len == BLE_HCI_RMV_FROM_RESOLV_LIST_LEN) {
            rc = ble_ll_resolv_list_rmv(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_CLR_RESOLV_LIST:
        if (len == 0) {
            rc = ble_ll_resolv_list_clr();
        }
        break;
    case BLE_HCI_OCF_LE_RD_RESOLV_LIST_SIZE:
        if (len == 0) {
            rc = ble_ll_resolv_list_read_size(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_RD_PEER_RESOLV_ADDR:
        if (len == BLE_HCI_RD_PEER_RESOLV_ADDR_LEN) {
            rc = ble_ll_resolv_peer_addr_rd(cmdbuf, rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_RD_LOCAL_RESOLV_ADDR:
        if(len == BLE_HCI_RD_LOC_RESOLV_ADDR_LEN) {
            rc = ble_ll_resolv_local_addr_rd(cmdbuf, rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_SET_ADDR_RES_EN:
        if (len == BLE_HCI_SET_ADDR_RESOL_ENA_LEN) {
            rc = ble_ll_resolv_enable_cmd(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_SET_RPA_TMO:
        if (len == BLE_HCI_SET_RESOLV_PRIV_ADDR_TO_LEN) {
            rc = ble_ll_resolv_set_rpa_tmo(cmdbuf);
        }
        break;
#endif
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_DATA_LEN_EXT)
    case BLE_HCI_OCF_LE_RD_MAX_DATA_LEN:
        if (len == 0) {
            rc = ble_ll_hci_le_rd_max_data_len(rspbuf, rsplen);
        }
        break;
#endif
#if (BLE_LL_BT5_PHY_SUPPORTED == 1)
    case BLE_HCI_OCF_LE_RD_PHY:
        if (len == BLE_HCI_LE_RD_PHY_LEN) {
            rc = ble_ll_conn_hci_le_rd_phy(cmdbuf, rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_SET_DEFAULT_PHY:
        if (len == BLE_HCI_LE_SET_DEFAULT_PHY_LEN) {
            rc = ble_ll_hci_le_set_def_phy(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_SET_PHY:
        if (len == BLE_HCI_LE_SET_PHY_LEN) {
            rc = ble_ll_conn_hci_le_set_phy(cmdbuf);
        }
        break;
#endif
#if MYNEWT_VAL(BLE_LL_DIRECT_TEST_MODE)
    case BLE_HCI_OCF_LE_ENH_RX_TEST:
        if (len == BLE_HCI_LE_ENH_RX_TEST_LEN) {
            rc = ble_ll_dtm_rx_test(cmdbuf, true);
        }
        break;
    case BLE_HCI_OCF_LE_ENH_TX_TEST:
        if (len == BLE_HCI_LE_ENH_TX_TEST_LEN) {
            rc = ble_ll_dtm_tx_test(cmdbuf, true);
        }
        break;
#endif
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
    case BLE_HCI_OCF_LE_SET_ADV_SET_RND_ADDR:
        if (len == BLE_HCI_LE_SET_ADV_SET_RND_ADDR_LEN) {
            rc = ble_ll_adv_set_random_addr(cmdbuf + 1, cmdbuf[0]);
        }
        break;
    case BLE_HCI_OCF_LE_SET_EXT_ADV_PARAM:
        if (len == BLE_HCI_LE_SET_EXT_ADV_PARAM_LEN) {
            rc = ble_ll_adv_ext_set_param(cmdbuf, rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_SET_EXT_ADV_DATA:
        /* variable length */
        rc = ble_ll_adv_ext_set_adv_data(cmdbuf, len);
        break;
    case BLE_HCI_OCF_LE_SET_EXT_SCAN_RSP_DATA:
        /* variable length */
        rc = ble_ll_adv_ext_set_scan_rsp(cmdbuf, len);
        break;
    case BLE_HCI_OCF_LE_SET_EXT_ADV_ENABLE:
        /* variable length */
        rc =  ble_ll_adv_ext_set_enable(cmdbuf, len);
        break;
    case BLE_HCI_OCF_LE_RD_MAX_ADV_DATA_LEN:
        if (len == 0) {
            rc = ble_ll_adv_rd_max_adv_data_len(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_RD_NUM_OF_ADV_SETS:
        if (len == 0) {
            rc = ble_ll_adv_rd_sup_adv_sets(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_LE_REMOVE_ADV_SET:
        if (len == BLE_HCI_LE_REMOVE_ADV_SET_LEN) {
            rc =  ble_ll_ext_adv_set_remove(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_CLEAR_ADV_SETS:
        if (len == 0) {
            rc =  ble_ll_adv_clear_all();
        }
        break;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PERIODIC_ADV)
    case BLE_HCI_OCF_LE_SET_PERIODIC_ADV_PARAMS:
        if (len == BLE_HCI_LE_SET_PERIODIC_ADV_PARAMS_LEN) {
            rc = ble_ll_adv_periodic_set_param(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_SET_PERIODIC_ADV_DATA:
        /* variable length */
        rc = ble_ll_adv_periodic_set_data(cmdbuf, len);
        break;
    case BLE_HCI_OCF_LE_SET_PERIODIC_ADV_ENABLE:
        if (len == BLE_HCI_LE_SET_PERIODIC_ADV_ENABLE_LEN) {
            rc = ble_ll_adv_periodic_enable(cmdbuf);
        }
        break;
#endif
#endif
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
    case BLE_HCI_OCF_LE_SET_EXT_SCAN_PARAM:
        /* variable length */
        rc = ble_ll_set_ext_scan_params(cmdbuf, len);
        break;
    case BLE_HCI_OCF_LE_SET_EXT_SCAN_ENABLE:
        if (len == BLE_HCI_LE_SET_EXT_SCAN_ENABLE_LEN) {
            rc = ble_ll_scan_set_enable(cmdbuf, 1);
        }
        break;
    case BLE_HCI_OCF_LE_EXT_CREATE_CONN:
        /* variable length */
        rc = ble_ll_ext_conn_create(cmdbuf, len);
        break;
#endif
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PERIODIC_ADV)
    case BLE_HCI_OCF_LE_PERIODIC_ADV_CREATE_SYNC:
        if (len == BLE_HCI_LE_PERIODIC_ADV_CREATE_SYNC_LEN) {
            rc = ble_ll_sync_create(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL:
        if (len == 0) {
            rc = ble_ll_sync_cancel(cb);
        }
        break;
    case BLE_HCI_OCF_LE_PERIODIC_ADV_TERM_SYNC:
        if (len == BLE_HCI_LE_PERIODIC_ADV_TERM_SYNC_LEN) {
            rc = ble_ll_sync_terminate(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_ADD_DEV_TO_PERIODIC_ADV_LIST:
        if (len == BLE_HCI_LE_ADD_DEV_TO_PERIODIC_ADV_LIST_LEN) {
            rc = ble_ll_sync_list_add(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_REM_DEV_FROM_PERIODIC_ADV_LIST:
        if (len == BLE_HCI_LE_REM_DEV_FROM_PERIODIC_ADV_LIST_LEN) {
            rc = ble_ll_sync_list_remove(cmdbuf);
        }
        break;
    case BLE_HCI_OCF_LE_CLEAR_PERIODIC_ADV_LIST:
        if (len == 0) {
            rc = ble_ll_sync_list_clear();
        }
        break;
    case BLE_HCI_OCF_LE_RD_PERIODIC_ADV_LIST_SIZE:
        if (len == 0) {
            rc = ble_ll_sync_list_size(rspbuf, rsplen);
        }
        break;
#endif
    case BLE_HCI_OCF_LE_RD_TRANSMIT_POWER:
        rc = ble_ll_read_tx_power(rspbuf, rsplen);
        break;
    case BLE_HCI_OCF_LE_RD_RF_PATH_COMPENSATION:
        rc = ble_ll_read_rf_path_compensation(rspbuf, rsplen);
        break;
    case BLE_HCI_OCF_LE_WR_RF_PATH_COMPENSATION:
        if (len == BLE_HCI_LE_WR_RF_PATH_COMPENSATION_LEN) {
            rc = ble_ll_write_rf_path_compensation(cmdbuf);
        }
        break;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY)
    case BLE_HCI_OCF_LE_SET_PRIVACY_MODE:
        if (len == BLE_HCI_LE_SET_PRIVACY_MODE_LEN) {
            rc = ble_ll_resolve_set_priv_mode(cmdbuf);
        }
        break;
#endif
    default:
        rc = BLE_ERR_UNKNOWN_HCI_CMD;
        break;
    }

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
ll_hci_le_cmd_exit:
#endif
    /*
     * This code is here because we add 256 to the return code to denote
     * that the reply to this command should be command status (as opposed to
     * command complete).
     *
     * For unknown HCI command let us return always command status as per
     * specification Bluetooth 5, Vol. 2, Chapter 4.4
     */
    if (ble_ll_hci_le_cmd_send_cmd_status(ocf) || rc == BLE_ERR_UNKNOWN_HCI_CMD) {
        rc += (BLE_ERR_MAX + 1);
    }

    return rc;
}

/**
 * Process a link control command sent from the host to the controller. The HCI
 * command has a 3 byte command header followed by data. The header is:
 *  -> opcode (2 bytes)
 *  -> Length of parameters (1 byte; does include command header bytes).
 *
 * @param cmdbuf Pointer to command buffer. Points to start of command header.
 * @param ocf    Opcode command field.
 * @param *rsplen Pointer to length of response
 *
 * @return int  This function returns a BLE error code. If a command status
 *              event should be returned as opposed to command complete,
 *              256 gets added to the return value.
 */
static int
ble_ll_hci_link_ctrl_cmd_proc(uint8_t *cmdbuf, uint16_t ocf, uint8_t *rsplen)
{
    int rc;
    uint8_t len;

    /* Assume error; if all pass rc gets set to 0 */
    rc = BLE_ERR_INV_HCI_CMD_PARMS;

    /* Get length from command */
    len = cmdbuf[sizeof(uint16_t)];

    /* Move past HCI command header */
    cmdbuf += BLE_HCI_CMD_HDR_LEN;

    switch (ocf) {
    case BLE_HCI_OCF_DISCONNECT_CMD:
        if (len == BLE_HCI_DISCONNECT_CMD_LEN) {
            rc = ble_ll_conn_hci_disconnect_cmd(cmdbuf);
        }
        /* Send command status instead of command complete */
        rc += (BLE_ERR_MAX + 1);
        break;

    case BLE_HCI_OCF_RD_REM_VER_INFO:
        if (len == sizeof(uint16_t)) {
            rc = ble_ll_conn_hci_rd_rem_ver_cmd(cmdbuf);
        }
        /* Send command status instead of command complete */
        rc += (BLE_ERR_MAX + 1);
        break;

    default:
        rc = BLE_ERR_UNKNOWN_HCI_CMD;
        break;
    }

    return rc;
}

static int
ble_ll_hci_ctlr_bb_cmd_proc(uint8_t *cmdbuf, uint16_t ocf, uint8_t *rsplen)
{
    int rc;
    uint8_t len;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_PING)
    uint8_t *rspbuf;
#endif

    /* Assume error; if all pass rc gets set to 0 */
    rc = BLE_ERR_INV_HCI_CMD_PARMS;

    /* Get length from command */
    len = cmdbuf[sizeof(uint16_t)];

    /* Move past HCI command header */
    cmdbuf += BLE_HCI_CMD_HDR_LEN;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_PING)
    rspbuf = cmdbuf + BLE_HCI_EVENT_CMD_COMPLETE_MIN_LEN;
#endif

    switch (ocf) {
    case BLE_HCI_OCF_CB_SET_EVENT_MASK:
        if (len == BLE_HCI_SET_EVENT_MASK_LEN) {
            memcpy(g_ble_ll_hci_event_mask, cmdbuf, len);
            rc = BLE_ERR_SUCCESS;
        }
        break;
    case BLE_HCI_OCF_CB_RESET:
        if (len == 0) {
            rc = ble_ll_reset();
        }
        break;
    case BLE_HCI_OCF_CB_SET_EVENT_MASK2:
        if (len == BLE_HCI_SET_EVENT_MASK_LEN) {
            memcpy(g_ble_ll_hci_event_mask2, cmdbuf, len);
            rc = BLE_ERR_SUCCESS;
        }
        break;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_PING)
    case BLE_HCI_OCF_CB_RD_AUTH_PYLD_TMO:
        rc = ble_ll_conn_hci_rd_auth_pyld_tmo(cmdbuf, rspbuf, rsplen);
        break;
    case BLE_HCI_OCF_CB_WR_AUTH_PYLD_TMO:
        rc = ble_ll_conn_hci_wr_auth_pyld_tmo(cmdbuf, rspbuf, rsplen);
        break;
#endif
    default:
        rc = BLE_ERR_UNKNOWN_HCI_CMD;
        break;
    }

    return rc;
}

static int
ble_ll_hci_info_params_cmd_proc(uint8_t *cmdbuf, uint16_t ocf, uint8_t *rsplen)
{
    int rc;
    uint8_t len;
    uint8_t *rspbuf;

    /* Assume error; if all pass rc gets set to 0 */
    rc = BLE_ERR_INV_HCI_CMD_PARMS;

    /* Get length from command */
    len = cmdbuf[sizeof(uint16_t)];

    /*
     * The command response pointer points into the same buffer as the
     * command data itself. That is fine, as each command reads all the data
     * before crafting a response.
     */
    rspbuf = cmdbuf + BLE_HCI_EVENT_CMD_COMPLETE_MIN_LEN;

    /* Move past HCI command header */
    cmdbuf += BLE_HCI_CMD_HDR_LEN;

    switch (ocf) {
    case BLE_HCI_OCF_IP_RD_LOCAL_VER:
        if (len == 0) {
            rc = ble_ll_hci_rd_local_version(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_IP_RD_LOC_SUPP_CMD:
        if (len == 0) {
            rc = ble_ll_hci_rd_local_supp_cmd(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_IP_RD_LOC_SUPP_FEAT:
        if (len == 0) {
            rc = ble_ll_hci_rd_local_supp_feat(rspbuf, rsplen);
        }
        break;
    case BLE_HCI_OCF_IP_RD_BD_ADDR:
        if (len == 0) {
            rc = ble_ll_hci_rd_bd_addr(rspbuf, rsplen);
        }
        break;
    default:
        rc = BLE_ERR_UNKNOWN_HCI_CMD;
        break;
    }

    return rc;
}

static int
ble_ll_hci_status_params_cmd_proc(uint8_t *cmdbuf, uint16_t ocf, uint8_t *rsplen)
{
    int rc;
    uint8_t len;
    uint8_t *rspbuf;

    /* Assume error; if all pass rc gets set to 0 */
    rc = BLE_ERR_INV_HCI_CMD_PARMS;

    /* Get length from command */
    len = cmdbuf[sizeof(uint16_t)];

    /*
     * The command response pointer points into the same buffer as the
     * command data itself. That is fine, as each command reads all the data
     * before crafting a response.
     */
    rspbuf = cmdbuf + BLE_HCI_EVENT_CMD_COMPLETE_MIN_LEN;

    /* Move past HCI command header */
    cmdbuf += BLE_HCI_CMD_HDR_LEN;

    switch (ocf) {
    case BLE_HCI_OCF_RD_RSSI:
        if (len == sizeof(uint16_t)) {
            rc = ble_ll_conn_hci_rd_rssi(cmdbuf, rspbuf, rsplen);
        }
        break;
    default:
        rc = BLE_ERR_UNKNOWN_HCI_CMD;
        break;
    }

    return rc;
}

/**
 * Called to process an HCI command from the host.
 *
 * @param ev Pointer to os event containing a pointer to command buffer
 */
static void
ble_ll_hci_cmd_proc(struct ble_npl_event *ev)
{
    int rc;
    uint8_t ogf;
    uint8_t rsplen;
    uint8_t *cmdbuf;
    uint16_t opcode;
    uint16_t ocf;
    ble_ll_hci_post_cmd_complete_cb post_cb = NULL;

#if MYNEWT_VAL(BLE_LL_DBG_HCI_CMD_PIN) >= 0
    hal_gpio_write(MYNEWT_VAL(BLE_LL_DBG_HCI_CMD_PIN), 1);
#endif

    /* The command buffer is the event argument */
    cmdbuf = (uint8_t *)ble_npl_event_get_arg(ev);
    BLE_LL_ASSERT(cmdbuf != NULL);

    /* Get the opcode from the command buffer */
    opcode = get_le16(cmdbuf);
    ocf = BLE_HCI_OCF(opcode);
    ogf = BLE_HCI_OGF(opcode);

    /* Assume response length is zero */
    rsplen = 0;

    switch (ogf) {
    case BLE_HCI_OGF_LINK_CTRL:
        rc = ble_ll_hci_link_ctrl_cmd_proc(cmdbuf, ocf, &rsplen);
        break;
    case BLE_HCI_OGF_CTLR_BASEBAND:
        rc = ble_ll_hci_ctlr_bb_cmd_proc(cmdbuf, ocf, &rsplen);
        break;
    case BLE_HCI_OGF_INFO_PARAMS:
        rc = ble_ll_hci_info_params_cmd_proc(cmdbuf, ocf, &rsplen);
        break;
    case BLE_HCI_OGF_STATUS_PARAMS:
        rc = ble_ll_hci_status_params_cmd_proc(cmdbuf, ocf, &rsplen);
        break;
    case BLE_HCI_OGF_LE:
        rc = ble_ll_hci_le_cmd_proc(cmdbuf, ocf, &rsplen, &post_cb);
        break;
    default:
        /* XXX: Need to support other OGF. For now, return unsupported */
        rc = BLE_ERR_UNKNOWN_HCI_CMD;
        break;
    }

    /* If no response is generated, we free the buffers */
    BLE_LL_ASSERT(rc >= 0);
    if (rc <= BLE_ERR_MAX) {
        /* Create a command complete event with status from command */
        cmdbuf[0] = BLE_HCI_EVCODE_COMMAND_COMPLETE;
        cmdbuf[1] = 4 + rsplen;
        cmdbuf[2] = ble_ll_hci_get_num_cmd_pkts();
        put_le16(cmdbuf + 3, opcode);
        cmdbuf[5] = (uint8_t)rc;
    } else {
        /* Create a command status event */
        rc -= (BLE_ERR_MAX + 1);
        cmdbuf[0] = BLE_HCI_EVCODE_COMMAND_STATUS;
        cmdbuf[1] = 4;
        cmdbuf[2] = (uint8_t)rc;
        cmdbuf[3] = ble_ll_hci_get_num_cmd_pkts();
        put_le16(cmdbuf + 4, opcode);
    }

    /* Count commands and those in error */
    if (rc) {
        STATS_INC(ble_ll_stats, hci_cmd_errs);
    } else {
        STATS_INC(ble_ll_stats, hci_cmds);
    }

    /* Send the event (events cannot be masked) */
    ble_ll_hci_event_send(cmdbuf);

    /* Call post callback if set by command handler */
    if (post_cb) {
        post_cb();
    }

#if MYNEWT_VAL(BLE_LL_DBG_HCI_CMD_PIN) >= 0
    hal_gpio_write(MYNEWT_VAL(BLE_LL_DBG_HCI_CMD_PIN), 0);
#endif
}

/**
 * Sends an HCI command to the controller.  On success, the supplied buffer is
 * relinquished to the controller task.  On failure, the caller must free the
 * buffer.
 *
 * @param cmd                   A flat buffer containing the HCI command to
 *                                  send.
 *
 * @return                      0 on success;
 *                              BLE_ERR_MEM_CAPACITY on HCI buffer exhaustion.
 */
int
ble_ll_hci_cmd_rx(uint8_t *cmd, void *arg)
{
    struct ble_npl_event *ev;

    /* Get an event structure off the queue */
    ev = &g_ble_ll_hci_cmd_ev;
    if (ble_npl_event_is_queued(ev)) {
        return BLE_ERR_MEM_CAPACITY;
    }

    /* Fill out the event and post to Link Layer */
    ble_npl_event_set_arg(ev, cmd);
    ble_npl_eventq_put(&g_ble_ll_data.ll_evq, ev);

    return 0;
}

/* Send ACL data from host to contoller */
int
ble_ll_hci_acl_rx(struct os_mbuf *om, void *arg)
{
    ble_ll_acl_data_in(om);
    return 0;
}

/**
 * Initalize the LL HCI.
 *
 * NOTE: This function is called by the HCI RESET command so if any code
 * is added here it must be OK to be executed when the reset command is used.
 */
void
ble_ll_hci_init(void)
{
#if MYNEWT_VAL(BLE_LL_DBG_HCI_CMD_PIN) >= 0
    hal_gpio_init_out(MYNEWT_VAL(BLE_LL_DBG_HCI_CMD_PIN), 0);
#endif
#if MYNEWT_VAL(BLE_LL_DBG_HCI_EV_PIN) >= 0
    hal_gpio_init_out(MYNEWT_VAL(BLE_LL_DBG_HCI_EV_PIN), 0);
#endif

    /* Set event callback for command processing */
    ble_npl_event_init(&g_ble_ll_hci_cmd_ev, ble_ll_hci_cmd_proc, NULL);

    /* Set defaults for LE events: Vol 2 Part E 7.8.1 */
    g_ble_ll_hci_le_event_mask[0] = 0x1f;

    /* Set defaults for controller/baseband events: Vol 2 Part E 7.3.1 */
    g_ble_ll_hci_event_mask[0] = 0xff;
    g_ble_ll_hci_event_mask[1] = 0xff;
    g_ble_ll_hci_event_mask[2] = 0xff;
    g_ble_ll_hci_event_mask[3] = 0xff;
    g_ble_ll_hci_event_mask[4] = 0xff;
    g_ble_ll_hci_event_mask[5] = 0x1f;

    /* Set page 2 to 0 */
    memset(g_ble_ll_hci_event_mask2, 0, BLE_HCI_SET_EVENT_MASK_LEN);

    /* reset RF path compensation values */
    rx_path_pwr_compensation = 0;
    tx_path_pwr_compensation = 0;

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
    /* after reset both legacy and extended advertising commands are allowed */
    hci_adv_mode = ADV_MODE_ANY;
#endif
}
