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
#include "ble/xcvr.h"
#include "nimble/ble.h"
#include "nimble/nimble_opt.h"
#include "controller/ble_phy.h"
#include "controller/ble_ll.h"

/* BLE PHY data structure */
struct ble_phy_obj
{
    uint8_t phy_stats_initialized;
    int8_t  phy_txpwr_dbm;
    int16_t rx_pwr_compensation;
    uint8_t phy_chan;
    uint8_t phy_state;
    uint8_t phy_transition;
    uint8_t phy_rx_started;
    uint8_t phy_encrypted;
    uint8_t phy_privacy;
    uint8_t phy_tx_pyld_len;
    uint32_t phy_aar_scratch;
    uint32_t phy_access_address;
    struct ble_mbuf_hdr rxhdr;
    void *txend_arg;
    uint8_t *rxdptr;
    ble_phy_tx_end_func txend_cb;
};
struct ble_phy_obj g_ble_phy_data;

/* Statistics */
struct ble_phy_statistics
{
    uint32_t tx_good;
    uint32_t tx_fail;
    uint32_t tx_late;
    uint32_t tx_bytes;
    uint32_t rx_starts;
    uint32_t rx_aborts;
    uint32_t rx_valid;
    uint32_t rx_crc_err;
    uint32_t phy_isrs;
    uint32_t radio_state_errs;
    uint32_t no_bufs;
};

struct ble_phy_statistics g_ble_phy_stats;

static uint8_t g_ble_phy_tx_buf[BLE_PHY_MAX_PDU_LEN];

/* XCVR object to emulate transceiver */
struct xcvr_data
{
    uint32_t irq_status;
};
static struct xcvr_data g_xcvr_data;

#define BLE_XCVR_IRQ_F_RX_START     (0x00000001)
#define BLE_XCVR_IRQ_F_RX_END       (0x00000002)
#define BLE_XCVR_IRQ_F_TX_START     (0x00000004)
#define BLE_XCVR_IRQ_F_TX_END       (0x00000008)
#define BLE_XCVR_IRQ_F_BYTE_CNTR    (0x00000010)

/* "Rail" power level if outside supported range */
#define BLE_XCVR_TX_PWR_MAX_DBM     (30)
#define BLE_XCVR_TX_PWR_MIN_DBM     (-20)

/* Statistics */
STATS_SECT_START(ble_phy_stats)
    STATS_SECT_ENTRY(phy_isrs)
    STATS_SECT_ENTRY(tx_good)
    STATS_SECT_ENTRY(tx_fail)
    STATS_SECT_ENTRY(tx_late)
    STATS_SECT_ENTRY(tx_bytes)
    STATS_SECT_ENTRY(rx_starts)
    STATS_SECT_ENTRY(rx_aborts)
    STATS_SECT_ENTRY(rx_valid)
    STATS_SECT_ENTRY(rx_crc_err)
    STATS_SECT_ENTRY(rx_late)
    STATS_SECT_ENTRY(no_bufs)
    STATS_SECT_ENTRY(radio_state_errs)
    STATS_SECT_ENTRY(rx_hw_err)
    STATS_SECT_ENTRY(tx_hw_err)
STATS_SECT_END
STATS_SECT_DECL(ble_phy_stats) ble_phy_stats;

STATS_NAME_START(ble_phy_stats)
    STATS_NAME(ble_phy_stats, phy_isrs)
    STATS_NAME(ble_phy_stats, tx_good)
    STATS_NAME(ble_phy_stats, tx_fail)
    STATS_NAME(ble_phy_stats, tx_late)
    STATS_NAME(ble_phy_stats, tx_bytes)
    STATS_NAME(ble_phy_stats, rx_starts)
    STATS_NAME(ble_phy_stats, rx_aborts)
    STATS_NAME(ble_phy_stats, rx_valid)
    STATS_NAME(ble_phy_stats, rx_crc_err)
    STATS_NAME(ble_phy_stats, rx_late)
    STATS_NAME(ble_phy_stats, no_bufs)
    STATS_NAME(ble_phy_stats, radio_state_errs)
    STATS_NAME(ble_phy_stats, rx_hw_err)
    STATS_NAME(ble_phy_stats, tx_hw_err)
STATS_NAME_END(ble_phy_stats)

/* XXX: TODO:

 * 1) Test the following to make sure it works: suppose an event is already
 * set to 1 and the interrupt is not enabled. What happens if you enable the
 * interrupt with the event bit already set to 1
 * 2) how to deal with interrupts?
 */
static uint32_t
ble_xcvr_get_irq_status(void)
{
    return g_xcvr_data.irq_status;
}

static void
ble_xcvr_clear_irq(uint32_t mask)
{
    g_xcvr_data.irq_status &= ~mask;
}

