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
#include "bsp/bsp.h"
#include "console/console.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/ble_gatt.h"
#include "btshell.h"

/* 0000xxxx-8c26-476f-89a7-a108033a69c7 */
#define PTS_UUID_DECLARE(uuid16)                                \
    ((const ble_uuid_t *) (&(ble_uuid128_t) BLE_UUID128_INIT(   \
        0xc7, 0x69, 0x3a, 0x03, 0x08, 0xa1, 0xa7, 0x89,         \
        0x6f, 0x47, 0x26, 0x8c, uuid16, uuid16 >> 8, 0x00, 0x00 \
    )))

#define  PTS_SVC                         0x0001
#define  PTS_CHR_READ                    0x0002
#define  PTS_CHR_WRITE                   0x0003
#define  PTS_CHR_RELIABLE_WRITE          0x0004
#define  PTS_CHR_WRITE_NO_RSP            0x0005
#define  PTS_CHR_READ_WRITE              0x0006
#define  PTS_CHR_READ_WRITE_ENC          0x0007
#define  PTS_CHR_READ_WRITE_AUTHEN       0x0008
#define  PTS_DSC_READ                    0x0009
#define  PTS_DSC_WRITE                   0x000a
#define  PTS_DSC_READ_WRITE              0x000b
#define  PTS_DSC_READ_WRITE_ENC          0x000c
#define  PTS_DSC_READ_WRITE_AUTHEN       0x000d

#define  PTS_LONG_SVC                    0x0011
#define  PTS_LONG_CHR_READ               0x0012
#define  PTS_LONG_CHR_WRITE              0x0013
#define  PTS_LONG_CHR_RELIABLE_WRITE     0x0014
#define  PTS_LONG_CHR_READ_WRITE         0x0015
#define  PTS_LONG_CHR_READ_WRITE_ALT     0x0016
#define  PTS_LONG_CHR_READ_WRITE_ENC     0x0017
#define  PTS_LONG_CHR_READ_WRITE_AUTHEN  0x0018
#define  PTS_LONG_DSC_READ               0x0019
#define  PTS_LONG_DSC_WRITE              0x001a
#define  PTS_LONG_DSC_READ_WRITE         0x001b
#define  PTS_LONG_DSC_READ_WRITE_ENC     0x001c
#define  PTS_LONG_DSC_READ_WRITE_AUTHEN  0x001d

#define  PTS_INC_SVC                     0x001e
#define  PTS_CHR_READ_WRITE_ALT          0x001f

/**
 * The vendor specific security test service consists of two characteristics:
 *     o random-number-generator: generates a random 32-bit number each time
 *       it is read.  This characteristic can only be read over an encrypted
 *       connection.
 *     o static-value: a single-byte characteristic that can always be read,
 *       but can only be written over an encrypted connection.
 */

/* 59462f12-9543-9999-12c8-58b459a2712d */
static const ble_uuid128_t gatt_svr_svc_sec_test_uuid =
    BLE_UUID128_INIT(0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12,
                     0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59);

/* 5c3a659e-897e-45e1-b016-007107c96df6 */
static const ble_uuid128_t gatt_svr_chr_sec_test_rand_uuid =
    BLE_UUID128_INIT(0xf6, 0x6d, 0xc9, 0x07, 0x71, 0x00, 0x16, 0xb0,
                     0xe1, 0x45, 0x7e, 0x89, 0x9e, 0x65, 0x3a, 0x5c);

/* 5c3a659e-897e-45e1-b016-007107c96df7 */
static const ble_uuid128_t gatt_svr_chr_sec_test_static_uuid =
    BLE_UUID128_INIT(0xf7, 0x6d, 0xc9, 0x07, 0x71, 0x00, 0x16, 0xb0,
                     0xe1, 0x45, 0x7e, 0x89, 0x9e, 0x65, 0x3a, 0x5c);

