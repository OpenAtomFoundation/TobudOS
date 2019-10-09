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
#include "hal/hal_spi.h"

/* BLE */
#include "nimble/ble.h"
#include "nimble/nimble_opt.h"
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"

#include "transport/emspi/ble_hci_emspi.h"

#include "am_mcu_apollo.h"

/***
 * NOTES:
 * The emspi HCI transport doesn't use event buffer priorities.  All incoming
 * and outgoing events use buffers from the same pool.
 *
 */

#define BLE_HCI_EMSPI_PKT_EVT_COUNT         \
    (MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT) + \
     MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT))

#define BLE_HCI_EMSPI_PKT_NONE          0x00
#define BLE_HCI_EMSPI_PKT_CMD           0x01
#define BLE_HCI_EMSPI_PKT_ACL           0x02
#define BLE_HCI_EMSPI_PKT_EVT           0x04

#define BLE_HCI_EMSPI_CTLR_STATUS_OK    0xc0
#define BLE_HCI_EMSPI_OP_TX             0x42
#define BLE_HCI_EMSPI_OP_RX             0x81

static os_event_fn ble_hci_emspi_event_txrx;

static struct os_event ble_hci_emspi_ev_txrx = {
    .ev_cb = ble_hci_emspi_event_txrx,
};

static struct os_eventq ble_hci_emspi_evq;
static struct os_task ble_hci_emspi_task;
static os_stack_t ble_hci_emspi_stack[MYNEWT_VAL(BLE_HCI_EMSPI_STACK_SIZE)];

static ble_hci_trans_rx_cmd_fn *ble_hci_emspi_rx_cmd_cb;
static void *ble_hci_emspi_rx_cmd_arg;

static ble_hci_trans_rx_acl_fn *ble_hci_emspi_rx_acl_cb;
static void *ble_hci_emspi_rx_acl_arg;

static struct os_mempool ble_hci_emspi_evt_hi_pool;
static os_membuf_t ble_hci_emspi_evt_hi_buf[
        OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                        MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE))
];

static struct os_mempool ble_hci_emspi_evt_lo_pool;
static os_membuf_t ble_hci_emspi_evt_lo_buf[
        OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                        MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE))
];

static struct os_mempool ble_hci_emspi_cmd_pool;
static os_membuf_t ble_hci_emspi_cmd_buf[
        OS_MEMPOOL_SIZE(1, BLE_HCI_TRANS_CMD_SZ)
];

static struct os_mbuf_pool ble_hci_emspi_acl_mbuf_pool;
static struct os_mempool_ext ble_hci_emspi_acl_pool;

/*
 * The MBUF payload size must accommodate the HCI data header size plus the
 * maximum ACL data packet length. The ACL block size is the size of the
 * mbufs we will allocate.
 */
#define ACL_BLOCK_SIZE  OS_ALIGN(MYNEWT_VAL(BLE_ACL_BUF_SIZE) \
                                 + BLE_MBUF_MEMBLOCK_OVERHEAD \
                                 + BLE_HCI_DATA_HDR_SZ, OS_ALIGNMENT)

static os_membuf_t ble_hci_emspi_acl_buf[
        OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                        ACL_BLOCK_SIZE)
];

/**
 * A packet to be sent over the EMSPI.  This can be a command, an event, or ACL
 * data.
 */
struct ble_hci_emspi_pkt {
    STAILQ_ENTRY(ble_hci_emspi_pkt) next;
    void *data;
    uint8_t type;
};
STAILQ_HEAD(, ble_hci_emspi_pkt) ble_hci_emspi_tx_q;

static struct os_mempool ble_hci_emspi_pkt_pool;
static os_membuf_t ble_hci_emspi_pkt_buf[
        OS_MEMPOOL_SIZE(BLE_HCI_EMSPI_PKT_EVT_COUNT + 1 +
                        MYNEWT_VAL(BLE_HCI_ACL_OUT_COUNT),
                        sizeof (struct ble_hci_emspi_pkt))
];

static void
ble_hci_emspi_rdy_isr(void *arg)
{
    os_eventq_put(&ble_hci_emspi_evq, &ble_hci_emspi_ev_txrx);
}

