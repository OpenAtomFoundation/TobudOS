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
 * @file data_template_config.c
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

#include "data_template_config.h"

/**
 * @brief Set property value.
 *
 * @param[in,out] property pointer to property
 * @param[in] value value to set, should match with property type
 * @return 0 for success.
 */
static int _set_property_value(DataTemplateProperty* property, UtilsJsonValue value)
{
    int i, rc = 0;

    switch (property->type) {
        case DATA_TEMPLATE_TYPE_INT:
        case DATA_TEMPLATE_TYPE_ENUM:
        case DATA_TEMPLATE_TYPE_BOOL:
            return utils_json_value_data_get(value, UTILS_JSON_VALUE_TYPE_INT32, &property->value.value_int);
        case DATA_TEMPLATE_TYPE_TIME:
            return utils_json_value_data_get(value, UTILS_JSON_VALUE_TYPE_UINT32, &property->value.value_time);
        case DATA_TEMPLATE_TYPE_STRING:
        case DATA_TEMPLATE_TYPE_STRING_ENUM:
            if (!property->value.value_string) {  // no need copy
                return 0;
            }
            strncpy(property->value.value_string, value.value, value.value_len);
            property->value.value_string[value.value_len] = '\0';
            return 0;
        case DATA_TEMPLATE_TYPE_FLOAT:
            return utils_json_value_data_get(value, UTILS_JSON_VALUE_TYPE_FLOAT, &property->value.value_float);
        case DATA_TEMPLATE_TYPE_STRUCT:
            for (i = 0; i < property->value.value_struct.count; i++) {
                rc |= _set_property_value(property->value.value_struct.property + i, value);
            }
            return rc;
        case DATA_TEMPLATE_TYPE_ARRAY:
            Log_e("array type is not supportted yet!");
            return -1;
        default:
            Log_e("unkown type!");
            return -1;
    }
}

/**
 * @brief Get property node in json.
 *
 * @param[out] json_buf buffer to put node
 * @param[in] buf_len buffer length
 * @param[in] property property to get node
 * @return 0 for success.
 */
static int _get_property_node(char* json_buf, int buf_len, const DataTemplateProperty* property)
{
    int len, i;
    switch (property->type) {
        case DATA_TEMPLATE_TYPE_INT:
        case DATA_TEMPLATE_TYPE_ENUM:
        case DATA_TEMPLATE_TYPE_BOOL:
            return HAL_Snprintf(json_buf, buf_len, "\"%s\":%d", property->key, property->value.value_int);
        case DATA_TEMPLATE_TYPE_TIME:
            return HAL_Snprintf(json_buf, buf_len, "\"%s\":%u", property->key, property->value.value_time);
        case DATA_TEMPLATE_TYPE_STRING:
        case DATA_TEMPLATE_TYPE_STRING_ENUM:
            if (!property->value.value_string) {
                return 0;
            }
            return HAL_Snprintf(json_buf, buf_len, "\"%s\":\"%s\"", property->key, property->value.value_string);
        case DATA_TEMPLATE_TYPE_FLOAT:
            return HAL_Snprintf(json_buf, buf_len, "\"%s\":%f", property->key, property->value.value_float);
        case DATA_TEMPLATE_TYPE_STRUCT:
            len = HAL_Snprintf(json_buf, buf_len, "\"%s\":{", property->key);
            for (i = 0; i < property->value.value_struct.count; i++) {
                len += _get_property_node(json_buf + len, buf_len - len, property->value.value_struct.property + i);
                json_buf[len++] = ',';
            }
            json_buf[--len] = '}';
            return len + 1;
        case DATA_TEMPLATE_TYPE_ARRAY:
            Log_e("array type is not supportted yet!");
            return 0;
        default:
            Log_e("unkown type!");
            return 0;
    }
}

/**
 * @brief Parse property array.
 *
 * @param[in] json_buf json string to parse
 * @param[in] buf_len json len
 * @param[in,out] properties pointer to property array
 * @param[in] property_count count of property
 */
static void _parse_property_array(const char* json_buf, int buf_len, DataTemplateProperty* properties,
                                  int property_count)
{
    DataTemplateProperty* property;
    UtilsJsonValue        value;
    for (int i = 0; i < property_count; i++) {
        property = &properties[i];
        if (!property->is_rw) {  // read only property should not be processed
            continue;
        }
        if (!utils_json_value_get(property->key, strlen(property->key), json_buf, buf_len, &value)) {
            property->is_change = property->need_report = !_set_property_value(property, value);
        }
    }
}

