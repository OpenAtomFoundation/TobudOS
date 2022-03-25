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
 * @file data_template_event.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-09-26
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-09-26 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "data_template.h"

/**
 * @brief Mqtt message callback for event topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see DataTemplateContext
 */
void data_template_event_message_handler(void *client, const MQTTMessage *message, void *usr_data)
{
    DataTemplateContext *data_template_context = (DataTemplateContext *)usr_data;

    int            rc, code = 0;
    UtilsJsonValue method, client_token, value_code;

    Log_d("receive event message:%.*s", message->payload_len, message->payload_str);

    rc = utils_json_value_get("method", strlen("method"), message->payload_str, message->payload_len, &method);
    if (rc) {
        return;
    }

    if (!strncmp(method.value, "event_reply", method.value_len)) {
        if (data_template_context->event_callback.method_event_reply_callback) {
            rc = utils_json_value_get("clientToken", strlen("clientToken"), message->payload_str, message->payload_len,
                                      &client_token);
            if (rc) {
                goto error;
            }

            rc = utils_json_value_get("code", strlen("code"), message->payload_str, message->payload_len, &value_code);
            if (rc) {
                goto error;
            }
            rc = utils_json_value_data_get(value_code, UTILS_JSON_VALUE_TYPE_INT32, &code);
            if (rc) {
                goto error;
            }
            data_template_context->event_callback.method_event_reply_callback(client_token, code,
                                                                              data_template_context->usr_data);
        }
    }
    return;
error:
    Log_e("invalid format of payload!");
}

/**
 * @brief Publish message to event topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] data @see IotDataTemplateEventData
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int data_template_event_reply_publish(void *client, char *buf, int buf_len, IotDataTemplateEventData data)
{
    const char *event_type[] = {
        "info",   // IOT_DATA_TEMPLATE_EVENT_TYPE_INFO
        "alert",  // IOT_DATA_TEMPLATE_EVENT_TYPE_ALERT
        "fault",  // IOT_DATA_TEMPLATE_EVENT_TYPE_FAULT
    };

    static uint32_t token_num = 0;

    int len = HAL_Snprintf(
        buf, buf_len,
        "{\"method\":\"event_post\",\"clientToken\":\"event-%u\",\"eventId\":\"%s\",\"type\":\"%s\",\"params\":%s}",
        token_num++, data.event_id, event_type[data.type], data.params);
    return data_template_publish(client, DATA_TEMPLATE_TYPE_EVENT, QOS0, buf, len);
}
