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
#include <errno.h>
#include <string.h>
#include "testutil/testutil.h"
#include "nimble/hci_common.h"
#include "host/ble_hs_adv.h"
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

#define BLE_ADV_TEST_DATA_OFF   4

static void
ble_hs_adv_test_misc_verify_tx_adv_data_hdr(uint8_t *cmd, int data_len)
{
    uint16_t opcode;

    opcode = get_le16(cmd + 0);
    TEST_ASSERT(BLE_HCI_OGF(opcode) == BLE_HCI_OGF_LE);
    TEST_ASSERT(BLE_HCI_OCF(opcode) == BLE_HCI_OCF_LE_SET_ADV_DATA);

    TEST_ASSERT(cmd[2] == BLE_HCI_SET_ADV_DATA_LEN);
    TEST_ASSERT(cmd[3] == data_len);
}

static void
ble_hs_adv_test_misc_verify_tx_rsp_data_hdr(uint8_t *cmd, int data_len)
{
    uint16_t opcode;

    opcode = get_le16(cmd + 0);
    TEST_ASSERT(BLE_HCI_OGF(opcode) == BLE_HCI_OGF_LE);
    TEST_ASSERT(BLE_HCI_OCF(opcode) == BLE_HCI_OCF_LE_SET_SCAN_RSP_DATA);

    TEST_ASSERT(cmd[2] == BLE_HCI_SET_SCAN_RSP_DATA_LEN);
    TEST_ASSERT(cmd[3] == data_len);
}

static void
ble_hs_adv_test_misc_verify_tx_field(uint8_t *cmd, uint8_t type,
                                     uint8_t val_len, void *val)
{
    TEST_ASSERT(cmd[0] == val_len + 1);
    TEST_ASSERT(cmd[1] == type);
    TEST_ASSERT(memcmp(cmd + 2, val, val_len) == 0);
}

struct ble_hs_adv_test_field {
    uint8_t type;       /* 0 indicates end of array. */
    uint8_t *val;
    uint8_t val_len;
};

static int
ble_hs_adv_test_misc_calc_data_len(struct ble_hs_adv_test_field *fields)
{
    struct ble_hs_adv_test_field *field;
    int len;

    len = 0;
    if (fields != NULL) {
        for (field = fields; field->type != 0; field++) {
            len += 2 + field->val_len;
        }
    }

    return len;
}

static void
ble_hs_adv_test_misc_verify_tx_fields(uint8_t *cmd,
                                      struct ble_hs_adv_test_field *fields)
{
    struct ble_hs_adv_test_field *field;

    for (field = fields; field->type != 0; field++) {
        ble_hs_adv_test_misc_verify_tx_field(cmd, field->type, field->val_len,
                                             field->val);
        cmd += 2 + field->val_len;
    }
}

static void
ble_hs_adv_test_misc_verify_tx_adv_data(struct ble_hs_adv_test_field *fields)
{
    int data_len;
    uint8_t *cmd;

    cmd = ble_hs_test_util_hci_out_last();
    TEST_ASSERT_FATAL(cmd != NULL);

    data_len = ble_hs_adv_test_misc_calc_data_len(fields);
    ble_hs_adv_test_misc_verify_tx_adv_data_hdr(cmd, data_len);
    if (fields != NULL) {
        ble_hs_adv_test_misc_verify_tx_fields(cmd + BLE_ADV_TEST_DATA_OFF,
                                              fields);
    }
}

static void
ble_hs_adv_test_misc_verify_tx_rsp_data(struct ble_hs_adv_test_field *fields)
{
    int data_len;
    uint8_t *cmd;

    cmd = ble_hs_test_util_hci_out_last();
    TEST_ASSERT_FATAL(cmd != NULL);

    data_len = ble_hs_adv_test_misc_calc_data_len(fields);
    ble_hs_adv_test_misc_verify_tx_rsp_data_hdr(cmd, data_len);
    if (fields != NULL) {
        ble_hs_adv_test_misc_verify_tx_fields(cmd + BLE_ADV_TEST_DATA_OFF,
                                              fields);
    }
}

static void
ble_hs_adv_test_misc_tx_and_verify_data(
    uint8_t disc_mode,
    struct ble_hs_adv_fields *adv_fields,
    struct ble_hs_adv_test_field *test_adv_fields,
    struct ble_hs_adv_fields *rsp_fields,
    struct ble_hs_adv_test_field *test_rsp_fields)
{
    struct ble_gap_adv_params adv_params;
    int rc;