#include "data_template_config_src_c.include"

/**************************************************************************************
 * API
 **************************************************************************************/

/**
 * @brief Init user data template(property/event/action).
 *
 */
void usr_data_template_init(void)
{
    _init_data_template_property();
    _init_data_template_event();
    _init_data_template_action();
}

/**
 * @brief Get property value.
 *
 * @param[in] index @see UsrPropertyIndex
 * @return @see DataTemplatePropertyValue
 */
DataTemplatePropertyValue usr_data_template_property_value_get(UsrPropertyIndex index)
{
    return sg_usr_data_template_property[index].value;
}

/**
 * @brief Set property value.
 *
 * @param[in] index @see UsrPropertyIndex
 * @param[in] value @see DataTemplatePropertyValue, @note value should match property type.
 */
void usr_data_template_property_value_set(UsrPropertyIndex index, DataTemplatePropertyValue value)
{
    if (sg_usr_data_template_property[index].type == DATA_TEMPLATE_TYPE_STRING ||
        sg_usr_data_template_property[index].type == DATA_TEMPLATE_TYPE_STRING_ENUM) {
        size_t value_len = strlen(value.value_string);
        strncpy(sg_usr_data_template_property[index].value.value_string, value.value_string, value_len);
        sg_usr_data_template_property[index].value.value_string[value_len] = '\0';
    } else {
        sg_usr_data_template_property[index].value = value;
    }
    sg_usr_data_template_property[index].need_report = 1;
}

/**
 * @brief Get property(struct) value.
 *
 * @param[in] struct_index @see UsrPropertyIndex, @note DATA_TEMPLATE_TYPE_STRUCT is required here.
 * @param[in] property_index depends on which struct
 * @return @see DataTemplatePropertyValue
 */
DataTemplatePropertyValue usr_data_template_property_struct_value_get(UsrPropertyIndex struct_index, int property_index)
{
    return sg_usr_data_template_property[struct_index].value.value_struct.property[property_index].value;
}

/**
 * @brief Set property(struct) value.
 *
 * @param[in] struct_index @see UsrPropertyIndex, @note DATA_TEMPLATE_TYPE_STRUCT is required here.
 * @param[in] property_index depends on which struct
 * @param[in] value  @see DataTemplatePropertyValue, @note value should match property type.
 */
void usr_data_template_property_struct_value_set(UsrPropertyIndex struct_index, int property_index,
                                                 DataTemplatePropertyValue value)
{
    if (sg_usr_data_template_property[struct_index].value.value_struct.property[property_index].type ==
            DATA_TEMPLATE_TYPE_STRING ||
        sg_usr_data_template_property[struct_index].value.value_struct.property[property_index].type ==
            DATA_TEMPLATE_TYPE_STRING_ENUM) {
        size_t value_len = strlen(value.value_string);
        char*  dst_str =
            sg_usr_data_template_property[struct_index].value.value_struct.property[property_index].value.value_string;
        strncpy(dst_str, value.value_string, value_len);
        dst_str[value_len] = '\0';
    } else {
        sg_usr_data_template_property[struct_index].value.value_struct.property[property_index].value = value;
    }
    sg_usr_data_template_property[struct_index].need_report = 1;
}

/**
 * @brief Parse control message and set property value.
 *
 * @param[in] params params filed of control message
 */
void usr_data_template_property_parse(UtilsJsonValue params)
{
    _parse_property_array(params.value, params.value_len, sg_usr_data_template_property, TOTAL_USR_PROPERTY_COUNT);
}

/**
 * @brief Get property status.
 *
 * @param[in] index @see UsrPropertyIndex
 * @return if property is changed
 */
int usr_data_template_property_status_get(UsrPropertyIndex index)
{
    return sg_usr_data_template_property[index].is_change;
}

/**
 * @brief Reset property status.
 *
 * @param[in] index @see UsrPropertyIndex
 */
void usr_data_template_property_status_reset(UsrPropertyIndex index)
{
    sg_usr_data_template_property[index].is_change = 0;
}

/**
 * @brief Get property type.
 *
 * @param[in] index @see UsrPropertyIndex
 * @return @see DataTemplatePropertyType
 */
