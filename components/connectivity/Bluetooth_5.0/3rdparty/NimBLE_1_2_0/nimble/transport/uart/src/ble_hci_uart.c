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

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include "sysinit/sysinit.h"
#include "syscfg/syscfg.h"
#include "os/os_cputime.h"
#include "bsp/bsp.h"
#include "os/os.h"
#include "mem/mem.h"
#include "hal/hal_gpio.h"
#include "hal/hal_uart.h"

/* BLE */
#include "nimble/ble.h"
#include "nimble/nimble_opt.h"
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"

#include "transport/uart/ble_hci_uart.h"

#define BLE_HCI_UART_EVT_COUNT  \
    (MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT) + MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT))

/***
 * NOTES:
 * The UART HCI transport doesn't use event buffer priorities.  All incoming
 * and outgoing events use buffers from the same pool.
 *
 * The "skip" definitions are here so that when buffers cannot be allocated,
 * the command or acl packets are simply skipped so that the HCI interface
 * does not lose synchronization and resets dont (necessarily) occur.
 */

/* XXX: for now, define this here */
#if MYNEWT_VAL(BLE_CONTROLLER)
extern void ble_ll_data_buffer_overflow(void);
extern void ble_ll_hw_error(uint8_t err);

static const uint8_t ble_hci_uart_reset_cmd[4] = { 0x01, 0x03, 0x0C, 0x00 };
#endif

/***
 * NOTES:
 * The "skip" definitions are here so that when buffers cannot be allocated,
 * the command or acl packets are simply skipped so that the HCI interface
 * does not lose synchronization and resets dont (necessarily) occur.
 */
#define BLE_HCI_UART_H4_NONE        0x00
#define BLE_HCI_UART_H4_CMD         0x01
#define BLE_HCI_UART_H4_ACL         0x02
#define BLE_HCI_UART_H4_SCO         0x03
#define BLE_HCI_UART_H4_EVT         0x04
#define BLE_HCI_UART_H4_SYNC_LOSS   0x80
#define BLE_HCI_UART_H4_SKIP_CMD    0x81
#define BLE_HCI_UART_H4_SKIP_ACL    0x82
#define BLE_HCI_UART_H4_LE_EVT      0x83
#define BLE_HCI_UART_H4_SKIP_EVT    0x84

static ble_hci_trans_rx_cmd_fn *ble_hci_uart_rx_cmd_cb;
static void *ble_hci_uart_rx_cmd_arg;

static ble_hci_trans_rx_acl_fn *ble_hci_uart_rx_acl_cb;
static void *ble_hci_uart_rx_acl_arg;

static struct os_mempool ble_hci_uart_evt_hi_pool;
static os_membuf_t ble_hci_uart_evt_hi_buf[
        OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                        MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE))
];

static struct os_mempool ble_hci_uart_evt_lo_pool;
static os_membuf_t ble_hci_uart_evt_lo_buf[
        OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                        MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE))
];

static struct os_mempool ble_hci_uart_cmd_pool;
static os_membuf_t ble_hci_uart_cmd_buf[
	OS_MEMPOOL_SIZE(1, BLE_HCI_TRANS_CMD_SZ)
];

static struct os_mbuf_pool ble_hci_uart_acl_mbuf_pool;
static struct os_mempool_ext ble_hci_uart_acl_pool;

/*
 * The MBUF payload size must accommodate the HCI data header size plus the
 * maximum ACL data packet length. The ACL block size is the size of the
 * mbufs we will allocate.
 */
#define ACL_BLOCK_SIZE  OS_ALIGN(MYNEWT_VAL(BLE_ACL_BUF_SIZE) \
                                 + BLE_MBUF_MEMBLOCK_OVERHEAD \
                                 + BLE_HCI_DATA_HDR_SZ, OS_ALIGNMENT)

static os_membuf_t ble_hci_uart_acl_buf[
	OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                        ACL_BLOCK_SIZE)
];

/**
 * A packet to be sent over the UART.  This can be a command, an event, or ACL
 * data.
 */
struct ble_hci_uart_pkt {
    STAILQ_ENTRY(ble_hci_uart_pkt) next;
    void *data;
    uint8_t type;
};

static struct os_mempool ble_hci_uart_pkt_pool;
static os_membuf_t ble_hci_uart_pkt_buf[
        OS_MEMPOOL_SIZE(BLE_HCI_UART_EVT_COUNT + 1 +
                        MYNEWT_VAL(BLE_HCI_ACL_OUT_COUNT),
                        sizeof (struct ble_hci_uart_pkt))
];

/**
 * An incoming or outgoing command or event.
 */
struct ble_hci_uart_cmd {
    uint8_t *data;      /* Pointer to ble_hci_uart_cmd data */
    uint16_t cur;       /* Number of bytes read/written */
    uint16_t len;       /* Total number of bytes to read/write */
};

/**
 * An incoming ACL data packet.
 */
struct ble_hci_uart_acl {
    struct os_mbuf *buf; /* Buffer containing the data */
    uint8_t *dptr;       /* Pointer to where bytes should be placed */
    uint16_t len;        /* Target size when buf is considered complete */
    uint16_t rxd_bytes;  /* current count of bytes received for packet */
};

