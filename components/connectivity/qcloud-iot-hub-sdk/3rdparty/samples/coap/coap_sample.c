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
#include "qcloud_iot_import.h"

void response_message_callback(void *coap_message, void *userContext)
{
    int ret_code = IOT_COAP_GetMessageCode(coap_message);
    switch (ret_code) {
        case COAP_EVENT_RECEIVE_ACK:
            Log_i("message received ACK, msgid: %d", IOT_COAP_GetMessageId(coap_message));
            break;
        case COAP_EVENT_RECEIVE_RESPCONTENT: {
            char *payload     = NULL;
            int   payload_len = 0;
            int   ret         = -1;
            ret               = IOT_COAP_GetMessagePayload(coap_message, &payload, &payload_len);
            if (ret == QCLOUD_RET_SUCCESS) {
                Log_i("message received response, content: %s", payload);
            } else {
                Log_e("message received response, content error.");
            }
        }

        break;
        case COAP_EVENT_UNAUTHORIZED:
            Log_i("coap client auth token expired or invalid, msgid: %d", IOT_COAP_GetMessageId(coap_message));
            break;
        case COAP_EVENT_FORBIDDEN:
            Log_i("coap URI is invalid for this device, msgid: %d", IOT_COAP_GetMessageId(coap_message));
            break;
        case COAP_EVENT_INTERNAL_SERVER_ERROR:
            Log_i("coap server internal error, msgid: %d", IOT_COAP_GetMessageId(coap_message));
            break;
        case COAP_EVENT_ACK_TIMEOUT:
            Log_i("message receive ACK timeout, msgid: %d", IOT_COAP_GetMessageId(coap_message));
            break;
        case COAP_EVENT_SEPRESP_TIMEOUT:
            Log_i("message received ACK but receive response timeout, msgid: %d", IOT_COAP_GetMessageId(coap_message));
            break;
        default:
            break;
    }
}

void event_handler(void *pcontext, CoAPEventMessage *message)
{
    switch (message->event_type) {
        case COAP_EVENT_RECEIVE_ACK:
            Log_i("message received ACK, msgid: %d", (unsigned)(uintptr_t)message->message);
            break;
        case COAP_EVENT_RECEIVE_RESPCONTENT:
            Log_i("message received response, content: %s", IOT_COAP_GetMessageId(message->message));
            break;
        case COAP_EVENT_UNAUTHORIZED:
            Log_i("coap client auth token expired or invalid, msgid: %d", (unsigned)(uintptr_t)message->message);
            break;
        case COAP_EVENT_FORBIDDEN:
            Log_i("coap URI is invalid for this device, msgid: %d", (unsigned)(uintptr_t)message->message);
            break;
        case COAP_EVENT_INTERNAL_SERVER_ERROR:
            Log_i("coap server internal error, msgid: %d", (unsigned)(uintptr_t)message->message);
            break;
        case COAP_EVENT_ACK_TIMEOUT:
            Log_i("message receive ACK timeout, msgid: %d", (unsigned)(uintptr_t)message->message);
            break;
        case COAP_EVENT_SEPRESP_TIMEOUT:
            Log_i("message received ACK but receive response timeout, msgid: %d",
                  (unsigned)(uintptr_t)message->message);
            break;
        default:
            Log_e("unrecogonized event type: %d", message->event_type);
            break;
    }
}

static int _setup_connect_init_params(CoAPInitParams *initParams, DeviceInfo *device_info)
{
    initParams->product_id  = device_info->product_id;
    initParams->device_name = device_info->device_name;

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
                 device_info->dev_cert_file_name);
    HAL_Snprintf(initParams->key_file, FILE_PATH_MAX_LEN, "%s\\%s\\%s", current_path, certs_dir,
                 device_info->dev_key_file_name);
#else
    HAL_Snprintf(initParams->cert_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 device_info->dev_cert_file_name);
    HAL_Snprintf(initParams->key_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 device_info->dev_key_file_name);
#endif

#else
    initParams->device_secret = device_info->device_secret;
#endif

    initParams->command_timeout   = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
    initParams->event_handle.h_fp = event_handler;
    initParams->max_retry_count   = 3;

    return QCLOUD_RET_SUCCESS;
}

int main(int argc, char **argv)
{
    int rc = QCLOUD_RET_SUCCESS;

    IOT_Log_Set_Level(eLOG_DEBUG);

    DeviceInfo device_info = {0};
    rc                     = HAL_GetDevInfo((void *)&device_info);
    if (QCLOUD_RET_SUCCESS != rc) {
        Log_e("get device info failed: %d", rc);
        return rc;
    }

    CoAPInitParams init_params = DEFAULT_COAPINIT_PARAMS;
    rc                         = _setup_connect_init_params(&init_params, &device_info);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("init params err,rc=%d", rc);
        return rc;
    }

    void *coap_client = IOT_COAP_Construct(&init_params);
    if (coap_client == NULL) {
        Log_e("COAP Client construct failed.");
        return QCLOUD_ERR_FAILURE;
    }

    do {
        SendMsgParams send_params = DEFAULT_SENDMSG_PARAMS;
        send_params.pay_load      = "{\"action\":\"hello world\"}";
        send_params.pay_load_len  = strlen("{\"action\":\"hello world\"}");
        send_params.resp_callback = response_message_callback;

        char topicName[128] = "";
        sprintf(topicName, "%s/%s/data", device_info.product_id, device_info.device_name);
        Log_i("topic name is %s", topicName);

        rc = IOT_COAP_SendMessage(coap_client, topicName, &send_params);
        if (rc < 0) {
            Log_e("client publish topic failed :%d.", rc);
        } else {
            Log_d("client topic has been sent, msg_id: %d", rc);
        }

        if (!(argc >= 2 && !strcmp("loop", argv[1]))) {
            HAL_SleepMs(1000);
        }

        rc = IOT_COAP_Yield(coap_client, 200);

        if (rc != QCLOUD_RET_SUCCESS) {
            Log_e("exit with error: %d", rc);
            break;
        }

        if (argc >= 2)
            HAL_SleepMs(1000);

    } while (argc >= 2 && !strcmp("loop", argv[1]));

    IOT_COAP_Destroy(&coap_client);

    return QCLOUD_RET_SUCCESS;
}
