/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file data_template_property.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-08-23
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-08-23 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_data_template.h"
#include "data_template.h"

/**
 * @brief Down method type.
 *
 */
typedef enum {
    PROPERTY_DOWN_METHOD_TYPE_CONTROL = 0,
    PROPERTY_DOWN_METHOD_TYPE_REPORT_REPLY,
    PROPERTY_DOWN_METHOD_TYPE_GET_STATUS_REPLY,
    PROPERTY_DOWN_METHOD_TYPE_REPORT_INFO_REPLY,
    PROPERTY_DOWN_METHOD_TYPE_CLEAR_CONTROL_REPLY,
} PropertyDownMethodType;

/**
 * @brief Parse payload and callback.
 *
 * @param[in] type @see PropertyDownMethodType
 * @param[in] message message from cloud
 * @param[in] callback callback for user
 * @param[in,out] usr_data user data used in callback
 */
static void _parse_method_payload_and_callback(PropertyDownMethodType type, const MQTTMessage *message,
                                               const PropertyMessageCallback *callback, void *usr_data)
{
    int            rc = 0, code;
    UtilsJsonValue client_token, value_code, params, reported, control;

    // get client token
    rc = utils_json_value_get("clientToken", strlen("clientToken"), message->payload_str, message->payload_len,
                              &client_token);
    if (rc) {
        goto error;
    }

    // get code
    if (PROPERTY_DOWN_METHOD_TYPE_REPORT_REPLY == type || PROPERTY_DOWN_METHOD_TYPE_GET_STATUS_REPLY == type ||
        PROPERTY_DOWN_METHOD_TYPE_REPORT_INFO_REPLY == type || PROPERTY_DOWN_METHOD_TYPE_CLEAR_CONTROL_REPLY == type) {
        rc = utils_json_value_get("code", strlen("code"), message->payload_str, message->payload_len, &value_code);
        if (rc) {
            goto error;
        }

        rc = utils_json_value_data_get(value_code, UTILS_JSON_VALUE_TYPE_INT32, &code);
        if (rc) {
            goto error;
        }
    }

    // callback
    switch (type) {
        case PROPERTY_DOWN_METHOD_TYPE_CONTROL:
            if (callback->method_control_callback) {
                rc = utils_json_value_get("params", strlen("params"), message->payload_str, message->payload_len,
                                          &params);
                if (rc) {
                    goto error;
                }
                callback->method_control_callback(client_token, params, usr_data);
            }
            break;
        case PROPERTY_DOWN_METHOD_TYPE_REPORT_REPLY:
            if (callback->method_report_reply_callback) {
                callback->method_report_reply_callback(client_token, code, usr_data);
            }
            break;
        case PROPERTY_DOWN_METHOD_TYPE_GET_STATUS_REPLY:
            if (callback->method_get_status_reply_callback) {
                reported.value     = NULL;
                reported.value_len = 0;
                control.value      = NULL;
                control.value_len  = 0;
                rc = utils_json_value_get("data.reported", strlen("data.reported"), message->payload_str,
                                          message->payload_len, &reported);
                rc &= utils_json_value_get("data.control", strlen("data.control"), message->payload_str,
                                           message->payload_len, &control);
                if (rc) {
                    goto error;
                }
                callback->method_get_status_reply_callback(client_token, code, reported, control, usr_data);
            }
            break;
        case PROPERTY_DOWN_METHOD_TYPE_REPORT_INFO_REPLY:
            if (callback->method_report_info_reply_callback) {
                callback->method_report_info_reply_callback(client_token, code, usr_data);
            }
            break;
        case PROPERTY_DOWN_METHOD_TYPE_CLEAR_CONTROL_REPLY:
            if (callback->method_clear_control_reply_callback) {
                callback->method_clear_control_reply_callback(client_token, code, usr_data);
            }
            break;
        default:
            break;
    }

    return;
error:
    Log_e("invalid format of payload!");
    return;
}

/**
 * @brief Mqtt message callback for property topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see DataTemplateContext
 */
void data_template_property_message_handler(void *client, const MQTTMessage *message, void *usr_data)
{
    const char *property_down_method_str[] = {
        "control",              // PROPERTY_DOWN_METHOD_TYPE_CONTROL
        "report_reply",         // PROPERTY_DOWN_METHOD_TYPE_REPORT_REPLY
        "get_status_reply",     // PROPERTY_DOWN_METHOD_TYPE_GET_STATUS_REPLY
        "report_info_reply",    // PROPERTY_DOWN_METHOD_TYPE_REPORT_INFO_REPLY
        "clear_control_reply",  // PROPERTY_DOWN_METHOD_TYPE_CLEAR_CONTROL_REPLY
    };

    int rc, i = 0;

    DataTemplateContext *data_template_context = (DataTemplateContext *)usr_data;
    UtilsJsonValue       method;

    Log_d("receive property message:%.*s", message->payload_len, message->payload_str);

    rc = utils_json_value_get("method", strlen("method"), message->payload_str, message->payload_len, &method);
    if (rc) {
        return;
    }

    for (i = PROPERTY_DOWN_METHOD_TYPE_CONTROL; i <= PROPERTY_DOWN_METHOD_TYPE_CLEAR_CONTROL_REPLY; i++) {
        if (!strncmp(method.value, property_down_method_str[i], method.value_len)) {
            _parse_method_payload_and_callback(i, message, &data_template_context->property_callback,
                                               data_template_context->usr_data);
        }
    }
}

/**
 * @brief Publish message to property topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] publish_type @see PropertyUpMethodType
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] params @see PropertyPublishParams
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int data_template_property_publish(void *client, PropertyUpMethodType publish_type, char *buf, int buf_len,
                                   PropertyPublishParams params)
{
    static uint32_t token_num = 0;

    int len = 0;

    switch (publish_type) {
        case PROPERTY_UP_METHOD_TYPE_REPORT:
            len = HAL_Snprintf(buf, buf_len, "{\"method\":\"report\",\"clientToken\":\"property-%u\",\"params\":%s}",
                               token_num++, params.json);
            break;
        case PROPERTY_UP_METHOD_TYPE_REPORT_INFO:
            len =
                HAL_Snprintf(buf, buf_len, "{\"method\":\"report_info\",\"clientToken\":\"property-%u\",\"params\":%s}",
                             token_num++, params.json);
            break;
        case PROPERTY_UP_METHOD_TYPE_GET_STATUS:
            len =
                HAL_Snprintf(buf, buf_len, "{\"method\":\"get_status\",\"clientToken\":\"property-%u\"}", token_num++);
            break;
        case PROPERTY_UP_METHOD_TYPE_CLEAR_CONTROL:
            len = HAL_Snprintf(buf, buf_len, "{\"method\":\"clear_control\",\"clientToken\":\"property-%u\"}",
                               token_num++);
            break;
        case PROPERTY_UP_METHOD_TYPE_CONTROL_REPLY:
            len = HAL_Snprintf(buf, buf_len, "{\"method\":\"control_reply\",\"clientToken\":\"%.*s\",\"code\":%d}",
                               params.control_reply.client_token.value_len, params.control_reply.client_token.value,
                               params.control_reply.code);
            break;
        default:
            return QCLOUD_ERR_FAILURE;
    }

    return data_template_publish(client, DATA_TEMPLATE_TYPE_PROPERTY, QOS0, buf, len);
}