/**
 * Copies the data from the phy receive buffer into a mbuf chain.
 *
 * @param dptr Pointer to receive buffer
 * @param rxpdu Pointer to already allocated mbuf chain
 *
 * NOTE: the packet header already has the total mbuf length in it. The
 * lengths of the individual mbufs are not set prior to calling.
 *
 */
void
ble_phy_rxpdu_copy(uint8_t *dptr, struct os_mbuf *rxpdu)
{
    uint16_t rem_bytes;
    uint16_t mb_bytes;
    uint16_t copylen;
    uint32_t *dst;
    uint32_t *src;
    struct os_mbuf *m;
    struct ble_mbuf_hdr *ble_hdr;
    struct os_mbuf_pkthdr *pkthdr;

    /* Better be aligned */
    assert(((uint32_t)dptr & 3) == 0);

    pkthdr = OS_MBUF_PKTHDR(rxpdu);
    rem_bytes = pkthdr->omp_len;

    /* Fill in the mbuf pkthdr first. */
    dst = (uint32_t *)(rxpdu->om_data);
    src = (uint32_t *)dptr;

    mb_bytes = (rxpdu->om_omp->omp_databuf_len - rxpdu->om_pkthdr_len - 4);
    copylen = min(mb_bytes, rem_bytes);
    copylen &= 0xFFFC;
    rem_bytes -= copylen;
    mb_bytes -= copylen;
    rxpdu->om_len = copylen;
    while (copylen > 0) {
        *dst = *src;
        ++dst;
        ++src;
        copylen -= 4;
    }

    /* Copy remaining bytes */
    m = rxpdu;
    while (rem_bytes > 0) {
        /* If there are enough bytes in the mbuf, copy them and leave */
        if (rem_bytes <= mb_bytes) {
            memcpy(m->om_data + m->om_len, src, rem_bytes);
            m->om_len += rem_bytes;
            break;
        }

        m = SLIST_NEXT(m, om_next);
        assert(m != NULL);

        mb_bytes = m->om_omp->omp_databuf_len;
        copylen = min(mb_bytes, rem_bytes);
        copylen &= 0xFFFC;
        rem_bytes -= copylen;
        mb_bytes -= copylen;
        m->om_len = copylen;
        dst = (uint32_t *)m->om_data;
        while (copylen > 0) {
            *dst = *src;
            ++dst;
            ++src;
            copylen -= 4;
        }
    }

    /* Copy ble header */
    ble_hdr = BLE_MBUF_HDR_PTR(rxpdu);
    memcpy(ble_hdr, &g_ble_phy_data.rxhdr, sizeof(struct ble_mbuf_hdr));
}

void
ble_phy_isr(void)
{
    int rc;
    uint8_t transition;
    uint32_t irq_en;
    struct ble_mbuf_hdr *ble_hdr;

    /* Check for disabled event. This only happens for transmits now */
    irq_en = ble_xcvr_get_irq_status();
    if (irq_en & BLE_XCVR_IRQ_F_TX_END) {

        /* Better be in TX state! */
        assert(g_ble_phy_data.phy_state == BLE_PHY_STATE_TX);
        ble_xcvr_clear_irq(BLE_XCVR_IRQ_F_TX_END);

        transition = g_ble_phy_data.phy_transition;
        if (transition == BLE_PHY_TRANSITION_TX_RX) {
            /* Disable the phy */
            /* XXX: count no bufs? */
            ble_phy_disable();
        } else {
            /* Better not be going from rx to tx! */
            assert(transition == BLE_PHY_TRANSITION_NONE);
        }
    }

    /* We get this if we have started to receive a frame */
    if (irq_en & BLE_XCVR_IRQ_F_RX_START) {

        ble_xcvr_clear_irq(BLE_XCVR_IRQ_F_RX_START);

        /* Call Link Layer receive start function */
        rc = ble_ll_rx_start(g_ble_phy_data.rxdptr, g_ble_phy_data.phy_chan,
                             &g_ble_phy_data.rxhdr);
        if (rc >= 0) {
            /* XXX: set rx end enable isr */
        } else {
            /* Disable PHY */
            ble_phy_disable();
            irq_en = 0;
            ++g_ble_phy_stats.rx_aborts;
        }

        /* Count rx starts */
        ++g_ble_phy_stats.rx_starts;
    }

    /* Receive packet end (we dont enable this for transmit) */
    if (irq_en & BLE_XCVR_IRQ_F_RX_END) {

        ble_xcvr_clear_irq(BLE_XCVR_IRQ_F_RX_END);

        /* Construct BLE header before handing up */
        ble_hdr = &g_ble_phy_data.rxhdr;
        ble_hdr->rxinfo.flags = 0;
        /* XXX: dummy rssi */
        ble_hdr->rxinfo.rssi = -77 + g_ble_phy_data.rx_pwr_compensation;
        ble_hdr->rxinfo.channel = g_ble_phy_data.phy_chan;
        ble_hdr->rxinfo.phy = BLE_PHY_1M;
#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_EXT_ADV)
        ble_hdr->rxinfo.aux_data = NULL;
#endif

        /* Count PHY valid packets */
        ++g_ble_phy_stats.rx_valid;
        ble_hdr->rxinfo.flags |= BLE_MBUF_HDR_F_CRC_OK;

        /* Call Link Layer receive payload function */
        rc = ble_ll_rx_end(g_ble_phy_data.rxdptr, ble_hdr);
        if (rc < 0) {
            /* Disable the PHY. */
            ble_phy_disable();
        }
    }

    /* Count # of interrupts */
    ++g_ble_phy_stats.phy_isrs;
}

