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

#ifndef IOT_SHADOW_CLIENT_H_
#define IOT_SHADOW_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "mqtt_client.h"
#include "qcloud_iot_device.h"
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "shadow_client_json.h"
#include "utils_param_check.h"

/* Max number of requests in appending state */
#define MAX_APPENDING_REQUEST_AT_ANY_GIVEN_TIME (10)

/* Max size of clientToken */
#define MAX_SIZE_OF_CLIENT_TOKEN (MAX_SIZE_OF_CLIENT_ID + 10)

/* Max size of JSON string which only contain clientToken field */
#define MAX_SIZE_OF_JSON_WITH_CLIENT_TOKEN (MAX_SIZE_OF_CLIENT_TOKEN + 20)

/* Size of buffer to receive JSON document from server */
#define CLOUD_IOT_JSON_RX_BUF_LEN (QCLOUD_IOT_MQTT_RX_BUF_LEN + 1)

/**
 * @brief define type of request parameters
 */
typedef struct _RequestParam {
    Method method;  // method type: GET, UPDATE, DELETE

    uint32_t timeout_sec;  // request timeout in second

    OnRequestCallback request_callback;  // request callback

    void *user_context;  // user context for callback

} RequestParams;

#define DEFAULT_REQUEST_PARAMS {GET, 4, NULL, NULL};

/**
 * @brief for property and it's callback
 */
typedef struct {
    void *property;

    OnPropRegCallback callback;

} PropertyHandler;

typedef struct _ShadowInnerData {
    uint32_t token_num;
    int32_t  sync_status;
    List *   request_list;
    List *   property_handle_list;
    char *   result_topic;
} ShadowInnerData;

typedef struct _Shadow {
    void *           mqtt;
    void *           mutex;
    eShadowType      shadow_type;
    MQTTEventHandler event_handle;
    ShadowInnerData  inner_data;
    char             shadow_recv_buf[CLOUD_IOT_JSON_RX_BUF_LEN];
} Qcloud_IoT_Shadow;

int qcloud_iot_shadow_init(Qcloud_IoT_Shadow *pShadow);

void qcloud_iot_shadow_reset(void *pClient);

/**
 * @brief handle the expired requests in list
 *
 * @param pShadow   shadow client
 */
void handle_expired_request(Qcloud_IoT_Shadow *pShadow);

/**
 * @brief Entry of all shadow JSON request
 *
 * @param pShadow       shadow client
 * @param pParams       request param
 * @param pJsonDoc      JSON buffer
 * @param sizeOfBuffer  size of buffer
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int do_shadow_request(Qcloud_IoT_Shadow *pShadow, RequestParams *pParams, char *pJsonDoc, size_t sizeOfBuffer);

/**
 * @brief subscribe shadow topic $shadow/operation/result
 *
 * @param pShadow       shadow client
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int subscribe_operation_result_to_cloud(Qcloud_IoT_Shadow *pShadow);

#ifdef __cplusplus
}
#endif

#endif /* IOT_SHADOW_CLIENT_H_ */