/**
 * Structure for transmitting ACL packets over UART
 *
 */
struct ble_hci_uart_h4_acl_tx
{
    uint8_t *dptr;
    struct os_mbuf *tx_acl;
};

static struct {
    /*** State of data received over UART. */
    uint8_t rx_type;    /* Pending packet type. 0 means nothing pending */
    union {
        struct ble_hci_uart_cmd rx_cmd;
        struct ble_hci_uart_acl rx_acl;
    };

    /*** State of data transmitted over UART. */
    uint8_t tx_type;    /* Pending packet type. 0 means nothing pending */
    uint16_t rem_tx_len; /* Used for acl tx only currently */
    union {
        struct ble_hci_uart_cmd tx_cmd;
        struct ble_hci_uart_h4_acl_tx tx_pkt;
    };
    STAILQ_HEAD(, ble_hci_uart_pkt) tx_pkts; /* Packet queue to send to UART */
} ble_hci_uart_state;

/**
 * Allocates a buffer (mbuf) for ACL operation.
 *
 * @return                      The allocated buffer on success;
 *                              NULL on buffer exhaustion.
 */
static struct os_mbuf *
ble_hci_trans_acl_buf_alloc(void)
{
    struct os_mbuf *m;
    uint8_t usrhdr_len;

#if MYNEWT_VAL(BLE_CONTROLLER)
    usrhdr_len = sizeof(struct ble_mbuf_hdr);
#elif MYNEWT_VAL(BLE_HS_FLOW_CTRL)
    usrhdr_len = BLE_MBUF_HS_HDR_LEN;
#else
    usrhdr_len = 0;
#endif

    m = os_mbuf_get_pkthdr(&ble_hci_uart_acl_mbuf_pool, usrhdr_len);
    return m;
}

static int
ble_hci_uart_acl_tx(struct os_mbuf *om)
{
    struct ble_hci_uart_pkt *pkt;
    os_sr_t sr;

    /* If this packet is zero length, just free it */
    if (OS_MBUF_PKTLEN(om) == 0) {
        os_mbuf_free_chain(om);
        return 0;
    }

    pkt = os_memblock_get(&ble_hci_uart_pkt_pool);
    if (pkt == NULL) {
        os_mbuf_free_chain(om);
        return BLE_ERR_MEM_CAPACITY;
    }

    pkt->type = BLE_HCI_UART_H4_ACL;
    pkt->data = om;

    OS_ENTER_CRITICAL(sr);
    STAILQ_INSERT_TAIL(&ble_hci_uart_state.tx_pkts, pkt, next);
    OS_EXIT_CRITICAL(sr);

    hal_uart_start_tx(MYNEWT_VAL(BLE_HCI_UART_PORT));

    return 0;
}

static int
ble_hci_uart_cmdevt_tx(uint8_t *hci_ev, uint8_t h4_type)
{
    struct ble_hci_uart_pkt *pkt;
    os_sr_t sr;

    pkt = os_memblock_get(&ble_hci_uart_pkt_pool);
    if (pkt == NULL) {
        ble_hci_trans_buf_free(hci_ev);
        return BLE_ERR_MEM_CAPACITY;
    }

    pkt->type = h4_type;
    pkt->data = hci_ev;

    OS_ENTER_CRITICAL(sr);
    STAILQ_INSERT_TAIL(&ble_hci_uart_state.tx_pkts, pkt, next);
    OS_EXIT_CRITICAL(sr);

    hal_uart_start_tx(MYNEWT_VAL(BLE_HCI_UART_PORT));

    return 0;
}

/**
 * @return                      The packet type to transmit on success;
 *                              -1 if there is nothing to send.
 */
static int
ble_hci_uart_tx_pkt_type(void)
{
    struct ble_hci_uart_pkt *pkt;
    struct os_mbuf *om;
    os_sr_t sr;
    int rc;

    OS_ENTER_CRITICAL(sr);

    pkt = STAILQ_FIRST(&ble_hci_uart_state.tx_pkts);
    if (!pkt) {
        OS_EXIT_CRITICAL(sr);
        return -1;
    }

    STAILQ_REMOVE(&ble_hci_uart_state.tx_pkts, pkt, ble_hci_uart_pkt, next);

    OS_EXIT_CRITICAL(sr);

    rc = pkt->type;
    switch (pkt->type) {
    case BLE_HCI_UART_H4_CMD:
        ble_hci_uart_state.tx_type = BLE_HCI_UART_H4_CMD;
        ble_hci_uart_state.tx_cmd.data = pkt->data;
        ble_hci_uart_state.tx_cmd.cur = 0;
        ble_hci_uart_state.tx_cmd.len = ble_hci_uart_state.tx_cmd.data[2] +
                                        BLE_HCI_CMD_HDR_LEN;
        break;

    case BLE_HCI_UART_H4_EVT:
        ble_hci_uart_state.tx_type = BLE_HCI_UART_H4_EVT;
        ble_hci_uart_state.tx_cmd.data = pkt->data;
        ble_hci_uart_state.tx_cmd.cur = 0;
        ble_hci_uart_state.tx_cmd.len = ble_hci_uart_state.tx_cmd.data[1] +
                                        BLE_HCI_EVENT_HDR_LEN;
        break;

    case BLE_HCI_UART_H4_ACL:
        ble_hci_uart_state.tx_type = BLE_HCI_UART_H4_ACL;
        om = (struct os_mbuf *)pkt->data;
        /* NOTE: first mbuf must have non-zero length */
        os_mbuf_trim_front(om);
        ble_hci_uart_state.tx_pkt.tx_acl = om;
        ble_hci_uart_state.tx_pkt.dptr = om->om_data;
        ble_hci_uart_state.rem_tx_len = OS_MBUF_PKTLEN(om);
        break;

    default:
        rc = -1;
        break;
    }

    os_memblock_put(&ble_hci_uart_pkt_pool, pkt);

    return rc;
}

