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
 * @file qcloud_iot_data_template.h
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

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_EXPLORER_QCLOUD_IOT_DATA_TEMPLATE_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_EXPLORER_QCLOUD_IOT_DATA_TEMPLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "qcloud_iot_common.h"

/**
 * @brief Callback of property.
 *
 */
typedef struct {
    void (*method_control_callback)(UtilsJsonValue client_token, UtilsJsonValue params, void *usr_data);
    void (*method_report_reply_callback)(UtilsJsonValue client_token, int code, void *usr_data);
    void (*method_get_status_reply_callback)(UtilsJsonValue client_token, int code, UtilsJsonValue reported,
                                             UtilsJsonValue control, void *usr_data);
    void (*method_report_info_reply_callback)(UtilsJsonValue client_token, int code, void *usr_data);
    void (*method_clear_control_reply_callback)(UtilsJsonValue client_token, int code, void *usr_data);
} PropertyMessageCallback;

/**
 * @brief Callback of event.
 *
 */
typedef struct {
    void (*method_event_reply_callback)(UtilsJsonValue client_token, int code, void *usr_data);
} EventMessageCallback;

/**
 * @brief Callback of action.
 *
 */
typedef struct {
    void (*method_action_callback)(UtilsJsonValue client_token, UtilsJsonValue action_id, UtilsJsonValue params,
                                   void *usr_data);
} ActionMessageCallback;

/**
 * @brief Callback of data template(including property/event/action).
 *
 */
typedef struct {
    PropertyMessageCallback property_callback;
    EventMessageCallback    event_callback;
    ActionMessageCallback   action_callback;
} IotDataTemplateCallback;

#define DEFAULT_DATA_TEMPLATE_CALLBACK                  \
    {                                                   \
        {NULL, NULL, NULL, NULL, NULL}, {NULL}, {NULL}, \
    }

/**
 * @brief Type of event.
 *
 */
typedef enum {
    IOT_DATA_TEMPLATE_EVENT_TYPE_INFO = 0,
    IOT_DATA_TEMPLATE_EVENT_TYPE_ALERT,
    IOT_DATA_TEMPLATE_EVENT_TYPE_FAULT,
} IotDataTemplateEventType;

/**
 * @brief Post data.
 *
 */
typedef struct {
    const char *             event_id; /**< event id defined in data template */
    IotDataTemplateEventType type;     /**< event type defined in data template */
    const char *             params;   /**< property json defined in data template */
} IotDataTemplateEventData;

/**
 * @brief Action reply.
 *
 */
typedef struct {
    UtilsJsonValue client_token;
    int            code;
    const char *   response; /**< property json defined in data template */
} IotDataTemplateActionReply;

/**
 * @brief Check and subscribe data template topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IotDataTemplateCallback
 * @param[in] usr_data usr data used in callback
 * @return 0 for success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_Init(void *client, IotDataTemplateCallback callback, void *usr_data);

/**
 * @brief Unsubscribe data template topic.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_DataTemplate_Deinit(void *client);

/**
 * @brief Report property.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @param[in] params params constructed with property
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_PropertyReport(void *client, char *buf, int buf_len, const char *params);

/**
 * @brief Get control message offline.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_PropertyGetStatus(void *client, char *buf, int buf_len);

/**
 * @brief Report device info.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @param[in] params params constructed with device info
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_PropertyReportInfo(void *client, char *buf, int buf_len, const char *params);

/**
 * @brief Clear control message offline.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_PropertyClearControl(void *client, char *buf, int buf_len);

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
int IOT_DataTemplate_PropertyControlReply(void *client, char *buf, int buf_len, int code, UtilsJsonValue client_token);

/**
 * @brief Post event.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @param[in] data @see IotDataTemplateEventData
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_EventPost(void *client, char *buf, int buf_len, IotDataTemplateEventData data);

/**
 * @brief Reply action message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer for message
 * @param[in] buf_len buffer length
 * @param[in] reply @see IotDataTemplateActionReply
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_DataTemplate_ActionReply(void *client, char *buf, int buf_len, IotDataTemplateActionReply reply);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_EXPLORER_QCLOUD_IOT_DATA_TEMPLATE_H_
