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
#include <string.h>
#include <stdlib.h>

#include "sysinit/sysinit.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "bleuart/bleuart.h"
#include "os/endian.h"
#include "console/console.h"

/* ble uart attr read handle */
uint16_t g_bleuart_attr_read_handle;

/* ble uart attr write handle */
uint16_t g_bleuart_attr_write_handle;

/* Pointer to a console buffer */
char *console_buf;

uint16_t g_console_conn_handle;
/**
 * The vendor specific "bleuart" service consists of one write no-rsp characteristic
 * and one notification only read charateristic
 *     o "write no-rsp": a single-byte characteristic that can be written only
 *       over a non-encrypted connection
 *     o "read": a single-byte characteristic that can always be read only via
 *       notifications
 */

/* {6E400001-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_svc_uart_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e);

/* {6E400002-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_chr_uart_write_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e);


/* {6E400003-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_chr_uart_read_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e);

static int
gatt_svr_chr_access_uart_write(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Service: uart */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uart_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = &gatt_svr_chr_uart_read_uuid.u,
            .val_handle = &g_bleuart_attr_read_handle,
            .access_cb = gatt_svr_chr_access_uart_write,
            .flags = BLE_GATT_CHR_F_NOTIFY,
        }, {
            /* Characteristic: Write */
            .uuid = &gatt_svr_chr_uart_write_uuid.u,
            .access_cb = gatt_svr_chr_access_uart_write,
            .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
            .val_handle = &g_bleuart_attr_write_handle,
        }, {
            0, /* No more characteristics in this service */
        } },
    },

    {
        0, /* No more services */
    },
};

static int
gatt_svr_chr_access_uart_write(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    struct os_mbuf *om = ctxt->om;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
              while(om) {
                  console_write((char *)om->om_data, om->om_len);
                  om = SLIST_NEXT(om, om_next);
              }
              console_write("\n", 1);
              return 0;
        default:
            assert(0);
            return BLE_ATT_ERR_UNLIKELY;
    }
}

/**
 * bleuart GATT server initialization
 *
 * @param eventq
 * @return 0 on success; non-zero on failure
 */
int
bleuart_gatt_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        goto err;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

err:
    return rc;
}

/**
 * Reads console and sends data over BLE
 */
static void
bleuart_uart_read(void)
{
    int rc;
    int off;
    int full_line;
    struct os_mbuf *om;

    off = 0;
    while (1) {
        rc = console_read(console_buf + off,
                          MYNEWT_VAL(BLEUART_MAX_INPUT) - off, &full_line);
        if (rc <= 0 && !full_line) {
            continue;
        }
        off += rc;
        if (!full_line) {
            continue;
        }

        om = ble_hs_mbuf_from_flat(console_buf, off);
        if (!om) {
            return;
        }
        ble_gattc_notify_custom(g_console_conn_handle,
                                g_bleuart_attr_read_handle, om);
        off = 0;
        break;
    }
}

/**
 * Sets the global connection handle
 *
 * @param connection handle
 */
void
bleuart_set_conn_handle(uint16_t conn_handle) {
    g_console_conn_handle = conn_handle;
}

/**
 * BLEuart console initialization
 *
 * @param Maximum input
 */
void
bleuart_init(void)
{
    int rc;

    /* Ensure this function only gets called by sysinit. */
    SYSINIT_ASSERT_ACTIVE();

    rc = console_init(bleuart_uart_read);
    SYSINIT_PANIC_ASSERT(rc == 0);

    console_buf = malloc(MYNEWT_VAL(BLEUART_MAX_INPUT));
    SYSINIT_PANIC_ASSERT(console_buf != NULL);
}
