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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "qcloud_iot_common.h"

#include "coap_client.h"

#include "qcloud_iot_ca.h"
#include "qcloud_iot_device.h"
#include "utils_param_check.h"
#include "utils_base64.h"


static char s_qcloud_iot_host[HOST_STR_LENGTH] = {0};
static int s_qcloud_iot_port = COAP_SERVER_PORT;

#ifndef AUTH_WITH_NOTLS
#ifndef AUTH_MODE_CERT
static unsigned char sg_psk_str[DECODE_PSK_LENGTH];
#endif
#endif

static uint16_t _get_random_start_packet_id(void)
{
    srand((unsigned)HAL_Timer_current_sec());
    return rand() % 65536 + 1;
}

void* IOT_COAP_Construct(CoAPInitParams *pParams)
{
    POINTER_SANITY_CHECK(pParams, NULL);
    STRING_PTR_SANITY_CHECK(pParams->product_id, NULL);
    STRING_PTR_SANITY_CHECK(pParams->device_name, NULL);

    CoAPClient *coap_client = NULL;
    if ((coap_client = (CoAPClient*) HAL_Malloc (sizeof(CoAPClient))) == NULL) {
		Log_e("memory not enough to malloc COAPClient");
		return NULL;
    }

    if (iot_device_info_init() == QCLOUD_RET_SUCCESS) {
        iot_device_info_set(pParams->product_id, pParams->device_name);
    }

	int rc = qcloud_iot_coap_init(coap_client, pParams);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("coap init failed: %d", rc);
		HAL_Free(coap_client);
		return NULL;
	}

    if (coap_client->network_stack.connect(&coap_client->network_stack) != QCLOUD_RET_SUCCESS) {
    	Log_e("coap connect failed: %d", rc);
		HAL_Free(coap_client);
		return NULL;
    }
	else {
		Log_i("coap connect success");
	}

    coap_client_auth(coap_client);
    while (coap_client->is_authed == -1) {
		IOT_COAP_Yield(coap_client, 200);
	}

	if (coap_client->is_authed == COAP_TRUE) {
		Log_i("device auth successfully, connid: %s", coap_client->conn_id);
		return coap_client;
	} else {
		Log_e("device auth failed, connid: %s", coap_client->conn_id);
		void *client = coap_client;
		IOT_COAP_Destroy(&client);
		return NULL;
	}
}

void IOT_COAP_Destroy(void **pClient) {
	POINTER_SANITY_CHECK_RTN(*pClient);

    CoAPClient *coap_client = (CoAPClient *)(*pClient);

    if ((coap_client)->network_stack.handle != 0) {
        (coap_client)->network_stack.disconnect(&(coap_client)->network_stack);
    }

	list_destroy(coap_client->message_list);

	HAL_MutexDestroy(coap_client->lock_send_buf);
	HAL_MutexDestroy(coap_client->lock_list_wait_ack);

	if (coap_client->auth_token != NULL) {
		HAL_Free(coap_client->auth_token);
		coap_client->auth_token = NULL;
	}
	coap_client->auth_token_len = 0;
	coap_client->is_authed = -1;

    HAL_Free(*pClient);
    *pClient = NULL;

    Log_i("coap release!");
}

int IOT_COAP_Yield(void *pClient, uint32_t timeout_ms) {
	IOT_FUNC_ENTRY;

	POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

	CoAPClient* coap_client = (CoAPClient*)pClient;

	return coap_message_cycle(coap_client, timeout_ms);
}

