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
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "ble_qiot_config.h"
#include "ble_qiot_export.h"
#include "ble_qiot_service.h"
#include "ble_qiot_import.h"

#define LLSYNC_LOG_TAG "LLSYNC"

/* Attributes State Machine */
enum
{
  IDX_SVC,
  IDX_CHAR_A,
  IDX_CHAR_VAL_A,

  IDX_CHAR_B,
  IDX_CHAR_VAL_B,

  IDX_CHAR_C,
  IDX_CHAR_VAL_C,
  IDX_CHAR_CFG_C,

  HRS_IDX_NB,
};

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define LLSYNC_APP_ID               0x55
#define SAMPLE_DEVICE_NAME          "l"
#define SVC_INST_ID                 0

/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than LLSYNC_CHAR_VAL_LEN_MAX.
*/
#define LLSYNC_CHAR_VAL_LEN_MAX     500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

#define ADV_CONFIG_FLAG             (1 << 0)

static uint8_t adv_config_done       = 0;

uint16_t llsync_handle_table[HRS_IDX_NB];

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
    uint16_t handle;
} prepare_type_env_t;

static prepare_type_env_t prepare_write_env;

static uint8_t raw_adv_data[32] = {
        /* flags */
        0x02, 0x01, 0x06,
        /* service uuid */
        0x03, 0x03, 0xE0, 0xFF,
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min         = 0x20,
    .adv_int_max         = 0x40,
    .adv_type            = ADV_TYPE_IND,
    .own_addr_type       = BLE_ADDR_TYPE_PUBLIC,
    .channel_map         = ADV_CHNL_ALL,
    .adv_filter_policy   = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

ble_qiot_ret_status_t ble_advertising_start(adv_info_s *adv)
{
    uint8_t usr_adv_data[31] = {0};
    uint8_t len = 0;
    uint8_t index = 0;

    memcpy(usr_adv_data, &adv->manufacturer_info.company_identifier, sizeof(uint16_t));
    len = sizeof(uint16_t);
    memcpy(usr_adv_data + len, adv->manufacturer_info.adv_data, adv->manufacturer_info.adv_data_len);
    len += adv->manufacturer_info.adv_data_len;

    index = 7;
    raw_adv_data[index++] = len + 1;
    raw_adv_data[index++] = 0xFF;
    memcpy(raw_adv_data + index, usr_adv_data, len);
    index += len;

    raw_adv_data[index++] = strlen(SAMPLE_DEVICE_NAME) + 1;
    raw_adv_data[index++] = 0x09;
    memcpy(raw_adv_data + index, SAMPLE_DEVICE_NAME, strlen(SAMPLE_DEVICE_NAME));
    index += strlen(SAMPLE_DEVICE_NAME);

    esp_log_buffer_hex("adv", raw_adv_data, index);
    //config adv data
    esp_err_t ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, index);
    if (ret) {
        ESP_LOGE(LLSYNC_LOG_TAG, "config adv data failed, error code = %x", ret);
    }
    adv_config_done |= ADV_CONFIG_FLAG;

    ESP_LOGI(LLSYNC_LOG_TAG, "start advertising");
    esp_ble_gap_start_advertising(&adv_params);

    return 0;
}

