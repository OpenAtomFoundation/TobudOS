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

__QCLOUD_STATIC__ qcloud_err_t coap_client_network_host_construct(qcloud_network_t *network, qcloud_device_t *device)
{
    int server_len;
    char coap_server[QCLOUD_SERVER_DOMAIN_MAX];

    memset(network->host, 0, sizeof(network->host));
    server_len = osal_snprintf(coap_server, sizeof(coap_server), "%s.%s", device->product_id, qcloud_coap_server);
    if (server_len < 0 || server_len > QCLOUD_SERVER_DOMAIN_MAX - 1) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}
    memcpy(network->host, coap_server, sizeof(network->host));

    network->port = qcloud_coap_port;

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_STATIC__ qcloud_err_t coap_client_network_init(qcloud_network_t *network, qcloud_device_t *device)
{
#if (QCLOUD_CFG_TLS_EN > 0u)
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_tls_init(&network->tls_opt, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_network_dtls_init(network), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
#else
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_network_udp_init(network), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
#endif

    QCLOUD_FUNC_EXIT_RC_IF_NOT(coap_client_network_host_construct(network, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_STATIC__ uint16_t coap_client_random_packet_id_generate(void)
{
#define PACKET_ID_MAX   (65535)

    srand((unsigned)osal_timer_current_sec());
    return rand() % (PACKET_ID_MAX + 1) + 1;
}

__QCLOUD_STATIC__ qcloud_err_t coap_client_construct(qcloud_coap_client_t *client,
                                                                    qcloud_device_t *device,
                                                                    coap_event_handler_fn_t handler)
{
    int len = 0;

    client->auth_state                      = QCLOUD_COAP_AUTH_STATE_NONE;
    client->command_timeout                 = QCLOUD_COAP_COMMAND_TIMEOUT;
    client->message_token                   = 0;

    client->event_handler.handler           = handler;

    // packet id 取随机数 1- 65536
    client->packet_id                       = coap_client_random_packet_id_generate();
    client->auth_token                      = NULL;
    client->auth_token_len                  = 0;
    client->retransmit_max                  = 1;

    len = osal_snprintf(client->auth_uri, sizeof(client->auth_uri), "%s/%s/%s", device->product_id, device->device_name, QCLOUD_COAP_AUTH_URI);
    if (len < 0 || len >= QCLOUD_COAP_AUTH_URI_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    qcloud_list_init(&client->message_list);

    if ((client->message_list_lock = osal_mutex_create()) == NULL) {
        goto errout;
    }

    if ((client->tx_lock = osal_mutex_create()) == NULL) {
        goto errout;
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);

errout:
    if (client->message_list_lock) {
        osal_mutex_destroy(client->message_list_lock);
    }

    if (client->tx_lock) {
        osal_mutex_destroy(client->tx_lock);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
}

__QCLOUD_API__ qcloud_err_t qcloud_coap_client_create(qcloud_coap_client_t *client,
                                                                        qcloud_device_t *device,
                                                                        coap_event_handler_fn_t handler)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(device, QCLOUD_ERR_INVAL);

    memset(client, 0, sizeof(qcloud_coap_client_t));

    QCLOUD_FUNC_EXIT_RC_IF_NOT(coap_client_network_init(&client->network, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    QCLOUD_FUNC_EXIT_RC_IF_NOT(coap_client_construct(client, device, handler), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_API__ qcloud_err_t qcloud_coap_client_connect(qcloud_coap_client_t *client)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    int len;
    qcloud_err_t rc;
    char connection_id[QCLOUD_COAP_CONNECT_ID_MAX + 1];

    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc = client->network.connect(&client->network), QCLOUD_ERR_SUCCESS, rc);

    coap_glue_connect_id_generate(connection_id);

    len = osal_snprintf(client->auth_id, sizeof(client->auth_id), "%s;%s", QCLOUD_APPID, connection_id);
    if (len < 0 || len >= QCLOUD_COAP_AUTH_ID_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    coap_auth(client, connection_id);

    while (client->auth_state == QCLOUD_COAP_AUTH_STATE_NONE) {
		qcloud_coap_client_yield(client, 200);
	}

    if (client->auth_state != QCLOUD_COAP_AUTH_STATE_SUCCESS) {
        QCLOUD_LOG_I("auth failed");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_API__ qcloud_err_t qcloud_coap_client_yield(qcloud_coap_client_t *client, uint32_t timeout_ms)
{
	QCLOUD_FUNC_ENTRY;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

	return coap_glue_spin(client, timeout_ms);
}

__QCLOUD_API__ qcloud_err_t qcloud_coap_client_destroy(qcloud_coap_client_t *client)
{
	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    if (client->network.is_connected && client->network.is_connected(&client->network)) {
        client->network.disconnect(&client->network);
    }

    coap_glue_message_list_destroy(client);

	osal_mutex_destroy(client->tx_lock);
	osal_mutex_destroy(client->message_list_lock);

	if (client->auth_token) {
		osal_free(client->auth_token);
		client->auth_token = NULL;
	}

	client->auth_token_len = 0;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}


__QCLOUD_API__ qcloud_err_t qcloud_coap_client_msg_send(qcloud_coap_client_t *client,
                                                                            char *topic,
                                                                            coap_send_opt_t *send_opt)
{
	QCLOUD_FUNC_ENTRY

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(topic, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(send_opt, QCLOUD_ERR_INVAL);

    int len;
    qcloud_err_t rc;
    char message_token[8] = {0};
	coap_message_t send_msg = COAP_MESSAGE_INITIALIZER;

	if (strlen(topic) > QCLOUD_COAP_URI_MAX) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
	}

    coap_message_init(&send_msg);

	coap_message_type_set(&send_msg, COAP_MSG_TYPE_CON);
    coap_message_code_set(&send_msg, COAP_CODE_CLASS_REQ, COAP_REQUEST_METHOD_POST);

    coap_message_id_set(&send_msg, coap_glue_packet_id_generate(client));

    len = coap_message_token_get(client, message_token);
    coap_message_token_set(&send_msg, message_token, len);

    send_msg.payload = (char *)osal_malloc(send_opt->payload_len);
    if (!send_msg.payload) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    coap_message_payload_set(&send_msg, send_opt->payload, send_opt->payload_len);

    coap_message_option_add(&send_msg, COAP_MSG_OPTION_CODE_URI_PATH, strlen(topic), topic);
    coap_message_option_add(&send_msg, COAP_MSG_OPTION_CODE_AUTH_TOKEN, client->auth_token_len, client->auth_token);

    if (send_opt->resp_cb) {
        coap_message_option_add(&send_msg, COAP_MSG_OPTION_CODE_NEED_RESP, 1, "1");
        coap_message_callback_set(&send_msg, send_opt->resp_cb);
    } else {
    	coap_message_option_add(&send_msg, COAP_MSG_OPTION_CODE_NEED_RESP, 1, "0");
    }

    coap_message_context_set(&send_msg, send_opt->context);

	rc = coap_glue_msg_send(client, &send_msg);
	osal_free(send_msg.payload);

    QCLOUD_FUNC_EXIT_RC(rc)
}

__QCLOUD_API__ uint16_t qcloud_coap_msg_id_get(coap_message_t *message)
{
    QCLOUD_FUNC_ENTRY

    if (!message) {
        return COAP_MSG_ID_MAX;
    }

    return message->id;
}

__QCLOUD_API__ qcloud_err_t qcloud_coap_msg_payload_get(coap_message_t *message, char **payload, int *payload_len)
{
	QCLOUD_FUNC_ENTRY

	QCLOUD_POINTER_SANITY_CHECK(message, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(payload, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(payload_len, QCLOUD_ERR_INVAL);

	if (message->code_class != COAP_CODE_CLASS_SUCCESS ||
        message->code_detail != COAP_CODE_DETAIL_205_CONTENT) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
	}

	*payload        =  message->payload;
	*payload_len    =  message->payload_len;

	QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_API__ coap_event_type_t qcloud_coap_event_type_get(coap_message_t *message)
{
	QCLOUD_FUNC_ENTRY

	QCLOUD_POINTER_SANITY_CHECK(message, COAP_EVENT_TYPE_UNAUTHORIZED);

    switch (message->code_class) {
        case COAP_CODE_CLASS_SUCCESS:
            return COAP_EVENT_TYPE_RECEIVE_RESPCONTENT;

        case COAP_CODE_CLASS_SERVER_ERR:
            return COAP_EVENT_TYPE_INTERNAL_SERVER_ERROR;

        case COAP_CODE_CLASS_INTERNAL_ERR:
            return COAP_EVENT_TYPE_SEPRESP_TIMEOUT;

        case COAP_CODE_CLASS_CLIENT_ERR:
            if (message->code_detail == COAP_CODE_DETAIL_401_UNAUTHORIZED) {
                return COAP_EVENT_TYPE_UNAUTHORIZED;
            } else {
                return COAP_EVENT_TYPE_FORBIDDEN;
            }

        default:
            QCLOUD_LOG_E("not supported code class: %d", message->code_class);
            return COAP_EVENT_TYPE_ACK_TIMEOUT;
    }
}

#ifdef __cplusplus
}
#endif

