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
 * @file mqtt_client_publish.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-08 <td>1.1     <td>fancyxu   <td>fix code standard of IotReturnCode and QcloudIotClient
 * </table>
 */

#include "mqtt_client.h"

/**
 * @brief Push pub info to list for republish.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] packet_len packet len of publish packet
 * @param[in] packet_id packet id
 * @param[out] node list node
 * @return @see IotReturnCode
 */
static int _push_pub_info_to_list(QcloudIotClient *client, int packet_len, uint16_t packet_id, void **node)
{
    IOT_FUNC_ENTRY;
    void *            list       = client->list_pub_wait_ack;
    QcloudIotPubInfo *repub_info = NULL;

    // construct republish info
    repub_info = (QcloudIotPubInfo *)HAL_Malloc(sizeof(QcloudIotPubInfo) + packet_len);
    if (!repub_info) {
        Log_e("memory malloc failed!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    repub_info->buf       = (uint8_t *)repub_info + sizeof(QcloudIotPubInfo);
    repub_info->len       = packet_len;
    repub_info->packet_id = packet_id;
    memcpy(repub_info->buf, client->write_buf, packet_len);  // save the whole packet
    HAL_Timer_CountdownMs(&repub_info->pub_start_time, client->command_timeout_ms);

    // push republish info to list
    *node = utils_list_push(list, repub_info);
    if (!*node) {
        HAL_Free(repub_info);
        Log_e("list push failed! Check the list len!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }
    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief Check pub wait list timeout.
 *
 * @param[in,out] list pointer to pub wait ack list.
 * @param[in] node pointer to list node
 * @param[in] val pointer to value, @see QcloudIotPubInfo
 * @param[in] usr_data @see QcloudIotClient
 * @return @see UtilsListResult
 */
static UtilsListResult _pub_wait_list_process_check_timeout(void *list, void *node, void *val, void *usr_data)
{
    IOT_FUNC_ENTRY;

    MQTTEventMsg      msg;
    QcloudIotPubInfo *repub_info = (QcloudIotPubInfo *)val;
    QcloudIotClient * client     = (QcloudIotClient *)usr_data;

    // check the request if timeout or not
    if (HAL_Timer_Remain(&repub_info->pub_start_time) > 0) {
        IOT_FUNC_EXIT_RC(LIST_TRAVERSE_CONTINUE);
    }

    // notify timeout event
    if (client->event_handle.h_fp) {
        msg.event_type = MQTT_EVENT_PUBLISH_TIMEOUT;
        msg.msg        = (void *)(uintptr_t)repub_info->packet_id;
        client->event_handle.h_fp(client, client->event_handle.context, &msg);
    }
    utils_list_remove(list, node);
    IOT_FUNC_EXIT_RC(LIST_TRAVERSE_CONTINUE);
}

/**
 * @brief Remove info from pub wait list.
 *
 * @param[in,out] list pointer to pub wait ack list.
 * @param[in] node pointer to list node
 * @param[in] val pointer to value, @see QcloudIotPubInfo
 * @param[in] usr_data pointer to packet id
 * @return @see UtilsListResult
 */
static UtilsListResult _pub_wait_list_process_remove_info(void *list, void *node, void *val, void *usr_data)
{
    IOT_FUNC_ENTRY;

    QcloudIotPubInfo *repub_info = (QcloudIotPubInfo *)val;
    if (repub_info->packet_id == *((uint16_t *)usr_data)) {
        utils_list_remove(list, node);
        IOT_FUNC_EXIT_RC(LIST_TRAVERSE_BREAK);
    }
    IOT_FUNC_EXIT_RC(LIST_TRAVERSE_CONTINUE);
}

/**
 * @brief Remove node signed with packet id from publish ACK wait list.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] packet_id packet id
 */
static void _remove_pub_info_from_list(QcloudIotClient *client, uint16_t packet_id)
{
    utils_list_process(client->list_pub_wait_ack, LIST_HEAD, _pub_wait_list_process_remove_info, &packet_id);
}

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
 * @brief deliver the message to user callback
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message to deliver, @see MQTTMessage
 */
static void _deliver_message(QcloudIotClient *client, MQTTMessage *message)
{
    int i;

    MQTTEventMsg msg;

    HAL_MutexLock(client->lock_generic);
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        if (client->sub_handles[i].topic_filter &&
            _is_topic_matched((char *)client->sub_handles[i].topic_filter, message->topic_name, message->topic_len)) {
            if (client->sub_handles[i].params.on_message_handler) {
                HAL_MutexUnlock(client->lock_generic);
                // if found, then handle it, then return
                client->sub_handles[i].params.on_message_handler(client, message,
                                                                 client->sub_handles[i].params.user_data);
                return;
            }
        }
    }
    HAL_MutexUnlock(client->lock_generic);

    /* Message handler not found for topic */
    /* May be we do not care  change FAILURE  use SUCCESS*/
    Log_d("no matching any topic, call default handle function");

    if (client->event_handle.h_fp) {
        msg.event_type = MQTT_EVENT_PUBLISH_RECEIVED;
        msg.msg        = message;
        client->event_handle.h_fp(client, client->event_handle.context, &msg);
    }
}

#ifdef MQTT_RMDUP_MSG_ENABLED

/**
 * @brief Check if packet id repeat.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] packet_id packet_id
 * @return < 0 for failed.
 */
static int _get_packet_id_repeat_buf(QcloudIotClient *client, uint16_t packet_id)
{
    int i;
    for (i = 0; i < MQTT_MAX_REPEAT_BUF_LEN; ++i) {
        if (packet_id == client->repeat_packet_id_buf[i]) {
            return packet_id;
        }
    }
    return -1;
}

/**
 * @brief Add packet id, if buf full, then overwrite.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] packet_id packet_id
 */
static void _add_packet_id_to_repeat_buf(QcloudIotClient *client, uint16_t packet_id)
{
    if (_get_packet_id_repeat_buf(client, packet_id) >= 0) {
        return;
    }
    client->repeat_packet_id_buf[client->current_packet_id_cnt++] = packet_id;
    client->current_packet_id_cnt %= MQTT_MAX_REPEAT_BUF_LEN;
}

#endif

/**
 * @brief Serialize and send publish packet.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] topic_name topic to publish
 * @param[in] params publish params
 * @return >=0 for packet id, < 0 for failed @see IotReturnCode
 */
int qcloud_iot_mqtt_publish(QcloudIotClient *client, const char *topic_name, const PublishParams *params)
{
    IOT_FUNC_ENTRY;
    int              rc, packet_len;
    MQTTPublishFlags flags;
    void *           node      = NULL;
    uint16_t         packet_id = 0;

    if (params->qos > QOS0) {
        packet_id = get_next_packet_id(client);
    }

    Log_d("publish qos=%d|packet_id=%d|topic_name=%s|payload=%s", params->qos, packet_id, topic_name,
          STRING_PTR_PRINT_SANITY_CHECK((char *)params->payload));

    flags.dup    = params->dup;
    flags.qos    = params->qos;
    flags.retain = params->retain;

    // serialize packet
    HAL_MutexLock(client->lock_write_buf);
    packet_len = mqtt_publish_packet_serialize(client->write_buf, client->write_buf_size, &flags, packet_id, topic_name,
                                               params->payload, params->payload_len);
    if (packet_len < 0) {
        HAL_MutexUnlock(client->lock_write_buf);
        rc = packet_len == MQTT_ERR_SHORT_BUFFER ? QCLOUD_ERR_BUF_TOO_SHORT : QCLOUD_ERR_FAILURE;
        IOT_FUNC_EXIT_RC(rc);
    }

    if (params->qos > QOS0) {
        rc = _push_pub_info_to_list(client, packet_len, packet_id, &node);
        if (rc) {
            Log_e("push publish info failed!");
            HAL_MutexUnlock(client->lock_write_buf);
            IOT_FUNC_EXIT_RC(rc);
        }
    }

    // send the publish packet
    rc = send_mqtt_packet(client, packet_len);
    HAL_MutexUnlock(client->lock_write_buf);
    if (rc) {
        if (params->qos > QOS0) {
            utils_list_remove(client->list_pub_wait_ack, node);
        }
        IOT_FUNC_EXIT_RC(rc);
    }
    IOT_FUNC_EXIT_RC(packet_id);
}

/**
 * @brief Deserialize publish packet and deliver_message.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] params publish params
 * @return >=0 for packet id, < 0 for failed @see IotReturnCode
 */
int qcloud_iot_mqtt_handle_publish(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;
    int              rc, packet_len = 0;
    MQTTPublishFlags flags;
    MQTTMessage      msg;

    rc = mqtt_publish_packet_deserialize(client->read_buf, client->read_buf_size, &flags, &msg.packet_id,
                                         &msg.topic_name, &msg.topic_len, &msg.payload, &msg.payload_len);
    if (rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    msg.qos = flags.qos;

    if (QOS0 == msg.qos) {
        // No further processing required for QOS0
        _deliver_message(client, &msg);
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
    }

    // only Qos 1 is support
#ifdef MQTT_RMDUP_MSG_ENABLED
    // check if packet_id has been received before
    if (_get_packet_id_repeat_buf(client, msg.packet_id) < 0)
#endif
    {
        // deliver to msg callback
        _deliver_message(client, &msg);
    }
#ifdef MQTT_RMDUP_MSG_ENABLED
    // just add packet id
    _add_packet_id_to_repeat_buf(client, msg.packet_id);
#endif

    // reply with puback
    HAL_MutexLock(client->lock_write_buf);
    packet_len = mqtt_puback_packet_serialize(client->write_buf, client->write_buf_size, msg.packet_id);
    if (packet_len > 0) {
        rc = send_mqtt_packet(client, packet_len);
    } else {
        rc = packet_len == MQTT_ERR_SHORT_BUFFER ? QCLOUD_ERR_BUF_TOO_SHORT : QCLOUD_ERR_FAILURE;
    }
    HAL_MutexUnlock(client->lock_write_buf);
    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief Deserialize puback packet.
 *
 * @param[in,out] client pointer to mqtt_client
 * @return 0 for success.
 */
int qcloud_iot_mqtt_handle_puback(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;

    int          rc;
    uint16_t     packet_id;
    MQTTEventMsg msg;

    rc = mqtt_puback_packet_deserialize(client->read_buf, client->read_buf_size, &packet_id);
    if (rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    _remove_pub_info_from_list(client, packet_id);

    /* notify this event to user callback */
    if (client->event_handle.h_fp) {
        msg.event_type = MQTT_EVENT_PUBLISH_SUCCESS;
        msg.msg        = (void *)(uintptr_t)packet_id;
        client->event_handle.h_fp(client, client->event_handle.context, &msg);
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief Process puback waiting timout.
 *
 * @param[in,out] client pointer to mqtt_client
 */
void qcloud_iot_mqtt_check_pub_timeout(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;
    utils_list_process(client->list_pub_wait_ack, LIST_HEAD, _pub_wait_list_process_check_timeout, client);
    IOT_FUNC_EXIT;
}