    ble_hs_test_util_init();

    adv_params = ble_hs_test_util_adv_params;
    adv_params.disc_mode = disc_mode;

    rc = ble_hs_test_util_adv_set_fields(adv_fields, 0, 0);
    TEST_ASSERT_FATAL(rc == 0);
    ble_hs_adv_test_misc_verify_tx_adv_data(test_adv_fields);

    if (test_rsp_fields != NULL) {
        rc = ble_hs_test_util_adv_rsp_set_fields(rsp_fields, 0, 0);
        TEST_ASSERT_FATAL(rc == 0);
        ble_hs_adv_test_misc_verify_tx_rsp_data(test_rsp_fields);
    }

    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, &adv_params,
                                    BLE_HS_FOREVER, NULL, NULL, 0, 0);
    TEST_ASSERT_FATAL(rc == 0);

    /* Discard the adv-enable command. */
    ble_hs_test_util_hci_out_last();

    /* Ensure the same data gets sent on repeated advertise procedures. */
    rc = ble_hs_test_util_adv_stop(0);
    TEST_ASSERT_FATAL(rc == 0);

    rc = ble_hs_test_util_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, &adv_params,
                                    BLE_HS_FOREVER, NULL, NULL, 0, 0);
    TEST_ASSERT_FATAL(rc == 0);

    /* Discard the adv-enable command. */
    ble_hs_test_util_hci_out_last();
}

