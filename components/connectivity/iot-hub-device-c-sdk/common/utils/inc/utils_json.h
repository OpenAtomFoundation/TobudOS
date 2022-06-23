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
 * @file utils_json.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-24
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-24 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-29 <td>1.1     <td>fancyxu   <td>fix bug and add utils_json_value_data_get
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_JSON_H_
#define IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_JSON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

/**
 * @brief Json value type
 *
 */
typedef enum {
    UTILS_JSON_VALUE_TYPE_INT32 = 0,
    UTILS_JSON_VALUE_TYPE_INT64,
    UTILS_JSON_VALUE_TYPE_UINT32,
    UTILS_JSON_VALUE_TYPE_UINT64,
    UTILS_JSON_VALUE_TYPE_FLOAT,
    UTILS_JSON_VALUE_TYPE_DOUBLE,
    UTILS_JSON_VALUE_TYPE_BOOLEAN,
} UtilsJsonValueType;

/**
 * @brief Json array result
 *
 */
typedef enum {
    UTILS_JSON_ARRAY_ITER_CONTINUE,
    UTILS_JSON_ARRAY_ITER_STOP,
} UtilsJsonArrayIterResult;

/**
 * @brief Json value
 *
 */
typedef struct {
    const char *value;
    int         value_len;
} UtilsJsonValue;

/**
 * @brief Get value from json string. Not strict, just for iot scene, we suppose all the string is valid json.
 *
 * @param[in] key key in json, support nesting with '.'
 * @param[in] key_len key len
 * @param[in] src json string
 * @param[in] src_len src length
 * @param[out] value value
 * @return 0 for success
 */
int utils_json_value_get(const char *key, int key_len, const char *src, int src_len, UtilsJsonValue *value);

/**
 * @brief Get data of value with type.
 *
 * @param[in] value @see UtilsJsonValue
 * @param[in] type value type, string can use value directly @see UtilsJsonValueType
 * @param[out] data data pointer, user should match the type
 * @return 0 for success
 */
int utils_json_value_data_get(UtilsJsonValue value, UtilsJsonValueType type, void *data);

/**
 * @brief Return unsigned int value of key in json.
 *
 * @param[in] key key in json, support nesting with '.'
 * @param[in] key_len key len
 * @param[in] src json string
 * @param[in] src_len src length
 * @param[out] data data value
 * @return 0 for success
 */
int utils_json_get_uint32(const char *key, int key_len, const char *src, int src_len, uint32_t *data);

/**
 * @brief Return int value of key in json.
 *
 * @param[in] key key in json, support nesting with '.'
 * @param[in] key_len key len
 * @param[in] src json string
 * @param[in] src_len src length
 * @param[out] data data value
 * @return 0 for success
 */
int utils_json_get_int32(const char *key, int key_len, const char *src, int src_len, int32_t *data);

/**
 * @brief Remove '\\' in json string.
 *
 * @param[in,out] src string to transfer
 * @param[in] src_len string len
 * @return length after transfer
 */
int utils_json_strip_transfer(char *src, int src_len);

/**
 * @brief Parse array object, assume array json is legal, src should be like "[12, 456]", this function will split array
 * according to array_elem_type, obj_cb will be called for each elements.
 *
 * @param[in] src array string
 * @param[in] src_len length of src
 * @param[in] obj_cb callback to deal with array element
 * @param[in] arg argument passed to the obj_cb
 */
void utils_json_array_parse(const char *src, int src_len, UtilsJsonArrayIterResult (*obj_cb)(const char *, int, void *),
                            void *arg);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_JSON_H_