static void
ble_hci_emspi_initiate_write(void)
{
    hal_gpio_irq_disable(MYNEWT_VAL(BLE_HCI_EMSPI_RDY_PIN));

    /* Assert slave select. */
    hal_gpio_write(MYNEWT_VAL(BLE_HCI_EMSPI_SS_PIN), 0);

    /* Wait for controller to indicate ready-to-receive. */
    while (!hal_gpio_read(MYNEWT_VAL(BLE_HCI_EMSPI_RDY_PIN))) { }
}

static void
ble_hci_emspi_terminate_write(void)
{
    const uint64_t rdy_mask =
        AM_HAL_GPIO_BIT(MYNEWT_VAL(BLE_HCI_EMSPI_RDY_PIN));
    os_sr_t sr;

    am_hal_gpio_int_clear(rdy_mask);

    /* Deassert slave select. */
    hal_gpio_write(MYNEWT_VAL(BLE_HCI_EMSPI_SS_PIN), 1);

    OS_ENTER_CRITICAL(sr);
    hal_gpio_irq_enable(MYNEWT_VAL(BLE_HCI_EMSPI_RDY_PIN));

    if (hal_gpio_read(MYNEWT_VAL(BLE_HCI_EMSPI_RDY_PIN))) {
        am_hal_gpio_int_set(rdy_mask);
    }

    OS_EXIT_CRITICAL(sr);
}

static int
ble_hci_emspi_write_hdr(uint8_t first_byte, uint8_t *out_buf_size)
{
    const uint8_t hdr[2] = { first_byte, 0x00 };
    uint8_t rx[2];
    int rc;

    /* Send command header. */
    rc = hal_spi_txrx(MYNEWT_VAL(BLE_HCI_EMSPI_SPI_NUM), (void *)hdr, rx, 2);
    if (rc != 0) {
        return rc;
    }

    /* Check for "OK" status. */
    if (rx[0] != BLE_HCI_EMSPI_CTLR_STATUS_OK) {
        return BLE_ERR_HW_FAIL;
    }

    *out_buf_size = rx[1];
    return 0;
}

/**
 * Transmits a chunk of bytes to the controller.
 */
static int
ble_hci_emspi_tx_chunk(const uint8_t *data, int len, int *out_bytes_txed)
{
    uint8_t buf_size;
    int rc;

    /* Silence spurious "may be used uninitialized" warning. */
    *out_bytes_txed = 0;

    ble_hci_emspi_initiate_write();

    rc = ble_hci_emspi_write_hdr(BLE_HCI_EMSPI_OP_TX, &buf_size);
    if (rc != 0) {
        goto done;
    }

    if (buf_size == 0) {
        rc = 0;
        goto done;
    }

    if (buf_size < len) {
        len = buf_size;
    }
    rc = hal_spi_txrx(MYNEWT_VAL(BLE_HCI_EMSPI_SPI_NUM), (void *)data, NULL,
                      len);
    if (rc != 0) {
        goto done;
    }
    *out_bytes_txed = len;

done:
    ble_hci_emspi_terminate_write();
    return rc;
}

/**
 * Transmits a full command or ACL data packet to the controller.
 */
static int
ble_hci_emspi_tx(const uint8_t *data, int len)
{
    int bytes_txed;
    int rc;

    while (len > 0) {
        rc = ble_hci_emspi_tx_chunk(data, len, &bytes_txed);
        if (rc != 0) {
            goto done;
        }

        data += bytes_txed;
        len -= bytes_txed;
    }

    rc = 0;

done:
    return rc;
}

/**
 * Reads the specified number of bytes from the controller.
 */
static int
ble_hci_emspi_rx(uint8_t *data, int max_len)
{
    uint8_t buf_size;
    int rc;

    ble_hci_emspi_initiate_write();

    rc = ble_hci_emspi_write_hdr(BLE_HCI_EMSPI_OP_RX, &buf_size);
    if (rc != 0) {
        goto done;
    }

    if (buf_size > max_len) {
        buf_size = max_len;
    }

    rc = hal_spi_txrx(MYNEWT_VAL(BLE_HCI_EMSPI_SPI_NUM), NULL, data, buf_size);
    if (rc != 0) {
        rc = BLE_ERR_HW_FAIL;
        goto done;
    }

done:
    ble_hci_emspi_terminate_write();
    return rc;
}

/**
 * Allocates a buffer (mbuf) for ACL operation.
 *
 * @return                      The allocated buffer on success;
 *                              NULL on buffer exhaustion.
 */