/**
 * @return                      The byte to transmit on success;
 *                              -1 if there is nothing to send.
 */
static int
ble_hci_uart_tx_char(void *arg)
{
    uint8_t u8;
    int rc;
    struct os_mbuf *om;

    switch (ble_hci_uart_state.tx_type) {
    case BLE_HCI_UART_H4_NONE: /* No pending packet, pick one from the queue */
        rc = ble_hci_uart_tx_pkt_type();
        break;

    case BLE_HCI_UART_H4_CMD:
    case BLE_HCI_UART_H4_EVT:
        rc = ble_hci_uart_state.tx_cmd.data[ble_hci_uart_state.tx_cmd.cur++];

        if (ble_hci_uart_state.tx_cmd.cur == ble_hci_uart_state.tx_cmd.len) {
            ble_hci_trans_buf_free(ble_hci_uart_state.tx_cmd.data);
            ble_hci_uart_state.tx_type = BLE_HCI_UART_H4_NONE;
        }
        break;

    case BLE_HCI_UART_H4_ACL:
        /* Copy the first unsent byte from the tx buffer and remove it from the
         * source.
         */
        u8 = ble_hci_uart_state.tx_pkt.dptr[0];
        --ble_hci_uart_state.rem_tx_len;
        if (ble_hci_uart_state.rem_tx_len == 0) {
            os_mbuf_free_chain(ble_hci_uart_state.tx_pkt.tx_acl);
            ble_hci_uart_state.tx_type = BLE_HCI_UART_H4_NONE;
        } else {
            om = ble_hci_uart_state.tx_pkt.tx_acl;
            --om->om_len;
            if (om->om_len == 0) {
                /* Remove and free any zero mbufs */
                while ((om != NULL) && (om->om_len == 0)) {
                    ble_hci_uart_state.tx_pkt.tx_acl = SLIST_NEXT(om, om_next);
                    os_mbuf_free(om);
                    om = ble_hci_uart_state.tx_pkt.tx_acl;
                }
                /* NOTE: om should never be NULL! What to do? */
                if (om == NULL) {
                    assert(0);
                    ble_hci_uart_state.tx_type = BLE_HCI_UART_H4_NONE;
                } else {
                    ble_hci_uart_state.tx_pkt.dptr = om->om_data;
                }
            } else {
                ble_hci_uart_state.tx_pkt.dptr++;
            }
        }
        rc = u8;
        break;
    default:
        rc = -1;
        break;
    }

    return rc;
}

#if MYNEWT_VAL(BLE_CONTROLLER)
/**
 * HCI uart sync lost.
 *
 * This occurs when the controller receives an invalid packet type or a length
 * field that is out of range. The controller needs to send a HW error to the
 * host and wait to find a LL reset command.
 */
static void
ble_hci_uart_sync_lost(void)
{
    ble_hci_uart_state.rx_cmd.len = 0;
    ble_hci_uart_state.rx_cmd.cur = 0;
    ble_hci_uart_state.rx_cmd.data =
        ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_CMD);
    ble_ll_hw_error(BLE_HW_ERR_HCI_SYNC_LOSS);
    ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_SYNC_LOSS;
}
#endif

/**
 * @return                      The type of packet to follow success;
 *                              -1 if there is no valid packet to receive.
 */