/* 5c3a659e-897e-45e1-b016-007107c96df8 */
static const ble_uuid128_t gatt_svr_chr_sec_test_static_auth_uuid =
    BLE_UUID128_INIT(0xf8, 0x6d, 0xc9, 0x07, 0x71, 0x00, 0x16, 0xb0,
                     0xe1, 0x45, 0x7e, 0x89, 0x9e, 0x65, 0x3a, 0x5c);

static uint8_t gatt_svr_sec_test_static_val;

static uint8_t gatt_svr_pts_static_val;
static uint8_t gatt_svr_pts_static_long_val[30];
static uint8_t gatt_svr_pts_static_long_val_alt[30];

static int
gatt_svr_chr_access_sec_test(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg);

static int
gatt_svr_access_test(uint16_t conn_handle, uint16_t attr_handle,
                     struct ble_gatt_access_ctxt *ctxt,
                     void *arg);

static int
gatt_svr_long_access_test(uint16_t conn_handle, uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt,
                                  void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /*** Service: PTS test. */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = PTS_UUID_DECLARE(PTS_SVC),
        .characteristics = (struct ble_gatt_chr_def[]) { {
                .uuid = PTS_UUID_DECLARE(PTS_CHR_READ),
                .access_cb = gatt_svr_access_test,
                .flags = BLE_GATT_CHR_F_READ,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_CHR_WRITE),
                .access_cb = gatt_svr_access_test,
                .flags = BLE_GATT_CHR_F_WRITE,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_CHR_RELIABLE_WRITE),
                .access_cb = gatt_svr_access_test,
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_RELIABLE_WRITE,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_CHR_WRITE_NO_RSP),
                .access_cb = gatt_svr_access_test,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE_NO_RSP,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE),
                .access_cb = gatt_svr_access_test,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_ENC),
                .access_cb = gatt_svr_access_test,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_AUTHEN),
                .access_cb = gatt_svr_access_test,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_AUTHEN |
                BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_AUTHEN,

                .descriptors = (struct ble_gatt_dsc_def[]){ {
                        .uuid = PTS_UUID_DECLARE(PTS_DSC_READ),
                        .access_cb = gatt_svr_access_test,
                        .att_flags = BLE_ATT_F_READ,
                    }, {
                        .uuid = PTS_UUID_DECLARE(PTS_DSC_WRITE),
                        .access_cb = gatt_svr_access_test,
                        .att_flags = BLE_ATT_F_WRITE,
                    }, {
                        .uuid = PTS_UUID_DECLARE(PTS_DSC_READ_WRITE),
                        .access_cb = gatt_svr_access_test,
                        .att_flags = BLE_ATT_F_READ | BLE_ATT_F_WRITE,
                    }, {
                        .uuid = PTS_UUID_DECLARE(PTS_DSC_READ_WRITE_ENC),
                        .access_cb = gatt_svr_access_test,
                        .att_flags = BLE_ATT_F_READ | BLE_ATT_F_READ_ENC |
                        BLE_ATT_F_WRITE | BLE_ATT_F_WRITE_ENC,
                        .min_key_size = 16,
                    }, {
                        .uuid = PTS_UUID_DECLARE(PTS_DSC_READ_WRITE_AUTHEN),
                        .access_cb = gatt_svr_access_test,
                        .att_flags = BLE_ATT_F_READ | BLE_ATT_F_READ_AUTHEN |
                        BLE_ATT_F_WRITE | BLE_ATT_F_WRITE_AUTHEN,
                    }, {
                        0, /* No more descriptors in this characteristic. */
                    } }
            }, {
                0, /* No more characteristics in this service. */
            } },
    },

    {
        /*** Service: PTS long test. */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = PTS_UUID_DECLARE(PTS_LONG_SVC),
        .characteristics = (struct ble_gatt_chr_def[]) { {
                .uuid = PTS_UUID_DECLARE(PTS_LONG_CHR_READ),
                .access_cb = gatt_svr_long_access_test,
                .flags = BLE_GATT_CHR_F_READ,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_LONG_CHR_WRITE),
                .access_cb = gatt_svr_long_access_test,
                .flags = BLE_GATT_CHR_F_WRITE,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_LONG_CHR_RELIABLE_WRITE),
                .access_cb = gatt_svr_long_access_test,
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_RELIABLE_WRITE,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_LONG_CHR_READ_WRITE),
                .access_cb = gatt_svr_long_access_test,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_LONG_CHR_READ_WRITE_ALT),
                .access_cb = gatt_svr_long_access_test,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_LONG_CHR_READ_WRITE_ENC),
                .access_cb = gatt_svr_long_access_test,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = PTS_UUID_DECLARE(PTS_LONG_CHR_READ_WRITE_AUTHEN),
                .access_cb = gatt_svr_long_access_test,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_AUTHEN |
                BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_AUTHEN,

                .descriptors = (struct ble_gatt_dsc_def[]){ {
                        .uuid = PTS_UUID_DECLARE(PTS_LONG_DSC_READ),
                        .access_cb = gatt_svr_long_access_test,
                        .att_flags = BLE_ATT_F_READ,
                    }, {
                        .uuid = PTS_UUID_DECLARE(PTS_LONG_DSC_WRITE),
                        .access_cb = gatt_svr_long_access_test,
                        .att_flags = BLE_ATT_F_WRITE,
                    }, {
                        .uuid = PTS_UUID_DECLARE(PTS_LONG_DSC_READ_WRITE),
                        .access_cb = gatt_svr_long_access_test,
                        .att_flags = BLE_ATT_F_READ | BLE_ATT_F_WRITE,
                    }, {
                        .uuid = PTS_UUID_DECLARE(PTS_LONG_DSC_READ_WRITE_ENC),
                        .access_cb = gatt_svr_long_access_test,
                        .att_flags = BLE_ATT_F_READ | BLE_ATT_F_READ_ENC |
                        BLE_ATT_F_WRITE | BLE_ATT_F_WRITE_ENC,
                        .min_key_size = 16,
                    }, {
                        .uuid = PTS_UUID_DECLARE(PTS_LONG_DSC_READ_WRITE_AUTHEN),
                        .access_cb = gatt_svr_long_access_test,
                        .att_flags = BLE_ATT_F_READ | BLE_ATT_F_READ_AUTHEN |
                        BLE_ATT_F_WRITE | BLE_ATT_F_WRITE_AUTHEN,
                    }, {
                        0, /* No more descriptors in this characteristic. */
                    } }
            }, {
                0, /* No more characteristics in this service. */
            } },
    },

    {
        /*** Service: Security test. */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_sec_test_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /*** Characteristic: Random number generator. */
            .uuid = &gatt_svr_chr_sec_test_rand_uuid.u,
            .access_cb = gatt_svr_chr_access_sec_test,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
        }, {
            /*** Characteristic: Static value. */
            .uuid = &gatt_svr_chr_sec_test_static_uuid.u,
            .access_cb = gatt_svr_chr_access_sec_test,
            .flags = BLE_GATT_CHR_F_READ |
                     BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC,
        }, {
            /*** Characteristic: Static value. */
            .uuid = &gatt_svr_chr_sec_test_static_auth_uuid.u,
            .access_cb = gatt_svr_chr_access_sec_test,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_AUTHEN,
        }, {
            0, /* No more characteristics in this service. */
        } },
    },

    {
        0, /* No more services. */
    },
};

