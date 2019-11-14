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

#ifndef OTA_MQTT_CHANNEL

#include "ota_client.h"

#include <string.h>

typedef struct  {
    void 					*coap;

    const char 				*product_id;
    const char 				*device_name;

    char 					topic_upgrade[OTA_MAX_TOPIC_LEN];

    OnOTAMessageCallback 	msg_callback;

    void *context;
} OTA_CoAP_Struct_t;

static void _otacoap_upgrage_cb(void* coap_message, void *pcontext) {
	int ret_code = IOT_COAP_GetMessageCode(coap_message);

	switch (ret_code) {
		case COAP_EVENT_RECEIVE_ACK:
			Log_i("received OTA message ACK, msgid: %d", IOT_COAP_GetMessageId(coap_message));
			break;
		case COAP_EVENT_RECEIVE_RESPCONTENT:
		{
			Log_i("received OTA message respconetent.");
			char* payload = NULL;
			int payload_len = 0;
			int ret = -1;
			ret = IOT_COAP_GetMessagePayload(coap_message, &payload, &payload_len);
			if (ret == QCLOUD_RET_SUCCESS) {
				OTA_CoAP_Struct_t *handle = (OTA_CoAP_Struct_t *) pcontext;
				if (NULL != handle->msg_callback) {
					handle->msg_callback(handle->context, payload, payload_len);
				}
			}
			else {
				Log_e("message received response, content error.");
			}
		}
			break;
		case COAP_EVENT_UNAUTHORIZED:
			Log_e("coap client auth token expired or invalid, msgid: %d", IOT_COAP_GetMessageId(coap_message));
			break;
		case COAP_EVENT_FORBIDDEN:
			Log_e("coap URI is invalid for this device, msgid: %d", IOT_COAP_GetMessageId(coap_message));
			break;
		case COAP_EVENT_INTERNAL_SERVER_ERROR:
			Log_e("coap server internal error, msgid: %d", IOT_COAP_GetMessageId(coap_message));
			break;
		case COAP_EVENT_ACK_TIMEOUT:
			Log_e("message receive ACK timeout, msgid: %d", IOT_COAP_GetMessageId(coap_message));
			break;
		case COAP_EVENT_SEPRESP_TIMEOUT:
			Log_i("message received ACK but receive response timeout, msgid: %d", IOT_COAP_GetMessageId(coap_message));
			break;
		default:
			break;
	}
}

/* Generate topic name according to @OTATopicType, @productId, @deviceName */
/* and then copy to @buf. */
/* 0, successful; -1, failed */
static int _otacoap_gen_topic_name(char *buf, size_t bufLen, const char *OTATopicType, const char *productId, const char *deviceName)
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
static int _otacoap_publish(OTA_CoAP_Struct_t *handle, const char *topicType, const char *msg, bool needResp)
{
    IOT_FUNC_ENTRY;

    int ret;
    char topic_name[OTA_MAX_TOPIC_LEN];

    SendMsgParams send_params = DEFAULT_SENDMSG_PARAMS;
    send_params.pay_load = (void *)msg;
    send_params.pay_load_len = strlen(msg);
    send_params.resp_callback = _otacoap_upgrage_cb;
    send_params.need_resp = needResp;
    send_params.user_context = handle;

    /* inform OTA to topic: "/ota/device/progress/$(product_id)/$(device_name)" */
    ret = _otacoap_gen_topic_name(topic_name, OTA_MAX_TOPIC_LEN, topicType, handle->product_id, handle->device_name);
    if (ret < 0) {
       Log_e("generate topic name of info failed");
       IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FAIL);
    }

    ret = IOT_COAP_SendMessage(handle->coap, topic_name, &send_params);
    if (ret < 0) {
        Log_e("publish failed");
        IOT_FUNC_EXIT_RC(IOT_OTA_ERR_OSC_FAILED);
    }

    IOT_FUNC_EXIT_RC(ret);
}

void *qcloud_osc_init(const char *productId, const char *deviceName, void *channel, OnOTAMessageCallback callback, void *context)
{
	OTA_CoAP_Struct_t *h_osc = NULL;

	if (NULL == (h_osc = HAL_Malloc(sizeof(OTA_CoAP_Struct_t)))) {
		Log_e("allocate for h_osc failed");
		goto do_exit;
	}

	memset(h_osc, 0, sizeof(OTA_CoAP_Struct_t));

	h_osc->coap = channel;
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
    return _otacoap_publish(handle, "report", msg, false);
}

/* report version of OTA firmware */
int qcloud_osc_report_version(void *handle, const char *msg)
{
    return _otacoap_publish(handle, "report", msg, true);
}

/* report upgrade begin of OTA firmware */
int qcloud_osc_report_upgrade_result(void *handle, const char *msg)
{
    return _otacoap_publish(handle, "report", msg, false);
}

/* OSC, OTA signal channel */
#endif 

#ifdef __cplusplus
}
#endif
