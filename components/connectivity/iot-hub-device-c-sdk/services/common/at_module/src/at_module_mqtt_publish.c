/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2022 THL A29 Limited, a Tencent company.All rights reserved.
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
 * @file at_module_mqtt_publish.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-04-25
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-04-25 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "at_module.h"

/**
 * @brief Check if topic match
 *
 * @param[in] topic_filter topic name filter, wildcard is supported
 * @param[in] topic_name topic name, no wildcard
 * @param[in] topic_name_len length of topic name
 * @return 1 for matched, 0 for no matched
 *
 * @note  assume topic filter and name is in correct format;
 * # can only be at end;
 * + and # can only be next to separator.
 */
static uint8_t _is_topic_matched(char *topic_filter, const char *topic_name, uint16_t topic_name_len)
{
    char *curf     = topic_filter;
    char *curn     = (char *)topic_name;
    char *curn_end = curn + topic_name_len;

    while (*curf && curn < curn_end) {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+') {  // skip until we meet the next separator, or end of string
            char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/') nextpos = ++curn + 1;
        } else if (*curf == '#')
            curn = curn_end - 1;  // skip until end of string
        curf++;
        curn++;
    };

    return (curn == curn_end) && (*curf == '\0');
}

/**
 * @brief Deliver message to subscribe message handler.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_name topic name, no wildcard
 * @param[in] payload publish packet payload
 * @param[in] payload_len payload length
 */
void qcloud_iot_deliver_message(QcloudIotClient *client, char *topic_name, char *payload, int payload_len)
{
    int i;

    MQTTMessage message = {
        .qos         = QOS0,  // no use
        .retain      = 0,
        .dup         = 0,
        .packet_id   = 0,
        .topic_name  = topic_name,
        .topic_len   = strlen(topic_name),
        .payload_str = payload,
        .payload_len = payload_len,
    };

    HAL_MutexLock(client->lock_generic);
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        if (client->sub_handles[i].topic_filter &&
            _is_topic_matched(client->sub_handles[i].topic_filter, message.topic_name, message.topic_len)) {
            if (client->sub_handles[i].params.on_message_handler) {
                HAL_MutexUnlock(client->lock_generic);
                // if found, then handle it, then return
                client->sub_handles[i].params.on_message_handler(client, &message,
                                                                 client->sub_handles[i].params.user_data);
                return;
            }
        }
    }
    HAL_MutexUnlock(client->lock_generic);

    /* Message handler not found for topic */
    /* May be we do not care  change FAILURE  use SUCCESS*/
    Log_d("no matching any topic, call default handle function");

    MQTTEventMsg msg;
    if (client->event_handle.h_fp) {
        msg.event_type = MQTT_EVENT_PUBLISH_RECEIVED;
        msg.msg        = &message;
        client->event_handle.h_fp(client, client->event_handle.context, &msg);
    }
}
