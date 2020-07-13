/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include "lite-utils.h"

/**
The descriptions of the json value node type
**/
enum JSONTYPE { JSNONE = -1, JSSTRING = 0, JSOBJECT, JSARRAY, JSNUMBER, JSBOOLEAN, JSNULL, JSTYPEMAX };

/**
The error codes produced by the JSON parsers
**/
enum JSON_PARSE_CODE { JSON_PARSE_ERR, JSON_PARSE_OK, JSON_PARSE_FINISH };

/**
The return codes produced by the JSON parsers
**/
enum JSON_PARSE_RESULT { JSON_RESULT_ERR = -1, JSON_RESULT_OK };

typedef int (*json_parse_cb)(char *p_cName, int iNameLen, char *p_cValue, int iValueLen, int iValueType,
                             void *p_Result);

/**
 * @brief Parse the JSON string, and iterate through all keys and values,
 * then handle the keys and values by callback function.
 *
 * @param[in]  p_cJsonStr @n  The JSON string
 * @param[in]  iStrLen    @n  The JSON string length
 * @param[in]  pfnCB      @n  Callback function
 * @param[out] p_CBData   @n  User data
 * @return JSON_RESULT_OK success, JSON_RESULT_ERR failed
 * @see None.
 * @note None.
 **/
int json_parse_name_value(char *p_cJsonStr, int iStrLen, json_parse_cb pfnCB, void *p_CBData);

/**
 * @brief Get the value by a specified key from a json string
 *
 * @param[in]  p_cJsonStr   @n the JSON string
 * @param[in]  iStrLen      @n the JSON string length
 * @param[in]  p_cName      @n the specified key string
 * @param[out] p_iValueLen  @n the value length
 * @param[out] p_iValueType @n the value type
 * @return A pointer to the value
 * @see None.
 * @note None.
 **/
char *json_get_value_by_name(char *p_cJsonStr, int iStrLen, char *p_cName, int *p_iValueLen, int *p_iValueType);

/**
 * @brief Get the JSON object point associate with a given type.
 *
 * @param[in] type @n The object type
 * @param[in] str  @n The JSON string
 * @returns The json object point with the given field type.
 * @see None.
 * @note None.
 */
char *json_get_object(int type, char *str);
char *json_get_next_object(int type, char *str, char **key, int *key_len, char **val, int *val_len, int *val_type);

/**
 * @brief retrieve each key&value pair from the json string
 *
 * @param[in]  str   @n Json string to revolve
 * @param[in]  pos   @n cursor
 * @param[out] key   @n pointer to the next Key object
 * @param[out] klen  @n Key object length
 * @param[out] val   @n pointer to the next Value object
 * @param[out] vlen  @n Value object length
 * @param[out] vtype @n Value object type(digital, string, object, array)
 * @see None.
 * @note None.
 */
#define json_object_for_each_kv(str, pos, key, klen, val, vlen, vtype) \
    for (pos = json_get_object(JSOBJECT, str);                         \
         pos != 0 && *pos != 0 && (pos = json_get_next_object(JSOBJECT, pos, &key, &klen, &val, &vlen, &vtype)) != 0;)

/**
 * @brief retrieve each entry from the json array
 *
 * @param[in]  str   @n Json array to revolve
 * @param[in]  pos   @n cursor
 * @param[out] entry @n pointer to the next entry from the array
 * @param[out] len   @n entry length
 * @param[out] type  @n entry type(digital, string, object, array)
 * @see None.
 * @note None.
 */
#define json_array_for_each_entry(str, pos, entry, len, type) \
    for (pos = json_get_object(JSARRAY, str);                 \
         pos != 0 && *pos != 0 && (pos = json_get_next_object(JSARRAY, ++pos, 0, 0, &entry, &len, &type)) != 0;)

/**
 * @brief backup the last character to register parameters,
 *          and set the end character with '\0'
 *
 * @param[in]  json_str @n json string
 * @param[in]  str_len  @n json string lenth
 * @param[out] register @n used to backup the last character
 * @see None.
 * @note None.
 */
#define backup_json_str_last_char(json_str, str_len, register)         \
    {                                                                  \
        register                      = *((char *)json_str + str_len); \
        *((char *)json_str + str_len) = '\0';                          \
    }

/**
 * @brief restore the last character from register parameters
 *
 * @param[in]  json_str @n json string
 * @param[in]  str_len  @n json string lenth
 * @param[in] register  @n used to restore the last character
 * @see None.
 * @note None.
 */
#define restore_json_str_last_char(json_str, str_len, register) \
    {                                                           \
        *((char *)json_str + str_len) = register;               \
    }

#endif /* __JSON_PARSER_H__ */