static int
ble_hci_uart_rx_pkt_type(uint8_t data)
{
    struct os_mbuf *m;

    ble_hci_uart_state.rx_type = data;

    switch (ble_hci_uart_state.rx_type) {
    /* Host should never receive a command! */
#if MYNEWT_VAL(BLE_CONTROLLER)
    case BLE_HCI_UART_H4_CMD:
        ble_hci_uart_state.rx_cmd.len = 0;
        ble_hci_uart_state.rx_cmd.cur = 0;
        ble_hci_uart_state.rx_cmd.data =
            ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_CMD);
        if (ble_hci_uart_state.rx_cmd.data == NULL) {
            ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_SKIP_CMD;
        }
        break;
#endif

        /* Controller should never receive an event */
#if MYNEWT_VAL(BLE_HOST)
    case BLE_HCI_UART_H4_EVT:
        /*
         * The event code is unknown at the moment. Depending on event priority,
         * buffer *shall* be allocated from ble_hci_uart_evt_hi_pool
         * or "may* be allocated from ble_hci_uart_evt_lo_pool.
         * Thus do not allocate the buffer yet.
         */
        ble_hci_uart_state.rx_cmd.data = NULL;
        ble_hci_uart_state.rx_cmd.len = 0;
        ble_hci_uart_state.rx_cmd.cur = 0;
        break;
#endif

    case BLE_HCI_UART_H4_ACL:
        ble_hci_uart_state.rx_acl.len = 0;
        ble_hci_uart_state.rx_acl.rxd_bytes = 0;
        m = ble_hci_trans_acl_buf_alloc();
        if (m) {
            ble_hci_uart_state.rx_acl.dptr = m->om_data;
        } else {
            ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_SKIP_ACL;
        }
        ble_hci_uart_state.rx_acl.buf = m;
        break;

    default:
#if MYNEWT_VAL(BLE_CONTROLLER)
        /*
         * If we receive an unknown HCI packet type this is considered a loss
         * of sync.
         */
        ble_hci_uart_sync_lost();
#else
        /*
         * XXX: not sure what to do about host in this case. Just go back to
         * none for now.
         */
        ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;
#endif
        break;
    }

    return 0;
}

#if MYNEWT_VAL(BLE_CONTROLLER)
/**
 * HCI uart sync loss.
 *
 * Find a LL reset command in the byte stream. The LL reset command is a
 * sequence of 4 bytes:
 *  0x01    HCI Packet Type = HCI CMD
 *  0x03    OCF for reset command
 *  0x0C    OGF for reset command (0x03 shifted left by two bits as the OGF
 *          occupies the uopper 6 bits of this byte.
 *  0x00    Parameter length of reset command (no parameters).
 *
 * @param data Byte received over serial port
 */
void
ble_hci_uart_rx_sync_loss(uint8_t data)
{
    int rc;
    int index;

    /*
     * If we couldnt allocate a command buffer (should not occur but
     * possible) try to allocate one on each received character. If we get
     * a reset and buffer is not available we have to ignore reset.
     */
    if (ble_hci_uart_state.rx_cmd.data == NULL) {
        ble_hci_uart_state.rx_cmd.data =
            ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_CMD);
    }

    index = ble_hci_uart_state.rx_cmd.cur;
    if (data == ble_hci_uart_reset_cmd[index]) {
        if (index == 3) {
            if (ble_hci_uart_state.rx_cmd.data == NULL) {
                index = 0;
            } else {
                assert(ble_hci_uart_rx_cmd_cb != NULL);
                ble_hci_uart_state.rx_cmd.data[0] = 0x03;
                ble_hci_uart_state.rx_cmd.data[1] = 0x0C;
                ble_hci_uart_state.rx_cmd.data[2] = 0x00;
                rc = ble_hci_uart_rx_cmd_cb(ble_hci_uart_state.rx_cmd.data,
                                            ble_hci_uart_rx_cmd_arg);
                if (rc != 0) {
                    ble_hci_trans_buf_free(ble_hci_uart_state.rx_cmd.data);
                }
                ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;
            }
        } else {
            ++index;
        }
    } else {
        index = 0;
    }

    ble_hci_uart_state.rx_cmd.cur = index;
}

static void
ble_hci_uart_rx_cmd(uint8_t data)
{
    int rc;

    ble_hci_uart_state.rx_cmd.data[ble_hci_uart_state.rx_cmd.cur++] = data;

    if (ble_hci_uart_state.rx_cmd.cur < BLE_HCI_CMD_HDR_LEN) {
        return;
    }

    if (ble_hci_uart_state.rx_cmd.cur == BLE_HCI_CMD_HDR_LEN) {
        ble_hci_uart_state.rx_cmd.len = ble_hci_uart_state.rx_cmd.data[2] +
                                         BLE_HCI_CMD_HDR_LEN;
    }

    if (ble_hci_uart_state.rx_cmd.cur == ble_hci_uart_state.rx_cmd.len) {
        assert(ble_hci_uart_rx_cmd_cb != NULL);
        rc = ble_hci_uart_rx_cmd_cb(ble_hci_uart_state.rx_cmd.data,
                                    ble_hci_uart_rx_cmd_arg);
        if (rc != 0) {
            ble_hci_trans_buf_free(ble_hci_uart_state.rx_cmd.data);
        }
        ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;
    }
}

static void
ble_hci_uart_rx_skip_cmd(uint8_t data)
{
    ble_hci_uart_state.rx_cmd.cur++;

    if (ble_hci_uart_state.rx_cmd.cur < BLE_HCI_CMD_HDR_LEN) {
        return;
    }

    if (ble_hci_uart_state.rx_cmd.cur == BLE_HCI_CMD_HDR_LEN) {
        ble_hci_uart_state.rx_cmd.len = data + BLE_HCI_CMD_HDR_LEN;
    }

    if (ble_hci_uart_state.rx_cmd.cur == ble_hci_uart_state.rx_cmd.len) {
        /*
         * XXX: for now we simply skip the command and do nothing. This
         * should not happen but at least we retain HCI synch. The host
         * can decide what to do in this case. It may be appropriate for
         * the controller to attempt to send back a command complete or
         * command status in this case.
         */
        ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;
    }
}
#endif

