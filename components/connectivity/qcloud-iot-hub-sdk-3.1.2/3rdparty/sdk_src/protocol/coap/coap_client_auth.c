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

#include <string.h>
#include <time.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "qcloud_iot_common.h"

#include "coap_client.h"

#include "qcloud_iot_device.h"
#include "utils_param_check.h"

static void _coap_client_auth_callback(void *message, void *userContext)
{
	IOT_FUNC_ENTRY

	POINTER_SANITY_CHECK_RTN(message);
	POINTER_SANITY_CHECK_RTN(userContext);

    CoAPClient *client = (CoAPClient *)userContext;
    CoAPMessage *msg = (CoAPMessage *)message;

    if (msg->code_class == COAP_MSG_SUCCESS && msg->code_detail == COAP_MSG_CODE_205_CONTENT) {
    	Log_i("auth token message success, code_class: %d code_detail: %d", msg->code_class, msg->code_detail);
    	if (msg->pay_load_len == 0 || msg->pay_load == NULL || strlen(msg->pay_load) == 0) {
    		client->is_authed = COAP_FALSE;
    		Log_e("auth token response empty");
    	}
    	else {
    		client->auth_token_len = msg->pay_load_len;
			client->auth_token = HAL_Malloc(client->auth_token_len);
			strncpy(client->auth_token, msg->pay_load,client->auth_token_len);
			client->is_authed = COAP_TRUE;
			Log_d("auth_token_len = %d, auth_token = %.*s", client->auth_token_len, client->auth_token_len, client->auth_token);
    	}
    }
    else {
    	client->is_authed = COAP_FALSE;
    	Log_e("auth token message failed, code_class: %d code_detail: %d", msg->code_class, msg->code_detail);
    }

    IOT_FUNC_EXIT
}

static void get_coap_next_conn_id(CoAPClient *pclient) {
    int i = 0;

	srand((unsigned)HAL_Timer_current_sec());

	for (i = 0; i < COAP_MAX_CONN_ID_LEN - 1; i++) {
		int flag = rand() % 3;

		switch(flag) {
			case 0:
				pclient->conn_id[i] = (rand() % 26) + 'a';
				break;
			case 1:
				pclient->conn_id[i] = (rand() % 26) + 'A';
				break;
			case 2:
				pclient->conn_id[i] = (rand() % 10) + '0';
				break;
		}
	}

	pclient->conn_id[COAP_MAX_CONN_ID_LEN - 1] = '\0';

    return;
}

int coap_client_auth(CoAPClient *pclient) {
	IOT_FUNC_ENTRY

	POINTER_SANITY_CHECK(pclient, QCLOUD_ERR_COAP_NULL);

	int     ret = QCLOUD_RET_SUCCESS;

	CoAPClient* coap_client = (CoAPClient*)pclient;
	CoAPMessage send_message = DEFAULT_COAP_MESSAGE;

	coap_message_type_set(&send_message, COAP_MSG_CON);
    coap_message_code_set(&send_message, COAP_MSG_REQ, COAP_MSG_POST);

    coap_message_id_set(&send_message, get_next_coap_msg_id(coap_client));

    char message_token[8] = {0};
    int  len = get_coap_message_token(pclient, message_token);
    coap_message_token_set(&send_message, message_token, len);

    len = MAX_SIZE_OF_PRODUCT_ID + strlen(iot_device_info_get()->device_name)
    		+ strlen(COAP_AUTH_URI) + 4;
    char *auth_path = (char*)HAL_Malloc(len);
    HAL_Snprintf(auth_path, len, "%s/%s/%s", iot_device_info_get()->product_id,
             iot_device_info_get()->device_name, COAP_AUTH_URI);
    coap_message_option_add(&send_message, COAP_MSG_URI_PATH, strlen(auth_path), auth_path);
    HAL_Free(auth_path);

    coap_message_option_add(&send_message, COAP_MSG_NEED_RESP, 1, "0");

    coap_message_callback_set(&send_message, _coap_client_auth_callback);
    coap_message_context_set(&send_message, pclient);

    get_coap_next_conn_id(coap_client);

    send_message.pay_load_len = strlen(QCLOUD_IOT_DEVICE_SDK_APPID) + COAP_MAX_CONN_ID_LEN + 2;
    send_message.pay_load = (char *) HAL_Malloc (send_message.pay_load_len);
    if (NULL == send_message.pay_load) IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);

    char *temp_pay_load = (char*)HAL_Malloc(send_message.pay_load_len);
    if (NULL == temp_pay_load) IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);

    HAL_Snprintf(temp_pay_load, send_message.pay_load_len, "%s;%s", QCLOUD_IOT_DEVICE_SDK_APPID, coap_client->conn_id);

    coap_message_payload_set(&send_message, temp_pay_load, send_message.pay_load_len);

	ret = coap_message_send(coap_client, &send_message);

    HAL_Free(temp_pay_load);
    HAL_Free(send_message.pay_load);

	IOT_FUNC_EXIT_RC(ret)
}

#ifdef __cplusplus
}
#endif
