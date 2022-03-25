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
 * @file mqtt_client_subscribe.c
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
 * @brief Context of subscribe
 *
 */
typedef struct {
    uint16_t       packet_id;
    SubTopicHandle handler;
} QcloudIotSubConext;

/**
 * @brief Free topic_filter and user_data
 *
 * @param[in] handler subtopic handle
 */
static void _clear_sub_handle(SubTopicHandle *handler)
{
    if (handler->topic_filter) {
        HAL_Free(handler->topic_filter);
        handler->topic_filter = NULL;
    }

    if (handler->params.user_data && handler->params.user_data_free) {
        handler->params.user_data_free(handler->params.user_data);
        handler->params.user_data = NULL;
    }
}

/**
 * @brief Push node to subscribe(unsubscribe) ACK wait list.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] packet_len packet len of publish packet
 * @param[in] packet_id packet id
 * @param[in] type mqtt packet type SUBSCRIBE or UNSUBSCRIBE
 * @param[in] handler subtopic handle
 * @param[out] node node to push to list
 * @return @see IotReturnCode
 */
static int _push_sub_info_to_list(QcloudIotClient *client, int packet_len, uint16_t packet_id, MQTTPacketType type,
                                  const SubTopicHandle *handler, void **node)
{
    IOT_FUNC_ENTRY;
    void             *list     = client->list_sub_wait_ack;
    QcloudIotSubInfo *sub_info = NULL;

    sub_info = (QcloudIotSubInfo *)HAL_Malloc(sizeof(QcloudIotSubInfo) + packet_len);
    if (!sub_info) {
        Log_e("memory malloc failed!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    sub_info->buf       = (uint8_t *)sub_info + sizeof(QcloudIotSubInfo);
    sub_info->len       = packet_len;
    sub_info->type      = type;
    sub_info->packet_id = packet_id;
    sub_info->handler   = *handler;
    memcpy(sub_info->buf, client->write_buf, packet_len);
    HAL_Timer_CountdownMs(&sub_info->sub_start_time, client->command_timeout_ms);

    *node = utils_list_push(list, sub_info);
    if (!*node) {
        HAL_Free(sub_info);
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
static UtilsListResult _sub_wait_list_process_pop_info(void *list, void *node, void *val, void *usr_data)
{
    QcloudIotSubConext *sub_context = (QcloudIotSubConext *)usr_data;
    QcloudIotSubInfo   *sub_info    = (QcloudIotSubInfo *)val;

    if (sub_info->packet_id == sub_context->packet_id) {
        memcpy(&sub_context->handler, &sub_info->handler, sizeof(SubTopicHandle));
        utils_list_remove(list, node);
        return LIST_TRAVERSE_BREAK;
    }
    return LIST_TRAVERSE_CONTINUE;
}

/**
 * @brief Check sub wait list timeout.
 *
 * @param[in,out] list pointer to sub wait ack list.
 * @param[in] node pointer to list node
 * @param[in] val pointer to value, @see QcloudIotSubInfo
 * @param[in] usr_data @see QcloudIotClient
 * @return @see UtilsListResult
 */
static UtilsListResult _sub_wait_list_process_check_timeout(void *list, void *node, void *val, void *usr_data)
{
    MQTTEventMsg      msg;
    QcloudIotSubInfo *sub_info = (QcloudIotSubInfo *)val;
    QcloudIotClient  *client   = (QcloudIotClient *)usr_data;

    // check the request if timeout or not
    if (HAL_Timer_Remain(&sub_info->sub_start_time) > 0) {
        return LIST_TRAVERSE_CONTINUE;
    }

    // notify timeout event
    if (client->event_handle.h_fp) {
        msg.event_type = SUBSCRIBE == sub_info->type ? MQTT_EVENT_SUBSCRIBE_TIMEOUT : MQTT_EVENT_UNSUBSCRIBE_TIMEOUT;
        msg.msg        = (void *)(uintptr_t)sub_info->packet_id;
        if (sub_info->handler.params.on_sub_event_handler) {
            sub_info->handler.params.on_sub_event_handler(client, MQTT_EVENT_SUBSCRIBE_TIMEOUT,
                                                          sub_info->handler.params.user_data);
        }
        client->event_handle.h_fp(client, client->event_handle.context, &msg);
    }

    _clear_sub_handle(&sub_info->handler);
    utils_list_remove(list, node);
    return LIST_TRAVERSE_CONTINUE;
}

/**
 * @brief Clear sub wait list.
 *
 * @param[in,out] list pointer to sub wait list.
 * @param[in] node pointer to list node
 * @param[in] val pointer to value, @see QcloudIotSubInfo
 * @param[in] usr_data null
 * @return @see UtilsListResult
 */
static UtilsListResult _sub_wait_list_process_clear(void *list, void *node, void *val, void *usr_data)
{
    QcloudIotSubInfo *sub_info = (QcloudIotSubInfo *)val;
    if (sub_info->type == UNSUBSCRIBE) {
        _clear_sub_handle(&sub_info->handler);
    }
    utils_list_remove(list, node);
    return LIST_TRAVERSE_CONTINUE;
}

/**
 * @brief Pop node signed with packet id from subscribe ACK wait list, and return the sub handler
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] packet_id packet id
 * @param[out] sub_handle @see SubTopicHandle
 */
static void _pop_sub_info_from_list(QcloudIotClient *client, uint16_t packet_id, SubTopicHandle *sub_handle)
{
    QcloudIotSubConext sub_context = {.packet_id = packet_id};
    utils_list_process(client->list_sub_wait_ack, LIST_HEAD, _sub_wait_list_process_pop_info, &sub_context);
    memcpy(sub_handle, &sub_context.handler, sizeof(SubTopicHandle));
}

/**
 * @brief Remove sub handle when unsubscribe.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] topic_filter topic to remove
 * @return true topic exist
 * @return false topic no exist
 */
static bool _remove_sub_handle_from_array(QcloudIotClient *client, const char *topic_filter)
{
    int  i;
    bool topic_exists = false;

    // remove from message handler array
    HAL_MutexLock(client->lock_generic);
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        if ((client->sub_handles[i].topic_filter && !strcmp(client->sub_handles[i].topic_filter, topic_filter)) ||
            strstr(topic_filter, "/#") || strstr(topic_filter, "/+")) {
            // notify this event to topic subscriber
            if (client->sub_handles[i].params.on_sub_event_handler) {
                client->sub_handles[i].params.on_sub_event_handler(client, MQTT_EVENT_UNSUBSCRIBE,
                                                                   client->sub_handles[i].params.user_data);
            }
            _clear_sub_handle(&client->sub_handles[i]);
            // we don't want to break here, if the same topic is registered*with 2 callbacks.Unlikely scenario
            topic_exists = true;
        }
    }
    HAL_MutexUnlock(client->lock_generic);
    return topic_exists;
}

/**
 * @brief Add sub handle when subscribe.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] sub_handle sub_handle to be add to array
 * @return true topic exist
 * @return false topic no exist
 */
static int _add_sub_handle_to_array(QcloudIotClient *client, const SubTopicHandle *sub_handle)
{
    IOT_FUNC_ENTRY;
    int i, i_free = -1;

    HAL_MutexLock(client->lock_generic);

    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        if (client->sub_handles[i].topic_filter) {
            if (!strcmp(client->sub_handles[i].topic_filter, sub_handle->topic_filter)) {
                i_free = i;
                // free the memory before
                _clear_sub_handle(&client->sub_handles[i]);
                Log_w("Identical topic found: %s", sub_handle->topic_filter);
                break;
            }
        } else {
            i_free = i_free == -1 ? i : i_free;
        }
    }

    if (-1 == i_free) {
        Log_e("NO more @sub_handles space!");
        HAL_MutexUnlock(client->lock_generic);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    client->sub_handles[i_free] = *sub_handle;
    HAL_MutexUnlock(client->lock_generic);
    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief Set sub handle status.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] topic_filter topic to set status
 * @param[in] status @see SubStatus
 */
static void _set_sub_handle_status_to_array(QcloudIotClient *client, const char *topic_filter, SubStatus status)
{
    IOT_FUNC_ENTRY;
    int i;

    HAL_MutexLock(client->lock_generic);

    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        if (client->sub_handles[i].topic_filter) {
            if (!strcmp(client->sub_handles[i].topic_filter, topic_filter)) {
                client->sub_handles[i].status = status;
                break;
            }
        }
    }
    HAL_MutexUnlock(client->lock_generic);
    IOT_FUNC_EXIT;
}

/**
 * @brief Serialize and send subscribe packet.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic to subscribe
 * @param[in] params subscribe params
 * @return >=0 for packet id, < 0 for failed @see IotReturnCode
 */
int qcloud_iot_mqtt_subscribe(QcloudIotClient *client, const char *topic_filter, const SubscribeParams *params)
{
    IOT_FUNC_ENTRY;
    int            rc, packet_len, qos = params->qos;
    uint16_t       packet_id;
    char          *topic_filter_stored;
    void          *node = NULL;
    SubTopicHandle sub_handle;

    // topic filter should be valid in the whole sub life
    topic_filter_stored = HAL_Malloc(strlen(topic_filter) + 1);
    if (!topic_filter_stored) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MALLOC);
    }
    strncpy(topic_filter_stored, topic_filter, strlen(topic_filter) + 1);

    sub_handle.topic_filter = topic_filter_stored;
    sub_handle.params       = *params;
    sub_handle.status       = client->default_subscribe ? SUB_ACK_RECEIVED : SUB_ACK_NOT_RECEIVED;

    // add sub handle first to process
    rc = _add_sub_handle_to_array(client, &sub_handle);
    if (rc) {
        goto exit;
    }

    if (client->default_subscribe) {
        return 0;
    }

    packet_id = get_next_packet_id(client);
    Log_d("subscribe topic_name=%s|packet_id=%d", topic_filter_stored, packet_id);
    // serialize packet
    HAL_MutexLock(client->lock_write_buf);
    packet_len = mqtt_subscribe_packet_serialize(client->write_buf, client->write_buf_size, packet_id, 1,
                                                 &topic_filter_stored, &qos);
    if (packet_len < 0) {
        HAL_MutexUnlock(client->lock_write_buf);
        rc = packet_len == MQTT_ERR_SHORT_BUFFER ? QCLOUD_ERR_BUF_TOO_SHORT : QCLOUD_ERR_FAILURE;
        goto exit;
    }

    // add node into sub ack wait list
    rc = _push_sub_info_to_list(client, packet_len, packet_id, SUBSCRIBE, &sub_handle, &node);
    if (rc) {
        HAL_MutexUnlock(client->lock_write_buf);
        goto exit;
    }

    // send packet
    rc = send_mqtt_packet(client, packet_len);
    HAL_MutexUnlock(client->lock_write_buf);
    if (rc) {
        utils_list_remove(client->list_sub_wait_ack, node);
        goto exit;
    }
    IOT_FUNC_EXIT_RC(packet_id);
exit:
    _remove_sub_handle_from_array(client, topic_filter_stored);
    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief Deserialize suback packet and return sub result.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int qcloud_iot_mqtt_handle_suback(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;
    int            rc, count = 0;
    uint16_t       packet_id = 0;
    int            granted_qos;
    MQTTEventMsg   msg        = {MQTT_EVENT_SUBSCRIBE_SUCCESS, NULL};
    SubTopicHandle sub_handle = {0};
    MQTTEventType  event_type = MQTT_EVENT_SUBSCRIBE_SUCCESS;

    rc = mqtt_suback_packet_deserialize(client->read_buf, client->read_buf_size, 1, &count, &packet_id, &granted_qos);
    if (rc) {
        IOT_FUNC_EXIT_RC(rc);
    }
    msg.msg = (void *)(uintptr_t)packet_id;

    // pop sub info and get sub handle
    _pop_sub_info_from_list(client, packet_id, &sub_handle);
    if (!sub_handle.topic_filter) {
        Log_w("can't get sub handle from list!");
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);  // in case of resubscribe when reconnect
    }

    // check return code in SUBACK packet: 0x00(QOS0, SUCCESS),0x01(QOS1, SUCCESS),0x02(QOS2,SUCCESS),0x80(Failure)
    if (0x80 == granted_qos) {
        msg.event_type = MQTT_EVENT_SUBSCRIBE_NACK;
        event_type     = MQTT_EVENT_SUBSCRIBE_NACK;
        Log_e("MQTT SUBSCRIBE failed, packet_id: %u topic: %s", packet_id, sub_handle.topic_filter);
        _remove_sub_handle_from_array(client, sub_handle.topic_filter);
        rc = QCLOUD_ERR_MQTT_SUB;
    }

    if (MQTT_EVENT_SUBSCRIBE_SUCCESS == msg.event_type) {
        _set_sub_handle_status_to_array(client, sub_handle.topic_filter, SUB_ACK_RECEIVED);
    }

    // notify this event to user callback
    if (client->event_handle.h_fp) {
        client->event_handle.h_fp(client, client->event_handle.context, &msg);
    }

    // notify this event to topic subscriber
    if (sub_handle.params.on_sub_event_handler) {
        sub_handle.params.on_sub_event_handler(client, event_type, sub_handle.params.user_data);
    }
    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief Serialize and send unsubscribe packet.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic to unsubscribe
 * @return >=0 packet id, < 0 for failed @see IotReturnCode
 */
int qcloud_iot_mqtt_unsubscribe(QcloudIotClient *client, const char *topic_filter)
{
    IOT_FUNC_ENTRY;
    int      rc, packet_len;
    uint16_t packet_id;

    void          *node = NULL;
    SubTopicHandle sub_handle;
    memset(&sub_handle, 0, sizeof(SubTopicHandle));

    // remove from sub handle
    if (!_remove_sub_handle_from_array(client, topic_filter)) {
        Log_w("subscription does not exists: %s", topic_filter);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_UNSUB_FAIL);
    }

    // topic filter should be valid in the whole sub life
    char *topic_filter_stored = HAL_Malloc(strlen(topic_filter) + 1);
    if (!topic_filter_stored) {
        Log_e("malloc failed");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }
    strncpy(topic_filter_stored, topic_filter, strlen(topic_filter) + 1);
    sub_handle.topic_filter = topic_filter_stored;

    packet_id = get_next_packet_id(client);
    Log_d("unsubscribe topic_name=%s|packet_id=%d", topic_filter_stored, packet_id);

    HAL_MutexLock(client->lock_write_buf);
    packet_len = mqtt_unsubscribe_packet_serialize(client->write_buf, client->write_buf_size, packet_id, 1,
                                                   &topic_filter_stored);
    if (packet_len < 0) {
        HAL_MutexUnlock(client->lock_write_buf);
        rc = packet_len == MQTT_ERR_SHORT_BUFFER ? QCLOUD_ERR_BUF_TOO_SHORT : QCLOUD_ERR_FAILURE;
        goto exit;
    }

    // add node into sub ack wait list
    rc = _push_sub_info_to_list(client, packet_len, packet_id, UNSUBSCRIBE, &sub_handle, &node);
    if (rc) {
        Log_e("push unsubscribe info failed!");
        HAL_MutexUnlock(client->lock_write_buf);
        goto exit;
    }

    /* send the unsubscribe packet */
    rc = send_mqtt_packet(client, packet_len);
    HAL_MutexUnlock(client->lock_write_buf);
    if (rc) {
        utils_list_remove(client->list_sub_wait_ack, node);
        goto exit;
    }

    IOT_FUNC_EXIT_RC(packet_id);
exit:
    _clear_sub_handle(&sub_handle);
    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief Deserialize unsuback packet and remove from list.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int qcloud_iot_mqtt_handle_unsuback(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;
    int            rc;
    uint16_t       packet_id  = 0;
    SubTopicHandle sub_handle = {0};
    MQTTEventMsg   msg;

    rc = mqtt_unsuback_packet_deserialize(client->read_buf, client->read_buf_size, &packet_id);
    if (rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    _pop_sub_info_from_list(client, packet_id, &sub_handle);
    _clear_sub_handle(&sub_handle);

    if (client->event_handle.h_fp) {
        msg.event_type = MQTT_EVENT_UNSUBSCRIBE_SUCCESS;
        msg.msg        = (void *)(uintptr_t)packet_id;
        client->event_handle.h_fp(client, client->event_handle.context, &msg);
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief Process suback waiting timeout.
 *
 * @param[in,out] client pointer to mqtt client
 */
void qcloud_iot_mqtt_check_sub_timeout(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;
    utils_list_process(client->list_sub_wait_ack, LIST_HEAD, _sub_wait_list_process_check_timeout, client);
    IOT_FUNC_EXIT;
}

/**
 * @brief Resubscribe topic when reconnect.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int qcloud_iot_mqtt_resubscribe(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;
    int             rc, packet_len, qos, itr = 0;
    uint16_t        packet_id;
    SubTopicHandle *sub_handle;

    for (itr = 0; itr < MAX_MESSAGE_HANDLERS; itr++) {
        sub_handle = &client->sub_handles[itr];
        if (!sub_handle->topic_filter) {
            continue;
        }

        packet_id = get_next_packet_id(client);
        Log_d("subscribe topic_name=%s|packet_id=%d", sub_handle->topic_filter, packet_id);

        HAL_MutexLock(client->lock_write_buf);
        qos        = sub_handle->params.qos;
        packet_len = mqtt_subscribe_packet_serialize(client->write_buf, client->write_buf_size, packet_id, 1,
                                                     &sub_handle->topic_filter, &qos);
        if (packet_len < 0) {
            HAL_MutexUnlock(client->lock_write_buf);
            continue;
        }

        // send packet
        rc = send_mqtt_packet(client, packet_len);
        HAL_MutexUnlock(client->lock_write_buf);
        if (rc) {
            IOT_FUNC_EXIT_RC(rc);
        }
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief Return if topic is sub ready.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter
 * @return true for ready
 * @return false for not ready
 */
bool qcloud_iot_mqtt_is_sub_ready(QcloudIotClient *client, const char *topic_filter)
{
    IOT_FUNC_ENTRY;
    int i = 0;
    HAL_MutexLock(client->lock_generic);
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        if ((client->sub_handles[i].topic_filter && !strcmp(client->sub_handles[i].topic_filter, topic_filter)) ||
            strstr(topic_filter, "/#") || strstr(topic_filter, "/+")) {
            HAL_MutexUnlock(client->lock_generic);
            return client->sub_handles[i].status == SUB_ACK_RECEIVED;
        }
    }
    HAL_MutexUnlock(client->lock_generic);
    IOT_FUNC_EXIT_RC(false);
}

/**
 * @brief Get usr data, usr should handle lock/unlock usrdata itself in callback and caller.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter
 * @return NULL or user data
 */
void *qcloud_iot_mqtt_get_subscribe_usr_data(QcloudIotClient *client, const char *topic_filter)
{
    IOT_FUNC_ENTRY;
    int i = 0;
    HAL_MutexLock(client->lock_generic);
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        if ((client->sub_handles[i].topic_filter && !strcmp(client->sub_handles[i].topic_filter, topic_filter)) ||
            strstr(topic_filter, "/#") || strstr(topic_filter, "/+")) {
            HAL_MutexUnlock(client->lock_generic);
            return client->sub_handles[i].params.user_data;
        }
    }
    HAL_MutexUnlock(client->lock_generic);
    IOT_FUNC_EXIT_RC(NULL);
}

/**
 * @brief Clear sub handle array.
 *
 * @param[in,out] client pointer to mqtt client
 */
void qcloud_iot_mqtt_sub_handle_array_clear(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;
    int i;
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        /* notify this event to topic subscriber */
        if (client->sub_handles[i].topic_filter && client->sub_handles[i].params.on_sub_event_handler) {
            client->sub_handles[i].params.on_sub_event_handler(client, MQTT_EVENT_CLIENT_DESTROY,
                                                               client->sub_handles[i].params.user_data);
        }
        _clear_sub_handle(&client->sub_handles[i]);
    }

    IOT_FUNC_EXIT;
}

/**
 * @brief Clear suback wait list and clear sub handle.
 *
 * @param[in,out] client pointer to mqtt client
 */
void qcloud_iot_mqtt_suback_wait_list_clear(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;
    utils_list_process(client->list_sub_wait_ack, LIST_HEAD, _sub_wait_list_process_clear, client);
    IOT_FUNC_EXIT;
}