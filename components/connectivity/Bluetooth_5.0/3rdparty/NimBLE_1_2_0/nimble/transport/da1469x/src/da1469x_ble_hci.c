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
#include "os/mynewt.h"
#include "nimble/ble.h"
#include "nimble/ble_hci_trans.h"
#include "nimble/hci_common.h"
#include "mcu/da1469x_cmac.h"

#define HCI_PKT_NONE    0x00
#define HCI_PKT_CMD     0x01
#define HCI_PKT_ACL     0x02
#define HCI_PKT_EVT     0x04

#define POOL_ACL_BLOCK_SIZE                                             \
                        OS_ALIGN(MYNEWT_VAL(BLE_ACL_BUF_SIZE) +         \
                                BLE_MBUF_MEMBLOCK_OVERHEAD +            \
                                BLE_HCI_DATA_HDR_SZ, OS_ALIGNMENT)

struct da1469x_ble_hci_host_api {
    ble_hci_trans_rx_cmd_fn *evt_cb;
    void *evt_arg;
    ble_hci_trans_rx_acl_fn *acl_cb;
    void *acl_arg;
};

struct da1469x_ble_hci_rx_data {
    uint8_t type;
    uint8_t hdr[4];
    uint8_t min_len;
    uint16_t len;
    uint16_t expected_len;
    union {
        uint8_t *buf;
        struct os_mbuf *om;
    };
};

struct da1469x_ble_hci_pool_cmd {
    uint8_t cmd[BLE_HCI_TRANS_CMD_SZ];
    bool allocated;
};

/* (Pseudo)pool for HCI commands */
static struct da1469x_ble_hci_pool_cmd da1469x_ble_hci_pool_cmd;

/* Pools for HCI events (high and low priority) */
static uint8_t da1469x_ble_hci_pool_evt_hi_buf[ OS_MEMPOOL_BYTES(
                                            MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                                            MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE)) ];
static struct os_mempool da1469x_ble_hci_pool_evt_hi;
static uint8_t da1469x_ble_hci_pool_evt_lo_buf[ OS_MEMPOOL_BYTES(
                                            MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                                            MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE)) ];
static struct os_mempool da1469x_ble_hci_pool_evt_lo;

/* Pool for ACL data */
static uint8_t da1469x_ble_hci_pool_acl_buf[ OS_MEMPOOL_BYTES(
                                            MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                                            POOL_ACL_BLOCK_SIZE) ];
static struct os_mempool da1469x_ble_hci_pool_acl;
static struct os_mbuf_pool da1469x_ble_hci_pool_acl_mbuf;

/* Interface to host */
static struct da1469x_ble_hci_host_api da1469x_ble_hci_host_api;

/* State of RX currently in progress (needs to reassemble frame) */
static struct da1469x_ble_hci_rx_data da1469x_ble_hci_rx_data;

int
ble_hci_trans_reset(void)
{
    /* XXX Should we do something with RF and/or BLE core? */
    return 0;
}

void
ble_hci_trans_cfg_hs(ble_hci_trans_rx_cmd_fn *evt_cb, void *evt_arg,
                     ble_hci_trans_rx_acl_fn *acl_cb, void *acl_arg)
{
    da1469x_ble_hci_host_api.evt_cb = evt_cb;
    da1469x_ble_hci_host_api.evt_arg = evt_arg;
    da1469x_ble_hci_host_api.acl_cb = acl_cb;
    da1469x_ble_hci_host_api.acl_arg = acl_arg;
}

uint8_t *
ble_hci_trans_buf_alloc(int type)
{
    uint8_t *buf;

    switch (type) {
    case BLE_HCI_TRANS_BUF_CMD:
        assert(!da1469x_ble_hci_pool_cmd.allocated);
        da1469x_ble_hci_pool_cmd.allocated = 1;
        buf = da1469x_ble_hci_pool_cmd.cmd;
        break;
    case BLE_HCI_TRANS_BUF_EVT_HI:
        buf = os_memblock_get(&da1469x_ble_hci_pool_evt_hi);
        if (buf) {
            break;
        }
        /* no break */
    case BLE_HCI_TRANS_BUF_EVT_LO:
        buf = os_memblock_get(&da1469x_ble_hci_pool_evt_lo);
        break;
    default:
        assert(0);
        buf = NULL;
    }

    return buf;
}