#if MYNEWT_VAL(BLE_HOST)
static inline void
ble_hci_uart_rx_evt_cb()
{
    int rc;

    if (ble_hci_uart_state.rx_cmd.cur == ble_hci_uart_state.rx_cmd.len) {
        assert(ble_hci_uart_rx_cmd_cb != NULL);
        rc = ble_hci_uart_rx_cmd_cb(ble_hci_uart_state.rx_cmd.data,
                                    ble_hci_uart_rx_cmd_arg);
        if (rc != 0) {
            ble_hci_trans_buf_free(ble_hci_uart_state.rx_cmd.data);
        }
        ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;
    }
}

static void
ble_hci_uart_rx_evt(uint8_t data)
{
    /* Determine event priority to allocate buffer */
    if (!ble_hci_uart_state.rx_cmd.data) {
        /* In case of LE Meta Event priority might be still unknown */
        if (data == BLE_HCI_EVCODE_LE_META) {
            ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_LE_EVT;
            ble_hci_uart_state.rx_cmd.cur++;
            return;
        }

        ble_hci_uart_state.rx_cmd.data =
            ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
        assert(ble_hci_uart_state.rx_cmd.data != NULL);
    }

    ble_hci_uart_state.rx_cmd.data[ble_hci_uart_state.rx_cmd.cur++] = data;

    if (ble_hci_uart_state.rx_cmd.cur < BLE_HCI_EVENT_HDR_LEN) {
        return;
    }

    if (ble_hci_uart_state.rx_cmd.cur == BLE_HCI_EVENT_HDR_LEN) {
        ble_hci_uart_state.rx_cmd.len = ble_hci_uart_state.rx_cmd.data[1] +
                                        BLE_HCI_EVENT_HDR_LEN;
    }

    ble_hci_uart_rx_evt_cb();
}

static void
ble_hci_uart_rx_le_evt(uint8_t data)
{
    ble_hci_uart_state.rx_cmd.cur++;

    if (ble_hci_uart_state.rx_cmd.cur == BLE_HCI_EVENT_HDR_LEN) {
        /* LE Meta Event parameter length is never 0 */
        assert(data != 0);
        ble_hci_uart_state.rx_cmd.len = data + BLE_HCI_EVENT_HDR_LEN;
        return;
    }

    /* Determine event priority to allocate buffer */
    if (!ble_hci_uart_state.rx_cmd.data) {
        /* Determine event priority to allocate buffer */
        if (data == BLE_HCI_LE_SUBEV_ADV_RPT ||
                data == BLE_HCI_LE_SUBEV_EXT_ADV_RPT) {
            ble_hci_uart_state.rx_cmd.data =
                ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_LO);
            if (ble_hci_uart_state.rx_cmd.data == NULL) {
                ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_SKIP_EVT;
                return;
            }
        } else {
            ble_hci_uart_state.rx_cmd.data =
                ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
            assert(ble_hci_uart_state.rx_cmd.data != NULL);
        }

        ble_hci_uart_state.rx_cmd.data[0] = BLE_HCI_EVCODE_LE_META;
        ble_hci_uart_state.rx_cmd.data[1] =
                ble_hci_uart_state.rx_cmd.len - BLE_HCI_EVENT_HDR_LEN;
    }

    ble_hci_uart_state.rx_cmd.data[ble_hci_uart_state.rx_cmd.cur - 1] = data;
    ble_hci_uart_rx_evt_cb();
}

static void
ble_hci_uart_rx_skip_evt(uint8_t data)
{
    if (++ble_hci_uart_state.rx_cmd.cur == ble_hci_uart_state.rx_cmd.len) {
        ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;
    }
}
#endif

static void
ble_hci_uart_rx_acl(uint8_t data)
{
    uint16_t rxd_bytes;
    uint16_t pktlen;

    rxd_bytes = ble_hci_uart_state.rx_acl.rxd_bytes;
    ble_hci_uart_state.rx_acl.dptr[rxd_bytes] = data;
    ++rxd_bytes;
    ble_hci_uart_state.rx_acl.rxd_bytes = rxd_bytes;

    if (rxd_bytes < BLE_HCI_DATA_HDR_SZ) {
        return;
    }

    if (rxd_bytes == BLE_HCI_DATA_HDR_SZ) {
        pktlen = ble_hci_uart_state.rx_acl.dptr[3];
        pktlen = (pktlen << 8) + ble_hci_uart_state.rx_acl.dptr[2];
        ble_hci_uart_state.rx_acl.len = pktlen + BLE_HCI_DATA_HDR_SZ;

        /*
         * Data portion cannot exceed data length of acl buffer. If it does
         * this is considered to be a loss of sync.
         */
        if (pktlen > MYNEWT_VAL(BLE_ACL_BUF_SIZE)) {
            os_mbuf_free_chain(ble_hci_uart_state.rx_acl.buf);
#if MYNEWT_VAL(BLE_CONTROLLER)
            ble_hci_uart_sync_lost();
#else
        /*
         * XXX: not sure what to do about host in this case. Just go back to
         * none for now.
         */
        ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;
#endif
        }
    }

    if (rxd_bytes == ble_hci_uart_state.rx_acl.len) {
        assert(ble_hci_uart_rx_acl_cb != NULL);
        /* XXX: can this callback fail? What if it does? */
        OS_MBUF_PKTLEN(ble_hci_uart_state.rx_acl.buf) = rxd_bytes;
        ble_hci_uart_state.rx_acl.buf->om_len = rxd_bytes;
        ble_hci_uart_rx_acl_cb(ble_hci_uart_state.rx_acl.buf,
                               ble_hci_uart_rx_acl_arg);
        ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;
    }
}

