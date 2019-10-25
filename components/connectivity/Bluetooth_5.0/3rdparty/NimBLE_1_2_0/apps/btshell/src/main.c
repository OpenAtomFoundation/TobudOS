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
#include "os/mynewt.h"
#include "bsp/bsp.h"
#include "log/log.h"
#include "stats/stats.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"
#include "console/console.h"
#include "btshell.h"
#include "cmd.h"

/* BLE */
#include "nimble/ble.h"
#include "nimble/nimble_opt.h"
#include "nimble/ble_hci_trans.h"
#include "host/ble_hs.h"
#include "host/ble_hs_adv.h"
#include "host/ble_uuid.h"
#include "host/ble_att.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_store.h"
#include "host/ble_sm.h"

/* Mandatory services. */
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

/* XXX: An app should not include private headers from a library.  The btshell
 * app uses some of nimble's internal details for logging.
 */
#include "../src/ble_hs_conn_priv.h"
#include "../src/ble_hs_atomic_priv.h"
#include "../src/ble_hs_priv.h"

#if MYNEWT_VAL(BLE_ROLE_CENTRAL)
#define BTSHELL_MAX_SVCS               32
#define BTSHELL_MAX_CHRS               64
#define BTSHELL_MAX_DSCS               64
#else
#define BTSHELL_MAX_SVCS               1
#define BTSHELL_MAX_CHRS               1
#define BTSHELL_MAX_DSCS               1
#endif

#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM)
#define BTSHELL_COC_MTU               (256)
/* We use same pool for incoming and outgoing sdu */
#define BTSHELL_COC_BUF_COUNT         (3 * MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM))

#define INT_TO_PTR(x)     (void *)((intptr_t)(x))
#define PTR_TO_INT(x)     (int) ((intptr_t)(x))
#endif

bssnz_t struct btshell_conn btshell_conns[MYNEWT_VAL(BLE_MAX_CONNECTIONS)];
int btshell_num_conns;

static os_membuf_t btshell_svc_mem[
    OS_MEMPOOL_SIZE(BTSHELL_MAX_SVCS, sizeof(struct btshell_svc))
];
static struct os_mempool btshell_svc_pool;

static os_membuf_t btshell_chr_mem[
    OS_MEMPOOL_SIZE(BTSHELL_MAX_CHRS, sizeof(struct btshell_chr))
];
static struct os_mempool btshell_chr_pool;

static os_membuf_t btshell_dsc_mem[
    OS_MEMPOOL_SIZE(BTSHELL_MAX_DSCS, sizeof(struct btshell_dsc))
];
static struct os_mempool btshell_dsc_pool;

#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM)
static os_membuf_t btshell_coc_conn_mem[
    OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM),
    sizeof(struct btshell_l2cap_coc))
];
static struct os_mempool btshell_coc_conn_pool;

static os_membuf_t btshell_sdu_coc_mem[
    OS_MEMPOOL_SIZE(BTSHELL_COC_BUF_COUNT, BTSHELL_COC_MTU)
];
struct os_mbuf_pool sdu_os_mbuf_pool;
static struct os_mempool sdu_coc_mbuf_mempool;
#endif

static struct os_callout btshell_tx_timer;
struct btshell_tx_data_s
{
    uint16_t tx_num;
    uint16_t tx_num_requested;
    uint16_t tx_rate;
    uint16_t tx_conn_handle;
    uint16_t tx_len;
    struct ble_hs_conn *conn;
};
static struct btshell_tx_data_s btshell_tx_data;
int btshell_full_disc_prev_chr_val;

#define XSTR(s) STR(s)
#ifndef STR
#define STR(s) #s
#endif


#ifdef DEVICE_NAME
#define BTSHELL_AUTO_DEVICE_NAME    XSTR(DEVICE_NAME)
#else
#define BTSHELL_AUTO_DEVICE_NAME    ""
#endif

#if MYNEWT_VAL(BLE_EXT_ADV)
struct {
    bool restart;
    uint16_t conn_handle;
} ext_adv_restart[BLE_ADV_INSTANCES];
#endif

static struct {
    bool restart;
    uint8_t own_addr_type;
    ble_addr_t direct_addr;
    int32_t duration_ms;
    struct ble_gap_adv_params params;
} adv_params;

static void
btshell_print_error(char *msg, uint16_t conn_handle,
                    const struct ble_gatt_error *error)
{
    if (msg == NULL) {
        msg = "ERROR";
    }

    console_printf("%s: conn_handle=%d status=%d att_handle=%d\n",
                   msg, conn_handle, error->status, error->att_handle);
}

static void
btshell_print_adv_fields(const struct ble_hs_adv_fields *fields)
{
    uint8_t *u8p;
    int i;

    if (fields->flags != 0) {
        console_printf("    flags=0x%02x:\n", fields->flags);

        if (!(fields->flags & BLE_HS_ADV_F_DISC_LTD) &&
                !(fields->flags & BLE_HS_ADV_F_DISC_GEN)) {
                console_printf("        Non-discoverable mode\n");
        }

        if (fields->flags & BLE_HS_ADV_F_DISC_LTD) {
                console_printf("        Limited discoverable mode\n");
        }

        if (fields->flags & BLE_HS_ADV_F_DISC_GEN) {
                console_printf("        General discoverable mode\n");
        }

        if (fields->flags & BLE_HS_ADV_F_BREDR_UNSUP) {
                console_printf("        BR/EDR not supported\n");
        }
    }

    if (fields->uuids16 != NULL) {
        console_printf("    uuids16(%scomplete)=",
                       fields->uuids16_is_complete ? "" : "in");
        for (i = 0; i < fields->num_uuids16; i++) {
            print_uuid(&fields->uuids16[i].u);
            console_printf(" ");
        }
        console_printf("\n");
    }

    if (fields->uuids32 != NULL) {
        console_printf("    uuids32(%scomplete)=",
                       fields->uuids32_is_complete ? "" : "in");
        for (i = 0; i < fields->num_uuids32; i++) {
            print_uuid(&fields->uuids32[i].u);
            console_printf(" ");
        }
        console_printf("\n");
    }

    if (fields->uuids128 != NULL) {
        console_printf("    uuids128(%scomplete)=",
                       fields->uuids128_is_complete ? "" : "in");
        for (i = 0; i < fields->num_uuids128; i++) {
            print_uuid(&fields->uuids128[i].u);
            console_printf(" ");
        }
        console_printf("\n");
    }

    if (fields->name != NULL) {
        console_printf("    name(%scomplete)=",
                       fields->name_is_complete ? "" : "in");
        console_write((char *)fields->name, fields->name_len);
        console_printf("\n");
    }

    if (fields->tx_pwr_lvl_is_present) {
        console_printf("    tx_pwr_lvl=%d\n", fields->tx_pwr_lvl);
    }

    if (fields->slave_itvl_range != NULL) {
        console_printf("    slave_itvl_range=");
        print_bytes(fields->slave_itvl_range,
                            BLE_HS_ADV_SLAVE_ITVL_RANGE_LEN);
        console_printf("\n");
    }

    if (fields->svc_data_uuid16 != NULL) {
        console_printf("    svc_data_uuid16=");
        print_bytes(fields->svc_data_uuid16,
                            fields->svc_data_uuid16_len);
        console_printf("\n");
    }

    if (fields->public_tgt_addr != NULL) {
        console_printf("    public_tgt_addr=");
        u8p = fields->public_tgt_addr;
        for (i = 0; i < fields->num_public_tgt_addrs; i++) {
            print_addr(u8p);
            u8p += BLE_HS_ADV_PUBLIC_TGT_ADDR_ENTRY_LEN;
        }
        console_printf("\n");
    }

    if (fields->appearance_is_present) {
        console_printf("    appearance=0x%04x\n", fields->appearance);
    }

    if (fields->adv_itvl_is_present) {
        console_printf("    adv_itvl=0x%04x\n", fields->adv_itvl);
    }

    if (fields->svc_data_uuid32 != NULL) {
        console_printf("    svc_data_uuid32=");
        print_bytes(fields->svc_data_uuid32,
                             fields->svc_data_uuid32_len);
        console_printf("\n");
    }

    if (fields->svc_data_uuid128 != NULL) {
        console_printf("    svc_data_uuid128=");
        print_bytes(fields->svc_data_uuid128,
                            fields->svc_data_uuid128_len);
        console_printf("\n");
    }

    if (fields->uri != NULL) {
        console_printf("    uri=");
        print_bytes(fields->uri, fields->uri_len);
        console_printf("\n");
    }

    if (fields->mfg_data != NULL) {
        console_printf("    mfg_data=");
        print_bytes(fields->mfg_data, fields->mfg_data_len);
        console_printf("\n");
    }
}

