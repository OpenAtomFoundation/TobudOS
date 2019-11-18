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
#include <errno.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#include "coap_client.h"

#include "utils_param_check.h"

#define COAP_MSG_OPLIST_LAST(list)         ((list)->last)                   /**< Get the last option in an option linked-list */
#define COAP_MSG_OPLIST_EMPTY(list)        ((list)->first == NULL)          /**< Indicate whether or not an option linked-list is empty */

#define Swap16(A) ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))

/**
 *  @brief Check a message for correctness
 *
 *  The following checks from RFC7252 are performed:
 *
 *  An Empty message has the Code field set to 0.00. The Token Length
 *  field MUST be set to 0 and bytes of data MUST NOT be present after
 *  the Message ID field.  If there are any bytes, they MUST be processed
 *  as a message format error.
 *
 *  The Reset message MUST echo the Message ID of the Confirmable message
 *  and MUST be Empty.
 *
 *  A Non-confirmable message always carries either a request or response
 *  and MUST NOT be Empty.
 *
 *  @param[in] msg Pointer to a message structure
 *  @returns Operation status
 *  @retval 0 Success
 *  @retval <0 Error
 */
static int _coap_msg_check(CoAPMessage *msg)
{
	IOT_FUNC_ENTRY

    if ((msg->code_class == 0) && (msg->code_detail == 0))
    {
        /* empty message */
        if ((msg->type == COAP_MSG_NON) || (msg->token_len != 0) || (!COAP_MSG_OPLIST_EMPTY(&msg->op_list))
        		|| (msg->pay_load_len != 0))
        {
            Log_e("coap msg op list is not empty");
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
    }
    else
    {
        /* non-empty message */
        if (msg->type == COAP_MSG_RST)
        {
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

/**
 *  @brief Allocate an option structure and add it to the end of an option linked-list structure
 *
 *  @param[in,out] list Pointer to an option linked-list structure
 *  @param[in] num Option number
 *  @param[in] len Option length
 *  @param[in] val Pointer to a buffer containing the option value
 *
 *  @returns Operation status
 *  @retval 0 Success
 *  @retval <0 Error
 */
static int _coap_msg_op_list_add_last(CoAPMsgOptionList *list, unsigned num, unsigned len, const char *val) {
	IOT_FUNC_ENTRY

    CoAPMsgOption *option = NULL;

	if ((option = qcloud_iot_coap_option_init(num, len, val)) == NULL) {
		Log_e("allocate new option failed.");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
	}

    if (list->first == NULL)
    {
        list->first = option;
        list->last = option;
    }
    else
    {
        list->last->next = option;
        list->last = option;
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

/**
 *  @brief Parse the header in a message
 *
 *  @param[out] msg Pointer to a message structure
 *  @param[in] buf Pointer to a buffer containing the message
 *  @param[in] len Length of the buffer
 *
 *  @returns Number of bytes parsed or error code
 *  @retval >0 Number of bytes parsed
 *  @retval <0 Error
 */
static ssize_t _coap_message_deserialize_header(CoAPMessage *msg, char *buf, size_t len)
{
	IOT_FUNC_ENTRY

    char *p = buf;

    if (len < 4)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    msg->version = (p[0] >> 6) & 0x03;
    if (msg->version != COAP_MSG_VER)
    {
        return -EINVAL;
    }

    msg->type = (p[0] >> 4) & 0x03;
    msg->token_len = p[0] & 0x0f;
    if (msg->token_len > sizeof(msg->token))
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    msg->code_detail = p[1] & 0x1f;
    msg->code_class = (p[1] >> 5) & 0x07;

    if ((msg->code_class != COAP_MSG_REQ) && (msg->code_class != COAP_MSG_SUCCESS)
    		&& (msg->code_class != COAP_MSG_CLIENT_ERR) && (msg->code_class != COAP_MSG_SERVER_ERR))
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_BADMSG)
    }

    msg->msg_id = Swap16(*((uint16_t *)(&p[2])));
    p += 4;
    len -= 4;

    IOT_FUNC_EXIT_RC(p - buf)
}

/**
 *  @brief Parse the token in a message
 *
 *  @param[out] msg Pointer to a message structure
 *  @param[in] buf Pointer to a buffer containing the message
 *  @param[in] len Length of the buffer
 *
 *  @returns Number of bytes parsed or error code
 *  @retval >0 Number of bytes parsed
 *  @retval <0 Error
 */
static ssize_t _coap_message_deserialize_token(CoAPMessage *msg, char *buf, size_t len)
{
	IOT_FUNC_ENTRY

    if (len < msg->token_len)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    memcpy(msg->token, buf, msg->token_len);

    IOT_FUNC_EXIT_RC(msg->token_len)
}

/**
 *  @brief Parse an option in a message
 *
 *  @param[in,out] msg Pointer to a message structure
 *  @param[in] buf Pointer to a buffer containing the message
 *  @param[in] len Length of the buffer
 *
 *  @returns Number of bytes parsed or error code
 *  @retval >0 Number of bytes parsed
 *  @retval <0 Error
 */
static ssize_t _coap_message_deserialize_option(CoAPMessage *msg, char *buf, size_t len)
{
	IOT_FUNC_ENTRY

    CoAPMsgOption *prev = NULL;
    unsigned op_delta = 0;
    unsigned op_len = 0;
    unsigned op_num = 0;
    char *p = buf;
    int ret = 0;

    if (len < 1)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }
    op_delta = (p[0] >> 4) & 0x0f;
    op_len = p[0] & 0x0f;
    if ((op_delta == 15) || (op_len == 15))
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }
    p++;
    len--;
    if (op_delta == 13)
    {
        if (len < 1)
        {
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
        op_delta += p[0];
        p++;
        len--;
    }
    else if (op_delta == 14)
    {
        if (len < 2)
        {
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
        op_delta = 269 + Swap16(*((uint16_t *)(&p[0])));
        p += 2;
        len -= 2;
    }
    if (op_len == 13)
    {
        if (len < 1)
        {
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
        op_len += p[0];
        p++;
        len--;
    }
    else if (op_len == 14)
    {
        if (len < 2)
        {
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
        op_len = 269 + Swap16(*((uint16_t *)(&p[0])));
        p += 2;
        len -= 2;
    }
    if (len < op_len)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }
    prev = COAP_MSG_OPLIST_LAST(&msg->op_list);
    if (prev == NULL)
    {
        op_num = op_delta;
    }
    else
    {
        op_num = COAP_MSG_OPTION_NUM(prev) + op_delta;
    }
    ret = _coap_msg_op_list_add_last(&msg->op_list, op_num, op_len, p);
    if (ret < 0)
    {
        IOT_FUNC_EXIT_RC(ret)
    }
    p += op_len;

    IOT_FUNC_EXIT_RC(p - buf)
}

/**
 *  @brief Parse the options in a message
 *
 *  @param[in,out] msg Pointer to a message structure
 *  @param[in] buf Pointer to a buffer containing the message
 *  @param[in] len Length of the buffer
 *
 *  @returns Number of bytes parsed or error code
 *  @retval >0 Number of bytes parsed
 *  @retval <0 Error
 */
static ssize_t _coap_message_deserialize_options(CoAPMessage *msg, char *buf, size_t len)
{
	IOT_FUNC_ENTRY

    ssize_t num = 0;
    char *p = buf;

    while (1)
    {
        if (((p[0] & 0xff) == 0xff) || (len == 0))
        {
            break;
        }
        num = _coap_message_deserialize_option(msg, p, len);
        if (num < 0)
        {
            return num;
        }
        p += num;
        len -= num;
    }

    IOT_FUNC_EXIT_RC(p - buf)
}

/**
 *  @brief Parse the payload in a message
 *
 *  @param[out] msg Pointer to a message structure
 *  @param[in] buf Pointer to a buffer containing the message
 *  @param[in] len Length of the buffer
 *
 *  @returns Number of bytes parsed or error code
 *  @retval >0 Number of bytes parsed
 *  @retval <0 Error
 */
static ssize_t _coap_message_deserialize_payload(CoAPMessage *msg, char *buf, size_t len)
{
	IOT_FUNC_ENTRY

    char *p = buf;

    if (len == 0)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
    }
    if ((p[0] & 0xff) != 0xff)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }
    p++;
    len--;
    if (len == 0)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    msg->pay_load = (char *)HAL_Malloc(len);

    if (msg->pay_load == NULL)
    {
        return -ENOMEM;
    }

    memcpy(msg->pay_load, p, len);
    msg->pay_load_len = len;
    p += len;

    IOT_FUNC_EXIT_RC(p - buf)
}

ssize_t deserialize_coap_message(CoAPMessage *message, char *buf, size_t len)
{
	IOT_FUNC_ENTRY

    ssize_t num = 0;
    char *p = buf;

    num = _coap_message_deserialize_header(message, p, len);
    if (num < 0)
    {
    	Log_e("coap_message_deserialize_header failed, num:%lu", num);
        coap_message_destroy(message);
        goto error;
    }

    p += num;
    len -= num;
    num = _coap_message_deserialize_token(message, p, len);
    if (num < 0)
    {
    	Log_e("coap_message_deserialize_token failed, num:%lu", num);
        coap_message_destroy(message);
        goto error;
    }

    p += num;
    len -= num;
    num = _coap_message_deserialize_options(message, p, len);
    if (num < 0)
    {
    	Log_e("coap_message_deserialize_options failed, num:%lu", num);
        coap_message_destroy(message);
        goto error;
    }

    p += num;
    len -= num;
    num = _coap_message_deserialize_payload(message, p, len);
    if (num < 0)
    {
    	Log_e("coap_message_deserialize_payload failed, num:%lu", num);
        coap_message_destroy(message);
        goto error;
    }

    IOT_FUNC_EXIT_RC(_coap_msg_check(message))

error:
	IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_INTERNAL)
}

#ifdef __cplusplus
}
#endif

