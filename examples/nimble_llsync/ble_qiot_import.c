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

// #include "nrf52832_xxaa_service.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "ble_qiot_import.h"

// #include "host/ble_hs.h"
// #include "host/ble_uuid.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"

#include "flash_storage.h"
#include "ble_qiot_log.h"


// divece configs
// this only for reference, please create your own device and replace it!
// https://cloud.tencent.com/product/iotexplorer
// #define PRODUCT_ID  "PRODUCT_ID"               // 10 Bytes
// #define DEVICE_NAME "YOUR_DEVICE_NAME"         // equal or lesser than 48 Bytes
// #define SECRET_KEY  "YOUR_PRODUCT_SECRET_KEY_" // 24 Bytes

#define ADV_DEVICE_NAME "tos_llsync"


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

static ble_uuid128_t ble_uuid128_t_service = BLE_UUID128_INIT(IOT_BLE_UUID128_SERVICE);
static ble_uuid128_t ble_uuid128_t_device_info = BLE_UUID128_INIT(IOT_BLE_UUID128_DEVICE_INFO);
static ble_uuid128_t ble_uuid128_t_data = BLE_UUID128_INIT(IOT_BLE_UUID128_DATA);
static ble_uuid128_t ble_uuid128_t_event = BLE_UUID128_INIT(IOT_BLE_UUID128_EVENT);

static uint16_t qiot_gap_connect_handle;

static uint16_t qiot_device_info_handle;
static uint16_t qiot_data_handle;
static uint16_t qiot_event_handle;

static int ble_svc_qiot_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

// LLsync BLE services and characteristics
static const struct ble_gatt_svc_def ble_svc_qiot_defs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &ble_uuid128_t_service.u,
        // .uuid = BLE_UUID16_DECLARE(IOT_BLE_UUID_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = &ble_uuid128_t_device_info.u,
                // .uuid = BLE_UUID16_DECLARE(IOT_BLE_UUID_DEVICE_INFO),
                .access_cb = ble_svc_qiot_access,
                // .val_handle = &qiot_device_info_handle,
                // .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE_NO_RSP,
                .flags = BLE_GATT_CHR_F_WRITE_NO_RSP,
                // .flags = 0xFFF,
            },
            {
                .uuid = &ble_uuid128_t_data.u,
                // .uuid = BLE_UUID16_DECLARE(IOT_BLE_UUID_DATA),
                .access_cb = ble_svc_qiot_access,
                // .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE_NO_RSP,
                .flags = BLE_GATT_CHR_F_WRITE_NO_RSP,
            },
            {
                .uuid = &ble_uuid128_t_event.u,
                // .uuid = BLE_UUID16_DECLARE(IOT_BLE_UUID_EVENT),
                .access_cb = ble_svc_qiot_access,
                .val_handle = &qiot_event_handle,
                .flags = BLE_GATT_CHR_F_NOTIFY,
            },

            // end of characteristics
            {.uuid = 0},
        },
    },

    // end of services
    {.type = 0},
};

static int ble_svc_qiot_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    const ble_uuid_t *uuid;
    // int rand_num;
    int rc, ret;
    uint16_t len = 0;
    uint8_t buf[256] = {0};

    uuid = ctxt->chr->uuid;

    if(ble_uuid_cmp(uuid, &ble_uuid128_t_device_info.u) == 0)
    {
        switch (ctxt->op)
        {
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                rc = ble_hs_mbuf_to_flat(ctxt->om, buf, sizeof(buf), &len);
                if (rc != 0)
                {
                    ble_qiot_log_e("ble att err, ret: %d", ret);
                    return BLE_ATT_ERR_UNLIKELY;
                }
                ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_DEBUG, "device info raw", (char *)buf, len);
                ble_device_info_write_cb(buf, len);
            default:
                return BLE_ATT_ERR_UNLIKELY;
        }
    }
    else if(ble_uuid_cmp(uuid, &ble_uuid128_t_data.u) == 0)
    {
        switch (ctxt->op)
        {
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                rc = ble_hs_mbuf_to_flat(ctxt->om, buf, sizeof(buf), &len);
                if (rc != 0)
                {
                    ble_qiot_log_e("ble att err, ret: %d", ret);
                    return BLE_ATT_ERR_UNLIKELY;
                }
                ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_DEBUG, "lldata raw", (char *)buf, len);
                ble_lldata_write_cb(buf, len);
            default:
                return BLE_ATT_ERR_UNLIKELY;
        }
    }
    else if(ble_uuid_cmp(uuid, &ble_uuid128_t_event.u) == 0)
    {
        // this characteristic not have read or write attribute
    }
    else
    {
        ble_qiot_log_e("unknown service or characteristic");
    }

    return 0;
}