static const struct ble_gatt_svc_def *inc_svcs[] = {
    &gatt_svr_svcs[0],
    NULL,
};

static const struct ble_gatt_svc_def gatt_svr_inc_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = PTS_UUID_DECLARE(PTS_INC_SVC),
        .includes = inc_svcs,
        .characteristics = (struct ble_gatt_chr_def[]) {{
            .uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_ALT),
            .access_cb = gatt_svr_access_test,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            0, /* No more characteristics */
        }, },
    },

    {
        0, /* No more services. */
    },
};

static int
gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
                   void *dst, uint16_t *len)
{
    uint16_t om_len;
    int rc;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

static int
gatt_svr_chr_access_sec_test(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg)
{
    const ble_uuid_t *uuid;
    int rand_num;
    int rc;

    uuid = ctxt->chr->uuid;

    /* Determine which characteristic is being accessed by examining its
     * 128-bit UUID.
     */

    if (ble_uuid_cmp(uuid, &gatt_svr_chr_sec_test_rand_uuid.u) == 0) {
        assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

        /* Respond with a 32-bit random number. */
        rand_num = rand();
        rc = os_mbuf_append(ctxt->om, &rand_num, sizeof rand_num);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (ble_uuid_cmp(uuid, &gatt_svr_chr_sec_test_static_uuid.u) == 0 ||
        ble_uuid_cmp(uuid, &gatt_svr_chr_sec_test_static_auth_uuid.u) == 0) {
        switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            rc = os_mbuf_append(ctxt->om, &gatt_svr_sec_test_static_val,
                                sizeof gatt_svr_sec_test_static_val);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            rc = gatt_svr_chr_write(ctxt->om,
                                    sizeof gatt_svr_sec_test_static_val,
                                    sizeof gatt_svr_sec_test_static_val,
                                    &gatt_svr_sec_test_static_val, NULL);
            return rc;

        default:
            assert(0);
            return BLE_ATT_ERR_UNLIKELY;
        }
    }

    /* Unknown characteristic; the nimble stack should not have called this
     * function.
     */
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

/* This method is used for PTS testing only, to extract 16 bit value
 * from 128 bit vendor specific UUID.
 */
static uint16_t
extract_uuid16_from_pts_uuid128(const ble_uuid_t *uuid)
{
    const uint8_t *u8ptr;
    uint16_t uuid16;

    u8ptr = BLE_UUID128(uuid)->value;
    uuid16 = u8ptr[12];
    uuid16 |= (uint16_t)u8ptr[13] << 8;
    return uuid16;
}

static int
gatt_svr_access_test(uint16_t conn_handle, uint16_t attr_handle,
                     struct ble_gatt_access_ctxt *ctxt,
                     void *arg)
{
    uint16_t uuid16;
    int rc;

    uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
    assert(uuid16 != 0);

    switch (uuid16) {
    case PTS_CHR_READ:
        assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);
        rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_val,
                            sizeof gatt_svr_pts_static_val);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    case PTS_CHR_WRITE:
    case PTS_CHR_RELIABLE_WRITE:
    case PTS_CHR_WRITE_NO_RSP:
        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
            rc = gatt_svr_chr_write(ctxt->om,0,
                                    sizeof gatt_svr_pts_static_val,
                                    &gatt_svr_pts_static_val, NULL);
            return rc;
        } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
            rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_val,
                                sizeof gatt_svr_pts_static_val);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

    case PTS_CHR_READ_WRITE:
    case PTS_CHR_READ_WRITE_ENC:
    case PTS_CHR_READ_WRITE_AUTHEN:
    case PTS_CHR_READ_WRITE_ALT:
        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
            rc = gatt_svr_chr_write(ctxt->om,0,
                                    sizeof gatt_svr_pts_static_val,
                                    &gatt_svr_pts_static_val, NULL);
            return rc;
        } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
            rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_val,
                                sizeof gatt_svr_pts_static_val);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

    case PTS_DSC_READ:
        assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC);
        rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_val,
                            sizeof gatt_svr_pts_static_val);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    case PTS_DSC_WRITE:
        assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_DSC);
        rc = gatt_svr_chr_write(ctxt->om,0,
                                sizeof gatt_svr_pts_static_val,
                                &gatt_svr_pts_static_val, NULL);
        return rc;

    case PTS_DSC_READ_WRITE:
    case PTS_DSC_READ_WRITE_ENC:
    case PTS_DSC_READ_WRITE_AUTHEN:
        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_DSC) {
            rc = gatt_svr_chr_write(ctxt->om,0,
                                    sizeof gatt_svr_pts_static_val,
                                    &gatt_svr_pts_static_val, NULL);
            return rc;
        } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
            rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_val,
                                sizeof gatt_svr_pts_static_val);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

    default:
        assert(0);
        return BLE_ATT_ERR_UNLIKELY;
    }
}