ble_qiot_ret_status_t ble_advertising_stop(void)
{
    esp_ble_gap_stop_advertising();
    return 0;
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst llsync_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

ble_qiot_ret_status_t ble_send_notify(uint8_t *buf, uint8_t len)
{
    esp_ble_gatts_send_indicate(llsync_profile_tab[PROFILE_APP_IDX].gatts_if, llsync_profile_tab[PROFILE_APP_IDX].conn_id,
            llsync_handle_table[IDX_CHAR_VAL_C],len, buf, false);
    return BLE_QIOT_RS_OK;
}

static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_write               = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_notify              = ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t heart_measurement_ccc[2]      = {0x00, 0x00};

/* service_uuid and Characteristic_uuid*/
static uint8_t llsync_service_uuid[16] = {
        0xe2, 0xa4, 0x1b, 0x54, 0x93, 0xe4, 0x6a, 0xb5, 0x20, 0x4e, 0xd0, 0x65, 0xe0, 0xff, 0x00, 0x00,
};
static uint8_t llsync_device_info_uuid[16] = {
        0xe2, 0xa4, 0x1b, 0x54, 0x93, 0xe4, 0x6a, 0xb5, 0x20, 0x4e, 0xd0, 0x65, 0xe1, 0xff, 0x00, 0x00,
};
static uint8_t llsync_data_uuid[16] = {
        0xe2, 0xa4, 0x1b, 0x54, 0x93, 0xe4, 0x6a, 0xb5, 0x20, 0x4e, 0xd0, 0x65, 0xe2, 0xff, 0x00, 0x00,
};
static uint8_t llsync_event_uuid[16] = {
        0xe2, 0xa4, 0x1b, 0x54, 0x93, 0xe4, 0x6a, 0xb5, 0x20, 0x4e, 0xd0, 0x65, 0xe3, 0xff, 0x00, 0x00,
};


/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] =
    {
        // Service Declaration
        [IDX_SVC]        =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
                sizeof(llsync_service_uuid), sizeof(llsync_service_uuid), (uint8_t *)llsync_service_uuid}},

        /* Characteristic Declaration */
        [IDX_CHAR_A]     =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},
        /* Characteristic Value */
        [IDX_CHAR_VAL_A] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)llsync_device_info_uuid, ESP_GATT_PERM_WRITE,
                LLSYNC_CHAR_VAL_LEN_MAX, 0, NULL}},

        /* Characteristic Declaration */
        [IDX_CHAR_B]      =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},
        /* Characteristic Value */
        [IDX_CHAR_VAL_B]  =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)llsync_data_uuid, ESP_GATT_PERM_WRITE,
                LLSYNC_CHAR_VAL_LEN_MAX, 0, NULL}},

        /* Characteristic Declaration */
        [IDX_CHAR_C]      =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_notify}},
        /* Characteristic Value */
        [IDX_CHAR_VAL_C]  =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)llsync_event_uuid, 0,
                LLSYNC_CHAR_VAL_LEN_MAX, 0, NULL}},
        /* Characteristic User Descriptor */
        [IDX_CHAR_CFG_C]  =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},
    };

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            ESP_LOGI(LLSYNC_LOG_TAG, "ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT");
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0) {
                ble_qiot_advertising_start();
                adv_config_done = 2;
            }
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            /* advertising start complete event to indicate advertising start successfully or failed */
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(LLSYNC_LOG_TAG, "advertising start failed");
            }else{
                ESP_LOGI(LLSYNC_LOG_TAG, "advertising start successfully");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(LLSYNC_LOG_TAG, "Advertising stop failed");
            }
            else {
                ESP_LOGI(LLSYNC_LOG_TAG, "Stop adv successfully\n");
            }
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(LLSYNC_LOG_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
            param->update_conn_params.status,
            param->update_conn_params.min_int,
            param->update_conn_params.max_int,
            param->update_conn_params.conn_int,
            param->update_conn_params.latency,
            param->update_conn_params.timeout);
            break;
        default:
            break;
    }
}

