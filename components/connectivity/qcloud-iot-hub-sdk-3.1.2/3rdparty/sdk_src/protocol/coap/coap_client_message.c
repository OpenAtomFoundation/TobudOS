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

// #include <memory.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#include "coap_client.h"
#include "coap_client_net.h"

#include "utils_param_check.h"
#include "utils_timer.h"

#define PROCESS_ACK_CMD								(0)
#define PROCESS_PIGGY_CMD							(1)
#define PROCESS_RESP_CMD							(2)
#define PROCESS_WAIT_CMD							(3)

static void _event_message_type_set(CoAPEventMessage* eventMsg, CoAPMessage *message, uint16_t processCmd) {
	if (message->code_class == COAP_MSG_SUCCESS && message->code_detail == COAP_MSG_CODE_205_CONTENT) {
		eventMsg->event_type = processCmd == PROCESS_RESP_CMD ? COAP_EVENT_RECEIVE_RESPCONTENT :
				COAP_EVENT_RECEIVE_ACK;
	}
	else if (message->code_class == COAP_MSG_CLIENT_ERR && message->code_detail == COAP_MSG_CODE_401_UNAUTHORIZED) {
		eventMsg->event_type = COAP_EVENT_UNAUTHORIZED;
		Log_e("coap messagefailed, message id: %d, failure reason: %d.%d",
				message->msg_id, message->code_class, message->code_detail);
	}
	else if (message->code_class == COAP_MSG_CLIENT_ERR && message->code_detail == COAP_MSG_CODE_403_FORBIDDEN) {
		eventMsg->event_type = COAP_EVENT_FORBIDDEN;
		Log_e("coap message failed, message id: %d, failure reason: %d.%d",
				message->msg_id, message->code_class, message->code_detail);
	}
	else {
		eventMsg->event_type = COAP_EVENT_INTERNAL_SERVER_ERROR;
		Log_e("coap message failed, message id: %d, failure reason: %d.%d",
						message->msg_id, message->code_class, message->code_detail);
	}
}