// add services and characteristic
void ble_services_add(const qiot_service_init_s *p_service)
{
    int ret = 0;

    ret = ble_gatts_count_cfg(ble_svc_qiot_defs);
    if(ret != 0)
    {
        ble_qiot_log_e("ble_gatts_count_cfg() err, ret: %d", ret);
    }

    ret = ble_gatts_add_svcs(ble_svc_qiot_defs);
    if(ret != 0)
    {
        ble_qiot_log_e("ble_gatts_add_svcs() err, ret: %d", ret);
    }

    return;
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
    memcpy(mac, MYNEWT_VAL_BLE_PUBLIC_DEV_ADDR, sizeof(MYNEWT_VAL_BLE_PUBLIC_DEV_ADDR));
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_DEBUG, "mac_addr", mac, 6);

    return 0;
}

int ble_write_flash(uint32_t flash_addr, const char *write_buf, uint16_t write_len)
{
	return fstorage_write(flash_addr, write_len, write_buf);
}

int ble_read_flash(uint32_t flash_addr, char *read_buf, uint16_t read_len)
{
	return fstorage_read(flash_addr, read_len, read_buf);
}

int ble_qiot_gap_event_fn(struct ble_gap_event *event, void *arg)
{
    ble_qiot_log_d("get event, type: %d", event->type);

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed */
        MODLOG_DFLT(INFO, "connection %s; status=%d\n",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);

        if (event->connect.status != 0) {
            /* Connection failed; resume advertising */
            ble_qiot_advertising_start();
            qiot_gap_connect_handle = 0;
        }
        else
        {
            qiot_gap_connect_handle = event->connect.conn_handle;
        }
        
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d\n", event->disconnect.reason);

        /* Connection terminated; resume advertising */
        ble_qiot_advertising_start();
        qiot_gap_connect_handle = 0;
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "adv complete\n");
        ble_qiot_advertising_start();
        break;

    // case BLE_GAP_EVENT_SUBSCRIBE:
    //     MODLOG_DFLT(INFO, "subscribe event; cur_notify=%d\n value handle; "
    //                       "val_handle=%d\n",
    //                 event->subscribe.cur_notify, hrs_hrm_handle);
    //     if (event->subscribe.attr_handle == hrs_hrm_handle) {
    //         notify_state = event->subscribe.cur_notify;
    //         notify_conn_handle = event->subscribe.conn_handle;
    //         blehr_tx_hrate_reset();
    //     } else if (event->subscribe.attr_handle != hrs_hrm_handle) {
    //         notify_state = event->subscribe.cur_notify;
    //         notify_conn_handle = 0;
    //         blehr_tx_hrate_stop();
    //     }
    //     break;

    case BLE_GAP_EVENT_MTU:
        MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.value);
        break;

    }

    return 0;
}


#define ADV_MAX_UUID16_NUM 15
static uint8_t ble_addr_type;