DataTemplatePropertyType usr_data_template_property_type_get(UsrPropertyIndex index)
{
    return sg_usr_data_template_property[index].type;
}

/**
 * @brief Get property key.
 *
 * @param[in] index @see UsrPropertyIndex
 * @return key string
 */
const char* usr_data_template_property_key_get(UsrPropertyIndex index)
{
    return sg_usr_data_template_property[index].key;
}

/**
 * @brief Get property(struct) key.
 *
 * @param[in] struct_index @see UsrPropertyIndex, @note DATA_TEMPLATE_TYPE_STRUCT is required here.
 * @param[in] property_index depends on which struct
 * @return key string
 */
const char* usr_data_template_property_struct_key_get(UsrPropertyIndex struct_index, int property_index)
{
    return sg_usr_data_template_property[struct_index].value.value_struct.property[property_index].key;
}

/**
 * @brief Report all the properties needed report.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer to report
 * @param[in] buf_len buffer length
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int usr_data_template_property_report(void* client, char* buf, int buf_len)
{
    char params[1024];
    memset(params, 0, sizeof(params));
    params[0] = '{';
    int offset, param_offset = 1;
    for (int i = 0; i < TOTAL_USR_PROPERTY_COUNT; i++) {
        DataTemplateProperty* property = &sg_usr_data_template_property[i];
        if (property->need_report) {
            offset = _get_property_node(params + param_offset, sizeof(params) - param_offset, property);
            if (offset) {
                param_offset += offset;
                params[param_offset++] = ',';
            }
            property->need_report = 0;
        }
    }
    params[--param_offset] = '}';

    if (param_offset) {
        return IOT_DataTemplate_PropertyReport(client, buf, buf_len, params);
    }
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Post event.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer to report
 * @param[in] buf_len buffer length
 * @param[in] id @see UsrEventIndex
 * @param[in] params user should construct params according to event.
 */
void usr_data_template_event_post(void* client, char* buf, int buf_len, UsrEventIndex id, const char* params)
{
    if (params) {
        sg_usr_data_template_event[id].params = params;
    }
    IOT_DataTemplate_EventPost(client, buf, buf_len, sg_usr_data_template_event[id]);
}

/**
 * @brief Parse action message and set action input params.
 *
 * @param[in] action_id action id
 * @param[in] params params of action
 * @param[out] index @see UsrActionIndex
 * @return 0 for success, QCLOUD_ERR_JSON_PARSE for invalid json.
 */
int usr_data_template_action_parse(UtilsJsonValue action_id, UtilsJsonValue params, UsrActionIndex* index)
{
    DataTemplateProperty* property;

    int input_property_count;

    for (int i = 0; i < TOTAL_USR_ACTION_COUNT; i++) {
        if (!strncmp(action_id.value, sg_usr_data_template_action[i].action_id, action_id.value_len)) {
            property             = sg_usr_data_template_action[i].input_struct.value_struct.property;
            input_property_count = sg_usr_data_template_action[i].input_struct.value_struct.count;

            // 1. reset need report
            for (int j = 0; j < input_property_count; j++) {
                property[j].need_report = 0;
            }

            // 2. parse
            _parse_property_array(params.value, params.value_len, property, input_property_count);

            // 3. check all the input params is set
            for (int j = 0; j < input_property_count; j++) {
                if (!property[j].need_report) {
                    return QCLOUD_ERR_JSON_PARSE;
                }
            }
            *index = i;
            return QCLOUD_RET_SUCCESS;
        }
    }
    return QCLOUD_ERR_JSON_PARSE;
}

/**
 * @brief Get input value, should call after usr_data_template_action_parse
 *
 * @param[in] index index get from usr_data_template_action_parse
 * @param[in] property_index property index of action input params
 * @return @see DataTemplatePropertyValue
 */
DataTemplatePropertyValue usr_data_template_action_input_value_get(UsrActionIndex index, int property_index)
{
    return sg_usr_data_template_action[index].input_struct.value_struct.property[property_index].value;
}

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
                                   UtilsJsonValue client_token, int code, const char* response)
{
    sg_usr_data_template_action[index].reply.code         = code;
    sg_usr_data_template_action[index].reply.client_token = client_token;
    sg_usr_data_template_action[index].reply.response     = response;
    return IOT_DataTemplate_ActionReply(client, buf, buf_len, sg_usr_data_template_action[index].reply);
}
