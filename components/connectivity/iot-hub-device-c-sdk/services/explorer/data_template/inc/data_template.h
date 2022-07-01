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
 * @file data_template.h
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

#ifndef IOT_HUB_DEVICE_C_SDK_SERVICES_EXPLORER_DATA_TEMPLATE_INC_DATA_TEMPLATE_H_
#define IOT_HUB_DEVICE_C_SDK_SERVICES_EXPLORER_DATA_TEMPLATE_INC_DATA_TEMPLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_common.h"
#include "qcloud_iot_data_template.h"

/**************************************************************************************
 * common
 **************************************************************************************/

/**
 * @brief Type of data template(property/event/action).
 *
 */
typedef enum {
    DATA_TEMPLATE_TYPE_PROPERTY = 0,
    DATA_TEMPLATE_TYPE_EVENT,
    DATA_TEMPLATE_TYPE_ACTION,
} DataTemplateType;

/**
 * @brief Context of data template, callback and user data.
 *
 */
typedef struct {
    union {
        PropertyMessageCallback property_callback;
        EventMessageCallback    event_callback;
        ActionMessageCallback   action_callback;
    };
    void *usr_data;
} DataTemplateContext;

/**
 * @brief Check if topic already subscribed, if not then subscribe.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] type @see DataTemplateType
 * @param[in] on_message_handler message handle of topic
 * @param[in] context @see DataTemplateContext
 * @return @see IotReturnCode
 */
int data_template_topic_check_and_sub(void *client, DataTemplateType type, OnMessageHandler on_message_handler,
                                      DataTemplateContext context);

/**
 * @brief Unsubscribe data template topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] type @see DataTemplateType
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int data_template_topic_unsubscribe(void *client, DataTemplateType type);

/**
 * @brief Publish to data template topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] type @see DataTemplateType
 * @param[in] qos @see QoS
 * @param[in] payload payload of mqtt packet
 * @param[in] payload_len payload len
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int data_template_publish(void *client, DataTemplateType type, QoS qos, const char *payload, int payload_len);

/**************************************************************************************
 * property
 **************************************************************************************/

typedef enum {
    PROPERTY_UP_METHOD_TYPE_REPORT = 0,
    PROPERTY_UP_METHOD_TYPE_REPORT_INFO,
    PROPERTY_UP_METHOD_TYPE_GET_STATUS,
    PROPERTY_UP_METHOD_TYPE_CLEAR_CONTROL,
    PROPERTY_UP_METHOD_TYPE_CONTROL_REPLY,
} PropertyUpMethodType;

typedef union {
    const char *json;
    int         code;
    struct {
        int            code;
        UtilsJsonValue client_token;
    } control_reply;
} PropertyPublishParams;

/**
 * @brief Mqtt message callback for property topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see DataTemplateContext
 */
void data_template_property_message_handler(void *client, const MQTTMessage *message, void *usr_data);

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
                                   PropertyPublishParams params);

/**************************************************************************************
 * event
 **************************************************************************************/

/**
 * @brief Mqtt message callback for event topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see DataTemplateContext
 */
void data_template_event_message_handler(void *client, const MQTTMessage *message, void *usr_data);

/**
 * @brief Publish message to event topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] data @see IotDataTemplateEventData
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int data_template_event_reply_publish(void *client, char *buf, int buf_len, IotDataTemplateEventData data);

/**************************************************************************************
 * action
 **************************************************************************************/

/**
 * @brief Mqtt message callback for action topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see DataTemplateContext
 */
void data_template_action_message_handler(void *client, const MQTTMessage *message, void *usr_data);

/**
 * @brief Publish message to event topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] reply @see IotDataTemplateActionReply
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int data_template_action_reply_publish(void *client, char *buf, int buf_len, IotDataTemplateActionReply reply);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_SERVICES_EXPLORER_DATA_TEMPLATE_INC_DATA_TEMPLATE_H_