static int
btshell_conn_find_idx(uint16_t handle)
{
    int i;

    for (i = 0; i < btshell_num_conns; i++) {
        if (btshell_conns[i].handle == handle) {
            return i;
        }
    }

    return -1;
}

static struct btshell_conn *
btshell_conn_find(uint16_t handle)
{
    int idx;

    idx = btshell_conn_find_idx(handle);
    if (idx == -1) {
        return NULL;
    } else {
        return btshell_conns + idx;
    }
}

static struct btshell_svc *
btshell_svc_find_prev(struct btshell_conn *conn, uint16_t svc_start_handle)
{
    struct btshell_svc *prev;
    struct btshell_svc *svc;

    prev = NULL;
    SLIST_FOREACH(svc, &conn->svcs, next) {
        if (svc->svc.start_handle >= svc_start_handle) {
            break;
        }

        prev = svc;
    }

    return prev;
}

static struct btshell_svc *
btshell_svc_find(struct btshell_conn *conn, uint16_t svc_start_handle,
                 struct btshell_svc **out_prev)
{
    struct btshell_svc *prev;
    struct btshell_svc *svc;

    prev = btshell_svc_find_prev(conn, svc_start_handle);
    if (prev == NULL) {
        svc = SLIST_FIRST(&conn->svcs);
    } else {
        svc = SLIST_NEXT(prev, next);
    }

    if (svc != NULL && svc->svc.start_handle != svc_start_handle) {
        svc = NULL;
    }

    if (out_prev != NULL) {
        *out_prev = prev;
    }
    return svc;
}

static struct btshell_svc *
btshell_svc_find_range(struct btshell_conn *conn, uint16_t attr_handle)
{
    struct btshell_svc *svc;

    SLIST_FOREACH(svc, &conn->svcs, next) {
        if (svc->svc.start_handle <= attr_handle &&
            svc->svc.end_handle >= attr_handle) {

            return svc;
        }
    }

    return NULL;
}

static void
btshell_chr_delete(struct btshell_chr *chr)
{
    struct btshell_dsc *dsc;

    while ((dsc = SLIST_FIRST(&chr->dscs)) != NULL) {
        SLIST_REMOVE_HEAD(&chr->dscs, next);
        os_memblock_put(&btshell_dsc_pool, dsc);
    }

    os_memblock_put(&btshell_chr_pool, chr);
}

static void
btshell_svc_delete(struct btshell_svc *svc)
{
    struct btshell_chr *chr;

    while ((chr = SLIST_FIRST(&svc->chrs)) != NULL) {
        SLIST_REMOVE_HEAD(&svc->chrs, next);
        btshell_chr_delete(chr);
    }

    os_memblock_put(&btshell_svc_pool, svc);
}

static struct btshell_svc *
btshell_svc_add(uint16_t conn_handle, const struct ble_gatt_svc *gatt_svc)
{
    struct btshell_conn *conn;
    struct btshell_svc *prev;
    struct btshell_svc *svc;

    conn = btshell_conn_find(conn_handle);
    if (conn == NULL) {
        MODLOG_DFLT(DEBUG, "RECEIVED SERVICE FOR UNKNOWN CONNECTION; "
                           "HANDLE=%d\n",
                    conn_handle);
        return NULL;
    }

    svc = btshell_svc_find(conn, gatt_svc->start_handle, &prev);
    if (svc != NULL) {
        /* Service already discovered. */
        return svc;
    }

    svc = os_memblock_get(&btshell_svc_pool);
    if (svc == NULL) {
        MODLOG_DFLT(DEBUG, "OOM WHILE DISCOVERING SERVICE\n");
        return NULL;
    }
    memset(svc, 0, sizeof *svc);

    svc->svc = *gatt_svc;
    SLIST_INIT(&svc->chrs);

    if (prev == NULL) {
        SLIST_INSERT_HEAD(&conn->svcs, svc, next);
    } else {
        SLIST_INSERT_AFTER(prev, svc, next);
    }

    return svc;
}

static struct btshell_chr *
btshell_chr_find_prev(const struct btshell_svc *svc, uint16_t chr_val_handle)
{
    struct btshell_chr *prev;
    struct btshell_chr *chr;

    prev = NULL;
    SLIST_FOREACH(chr, &svc->chrs, next) {
        if (chr->chr.val_handle >= chr_val_handle) {
            break;
        }

        prev = chr;
    }

    return prev;
}

static struct btshell_chr *
btshell_chr_find(const struct btshell_svc *svc, uint16_t chr_val_handle,
                 struct btshell_chr **out_prev)
{
    struct btshell_chr *prev;
    struct btshell_chr *chr;

    prev = btshell_chr_find_prev(svc, chr_val_handle);
    if (prev == NULL) {
        chr = SLIST_FIRST(&svc->chrs);
    } else {
        chr = SLIST_NEXT(prev, next);
    }

    if (chr != NULL && chr->chr.val_handle != chr_val_handle) {
        chr = NULL;
    }

    if (out_prev != NULL) {
        *out_prev = prev;
    }
    return chr;
}

static struct btshell_chr *
btshell_chr_add(uint16_t conn_handle,  uint16_t svc_start_handle,
                const struct ble_gatt_chr *gatt_chr)
{
    struct btshell_conn *conn;
    struct btshell_chr *prev;
    struct btshell_chr *chr;
    struct btshell_svc *svc;

    conn = btshell_conn_find(conn_handle);
    if (conn == NULL) {
        MODLOG_DFLT(DEBUG, "RECEIVED SERVICE FOR UNKNOWN CONNECTION; "
                           "HANDLE=%d\n",
                    conn_handle);
        return NULL;
    }

    svc = btshell_svc_find(conn, svc_start_handle, NULL);
    if (svc == NULL) {
        MODLOG_DFLT(DEBUG, "CAN'T FIND SERVICE FOR DISCOVERED CHR; HANDLE=%d\n",
                    conn_handle);
        return NULL;
    }

    chr = btshell_chr_find(svc, gatt_chr->val_handle, &prev);
    if (chr != NULL) {
        /* Characteristic already discovered. */
        return chr;
    }

    chr = os_memblock_get(&btshell_chr_pool);
    if (chr == NULL) {
        MODLOG_DFLT(DEBUG, "OOM WHILE DISCOVERING CHARACTERISTIC\n");
        return NULL;
    }
    memset(chr, 0, sizeof *chr);

    chr->chr = *gatt_chr;

    if (prev == NULL) {
        SLIST_INSERT_HEAD(&svc->chrs, chr, next);
    } else {
        SLIST_NEXT(prev, next) = chr;
    }

    return chr;
}

static struct btshell_dsc *
btshell_dsc_find_prev(const struct btshell_chr *chr, uint16_t dsc_handle)
{
    struct btshell_dsc *prev;
    struct btshell_dsc *dsc;

    prev = NULL;
    SLIST_FOREACH(dsc, &chr->dscs, next) {
        if (dsc->dsc.handle >= dsc_handle) {
            break;
        }

        prev = dsc;
    }

    return prev;
}

static struct btshell_dsc *
btshell_dsc_find(const struct btshell_chr *chr, uint16_t dsc_handle,
                 struct btshell_dsc **out_prev)
{
    struct btshell_dsc *prev;
    struct btshell_dsc *dsc;

    prev = btshell_dsc_find_prev(chr, dsc_handle);
    if (prev == NULL) {
        dsc = SLIST_FIRST(&chr->dscs);
    } else {
        dsc = SLIST_NEXT(prev, next);
    }

    if (dsc != NULL && dsc->dsc.handle != dsc_handle) {
        dsc = NULL;
    }

    if (out_prev != NULL) {
        *out_prev = prev;
    }
    return dsc;
}