void example_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(LLSYNC_LOG_TAG, "prepare write, handle = %d, value len = %d", param->write.handle, param->write.len);
    esp_gatt_status_t status = ESP_GATT_OK;
    if (prepare_write_env->prepare_buf == NULL) {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
        prepare_write_env->handle = 0;
        if (prepare_write_env->prepare_buf == NULL) {
            ESP_LOGE(LLSYNC_LOG_TAG, "%s, Gatt_server prep no mem", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
    } else {
        if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_OFFSET;
        } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_ATTR_LEN;
        }
    }
    /*send response when param->write.need_rsp is true */
    if (param->write.need_rsp){
        esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
        if (gatt_rsp != NULL){
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
                ESP_LOGE(LLSYNC_LOG_TAG, "Send response error");
            }
            free(gatt_rsp);
        }else{
            ESP_LOGE(LLSYNC_LOG_TAG, "%s, malloc failed", __func__);
        }
    }
    if (status != ESP_GATT_OK){
        return;
    }
    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);
    prepare_write_env->prepare_len += param->write.len;
    prepare_write_env->handle = param->write.handle;
}

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC && prepare_write_env->prepare_buf){
        if (prepare_write_env->handle == llsync_handle_table[IDX_CHAR_VAL_A]){
            ESP_LOGI(LLSYNC_LOG_TAG, "ble_device_info_write_cb");
            ble_device_info_write_cb(prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
        }else if (prepare_write_env->handle == llsync_handle_table[IDX_CHAR_VAL_B]){
            ESP_LOGI(LLSYNC_LOG_TAG, "ble_lldata_write_cb");
            ble_lldata_write_cb(prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
        }
        esp_log_buffer_hex(LLSYNC_LOG_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }else{
        ESP_LOGI(LLSYNC_LOG_TAG,"ESP_GATT_PREP_WRITE_CANCEL");
    }
    if (prepare_write_env->prepare_buf) {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}


static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:
            {
                esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(SAMPLE_DEVICE_NAME);
                if (set_dev_name_ret) {
                    ESP_LOGE(LLSYNC_LOG_TAG, "set device name failed, error code = %x", set_dev_name_ret);
                }
                esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, 7);
                if (raw_adv_ret){
                    ESP_LOGE(LLSYNC_LOG_TAG, "config raw adv data failed, error code = %x ", raw_adv_ret);
                }
                adv_config_done |= ADV_CONFIG_FLAG;
                esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
                if (create_attr_ret) {
                    ESP_LOGE(LLSYNC_LOG_TAG, "create attr table failed, error code = %x", create_attr_ret);
                }
            }
            break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(LLSYNC_LOG_TAG, "ESP_GATTS_READ_EVT");
            break;
        case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep){
                // the data length of gattc write  must be less than LLSYNC_CHAR_VAL_LEN_MAX.
                ESP_LOGI(LLSYNC_LOG_TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :", param->write.handle, param->write.len);
                esp_log_buffer_hex(LLSYNC_LOG_TAG, param->write.value, param->write.len);
                if (param->write.handle == llsync_handle_table[IDX_CHAR_VAL_A]){
                    ESP_LOGI(LLSYNC_LOG_TAG, "ble_device_info_write_cb");
                    ble_device_info_write_cb(param->write.value, param->write.len);
                }else if (param->write.handle == llsync_handle_table[IDX_CHAR_VAL_B]){
                    ESP_LOGI(LLSYNC_LOG_TAG, "ble_lldata_write_cb");
                    ble_lldata_write_cb(param->write.value, param->write.len);
                }
                ESP_LOGI(LLSYNC_LOG_TAG, "GATT_WRITE_EVT OVER");
                /* send response when param->write.need_rsp is true*/
                if (param->write.need_rsp){
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
                }
            }else{
                example_prepare_write_event_env(gatts_if, &prepare_write_env, param);
            }
            break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            // the length of gattc prepare write data must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
            ESP_LOGI(LLSYNC_LOG_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
            example_exec_write_event_env(&prepare_write_env, param);
            break;
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(LLSYNC_LOG_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
            break;
        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(LLSYNC_LOG_TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d", param->conf.status, param->conf.handle);
            break;
        case ESP_GATTS_START_EVT:
            ESP_LOGI(LLSYNC_LOG_TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
            break;
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(LLSYNC_LOG_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
            esp_log_buffer_hex(LLSYNC_LOG_TAG, param->connect.remote_bda, 6);
            esp_ble_conn_update_params_t conn_params = {0};
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            /* For the iOS system, please refer to Apple official documents about the BLE connection parameters restrictions. */
            conn_params.latency = 0;
            conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
            conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
            conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
            //start sent the update connection parameters to the peer device.
            esp_ble_gap_update_conn_params(&conn_params);
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(LLSYNC_LOG_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
            ble_qiot_advertising_start();
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:{
            if (param->add_attr_tab.status != ESP_GATT_OK){
                ESP_LOGE(LLSYNC_LOG_TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
            }
            else if (param->add_attr_tab.num_handle != HRS_IDX_NB){
                ESP_LOGE(LLSYNC_LOG_TAG, "create attribute table abnormally, num_handle (%d) doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, HRS_IDX_NB);
            }
            else {
                ESP_LOGI(LLSYNC_LOG_TAG, "create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
                memcpy(llsync_handle_table, param->add_attr_tab.handles, sizeof(llsync_handle_table));
                esp_ble_gatts_start_service(llsync_handle_table[IDX_SVC]);
            }
            break;
            }
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_DELETE_EVT:
        default:
            break;
        }
}


static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            llsync_profile_tab[PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGE(LLSYNC_LOG_TAG, "reg app failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        }
    }
    
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
            if (gatts_if == ESP_GATT_IF_NONE || gatts_if == llsync_profile_tab[idx].gatts_if) {
                if (llsync_profile_tab[idx].gatts_cb) {
                    llsync_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

void ble_services_add(const qiot_service_init_s *p_service)
{
    // do nothing
    return;
}

void ble_qiot_service_init(void)
{
    esp_err_t ret;

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(LLSYNC_LOG_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(LLSYNC_LOG_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(LLSYNC_LOG_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(LLSYNC_LOG_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    // init llsync sdk
    ble_qiot_explorer_init();
    
    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        ESP_LOGE(LLSYNC_LOG_TAG, "gatts register error, error code = %x", ret);
        return;
    }
    
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(LLSYNC_LOG_TAG, "gap register error, error code = %x", ret);
        return;
    }
    
    ret = esp_ble_gatts_app_register(LLSYNC_APP_ID);
    if (ret){
        ESP_LOGE(LLSYNC_LOG_TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    return;
}

#ifdef __cplusplus
}
#endif
