/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

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

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#ifdef OTA_MQTT_CHANNEL

#include "ota_client.h"

#include <string.h>

/* OSC, OTA signal channel */
typedef struct  {
    void 					*mqtt;									//MQTT cient

    const char 				*product_id;
    const char 				*device_name;

    char 					topic_upgrade[OTA_MAX_TOPIC_LEN];		//OTA MQTT Topic
    OnOTAMessageCallback 	msg_callback;

    void *context;
} OTA_MQTT_Struct_t;

/* Generate topic name according to @OTATopicType, @productId, @deviceName */
/* and then copy to @buf. */
/* 0, successful; -1, failed */
static int _otamqtt_gen_topic_name(char *buf, size_t bufLen, const char *OTATopicType, const char *productId, const char *deviceName)
{
    IOT_FUNC_ENTRY;

    int ret;

    ret = HAL_Snprintf(buf, bufLen, "$ota/%s/%s/%s", OTATopicType, productId, deviceName);

    if(ret >= bufLen) IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);

    if (ret < 0) {
        Log_e("HAL_Snprintf failed");
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/* report progress of OTA */
static int _otamqtt_publish(OTA_MQTT_Struct_t *handle, const char *topicType, int qos, const char *msg)
{
    IOT_FUNC_ENTRY;

    int ret;
    char topic_name[OTA_MAX_TOPIC_LEN];
    PublishParams pub_params = DEFAULT_PUB_PARAMS;

    if (0 == qos) {
        pub_params.qos = QOS0;
    } else {
        pub_params.qos = QOS1;
    }
    pub_params.payload = (void *)msg;
    pub_params.payload_len = strlen(msg);

    /* inform OTA to topic: "/ota/device/progress/$(product_id)/$(device_name)" */
    ret = _otamqtt_gen_topic_name(topic_name, OTA_MAX_TOPIC_LEN, topicType, handle->product_id, handle->device_name);
    if (ret < 0) {
       Log_e("generate topic name of info failed");
       IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    }

    ret = IOT_MQTT_Publish(handle->mqtt, topic_name, &pub_params);
    if (ret < 0) {
        Log_e("publish to topic: %s failed", topic_name);
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_OSC_FAILED);
    }

    IOT_FUNC_EXIT_RC(ret);
}

/* callback after OTA topic is subscribed */
/* Parse firmware info (version/URL/file size/MD5) from JSON text */
static void _otamqtt_upgrage_cb(void *pClient, MQTTMessage *message, void *pcontext)
{
    OTA_MQTT_Struct_t *handle = (OTA_MQTT_Struct_t *) pcontext;

    Log_d("topic=%.*s", message->topic_len, message->ptopic);
    Log_i("len=%u, topic_msg=%.*s", message->payload_len, message->payload_len, (char *)message->payload);

    if (NULL != handle->msg_callback) {
        handle->msg_callback(handle->context, message->payload, message->payload_len);
    }
}

void *qcloud_osc_init(const char *productId, const char *deviceName, void *channel, OnOTAMessageCallback callback, void *context)
{
    int ret;
    OTA_MQTT_Struct_t *h_osc = NULL;

    if (NULL == (h_osc = HAL_Malloc(sizeof(OTA_MQTT_Struct_t)))) {
        Log_e("allocate for h_osc failed");
        goto do_exit;
    }

    memset(h_osc, 0, sizeof(OTA_MQTT_Struct_t));

    /* subscribe the OTA topic: "$ota/update/$(product_id)/$(device_name)" */
    ret = _otamqtt_gen_topic_name(h_osc->topic_upgrade, OTA_MAX_TOPIC_LEN, "update", productId, deviceName);
    if (ret < 0) {
        Log_e("generate topic name of upgrade failed");
        goto do_exit;
    }

    SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = _otamqtt_upgrage_cb;
    sub_params.qos = QOS1;
    sub_params.user_data = h_osc;

    ret = IOT_MQTT_Subscribe(channel, h_osc->topic_upgrade, &sub_params);
    if (ret < 0) {
        Log_e("ota mqtt subscribe failed!");
        goto do_exit;
    }

    h_osc->mqtt = channel;
    h_osc->product_id = productId;
    h_osc->device_name = deviceName;
    h_osc->msg_callback = callback;
    h_osc->context = context;

    return h_osc;

do_exit:
    if (NULL != h_osc) {
         HAL_Free(h_osc);
    }

    return NULL;
}

int qcloud_osc_deinit(void *handle)
{
    IOT_FUNC_ENTRY;

    if (NULL != handle) {
        HAL_Free(handle);
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/* report progress of OTA */
int qcloud_osc_report_progress(void *handle, const char *msg)
{
    return _otamqtt_publish(handle, "report", QOS0, msg);
}

/* report version of OTA firmware */
int qcloud_osc_report_version(void *handle, const char *msg)
{
    return _otamqtt_publish(handle, "report", QOS1, msg);
}

/* report upgrade begin of OTA firmware */
int qcloud_osc_report_upgrade_result(void *handle, const char *msg)
{
    return _otamqtt_publish(handle, "report", QOS1, msg);
}

#endif 

#ifdef __cplusplus
}
#endif
