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
__QCLOUD_STATIC__ int coap_message_serialize_header(coap_message_t *message, char *buf, size_t len)
{
	QCLOUD_FUNC_ENTRY

    if (len < 4) {
        return -1;
    }

    buf[0] = (char)((COAP_VERSION << 6)
                  | ((message->type & 0x03) << 4)
                  | (message->token_len & 0x0f));
    buf[1] = (char)(((message->code_class & 0x07) << 5)
                  | (message->code_detail & 0x1f));

    buf[2] = (message->id & 0xFF00) >> 8;
    buf[3] = (message->id & 0x00FF);

    QCLOUD_FUNC_EXIT_RC(4)
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
__QCLOUD_STATIC__ int coap_message_serialize_token(coap_message_t *message, char *buf, size_t len)
{
	QCLOUD_FUNC_ENTRY

    if (len < message->token_len) {
        return -1;
    }

    memcpy(buf, message->token, message->token_len);

    return message->token_len;
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
__QCLOUD_STATIC__ int coap_message_option_format(coap_msg_option_t *option, uint16_t prev_option_code, char *buf, size_t buf_len)
{
	QCLOUD_FUNC_ENTRY

    char *p = NULL;
    uint32_t len = 0;
    uint16_t option_code_delta = 0;

    p = buf;

    option_code_delta = option->option_code - prev_option_code;
    len++;

    /* option delta */
    if (option_code_delta >= 269) {
        len += 2;
    } else if (option_code_delta >= 13) {
        len += 1;
    }

    /* option length */
    if (option->val_len >= 269) {
        len += 2;
    } else if (option->option_code >= 13) {
        len += 1;
    }

    /* option value */
    len += option->val_len;
    if (len > buf_len) {
        return QCLOUD_ERR_COAP_DATA_SIZE;
    }

    /* option delta */
    if (option_code_delta >= 269) {
        p[0] = 14 << 4;
    } else if (option_code_delta >= 13) {
        p[0] = 13 << 4;
    } else {
        p[0] = option_code_delta << 4;
    }

    /* option length */
    if (option->val_len >= 269) {
        p[0] |= 14;
    } else if (option->val_len >= 13) {
        p[0] |= 13;
    } else {
        p[0] |= option->val_len;
    }

    p++;
    buf_len--;

    /* option delta extended */
    if (option_code_delta >= 269) {
		*p          = (uint8_t)(((option_code_delta - 269) & 0xFF00) >> 8);
		*(p + 1)    = (uint8_t)(((option_code_delta - 269) & 0x00FF));
        p += 2;
        buf_len -= 2;
    } else if (option_code_delta >= 13) {
        p[0] = option_code_delta - 13;
        p++;
        buf_len--;
    }

    /* option length extended */
    if (option->val_len >= 269) {
        *p          = (unsigned char)(((option->val_len - 269) & 0xFF00) >> 8);
        *(p + 1)    = (unsigned char)(((option->val_len - 269) & 0x00FF));
        p += 2;
        buf_len -= 2;
    } else if (option->val_len >= 13) {
        p[0] = option->val_len - 13;
        p++;
        buf_len--;
    }

    /* option value */
    memcpy(p, option->val, option->val_len);
    p += option->val_len;

    return p - buf;
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
__QCLOUD_STATIC__ int coap_message_serialize_options(coap_message_t *message, char *buf, size_t buf_len)
{
    QCLOUD_FUNC_ENTRY;

    int len;
    char *p = NULL;
    uint16_t prev_option_code = 0;
    qcloud_list_t *curr;
    coap_msg_option_t *option;

    p = buf;

    QCLOUD_LIST_FOR_EACH(curr, &message->option_list) {
        option = QCLOUD_LIST_ENTRY(curr, coap_msg_option_t, list);

        // coap_message_option_destroy(option);
        len = coap_message_option_format(option, prev_option_code, p, buf_len);
        if (len < 0) {
            return len;
        }

        p += len;
        buf_len -= len;
        prev_option_code = option->option_code;
    }

    return p - buf;
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
__QCLOUD_STATIC__ int coap_message_serialize_payload(coap_message_t *message, char *buf, size_t buf_len)
{
    QCLOUD_FUNC_ENTRY;

    if (message->payload_len == 0) {
        return 0;
    }

    if (message->payload_len + 1 > buf_len) {
        return -1;
    }

    buf[0] = 0xff;
    memcpy(&buf[1], message->payload, message->payload_len);

    return message->payload_len + 1;
}

__QCLOUD_INTERNAL__ int coap_message_serialize(coap_message_t *message, char *buf, size_t buf_len)
{
    QCLOUD_FUNC_ENTRY;

    int len = 0;
    char *p = buf;

    if (!message || !buf) {
        return -1;
    }

    len = coap_message_serialize_header(message, p, buf_len);
    if (len < 0) {
        QCLOUD_LOG_E("serialize header fail len: %lu", len);
        return -1;
    }

    p += len;
    buf_len -= len;
    len = coap_message_serialize_token(message, p, buf_len);
    if (len < 0) {
        QCLOUD_LOG_E("erialize token fail len: %lu", len);
        return -1;
    }

    p += len;
    buf_len -= len;
    len = coap_message_serialize_options(message, p, buf_len);
    if (len < 0) {
        QCLOUD_LOG_E("serialize options fail len: %lu", len);
        return -1;
    }

    p += len;
    buf_len -= len;
    len = coap_message_serialize_payload(message, p, buf_len);
    if (len < 0) {
        QCLOUD_LOG_E("coap_message_serialize_payload fail num=%lu", len);
        return -1;
    }

    p += len;

    return p - buf;
}

#ifdef __cplusplus
}
#endif