static void
ble_hci_uart_rx_skip_acl(uint8_t data)
{
    uint16_t rxd_bytes;
    uint16_t pktlen;

    rxd_bytes = ble_hci_uart_state.rx_acl.rxd_bytes;
    ++rxd_bytes;
    ble_hci_uart_state.rx_acl.rxd_bytes = rxd_bytes;

    if (rxd_bytes == (BLE_HCI_DATA_HDR_SZ - 1)) {
        ble_hci_uart_state.rx_acl.len = data;
        return;
    }

    if (rxd_bytes == BLE_HCI_DATA_HDR_SZ) {
        pktlen = data;
        pktlen = (pktlen << 8) + ble_hci_uart_state.rx_acl.len;
        ble_hci_uart_state.rx_acl.len = pktlen + BLE_HCI_DATA_HDR_SZ;
    }

    if (rxd_bytes == ble_hci_uart_state.rx_acl.len) {
/* XXX: I dont like this but for now this denotes controller only */
#if MYNEWT_VAL(BLE_CONTROLLER)
        ble_ll_data_buffer_overflow();
#endif
        ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;
    }
}

static int
ble_hci_uart_rx_char(void *arg, uint8_t data)
{
    switch (ble_hci_uart_state.rx_type) {
    case BLE_HCI_UART_H4_NONE:
        return ble_hci_uart_rx_pkt_type(data);
#if MYNEWT_VAL(BLE_CONTROLLER)
    case BLE_HCI_UART_H4_CMD:
        ble_hci_uart_rx_cmd(data);
        return 0;
    case BLE_HCI_UART_H4_SKIP_CMD:
        ble_hci_uart_rx_skip_cmd(data);
        return 0;
    case BLE_HCI_UART_H4_SYNC_LOSS:
        ble_hci_uart_rx_sync_loss(data);
        return 0;
#endif
#if MYNEWT_VAL(BLE_HOST)
    case BLE_HCI_UART_H4_EVT:
        ble_hci_uart_rx_evt(data);
        return 0;
    case BLE_HCI_UART_H4_LE_EVT:
        ble_hci_uart_rx_le_evt(data);
        return 0;
    case BLE_HCI_UART_H4_SKIP_EVT:
        ble_hci_uart_rx_skip_evt(data);
        return 0;
#endif
    case BLE_HCI_UART_H4_ACL:
        ble_hci_uart_rx_acl(data);
        return 0;
    case BLE_HCI_UART_H4_SKIP_ACL:
        ble_hci_uart_rx_skip_acl(data);
        return 0;
    default:
        /* This should never happen! */
        assert(0);
        return 0;
    }
}

static void
ble_hci_uart_set_rx_cbs(ble_hci_trans_rx_cmd_fn *cmd_cb,
                        void *cmd_arg,
                        ble_hci_trans_rx_acl_fn *acl_cb,
                        void *acl_arg)
{
    ble_hci_uart_rx_cmd_cb = cmd_cb;
    ble_hci_uart_rx_cmd_arg = cmd_arg;
    ble_hci_uart_rx_acl_cb = acl_cb;
    ble_hci_uart_rx_acl_arg = acl_arg;
}

static void
ble_hci_uart_free_pkt(uint8_t type, uint8_t *cmdevt, struct os_mbuf *acl)
{
    switch (type) {
    case BLE_HCI_UART_H4_NONE:
        break;

    case BLE_HCI_UART_H4_CMD:
    case BLE_HCI_UART_H4_EVT:
        ble_hci_trans_buf_free(cmdevt);
        break;

    case BLE_HCI_UART_H4_ACL:
        os_mbuf_free_chain(acl);
        break;

    default:
        assert(0);
        break;
    }
}

static int
ble_hci_uart_config(void)
{
    int rc;

    rc = hal_uart_init_cbs(MYNEWT_VAL(BLE_HCI_UART_PORT),
                           ble_hci_uart_tx_char, NULL,
                           ble_hci_uart_rx_char, NULL);
    if (rc != 0) {
        return BLE_ERR_UNSPECIFIED;
    }

    rc = hal_uart_config(MYNEWT_VAL(BLE_HCI_UART_PORT),
                         MYNEWT_VAL(BLE_HCI_UART_BAUD),
                         MYNEWT_VAL(BLE_HCI_UART_DATA_BITS),
                         MYNEWT_VAL(BLE_HCI_UART_STOP_BITS),
                         MYNEWT_VAL(BLE_HCI_UART_PARITY),
                         MYNEWT_VAL(BLE_HCI_UART_FLOW_CTRL));
    if (rc != 0) {
        return BLE_ERR_HW_FAIL;
    }

    return 0;
}