TEST_CASE_SELF(ble_hs_adv_test_case_user)
{
    struct ble_hs_adv_fields adv_fields;
    struct ble_hs_adv_fields rsp_fields;

    memset(&rsp_fields, 0, sizeof rsp_fields);

    /*** Complete 16-bit service class UUIDs. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    adv_fields.uuids16 = (ble_uuid16_t[]) {
        BLE_UUID16_INIT(0x0001),
        BLE_UUID16_INIT(0x1234),
        BLE_UUID16_INIT(0x54ab)
    };
    adv_fields.num_uuids16 = 3;
    adv_fields.uuids16_is_complete = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_COMP_UUIDS16,
                .val = (uint8_t[]) { 0x01, 0x00, 0x34, 0x12, 0xab, 0x54 },
                .val_len = 6,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** Incomplete 16-bit service class UUIDs. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.uuids16 = (ble_uuid16_t[]) {
        BLE_UUID16_INIT(0x0001),
        BLE_UUID16_INIT(0x1234),
        BLE_UUID16_INIT(0x54ab)
    };
    adv_fields.num_uuids16 = 3;
    adv_fields.uuids16_is_complete = 0;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_INCOMP_UUIDS16,
                .val = (uint8_t[]) { 0x01, 0x00, 0x34, 0x12, 0xab, 0x54 },
                .val_len = 6,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** Complete 32-bit service class UUIDs. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.uuids32 = (ble_uuid32_t[]) {
        BLE_UUID32_INIT(0x12345678),
        BLE_UUID32_INIT(0xabacadae)
    };
    adv_fields.num_uuids32 = 2;
    adv_fields.uuids32_is_complete = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_COMP_UUIDS32,
                .val = (uint8_t[]) { 0x78,0x56,0x34,0x12,0xae,0xad,0xac,0xab },
                .val_len = 8,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** Incomplete 32-bit service class UUIDs. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.uuids32 = (ble_uuid32_t[]) {
        BLE_UUID32_INIT(0x12345678),
        BLE_UUID32_INIT(0xabacadae)
    };
    adv_fields.num_uuids32 = 2;
    adv_fields.uuids32_is_complete = 0;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_INCOMP_UUIDS32,
                .val = (uint8_t[]) { 0x78,0x56,0x34,0x12,0xae,0xad,0xac,0xab },
                .val_len = 8,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** Complete 128-bit service class UUIDs. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.uuids128 = (ble_uuid128_t[]) {
        BLE_UUID128_INIT(0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff)
    };
    adv_fields.num_uuids128 = 1;
    adv_fields.uuids128_is_complete = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_COMP_UUIDS128,
                .val = (uint8_t[]) {
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
                },
                .val_len = 16,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** Incomplete 128-bit service class UUIDs. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.uuids128 = BLE_UUID128(BLE_UUID128_DECLARE(
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    ));
    adv_fields.num_uuids128 = 1;
    adv_fields.uuids128_is_complete = 0;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_INCOMP_UUIDS128,
                .val = (uint8_t[]) {
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
                },
                .val_len = 16,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** Complete name. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.name = (uint8_t *)"myname";
    adv_fields.name_len = 6;
    adv_fields.name_is_complete = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_COMP_NAME,
                .val = (uint8_t*)"myname",
                .val_len = 6,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** Incomplete name. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.name = (uint8_t *)"myname";
    adv_fields.name_len = 6;
    adv_fields.name_is_complete = 0;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_INCOMP_NAME,
                .val = (uint8_t*)"myname",
                .val_len = 6,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** Slave interval range. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.slave_itvl_range = (uint8_t[]){ 1,2,3,4 };

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_SLAVE_ITVL_RANGE,
                .val = (uint8_t[]) { 1,2,3,4 },
                .val_len = BLE_HS_ADV_SLAVE_ITVL_RANGE_LEN,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** 0x16 - Service data - 16-bit UUID. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.svc_data_uuid16 = (uint8_t[]){ 1,2,3,4 };
    adv_fields.svc_data_uuid16_len = 4;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_SVC_DATA_UUID16,
                .val = (uint8_t[]) { 1,2,3,4 },
                .val_len = 4,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** 0x17 - Public target address. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.public_tgt_addr = (uint8_t[]){ 1,2,3,4,5,6, 6,5,4,3,2,1 };
    adv_fields.num_public_tgt_addrs = 2;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_PUBLIC_TGT_ADDR,
                .val = (uint8_t[]){ 1,2,3,4,5,6, 6,5,4,3,2,1 },
                .val_len = 2 * BLE_HS_ADV_PUBLIC_TGT_ADDR_ENTRY_LEN,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** 0x19 - Appearance. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.appearance = 0x1234;
    adv_fields.appearance_is_present = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_APPEARANCE,
                .val = (uint8_t[]){ 0x34, 0x12 },
                .val_len = BLE_HS_ADV_APPEARANCE_LEN,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** 0x1a - Advertising interval. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.adv_itvl = 0x1234;
    adv_fields.adv_itvl_is_present = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_ADV_ITVL,
                .val = (uint8_t[]){ 0x34, 0x12 },
                .val_len = BLE_HS_ADV_ADV_ITVL_LEN,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** 0x20 - Service data - 32-bit UUID. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.svc_data_uuid32 = (uint8_t[]){ 1,2,3,4,5 };
    adv_fields.svc_data_uuid32_len = 5;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_SVC_DATA_UUID32,
                .val = (uint8_t[]) { 1,2,3,4,5 },
                .val_len = 5,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** 0x21 - Service data - 128-bit UUID. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.svc_data_uuid128 =
        (uint8_t[]){ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 };
    adv_fields.svc_data_uuid128_len = 18;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_SVC_DATA_UUID128,
                .val = (uint8_t[]){ 1,2,3,4,5,6,7,8,9,10,
                                    11,12,13,14,15,16,17,18 },
                .val_len = 18,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** 0x24 - URI. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.uri = (uint8_t[]){ 1,2,3,4 };
    adv_fields.uri_len = 4;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_URI,
                .val = (uint8_t[]) { 1,2,3,4 },
                .val_len = 4,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** 0xff - Manufacturer specific data. */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;
    adv_fields.mfg_data = (uint8_t[]){ 1,2,3,4 };
    adv_fields.mfg_data_len = 4;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_MFG_DATA,
                .val = (uint8_t[]) { 1,2,3,4 },
                .val_len = 4,
            },
            { 0 },
        }, &rsp_fields, NULL);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_adv_test_case_user_rsp)
{
    struct ble_hs_adv_fields rsp_fields;
    struct ble_hs_adv_fields adv_fields;

    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.flags = BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.tx_pwr_lvl_is_present = 1;

    /*** Complete 16-bit service class UUIDs. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.uuids16 = (ble_uuid16_t[]) {
        BLE_UUID16_INIT(0x0001),
        BLE_UUID16_INIT(0x1234),
        BLE_UUID16_INIT(0x54ab)
    };
    rsp_fields.num_uuids16 = 3;
    rsp_fields.uuids16_is_complete = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_COMP_UUIDS16,
                .val = (uint8_t[]) { 0x01, 0x00, 0x34, 0x12, 0xab, 0x54 },
                .val_len = 6,
            },
            { 0 },
        });

    /*** Incomplete 16-bit service class UUIDs. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.uuids16 = (ble_uuid16_t[]) {
        BLE_UUID16_INIT(0x0001),
        BLE_UUID16_INIT(0x1234),
        BLE_UUID16_INIT(0x54ab)
    };
    rsp_fields.num_uuids16 = 3;
    rsp_fields.uuids16_is_complete = 0;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_INCOMP_UUIDS16,
                .val = (uint8_t[]) { 0x01, 0x00, 0x34, 0x12, 0xab, 0x54 },
                .val_len = 6,
            },
            { 0 },
        });

    /*** Complete 32-bit service class UUIDs. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.uuids32 = (ble_uuid32_t[]) {
        BLE_UUID32_INIT(0x12345678),
        BLE_UUID32_INIT(0xabacadae)
    };
    rsp_fields.num_uuids32 = 2;
    rsp_fields.uuids32_is_complete = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_COMP_UUIDS32,
                .val = (uint8_t[]) { 0x78,0x56,0x34,0x12,0xae,0xad,0xac,0xab },
                .val_len = 8,
            },
            { 0 },
        });

    /*** Incomplete 32-bit service class UUIDs. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.uuids32 = (ble_uuid32_t[]) {
        BLE_UUID32_INIT(0x12345678),
        BLE_UUID32_INIT(0xabacadae)
    };
    rsp_fields.num_uuids32 = 2;
    rsp_fields.uuids32_is_complete = 0;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_INCOMP_UUIDS32,
                .val = (uint8_t[]) { 0x78,0x56,0x34,0x12,0xae,0xad,0xac,0xab },
                .val_len = 8,
            },
            { 0 },
        });

    /*** Complete 128-bit service class UUIDs. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.uuids128 = (ble_uuid128_t[]) {
        BLE_UUID128_INIT(0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff)
    };
    rsp_fields.num_uuids128 = 1;
    rsp_fields.uuids128_is_complete = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_COMP_UUIDS128,
                .val = (uint8_t[]) {
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
                },
                .val_len = 16,
            },
            { 0 },
        });

    /*** Incomplete 128-bit service class UUIDs. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.uuids128 = (ble_uuid128_t[]) {
        BLE_UUID128_INIT(0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff)
    };
    rsp_fields.num_uuids128 = 1;
    rsp_fields.uuids128_is_complete = 0;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_INCOMP_UUIDS128,
                .val = (uint8_t[]) {
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
                },
                .val_len = 16,
            },
            { 0 },
        });

    /*** Complete name. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.name = (uint8_t *)"myname";
    rsp_fields.name_len = 6;
    rsp_fields.name_is_complete = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_COMP_NAME,
                .val = (uint8_t*)"myname",
                .val_len = 6,
            },
            { 0 },
        });

    /*** Incomplete name. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.name = (uint8_t *)"myname";
    rsp_fields.name_len = 6;
    rsp_fields.name_is_complete = 0;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_INCOMP_NAME,
                .val = (uint8_t*)"myname",
                .val_len = 6,
            },
            { 0 },
        });

    /*** Slave interval range. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.slave_itvl_range = (uint8_t[]){ 1,2,3,4 };

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_SLAVE_ITVL_RANGE,
                .val = (uint8_t[]) { 1,2,3,4 },
                .val_len = BLE_HS_ADV_SLAVE_ITVL_RANGE_LEN,
            },
            { 0 },
        });

    /*** 0x16 - Service data - 16-bit UUID. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.svc_data_uuid16 = (uint8_t[]){ 1,2,3,4 };
    rsp_fields.svc_data_uuid16_len = 4;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_SVC_DATA_UUID16,
                .val = (uint8_t[]) { 1,2,3,4 },
                .val_len = 4,
            },
            { 0 },
        });

    /*** 0x17 - Public target address. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.public_tgt_addr = (uint8_t[]){ 1,2,3,4,5,6, 6,5,4,3,2,1 };
    rsp_fields.num_public_tgt_addrs = 2;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_PUBLIC_TGT_ADDR,
                .val = (uint8_t[]){ 1,2,3,4,5,6, 6,5,4,3,2,1 },
                .val_len = 2 * BLE_HS_ADV_PUBLIC_TGT_ADDR_ENTRY_LEN,
            },
            { 0 },
        });

    /*** 0x19 - Appearance. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.appearance = 0x1234;
    rsp_fields.appearance_is_present = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_APPEARANCE,
                .val = (uint8_t[]){ 0x34, 0x12 },
                .val_len = BLE_HS_ADV_APPEARANCE_LEN,
            },
            { 0 },
        });

    /*** 0x1a - Advertising interval. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.adv_itvl = 0x1234;
    rsp_fields.adv_itvl_is_present = 1;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_ADV_ITVL,
                .val = (uint8_t[]){ 0x34, 0x12 },
                .val_len = BLE_HS_ADV_ADV_ITVL_LEN,
            },
            { 0 },
        });

    /*** 0x20 - Service data - 32-bit UUID. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.svc_data_uuid32 = (uint8_t[]){ 1,2,3,4,5 };
    rsp_fields.svc_data_uuid32_len = 5;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_SVC_DATA_UUID32,
                .val = (uint8_t[]) { 1,2,3,4,5 },
                .val_len = 5,
            },
            { 0 },
        });

    /*** 0x21 - Service data - 128-bit UUID. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.svc_data_uuid128 =
        (uint8_t[]){ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 };
    rsp_fields.svc_data_uuid128_len = 18;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_SVC_DATA_UUID128,
                .val = (uint8_t[]){ 1,2,3,4,5,6,7,8,9,10,
                                    11,12,13,14,15,16,17,18 },
                .val_len = 18,
            },
            { 0 },
        });

    /*** 0x24 - URI. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.uri = (uint8_t[]){ 1,2,3,4 };
    rsp_fields.uri_len = 4;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_URI,
                .val = (uint8_t[]) { 1,2,3,4 },
                .val_len = 4,
            },
            { 0 },
        });

    /*** 0xff - Manufacturer specific data. */
    memset(&rsp_fields, 0, sizeof rsp_fields);
    rsp_fields.mfg_data = (uint8_t[]){ 1,2,3,4 };
    rsp_fields.mfg_data_len = 4;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_FLAGS,
                .val = (uint8_t[]){ BLE_HS_ADV_F_BREDR_UNSUP },
                .val_len = 1,
            },
            {
                .type = BLE_HS_ADV_TYPE_TX_PWR_LVL,
                .val = (uint8_t[]){ 0 },
                .val_len = 1,
            },
            { 0 },
        },
        &rsp_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_MFG_DATA,
                .val = (uint8_t[]) { 1,2,3,4 },
                .val_len = 4,
            },
            { 0 },
        });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_hs_adv_test_case_user_full_payload)
{
    /* Intentionally allocate an extra byte. */
    static const uint8_t mfg_data[30] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
    };

    struct ble_hs_adv_fields adv_fields;
    struct ble_hs_adv_fields rsp_fields;
    int rc;

    ble_hs_test_util_init();

    memset(&rsp_fields, 0, sizeof rsp_fields);

    /***
     * An advertisement should allow 31 bytes of user data.  Each field has a
     * two-byte header, leaving 29 bytes of payload.
     */
    memset(&adv_fields, 0, sizeof adv_fields);
    adv_fields.mfg_data = (void *)mfg_data;
    adv_fields.mfg_data_len = 29;

    ble_hs_adv_test_misc_tx_and_verify_data(BLE_GAP_DISC_MODE_NON, &adv_fields,
        (struct ble_hs_adv_test_field[]) {
            {
                .type = BLE_HS_ADV_TYPE_MFG_DATA,
                .val = (void *)mfg_data,
                .val_len = 29,
            },
            { 0 },
        }, &rsp_fields, NULL);

    /*** Fail with 30 bytes. */
    rc = ble_hs_test_util_adv_stop(0);
    TEST_ASSERT_FATAL(rc == 0);

    adv_fields.mfg_data_len = 30;
    rc = ble_gap_adv_set_fields(&adv_fields);
    TEST_ASSERT(rc == BLE_HS_EMSGSIZE);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_hs_adv_test_suite)
{
    ble_hs_adv_test_case_user();
    ble_hs_adv_test_case_user_rsp();
    ble_hs_adv_test_case_user_full_payload();
}