/**
 * ble phy init
 *
 * Initialize the PHY. This is expected to be called once.
 *
 * @return int 0: success; PHY error code otherwise
 */
int
ble_phy_init(void)
{
    /* Set phy channel to an invalid channel so first set channel works */
    g_ble_phy_data.phy_state = BLE_PHY_STATE_IDLE;
    g_ble_phy_data.phy_chan = BLE_PHY_NUM_CHANS;

    g_ble_phy_data.rx_pwr_compensation = 0;

    /* XXX: emulate ISR? */

    return 0;
}

int
ble_phy_rx(void)
{
    /* Check radio state */
    if (ble_phy_state_get() != BLE_PHY_STATE_IDLE) {
        ble_phy_disable();
        ++g_ble_phy_stats.radio_state_errs;
        return BLE_PHY_ERR_RADIO_STATE;
    }

    g_ble_phy_data.phy_state = BLE_PHY_STATE_RX;

    return 0;
}

void
ble_phy_restart_rx(void)
{
}

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION)
/**
 * Called to enable encryption at the PHY. Note that this state will persist
 * in the PHY; in other words, if you call this function you have to call
 * disable so that future PHY transmits/receives will not be encrypted.
 *
 * @param pkt_counter
 * @param iv
 * @param key
 * @param is_master
 */
void
ble_phy_encrypt_enable(uint64_t pkt_counter, uint8_t *iv, uint8_t *key,
                       uint8_t is_master)
{
}

void
ble_phy_encrypt_set_pkt_cntr(uint64_t pkt_counter, int dir)
{
}

void
ble_phy_encrypt_disable(void)
{
}
#endif

void
ble_phy_set_txend_cb(ble_phy_tx_end_func txend_cb, void *arg)
{
    /* Set transmit end callback and arg */
    g_ble_phy_data.txend_cb = txend_cb;
    g_ble_phy_data.txend_arg = arg;
}

/**
 * Called to set the start time of a transmission.
 *
 * This function is called to set the start time when we are not going from
 * rx to tx automatically.
 *
 * NOTE: care must be taken when calling this function. The channel should
 * already be set.
 *
 * @param cputime
 * @param rem_usecs
 *
 * @return int
 */
int
ble_phy_tx_set_start_time(uint32_t cputime, uint8_t rem_usecs)
{
    return 0;
}

/**
 * Called to set the start time of a reception
 *
 * This function acts a bit differently than transmit. If we are late getting
 * here we will still attempt to receive.
 *
 * NOTE: care must be taken when calling this function. The channel should
 * already be set.
 *
 * @param cputime
 * @param rem_usecs
 *
 * @return int
 */
int
ble_phy_rx_set_start_time(uint32_t cputime, uint8_t rem_usecs)
{
    return 0;
}


int
ble_phy_tx(ble_phy_tx_pducb_t pducb, void *pducb_arg, uint8_t end_trans)
{
    uint8_t hdr_byte;
    int rc;

    if (ble_phy_state_get() != BLE_PHY_STATE_IDLE) {
        ble_phy_disable();
        ++g_ble_phy_stats.radio_state_errs;
        return BLE_PHY_ERR_RADIO_STATE;
    }

    /* Select tx address */
    if (g_ble_phy_data.phy_chan < BLE_PHY_NUM_DATA_CHANS) {
        /* XXX: fix this */
        assert(0);
    } else {
    }

    /* Set the PHY transition */
    g_ble_phy_data.phy_transition = end_trans;

    /* Set phy state to transmitting and count packet statistics */
    g_ble_phy_data.phy_state = BLE_PHY_STATE_TX;
    ++g_ble_phy_stats.tx_good;
    g_ble_phy_stats.tx_bytes += pducb(g_ble_phy_tx_buf, pducb_arg, &hdr_byte) +
                                BLE_LL_PDU_HDR_LEN;
    rc = BLE_ERR_SUCCESS;

    return rc;
}