static struct btshell_dsc *
btshell_dsc_add(uint16_t conn_handle, uint16_t chr_val_handle,
                const struct ble_gatt_dsc *gatt_dsc)
{
    struct btshell_conn *conn;
    struct btshell_dsc *prev;
    struct btshell_dsc *dsc;
    struct btshell_svc *svc;
    struct btshell_chr *chr;

    conn = btshell_conn_find(conn_handle);
    if (conn == NULL) {
        MODLOG_DFLT(DEBUG, "RECEIVED SERVICE FOR UNKNOWN CONNECTION; "
                           "HANDLE=%d\n",
                    conn_handle);
        return NULL;
    }

    svc = btshell_svc_find_range(conn, chr_val_handle);
    if (svc == NULL) {
        MODLOG_DFLT(DEBUG, "CAN'T FIND SERVICE FOR DISCOVERED DSC; HANDLE=%d\n",
                    conn_handle);
        return NULL;
    }

    chr = btshell_chr_find(svc, chr_val_handle, NULL);
    if (chr == NULL) {
        MODLOG_DFLT(DEBUG, "CAN'T FIND CHARACTERISTIC FOR DISCOVERED DSC; "
                           "HANDLE=%d\n",
                    conn_handle);
        return NULL;
    }

    dsc = btshell_dsc_find(chr, gatt_dsc->handle, &prev);
    if (dsc != NULL) {
        /* Descriptor already discovered. */
        return dsc;
    }

    dsc = os_memblock_get(&btshell_dsc_pool);
    if (dsc == NULL) {
        console_printf("OOM WHILE DISCOVERING DESCRIPTOR\n");
        return NULL;
    }
    memset(dsc, 0, sizeof *dsc);

    dsc->dsc = *gatt_dsc;

    if (prev == NULL) {
        SLIST_INSERT_HEAD(&chr->dscs, dsc, next);
    } else {
        SLIST_NEXT(prev, next) = dsc;
    }

    return dsc;
}

static struct btshell_conn *
btshell_conn_add(struct ble_gap_conn_desc *desc)
{
    struct btshell_conn *conn;

    assert(btshell_num_conns < MYNEWT_VAL(BLE_MAX_CONNECTIONS));

    conn = btshell_conns + btshell_num_conns;
    btshell_num_conns++;

    conn->handle = desc->conn_handle;
    SLIST_INIT(&conn->svcs);
    SLIST_INIT(&conn->coc_list);

    return conn;
}

static void
btshell_conn_delete_idx(int idx)
{
    struct btshell_conn *conn;
    struct btshell_svc *svc;

    assert(idx >= 0 && idx < btshell_num_conns);

    conn = btshell_conns + idx;
    while ((svc = SLIST_FIRST(&conn->svcs)) != NULL) {
        SLIST_REMOVE_HEAD(&conn->svcs, next);
        btshell_svc_delete(svc);
    }

    /* This '#if' is not strictly necessary.  It is here to prevent a spurious
     * warning from being reported.
     */
#if MYNEWT_VAL(BLE_MAX_CONNECTIONS) > 1
    int i;
    for (i = idx + 1; i < btshell_num_conns; i++) {
        btshell_conns[i - 1] = btshell_conns[i];
    }
#endif

    btshell_num_conns--;
}

static int
btshell_on_mtu(uint16_t conn_handle, const struct ble_gatt_error *error,
               uint16_t mtu, void *arg)
{
    switch (error->status) {
    case 0:
        console_printf("mtu exchange complete: conn_handle=%d mtu=%d\n",
                       conn_handle, mtu);
        break;

    default:
        btshell_print_error(NULL, conn_handle, error);
        break;
    }

    return 0;
}

static void
btshell_full_disc_complete(int rc)
{
    console_printf("full discovery complete; rc=%d\n", rc);
    btshell_full_disc_prev_chr_val = 0;
}

static void
btshell_disc_full_dscs(uint16_t conn_handle)
{
    struct btshell_conn *conn;
    struct btshell_chr *chr;
    struct btshell_svc *svc;
    int rc;

    conn = btshell_conn_find(conn_handle);
    if (conn == NULL) {
        MODLOG_DFLT(DEBUG, "Failed to discover descriptors for conn=%d; "
                           "not connected\n", conn_handle);
        btshell_full_disc_complete(BLE_HS_ENOTCONN);
        return;
    }

    SLIST_FOREACH(svc, &conn->svcs, next) {
        SLIST_FOREACH(chr, &svc->chrs, next) {
            if (!chr_is_empty(svc, chr) &&
                SLIST_EMPTY(&chr->dscs) &&
                btshell_full_disc_prev_chr_val <= chr->chr.def_handle) {

                rc = btshell_disc_all_dscs(conn_handle,
                                           chr->chr.val_handle,
                                           chr_end_handle(svc, chr));
                if (rc != 0) {
                    btshell_full_disc_complete(rc);
                }

                btshell_full_disc_prev_chr_val = chr->chr.val_handle;
                return;
            }
        }
    }

    /* All descriptors discovered. */
    btshell_full_disc_complete(0);
}

static void
btshell_disc_full_chrs(uint16_t conn_handle)
{
    struct btshell_conn *conn;
    struct btshell_svc *svc;
    int rc;

    conn = btshell_conn_find(conn_handle);
    if (conn == NULL) {
        MODLOG_DFLT(DEBUG, "Failed to discover characteristics for conn=%d; "
                           "not connected\n", conn_handle);
        btshell_full_disc_complete(BLE_HS_ENOTCONN);
        return;
    }

    SLIST_FOREACH(svc, &conn->svcs, next) {
        if (!svc_is_empty(svc) && SLIST_EMPTY(&svc->chrs)) {
            rc = btshell_disc_all_chrs(conn_handle, svc->svc.start_handle,
                                       svc->svc.end_handle);
            if (rc != 0) {
                btshell_full_disc_complete(rc);
            }
            return;
        }
    }

    /* All characteristics discovered. */
    btshell_disc_full_dscs(conn_handle);
}

static int
btshell_on_disc_s(uint16_t conn_handle, const struct ble_gatt_error *error,
                  const struct ble_gatt_svc *service, void *arg)
{
    switch (error->status) {
    case 0:
        btshell_svc_add(conn_handle, service);
        break;

    case BLE_HS_EDONE:
        console_printf("service discovery successful\n");
        if (btshell_full_disc_prev_chr_val > 0) {
            btshell_disc_full_chrs(conn_handle);
        }
        break;

    default:
        btshell_print_error(NULL, conn_handle, error);
        break;
    }

    return 0;
}

static int
btshell_on_disc_c(uint16_t conn_handle, const struct ble_gatt_error *error,
                  const struct ble_gatt_chr *chr, void *arg)
{
    intptr_t svc_start_handle;

    svc_start_handle = (intptr_t)arg;

    switch (error->status) {
    case 0:
        btshell_chr_add(conn_handle, svc_start_handle, chr);
        break;

    case BLE_HS_EDONE:
        console_printf("characteristic discovery successful\n");
        if (btshell_full_disc_prev_chr_val > 0) {
            btshell_disc_full_chrs(conn_handle);
        }
        break;

    default:
        btshell_print_error(NULL, conn_handle, error);
        break;
    }

    return 0;
}

static int
btshell_on_disc_d(uint16_t conn_handle, const struct ble_gatt_error *error,
                  uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                  void *arg)
{
    switch (error->status) {
    case 0:
        btshell_dsc_add(conn_handle, chr_val_handle, dsc);
        break;

    case BLE_HS_EDONE:
        console_printf("descriptor discovery successful\n");
        if (btshell_full_disc_prev_chr_val > 0) {
            btshell_disc_full_dscs(conn_handle);
        }
        break;

    default:
        btshell_print_error(NULL, conn_handle, error);
        break;
    }

    return 0;
}

static int
btshell_on_read(uint16_t conn_handle, const struct ble_gatt_error *error,
                struct ble_gatt_attr *attr, void *arg)
{
    switch (error->status) {
    case 0:
        console_printf("characteristic read; conn_handle=%d "
                       "attr_handle=%d len=%d value=", conn_handle,
                       attr->handle, OS_MBUF_PKTLEN(attr->om));
        print_mbuf(attr->om);
        console_printf("\n");
        break;

    case BLE_HS_EDONE:
        console_printf("characteristic read complete\n");
        break;

    default:
        btshell_print_error(NULL, conn_handle, error);
        break;
    }

    return 0;
}

static int
btshell_on_write(uint16_t conn_handle, const struct ble_gatt_error *error,
                 struct ble_gatt_attr *attr, void *arg)
{
    switch (error->status) {
    case 0:
        console_printf("characteristic write complete; conn_handle=%d "
                       "attr_handle=%d\n", conn_handle, attr->handle);
        break;

    default:
        btshell_print_error(NULL, conn_handle, error);
        break;
    }

    return 0;
}

static int
btshell_on_write_reliable(uint16_t conn_handle,
                          const struct ble_gatt_error *error,
                          struct ble_gatt_attr *attrs, uint8_t num_attrs,
                          void *arg)
{
    int i;

    switch (error->status) {
    case 0:
        console_printf("characteristic write reliable complete; "
                       "conn_handle=%d", conn_handle);

        for (i = 0; i < num_attrs; i++) {
            console_printf(" attr_handle=%d len=%d value=", attrs[i].handle,
                           OS_MBUF_PKTLEN(attrs[i].om));
            print_mbuf(attrs[i].om);
        }
        console_printf("\n");
        break;

    default:
        btshell_print_error(NULL, conn_handle, error);
        break;
    }

    return 0;
}