ble_qiot_ret_status_t ble_advertising_start(adv_info_s *adv)
{
    int ret = 0, i = 0;
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    struct ble_hs_adv_fields rsp_fields;
    uint8_t adv_manufacture_data[32] = {0};
    ble_uuid16_t adv_uuids16_list[ADV_MAX_UUID16_NUM];

    memset(&fields, 0, sizeof(fields));
    memset(&rsp_fields, 0, sizeof(rsp_fields));

    // set advertising data
    /* Advertise two flags:
     *     o Discoverability in forthcoming advertisement (general)
     *     o BLE-only (BR/EDR unsupported).
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    // fields.uuids16 = (ble_uuid16_t)adv->uuid_info.uuids;??
    if (adv->uuid_info.uuid_num > ADV_MAX_UUID16_NUM)
    {
        printf("err: too many uuids16 in adv data! (uuids num %d)\r\n", adv->uuid_info.uuid_num);
        return BLE_QIOT_RS_ERR;
    }

    for (i=0; i<adv->uuid_info.uuid_num; i++)
    {
        adv_uuids16_list[i].u.type = BLE_UUID_TYPE_16;
        adv_uuids16_list[i].value = adv->uuid_info.uuids[i];
    }
    fields.uuids16 = adv_uuids16_list;
    fields.num_uuids16 = adv->uuid_info.uuid_num;
    fields.uuids16_is_complete = 1;

    memcpy(&adv_manufacture_data[0], (uint8_t *)&adv->manufacturer_info.company_identifier, 2);
    memcpy(&adv_manufacture_data[2], adv->manufacturer_info.adv_data, adv->manufacturer_info.adv_data_len);
    fields.mfg_data = adv_manufacture_data;
    fields.mfg_data_len = adv->manufacturer_info.adv_data_len + 2;

    ret = ble_gap_adv_set_fields(&fields);
    if (ret != 0)
    {
        ble_qiot_log_e("ble_gap_adv_set_fields() err, ret: %d", ret);
        return BLE_QIOT_RS_ERR;
    }

    // set advertising respond data
    rsp_fields.name = ADV_DEVICE_NAME;
    rsp_fields.name_len = strlen(ADV_DEVICE_NAME);
    rsp_fields.name_is_complete = 1;
    ret = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if(ret != 0)
    {
        ble_qiot_log_e("ble_gap_adv_rsp_set_fields() err, ret: %d", ret);
        return BLE_QIOT_RS_ERR;
    }

    ret = ble_hs_id_infer_auto(0, &ble_addr_type);
    if (ret != 0)
    {
        ble_qiot_log_e("ble_hs_id_infer_auto() err, ret: %d", ret);
        return BLE_QIOT_RS_ERR;
    }

    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ret = ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_qiot_gap_event_fn, NULL);
    if (ret != 0)
    {
        ble_qiot_log_e("ble_gap_adv_start() err, ret: %d", ret);
        return BLE_QIOT_RS_ERR;
    }

    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_advertising_stop(void)
{
    int ret = 0;

    ble_gap_adv_stop();
    if(ret != 0)
    {
        ble_qiot_log_e("ble_gap_adv_stop() err, ret: %d", ret);
        return BLE_QIOT_RS_ERR;
    }

    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_send_notify(uint8_t *buf, uint8_t len)
{
    struct os_mbuf *om;
    int rc = 0;

    om = ble_hs_mbuf_from_flat(buf, len);

    rc = ble_gattc_notify_custom(qiot_gap_connect_handle, qiot_event_handle, om);
    if(rc)
    {
        ble_qiot_log_e("send notify err, return %d", rc);
        return BLE_QIOT_RS_ERR;
    }
    
    return BLE_QIOT_RS_OK;
}

ble_timer_t ble_timer_create(uint8_t type, ble_timer_cb timeout_handle)
{
    return (ble_timer_t)NULL;
}

ble_qiot_ret_status_t ble_timer_start(ble_timer_t timer_id, uint32_t period)
{
    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_timer_stop(ble_timer_t timer_id)
{
    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_timer_delete(ble_timer_t timer_id)
{
    return BLE_QIOT_RS_OK;
}

// should return ATT_MTU - 3
uint16_t ble_get_user_data_mtu_size(void)
{
    // this is an example, the default mtu is 23, user should get real mtu and return it
    int mtu = 23;

    return mtu - 3;
}
