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
 * @file data_template_config.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-09
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-09 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_APP_DATA_TEMPLATE_DATA_TEMPLATE_CONFIG_H_
#define IOT_HUB_DEVICE_C_SDK_APP_DATA_TEMPLATE_DATA_TEMPLATE_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_common.h"
#include "qcloud_iot_data_template.h"

/**
 * @brief Type of property.
 *
 */
typedef enum {
    // basic type
    DATA_TEMPLATE_TYPE_INT,
    DATA_TEMPLATE_TYPE_ENUM,
    DATA_TEMPLATE_TYPE_STRING_ENUM,
    DATA_TEMPLATE_TYPE_FLOAT,
    DATA_TEMPLATE_TYPE_BOOL,
    DATA_TEMPLATE_TYPE_STRING,
    DATA_TEMPLATE_TYPE_TIME,
    // construct by basic type
    DATA_TEMPLATE_TYPE_STRUCT,
    DATA_TEMPLATE_TYPE_ARRAY,
} DataTemplatePropertyType;

/**
 * @brief Declaration.
 *
 */
typedef struct DataTemplateProperty     DataTemplateProperty;
typedef union DataTemplatePropertyValue DataTemplatePropertyValue;

/**
 * @brief Property value definition.
 *
 */
union DataTemplatePropertyValue {
    int32_t  value_int;
    int32_t  value_enum;
    char*    value_string_enum;
    float    value_float;
    int32_t  value_bool;
    char*    value_string;
    uint32_t value_time;
    struct {
        DataTemplateProperty* property;
        int                   count;
    } value_struct;
    DataTemplatePropertyValue* value_arrary; /**< not supportted yet */
};

/**
 * @brief Property definition.
 *
 */
struct DataTemplateProperty {
    DataTemplatePropertyType  type;
    const char*               key;
    DataTemplatePropertyValue value;
    int                       need_report;
};

/**
 * @brief Event definition.
 *
 */
#define DataTemplateEvent IotDataTemplateEventData

/**
 * @brief Action definition.
 *
 */
typedef struct {
    const char*                action_id;
    DataTemplatePropertyValue  input_struct;
    IotDataTemplateActionReply reply;
} DataTemplateAction;

/**************************************************************************************
 * usr data template definition
 **************************************************************************************/

typedef enum {
    USR_PROPERTY_INDEX_POWER_SWITCH = 0,
    USR_PROPERTY_INDEX_COLOR,
    USR_PROPERTY_INDEX_BRIGHTNESS,
    USR_PROPERTY_INDEX_NAME,
    USR_PROPERTY_INDEX_POSITION,
    USR_PROPERTY_INDEX_POWER,
} UsrPropertyIndex;

typedef enum {
    USR_PROPERTY_POSITION_INDEX_LONGITUDE = 0,
    USR_PROPERTY_POSITION_INDEX_LATITUDE,
} UsrPropertyPositionIndex;

typedef enum {
    USR_EVENT_INDEX_STATUS_REPORT = 0,
    USR_EVENT_INDEX_LOW_VOLTAGE,
    USR_EVENT_INDEX_HARDWARE_FAULT,
} UsrEventIndex;

typedef enum {
    USR_ACTION_INDEX_LIGHT_BLINK = 0,
} UsrActionIndex;

typedef enum {
    USR_ACTION_LIGHT_BLINK_INPUT_INDEX_TIME = 0,
    USR_ACTION_LIGHT_BLINK_INPUT_INDEX_COLOR,
    USR_ACTION_LIGHT_BLINK_INPUT_INDEX_TOTAL_TIME,
} UsrActionLightBlinkInputIndex;

/**************************************************************************************
 * api for user data template
 **************************************************************************************/

/**
 * @brief Init user data template(property/event/action).
 *
 */
void usr_data_template_init(void);

/**
 * @brief Get property value.
 *
 * @param[in] index @see UsrPropertyIndex
 * @return @see DataTemplatePropertyValue
 */
DataTemplatePropertyValue usr_data_template_property_value_get(UsrPropertyIndex index);

/**
 * @brief Set property value.
 *
 * @param[in] index @see UsrPropertyIndex
 * @param[in] value @see DataTemplatePropertyValue, @note value should match property type.
 */
void usr_data_template_property_value_set(UsrPropertyIndex index, DataTemplatePropertyValue value);

/**
 * @brief Get property(struct) value.
 *
 * @param[in] struct_index @see UsrPropertyIndex, @note DATA_TEMPLATE_TYPE_STRUCT is required here.
 * @param[in] property_index depends on which struct
 * @return @see DataTemplatePropertyValue
 */
DataTemplatePropertyValue usr_data_template_property_struct_value_get(UsrPropertyIndex struct_index,
                                                                      int              property_index);

/**
 * @brief Set property(struct) value.
 *
 * @param[in] struct_index @see UsrPropertyIndex, @note DATA_TEMPLATE_TYPE_STRUCT is required here.
 * @param[in] property_index depends on which struct
 * @param[in] value  @see DataTemplatePropertyValue, @note value should match property type.
 */
void usr_data_template_property_struct_value_set(UsrPropertyIndex struct_index, int property_index,
                                                 DataTemplatePropertyValue value);

/**
 * @brief Parse control message and set property value.
 *
 * @param[in] params params filed of control message
 */
void usr_data_template_property_parse(UtilsJsonValue params);

/**
 * @brief Get property status.
 *
 * @param[in] index @see UsrPropertyIndex
 * @return need_report
 */
int usr_data_template_property_status_get(UsrPropertyIndex index);

/**
 * @brief Report all the properties needed report.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer to report
 * @param[in] buf_len buffer length
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int usr_data_template_property_report(void* client, char* buf, int buf_len);

/**
 * @brief Post event.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer to report
 * @param[in] buf_len buffer length
 * @param[in] id @see UsrEventIndex
 * @param[in] params user should construct params according to event.
 */
void usr_data_template_event_post(void* client, char* buf, int buf_len, UsrEventIndex id, const char* params);

/**
 * @brief Parse action message and set action input params.
 *
 * @param[in] action_id action id
 * @param[in] params params of action
 * @param[out] index @see UsrActionIndex
 * @return 0 for success, QCLOUD_ERR_JSON_PARSE for invalid json.
 */
int usr_data_template_action_parse(UtilsJsonValue action_id, UtilsJsonValue params, UsrActionIndex* index);

/**
 * @brief Get input value, should call after usr_data_template_action_parse
 *
 * @param[in] index index get from usr_data_template_action_parse
 * @param[in] property_index property index of action input params
 * @return @see DataTemplatePropertyValue
 */
DataTemplatePropertyValue usr_data_template_action_input_value_get(UsrActionIndex index, int property_index);

/**
 * @brief Reply action, should call after parse action message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer to report
 * @param[in] buf_len buffer length
 * @param[in] index @see UsrActionIndex
 * @param[in] client_token client token of action message
 * @param[in] code result code for action, 0 for success
 * @param[in] response action output params, user should construct params according to action
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int usr_data_template_action_reply(void* client, char* buf, int buf_len, UsrActionIndex index,
                                   UtilsJsonValue client_token, int code, const char* response);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_APP_DATA_TEMPLATE_DATA_TEMPLATE_CONFIG_H_