static int _coap_message_list_proc(CoAPClient *client, CoAPMessage *message, uint16_t processCmd)
{
    IOT_FUNC_ENTRY;
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

	HAL_MutexLock(client->lock_list_wait_ack);

    if (client->message_list->len <= 0) {
    	HAL_MutexUnlock(client->lock_list_wait_ack);
    	IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

	ListIterator *iter;
	ListNode *node = NULL;
	ListNode *temp_node = NULL;
	CoAPMsgSendInfo *send_info = NULL;

	if ((iter = list_iterator_new(client->message_list, LIST_TAIL)) == NULL) {
		HAL_MutexUnlock(client->lock_list_wait_ack);
		IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
	}

	for (;;) {
		node = list_iterator_next(iter);

		if (NULL != temp_node) {
			list_remove(client->message_list, temp_node);
			Log_i("remove node");
			temp_node = NULL;
		}

		if (NULL == node) {
			break;
		}

		send_info = (CoAPMsgSendInfo*) node->val;
		if (NULL == send_info) {
			Log_e("node's value is invalid!");
			continue;
		}

		if (processCmd == PROCESS_ACK_CMD) {
			if (send_info->msg_id == message->msg_id) {
				send_info->acked = 1; /* ACK is received */
			    InitTimer(&send_info->start_time);
			    countdown_ms(&send_info->start_time, client->command_timeout_ms);
			}
		}
		else if (processCmd == PROCESS_RESP_CMD) {
			if (0 != send_info->token_len && send_info->token_len == message->token_len
				&& 0 == memcmp(send_info->token, message->token, message->token_len)) {
				message->user_context  = send_info->user_context;

				if (send_info->handler != NULL) {
					send_info->handler(message, send_info->user_context);
				}
				else if (NULL != client->event_handle.h_fp) {	//event handle process
					CoAPEventMessage event_msg = {0};
					_event_message_type_set(&event_msg, message, processCmd);
					event_msg.message = message;
					message->msg_id = send_info->msg_id;
					client->event_handle.h_fp(client->event_handle.context, &event_msg);
				}
				else {
					Log_e("nether response callback nor event callback is set");
				}

				Log_d("remove the message id %d from list", send_info->msg_id);
				send_info->node_state = COAP_NODE_STATE_INVALID;
			}
		}
		else if (processCmd == PROCESS_PIGGY_CMD) {
			if (send_info->msg_id == message->msg_id) {
				message->user_context  = send_info->user_context;

				if (send_info->handler != NULL) {
					send_info->handler(message, send_info->user_context);
				}
				else if (NULL != client->event_handle.h_fp) {	//event handle process
					CoAPEventMessage event_msg = {0};
					_event_message_type_set(&event_msg, message, processCmd);
					event_msg.message = (void *)(uintptr_t)(message->msg_id);
					client->event_handle.h_fp(client->event_handle.context, &event_msg);
				}
				else {
					Log_e("nether response callback nor event callback is set");
				}

				Log_d("remove the message id %d from list", send_info->msg_id);

				send_info->acked = 1; /* ACK is received */
				send_info->node_state = COAP_NODE_STATE_INVALID;
			}
		}
		else if (processCmd == PROCESS_WAIT_CMD) {

			if (COAP_NODE_STATE_INVALID == send_info->node_state) {
				temp_node = node;
				continue;
			}

			if (left_ms(&send_info->start_time) > 0) {
				continue;
			}

			if (send_info->retrans_count < client->max_retry_count && (0 == send_info->acked)) {
			    InitTimer(&send_info->start_time);
			    countdown_ms(&send_info->start_time, client->command_timeout_ms);
				send_info->retrans_count++;
				Log_d("start to retansmit the message id %d len %d", send_info->msg_id, send_info->msglen);
				size_t written_len = 0;
				int ret = client->network_stack.write(&client->network_stack, send_info->message, send_info->msglen,
						left_ms(&send_info->start_time), &written_len);
				if (ret != QCLOUD_RET_SUCCESS) {
					Log_e("retansmit the message id %d failed.", send_info->msg_id, send_info->msglen);
					continue;
				}
			}
			else {
				send_info->node_state = COAP_NODE_STATE_INVALID;
				temp_node = node;

				if (send_info->handler != NULL) {
					message->type = COAP_MSG_ACK;
					message->user_context  = send_info->user_context;
					message->code_class = COAP_MSG_SDKINTERNAL_ERR;
					message->code_detail = COAP_MSG_CODE_600_TIMEOUT;
					message->msg_id = send_info->msg_id;
					send_info->handler(message, send_info->user_context);
				}
				else if (NULL != client->event_handle.h_fp){
					CoAPEventMessage event_msg = {0};
					if (send_info->acked) {
						event_msg.event_type = COAP_EVENT_SEPRESP_TIMEOUT;
					}
					else {
						event_msg.event_type = COAP_EVENT_ACK_TIMEOUT;
					}
					event_msg.message = (void *)(uintptr_t)(send_info->msg_id);
					client->event_handle.h_fp(client->event_handle.context, &event_msg);
				}
				else {
					Log_e("nether response callback nor event callback is set");
				}

				continue;
			}
		}
	}

	list_iterator_destroy(iter);
	HAL_MutexUnlock(client->lock_list_wait_ack);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 *  @brief Send ACK to server
 *
 *  @param client reference to CoAP client
 *  @param msgId  msg Id
 *
 *  @returns Operation status
 *  @retval 0 Success
 *  @retval <0 Error
 */
static int _coap_client_send_ack(CoAPClient *client, int msgId) {
	IOT_FUNC_ENTRY

    CoAPMessage ack = DEFAULT_COAP_MESSAGE;
    int ret = 0;

    if ((ret = coap_message_type_set(&ack, COAP_MSG_ACK)) != QCLOUD_RET_SUCCESS)
    {
        coap_message_destroy(&ack);
        IOT_FUNC_EXIT_RC(ret)
    }

    if ((ret = coap_message_id_set(&ack, msgId)) != QCLOUD_RET_SUCCESS)
    {
        coap_message_destroy(&ack);
        IOT_FUNC_EXIT_RC(ret)
    }

    ret = coap_message_send(client, &ack);
    coap_message_destroy(&ack);

    IOT_FUNC_EXIT_RC(ret)
}

static int _coap_piggyresp_message_handle(CoAPClient *client, CoAPMessage *message) {
	IOT_FUNC_ENTRY

	int rc = _coap_message_list_proc(client, message, PROCESS_PIGGY_CMD);

    IOT_FUNC_EXIT_RC(rc)
}

static int _coap_ack_message_handle(CoAPClient *client, CoAPMessage *message) {
	IOT_FUNC_ENTRY

	int rc = _coap_message_list_proc(client, message, PROCESS_ACK_CMD);

    IOT_FUNC_EXIT_RC(rc)
}

static int _coap_resp_message_handle(CoAPClient *client, CoAPMessage *message) {
	IOT_FUNC_ENTRY

	int rc = 0;

    if (COAP_MSG_CON == message->type) {
    	rc = _coap_client_send_ack(client, message->msg_id);
    	Log_d("send ack message for id: %d rc: %d", message->msg_id, rc);
    }

    rc = _coap_message_list_proc(client, message, PROCESS_RESP_CMD);

    IOT_FUNC_EXIT_RC(rc)
}

static void _coap_message_handle(CoAPClient *client, unsigned char *buf, unsigned short datalen) {
	IOT_FUNC_ENTRY

    int rc = QCLOUD_RET_SUCCESS;
    CoAPMessage     recv_message;
    memset(&recv_message, 0x00, sizeof(CoAPMessage));

    if ((rc = deserialize_coap_message(&recv_message, (char*)buf, datalen)) != QCLOUD_RET_SUCCESS) {
    	Log_e("deserialize message failed: %d", rc);
    }

    if (recv_message.type == COAP_MSG_ACK && COAP_MSG_IS_EMPTY_ACK(&recv_message)) {	//empty ACK
        Log_d("receive coap ACK message, id %d", recv_message.msg_id);
        _coap_ack_message_handle(client, &recv_message);
    }
    else if (recv_message.type == COAP_MSG_ACK && !COAP_MSG_IS_EMPTY(&recv_message)) {	//piggy Response
		Log_d("receive coap piggy ACK message, id %d", recv_message.msg_id);
		_coap_piggyresp_message_handle(client, &recv_message);
	}
    else if (recv_message.type == COAP_MSG_CON && COAP_MSG_IS_EMPTY_RSP(&recv_message)) {	//payload Response
    	Log_d("receive coap response message, id: %d", recv_message.msg_id);
        _coap_resp_message_handle(client, &recv_message);
    }
    else if (recv_message.type == COAP_MSG_NON && COAP_MSG_IS_EMPTY_RSP(&recv_message)) {	//payload Response
        	Log_d("receive coap response message, id: %d", recv_message.msg_id);
            _coap_resp_message_handle(client, &recv_message);
        }
    else {
    	Log_e("not recgonized recv message type");
    }

    if (recv_message.pay_load != NULL) {
    	HAL_Free(recv_message.pay_load);
    	recv_message.pay_load_len = 0;
    }

    IOT_FUNC_EXIT
}

static int _coap_message_list_add(CoAPClient *client, CoAPMessage *message, int len) {
	IOT_FUNC_ENTRY

	CoAPMsgSendInfo *send_info = (CoAPMsgSendInfo*)HAL_Malloc(sizeof(CoAPMsgSendInfo));
	if (send_info == NULL) {
		Log_e("no memory to malloc SendInfo");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
	}

	send_info->node_state	= COAP_NODE_STATE_NORMANL;
	send_info->acked        = 0;
	send_info->user_context = message->user_context;
	send_info->msg_id        = message->msg_id;
	send_info->handler      = message->handler;
	send_info->msglen       = len;

	if (COAP_MSG_CON == message->type) {
		send_info->retrans_count = 0;
	    InitTimer(&send_info->start_time);
	    countdown_ms(&send_info->start_time, client->command_timeout_ms);
	}
	else {

	}

	send_info->token_len     = message->token_len;
	memcpy(send_info->token, message->token, message->token_len);
	send_info->message      = (unsigned char *)HAL_Malloc(len);

	if (NULL != send_info->message) {
		memcpy(send_info->message, client->send_buf, len);
	}

	ListNode *node = list_node_new(send_info);
	if (NULL == node) {
		Log_e("run list_node_new is error!");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
	}

	HAL_MutexLock(client->lock_list_wait_ack);
	list_rpush(client->message_list, node);
	HAL_MutexUnlock(client->lock_list_wait_ack);

	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int coap_message_cycle(CoAPClient *client, uint32_t timeout_ms) {
	POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

	int rc = coap_message_recv(client, timeout_ms);

	if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_ERR_SSL_READ_TIMEOUT) {
		IOT_FUNC_EXIT_RC(rc)
	}

	CoAPMessage message = DEFAULT_COAP_MESSAGE;
	rc = _coap_message_list_proc(client, &message, PROCESS_WAIT_CMD);

	IOT_FUNC_EXIT_RC(rc)
}

ssize_t coap_message_send(CoAPClient *client, CoAPMessage *message)
{
	IOT_FUNC_ENTRY

    ssize_t ret;
    size_t written_len = 0;

    HAL_MutexLock(client->lock_send_buf);

    if ((ret = serialize_coap_message(message, (char*)client->send_buf,
    		client->send_buf_size)) < 0)
    {
        Log_e("failed to serialize coap message");
        HAL_MutexUnlock(client->lock_send_buf);
        IOT_FUNC_EXIT_RC(ret)
    }

    ret = client->network_stack.write(&client->network_stack, client->send_buf,
    		ret, 0, &written_len);

    if (ret == QCLOUD_RET_SUCCESS) {
    	if (message->type == COAP_MSG_CON && message->code_class == COAP_MSG_REQ) {
    		ret = _coap_message_list_add(client, message, written_len);
    		Log_i("add coap message id: %d into wait list ret: %d", message->msg_id, ret);
		} else {
			Log_i("The message doesn't need to be retransmitted");
		}
    }
    else {
    	Log_e("coap net fail to write rc: %d", ret);
    }

    HAL_MutexUnlock(client->lock_send_buf);

    IOT_FUNC_EXIT_RC(ret)
}

ssize_t coap_message_recv(CoAPClient *client, uint32_t timeout_ms)
{
	IOT_FUNC_ENTRY

    size_t read_lean = 0;
    int rc = 0;

    rc = client->network_stack.read(&client->network_stack, client->recv_buf, client->read_buf_size, timeout_ms, &read_lean);
    switch(rc) {
		case QCLOUD_RET_SUCCESS:
			_coap_message_handle(client, client->recv_buf, read_lean);
			break;
		case QCLOUD_ERR_SSL_READ_TIMEOUT:
			break;
		default:
			break;
    }

    IOT_FUNC_EXIT_RC(rc)
}

#ifdef __cplusplus
}
#endif


