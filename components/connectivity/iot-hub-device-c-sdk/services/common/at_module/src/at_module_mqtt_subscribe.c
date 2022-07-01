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
 * @file at_module_mqtt_subscribe.c
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
    int            rc, qos = params->qos;
    char          *topic_filter_stored;
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

    rc = module_mqtt_subscribe(client, topic_filter, qos);
    if (!rc) {
        sub_handle.status = SUB_ACK_RECEIVED;
    }
    IOT_FUNC_EXIT_RC(rc);
exit:
    _remove_sub_handle_from_array(client, topic_filter_stored);
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
    SubTopicHandle sub_handle;
    memset(&sub_handle, 0, sizeof(SubTopicHandle));

    // remove from sub handle
    if (!_remove_sub_handle_from_array(client, topic_filter)) {
        Log_w("subscription does not exists: %s", topic_filter);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_UNSUB_FAIL);
    }

    IOT_FUNC_EXIT_RC(module_mqtt_unsubscribe(client, topic_filter));
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
