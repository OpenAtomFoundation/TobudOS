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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#include "coap_client.h"

#include "utils_param_check.h"

/**
 *  @brief Free an option structure that was allocated by coap_msg_op_new
 *
 *  @param[in,out] op Pointer to the option structure
 */
static void _coap_msg_op_delete(CoAPMsgOption *option) {
	IOT_FUNC_ENTRY
    HAL_Free(option->val);
    HAL_Free(option);
	IOT_FUNC_EXIT
}

/**
 *  @brief Deinitialise an option linked-list structure
 *
 *  @param[in,out] list Pointer to an option linked-list structure
 */
static void _coap_msg_op_list_destroy(CoAPMsgOptionList *list) {
	IOT_FUNC_ENTRY

    CoAPMsgOption *prev = NULL;
    CoAPMsgOption *option = NULL;

    option = list->first;
    while (option != NULL)
    {
        prev = option;
        option = option->next;
        _coap_msg_op_delete(prev);
    }
    memset(list, 0, sizeof(CoAPMsgOptionList));

    IOT_FUNC_EXIT
}

uint16_t get_next_coap_msg_id(CoAPClient *pClient) {
    IOT_FUNC_ENTRY

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    unsigned int id = 0;
    id = pClient->next_msg_id = (uint16_t) ((COAP_MSG_MAX_MSG_ID == pClient->next_msg_id) ? 1 : (pClient->next_msg_id + 1));

    IOT_FUNC_EXIT_RC(id)
}

unsigned int get_coap_message_token(CoAPClient *client, char *tokenData)
{
    unsigned int value = client->message_token;
    tokenData[0] = ((value & 0x00FF) >> 0);
    tokenData[1] = ((value & 0xFF00) >> 8);
    tokenData[2] = ((value & 0xFF0000) >> 16);
    tokenData[3] = ((value & 0xFF000000) >> 24);
    client->message_token++;
    return sizeof(unsigned int);
}

int coap_message_type_set(CoAPMessage *message, unsigned type) {
	IOT_FUNC_ENTRY

    if ((type != COAP_MSG_CON)
     && (type != COAP_MSG_NON)
     && (type != COAP_MSG_ACK)
     && (type != COAP_MSG_RST))
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }
    message->type = type;

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int coap_message_code_set(CoAPMessage *message, unsigned code_class, unsigned code_detail) {
	IOT_FUNC_ENTRY

    if (code_class > COAP_MSG_MAX_CODE_CLASS)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }
    if (code_detail > COAP_MSG_MAX_CODE_DETAIL)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }
    message->code_class = code_class;
    message->code_detail = code_detail;

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int coap_message_id_set(CoAPMessage *message, unsigned msg_id) {
	IOT_FUNC_ENTRY

    if (msg_id > COAP_MSG_MAX_MSG_ID)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }
    message->msg_id = msg_id;

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int coap_message_token_set(CoAPMessage *message, char *buf, size_t len) {
	IOT_FUNC_ENTRY

    if (len > COAP_MSG_MAX_TOKEN_LEN)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }
    memcpy(message->token, buf, len);
    message->token_len = len;

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int coap_message_payload_set(CoAPMessage *message, char *buf, size_t len) {
	IOT_FUNC_ENTRY

    message->pay_load_len = 0;

    if (len > 0)
    {
        if (message->pay_load == NULL)
        {
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
        }

        memcpy(message->pay_load, buf, len);
        message->pay_load_len = len;
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int coap_message_option_add(CoAPMessage *message, unsigned num, unsigned len, const char *val) {
	IOT_FUNC_ENTRY

	CoAPMsgOption *prev = NULL;
	CoAPMsgOption *option = NULL;
	CoAPMsgOptionList *list = &message->op_list;

    if ((option = qcloud_iot_coap_option_init(num, len, val)) == NULL) {
			Log_e("allocate new option failed.");
			IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
	}

	if (list->first == NULL) {
		/* empty list */
		list->first = option;
		list->last = option;
		IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
	}

	if (option->option_num < list->first->option_num) {
		/* start of the list */
		option->next = list->first;
		list->first = option;
		IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
	}

	prev = list->first;
	while (prev != list->last) {
		/* middle of the list */
		if ((prev->option_num <= option->option_num) && (option->option_num < prev->next->option_num))
		{
			option->next = prev->next;
			prev->next = option;
			IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
		}
		prev = prev->next;
	}

	/* end of the list */
	list->last->next = option;
	list->last = option;

	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int coap_message_callback_set(CoAPMessage *message, OnRespCallback callback) {
	IOT_FUNC_ENTRY

    message->handler = callback;

	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int coap_message_context_set(CoAPMessage *message, void *userContext) {
	IOT_FUNC_ENTRY

    message->user_context = userContext;

	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

CoAPMsgOption * qcloud_iot_coap_option_init(unsigned num, unsigned len, const char *val) {
	IOT_FUNC_ENTRY

	CoAPMsgOption *option = NULL;

	option = (CoAPMsgOption *) HAL_Malloc (sizeof(CoAPMsgOption));
    if (option == NULL) {
    	Log_e("no space to malloc option");
    	return NULL;
    }

    option->option_num = num;
    option->val_len = len;
    option->val = (char *) HAL_Malloc (len);
    if (option->val == NULL)
    {
        HAL_Free(option);
        option = NULL;
        Log_e("no space to malloc option");
        return NULL;
    }

    memcpy(option->val, val, len);
    option->next = NULL;

    return option;
}

void coap_message_destroy(CoAPMessage *message)
{
	IOT_FUNC_ENTRY

    _coap_msg_op_list_destroy(&message->op_list);

    if (message->pay_load != NULL)
    {
        HAL_Free(message->pay_load);
    }

    memset(message, 0, sizeof(CoAPMessage));

    IOT_FUNC_EXIT
}

void coap_msg_dump(CoAPMessage* msg)
{
    Log_i("msg->version      = %u",msg->version);
    Log_i("msg->type         = %d",msg->type);
    Log_i("msg->code_class   = %u",msg->code_class);
    Log_i("msg->code_detail  = %u",msg->code_detail);
    Log_i("msg->msg_id       = %d",msg->msg_id);
    Log_i("msg->pay_load_len = %d",msg->pay_load_len);
    Log_i("msg->pay_load: %s",msg->pay_load);

    Log_i("msg->token_len = %u",msg->token_len);
    Log_i("msg->token: %s",msg->token);
    return;
}


#ifdef __cplusplus
}
#endif
