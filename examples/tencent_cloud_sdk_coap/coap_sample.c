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

#include "tos_k.h"
#include "qcloud.h"

static void response_message_callback(void *msg, void *context)
{
    int ret = -1;
    char* payload = NULL;
    int payload_len = 0;
    coap_event_type_t event_type;
    coap_message_t *message = NULL;

    message = msg;
    event_type = qcloud_coap_event_type_get(message);

	switch (event_type) {
		case COAP_EVENT_TYPE_RECEIVE_ACK:
			QCLOUD_LOG_I("message received ACK, msgid: %d", qcloud_coap_msg_id_get(message));
			break;

		case COAP_EVENT_TYPE_RECEIVE_RESPCONTENT:
			ret = qcloud_coap_msg_payload_get(message, &payload, &payload_len);
			if (ret == QCLOUD_ERR_SUCCESS) {
				QCLOUD_LOG_I("message received response, len: %d  content: %s", payload_len, payload);
			} else {
				QCLOUD_LOG_E("message received response, content error.");
			}
			break;

		case COAP_EVENT_TYPE_UNAUTHORIZED:
			QCLOUD_LOG_I("coap client auth token expired or invalid, msgid: %d", qcloud_coap_msg_id_get(message));
			break;

		case COAP_EVENT_TYPE_FORBIDDEN:
			QCLOUD_LOG_I("coap URI is invalid for this device, msgid: %d", qcloud_coap_msg_id_get(message));
			break;

		case COAP_EVENT_TYPE_INTERNAL_SERVER_ERROR:
			QCLOUD_LOG_I("coap server internal error, msgid: %d", qcloud_coap_msg_id_get(message));
			break;

		case COAP_EVENT_TYPE_ACK_TIMEOUT:
			QCLOUD_LOG_I("message receive ACK timeout, msgid: %d", qcloud_coap_msg_id_get(message));
			break;

		case COAP_EVENT_TYPE_SEPRESP_TIMEOUT:
			QCLOUD_LOG_I("message received ACK but receive response timeout, msgid: %d", qcloud_coap_msg_id_get(message));
			break;

		default:
			break;
	}
}

static void event_handler(void *context, coap_event_t *event)
{
	switch (event->type) {
		case COAP_EVENT_TYPE_RECEIVE_ACK:
			QCLOUD_LOG_I("message received ACK, msgid: %d", *(uint16_t *)event->message);
			break;

		case COAP_EVENT_TYPE_RECEIVE_RESPCONTENT:
			QCLOUD_LOG_I("message received response, content: %s", qcloud_coap_msg_id_get(event->message));
			break;

		case COAP_EVENT_TYPE_UNAUTHORIZED:
			QCLOUD_LOG_I("coap client auth token expired or invalid, msgid: %d", *(uint16_t *)event->message);
			break;

		case COAP_EVENT_TYPE_FORBIDDEN:
			QCLOUD_LOG_I("coap URI is invalid for this device, msgid: %d", *(uint16_t *)event->message);
			break;

		case COAP_EVENT_TYPE_INTERNAL_SERVER_ERROR:
			QCLOUD_LOG_I("coap server internal error, msgid: %d", *(uint16_t *)event->message);
			break;

		case COAP_EVENT_TYPE_ACK_TIMEOUT:
			QCLOUD_LOG_I("message receive ACK timeout, msgid: %d", *(uint16_t *)event->message);
			break;

		case COAP_EVENT_TYPE_SEPRESP_TIMEOUT:
			QCLOUD_LOG_I("message received ACK but receive response timeout, msgid: %d", *(uint16_t *)event->message);
			break;

		default:
			QCLOUD_LOG_E("unrecogonized event type: %d", event->type);
			break;
	}
}

qcloud_device_t device;
qcloud_coap_client_t client;

int coap_thread(void)
{
    char topic[128];
	qcloud_err_t rc;
    coap_send_opt_t send_opt;

    qcloud_device_create(&device, "ABU45A5KT8", "dev001", "RPpqUxL03frSDSufVhjuLw==");

    qcloud_coap_client_create(&client, &device, event_handler);

    qcloud_coap_client_connect(&client);

    do {
        memset(&send_opt, 0, sizeof(coap_send_opt_t));
    	send_opt.payload        = "{\"name\":\"hello world\"}";
    	send_opt.payload_len    = strlen("{\"name\":\"hello world\"}");
    	send_opt.resp_cb        = response_message_callback;

        sprintf(topic, "%s/%s/data", device.product_id, device.device_name);
        QCLOUD_LOG_I("topic: %s", topic);

        rc = qcloud_coap_client_msg_send(&client, topic, &send_opt);
	    if (rc != QCLOUD_ERR_SUCCESS) {
	        QCLOUD_LOG_E("client publish topic failed :%d.", rc);
	    } else {
	    	QCLOUD_LOG_D("client topic has been sent, msg_id: %d", rc);
	    }

    	rc = qcloud_coap_client_yield(&client, 200);

		if (rc != QCLOUD_ERR_SUCCESS){
			QCLOUD_LOG_E("exit with error: %d", rc);
			break;
		}

		osal_sleep_ms(4000);
    } while (1);

    qcloud_coap_client_destroy(&client);

    return 1;
}