static void
btshell_decode_adv_data(uint8_t *adv_data, uint8_t adv_data_len, void *arg)
{
    struct btshell_scan_opts *scan_opts = arg;
    struct ble_hs_adv_fields fields;

    console_printf(" data_length=%d data=", adv_data_len);

    if (scan_opts) {
        adv_data_len = min(adv_data_len, scan_opts->limit);
    }

    print_bytes(adv_data, adv_data_len);

    console_printf(" fields:\n");
    ble_hs_adv_parse_fields(&fields, adv_data, adv_data_len);
    btshell_print_adv_fields(&fields);
}

#if MYNEWT_VAL(BLE_EXT_ADV)
static void
btshell_decode_event_type(struct ble_gap_ext_disc_desc *desc, void *arg)
{
    struct btshell_scan_opts *scan_opts = arg;
    uint8_t directed = 0;

    if (desc->props & BLE_HCI_ADV_LEGACY_MASK) {
        if (scan_opts && scan_opts->ignore_legacy) {
            return;
        }

        console_printf("Legacy PDU type %d", desc->legacy_event_type);
        if (desc->legacy_event_type == BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
            directed = 1;
        }
        goto common_data;
    } else {
        if (scan_opts && scan_opts->periodic_only && desc->periodic_adv_itvl == 0) {
            return;
        }
    }

    console_printf("Extended adv: ");
    if (desc->props & BLE_HCI_ADV_CONN_MASK) {
        console_printf("'conn' ");
    }
    if (desc->props & BLE_HCI_ADV_SCAN_MASK) {
        console_printf("'scan' ");
    }
    if (desc->props & BLE_HCI_ADV_DIRECT_MASK) {
        console_printf("'dir' ");
        directed = 1;
    }

    if (desc->props & BLE_HCI_ADV_SCAN_RSP_MASK) {
        console_printf("'scan rsp' ");
    }

    switch(desc->data_status) {
    case BLE_GAP_EXT_ADV_DATA_STATUS_COMPLETE:
        console_printf("complete");
        break;
    case BLE_GAP_EXT_ADV_DATA_STATUS_INCOMPLETE:
        console_printf("incomplete");
        break;
    case BLE_GAP_EXT_ADV_DATA_STATUS_TRUNCATED:
        console_printf("truncated");
        break;
    default:
        console_printf("reserved %d", desc->data_status);
        break;
    }

common_data:
    console_printf(" rssi=%d txpower=%d, pphy=%d, sphy=%d, sid=%d,"
                   " periodic_adv_itvl=%u, addr_type=%d addr=",
                   desc->rssi, desc->tx_power, desc->prim_phy, desc->sec_phy,
                   desc->sid, desc->periodic_adv_itvl, desc->addr.type);
    print_addr(desc->addr.val);
    if (directed) {
        console_printf(" init_addr_type=%d inita=", desc->direct_addr.type);
        print_addr(desc->direct_addr.val);
    }

    console_printf("\n");

    if(!desc->length_data) {
        return;
    }

    btshell_decode_adv_data(desc->data, desc->length_data, arg);
}
#endif

static int
btshell_restart_adv(struct ble_gap_event *event)
{
    int rc = 0;
#if MYNEWT_VAL(BLE_EXT_ADV)
    uint8_t i;
#endif

    if (event->type != BLE_GAP_EVENT_DISCONNECT) {
        return -1;
    }

#if MYNEWT_VAL(BLE_EXT_ADV)
    for (i = 0; i < BLE_ADV_INSTANCES; ++i) {
        if (ext_adv_restart[i].restart &&
            (ext_adv_restart[i].conn_handle ==
             event->disconnect.conn.conn_handle)) {
            rc = ble_gap_ext_adv_start(i, 0, 0);
            break;
        }
    }
#else
    if (!adv_params.restart) {
        return 0;
    }

    rc = ble_gap_adv_start(adv_params.own_addr_type, &adv_params.direct_addr,
                           adv_params.duration_ms, &adv_params.params,
                           btshell_gap_event, NULL);
#endif

    return rc;
}

#if MYNEWT_VAL(BLE_PERIODIC_ADV)
struct psync {
    bool established;
    unsigned int complete;
    unsigned int truncated;
    size_t off;
    bool changed;
    uint8_t data[1650]; /* TODO make this configurable */
};

static struct psync g_periodic_data[MYNEWT_VAL(BLE_MAX_PERIODIC_SYNCS)];

void
btshell_sync_stats(uint16_t handle)
{
    struct psync *psync;

    if (handle >= MYNEWT_VAL(BLE_MAX_PERIODIC_SYNCS)) {
        return;
    }

    psync = &g_periodic_data[handle];
    if (!psync->established) {
        console_printf("Sync not established\n");
        return;
    }

    console_printf("completed=%u truncated=%u\n",
                    psync->complete, psync->truncated);
}

static void
handle_periodic_report(struct ble_gap_event *event)
{
    struct psync *psync;
    uint16_t handle = event->periodic_report.sync_handle;

    if (handle >= MYNEWT_VAL(BLE_MAX_PERIODIC_SYNCS)) {
        return;
    }

    psync = &g_periodic_data[handle];

    if (psync->changed ||
            memcmp(psync->data + psync->off, event->periodic_report.data,
                   event->periodic_report.data_length)) {
        /* first fragment with changed data */
        if (!psync->changed) {
            console_printf("Sync data changed, completed=%u, truncated=%u\n",
                           psync->complete, psync->truncated);
        }

        psync->changed = true;

        console_printf("Sync report handle=%u status=", handle);
        switch(event->periodic_report.data_status) {
        case BLE_HCI_PERIODIC_DATA_STATUS_COMPLETE:
            console_printf("complete");
            break;
        case BLE_HCI_PERIODIC_DATA_STATUS_INCOMPLETE:
            console_printf("incomplete");
            break;
        case BLE_HCI_PERIODIC_DATA_STATUS_TRUNCATED:
            console_printf("truncated");
            break;
        default:
            console_printf("reserved 0x%x", event->periodic_report.data_status);
            break;
        }

        btshell_decode_adv_data(event->periodic_report.data,
                                event->periodic_report.data_length, NULL);

        psync->complete = 0;
        psync->truncated = 0;
    }

    /* cache data */
    memcpy(psync->data + psync->off, event->periodic_report.data,
           event->periodic_report.data_length);

    switch(event->periodic_report.data_status) {
    case BLE_HCI_PERIODIC_DATA_STATUS_INCOMPLETE:
        psync->off += event->periodic_report.data_length;
        break;
    case BLE_HCI_PERIODIC_DATA_STATUS_COMPLETE:
        psync->complete++;
        psync->off = 0;
        psync->changed = false;
        break;
    case BLE_HCI_PERIODIC_DATA_STATUS_TRUNCATED:
        psync->truncated++;
        psync->off = 0;
        psync->changed = false;
        break;
    default:
        break;
    }
}
#endif

int
btshell_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int conn_idx;
    int rc;
#if MYNEWT_VAL(BLE_PERIODIC_ADV)
    struct psync *psync;
#endif

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        console_printf("connection %s; status=%d ",
                       event->connect.status == 0 ? "established" : "failed",
                       event->connect.status);

        if (event->connect.status == 0) {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);
            btshell_conn_add(&desc);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        console_printf("disconnect; reason=%d ", event->disconnect.reason);
        print_conn_desc(&event->disconnect.conn);

        conn_idx = btshell_conn_find_idx(event->disconnect.conn.conn_handle);
        if (conn_idx != -1) {
            btshell_conn_delete_idx(conn_idx);
        }

        return btshell_restart_adv(event);
#if MYNEWT_VAL(BLE_EXT_ADV)
    case BLE_GAP_EVENT_EXT_DISC:
        btshell_decode_event_type(&event->ext_disc, arg);
        return 0;
