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

#include "qcloud.h"

__QCLOUD_STATIC__ void coap_auth_callback(void *message, void *context)
{
	QCLOUD_FUNC_ENTRY

	QCLOUD_POINTER_SANITY_CHECK_RTN(message);
	QCLOUD_POINTER_SANITY_CHECK_RTN(context);

    coap_message_t *msg = NULL;
    qcloud_coap_client_t *client = NULL;

    msg = (coap_message_t *)message;
    client = (qcloud_coap_client_t *)context;

    if (msg->code_class != COAP_CODE_CLASS_SUCCESS ||
        msg->code_detail != COAP_CODE_DETAIL_205_CONTENT) {
        client->auth_state = QCLOUD_COAP_AUTH_STATE_FAIL;
        QCLOUD_LOG_E("auth token failed, code_class: %d code_detail: %d", msg->code_class, msg->code_detail);
        return;
    }

	QCLOUD_LOG_I("auth token success, code_class: %d code_detail: %d", msg->code_class, msg->code_detail);

	if (msg->payload_len == 0 ||
        msg->payload == NULL ||
        strlen(msg->payload) == 0) {
		client->auth_state = QCLOUD_COAP_AUTH_STATE_FAIL;
		QCLOUD_LOG_E("auth token response empty");
	} else {
		client->auth_token_len  = msg->payload_len;
		client->auth_token      = osal_malloc(client->auth_token_len);
		strncpy(client->auth_token, msg->payload,client->auth_token_len);
		client->auth_state      = QCLOUD_COAP_AUTH_STATE_SUCCESS;
		QCLOUD_LOG_D("auth_token_len = %d, auth_token = %.*s", client->auth_token_len, client->auth_token_len, client->auth_token);
	}

    QCLOUD_FUNC_EXIT
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_auth(qcloud_coap_client_t *client, char *connection_id)
{
	QCLOUD_FUNC_ENTRY

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_COAP_NULL);

    int len;
	qcloud_err_t rc;
    char message_token[8] = {0};
	coap_message_t connect_msg = COAP_MESSAGE_INITIALIZER;

    coap_message_init(&connect_msg);

	coap_message_type_set(&connect_msg, COAP_MSG_TYPE_CON);
    coap_message_code_set(&connect_msg, COAP_CODE_CLASS_REQ, COAP_REQUEST_METHOD_POST);

    coap_message_id_set(&connect_msg, coap_glue_packet_id_generate(client));

    len = coap_message_token_get(client, message_token);
    coap_message_token_set(&connect_msg, message_token, len);

    coap_message_option_add(&connect_msg, COAP_MSG_OPTION_CODE_URI_PATH, strlen(client->auth_uri), client->auth_uri);

    coap_message_option_add(&connect_msg, COAP_MSG_OPTION_CODE_NEED_RESP, 1, "0");

    coap_message_callback_set(&connect_msg, coap_auth_callback);
    coap_message_context_set(&connect_msg, client);

    connect_msg.payload_len = sizeof(client->auth_id);
    connect_msg.payload     = (char *)osal_malloc(connect_msg.payload_len);
    if (!connect_msg.payload) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    coap_message_payload_set(&connect_msg, client->auth_id, connect_msg.payload_len);

	rc = coap_glue_msg_send(client, &connect_msg);

    osal_free(connect_msg.payload);

	QCLOUD_FUNC_EXIT_RC(rc)
}

#ifdef __cplusplus
}
#endif

