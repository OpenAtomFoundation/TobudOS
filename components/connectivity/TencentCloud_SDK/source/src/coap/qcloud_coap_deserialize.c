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

#define COAP_SWAP_UINT16(n) ((((uint16_t)(n) & 0xff00) >> 8) | (((uint16_t)(n) & 0x00ff) << 8))

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
__QCLOUD_STATIC__ qcloud_err_t coap_message_verify(coap_message_t *message)
{
	QCLOUD_FUNC_ENTRY

    if (COAP_MSG_IS_EMPTY(message)) {
        /* empty message */
        if (message->type == COAP_MSG_TYPE_NON ||
            message->token_len != 0 ||
            !qcloud_list_empty(&message->option_list) ||
            message->payload_len != 0) {
            QCLOUD_LOG_E("message option not empty");
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
    }

    /* non-empty message */
    if (message->type == COAP_MSG_TYPE_RST) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
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
__QCLOUD_STATIC__ qcloud_err_t coap_message_option_list_add(coap_message_t *message, uint16_t option_code, uint32_t len, const char *val)
{
	QCLOUD_FUNC_ENTRY

    coap_msg_option_t *option = NULL;

    option = coap_message_option_construct(option_code, len, val);
	if (!option) {
		QCLOUD_LOG_E("allocate new option failed.");
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
	}

    qcloud_list_add_tail(&option->list, &message->option_list);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
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
__QCLOUD_STATIC__ int coap_message_deserialize_header(coap_message_t *message, char *buf, size_t len)
{
	QCLOUD_FUNC_ENTRY

    char *p = buf;

    if (len < 4) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE);
    }

    message->version = (p[0] >> 6) & 0x03;
    if (message->version != COAP_VERSION) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    message->type = (p[0] >> 4) & 0x03;
    message->token_len = p[0] & 0x0f;
    if (message->token_len > sizeof(message->token)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    message->code_detail = p[1] & 0x1f;
    message->code_class = (p[1] >> 5) & 0x07;

    if (message->code_class != COAP_CODE_CLASS_REQ &&
        message->code_class != COAP_CODE_CLASS_SUCCESS &&
        message->code_class != COAP_CODE_CLASS_CLIENT_ERR &&
        message->code_class != COAP_CODE_CLASS_SERVER_ERR) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_BADMSG)
    }

    message->id = COAP_SWAP_UINT16(*((uint16_t *)(&p[2])));
    p += 4;
    len -= 4;

    QCLOUD_FUNC_EXIT_RC(p - buf)
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
__QCLOUD_STATIC__ int coap_message_deserialize_token(coap_message_t *message, char *buf, size_t len)
{
	QCLOUD_FUNC_ENTRY

    if (len < message->token_len) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    memcpy(message->token, buf, message->token_len);

    QCLOUD_FUNC_EXIT_RC(message->token_len)
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
__QCLOUD_STATIC__ int coap_message_deserialize_option(coap_message_t *message, char *buf, size_t len)
{
	QCLOUD_FUNC_ENTRY

    char *p = buf;
    qcloud_err_t rc;
    uint16_t option_code_delta = 0, option_len = 0, option_code = 0;

    if (len < 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    option_code_delta = (p[0] >> 4) & 0x0f;
    option_len = p[0] & 0x0f;
    if ((option_code_delta == 15) || (option_len == 15)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    p++;
    len--;
    if (option_code_delta == 13) {
        if (len < 1) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
        option_code_delta += p[0];
        p++;
        len--;
    } else if (option_code_delta == 14) {
        if (len < 2) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
        option_code_delta = 269 + COAP_SWAP_UINT16(*((uint16_t *)(&p[0])));
        p += 2;
        len -= 2;
    }

    if (option_len == 13) {
        if (len < 1) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
        option_len += p[0];
        p++;
        len--;
    } else if (option_len == 14) {
        if (len < 2) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
        }
        option_len = 269 + COAP_SWAP_UINT16(*((uint16_t *)(&p[0])));
        p += 2;
        len -= 2;
    }
    if (len < option_len) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    option_code += option_code_delta;

    rc = coap_message_option_list_add(message, option_code, option_len, p);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, -1);

    p += option_len;

    QCLOUD_FUNC_EXIT_RC(p - buf)
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
__QCLOUD_STATIC__ int coap_message_deserialize_options(coap_message_t *message, char *buf, size_t buf_len)
{
	QCLOUD_FUNC_ENTRY

    int len = 0;
    char *p = buf;

    while ((p[0] & 0xff) != 0xff && buf_len != 0) {
        len = coap_message_deserialize_option(message, p, buf_len);
        if (len < 0) {
            return len;
        }

        p += len;
        buf_len -= len;
    }

    QCLOUD_FUNC_EXIT_RC(p - buf)
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
__QCLOUD_STATIC__ int coap_message_deserialize_payload(coap_message_t *message, char *buf, size_t len)
{
	QCLOUD_FUNC_ENTRY

    char *p = buf;

    if (len == 0) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
    }

    if ((p[0] & 0xff) != 0xff) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    p++;
    len--;
    if (len == 0) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_DATA_SIZE)
    }

    message->payload = (char *)osal_malloc(len);

    if (!message->payload){
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL)
    }

    memcpy(message->payload, p, len);
    message->payload_len = len;
    p += len;

    QCLOUD_FUNC_EXIT_RC(p - buf)
}

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_deserialize(coap_message_t *message, char *buf, size_t buf_len)
{
	QCLOUD_FUNC_ENTRY

    int len = 0;
    char *p = buf;

    len = coap_message_deserialize_header(message, p, buf_len);
    if (len < 0) {
    	QCLOUD_LOG_E("coap_message_deserialize_header failed, num:%lu", len);
        goto errout;
    }

    p += len;
    buf_len -= len;
    len = coap_message_deserialize_token(message, p, buf_len);
    if (len < 0) {
    	QCLOUD_LOG_E("coap_message_deserialize_token failed, num:%lu", len);
        goto errout;
    }

    p += len;
    buf_len -= len;
    len = coap_message_deserialize_options(message, p, buf_len);
    if (len < 0) {
    	QCLOUD_LOG_E("coap_message_deserialize_options failed, num:%lu", len);
        goto errout;
    }

    p += len;
    buf_len -= len;
    len = coap_message_deserialize_payload(message, p, buf_len);
    if (len < 0) {
    	QCLOUD_LOG_E("coap_message_deserialize_payload failed, num:%lu", len);
        goto errout;
    }

    QCLOUD_FUNC_EXIT_RC(coap_message_verify(message))

errout:
    coap_message_destroy(message);
	QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_COAP_INTERNAL)
}

#ifdef __cplusplus
}
#endif