int IOT_COAP_SendMessage(void *pClient, char *topicName, SendMsgParams *sendParams) {
	IOT_FUNC_ENTRY

	POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(topicName, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(sendParams, QCLOUD_ERR_INVAL);

	if (strlen(topicName) > URI_PATH_MAX_LEN) {
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
	}

	int ret = QCLOUD_RET_SUCCESS;

	CoAPClient* coap_client = (CoAPClient*)pClient;

	CoAPMessage send_message = DEFAULT_COAP_MESSAGE;

	coap_message_type_set(&send_message, COAP_MSG_CON);
    coap_message_code_set(&send_message, COAP_MSG_REQ, COAP_MSG_POST);

    coap_message_id_set(&send_message, get_next_coap_msg_id(coap_client));

    char    message_token[8] = {0};
    int len = get_coap_message_token(pClient, message_token);
    coap_message_token_set(&send_message, message_token, len);

    send_message.pay_load = (char *) HAL_Malloc (sendParams->pay_load_len);
    if (NULL == send_message.pay_load) IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);

    coap_message_payload_set(&send_message, sendParams->pay_load, sendParams->pay_load_len);

    coap_message_option_add(&send_message, COAP_MSG_URI_PATH, strlen(topicName), topicName);
    coap_message_option_add(&send_message, COAP_MSG_AUTH_TOKEN, coap_client->auth_token_len, coap_client->auth_token);
    if (sendParams->need_resp == false) {
    	coap_message_option_add(&send_message, COAP_MSG_NEED_RESP, 1, "0");
    	coap_message_context_set(&send_message, sendParams->user_context);
    }
    else {
    	coap_message_option_add(&send_message, COAP_MSG_NEED_RESP, 1, "1");
        coap_message_callback_set(&send_message, sendParams->resp_callback);
        coap_message_context_set(&send_message, sendParams->user_context);
    }

	ret = coap_message_send(coap_client, &send_message);

	HAL_Free(send_message.pay_load);

	if (ret != QCLOUD_RET_SUCCESS) {
		IOT_FUNC_EXIT_RC(ret)
	}

	IOT_FUNC_EXIT_RC(send_message.msg_id)
}

int   IOT_COAP_GetMessageId(void *pMessage) {
	IOT_FUNC_ENTRY

	POINTER_SANITY_CHECK(pMessage, QCLOUD_ERR_INVAL);
	CoAPMessage *message = (CoAPMessage *)pMessage;

	IOT_FUNC_EXIT_RC(message->msg_id)
}

