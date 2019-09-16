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

/**
 *  @brief Free an option structure that was allocated by coap_msg_op_new
 *
 *  @param[in,out] op Pointer to the option structure
 */
__QCLOUD_STATIC__ void coap_message_option_destroy(coap_msg_option_t *option)
{
	QCLOUD_FUNC_ENTRY

    if (option->val) {
        osal_free(option->val);
    }
    osal_free(option);

	QCLOUD_FUNC_EXIT
}

/**
 *  @brief Deinitialise an option linked-list structure
 *
 *  @param[in,out] list Pointer to an option linked-list structure
 */
__QCLOUD_STATIC__ void coap_message_option_list_destroy(coap_message_t *message)
{
	QCLOUD_FUNC_ENTRY

    QCLOUD_POINTER_SANITY_CHECK_RTN(message);

    qcloud_list_t *curr, *next;
    coap_msg_option_t *option;

    if (qcloud_list_empty(&message->option_list)) {
        QCLOUD_FUNC_EXIT;
    }

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &message->option_list) {
        option = QCLOUD_LIST_ENTRY(curr, coap_msg_option_t, list);

        coap_message_option_destroy(option);
    }

    QCLOUD_FUNC_EXIT;
}

__QCLOUD_INTERNAL__ void coap_message_init(coap_message_t *message)
{
    message->version = COAP_VERSION;
    qcloud_list_init(&message->option_list);
}

__QCLOUD_INTERNAL__ int coap_message_token_get(qcloud_coap_client_t *client, char *buf)
{
    uint32_t token;

    token = client->message_token;

    buf[0] = ((token & 0x00FF) >> 0);
    buf[1] = ((token & 0xFF00) >> 8);
    buf[2] = ((token & 0xFF0000) >> 16);
    buf[3] = ((token & 0xFF000000) >> 24);

    ++client->message_token;

    return sizeof(uint32_t);
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_type_set(coap_message_t *message, uint8_t type)
{
	QCLOUD_FUNC_ENTRY

    message->type = type;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_code_set(coap_message_t *message, uint32_t code_class, uint32_t code_detail)
{
	QCLOUD_FUNC_ENTRY

    if (code_class > COAP_MSG_CODE_CLASS_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }

    if (code_detail > COAP_MSG_CODE_DETAIL_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }

    message->code_class     = code_class;
    message->code_detail    = code_detail;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_id_set(coap_message_t *message, uint16_t id)
{
	QCLOUD_FUNC_ENTRY

    if (id > COAP_MSG_ID_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }

    message->id = id;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_token_set(coap_message_t *message, char *buf, uint8_t len)
{
	QCLOUD_FUNC_ENTRY

    if (len > COAP_MSG_TOKEN_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }

    memcpy(message->token, buf, len);
    message->token_len = len;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_payload_set(coap_message_t *message, char *buf, size_t len)
{
	QCLOUD_FUNC_ENTRY

    if (len > 0 && !message->payload) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
    }

    message->payload_len = 0;

    if (len > 0) {
        memcpy(message->payload, buf, len);
        message->payload_len = len;
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_INTERNAL__ coap_msg_option_t *coap_message_option_construct(uint16_t option_code, uint32_t len, const char *val)
{
	QCLOUD_FUNC_ENTRY

    char *this_val = NULL;
	coap_msg_option_t *option = NULL;

	option = (coap_msg_option_t *)osal_malloc(sizeof(coap_msg_option_t));
    if (!option) {
        QCLOUD_LOG_E("memory alloc failed");
    	return NULL;
    }

    this_val = (char *)osal_malloc(len);
    if (!this_val) {
        osal_free(option);
        QCLOUD_LOG_E("memory alloc failed");
        return NULL;
    }

    option->option_code = option_code;
    option->val_len     = len;
    option->val         = this_val;

    memcpy(option->val, val, len);

    qcloud_list_init(&option->list);

    return option;
}

__QCLOUD_STATIC__ void coap_message_option_do_add(coap_message_t *message, coap_msg_option_t *option)
{
    coap_msg_option_t *iter;
    qcloud_list_t *curr, *option_list;

    option_list = &message->option_list;
    /* keep option_code in ascending order */
    QCLOUD_LIST_FOR_EACH(curr, option_list) {
        iter = QCLOUD_LIST_ENTRY(curr, coap_msg_option_t, list);
        if (option->option_code <= iter->option_code) {
            break;
        }
    }
    qcloud_list_add_tail(&option->list, curr);
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_option_add(coap_message_t *message, coap_msg_opt_code_t option_code, uint32_t len, const char *val)
{
	QCLOUD_FUNC_ENTRY

    QCLOUD_POINTER_SANITY_CHECK(message, QCLOUD_ERR_INVAL);

	coap_msg_option_t *option = NULL;

    option = coap_message_option_construct(option_code, len, val);
    if (!option) {
        QCLOUD_LOG_E("option alloc failed.");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
	}

    coap_message_option_do_add(message, option);

	QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_callback_set(coap_message_t *message, coap_resp_callback_t callback)
{
	QCLOUD_FUNC_ENTRY

    message->resp_cb = callback;

	QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_context_set(coap_message_t *message, void *context)
{
	QCLOUD_FUNC_ENTRY

    message->context = context;

	QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_INTERNAL__ void coap_message_destroy(coap_message_t *message)
{
	QCLOUD_FUNC_ENTRY

    coap_message_option_list_destroy(message);

    if (message->payload) {
        osal_free(message->payload);
    }

    memset(message, 0, sizeof(coap_message_t));

    QCLOUD_FUNC_EXIT
}

__QCLOUD_INTERNAL__ void coap_message_dump(coap_message_t* message)
{
    QCLOUD_LOG_I("version       = %u", message->version);
    QCLOUD_LOG_I("type          = %d", message->type);
    QCLOUD_LOG_I("code_class    = %u", message->code_class);
    QCLOUD_LOG_I("code_detail   = %u", message->code_detail);
    QCLOUD_LOG_I("id            = %d", message->id);
    QCLOUD_LOG_I("payload_len   = %d", message->payload_len);
    QCLOUD_LOG_I("payload: %s", message->payload);

    QCLOUD_LOG_I("token_len     = %u", message->token_len);
    QCLOUD_LOG_I("token: %s", message->token);
}

#ifdef __cplusplus
}
#endif

