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

__QCLOUD_INTERNAL__ void coap_glue_connect_id_generate(char *conn_id)
{
    int i = 0, flag;

	srand((unsigned)osal_timer_current_sec());

	for (i = 0; i < QCLOUD_COAP_CONNECT_ID_MAX - 1; ++i) {
        flag = rand() % 3;

		switch (flag) {
			case 0:
				conn_id[i] = (rand() % 26) + 'a';
				break;

			case 1:
				conn_id[i] = (rand() % 26) + 'A';
				break;

			case 2:
				conn_id[i] = (rand() % 10) + '0';
				break;
		}
	}

	conn_id[QCLOUD_COAP_CONNECT_ID_MAX - 1] = '\0';
}

__QCLOUD_INTERNAL__ uint16_t coap_glue_packet_id_generate(qcloud_coap_client_t *client)
{
#define PACKET_ID_MAX      (65535)

    uint16_t packet_id = client->packet_id;

    client->packet_id = (packet_id == PACKET_ID_MAX ? 1 : (packet_id + 1));
    return client->packet_id;
}

__QCLOUD_STATIC__ coap_event_type_t coap_glue_event_type_get(coap_message_t *message, coap_incoming_msg_type_t msg_type)
{
	if (message->code_class == COAP_CODE_CLASS_SUCCESS &&
        message->code_detail == COAP_CODE_DETAIL_205_CONTENT) {
        if (msg_type == COAP_INCOMING_MSG_TYPE_RESP) {
            return COAP_EVENT_TYPE_RECEIVE_RESPCONTENT;
        } else {
            return COAP_EVENT_TYPE_RECEIVE_ACK;
        }
	} else if (message->code_class == COAP_CODE_CLASS_CLIENT_ERR &&
	            message->code_detail == COAP_CODE_DETAIL_401_UNAUTHORIZED) {
        return COAP_EVENT_TYPE_UNAUTHORIZED;
	} else if (message->code_class == COAP_CODE_CLASS_CLIENT_ERR &&
	            message->code_detail == COAP_CODE_DETAIL_403_FORBIDDEN) {
        return COAP_EVENT_TYPE_FORBIDDEN;
	} else {
        return COAP_EVENT_TYPE_INTERNAL_SERVER_ERROR;
	}
}