static struct os_mbuf *
ble_hci_trans_acl_buf_alloc(void)
{
    uint8_t usrhdr_len;

#if MYNEWT_VAL(BLE_HS_FLOW_CTRL)
    usrhdr_len = BLE_MBUF_HS_HDR_LEN;
#else
    usrhdr_len = 0;
#endif

    return os_mbuf_get_pkthdr(&ble_hci_emspi_acl_mbuf_pool, usrhdr_len);
}

/**
 * Transmits an ACL data packet to the controller.  The caller relinquishes the
 * specified mbuf, regardless of return status.
 */
static int
ble_hci_emspi_acl_tx(struct os_mbuf *om)
{
    struct ble_hci_emspi_pkt *pkt;
    os_sr_t sr;

    /* If this packet is zero length, just free it */
    if (OS_MBUF_PKTLEN(om) == 0) {
        os_mbuf_free_chain(om);
        return 0;
    }

    pkt = os_memblock_get(&ble_hci_emspi_pkt_pool);
    if (pkt == NULL) {
        os_mbuf_free_chain(om);
        return BLE_ERR_MEM_CAPACITY;
    }

    pkt->type = BLE_HCI_EMSPI_PKT_ACL;
    pkt->data = om;

    OS_ENTER_CRITICAL(sr);
    STAILQ_INSERT_TAIL(&ble_hci_emspi_tx_q, pkt, next);
    OS_EXIT_CRITICAL(sr);

    os_eventq_put(&ble_hci_emspi_evq, &ble_hci_emspi_ev_txrx);

    return 0;
}

/**
 * Transmits a command packet to the controller.  The caller relinquishes the
 * specified buffer, regardless of return status.
 */
static int
ble_hci_emspi_cmdevt_tx(uint8_t *cmd_buf, uint8_t pkt_type)
{
    struct ble_hci_emspi_pkt *pkt;
    os_sr_t sr;

    pkt = os_memblock_get(&ble_hci_emspi_pkt_pool);
    if (pkt == NULL) {
        ble_hci_trans_buf_free(cmd_buf);
        return BLE_ERR_MEM_CAPACITY;
    }

    pkt->type = pkt_type;
    pkt->data = cmd_buf;

    OS_ENTER_CRITICAL(sr);
    STAILQ_INSERT_TAIL(&ble_hci_emspi_tx_q, pkt, next);
    OS_EXIT_CRITICAL(sr);

    os_eventq_put(&ble_hci_emspi_evq, &ble_hci_emspi_ev_txrx);

    return 0;
}

static int
ble_hci_emspi_tx_flat(const uint8_t *data, int len)
{
    int rc;

    rc = ble_hci_emspi_tx(data, len);
    return rc;
}

static int
ble_hci_emspi_tx_pkt_type(uint8_t pkt_type)
{
    return ble_hci_emspi_tx_flat(&pkt_type, 1);
}