/**
 * Sends an HCI event from the controller to the host.
 *
 * @param cmd                   The HCI event to send.  This buffer must be
 *                                  allocated via ble_hci_trans_buf_alloc().
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_ll_evt_tx(uint8_t *cmd)
{
    int rc;

    rc = ble_hci_uart_cmdevt_tx(cmd, BLE_HCI_UART_H4_EVT);
    return rc;
}

/**
 * Sends ACL data from controller to host.
 *
 * @param om                    The ACL data packet to send.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_ll_acl_tx(struct os_mbuf *om)
{
    int rc;

    rc = ble_hci_uart_acl_tx(om);
    return rc;
}

/**
 * Sends an HCI command from the host to the controller.
 *
 * @param cmd                   The HCI command to send.  This buffer must be
 *                                  allocated via ble_hci_trans_buf_alloc().
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_hs_cmd_tx(uint8_t *cmd)
{
    int rc;

    rc = ble_hci_uart_cmdevt_tx(cmd, BLE_HCI_UART_H4_CMD);
    return rc;
}

/**
 * Sends ACL data from host to controller.
 *
 * @param om                    The ACL data packet to send.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_hs_acl_tx(struct os_mbuf *om)
{
    int rc;

    rc = ble_hci_uart_acl_tx(om);
    return rc;
}

/**
 * Configures the HCI transport to call the specified callback upon receiving
 * HCI packets from the controller.  This function should only be called by by
 * host.
 *
 * @param cmd_cb                The callback to execute upon receiving an HCI
 *                                  event.
 * @param cmd_arg               Optional argument to pass to the command
 *                                  callback.
 * @param acl_cb                The callback to execute upon receiving ACL
 *                                  data.
 * @param acl_arg               Optional argument to pass to the ACL
 *                                  callback.
 */
void
ble_hci_trans_cfg_hs(ble_hci_trans_rx_cmd_fn *cmd_cb,
                     void *cmd_arg,
                     ble_hci_trans_rx_acl_fn *acl_cb,
                     void *acl_arg)
{
    ble_hci_uart_set_rx_cbs(cmd_cb, cmd_arg, acl_cb, acl_arg);
}

/**
 * Configures the HCI transport to operate with a host.  The transport will
 * execute specified callbacks upon receiving HCI packets from the controller.
 *
 * @param cmd_cb                The callback to execute upon receiving an HCI
 *                                  event.
 * @param cmd_arg               Optional argument to pass to the command
 *                                  callback.
 * @param acl_cb                The callback to execute upon receiving ACL
 *                                  data.
 * @param acl_arg               Optional argument to pass to the ACL
 *                                  callback.
 */
void
ble_hci_trans_cfg_ll(ble_hci_trans_rx_cmd_fn *cmd_cb,
                     void *cmd_arg,
                     ble_hci_trans_rx_acl_fn *acl_cb,
                     void *acl_arg)
{
    ble_hci_uart_set_rx_cbs(cmd_cb, cmd_arg, acl_cb, acl_arg);
}

/**
 * Allocates a flat buffer of the specified type.
 *
 * @param type                  The type of buffer to allocate; one of the
 *                                  BLE_HCI_TRANS_BUF_[...] constants.
 *
 * @return                      The allocated buffer on success;
 *                              NULL on buffer exhaustion.
 */
uint8_t *
ble_hci_trans_buf_alloc(int type)
{
    uint8_t *buf;

    switch (type) {
    case BLE_HCI_TRANS_BUF_CMD:
        buf = os_memblock_get(&ble_hci_uart_cmd_pool);
        break;
    case BLE_HCI_TRANS_BUF_EVT_HI:
        buf = os_memblock_get(&ble_hci_uart_evt_hi_pool);
        if (buf == NULL) {
            /* If no high-priority event buffers remain, try to grab a
             * low-priority one.
             */
            buf = os_memblock_get(&ble_hci_uart_evt_lo_pool);
        }
        break;

    case BLE_HCI_TRANS_BUF_EVT_LO:
        buf = os_memblock_get(&ble_hci_uart_evt_lo_pool);
        break;

    default:
        assert(0);
        buf = NULL;
    }

    return buf;
}

/**
 * Frees the specified flat buffer.  The buffer must have been allocated via
 * ble_hci_trans_buf_alloc().
 *
 * @param buf                   The buffer to free.
 */
void
ble_hci_trans_buf_free(uint8_t *buf)
{
    int rc;

    /*
     * XXX: this may look a bit odd, but the controller uses the command
     * buffer to send back the command complete/status as an immediate
     * response to the command. This was done to insure that the controller
     * could always send back one of these events when a command was received.
     * Thus, we check to see which pool the buffer came from so we can free
     * it to the appropriate pool
     */
    if (os_memblock_from(&ble_hci_uart_evt_hi_pool, buf)) {
        rc = os_memblock_put(&ble_hci_uart_evt_hi_pool, buf);
        assert(rc == 0);
    } else if (os_memblock_from(&ble_hci_uart_evt_lo_pool, buf)) {
        rc = os_memblock_put(&ble_hci_uart_evt_lo_pool, buf);
        assert(rc == 0);
    } else {
        assert(os_memblock_from(&ble_hci_uart_cmd_pool, buf));
        rc = os_memblock_put(&ble_hci_uart_cmd_pool, buf);
        assert(rc == 0);
    }
}

