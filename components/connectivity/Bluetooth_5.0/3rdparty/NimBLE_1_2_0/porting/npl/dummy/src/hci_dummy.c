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
#include <stdio.h>
#include <stdint.h>
#include "syscfg/syscfg.h"
#include "sysinit/sysinit.h"
#include "os/os_mempool.h"
#include "nimble/ble.h"
#include "nimble/ble_hci_trans.h"
#include "nimble/hci_common.h"

/* HCI packet types */
#define HCI_PKT_CMD     0x01
#define HCI_PKT_ACL     0x02
#define HCI_PKT_EVT     0x04
#define HCI_PKT_GTL     0x05

/* Buffers for HCI commands data */
static uint8_t trans_buf_cmd[BLE_HCI_TRANS_CMD_SZ];
static uint8_t trans_buf_cmd_allocd;

/* Buffers for HCI events data */
static uint8_t trans_buf_evt_hi_pool_buf[ OS_MEMPOOL_BYTES(
                                            MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                                            MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE)) ];
static struct os_mempool trans_buf_evt_hi_pool;
static uint8_t trans_buf_evt_lo_pool_buf[ OS_MEMPOOL_BYTES(
                                            MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                                            MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE)) ];
static struct os_mempool trans_buf_evt_lo_pool;

/* Buffers for HCI ACL data */
#define ACL_POOL_BLOCK_SIZE OS_ALIGN(MYNEWT_VAL(BLE_ACL_BUF_SIZE) + \
                                            BLE_MBUF_MEMBLOCK_OVERHEAD + \
                                            BLE_HCI_DATA_HDR_SZ, OS_ALIGNMENT)
static uint8_t trans_buf_acl_pool_buf[ OS_MEMPOOL_BYTES(
                                            MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                                            ACL_POOL_BLOCK_SIZE) ];
static struct os_mempool trans_buf_acl_pool;
static struct os_mbuf_pool trans_buf_acl_mbuf_pool;

/* Host interface */
static ble_hci_trans_rx_cmd_fn *trans_rx_cmd_cb;
static void *trans_rx_cmd_arg;
static ble_hci_trans_rx_acl_fn *trans_rx_acl_cb;
static void *trans_rx_acl_arg;

/* Called by NimBLE host to reset HCI transport state (i.e. on host reset) */
int
ble_hci_trans_reset(void)
{
    return 0;
}

/* Called by NimBLE host to setup callbacks from HCI transport */
void
ble_hci_trans_cfg_hs(ble_hci_trans_rx_cmd_fn *cmd_cb, void *cmd_arg,
                     ble_hci_trans_rx_acl_fn *acl_cb, void *acl_arg)
{
    trans_rx_cmd_cb = cmd_cb;
    trans_rx_cmd_arg = cmd_arg;
    trans_rx_acl_cb = acl_cb;
    trans_rx_acl_arg = acl_arg;
}

/*
 * Called by NimBLE host to allocate buffer for HCI Command packet.
 * Called by HCI transport to allocate buffer for HCI Event packet.
 */
uint8_t *
ble_hci_trans_buf_alloc(int type)
{
    uint8_t *buf;

    switch (type) {
    case BLE_HCI_TRANS_BUF_CMD:
        assert(!trans_buf_cmd_allocd);
        trans_buf_cmd_allocd = 1;
        buf = trans_buf_cmd;
        break;
    case BLE_HCI_TRANS_BUF_EVT_HI:
        buf = os_memblock_get(&trans_buf_evt_hi_pool);
        if (buf) {
            break;
        }
        /* no break */
    case BLE_HCI_TRANS_BUF_EVT_LO:
        buf = os_memblock_get(&trans_buf_evt_lo_pool);
        break;
    default:
        assert(0);
        buf = NULL;
    }

    return buf;
}

/*
 * Called by NimBLE host to free buffer allocated for HCI Event packet.
 * Called by HCI transport to free buffer allocated for HCI Command packet.
 */
