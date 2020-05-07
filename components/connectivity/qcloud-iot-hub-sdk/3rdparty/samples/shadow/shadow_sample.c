/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_export.h"

void OnDeltaCallback(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty)
{
    Log_i(">>>>> Delta str: %s", pJsonValueBuffer);
    pProperty->delta_arrived = true;
}

void OnShadowUpdateCallback(void *pClient, Method method, RequestAck requestAck, const char *pJsonDocument,
                            void *pUserdata)
{
    Log_i("recv shadow update response, response ack: %d", requestAck);
}

static int _report_desire_null(void *handle, char *jsonBuffer, size_t sizeOfBuffer)
{
    /* device data updated, desire should be set null */
    int rc = IOT_Shadow_JSON_ConstructDesireAllNull(handle, jsonBuffer, sizeOfBuffer);
    if (rc == QCLOUD_RET_SUCCESS) {
        rc = IOT_Shadow_Update(handle, jsonBuffer, sizeOfBuffer, OnShadowUpdateCallback, "desire_null",
                               QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
        if (rc == QCLOUD_RET_SUCCESS) {
            Log_d("shadow update(desired) success");
        } else {
            Log_e("shadow update(desired) failed, err: %d", rc);
        }

    } else {
        Log_e("construct desire failed, err: %d", rc);
    }

    return rc;
}

static int _setup_connect_init_params(ShadowInitParams *initParams, DeviceInfo *dev_info)
{
    initParams->device_name = dev_info->device_name;
    initParams->product_id  = dev_info->product_id;

#ifdef AUTH_MODE_CERT
    char  certs_dir[16] = "certs";
    char  current_path[128];
    char *cwd = getcwd(current_path, sizeof(current_path));

    if (cwd == NULL) {
        Log_e("getcwd return NULL");
        return QCLOUD_ERR_FAILURE;
    }

#ifdef WIN32
    HAL_Snprintf(initParams->cert_file, FILE_PATH_MAX_LEN, "%s\\%s\\%s", current_path, certs_dir,
                 dev_info->dev_cert_file_name);
    HAL_Snprintf(initParams->key_file, FILE_PATH_MAX_LEN, "%s\\%s\\%s", current_path, certs_dir,
                 dev_info->dev_key_file_name);
#else
    HAL_Snprintf(initParams->cert_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 dev_info->dev_cert_file_name);
    HAL_Snprintf(initParams->key_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 dev_info->dev_key_file_name);
#endif

#else
    initParams->device_secret = dev_info->device_secret;
#endif

    initParams->command_timeout        = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
    initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;
    initParams->auto_connect_enable    = 1;

    return QCLOUD_RET_SUCCESS;
}

int demo_device_shadow()
{
    int rc = QCLOUD_ERR_FAILURE;

    void *shadow_client = NULL;

    char   shadow_json_buffer[200];
    size_t shadow_json_buf_size = sizeof(shadow_json_buffer) / sizeof(shadow_json_buffer[0]);

    DeviceProperty shadow_property;
    int            current_update_count = 0;

    DeviceInfo device_info = {0};
    rc                     = HAL_GetDevInfo((void *)&device_info);
    if (QCLOUD_RET_SUCCESS != rc) {
        Log_e("get device info failed: %d", rc);
        return rc;
    }

    // init connection
    ShadowInitParams init_params = DEFAULT_SHAWDOW_INIT_PARAMS;
    rc                           = _setup_connect_init_params(&init_params, &device_info);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("init params err,rc=%d", rc);
        return rc;
    }

    shadow_client = IOT_Shadow_Construct(&init_params);
    if (shadow_client == NULL) {
        Log_e("shadow client constructed failed.");
        return QCLOUD_ERR_FAILURE;
    }

    // register delta property
    shadow_property.key  = "updateCount";
    shadow_property.data = &current_update_count;
    shadow_property.type = JINT32;
    rc                   = IOT_Shadow_Register_Property(shadow_client, &shadow_property, OnDeltaCallback);
    if (rc != QCLOUD_RET_SUCCESS) {
        rc = IOT_Shadow_Destroy(shadow_client);
        Log_e("register device shadow property failed, err: %d", rc);
        return rc;
    }

    // do get and sync operation before update
    rc = IOT_Shadow_Get_Sync(shadow_client, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("get device shadow failed, err: %d", rc);
        return rc;
    }

    while (IOT_Shadow_IsConnected(shadow_client) || QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT == rc ||
           QCLOUD_RET_MQTT_RECONNECTED == rc || QCLOUD_RET_SUCCESS == rc) {
        rc = IOT_Shadow_Yield(shadow_client, 500);

        if (QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT == rc) {
            HAL_SleepMs(1000);
            continue;
        } else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
            Log_e("exit with error: %d", rc);
            break;
        }

        if (shadow_property.delta_arrived) {
            _report_desire_null(shadow_client, shadow_json_buffer, shadow_json_buf_size);

            shadow_property.delta_arrived = false;
        }

        IOT_Shadow_JSON_ConstructReport(shadow_client, shadow_json_buffer, shadow_json_buf_size, 1, &shadow_property);
        rc = IOT_Shadow_Update(shadow_client, shadow_json_buffer, shadow_json_buf_size, OnShadowUpdateCallback, NULL,
                               QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
        current_update_count++;

        // sleep for some time
        HAL_SleepMs(3000);
    }

    Log_e("loop exit with error: %d", rc);

    rc            = IOT_Shadow_Destroy(shadow_client);
    shadow_client = NULL;

    return rc;
}

int main()
{
    IOT_Log_Set_Level(eLOG_DEBUG);

    demo_device_shadow();

    return 0;
}
