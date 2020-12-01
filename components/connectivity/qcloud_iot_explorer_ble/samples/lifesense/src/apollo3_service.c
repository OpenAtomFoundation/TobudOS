/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include "apollo3_service.h"

#include <stdint.h>
#include <string.h>

#include "SEGGER_RTT.h"

#include "wsf_types.h"
#include "att_api.h"
#include "app_api.h"
#include "watch_api.h"

#include "ble_qiot_service.h"
#include "ble_qiot_import.h"
#include "ble_qiot_config.h"



// divece configs
#define PRODUCT_ID  "LR5NRSX9F5"
#define DEVICE_NAME "ble01"
#define SECRET_KEY  "uG2TRl0hnCNsuLgls/65Bg=="

#define ADV_DEVICE_NAME "life_sense"

#define ATTRIBUTE_VALUE_MAX_LEN 256


#define IOT_BLE_UUID_BASE96      0xe2, 0xa4, 0x1b, 0x54, 0x93, 0xe4, 0x6a, 0xb5, \
                                 0x20, 0x4e, 0xd0, 0x65

#define UINT16_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8))
#define UINT32_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8)), ((uint8_t)((n) >> 16)), ((uint8_t)((n) >> 24))

#define UINT16_TO_UUID32(n)         ((uint8_t) (n)), ((uint8_t)((n) >> 8)), ((uint8_t)((n) >> 16)), ((uint8_t)((n) >> 24))
#define IOT_BLE_UUID128_BUILD(uuid) IOT_BLE_UUID_BASE96, UINT16_TO_UUID32(uuid)

#define IOT_BLE_UUID128_SERVICE     IOT_BLE_UUID128_BUILD(IOT_BLE_UUID_SERVICE)
#define IOT_BLE_UUID128_DEVICE_INFO IOT_BLE_UUID128_BUILD(IOT_BLE_UUID_DEVICE_INFO)
#define IOT_BLE_UUID128_DATA        IOT_BLE_UUID128_BUILD(IOT_BLE_UUID_DATA)
#define IOT_BLE_UUID128_EVENT       IOT_BLE_UUID128_BUILD(IOT_BLE_UUID_EVENT)

uint8_t iot_service_uuid[] = {IOT_BLE_UUID128_SERVICE};
uint16_t iot_service_uuid_len = sizeof(iot_service_uuid);

uint8_t service_uuid[] = {UINT16_TO_BYTES(IOT_BLE_UUID_SERVICE)};


// qiot device info characteristic and attribute
// uint8_t device_info_rx_char[] = {(ATT_PROP_READ | ATT_PROP_WRITE_NO_RSP | ATT_PROP_WRITE), UINT16_TO_BYTES(QIOT_DEVICE_INFO_HDL), IOT_BLE_UUID128_DEVICE_INFO};
uint8_t device_info_rx_char[] = {(ATT_PROP_WRITE_NO_RSP), UINT16_TO_BYTES(QIOT_DEVICE_INFO_HDL), IOT_BLE_UUID128_DEVICE_INFO};
uint16_t device_info_rx_char_len = sizeof(device_info_rx_char);

uint8_t device_info_uuid[] = {IOT_BLE_UUID128_DEVICE_INFO};
uint8_t device_info_attr_value[ATTRIBUTE_VALUE_MAX_LEN] = {0};
uint16_t device_info_attr_value_len = sizeof(device_info_attr_value);


// qiot data characteristic and attribute
// uint8_t data_rx_char[] = {(ATT_PROP_READ | ATT_PROP_WRITE_NO_RSP | ATT_PROP_WRITE), UINT16_TO_BYTES(QIOT_DATA_HDL), IOT_BLE_UUID128_DATA};
uint8_t data_rx_char[] = {(ATT_PROP_WRITE_NO_RSP), UINT16_TO_BYTES(QIOT_DATA_HDL), IOT_BLE_UUID128_DATA};
uint16_t data_rx_char_len = sizeof(data_rx_char);

uint8_t data_uuid[] = {IOT_BLE_UUID128_DATA};
uint8_t data_attr_value[ATTRIBUTE_VALUE_MAX_LEN] = {0};
uint16_t data_attr_value_len = sizeof(data_attr_value);


// qiot event characteristic and attribute
// uint8_t event_rx_char[] = {(ATT_PROP_READ | ATT_PROP_INDICATE | ATT_PROP_NOTIFY), UINT16_TO_BYTES(QIOT_EVENT_HDL), IOT_BLE_UUID128_EVENT};
// uint8_t event_rx_char[] = {(ATT_PROP_INDICATE), UINT16_TO_BYTES(QIOT_EVENT_HDL), IOT_BLE_UUID128_EVENT};
uint8_t event_rx_char[] = {(ATT_PROP_NOTIFY), UINT16_TO_BYTES(QIOT_EVENT_HDL), IOT_BLE_UUID128_EVENT};
uint16_t event_rx_char_len = sizeof(event_rx_char);