void
ble_hci_trans_buf_free(uint8_t *buf)
{
    int rc;

    if (buf == trans_buf_cmd) {
        assert(trans_buf_cmd_allocd);
        trans_buf_cmd_allocd = 0;
    } else if (os_memblock_from(&trans_buf_evt_hi_pool, buf)) {
        rc = os_memblock_put(&trans_buf_evt_hi_pool, buf);
        assert(rc == 0);
    } else {
        assert(os_memblock_from(&trans_buf_evt_lo_pool, buf));
        rc = os_memblock_put(&trans_buf_evt_lo_pool, buf);
        assert(rc == 0);
    }
}

/* Called by NimBLE host to send HCI Command packet over HCI transport */
int
ble_hci_trans_hs_cmd_tx(uint8_t *cmd)
{
    uint8_t *buf = cmd;

    /*
     * TODO Send HCI Command packet somewhere.
     * Buffer pointed by 'cmd' contains complete HCI Command packet as defined
     * by Core spec.
     */

    ble_hci_trans_buf_free(buf);

    return 0;
}

/* Called by NimBLE host to send HCI ACL Data packet over HCI transport */
int
ble_hci_trans_hs_acl_tx(struct os_mbuf *om)
{
    uint8_t *buf = om->om_data;

    /*
     * TODO Send HCI ACL Data packet somewhere.
     * mbuf pointed by 'om' contains complete HCI ACL Data packet as defined
     * by Core spec.
     */
    (void)buf;

    os_mbuf_free_chain(om);

    return 0;
}

/* Called by application to send HCI ACL Data packet to host */
int
hci_transport_send_acl_to_host(uint8_t *buf, uint16_t size)
{
    struct os_mbuf *trans_mbuf;
    int rc;

    trans_mbuf = os_mbuf_get_pkthdr(&trans_buf_acl_mbuf_pool,
                                    sizeof(struct ble_mbuf_hdr));
    os_mbuf_append(trans_mbuf, buf, size);
    rc = trans_rx_acl_cb(trans_mbuf, trans_rx_acl_arg);

    return rc;
}

/* Called by application to send HCI Event packet to host */
int
hci_transport_send_evt_to_host(uint8_t *buf, uint8_t size)
{
    uint8_t *trans_buf;
    int rc;

    /* Allocate LE Advertising Report Event from lo pool only */
    if ((buf[0] == BLE_HCI_EVCODE_LE_META) &&
        (buf[2] == BLE_HCI_LE_SUBEV_ADV_RPT)) {
        trans_buf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_LO);
        if (!trans_buf) {
            /* Skip advertising report if we're out of memory */
            return 0;
        }
    } else {
        trans_buf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
    }

    memcpy(trans_buf, buf, size);

    rc = trans_rx_cmd_cb(trans_buf, trans_rx_cmd_arg);
    if (rc != 0) {
        ble_hci_trans_buf_free(trans_buf);
    }

    return rc;
}

/* Called by application to initialize transport structures */
int
hci_transport_init(void)
{
    int rc;

    trans_buf_cmd_allocd = 0;

    rc = os_mempool_init(&trans_buf_acl_pool, MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                                ACL_POOL_BLOCK_SIZE, trans_buf_acl_pool_buf,
                                "dummy_hci_acl_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mbuf_pool_init(&trans_buf_acl_mbuf_pool, &trans_buf_acl_pool,
                                ACL_POOL_BLOCK_SIZE,
                                MYNEWT_VAL(BLE_ACL_BUF_COUNT));
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&trans_buf_evt_hi_pool,
                                MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                                MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                                trans_buf_evt_hi_pool_buf,
                                "dummy_hci_hci_evt_hi_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&trans_buf_evt_lo_pool,
                                MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                                MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                                trans_buf_evt_lo_pool_buf,
                                "dummy_hci_hci_evt_lo_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    return 0;
}