static int
ble_hci_emspi_tx_cmd(const uint8_t *data)
{
    int len;
    int rc;

    rc = ble_hci_emspi_tx_pkt_type(BLE_HCI_EMSPI_PKT_CMD);
    if (rc != 0) {
        return rc;
    }

    len = data[2] + BLE_HCI_CMD_HDR_LEN;
    rc = ble_hci_emspi_tx_flat(data, len);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

static int
ble_hci_emspi_tx_acl(struct os_mbuf *om)
{
    struct os_mbuf *cur;
    int rc;

    rc = ble_hci_emspi_tx_pkt_type(BLE_HCI_EMSPI_PKT_ACL);
    if (rc != 0) {
        return rc;
    }

    cur = om;
    while (cur != NULL) {
        rc = ble_hci_emspi_tx(cur->om_data, cur->om_len);
        if (rc != 0) {
            break;
        }

        cur = SLIST_NEXT(cur, om_next);
    }

    return rc;
}

static struct ble_hci_emspi_pkt *
ble_hci_emspi_pull_next_tx(void)
{
    struct ble_hci_emspi_pkt *pkt;
    os_sr_t sr;

    OS_ENTER_CRITICAL(sr);
    pkt = STAILQ_FIRST(&ble_hci_emspi_tx_q);
    if (pkt != NULL) {
        STAILQ_REMOVE(&ble_hci_emspi_tx_q, pkt, ble_hci_emspi_pkt, next);
    }
    OS_EXIT_CRITICAL(sr);

    return pkt;
}

static int
ble_hci_emspi_tx_pkt(void)
{
    struct ble_hci_emspi_pkt *pkt;
    int rc;

    pkt = ble_hci_emspi_pull_next_tx();
    if (pkt == NULL) {
        return -1;
    }

    switch (pkt->type) {
    case BLE_HCI_EMSPI_PKT_CMD:
        rc = ble_hci_emspi_tx_cmd(pkt->data);
        ble_hci_trans_buf_free(pkt->data);
        break;

    case BLE_HCI_EMSPI_PKT_ACL:
        rc = ble_hci_emspi_tx_acl(pkt->data);
        os_mbuf_free_chain(pkt->data);
        break;

    default:
        rc = -1;
        break;
    }

    os_memblock_put(&ble_hci_emspi_pkt_pool, pkt);

    return rc;
}

static int
ble_hci_emspi_rx_evt(void)
{
    uint8_t *data;
    uint8_t len;
    int rc;

    /* XXX: we should not assert if host cannot allocate an event. Need
     * to determine what to do here.
     */
    data = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
    assert(data != NULL);

    rc = ble_hci_emspi_rx(data, BLE_HCI_EVENT_HDR_LEN);
    if (rc != 0) {
        goto err;
    }

    len = data[1];
    if (len > 0) {
        rc = ble_hci_emspi_rx(data + BLE_HCI_EVENT_HDR_LEN, len);
        if (rc != 0) {
            goto err;
        }
    }

    assert(ble_hci_emspi_rx_cmd_cb != NULL);
    ble_hci_emspi_rx_cmd_cb(data, ble_hci_emspi_rx_cmd_arg);
    if (rc != 0) {
        goto err;
    }

    return 0;

err:
    ble_hci_trans_buf_free(data);
    return rc;
}

static int
ble_hci_emspi_rx_acl(void)
{
    struct os_mbuf *om;
    uint16_t len;
    int rc;

    /* XXX: we should not assert if host cannot allocate an mbuf. Need to
     * determine what to do here.
     */
    om = ble_hci_trans_acl_buf_alloc();
    assert(om != NULL);

    rc = ble_hci_emspi_rx(om->om_data, BLE_HCI_DATA_HDR_SZ);
    if (rc != 0) {
        goto err;
    }

    len = get_le16(om->om_data + 2);
    if (len > MYNEWT_VAL(BLE_ACL_BUF_SIZE)) {
        /*
         * Data portion cannot exceed data length of acl buffer. If it does
         * this is considered to be a loss of sync.
         */
        rc = BLE_ERR_UNSPECIFIED;
        goto err;
    }

    if (len > 0) {
        rc = ble_hci_emspi_rx(om->om_data + BLE_HCI_DATA_HDR_SZ, len);
        if (rc != 0) {
            goto err;
        }
    }

    OS_MBUF_PKTLEN(om) = BLE_HCI_DATA_HDR_SZ + len;
    om->om_len = BLE_HCI_DATA_HDR_SZ + len;

    assert(ble_hci_emspi_rx_cmd_cb != NULL);
    rc = ble_hci_emspi_rx_acl_cb(om, ble_hci_emspi_rx_acl_arg);
    if (rc != 0) {
        goto err;
    }

    return 0;

err:
    os_mbuf_free_chain(om);
    return rc;
}

/**
 * @return                      The type of packet to follow success;
 *                              -1 if there is no valid packet to receive.
 */
static int
ble_hci_emspi_rx_pkt(void)
{
    uint8_t pkt_type;
    int rc;

    /* XXX: This is awkward; should read the full packet in "one go". */
    rc = ble_hci_emspi_rx(&pkt_type, 1);
    if (rc != 0) {
        return rc;
    }

    switch (pkt_type) {
    case BLE_HCI_EMSPI_PKT_EVT:
        return ble_hci_emspi_rx_evt();

    case BLE_HCI_EMSPI_PKT_ACL:
        return ble_hci_emspi_rx_acl();

    default:
        /* XXX */
        return -1;
    }
}

static void
ble_hci_emspi_set_rx_cbs(ble_hci_trans_rx_cmd_fn *cmd_cb,
                        void *cmd_arg,
                        ble_hci_trans_rx_acl_fn *acl_cb,
                        void *acl_arg)
{
    ble_hci_emspi_rx_cmd_cb = cmd_cb;
    ble_hci_emspi_rx_cmd_arg = cmd_arg;
    ble_hci_emspi_rx_acl_cb = acl_cb;
    ble_hci_emspi_rx_acl_arg = acl_arg;
}

static void
ble_hci_emspi_free_pkt(uint8_t type, uint8_t *cmdevt, struct os_mbuf *acl)
{
    switch (type) {
    case BLE_HCI_EMSPI_PKT_NONE:
        break;

    case BLE_HCI_EMSPI_PKT_CMD:
    case BLE_HCI_EMSPI_PKT_EVT:
        ble_hci_trans_buf_free(cmdevt);
        break;

    case BLE_HCI_EMSPI_PKT_ACL:
        os_mbuf_free_chain(acl);
        break;

    default:
        assert(0);
        break;
    }
}

/**
 * Unsupported.  This is a host-only transport.
 */
int
ble_hci_trans_ll_evt_tx(uint8_t *cmd)
{
    return BLE_ERR_UNSUPPORTED;
}

/**
 * Unsupported.  This is a host-only transport.
 */
int
ble_hci_trans_ll_acl_tx(struct os_mbuf *om)
{
    return BLE_ERR_UNSUPPORTED;
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

    rc = ble_hci_emspi_cmdevt_tx(cmd, BLE_HCI_EMSPI_PKT_CMD);
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

    rc = ble_hci_emspi_acl_tx(om);
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
    ble_hci_emspi_set_rx_cbs(cmd_cb, cmd_arg, acl_cb, acl_arg);
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
    /* Unsupported. */
    assert(0);
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
        buf = os_memblock_get(&ble_hci_emspi_cmd_pool);
        break;
    case BLE_HCI_TRANS_BUF_EVT_HI:
        buf = os_memblock_get(&ble_hci_emspi_evt_hi_pool);
        if (buf == NULL) {
            /* If no high-priority event buffers remain, try to grab a
             * low-priority one.
             */
            buf = os_memblock_get(&ble_hci_emspi_evt_lo_pool);
        }
        break;

    case BLE_HCI_TRANS_BUF_EVT_LO:
        buf = os_memblock_get(&ble_hci_emspi_evt_lo_pool);
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

    if (buf != NULL) {
        if (os_memblock_from(&ble_hci_emspi_evt_hi_pool, buf)) {
            rc = os_memblock_put(&ble_hci_emspi_evt_hi_pool, buf);
            assert(rc == 0);
        } else if (os_memblock_from(&ble_hci_emspi_evt_lo_pool, buf)) {
            rc = os_memblock_put(&ble_hci_emspi_evt_lo_pool, buf);
            assert(rc == 0);
        } else {
            assert(os_memblock_from(&ble_hci_emspi_cmd_pool, buf));
            rc = os_memblock_put(&ble_hci_emspi_cmd_pool, buf);
            assert(rc == 0);
        }
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
    ble_hci_emspi_acl_pool.mpe_put_cb = cb;
    ble_hci_emspi_acl_pool.mpe_put_arg = arg;
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
    struct ble_hci_emspi_pkt *pkt;

    hal_gpio_write(MYNEWT_VAL(BLE_HCI_EMSPI_RESET_PIN), 1);

    while ((pkt = STAILQ_FIRST(&ble_hci_emspi_tx_q)) != NULL) {
        STAILQ_REMOVE(&ble_hci_emspi_tx_q, pkt, ble_hci_emspi_pkt, next);
        ble_hci_emspi_free_pkt(pkt->type, pkt->data, pkt->data);
        os_memblock_put(&ble_hci_emspi_pkt_pool, pkt);
    }

    return 0;
}

static void
ble_hci_emspi_event_txrx(struct os_event *ev)
{
    int rc;

    rc = ble_hci_emspi_rx_pkt();

    do {
        rc = ble_hci_emspi_tx_pkt();
    } while (rc == 0);
}

static void
ble_hci_emspi_loop(void *unused)
{
    while (1) {
        os_eventq_run(&ble_hci_emspi_evq);
    }
}

static void
ble_hci_emspi_init_hw(void)
{
    struct hal_spi_settings spi_cfg;
    int rc;

    rc = hal_gpio_init_out(MYNEWT_VAL(BLE_HCI_EMSPI_RESET_PIN), 0);
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = hal_gpio_init_out(MYNEWT_VAL(BLE_HCI_EMSPI_SS_PIN), 1);
    SYSINIT_PANIC_ASSERT(rc == 0);

    spi_cfg.data_order = HAL_SPI_MSB_FIRST;
    spi_cfg.data_mode = HAL_SPI_MODE0;
    spi_cfg.baudrate = MYNEWT_VAL(BLE_HCI_EMSPI_BAUD);
    spi_cfg.word_size = HAL_SPI_WORD_SIZE_8BIT;

    rc = hal_spi_config(MYNEWT_VAL(BLE_HCI_EMSPI_SPI_NUM), &spi_cfg);
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = hal_gpio_irq_init(MYNEWT_VAL(BLE_HCI_EMSPI_RDY_PIN),
                           ble_hci_emspi_rdy_isr, NULL,
                           HAL_GPIO_TRIG_RISING, HAL_GPIO_PULL_DOWN);
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = hal_spi_enable(MYNEWT_VAL(BLE_HCI_EMSPI_SPI_NUM));
    assert(rc == 0);

    hal_gpio_write(MYNEWT_VAL(BLE_HCI_EMSPI_RESET_PIN), 1);
}

/**
 * Initializes the UART HCI transport module.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
void
ble_hci_emspi_init(void)
{
    int rc;

    /* Ensure this function only gets called by sysinit. */
    SYSINIT_ASSERT_ACTIVE();

    rc = os_mempool_ext_init(&ble_hci_emspi_acl_pool,
                             MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                             ACL_BLOCK_SIZE,
                             ble_hci_emspi_acl_buf,
                             "ble_hci_emspi_acl_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mbuf_pool_init(&ble_hci_emspi_acl_mbuf_pool,
                           &ble_hci_emspi_acl_pool.mpe_mp,
                           ACL_BLOCK_SIZE,
                           MYNEWT_VAL(BLE_ACL_BUF_COUNT));
    SYSINIT_PANIC_ASSERT(rc == 0);

    /*
     * Create memory pool of HCI command buffers. NOTE: we currently dont
     * allow this to be configured. The controller will only allow one
     * outstanding command. We decided to keep this a pool in case we allow
     * allow the controller to handle more than one outstanding command.
     */
    rc = os_mempool_init(&ble_hci_emspi_cmd_pool,
                         1,
                         BLE_HCI_TRANS_CMD_SZ,
                         ble_hci_emspi_cmd_buf,
                         "ble_hci_emspi_cmd_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&ble_hci_emspi_evt_hi_pool,
                         MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                         MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                         ble_hci_emspi_evt_hi_buf,
                         "ble_hci_emspi_evt_hi_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&ble_hci_emspi_evt_lo_pool,
                         MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                         MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                         ble_hci_emspi_evt_lo_buf,
                         "ble_hci_emspi_evt_lo_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    /*
     * Create memory pool of packet list nodes. NOTE: the number of these
     * buffers should be, at least, the total number of event buffers (hi
     * and lo), the number of command buffers (currently 1) and the total
     * number of buffers that the controller could possibly hand to the host.
     */
    rc = os_mempool_init(&ble_hci_emspi_pkt_pool,
                         BLE_HCI_EMSPI_PKT_EVT_COUNT + 1 +
                         MYNEWT_VAL(BLE_HCI_ACL_OUT_COUNT),
                         sizeof (struct ble_hci_emspi_pkt),
                         ble_hci_emspi_pkt_buf,
                         "ble_hci_emspi_pkt_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    STAILQ_INIT(&ble_hci_emspi_tx_q);

    ble_hci_emspi_init_hw();

    /* Initialize the LL task */
    os_eventq_init(&ble_hci_emspi_evq);
    rc = os_task_init(&ble_hci_emspi_task, "ble_hci_emspi", ble_hci_emspi_loop,
                      NULL, MYNEWT_VAL(BLE_HCI_EMSPI_PRIO), OS_WAIT_FOREVER,
                      ble_hci_emspi_stack,
                      MYNEWT_VAL(BLE_HCI_EMSPI_STACK_SIZE));
    SYSINIT_PANIC_ASSERT(rc == 0);
}