int   IOT_COAP_GetMessagePayload(void *pMessage, char **payload, int *payloadLen) {
	IOT_FUNC_ENTRY

	POINTER_SANITY_CHECK(pMessage, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(payload, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(payloadLen, QCLOUD_ERR_INVAL);

	CoAPMessage *message = (CoAPMessage *)pMessage;
	if (message->code_class != COAP_MSG_SUCCESS || message->code_detail != COAP_MSG_CODE_205_CONTENT) {
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
	}

	*payload    =  message->pay_load;
	*payloadLen =  message->pay_load_len;

	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int   IOT_COAP_GetMessageCode(void *pMessage) {
	IOT_FUNC_ENTRY

	POINTER_SANITY_CHECK(pMessage, QCLOUD_ERR_INVAL);

	CoAPMessage *message = (CoAPMessage *)pMessage;

	int rc = COAP_EVENT_ACK_TIMEOUT;

	if (message->code_class == COAP_MSG_SUCCESS) {
		rc = COAP_EVENT_RECEIVE_RESPCONTENT;
	}
	else if (message->code_class == COAP_MSG_CLIENT_ERR) {
		if (message->code_detail == COAP_MSG_CODE_401_UNAUTHORIZED) {
			rc = COAP_EVENT_UNAUTHORIZED;
		}
		else {
			rc = COAP_EVENT_FORBIDDEN;
		}
	}
	else if (message->code_class == COAP_MSG_SERVER_ERR) {
		rc = COAP_EVENT_INTERNAL_SERVER_ERROR;
	}
	else if (message->code_class == COAP_MSG_SDKINTERNAL_ERR) {
		rc = COAP_EVENT_SEPRESP_TIMEOUT;
	}
	else {
		/**
		 * no more error code
		 */
		Log_e("not supported code class: %d", message->code_class);
	}

	IOT_FUNC_EXIT_RC(rc)
}

int qcloud_iot_coap_init(CoAPClient *pClient, CoAPInitParams *pParams) {
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pParams, QCLOUD_ERR_INVAL);

	memset(pClient, 0x0, sizeof(CoAPClient));

    int size = HAL_Snprintf(s_qcloud_iot_host, HOST_STR_LENGTH, "%s.%s", pParams->product_id, QCLOUD_IOT_COAP_DEIRECT_DOMAIN);
    if (size < 0 || size > HOST_STR_LENGTH - 1) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    pClient->is_authed = -1;

    if (pParams->command_timeout < MIN_COMMAND_TIMEOUT)
    	pParams->command_timeout = MIN_COMMAND_TIMEOUT;
    if (pParams->command_timeout > MAX_COMMAND_TIMEOUT)
    	pParams->command_timeout = MAX_COMMAND_TIMEOUT;
    pClient->command_timeout_ms = pParams->command_timeout;

#ifndef AUTH_WITH_NOTLS
#ifdef AUTH_MODE_CERT
    bool certEmpty = (pParams->cert_file == NULL || pParams->key_file == NULL);
	if (certEmpty) {
		Log_e("cert file or key file is empty!");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
	}
	Log_d("cert file: %s", pParams->cert_file);
	Log_d("key file: %s", pParams->key_file);

    // device param for TLS connection
	pClient->network_stack.ssl_connect_params.cert_file = pParams->cert_file;
	pClient->network_stack.ssl_connect_params.key_file = pParams->key_file;
    pClient->network_stack.ssl_connect_params.ca_crt = iot_ca_get();
    pClient->network_stack.ssl_connect_params.ca_crt_len = strlen(pClient->network_stack.ssl_connect_params.ca_crt);

#else
    pClient->network_stack.ssl_connect_params.psk_id = iot_device_info_get()->client_id;
    if (pParams->device_secret != NULL) {
        size_t src_len = strlen(pParams->device_secret);
        size_t len;
        memset(sg_psk_str, 0x00, DECODE_PSK_LENGTH);
        qcloud_iot_utils_base64decode(sg_psk_str, sizeof( sg_psk_str ), &len, (unsigned char *)pParams->device_secret, src_len );
        pClient->network_stack.ssl_connect_params.psk = (char *)sg_psk_str;
        pClient->network_stack.ssl_connect_params.psk_length = len;
        pClient->network_stack.ssl_connect_params.ca_crt = iot_ca_get();
        pClient->network_stack.ssl_connect_params.ca_crt_len = strlen(pClient->network_stack.ssl_connect_params.ca_crt);
    } else {
        Log_e("psk is empty!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }
#endif

    pClient->network_stack.host = s_qcloud_iot_host;
    pClient->network_stack.port = s_qcloud_iot_port;
#else
    pClient->network_stack.host = s_qcloud_iot_host;
    pClient->network_stack.port = s_qcloud_iot_port;
#endif

    pClient->auth_token = NULL;
    pClient->auth_token_len = 0;
    // next_msg_id, random: 1- 65536
	pClient->next_msg_id = _get_random_start_packet_id();
    pClient->read_buf_size = COAP_RECVMSG_MAX_BUFLEN;
    pClient->send_buf_size = COAP_SENDMSG_MAX_BUFLEN;

    pClient->lock_send_buf = HAL_MutexCreate();
    if (pClient->lock_send_buf == NULL) {
    	Log_e("create send buf lock failed");
    	IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    pClient->lock_list_wait_ack = HAL_MutexCreate();
    if (pClient->lock_list_wait_ack == NULL) {
    	Log_e("create send buf lock failed");
    	goto error;
    }

    pClient->message_list = list_new();
    pClient->max_retry_count = pParams->max_retry_count;
    pClient->event_handle = pParams->event_handle;

    // init network stack
    qcloud_iot_coap_network_init(&(pClient->network_stack));

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);

error:
	if (pClient->lock_send_buf != NULL) {
		HAL_MutexDestroy(pClient->lock_send_buf);
		pClient->lock_send_buf = NULL;
	}

	if (pClient->lock_list_wait_ack != NULL) {
		HAL_MutexDestroy(pClient->lock_list_wait_ack);
		pClient->lock_list_wait_ack = NULL;
	}

	IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
}

#ifdef __cplusplus
}
#endif

