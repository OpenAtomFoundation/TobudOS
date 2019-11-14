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

#define COAP_MSG_OPLIST_FIRST(list)       ((list)->first)                  /**< Get the first option from an option linked-list */

/**
 *  @brief Format the header in a message
 *
 *  @param[in] message Pointer to a message structure
 *  @param[out] buf Pointer to a buffer to contain the formatted message
 *  @param[in] len Length of the buffer
 *
 *  @returns Length of the formatted message or error code
 *  @retval >0 Length of the formatted message
 *  @retval <0 Error
 */
static ssize_t _coap_message_serialize_header(CoAPMessage *message, char *buf, size_t len) {
	IOT_FUNC_ENTRY

    if (len < 4)
    {
        return -ENOSPC;
    }

    buf[0] = (char)((COAP_MSG_VER << 6)
                  | ((message->type & 0x03) << 4)
                  | (message->token_len & 0x0f));
    buf[1] = (char)(((message->code_class & 0x07) << 5)
                  | (message->code_detail & 0x1f));

    buf[2] = (message->msg_id & 0xFF00) >> 8;
    buf[3] = (message->msg_id & 0x00FF);

    IOT_FUNC_EXIT_RC(4)
}

/**
 *  @brief Format the token in a message
 *
 *  @param[in] message Pointer to a message structure
 *  @param[out] buf Pointer to a buffer to contain the formatted message
 *  @param[in] len Length of the buffer
 *
 *  @returns Length of the formatted message or error code
 *  @retval >0 Length of the formatted message
 *  @retval <0 Error
 */
static ssize_t _coap_message_serialize_token(CoAPMessage *message, char *buf, size_t len) {
	IOT_FUNC_ENTRY

    if (len < message->token_len)
    {
        return -ENOSPC;
    }
    memcpy(buf, message->token, message->token_len);

    IOT_FUNC_EXIT_RC(message->token_len)
}

/**
 *  @brief Format an option in a message
 *
 *  @param[in] op Pointer to an option structure
 *  @param[in] prev_num option number of the previous option
 *  @param[out] buf Pointer to a buffer to contain the formatted message
 *  @param[in] len Length of the buffer
 *
 *  @returns Length of the formatted message or error code
 *  @retval >0 Length of the formatted message
 *  @retval <0 Error
 */
static ssize_t _coap_msg_format_op(CoAPMsgOption *op, unsigned prev_num, char *buf, size_t len) {
	IOT_FUNC_ENTRY

    unsigned short op_delta = 0;
    unsigned num = 0;
    char *p = buf;

    op_delta = op->option_num - prev_num;
    num++;

    /* option delta */
    if (op_delta >= 269)
    {
        num += 2;
    }
    else if (op_delta >= 13)
    {
        num += 1;
    }

    /* option length */
    if (op->val_len >= 269)
    {
        num += 2;
    }
    else if (op->option_num >= 13)
    {
        num += 1;
    }

    /* option value */
    num += op->val_len;
    if (num > len)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    /* option delta */
    if (op_delta >= 269)
    {
        p[0] = 14 << 4;
    }
    else if (op_delta >= 13)
    {
        p[0] = 13 << 4;
    }
    else
    {
        p[0] = op_delta << 4;
    }

    /* option length */
    if (op->val_len >= 269)
    {
        p[0] |= 14;
    }
    else if (op->val_len >= 13)
    {
        p[0] |= 13;
    }
    else
    {
        p[0] |= op->val_len;
    }
    p++;
    len--;

    /* option delta extended */
    if (op_delta >= 269)
    {
		*p     = (unsigned char)(((op_delta - 269) & 0xFF00) >> 8);
		*(p+1) = (unsigned char)(((op_delta - 269) & 0x00FF));
        p += 2;
        len -= 2;
    }
    else if (op_delta >= 13)
    {
        p[0] = op_delta - 13;
        p++;
        len--;
    }

    /* option length extended */
    if (op->val_len >= 269)
    {
        *p     = (unsigned char)(((op->val_len - 269) & 0xFF00) >> 8);
        *(p+1) = (unsigned char)(((op->val_len - 269) & 0x00FF));
        p += 2;
        len -= 2;
    }
    else if (op->val_len >= 13)
    {
        p[0] = op->val_len - 13;
        p++;
        len--;
    }

    /* option value */
    memcpy(p, op->val, op->val_len);
    p += op->val_len;

    IOT_FUNC_EXIT_RC(p - buf)
}

/**
 *  @brief Format the options in a message
 *
 *  @param[in] message Pointer to a message structure
 *  @param[out] buf Pointer to a buffer to contain the formatted message
 *  @param[in] len Length of the buffer
 *
 *  @returns Length of the formatted message or error code
 *  @retval >0 Length of the formatted message
 *  @retval <0 Error
 */
static ssize_t _coap_message_serialize_options(CoAPMessage *message, char *buf, size_t len) {
    IOT_FUNC_ENTRY;

    CoAPMsgOption *op = NULL;
    unsigned prev_num = 0;
    ssize_t num = 0;
    char *p = buf;

    op = COAP_MSG_OPLIST_FIRST(&message->op_list);
    while (op != NULL)
    {
        num = _coap_msg_format_op(op, prev_num, p, len);
        if (num < 0)
        {
            return num;
        }
        p += num;
        len -= num;
        prev_num = COAP_MSG_OPTION_NUM(op);
        op = COAP_MSG_OP_NEXT(op);
    }

    IOT_FUNC_EXIT_RC(p - buf)
}

/**
 *  @brief Format the payload in a message
 *
 *  @param[in] message Pointer to a message structure
 *  @param[out] buf Pointer to a buffer to contain the formatted message
 *  @param[in] len Length of the buffer
 *
 *  @returns Length of the formatted message or error code
 *  @retval >0 Length of the formatted message
 *  @retval <0 Error
 */
static ssize_t _coap_message_serialize_payload(CoAPMessage *message, char *buf, size_t len) {
    IOT_FUNC_ENTRY;

    if (message->pay_load_len == 0)
    {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
    }

    if (message->pay_load_len + 1 > len)
    {
        return -ENOSPC;
    }

    buf[0] = 0xff;
    memcpy(&buf[1], message->pay_load, message->pay_load_len);

    IOT_FUNC_EXIT_RC(message->pay_load_len + 1)
}

ssize_t serialize_coap_message(CoAPMessage *message, char *buf, size_t len)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(message, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    ssize_t num = 0;
    char *p = buf;

    num = _coap_message_serialize_header(message, p, len);
    if (num < 0)
    {
        Log_e("coap_message_serialize_header fail num=%lu", num);
        goto error;
    }

    p += num;
    len -= num;
    num = _coap_message_serialize_token(message, p, len);
    if (num < 0)
    {
        Log_e("coap_message_serialize_token fail num=%lu", num);
        goto error;
    }

    p += num;
    len -= num;
    num = _coap_message_serialize_options(message, p, len);
    if (num < 0)
    {
        Log_e("coap_message_serialize_options fail num=%lu", num);
        goto error;
    }

    p += num;
    len -= num;
    num = _coap_message_serialize_payload(message, p, len);
    if (num < 0)
    {
        Log_e("coap_message_serialize_payload fail num=%lu", num);
        goto error;
    }

    p += num;

    IOT_FUNC_EXIT_RC(p - buf)

error:
	IOT_FUNC_EXIT_RC(QCLOUD_ERR_COAP_INTERNAL)
}

#ifdef __cplusplus
}
#endif


