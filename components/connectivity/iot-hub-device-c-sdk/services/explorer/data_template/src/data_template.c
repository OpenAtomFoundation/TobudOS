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
 * @file data_template.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-08-22
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-08-22 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_data_template.h"

#include "data_template.h"

/**
 * @brief Check and subscribe data template topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IotDataTemplateCallback
 * @param[in] usr_data usr data used in callback
 * @return 0 for success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_Init(void *client, IotDataTemplateCallback callback, void *usr_data)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    int rc = 0;

    DataTemplateContext data_tempale_context;
    data_tempale_context.usr_data = usr_data;

    data_tempale_context.property_callback = callback.property_callback;
    rc |= data_template_topic_check_and_sub(client, DATA_TEMPLATE_TYPE_PROPERTY, data_template_property_message_handler,
                                            data_tempale_context);

    if (callback.event_callback.method_event_reply_callback) {
        data_tempale_context.event_callback = callback.event_callback;
        rc |= data_template_topic_check_and_sub(client, DATA_TEMPLATE_TYPE_EVENT, data_template_event_message_handler,
                                                data_tempale_context);
    }

    if (callback.action_callback.method_action_callback) {
        data_tempale_context.action_callback = callback.action_callback;
        rc |= data_template_topic_check_and_sub(client, DATA_TEMPLATE_TYPE_ACTION, data_template_action_message_handler,
                                                data_tempale_context);
    }
    return rc;
}

/**
 * @brief Unsubscribe data template topic.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_DataTemplate_Deinit(void *client)
{
    POINTER_SANITY_CHECK_RTN(client);

    data_template_topic_unsubscribe(client, DATA_TEMPLATE_TYPE_PROPERTY);
    data_template_topic_unsubscribe(client, DATA_TEMPLATE_TYPE_EVENT);
    data_template_topic_unsubscribe(client, DATA_TEMPLATE_TYPE_ACTION);
}

/**
 * @brief Report property.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @param[in] params params constructed with property
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_PropertyReport(void *client, char *buf, int buf_len, const char *params)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    PropertyPublishParams publish_params = {.json = params};
    return data_template_property_publish(client, PROPERTY_UP_METHOD_TYPE_REPORT, buf, buf_len, publish_params);
}

/**
 * @brief Get control message offline.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_PropertyGetStatus(void *client, char *buf, int buf_len)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    PropertyPublishParams publish_params = {0};
    return data_template_property_publish(client, PROPERTY_UP_METHOD_TYPE_GET_STATUS, buf, buf_len, publish_params);
}

/**
 * @brief Report device info.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @param[in] params params constructed with device info
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_PropertyReportInfo(void *client, char *buf, int buf_len, const char *params)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    PropertyPublishParams publish_params = {.json = params};
    return data_template_property_publish(client, PROPERTY_UP_METHOD_TYPE_REPORT_INFO, buf, buf_len, publish_params);
}

/**
 * @brief Clear control message offline.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_PropertyClearControl(void *client, char *buf, int buf_len)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    PropertyPublishParams publish_params = {0};
    return data_template_property_publish(client, PROPERTY_UP_METHOD_TYPE_CLEAR_CONTROL, buf, buf_len, publish_params);
}

/**
 * @brief Reply control message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @param[in] code 0 for success
 * @param[in] client_token client token of control message
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_PropertyControlReply(void *client, char *buf, int buf_len, int code, UtilsJsonValue client_token)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    PropertyPublishParams publish_params = {.control_reply.code = code, .control_reply.client_token = client_token};
    return data_template_property_publish(client, PROPERTY_UP_METHOD_TYPE_CONTROL_REPLY, buf, buf_len, publish_params);
}

/**
 * @brief Post event.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @param[in] data @see IotDataTemplateEventData
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_EventPost(void *client, char *buf, int buf_len, IotDataTemplateEventData data)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    return data_template_event_reply_publish(client, buf, buf_len, data);
}

/**
 * @brief Reply action message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @param[in] reply @see IotDataTemplateActionReply
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_ActionReply(void *client, char *buf, int buf_len, IotDataTemplateActionReply reply)
{
    return data_template_action_reply_publish(client, buf, buf_len, reply);
}