/**
 * ble phy txpwr set
 *
 * Set the transmit output power (in dBm).
 *
 * NOTE: If the output power specified is within the BLE limits but outside
 * the chip limits, we "rail" the power level so we dont exceed the min/max
 * chip values.
 *
 * @param dbm Power output in dBm.
 *
 * @return int 0: success; anything else is an error
 */
int
ble_phy_txpwr_set(int dbm)
{
    /* Check valid range */
    assert(dbm <= BLE_PHY_MAX_PWR_DBM);

    /* "Rail" power level if outside supported range */
    if (dbm > BLE_XCVR_TX_PWR_MAX_DBM) {
        dbm = BLE_XCVR_TX_PWR_MAX_DBM;
    } else {
        if (dbm < BLE_XCVR_TX_PWR_MIN_DBM) {
            dbm = BLE_XCVR_TX_PWR_MIN_DBM;
        }
    }

    g_ble_phy_data.phy_txpwr_dbm = dbm;

    return 0;
}

/**
 * ble phy txpwr round
 *
 * Get the rounded transmit output power (in dBm).
 *
 * @param dbm Power output in dBm.
 *
 * @return int Rounded power in dBm
 */
int ble_phy_txpower_round(int dbm)
{
    /* "Rail" power level if outside supported range */
    if (dbm > BLE_XCVR_TX_PWR_MAX_DBM) {
        dbm = BLE_XCVR_TX_PWR_MAX_DBM;
    } else {
        if (dbm < BLE_XCVR_TX_PWR_MIN_DBM) {
            dbm = BLE_XCVR_TX_PWR_MIN_DBM;
        }
    }

    return dbm;
}

/**
 * ble phy txpwr get
 *
 * Get the transmit power.
 *
 * @return int  The current PHY transmit power, in dBm
 */
int
ble_phy_txpwr_get(void)
{
    return g_ble_phy_data.phy_txpwr_dbm;
}

void
ble_phy_set_rx_pwr_compensation(int8_t compensation)
{
    g_ble_phy_data.rx_pwr_compensation = compensation;
}

/**
 * ble phy setchan
 *
 * Sets the logical frequency of the transceiver. The input parameter is the
 * BLE channel index (0 to 39, inclusive). The NRF52 frequency register
 * works like this: logical frequency = 2400 + FREQ (MHz).
 *
 * Thus, to get a logical frequency of 2402 MHz, you would program the
 * FREQUENCY register to 2.
 *
 * @param chan This is the Data Channel Index or Advertising Channel index
 *
 * @return int 0: success; PHY error code otherwise
 */
int
ble_phy_setchan(uint8_t chan, uint32_t access_addr, uint32_t crcinit)
{
    assert(chan < BLE_PHY_NUM_CHANS);

    /* Check for valid channel range */
    if (chan >= BLE_PHY_NUM_CHANS) {
        return BLE_PHY_ERR_INV_PARAM;
    }

    g_ble_phy_data.phy_access_address = access_addr;

    g_ble_phy_data.phy_chan = chan;

    return 0;
}

/**
 * Disable the PHY. This will do the following:
 *  -> Turn off all phy interrupts.
 *  -> Disable internal shortcuts.
 *  -> Disable the radio.
 *  -> Sets phy state to idle.
 */
void
ble_phy_disable(void)
{
    g_ble_phy_data.phy_state = BLE_PHY_STATE_IDLE;
}

/* Gets the current access address */
uint32_t ble_phy_access_addr_get(void)
{
    return g_ble_phy_data.phy_access_address;
}

/**
 * Return the phy state
 *
 * @return int The current PHY state.
 */
int
ble_phy_state_get(void)
{
    return g_ble_phy_data.phy_state;
}

/**
 * Called to see if a reception has started
 *
 * @return int
 */
int
ble_phy_rx_started(void)
{
    return g_ble_phy_data.phy_rx_started;
}

/**
 * Called to return the maximum data pdu payload length supported by the
 * phy. For this chip, if encryption is enabled, the maximum payload is 27
 * bytes.
 *
 * @return uint8_t Maximum data channel PDU payload size supported
 */
uint8_t
ble_phy_max_data_pdu_pyld(void)
{
    return BLE_LL_DATA_PDU_MAX_PYLD;
}

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY)
void
ble_phy_resolv_list_enable(void)
{
    g_ble_phy_data.phy_privacy = 1;
}

void
ble_phy_resolv_list_disable(void)
{
    g_ble_phy_data.phy_privacy = 0;
}

/**
 * Return the transceiver state
 *
 * @return int transceiver state.
 */
uint8_t
ble_phy_xcvr_state_get(void)
{
   return g_ble_phy_data.phy_state;
}

#endif

void
ble_phy_wfr_enable(int txrx, uint8_t tx_phy_mode, uint32_t wfr_usecs)
{
}