__QCLOUD_STATIC__ void coap_glue_msg_sent_info_destroy(coap_msg_sent_info_t *msg_sent_info)
{
    qcloud_list_del(&msg_sent_info->list);
    osal_free(msg_sent_info->message);
    osal_free(msg_sent_info);
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_glue_message_list_destroy(qcloud_coap_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    qcloud_list_t *curr, *next;
    coap_msg_sent_info_t *msg_sent_info = NULL;

    if (qcloud_list_empty(&client->message_list)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    osal_mutex_lock(client->message_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->message_list) {
        msg_sent_info = QCLOUD_LIST_ENTRY(curr, coap_msg_sent_info_t, list);

        coap_glue_msg_sent_info_destroy(msg_sent_info);
    }

    osal_mutex_unlock(client->message_list_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ qcloud_err_t coap_glue_message_list_scan(qcloud_coap_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    int ret;
    size_t write_len;
    qcloud_list_t *curr, *next;
    coap_event_t event;
    coap_message_t message = {0};
    coap_msg_sent_info_t *msg_sent_info = NULL;

    if (qcloud_list_empty(&client->message_list)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

	osal_mutex_lock(client->message_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->message_list) {
        msg_sent_info = QCLOUD_LIST_ENTRY(curr, coap_msg_sent_info_t, list);

        if (!osal_timer_is_expired(&msg_sent_info->timer)) {
            continue;
        }

        if (msg_sent_info->transmit_count < client->retransmit_max &&
            !msg_sent_info->is_acked) {
            osal_timer_init(&msg_sent_info->timer);
            osal_timer_countdown_ms(&msg_sent_info->timer, client->command_timeout);
            ++msg_sent_info->transmit_count;

            ret = client->network.write(&client->network, msg_sent_info->message, msg_sent_info->message_len,
                                        osal_timer_remain(&msg_sent_info->timer), &write_len);
            if (ret == QCLOUD_ERR_SUCCESS) {
                continue;
            }

            QCLOUD_LOG_E("retansmit the message id %d failed.", msg_sent_info->message_id);
        } else if (msg_sent_info->resp_cb) {
            message.type            = COAP_MSG_TYPE_ACK;
            message.context         = msg_sent_info->context;
            message.code_class      = COAP_CODE_CLASS_INTERNAL_ERR;
            message.code_detail     = COAP_CODE_DETAIL_600_TIMEOUT;
            message.id              = msg_sent_info->message_id;
            msg_sent_info->resp_cb(&message, msg_sent_info->context);
        } else if (client->event_handler.handler) {
            if (msg_sent_info->is_acked) {
                event.type = COAP_EVENT_TYPE_SEPRESP_TIMEOUT;
            } else {
                event.type = COAP_EVENT_TYPE_ACK_TIMEOUT;
            }
            event.message = (void *)(&msg_sent_info->message_id);
            client->event_handler.handler(client->event_handler.context, &event);
        } else {
            QCLOUD_LOG_E("response and event callback both null");
        }

        coap_glue_msg_sent_info_destroy(msg_sent_info);
    }

    osal_mutex_unlock(client->message_list_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ qcloud_err_t coap_glue_message_list_unrecord(qcloud_coap_client_t *client, coap_message_t *message, coap_incoming_msg_type_t msg_type)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    coap_event_t event;
    coap_event_type_t event_type;
    qcloud_list_t *curr, *next;
    coap_msg_sent_info_t *msg_sent_info = NULL;

    if (qcloud_list_empty(&client->message_list)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

	osal_mutex_lock(client->message_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->message_list) {
        msg_sent_info = QCLOUD_LIST_ENTRY(curr, coap_msg_sent_info_t, list);

        if (msg_type == COAP_INCOMING_MSG_TYPE_ACK &&
            msg_sent_info->message_id == message->id) {
            msg_sent_info->is_acked = QCLOUD_TRUE;
            osal_timer_init(&msg_sent_info->timer);
            osal_timer_countdown_ms(&msg_sent_info->timer, client->command_timeout);
        }

        if (msg_type == COAP_INCOMING_MSG_TYPE_RESP) {
			if (msg_sent_info->token_len != 0 &&
                msg_sent_info->token_len == message->token_len &&
                memcmp(msg_sent_info->token, message->token, message->token_len) == 0) {

				message->context = msg_sent_info->context;

				if (msg_sent_info->resp_cb) {
					msg_sent_info->resp_cb(message, msg_sent_info->context);
				} else if (client->event_handler.handler) { // event handle process
					event_type      = coap_glue_event_type_get(message, COAP_INCOMING_MSG_TYPE_RESP);
                    event.type      = event_type;
					event.message   = message;

					message->id     = msg_sent_info->message_id;
					client->event_handler.handler(client->event_handler.context, &event);
				} else {
					QCLOUD_LOG_E("response and event callback both null");
				}

                coap_glue_msg_sent_info_destroy(msg_sent_info);
			}
        }

        if (msg_type == COAP_INCOMING_MSG_TYPE_PIGGY) {
			if (msg_sent_info->message_id == message->id) {
				message->context = msg_sent_info->context;

				if (msg_sent_info->resp_cb) {
					msg_sent_info->resp_cb(message, msg_sent_info->context);
				} else if (client->event_handler.handler) { // event handle process
    				event_type      = coap_glue_event_type_get(message, COAP_INCOMING_MSG_TYPE_PIGGY);
                    event.type      = event_type;
					event.message   = (void *)(&message->id);
					client->event_handler.handler(client->event_handler.context, &event);
				} else {
					QCLOUD_LOG_E("response and event callback both null");
				}

                coap_glue_msg_sent_info_destroy(msg_sent_info);
			}
        }
    }

    osal_mutex_unlock(client->message_list_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
 *  @brief 向服务器发送确认消息
 *
 *  @param[in,out] client 指向客户端结构的指针
 *  @param[in] msg 指向消息结构的指针
 *
 *  @returns Operation status
 *  @retval 0 Success
 *  @retval <0 Error
 */
__QCLOUD_STATIC__ qcloud_err_t coap_glue_resp_ack_send(qcloud_coap_client_t *client, uint16_t id)
{
	QCLOUD_FUNC_ENTRY

    qcloud_err_t rc;
    coap_message_t ack_msg = COAP_MESSAGE_INITIALIZER;

    coap_message_init(&ack_msg);

    rc = coap_message_type_set(&ack_msg, COAP_MSG_TYPE_ACK);
    if (rc != QCLOUD_ERR_SUCCESS) {
        goto out;
    }

    rc = coap_message_id_set(&ack_msg, id);
    if (rc != QCLOUD_ERR_SUCCESS) {
        goto out;
    }

    rc = coap_glue_msg_send(client, &ack_msg);

out:
    coap_message_destroy(&ack_msg);
    QCLOUD_FUNC_EXIT_RC(rc)
}

__QCLOUD_STATIC__ qcloud_err_t coap_glue_piggy_message_handle(qcloud_coap_client_t *client, coap_message_t *message)
{
	QCLOUD_FUNC_ENTRY

	QCLOUD_FUNC_EXIT_RC(coap_glue_message_list_unrecord(client, message, COAP_INCOMING_MSG_TYPE_PIGGY));
}

__QCLOUD_STATIC__ qcloud_err_t coap_glue_ack_message_handle(qcloud_coap_client_t *client, coap_message_t *message)
{
	QCLOUD_FUNC_ENTRY

	QCLOUD_FUNC_EXIT_RC(coap_glue_message_list_unrecord(client, message, COAP_INCOMING_MSG_TYPE_ACK));
}

__QCLOUD_STATIC__ qcloud_err_t coap_glue_resp_message_handle(qcloud_coap_client_t *client, coap_message_t *message)
{
	QCLOUD_FUNC_ENTRY

	qcloud_err_t rc;

    if (message->type == COAP_MSG_TYPE_CON) {
    	rc = coap_glue_resp_ack_send(client, message->id);
    	QCLOUD_LOG_D("send ack message for id: %d, rc: %d", message->id, rc);
    }

    QCLOUD_FUNC_EXIT_RC(coap_glue_message_list_unrecord(client, message, COAP_INCOMING_MSG_TYPE_RESP));
}

__QCLOUD_STATIC__ qcloud_err_t coap_glue_incoming_message_handle(qcloud_coap_client_t *client, uint8_t *buf, size_t buf_len)
{
    QCLOUD_FUNC_ENTRY

    qcloud_err_t rc = QCLOUD_ERR_SUCCESS;
    coap_message_t incoming_msg;

    memset(&incoming_msg, 0x00, sizeof(coap_message_t));

    rc = coap_message_deserialize(&incoming_msg, (char *)buf, buf_len);
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_LOG_E("deserialize msg failed: %d", rc);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    if (incoming_msg.type == COAP_MSG_TYPE_ACK && COAP_MSG_IS_EMPTY_ACK(&incoming_msg)) {	// empty ACK
        QCLOUD_LOG_D("receive ACK msg, id %d", incoming_msg.id);
        coap_glue_ack_message_handle(client, &incoming_msg);
    } else if (incoming_msg.type == COAP_MSG_TYPE_ACK && !COAP_MSG_IS_EMPTY(&incoming_msg)) { // piggy Response
        QCLOUD_LOG_D("receive piggy ACK msg, id %d", incoming_msg.id);
        coap_glue_piggy_message_handle(client, &incoming_msg);
    } else if (incoming_msg.type == COAP_MSG_TYPE_CON && COAP_MSG_IS_EMPTY_RSP(&incoming_msg)) { // payload Response
        QCLOUD_LOG_D("receive response msg, id: %d", incoming_msg.id);
        coap_glue_resp_message_handle(client, &incoming_msg);
    } else if (incoming_msg.type == COAP_MSG_TYPE_NON && COAP_MSG_IS_EMPTY_RSP(&incoming_msg)) { // payload Response
        QCLOUD_LOG_D("receive response msg, id: %d", incoming_msg.id);
        coap_glue_resp_message_handle(client, &incoming_msg);
    } else {
        QCLOUD_LOG_E("msg type not recgonized");
    }

    if (incoming_msg.payload) {
        osal_free(incoming_msg.payload);
        incoming_msg.payload_len = 0;
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ coap_msg_sent_info_t *coap_glue_message_sent_info_construct(qcloud_coap_client_t *client, coap_message_t *message, int len)
{
    coap_msg_sent_info_t *msg_sent_info = NULL;

    msg_sent_info = (coap_msg_sent_info_t *)osal_malloc(sizeof(coap_msg_sent_info_t));
	if (!msg_sent_info) {
		QCLOUD_LOG_E("memory alloc failed");
		return NULL;
	}

	msg_sent_info->message = osal_malloc(len);
	if (!msg_sent_info->message) {
        QCLOUD_LOG_E("memory alloc failed");
        osal_free(msg_sent_info);
		return NULL;
	}

	msg_sent_info->is_acked     = QCLOUD_FALSE;
	msg_sent_info->context      = message->context;
	msg_sent_info->message_id   = message->id;
	msg_sent_info->resp_cb      = message->resp_cb;
	msg_sent_info->message_len  = len;
	msg_sent_info->token_len    = message->token_len;

	memcpy(msg_sent_info->token, message->token, message->token_len);
    memcpy(msg_sent_info->message, client->tx_buffer, len);

	if (message->type == COAP_MSG_TYPE_CON) {
		msg_sent_info->transmit_count = 0;
	    osal_timer_init(&msg_sent_info->timer);
	    osal_timer_countdown_ms(&msg_sent_info->timer, client->command_timeout);
	}

    return msg_sent_info;
}

__QCLOUD_STATIC__ qcloud_err_t coap_glue_message_list_record(qcloud_coap_client_t *client, coap_message_t *message, int len)
{
	QCLOUD_FUNC_ENTRY

    coap_msg_sent_info_t *msg_sent_info = NULL;

    msg_sent_info = coap_glue_message_sent_info_construct(client, message, len);
    if (!msg_sent_info) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
    }

    osal_mutex_lock(client->message_list_lock);
    qcloud_list_add(&msg_sent_info->list, &client->message_list);
    osal_mutex_unlock(client->message_list_lock);

	QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_STATIC__ qcloud_err_t coap_glue_recv(qcloud_coap_client_t *client, uint32_t timeout_ms)
{
	QCLOUD_FUNC_ENTRY

    size_t read_len = 0;
    qcloud_err_t rc;

    rc = client->network.read(&client->network, client->rx_buffer, sizeof(client->rx_buffer), timeout_ms, &read_len);

    switch (rc) {
		case QCLOUD_ERR_SUCCESS:
			rc = coap_glue_incoming_message_handle(client, client->rx_buffer, read_len);
			break;

		case QCLOUD_ERR_SSL_READ_TIMEOUT:
			break;

		default:
			break;
    }

    QCLOUD_FUNC_EXIT_RC(rc)
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_glue_spin(qcloud_coap_client_t *client, uint32_t timeout_ms)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;

    rc = coap_glue_recv(client, timeout_ms);
    if (rc != QCLOUD_ERR_SUCCESS && rc != QCLOUD_ERR_SSL_READ_TIMEOUT) {
        QCLOUD_FUNC_EXIT_RC(rc)
    }

    QCLOUD_FUNC_EXIT_RC(coap_glue_message_list_scan(client));
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_glue_msg_send(qcloud_coap_client_t *client, coap_message_t *message)
{
	QCLOUD_FUNC_ENTRY

    int len = 0;
    qcloud_err_t rc;
    size_t write_len = 0;

    osal_mutex_lock(client->tx_lock);

    len = coap_message_serialize(message, (char*)client->tx_buffer, sizeof(client->tx_buffer));
    if (len < 0) {
        QCLOUD_LOG_E("failed to serialize coap message");
        osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
    }

    rc = client->network.write(&client->network, client->tx_buffer, len, 0, &write_len);
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_LOG_E("coap net fail to write rc: %d", rc);
        osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(rc)
    }

	if (message->type == COAP_MSG_TYPE_CON &&
        message->code_class == COAP_CODE_CLASS_REQ) {
		rc = coap_glue_message_list_record(client, message, write_len);
		QCLOUD_LOG_I("add message id: %d, rc: %d", message->id, rc);
	} else {
		QCLOUD_LOG_I("message donot need retransmit");
	}

    osal_mutex_unlock(client->tx_lock);

    QCLOUD_FUNC_EXIT_RC(rc)
}

#ifdef __cplusplus
}
#endif