void
ble_hci_trans_buf_free(uint8_t *buf)
{
    int rc;

    if (buf == da1469x_ble_hci_pool_cmd.cmd) {
        assert(da1469x_ble_hci_pool_cmd.allocated);
        da1469x_ble_hci_pool_cmd.allocated = 0;
    } else if (os_memblock_from(&da1469x_ble_hci_pool_evt_hi, buf)) {
        rc = os_memblock_put(&da1469x_ble_hci_pool_evt_hi, buf);
        assert(rc == 0);
    } else {
        assert(os_memblock_from(&da1469x_ble_hci_pool_evt_lo, buf));
        rc = os_memblock_put(&da1469x_ble_hci_pool_evt_lo, buf);
        assert(rc == 0);
    }
}

int
ble_hci_trans_hs_cmd_tx(uint8_t *cmd)
{
    uint8_t ind = HCI_PKT_CMD;
    int len = 3 + cmd[2];

    da1469x_cmac_mbox_write(&ind, 1);
    da1469x_cmac_mbox_write(cmd, len);

    ble_hci_trans_buf_free(cmd);

    return 0;
}

int
ble_hci_trans_hs_acl_tx(struct os_mbuf *om)
{
    uint8_t ind = HCI_PKT_ACL;
    struct os_mbuf *x;

    da1469x_cmac_mbox_write(&ind, 1);

    x = om;
    while (x) {
        da1469x_cmac_mbox_write(x->om_data, x->om_len);
        x = SLIST_NEXT(x, om_next);
    }

    os_mbuf_free_chain(om);

    return 0;
}