#endif
    case BLE_GAP_EVENT_DISC:
        console_printf("received advertisement; event_type=%d rssi=%d "
                       "addr_type=%d addr=", event->disc.event_type,
                       event->disc.rssi, event->disc.addr.type);
        print_addr(event->disc.addr.val);

        /*
         * There is no adv data to print in case of connectable
         * directed advertising
         */
        if (event->disc.event_type == BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
                console_printf("\nConnectable directed advertising event\n");
                return 0;
        }

        btshell_decode_adv_data(event->disc.data, event->disc.length_data, arg);

        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        console_printf("connection updated; status=%d ",
                       event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        console_printf("connection update request\n");
        *event->conn_update_req.self_params =
            *event->conn_update_req.peer_params;
        return 0;

    case BLE_GAP_EVENT_PASSKEY_ACTION:
        console_printf("passkey action event; action=%d",
                       event->passkey.params.action);
        if (event->passkey.params.action == BLE_SM_IOACT_NUMCMP) {
            console_printf(" numcmp=%lu",
                           (unsigned long)event->passkey.params.numcmp);
        }
        console_printf("\n");
        return 0;


    case BLE_GAP_EVENT_DISC_COMPLETE:
        console_printf("discovery complete; reason=%d\n",
                       event->disc_complete.reason);
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
#if MYNEWT_VAL(BLE_EXT_ADV)
        console_printf("advertise complete; reason=%d, instance=%u, handle=%d\n",
                       event->adv_complete.reason, event->adv_complete.instance,
                       event->adv_complete.conn_handle);

        ext_adv_restart[event->adv_complete.instance].conn_handle =
            event->adv_complete.conn_handle;
#else
        console_printf("advertise complete; reason=%d\n",
                       event->adv_complete.reason);
#endif
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        console_printf("encryption change event; status=%d ",
                       event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        console_printf("notification rx event; attr_handle=%d indication=%d "
                       "len=%d data=",
                       event->notify_rx.attr_handle,
                       event->notify_rx.indication,
                       OS_MBUF_PKTLEN(event->notify_rx.om));

        print_mbuf(event->notify_rx.om);
        console_printf("\n");
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
        console_printf("notification tx event; status=%d attr_handle=%d "
                       "indication=%d\n",
                       event->notify_tx.status,
                       event->notify_tx.attr_handle,
                       event->notify_tx.indication);
        return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
        console_printf("subscribe event; conn_handle=%d attr_handle=%d "
                       "reason=%d prevn=%d curn=%d previ=%d curi=%d\n",
                       event->subscribe.conn_handle,
                       event->subscribe.attr_handle,
                       event->subscribe.reason,
                       event->subscribe.prev_notify,
                       event->subscribe.cur_notify,
                       event->subscribe.prev_indicate,
                       event->subscribe.cur_indicate);
        return 0;

    case BLE_GAP_EVENT_MTU:
        console_printf("mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                       event->mtu.conn_handle,
                       event->mtu.channel_id,
                       event->mtu.value);
        return 0;

    case BLE_GAP_EVENT_IDENTITY_RESOLVED:
        console_printf("identity resolved ");
        rc = ble_gap_conn_find(event->identity_resolved.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
        return 0;

    case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        console_printf("PHY update complete; status=%d, conn_handle=%d "
                       " tx_phy=%d, rx_phy=%d\n",
                       event->phy_updated.status,
                       event->phy_updated.conn_handle,
                       event->phy_updated.tx_phy,
                       event->phy_updated.rx_phy);
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        /* We already have a bond with the peer, but it is attempting to
         * establish a new secure link.  This app sacrifices security for
         * convenience: just throw away the old bond and accept the new link.
         */

        /* Delete the old bond. */
        rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        assert(rc == 0);
        ble_store_util_delete_peer(&desc.peer_id_addr);

        /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
         * continue with the pairing operation.
         */
        return BLE_GAP_REPEAT_PAIRING_RETRY;
#if MYNEWT_VAL(BLE_PERIODIC_ADV)
    case BLE_GAP_EVENT_PERIODIC_SYNC:
        if (event->periodic_sync.status) {
            console_printf("Periodic Sync Establishment Failed; status=%u\n",
                           event->periodic_sync.status);
        } else {
            console_printf("Periodic Sync Established; sync_handle=%u sid=%u "
                            "phy=%u adv_interval=%u ca=%u addr_type=%u addr=",
                       event->periodic_sync.sync_handle,
                       event->periodic_sync.sid, event->periodic_sync.adv_phy,
                       event->periodic_sync.per_adv_ival,
                       event->periodic_sync.adv_clk_accuracy,
                       event->periodic_sync.adv_addr.type);
            print_addr(event->periodic_sync.adv_addr.val);
            console_printf("\n");

            /* TODO non-NimBLE controllers may not start handles from 0 */
            if (event->periodic_sync.sync_handle >= MYNEWT_VAL(BLE_MAX_PERIODIC_SYNCS)) {
                console_printf("Unable to prepare cache for sync data\n");
            } else {
                psync = &g_periodic_data[event->periodic_sync.sync_handle];
                memset(psync, 0, sizeof(*psync));
                psync->changed = true;
                psync->established = true;
            }
        }
        return 0;
    case BLE_GAP_EVENT_PERIODIC_SYNC_LOST:
        /* TODO non-NimBLE controllers may not start handles from 0 */
        if (event->periodic_sync.sync_handle >= MYNEWT_VAL(BLE_MAX_PERIODIC_SYNCS)) {
            console_printf("Periodic Sync Lost; sync_handle=%d reason=%d\n",
                            event->periodic_sync_lost.sync_handle,
                            event->periodic_sync_lost.reason);
        } else {
            psync = &g_periodic_data[event->periodic_sync.sync_handle];

            console_printf("Periodic Sync Lost; sync_handle=%d reason=%d completed=%u truncated=%u\n",
                           event->periodic_sync_lost.sync_handle,
                           event->periodic_sync_lost.reason,
                           psync->complete, psync->truncated);

            memset(psync, 0, sizeof(*psync));
        }
        return 0;
    case BLE_GAP_EVENT_PERIODIC_REPORT:
        handle_periodic_report(event);
        return 0;
#endif
    default:
        return 0;
    }
}

static void
btshell_on_l2cap_update(uint16_t conn_handle, int status, void *arg)
{
    console_printf("l2cap update complete; conn_handle=%d status=%d\n",
                   conn_handle, status);
}

static void
btshell_tx_timer_cb(struct os_event *ev)
{
    uint8_t i;
    uint8_t len;
    int32_t timeout;
    struct ble_l2cap_hdr l2cap_hdr;
    struct os_mbuf *om;

    if ((btshell_tx_data.tx_num == 0) || (btshell_tx_data.tx_len == 0)) {
        return;
    }

    console_printf("Sending %d/%d len: %d\n",
                       btshell_tx_data.tx_num_requested - btshell_tx_data.tx_num + 1,
                       btshell_tx_data.tx_num_requested, btshell_tx_data.tx_len);

    len = btshell_tx_data.tx_len;

    om = NULL;
    if (os_msys_num_free() >= 4) {
        om = os_msys_get_pkthdr(len + BLE_L2CAP_HDR_SZ, BLE_HCI_DATA_HDR_SZ);
    }

    if (om) {
        /*
         * NOTE: CID is 0xffff so it is not confused with valid l2cap channel.
         * The rest of the data gets filled with incrementing pattern starting
         * from 0.
         */
        put_le16(&l2cap_hdr.len, len);
        put_le16(&l2cap_hdr.cid, 0xffff);

        os_mbuf_append(om, (void *)&l2cap_hdr, BLE_L2CAP_HDR_SZ);

        for (i = 0; i < len; ++i) {
            os_mbuf_append(om, (void *)&i, 1);
        }

        ble_hs_lock();
        ble_hs_hci_acl_tx_now(btshell_tx_data.conn, &om);
        ble_hs_unlock();

        --btshell_tx_data.tx_num;
    }

    if (btshell_tx_data.tx_num) {
        timeout = (int32_t)btshell_tx_data.tx_rate;
        timeout = (timeout * OS_TICKS_PER_SEC) / 1000;
        os_callout_reset(&btshell_tx_timer, timeout);
    }
}

int
btshell_exchange_mtu(uint16_t conn_handle)
{
    int rc;

    rc = ble_gattc_exchange_mtu(conn_handle, btshell_on_mtu, NULL);
    return rc;
}

int
btshell_disc_all_chrs(uint16_t conn_handle, uint16_t start_handle,
                      uint16_t end_handle)
{
    intptr_t svc_start_handle;
    int rc;

    svc_start_handle = start_handle;
    rc = ble_gattc_disc_all_chrs(conn_handle, start_handle, end_handle,
                                 btshell_on_disc_c, (void *)svc_start_handle);
    return rc;
}

int
btshell_disc_chrs_by_uuid(uint16_t conn_handle, uint16_t start_handle,
                           uint16_t end_handle, const ble_uuid_t *uuid)
{
    intptr_t svc_start_handle;
    int rc;

    svc_start_handle = start_handle;
    rc = ble_gattc_disc_chrs_by_uuid(conn_handle, start_handle, end_handle,
                                     uuid, btshell_on_disc_c,
                                     (void *)svc_start_handle);
    return rc;
}

int
btshell_disc_svcs(uint16_t conn_handle)
{
    int rc;

    rc = ble_gattc_disc_all_svcs(conn_handle, btshell_on_disc_s, NULL);
    return rc;
}

int
btshell_disc_svc_by_uuid(uint16_t conn_handle, const ble_uuid_t *uuid)
{
    int rc;

    rc = ble_gattc_disc_svc_by_uuid(conn_handle, uuid,
                                    btshell_on_disc_s, NULL);
    return rc;
}

int
btshell_disc_all_dscs(uint16_t conn_handle, uint16_t start_handle,
                      uint16_t end_handle)
{
    int rc;

    rc = ble_gattc_disc_all_dscs(conn_handle, start_handle, end_handle,
                                 btshell_on_disc_d, NULL);
    return rc;
}

int
btshell_disc_full(uint16_t conn_handle)
{
    struct btshell_conn *conn;
    struct btshell_svc *svc;

    /* Undiscover everything first. */
    conn = btshell_conn_find(conn_handle);
    if (conn == NULL) {
        return BLE_HS_ENOTCONN;
    }

    while ((svc = SLIST_FIRST(&conn->svcs)) != NULL) {
        SLIST_REMOVE_HEAD(&conn->svcs, next);
        btshell_svc_delete(svc);
    }

    btshell_full_disc_prev_chr_val = 1;
    btshell_disc_svcs(conn_handle);

    return 0;
}

int
btshell_find_inc_svcs(uint16_t conn_handle, uint16_t start_handle,
                       uint16_t end_handle)
{
    int rc;

    rc = ble_gattc_find_inc_svcs(conn_handle, start_handle, end_handle,
                                 btshell_on_disc_s, NULL);
    return rc;
}

int
btshell_read(uint16_t conn_handle, uint16_t attr_handle)
{
    struct os_mbuf *om;
    int rc;

    if (conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        rc = ble_att_svr_read_local(attr_handle, &om);
        if (rc == 0) {
            console_printf("read local; attr_handle=%d len=%d value=",
                           attr_handle, OS_MBUF_PKTLEN(om));
            print_mbuf(om);
            console_printf("\n");

            os_mbuf_free_chain(om);
        }
    } else {
        rc = ble_gattc_read(conn_handle, attr_handle, btshell_on_read, NULL);
    }
    return rc;
}

int
btshell_read_long(uint16_t conn_handle, uint16_t attr_handle, uint16_t offset)
{
    int rc;

    rc = ble_gattc_read_long(conn_handle, attr_handle, offset,
                             btshell_on_read, NULL);
    return rc;
}

int
btshell_read_by_uuid(uint16_t conn_handle, uint16_t start_handle,
                      uint16_t end_handle, const ble_uuid_t *uuid)
{
    int rc;

    rc = ble_gattc_read_by_uuid(conn_handle, start_handle, end_handle, uuid,
                                btshell_on_read, NULL);
    return rc;
}

int
btshell_read_mult(uint16_t conn_handle, uint16_t *attr_handles,
                   int num_attr_handles)
{
    int rc;

    rc = ble_gattc_read_mult(conn_handle, attr_handles, num_attr_handles,
                             btshell_on_read, NULL);
    return rc;
}

int
btshell_write(uint16_t conn_handle, uint16_t attr_handle, struct os_mbuf *om)
{
    int rc;

    if (conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        rc = ble_att_svr_write_local(attr_handle, om);
    } else {
        rc = ble_gattc_write(conn_handle, attr_handle, om,
                             btshell_on_write, NULL);
    }

    return rc;
}

int
btshell_write_no_rsp(uint16_t conn_handle, uint16_t attr_handle,
                     struct os_mbuf *om)
{
    int rc;

    rc = ble_gattc_write_no_rsp(conn_handle, attr_handle, om);

    return rc;
}

int
btshell_write_long(uint16_t conn_handle, uint16_t attr_handle,
                   uint16_t offset, struct os_mbuf *om)
{
    int rc;

    rc = ble_gattc_write_long(conn_handle, attr_handle, offset,
                              om, btshell_on_write, NULL);
    return rc;
}

int
btshell_write_reliable(uint16_t conn_handle,
                       struct ble_gatt_attr *attrs,
                       int num_attrs)
{
    int rc;

    rc = ble_gattc_write_reliable(conn_handle, attrs, num_attrs,
                                  btshell_on_write_reliable, NULL);
    return rc;
}

#if MYNEWT_VAL(BLE_EXT_ADV)
int
btshell_ext_adv_configure(uint8_t instance,
                          const struct ble_gap_ext_adv_params *params,
                          int8_t *selected_tx_power)
{
    return ble_gap_ext_adv_configure(instance, params, selected_tx_power,
                                     btshell_gap_event, NULL);
}

int
btshell_ext_adv_start(uint8_t instance, int duration,
                      int max_events, bool restart)
{
    int rc;

    /* Advertising restart doesn't make sense
     * with limited duration or events
     */
    if (restart && (duration == 0) && (max_events == 0)) {
        ext_adv_restart[instance].restart = restart;
    }

    rc = ble_gap_ext_adv_start(instance, duration, max_events);

    return rc;
}

int
btshell_ext_adv_stop(uint8_t instance)
{
    int rc;

    ext_adv_restart[instance].restart = false;

    rc = ble_gap_ext_adv_stop(instance);

    return rc;
}
#endif

int
btshell_adv_stop(void)
{
    int rc;

    adv_params.restart = false;

    rc = ble_gap_adv_stop();
    return rc;
}

int
btshell_adv_start(uint8_t own_addr_type, const ble_addr_t *direct_addr,
                  int32_t duration_ms, const struct ble_gap_adv_params *params,
                  bool restart)
{
    int rc;

    if (restart) {
        adv_params.restart = restart;
        adv_params.own_addr_type = own_addr_type;
        adv_params.duration_ms = duration_ms;

        if (direct_addr) {
            memcpy(&adv_params.direct_addr, direct_addr, sizeof(adv_params.direct_addr));
        }

        if (params) {
            memcpy(&adv_params.params, params, sizeof(adv_params.params));
        }
    }

    rc = ble_gap_adv_start(own_addr_type, direct_addr, duration_ms, params,
                           btshell_gap_event, NULL);
    return rc;
}

int
btshell_conn_initiate(uint8_t own_addr_type, const ble_addr_t *peer_addr,
                      int32_t duration_ms, struct ble_gap_conn_params *params)
{
    int rc;

    rc = ble_gap_connect(own_addr_type, peer_addr, duration_ms, params,
                         btshell_gap_event, NULL);

    return rc;
}

int
btshell_ext_conn_initiate(uint8_t own_addr_type, const ble_addr_t *peer_addr,
                          int32_t duration_ms,
                          struct ble_gap_conn_params *phy_1m_params,
                          struct ble_gap_conn_params *phy_2m_params,
                          struct ble_gap_conn_params *phy_coded_params)
{
#if !MYNEWT_VAL(BLE_EXT_ADV)
    console_printf("BLE extended advertising not supported.");
    console_printf(" Configure nimble host to enable it\n");
    return 0;
#else
    int rc;
    uint8_t phy_mask = 0;

    if (phy_1m_params) {
        phy_mask |= BLE_GAP_LE_PHY_1M_MASK;
    }

    if (phy_2m_params) {
        phy_mask |= BLE_GAP_LE_PHY_2M_MASK;
    }

    if (phy_coded_params) {
        phy_mask |= BLE_GAP_LE_PHY_CODED_MASK;
    }

    rc = ble_gap_ext_connect(own_addr_type, peer_addr, duration_ms, phy_mask,
                             phy_1m_params, phy_2m_params, phy_coded_params,
                             btshell_gap_event, NULL);

    return rc;
#endif
}

int
btshell_conn_cancel(void)
{
    int rc;

    rc = ble_gap_conn_cancel();
    return rc;
}

int
btshell_term_conn(uint16_t conn_handle, uint8_t reason)
{
    int rc;

    rc = ble_gap_terminate(conn_handle, reason);
    return rc;
}

int
btshell_wl_set(ble_addr_t *addrs, int addrs_count)
{
    int rc;

    rc = ble_gap_wl_set(addrs, addrs_count);
    return rc;
}

int
btshell_scan(uint8_t own_addr_type, int32_t duration_ms,
             const struct ble_gap_disc_params *disc_params, void *cb_args)
{
    int rc;

    rc = ble_gap_disc(own_addr_type, duration_ms, disc_params,
                      btshell_gap_event, cb_args);
    return rc;
}

int
btshell_ext_scan(uint8_t own_addr_type, uint16_t duration, uint16_t period,
                 uint8_t filter_duplicates, uint8_t filter_policy,
                 uint8_t limited,
                 const struct ble_gap_ext_disc_params *uncoded_params,
                 const struct ble_gap_ext_disc_params *coded_params,
                 void *cb_args)
{
#if !MYNEWT_VAL(BLE_EXT_ADV)
    console_printf("BLE extended advertising not supported.");
    console_printf(" Configure nimble host to enable it\n");
    return 0;
#else
    int rc;

    rc = ble_gap_ext_disc(own_addr_type, duration, period, filter_duplicates,
                          filter_policy, limited, uncoded_params, coded_params,
                          btshell_gap_event, cb_args);
    return rc;
#endif
}

int
btshell_scan_cancel(void)
{
    int rc;

    rc = ble_gap_disc_cancel();
    return rc;
}

int
btshell_update_conn(uint16_t conn_handle, struct ble_gap_upd_params *params)
{
    int rc;

    rc = ble_gap_update_params(conn_handle, params);
    return rc;
}

void
btshell_notify(uint16_t attr_handle)
{
    ble_gatts_chr_updated(attr_handle);
}

int
btshell_datalen(uint16_t conn_handle, uint16_t tx_octets, uint16_t tx_time)
{
    int rc;

    rc = ble_hs_hci_util_set_data_len(conn_handle, tx_octets, tx_time);
    return rc;
}

int
btshell_l2cap_update(uint16_t conn_handle,
                     struct ble_l2cap_sig_update_params *params)
{
    int rc;

    rc = ble_l2cap_sig_update(conn_handle, params, btshell_on_l2cap_update,
                              NULL);
    return rc;
}

int
btshell_sec_pair(uint16_t conn_handle)
{
#if !NIMBLE_BLE_SM
    return BLE_HS_ENOTSUP;
#endif

    int rc;

    rc = ble_gap_pair_initiate(conn_handle);
    return rc;
}

int
btshell_sec_unpair(ble_addr_t *peer_addr)
{
#if !NIMBLE_BLE_SM
    return BLE_HS_ENOTSUP;
#endif

    int rc;

    rc = ble_gap_unpair(peer_addr);
    return rc;
}

int
btshell_sec_start(uint16_t conn_handle)
{
#if !NIMBLE_BLE_SM
    return BLE_HS_ENOTSUP;
#endif

    int rc;

    rc = ble_gap_security_initiate(conn_handle);
    return rc;
}

int
btshell_sec_restart(uint16_t conn_handle,
                    uint8_t key_size,
                    uint8_t *ltk,
                    uint16_t ediv,
                    uint64_t rand_val,
                    int auth)
{
#if !NIMBLE_BLE_SM
    return BLE_HS_ENOTSUP;
#endif

    struct ble_store_value_sec value_sec;
    struct ble_store_key_sec key_sec;
    struct ble_gap_conn_desc desc;
    ble_hs_conn_flags_t conn_flags;
    int rc;

    if (ltk == NULL) {
        /* The user is requesting a store lookup. */
        rc = ble_gap_conn_find(conn_handle, &desc);
        if (rc != 0) {
            return rc;
        }

        memset(&key_sec, 0, sizeof key_sec);
        key_sec.peer_addr = desc.peer_id_addr;

        rc = ble_hs_atomic_conn_flags(conn_handle, &conn_flags);
        if (rc != 0) {
            return rc;
        }
        if (conn_flags & BLE_HS_CONN_F_MASTER) {
            rc = ble_store_read_peer_sec(&key_sec, &value_sec);
        } else {
            rc = ble_store_read_our_sec(&key_sec, &value_sec);
        }
        if (rc != 0) {
            return rc;
        }

        ltk = value_sec.ltk;
        key_size = value_sec.key_size;
        ediv = value_sec.ediv;
        rand_val = value_sec.rand_num;
        auth = value_sec.authenticated;
    }

    rc = ble_gap_encryption_initiate(conn_handle, key_size, ltk,
                                     ediv, rand_val, auth);
    return rc;
}

/**
 * Called to start transmitting 'num' packets at rate 'rate' of size 'size'
 * to connection handle 'handle'
 *
 * @param handle
 * @param len
 * @param rate
 * @param num
 *
 * @return int
 */
int
btshell_tx_start(uint16_t conn_handle, uint16_t len, uint16_t rate, uint16_t num)
{
    /* Cannot be currently in a session */
    if (num == 0) {
        return 0;
    }

    /* Do not allow start if already in progress */
    if (btshell_tx_data.tx_num != 0) {
        return -1;
    }

    /* XXX: for now, must have contiguous mbuf space */
    if ((len + 4) > MYNEWT_VAL_MSYS_1_BLOCK_SIZE) {
        return -2;
    }

    btshell_tx_data.tx_num = num;
    btshell_tx_data.tx_num_requested = num;
    btshell_tx_data.tx_rate = rate;
    btshell_tx_data.tx_len = len;
    btshell_tx_data.tx_conn_handle = conn_handle;

    ble_hs_lock();
    btshell_tx_data.conn = ble_hs_conn_find(conn_handle);
    ble_hs_unlock();

    if (!btshell_tx_data.conn) {
        console_printf("Could not find ble_hs_conn for handle: %d\n",
                       conn_handle);
        return -1;
    }

    os_callout_reset(&btshell_tx_timer, 0);

    return 0;
}

void
btshell_tx_stop(void)
{
    os_callout_stop(&btshell_tx_timer);
    btshell_tx_data.tx_num = 0;
}

int
btshell_rssi(uint16_t conn_handle, int8_t *out_rssi)
{
    int rc;

    rc = ble_gap_conn_rssi(conn_handle, out_rssi);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

static void
btshell_on_reset(int reason)
{
    console_printf("Error: Resetting state; reason=%d\n", reason);
}

static void
btshell_on_sync(void)
{
    console_printf("Host and controller synced\n");
}

#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) != 0

static int
btshell_l2cap_coc_add(uint16_t conn_handle, struct ble_l2cap_chan *chan)
{
    struct btshell_conn *conn;
    struct btshell_l2cap_coc *coc;
    struct btshell_l2cap_coc *prev, *cur;

    conn = btshell_conn_find(conn_handle);
    assert(conn != NULL);

    coc = os_memblock_get(&btshell_coc_conn_pool);
    if (!coc) {
        return ENOMEM;
    }

    coc->chan = chan;

    prev = NULL;
    SLIST_FOREACH(cur, &conn->coc_list, next) {
        prev = cur;
    }

    if (prev == NULL) {
        SLIST_INSERT_HEAD(&conn->coc_list, coc, next);
    } else {
        SLIST_INSERT_AFTER(prev, coc, next);
    }

    return 0;
}

static void
btshell_l2cap_coc_remove(uint16_t conn_handle, struct ble_l2cap_chan *chan)
{
    struct btshell_conn *conn;
    struct btshell_l2cap_coc *coc;
    struct btshell_l2cap_coc *cur;

    conn = btshell_conn_find(conn_handle);
    assert(conn != NULL);

    coc = NULL;
    SLIST_FOREACH(cur, &conn->coc_list, next) {
        if (cur->chan == chan) {
            coc = cur;
            break;
        }
    }

    if (!coc) {
        return;
    }

    SLIST_REMOVE(&conn->coc_list, coc, btshell_l2cap_coc, next);
    os_memblock_put(&btshell_coc_conn_pool, coc);
}

static void
btshell_l2cap_coc_recv(struct ble_l2cap_chan *chan, struct os_mbuf *sdu)
{
    console_printf("LE CoC SDU received, chan: 0x%08lx, data len %d\n",
                   (uint32_t) chan, OS_MBUF_PKTLEN(sdu));

    os_mbuf_free_chain(sdu);
    sdu = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    assert(sdu != NULL);

    if (ble_l2cap_recv_ready(chan, sdu) != 0) {
        assert(0);
    }
}

static int
btshell_l2cap_coc_accept(uint16_t conn_handle, uint16_t peer_mtu,
                           struct ble_l2cap_chan *chan)
{
    struct os_mbuf *sdu_rx;

    console_printf("LE CoC accepting, chan: 0x%08lx, peer_mtu %d\n",
                   (uint32_t) chan, peer_mtu);

    sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    if (!sdu_rx) {
        return BLE_HS_ENOMEM;
    }

    return ble_l2cap_recv_ready(chan, sdu_rx);
}

static int
btshell_l2cap_event(struct ble_l2cap_event *event, void *arg)
{
    int accept_response;

    switch(event->type) {
        case BLE_L2CAP_EVENT_COC_CONNECTED:
            if (event->connect.status) {
                console_printf("LE COC error: %d\n", event->connect.status);
                return 0;
            }

            console_printf("LE COC connected, conn: %d, chan: 0x%08lx, scid: 0x%04x, "
                           "dcid: 0x%04x, our_mtu: 0x%04x, peer_mtu: 0x%04x\n",
                           event->connect.conn_handle,
                           (uint32_t) event->connect.chan,
                           ble_l2cap_get_scid(event->connect.chan),
                           ble_l2cap_get_dcid(event->connect.chan),
                           ble_l2cap_get_our_mtu(event->connect.chan),
                           ble_l2cap_get_peer_mtu(event->connect.chan));

            btshell_l2cap_coc_add(event->connect.conn_handle,
                                  event->connect.chan);

            return 0;
        case BLE_L2CAP_EVENT_COC_DISCONNECTED:
            console_printf("LE CoC disconnected, chan: 0x%08lx\n",
                           (uint32_t) event->disconnect.chan);

            btshell_l2cap_coc_remove(event->disconnect.conn_handle,
                                     event->disconnect.chan);
            return 0;
        case BLE_L2CAP_EVENT_COC_ACCEPT:
            accept_response = PTR_TO_INT(arg);
            if (accept_response) {
                return accept_response;
            }

            return btshell_l2cap_coc_accept(event->accept.conn_handle,
                                            event->accept.peer_sdu_size,
                                            event->accept.chan);

        case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
            btshell_l2cap_coc_recv(event->receive.chan, event->receive.sdu_rx);
            return 0;
        case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
            console_printf("L2CAP CoC channel %p unstalled, last sdu sent with err=0x%02x\n",
                            event->tx_unstalled.chan, event->tx_unstalled.status);
            return 0;
        default:
            return 0;
    }
}
#endif

int
btshell_l2cap_create_srv(uint16_t psm, int accept_response)
{
#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) == 0
    console_printf("BLE L2CAP LE COC not supported.");
    console_printf(" Configure nimble host to enable it\n");
    return 0;
#else

    return ble_l2cap_create_server(psm, BTSHELL_COC_MTU, btshell_l2cap_event,
                                   INT_TO_PTR(accept_response));
#endif
}

int
btshell_l2cap_connect(uint16_t conn_handle, uint16_t psm)
{
#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) == 0
    console_printf("BLE L2CAP LE COC not supported.");
    console_printf(" Configure nimble host to enable it\n");
    return 0;
#else

    struct os_mbuf *sdu_rx;

    sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    assert(sdu_rx != NULL);

    return ble_l2cap_connect(conn_handle, psm, BTSHELL_COC_MTU, sdu_rx,
                             btshell_l2cap_event, NULL);
#endif
}

int
btshell_l2cap_disconnect(uint16_t conn_handle, uint16_t idx)
{
#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) == 0
    console_printf("BLE L2CAP LE COC not supported.");
    console_printf(" Configure nimble host to enable it\n");
    return 0;
#else

    struct btshell_conn *conn;
    struct btshell_l2cap_coc *coc;
    int i;
    int rc = 0;

    conn = btshell_conn_find(conn_handle);
    assert(conn != NULL);

    i = 0;
    SLIST_FOREACH(coc, &conn->coc_list, next) {
        if (i == idx) {
                break;
        }
        i++;
    }
    assert(coc != NULL);

    rc = ble_l2cap_disconnect(coc->chan);
    if (rc) {
        console_printf("Could not disconnect channel rc=%d\n", rc);
    }

    return rc;
#endif
}

int
btshell_l2cap_send(uint16_t conn_handle, uint16_t idx, uint16_t bytes)
{
#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) == 0
    console_printf("BLE L2CAP LE COC not supported.");
    console_printf(" Configure nimble host to enable it\n");
    return 0;
#else

    struct btshell_conn *conn;
    struct btshell_l2cap_coc *coc;
    struct os_mbuf *sdu_tx;
    uint8_t b[] = {0x00, 0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88, 0x99};
    int i;
    int rc;

    console_printf("conn=%d, idx=%d, bytes=%d\n", conn_handle, idx, bytes);

    conn = btshell_conn_find(conn_handle);
    if (conn == NULL) {
        console_printf("conn=%d does not exist\n", conn_handle);
        return 0;
    }

    i = 0;
    SLIST_FOREACH(coc, &conn->coc_list, next) {
        if (i == idx) {
            break;
        }
        i++;
    }
    if (coc == NULL) {
        console_printf("Are you sure your channel exist?\n");
        return 0;
    }

    sdu_tx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    if (sdu_tx == NULL) {
        console_printf("No memory in the test sdu pool\n");
        return 0;
    }

    /* For the testing purpose we fill up buffer with known data, easy
     * to validate on other side. In this loop we add as many full chunks as we
     * can
     */
    for (i = 0; i < bytes / sizeof(b); i++) {
        rc = os_mbuf_append(sdu_tx, b, sizeof(b));
        if (rc) {
            console_printf("Cannot append data %i !\n", i);
            os_mbuf_free_chain(sdu_tx);
            return rc;
        }
    }

    /* Here we add the rest < sizeof(b) */
    rc = os_mbuf_append(sdu_tx, b, bytes - (sizeof(b) * i));
    if (rc) {
        console_printf("Cannot append data %i !\n", i);
        os_mbuf_free_chain(sdu_tx);
        return rc;
    }

    rc = ble_l2cap_send(coc->chan, sdu_tx);
    if (rc) {
        console_printf("Could not send data rc=%d\n", rc);
        os_mbuf_free_chain(sdu_tx);
    }

    return rc;

#endif
}

static void
btshell_init_ext_adv_restart(void)
{
#if MYNEWT_VAL(BLE_EXT_ADV)
    int i;

    for (i = 0; i < BLE_ADV_INSTANCES; ++i) {
        ext_adv_restart[i].conn_handle = BLE_HS_CONN_HANDLE_NONE;
    }
#endif
}

/**
 * main
 *
 * The main task for the project. This function initializes the packages,
 * then starts serving events from default event queue.
 *
 * @return int NOTE: this function should never return!
 */
int
main(int argc, char **argv)
{
    int rc;

#ifdef ARCH_sim
    mcu_sim_parse_args(argc, argv);
#endif

    /* Initialize OS */
    sysinit();

    /* Initialize some application specific memory pools. */
    rc = os_mempool_init(&btshell_svc_pool, BTSHELL_MAX_SVCS,
                         sizeof (struct btshell_svc), btshell_svc_mem,
                         "btshell_svc_pool");
    assert(rc == 0);

    rc = os_mempool_init(&btshell_chr_pool, BTSHELL_MAX_CHRS,
                         sizeof (struct btshell_chr), btshell_chr_mem,
                         "btshell_chr_pool");
    assert(rc == 0);

    rc = os_mempool_init(&btshell_dsc_pool, BTSHELL_MAX_DSCS,
                         sizeof (struct btshell_dsc), btshell_dsc_mem,
                         "btshell_dsc_pool");
    assert(rc == 0);

#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) != 0
    /* For testing we want to support all the available channels */
    rc = os_mempool_init(&sdu_coc_mbuf_mempool, BTSHELL_COC_BUF_COUNT,
                         BTSHELL_COC_MTU, btshell_sdu_coc_mem,
                         "btshell_coc_sdu_pool");
    assert(rc == 0);

    rc = os_mbuf_pool_init(&sdu_os_mbuf_pool, &sdu_coc_mbuf_mempool,
                           BTSHELL_COC_MTU, BTSHELL_COC_BUF_COUNT);
    assert(rc == 0);

    rc = os_mempool_init(&btshell_coc_conn_pool,
                         MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM),
                         sizeof (struct btshell_l2cap_coc), btshell_coc_conn_mem,
                         "btshell_coc_conn_pool");
    assert(rc == 0);
#endif

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = btshell_on_reset;
    ble_hs_cfg.sync_cb = btshell_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    rc = gatt_svr_init();
    assert(rc == 0);

    cmd_init();

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("nimble-btshell");
    assert(rc == 0);

    /* Create a callout (timer).  This callout is used by the "tx" btshell
     * command to repeatedly send packets of sequential data bytes.
     */
    os_callout_init(&btshell_tx_timer, os_eventq_dflt_get(),
                    btshell_tx_timer_cb, NULL);

    btshell_init_ext_adv_restart();

    while (1) {
        os_eventq_run(os_eventq_dflt_get());
    }
    /* os start should never return. If it does, this should be an error */
    assert(0);

    return 0;
}