static int
gatt_svr_long_access_test(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt,
                          void *arg)
{
    uint16_t uuid16;
    int rc;

    uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
    assert(uuid16 != 0);

    switch (uuid16) {
    case PTS_LONG_CHR_READ:
        assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);
        rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_long_val,
                            sizeof gatt_svr_pts_static_long_val);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    case PTS_LONG_CHR_WRITE:
    case PTS_LONG_CHR_RELIABLE_WRITE:
        assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);
        rc = gatt_svr_chr_write(ctxt->om,0,
                                sizeof gatt_svr_pts_static_long_val,
                                &gatt_svr_pts_static_long_val, NULL);
        return rc;

    case PTS_LONG_CHR_READ_WRITE:
        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
            rc = gatt_svr_chr_write(ctxt->om,0,
                                    sizeof gatt_svr_pts_static_long_val,
                                    &gatt_svr_pts_static_long_val, NULL);
            return rc;
        } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
            rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_long_val,
                                sizeof gatt_svr_pts_static_long_val);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

    case PTS_LONG_CHR_READ_WRITE_ALT:
        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
            rc = gatt_svr_chr_write(ctxt->om,0,
                                    sizeof gatt_svr_pts_static_long_val_alt,
                                    &gatt_svr_pts_static_long_val_alt, NULL);
            return rc;
        } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
            rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_long_val_alt,
                                sizeof gatt_svr_pts_static_long_val_alt);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

    case PTS_LONG_CHR_READ_WRITE_ENC:
    case PTS_LONG_CHR_READ_WRITE_AUTHEN:
        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
            rc = gatt_svr_chr_write(ctxt->om,0,
                                    sizeof gatt_svr_pts_static_long_val,
                                    &gatt_svr_pts_static_long_val, NULL);
            return rc;
        } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
            rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_long_val,
                                sizeof gatt_svr_pts_static_long_val);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

    case PTS_LONG_DSC_READ:
        assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC);
        rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_long_val,
                            sizeof gatt_svr_pts_static_long_val);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    case PTS_LONG_DSC_WRITE:
        assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_DSC);
        rc = gatt_svr_chr_write(ctxt->om,0,
                                sizeof gatt_svr_pts_static_long_val,
                                &gatt_svr_pts_static_long_val, NULL);
        return rc;

    case PTS_LONG_DSC_READ_WRITE:
    case PTS_LONG_DSC_READ_WRITE_ENC:
    case PTS_LONG_DSC_READ_WRITE_AUTHEN:
        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_DSC) {
            rc = gatt_svr_chr_write(ctxt->om,0,
                                    sizeof gatt_svr_pts_static_long_val,
                                    &gatt_svr_pts_static_long_val, NULL);
            return rc;
        } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
            rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_long_val,
                                sizeof gatt_svr_pts_static_long_val);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

    default:
        assert(0);
        return BLE_ATT_ERR_UNLIKELY;
    }
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                    ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                           "def_handle=%d val_handle=%d\n",
                    ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                    ctxt->chr.def_handle,
                    ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                    ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

void
gatt_svr_print_svcs(void)
{
    ble_gatts_show_local();
}

int
gatt_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_count_cfg(gatt_svr_inc_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_inc_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