static int
da1469x_ble_hci_trans_ll_rx(const uint8_t *buf, uint16_t len)
{
    struct da1469x_ble_hci_rx_data *rxd = &da1469x_ble_hci_rx_data;
    void *data;
    int pool = BLE_HCI_TRANS_BUF_EVT_HI;
    int rc;

    assert(len);

    if (rxd->type == HCI_PKT_NONE) {
        rxd->type = buf[0];
        rxd->len = 0;
        rxd->expected_len = 0;

        switch (rxd->type) {
        case HCI_PKT_ACL:
            rxd->min_len = 4;
            break;
        case HCI_PKT_EVT:
            rxd->min_len = 2;
            break;
        default:
            assert(0);
            break;
        }

        return 1;
    }

    /* Ensure we have minimum length of bytes required to process header */
    if (rxd->len < rxd->min_len) {
        len = min(len, rxd->min_len - rxd->len);
        memcpy(&rxd->hdr[rxd->len], buf, len);
        rxd->len += len;
        return len;
    }

    /* Parse header and allocate proper buffer if not done yet */
    if (rxd->expected_len == 0) {
        switch (rxd->type) {
        case HCI_PKT_ACL:
            data = os_mbuf_get_pkthdr(&da1469x_ble_hci_pool_acl_mbuf,
                                      sizeof(struct ble_mbuf_hdr));
            if (!data) {
                return 0;
            }

            rxd->om = data;
            os_mbuf_append(rxd->om, rxd->hdr, rxd->len);
            rxd->expected_len = get_le16(&rxd->hdr[2]) + 4;
            break;
        case HCI_PKT_EVT:
            if (rxd->hdr[0] == BLE_HCI_EVCODE_LE_META) {
                /* For LE Meta event we need 3 bytes to parse header */
                if (rxd->min_len < 3) {
                    rxd->min_len = 3;
                    return 0;
                }

                /* Advertising reports shall be allocated from low-prio pool */
                if ((rxd->hdr[2] == BLE_HCI_LE_SUBEV_ADV_RPT) ||
                    (rxd->hdr[2] == BLE_HCI_LE_SUBEV_EXT_ADV_RPT)) {
                    pool = BLE_HCI_TRANS_BUF_EVT_LO;
                }
            }

            data = ble_hci_trans_buf_alloc(pool);
            if (!data) {
                /*
                 * Only care about valid buffer when shall be allocated from
                 * high-prio pool, otherwise NULL is fine and we'll just skip
                 * this event.
                 */
                if (pool != BLE_HCI_TRANS_BUF_EVT_LO) {
                    data = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_LO);
                    if (!data) {
                        return 0;
                    }
                }
            }

            rxd->buf = data;
            memcpy(rxd->buf, rxd->hdr, rxd->len);
            rxd->expected_len = rxd->hdr[1] + 2;
            break;
        default:
            assert(0);
            return len;
        }
    }

    len = min(len, rxd->expected_len - rxd->len);

    switch (rxd->type) {
    case HCI_PKT_ACL:
        os_mbuf_append(rxd->om, buf, len);
        rxd->len += len;

        if (rxd->len == rxd->expected_len) {
            rc = da1469x_ble_hci_host_api.acl_cb(rxd->om,
                                                 da1469x_ble_hci_host_api.acl_arg);
            if (rc != 0) {
                os_mbuf_free_chain(rxd->om);
            }
            rxd->type = HCI_PKT_NONE;
        }
        break;
    case HCI_PKT_EVT:
        if (rxd->buf) {
            memcpy(&rxd->buf[rxd->len], buf, len);
        }
        rxd->len += len;

        if (rxd->len == rxd->expected_len) {
            /*
             * XXX for unknown reason at startup controller sends command
             *     complete for a vendor specific command which we never sent
             *     and this messes up with our ack code - just discard this
             *     event
             */
            if ((rxd->buf[0] == 0x0E) && (get_le16(&rxd->buf[3]) == 0xfc11)) {
                ble_hci_trans_buf_free(rxd->buf);
            } else if (rxd->buf) {
                rc = da1469x_ble_hci_host_api.evt_cb(rxd->buf,
                                                     da1469x_ble_hci_host_api.evt_arg);
                if (rc != 0) {
                    ble_hci_trans_buf_free(rxd->buf);
                }
            }
            rxd->type = HCI_PKT_NONE;
        }
        break;
    default:
        assert(0);
        break;
    }

    return len;
}

static int
da1469x_ble_hci_read_cb(const uint8_t *buf, uint16_t len)
{
    return da1469x_ble_hci_trans_ll_rx(buf, len);
}

void
da1469x_ble_hci_init(void)
{
    int rc;

    SYSINIT_ASSERT_ACTIVE();

    rc = os_mempool_init(&da1469x_ble_hci_pool_acl, MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                         POOL_ACL_BLOCK_SIZE, da1469x_ble_hci_pool_acl_buf,
                         "da1469x_ble_hci_pool_acl");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mbuf_pool_init(&da1469x_ble_hci_pool_acl_mbuf,
                           &da1469x_ble_hci_pool_acl, POOL_ACL_BLOCK_SIZE,
                           MYNEWT_VAL(BLE_ACL_BUF_COUNT));
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&da1469x_ble_hci_pool_evt_hi,
                         MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                         MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                         da1469x_ble_hci_pool_evt_hi_buf,
                         "da1469x_ble_hci_pool_evt_hi");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&da1469x_ble_hci_pool_evt_lo,
                         MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                         MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                         da1469x_ble_hci_pool_evt_lo_buf,
                         "da1469x_ble_hci_pool_evt_lo");
    SYSINIT_PANIC_ASSERT(rc == 0);
}

void
da1469x_ble_hci_cmac_init(void)
{
    da1469x_cmac_mbox_set_read_cb(da1469x_ble_hci_read_cb);
    da1469x_cmac_init();
}