uint8_t event_uuid[] = {IOT_BLE_UUID128_EVENT};
uint8_t event_attr_value[ATTRIBUTE_VALUE_MAX_LEN] = {0};
uint16_t event_attr_value_len = sizeof(event_attr_value);

uint8_t event_cccd[] = {0x00, 0x00};
uint16_t event_cccd_len = sizeof(event_cccd);

/* Attribute list for AMDTP group */
static const attsAttr_t qiot_attr_list[] =
{
    {
        attPrimSvcUuid,
        iot_service_uuid,
        &iot_service_uuid_len,
        sizeof(iot_service_uuid),
        0,
        ATTS_PERMIT_READ,
    },


    // qiot device info characteristic and attribute
    {
        attChUuid,
        device_info_rx_char,
        &device_info_rx_char_len,
        sizeof(device_info_rx_char),
        0,
        ATTS_PERMIT_READ
    },
    {
        device_info_uuid,
        device_info_attr_value,
        &device_info_attr_value_len,
        sizeof(device_info_attr_value),
        (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
        // (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE),
        ATTS_PERMIT_WRITE,
    },


    // qiot data characteristic and attribute
    {
        attChUuid,
        data_rx_char,
        &data_rx_char_len,
        sizeof(data_rx_char),
        0,
        ATTS_PERMIT_READ
    },
    {
        data_uuid,
        data_attr_value,
        &data_attr_value_len,
        sizeof(data_attr_value),
        (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
        // (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE),
        ATTS_PERMIT_WRITE,
    },


    // qiot event characteristic and attribute
    {
        attChUuid,
        event_rx_char,
        &event_rx_char_len,
        sizeof(event_rx_char),
        0,
        ATTS_PERMIT_READ
    },
    {
        event_uuid,
        event_attr_value,
        &event_attr_value_len,
        sizeof(event_attr_value),
        // (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_WRITE_CBACK),
        // (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE),
        0,
        0,
    },
    {
        attCliChCfgUuid,
        (uint8_t *) event_cccd,
        (uint16_t *) &event_cccd_len,
        sizeof(event_cccd),
        ATTS_SET_CCC,
        (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)
    },
};


static attsGroup_t qiot_attr_group =
{
    NULL,
    (attsAttr_t *) &qiot_attr_list,
    NULL,
    NULL,
    QIOT_START_HDL,
    QIOT_END_HDL,
};

uint8_t qiot_write_cback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                           uint16_t offset, uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{
// SEGGER_RTT_printf(0, "id %d, hd %d, op %d, of %d \r\n", connId, handle, operation, offset);
// if(pAttr != NULL)
// {
//     SEGGER_RTT_printf(0, "uuid %d %d, ml %d, set %d, per %d \r\n", 
//     pAttr->pUuid[0],
//     pAttr->pUuid[1],
//     pAttr->maxLen,
//     pAttr->settings,
//     pAttr->permissions);
// }

    switch (handle)
    {
    case QIOT_DEVICE_INFO_HDL:
        ble_device_info_write_cb(pValue, len);
        break;
    case QIOT_DATA_HDL:
        ble_lldata_write_cb(pValue, len);
        break;
    case QIOT_EVENT_HDL:
        break;
    default:
        break;
    }
    return ATT_SUCCESS;
}

void ble_services_add(const qiot_service_init_s *p_service)
{
    qiot_attr_group.writeCback = qiot_write_cback;
    AttsAddGroup(&qiot_attr_group);
    // return 0;
}

int ble_get_product_id(char *product_id)
{
    memcpy(product_id, PRODUCT_ID, strlen(PRODUCT_ID));
    return 0;
}

int ble_get_device_name(char *device_name)
{
    memcpy(device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    return strlen(DEVICE_NAME);
}

int ble_get_psk(char *psk)
{
    memcpy(psk, SECRET_KEY, strlen(SECRET_KEY));
    return 0;
}

int ble_get_mac(char *mac)
{
    mac[0] = 0x99;
    mac[1] = 0x98;
    mac[2] = 0x97;
    mac[3] = 0x96;
    mac[4] = 0x95;
    mac[5] = 0x94;

    return 0;
}

// eeprom simulation
uint8_t eeprom[256] = {0};

int ble_write_flash(uint32_t flash_addr, const char *write_buf, uint16_t write_len)
{
    memcpy(eeprom, write_buf, write_len);
    return write_len;
}

int ble_read_flash(uint32_t flash_addr, char *read_buf, uint16_t read_len)
{
    memcpy(read_buf, eeprom, read_len);
    return read_len;
}

ble_qiot_ret_status_t ble_advertising_start(adv_info_s *adv)
{
    uint8_t adv_data[31] = {0};
    uint8_t scan_data[31] = {0};
    int adv_p = 0;
    int scan_p = 0;

    // adv flag
    adv_data[adv_p++] = 2;// length
    adv_data[adv_p++] = DM_ADV_TYPE_FLAGS;// AD type
    adv_data[adv_p++] = DM_FLAG_LE_GENERAL_DISC | DM_FLAG_LE_BREDR_NOT_SUP;// flags

    // adv deivce name
    // adv_data[adv_p++] = strlen(ADV_DEVICE_NAME) + 1; // length
    // adv_data[adv_p++] = DM_ADV_TYPE_LOCAL_NAME;
    // strncpy(&adv_data[adv_p], ADV_DEVICE_NAME, 31 - adv_p);
    // adv_p += strlen(ADV_DEVICE_NAME);

    scan_data[scan_p++] = strlen(ADV_DEVICE_NAME) + 1; // length
    scan_data[scan_p++] = DM_ADV_TYPE_LOCAL_NAME;
    strncpy((char *)&scan_data[scan_p], ADV_DEVICE_NAME, 31 - scan_p);
    scan_p += strlen(ADV_DEVICE_NAME);

    // Complete list of 16 bit UUIDs
    adv_data[adv_p++] = 3;
    adv_data[adv_p++] = DM_ADV_TYPE_16_UUID;
    adv_data[adv_p++] = (uint8_t)adv->uuid_info.uuids[0];
    adv_data[adv_p++] = (uint8_t)(adv->uuid_info.uuids[0] >> 8);

    // adv manufacture data
    adv_data[adv_p++] = adv->manufacturer_info.adv_data_len + 3;
    adv_data[adv_p++] = DM_ADV_TYPE_MANUFACTURER;
    adv_data[adv_p++] = (uint8_t)adv->manufacturer_info.company_identifier;
    adv_data[adv_p++] = (uint8_t)(adv->manufacturer_info.company_identifier >> 8);
    memcpy(&adv_data[adv_p], adv->manufacturer_info.adv_data, adv->manufacturer_info.adv_data_len);
    adv_p += adv->manufacturer_info.adv_data_len;

    // HexDump("adv", adv_data, sizeof(adv_data));
    // AppAdvSetAdValue();

    /* set advertising and scan response data for discoverable mode */
    AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, adv_p, (uint8_t *) adv_data);
    // AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, sizeof(watchScanDataDisc), (uint8_t *) watchScanDataDisc);
    AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, scan_p, (uint8_t *)scan_data);

    /* set advertising and scan response data for connectable mode */
    AppAdvSetData(APP_ADV_DATA_CONNECTABLE, 0, NULL);
    AppAdvSetData(APP_SCAN_DATA_CONNECTABLE, 0, NULL);

    /* start advertising; automatically set connectable/discoverable mode and bondable mode */
    AppAdvStart(APP_MODE_AUTO_INIT);

    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_advertising_stop(void)
{
    AppAdvStop();

    return BLE_QIOT_RS_OK;
}

dmConnId_t ind_conn_id = DM_CONN_ID_NONE;

ble_qiot_ret_status_t ble_send_notify(uint8_t *buf, uint8_t len)
{
    AttsHandleValueNtf(ind_conn_id, QIOT_EVENT_HDL, len, buf);
    return BLE_QIOT_RS_OK;
}


static wsfTimer_t ble_qiot_timer;
ble_timer_cb p_ble_timer_cb = NULL;

ble_timer_t ble_timer_create(uint8_t type, ble_timer_cb timeout_handle)
{
    ble_qiot_timer.handlerId = get_wsf_evt_handler();
    p_ble_timer_cb = timeout_handle;
    return (ble_timer_t)&ble_qiot_timer;
}

ble_qiot_ret_status_t ble_timer_start(ble_timer_t timer_id, uint32_t period)
{
    WsfTimerStartMs((wsfTimer_t *)timer_id, period);
    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_timer_stop(ble_timer_t timer_id)
{
    WsfTimerStop((wsfTimer_t *)timer_id);
    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_timer_delete(ble_timer_t timer_id)
{
    return BLE_QIOT_RS_OK;
}

// should return ATT_MTU - 3
uint16_t ble_get_user_data_mtu_size(void)
{
    return AttGetMtu(ind_conn_id) - 3;
}