/**
 * Configures a callback to get executed whenever an ACL data packet is freed.
 * The function is called in lieu of actually freeing the packet.
 *
 * @param cb                    The callback to configure.
 *
 * @return                      0 on success.
 */
int
ble_hci_trans_set_acl_free_cb(os_mempool_put_fn *cb, void *arg)
{
    ble_hci_uart_acl_pool.mpe_put_cb = cb;
    ble_hci_uart_acl_pool.mpe_put_arg = arg;
    return 0;
}

/**
 * Resets the HCI UART transport to a clean state.  Frees all buffers and
 * reconfigures the UART.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_reset(void)
{
    struct ble_hci_uart_pkt *pkt;
    int rc;

    /* Close the UART to prevent race conditions as the buffers are freed. */
    rc = hal_uart_close(MYNEWT_VAL(BLE_HCI_UART_PORT));
    if (rc != 0) {
        return BLE_ERR_HW_FAIL;
    }

    ble_hci_uart_free_pkt(ble_hci_uart_state.rx_type,
                          ble_hci_uart_state.rx_cmd.data,
                          ble_hci_uart_state.rx_acl.buf);
    ble_hci_uart_state.rx_type = BLE_HCI_UART_H4_NONE;

    ble_hci_uart_free_pkt(ble_hci_uart_state.tx_type,
                          ble_hci_uart_state.tx_cmd.data,
                          ble_hci_uart_state.tx_pkt.tx_acl);
    ble_hci_uart_state.tx_type = BLE_HCI_UART_H4_NONE;

    while ((pkt = STAILQ_FIRST(&ble_hci_uart_state.tx_pkts)) != NULL) {
        STAILQ_REMOVE(&ble_hci_uart_state.tx_pkts, pkt, ble_hci_uart_pkt,
                      next);
        ble_hci_uart_free_pkt(pkt->type, pkt->data, pkt->data);
        os_memblock_put(&ble_hci_uart_pkt_pool, pkt);
    }

    /* Reopen the UART. */
    rc = ble_hci_uart_config();
    if (rc != 0) {
        return rc;
    }

    return 0;
}

/**
 * Initializes the UART HCI transport module.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
void
ble_hci_uart_init(void)
{
    int rc;

    /* Ensure this function only gets called by sysinit. */
    SYSINIT_ASSERT_ACTIVE();

    rc = os_mempool_ext_init(&ble_hci_uart_acl_pool,
                             MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                             ACL_BLOCK_SIZE,
                             ble_hci_uart_acl_buf,
                             "ble_hci_uart_acl_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mbuf_pool_init(&ble_hci_uart_acl_mbuf_pool,
                           &ble_hci_uart_acl_pool.mpe_mp,
                           ACL_BLOCK_SIZE,
                           MYNEWT_VAL(BLE_ACL_BUF_COUNT));
    SYSINIT_PANIC_ASSERT(rc == 0);

    /*
     * Create memory pool of HCI command buffers. NOTE: we currently dont
     * allow this to be configured. The controller will only allow one
     * outstanding command. We decided to keep this a pool in case we allow
     * allow the controller to handle more than one outstanding command.
     */
    rc = os_mempool_init(&ble_hci_uart_cmd_pool,
                         1,
                         BLE_HCI_TRANS_CMD_SZ,
                         ble_hci_uart_cmd_buf,
                         "ble_hci_uart_cmd_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&ble_hci_uart_evt_hi_pool,
                         MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                         MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                         ble_hci_uart_evt_hi_buf,
                         "ble_hci_uart_evt_hi_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&ble_hci_uart_evt_lo_pool,
                         MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                         MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                         ble_hci_uart_evt_lo_buf,
                         "ble_hci_uart_evt_lo_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    /*
     * Create memory pool of packet list nodes. NOTE: the number of these
     * buffers should be, at least, the total number of event buffers (hi
     * and lo), the number of command buffers (currently 1) and the total
     * number of buffers that the controller could possibly hand to the host.
     */
    rc = os_mempool_init(&ble_hci_uart_pkt_pool,
                         BLE_HCI_UART_EVT_COUNT + 1 +
                         MYNEWT_VAL(BLE_HCI_ACL_OUT_COUNT),
                         sizeof (struct ble_hci_uart_pkt),
                         ble_hci_uart_pkt_buf,
                         "ble_hci_uart_pkt_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = ble_hci_uart_config();
    SYSINIT_PANIC_ASSERT_MSG(rc == 0, "Failure configuring UART HCI");

    memset(&ble_hci_uart_state, 0, sizeof ble_hci_uart_state);
    STAILQ_INIT(&ble_hci_uart_state.tx_pkts);
}
