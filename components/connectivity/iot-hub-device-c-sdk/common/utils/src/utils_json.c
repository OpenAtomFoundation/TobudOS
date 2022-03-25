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
 * @file utils_json.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-25
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-25 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-29 <td>1.1     <td>fancyxu   <td>fix bug and add utils_json_value_data_get
 * </table>
 */

#include "utils_json.h"

/**
 * @brief Delimiter of json
 *
 */
typedef enum {
    JSON_DELIMITER_ARRAY_BEGIN              = '[',
    JSON_DELIMITER_ARRAY_END                = ']',
    JSON_DELIMITER_OBJECT_BEGIN             = '{',
    JSON_DELIMITER_OBJECT_END               = '}',
    JSON_DELIMITER_KEY                      = '"',
    JSON_DELIMITER_VALUE                    = ':',
    JSON_DELIMITER_ELEMENT_END              = ',',
    JSON_DELIMITER_TYPE_STRING              = '"',
    JSON_DELIMITER_TYPE_BOOLEAN_TRUE_UPPER  = 'T',
    JSON_DELIMITER_TYPE_BOOLEAN_TRUE_LOWER  = 't',
    JSON_DELIMITER_TYPE_BOOLEAN_FALSE_UPPER = 'F',
    JSON_DELIMITER_TYPE_BOOLEAN_FALSE_LOWER = 'f',
    JSON_DELIMITER_TYPE_NULL_UPPER          = 'N',
    JSON_DELIMITER_TYPE_NULL_LOWER          = 'n',
    JSON_DELIMITER_SPACE                    = ' ',
    JSON_DELIMITER_NONE                     = 0,
} JsonDelimiter;

/**
 * @brief Element type support
 *
 */
typedef enum {
    JSON_ELEMENT_ARRAY = 0,
    JSON_ELEMENT_OBJECT,
    JSON_ELEMENT_VALUE_STRING,
    JSON_ELEMENT_VALUE_NUMBER,
    JSON_ELEMENT_VALUE_BOOLEAN,
    JSON_ELEMENT_VALUE_NULL,
    JSON_ELEMENT_KEY,
} JsonElementType;

/**
 * @brief Json Element
 *
 */
typedef struct {
    JsonElementType type;
    char           *pos_begin;
    char           *pos_end;
    int             element_len;
} JsonElement;

#define json_key   JsonElement
#define json_value JsonElement

/**
 * @brief Is delimiter begin or end matched
 *
 */
typedef struct {
    int (*is_begin_matched)(char *str);
    int (*is_end_matched)(char *str, int *offset);
} IsDelimiterMatched;

/**
 * @brief For array type, when meets '[', means depth++
 *
 * @param[in] str pointer to char
 * @return true for matched
 */
static int _is_array_begin_matched(char *str)
{
    return *str == JSON_DELIMITER_ARRAY_BEGIN;
}

/**
 * @brief For array type, when meets ']', means depth--
 *
 * @param[in] str pointer to char
 * @param[out] offset offset for pointer to move
 * @return true for matched
 */
static int _is_array_end_matched(char *str, int *offset)
{
    return *str == JSON_DELIMITER_ARRAY_END;
}

/**
 * @brief For object type, when meets '{', means depth++
 *
 * @param[in] str pointer to char
 * @return true for matched
 */
static int _is_object_begin_matched(char *str)
{
    return *str == JSON_DELIMITER_OBJECT_BEGIN;
}

/**
 * @brief For object type, when meets '}', means depth--
 *
 * @param[in] str pointer to char
 * @param[out] offset offset for pointer to move
 * @return true for matched
 */
static int _is_object_end_matched(char *str, int *offset)
{
    return *str == JSON_DELIMITER_OBJECT_END;
}

/**
 * @brief For string type, when meets '"', but not "\"", means the end
 *
 * @param[in] str pointer to char
 * @param[out] offset offset for pointer to move
 * @return true for matched
 */
static int _is_string_end_matched(char *str, int *offset)
{
    return *str == '"' && *(str - 1) != '\\';
}

/**
 * @brief For number type, when meets not '0~9', '.' or 'f' for float, means the end
 *
 * @param[in] str pointer to char
 * @param[out] offset offset for pointer to move
 * @return true for matched
 */
static int _is_number_end_matched(char *str, int *offset)
{
    return (*str < '0' || *str > '9') && *str != '.' && *str != 'f' && *str != '-';
}

/**
 * @brief For boolen type, when meets "true", "True", "false" or "False", means the end
 *
 * @param[in] str pointer to char
 * @param[out] offset offset for pointer to move, -1 means error
 * @return true for matched
 */
static int _is_boolean_end_matched(char *str, int *offset)
{
    *offset = !strncmp(str, "true", 4) || !strncmp(str, "TRUE", 4) ? 4 : *offset;
    *offset = !strncmp(str, "false", 5) || !strncmp(str, "FALSE", 5) ? 5 : *offset;
    if (*offset == 0) {
        *offset = -1;  // -1 means error
    }
    return 1;
}

/**
 * @brief For boolen type, when meets "null", "NULL", means the end
 *
 * @param[in] str pointer to char
 * @param[out] offset offset for pointer to move, -1 means error
 * @return true for matched
 */
static int _is_null_end_matched(char *str, int *offset)
{
    *offset = !strncmp(str, "null", 4) || !strncmp(str, "NULL", 4) ? 4 : 0;
    if (*offset == 0) {
        *offset = -1;  // -1 means error
    }
    return 1;
}

/**
 * @brief Table of matched function of elements
 *
 */
static IsDelimiterMatched sg_element_matched_func[] = {
    {_is_array_begin_matched, _is_array_end_matched},    // JSON_ELEMENT_ARRAY
    {_is_object_begin_matched, _is_object_end_matched},  // JSON_ELEMENT_OBJECT
    {NULL, _is_string_end_matched},                      // JSON_ELEMENT_VALUE_STRING
    {NULL, _is_number_end_matched},                      // JSON_ELEMENT_VALUE_NUMBER
    {NULL, _is_boolean_end_matched},                     // JSON_ELEMENT_VALUE_BOOLEAN
    {NULL, _is_null_end_matched},                        // JSON_ELEMENT_VALUE_NULL
    {NULL, NULL},                                        // JSON_ELEMENT_KEY
};

/**
 * @brief Increase pos and decrease remain len
 *
 * @param[in,out] pos current pos
 * @param[in,out] len current len
 */
static void _increase_pos(char **pos, int *len)
{
    (*pos)++;
    (*len)--;
};

/**
 * @brief Find delimiter and return pos
 *
 * @param[in] ch delimiter character
 * @param[in] spilt_char if 0 then spilt nothing but delimiter
 * @param[in] str input string
 * @param[in,out] remain_len remaining length
 * @return NULL for failed
 */
static char *_find_json_delimiter(JsonDelimiter ch, char spilt_char, char *str, int *remain_len)
{
    char *pos = NULL;

    while ((*remain_len) > 0 && str && *str) {
        if ((!spilt_char && *str != ch) || (spilt_char && *str == spilt_char)) {
            _increase_pos(&str, remain_len);
            continue;
        }

        pos = (*str == ch) ? str : 0;
        break;
    }

    return pos;
}

/**
 * @brief Find element end
 *
 * @param[in] is_begin_matched using in nesting
 * @param[in] is_end_matched match end
 * @param[in] str input string
 * @param[in,out] remain_len remaining length
 * @return NULL for failed
 */
static char *_find_json_value_element_end(int (*is_begin_matched)(char *), int (*is_end_matched)(char *, int *),
                                          char *str, int *remain_len)
{
    int   offset = 0;
    int   depth  = 0;
    char *pos    = NULL;

    while (str && *str) {
        offset = 0;

        if (*remain_len <= 0) {
            return NULL;
        }

        if (is_begin_matched) {
            if (is_begin_matched(str)) {
                depth++;
            }
        }

        if (is_end_matched(str, &offset)) {
            if (!depth) {
                if (offset < 0) {
                    break;  // failed
                }
                pos = str + offset;
                (*remain_len) -= offset;
                break;
            }

            if (depth > 0) {
                depth--;
            }
        }

        str++;
        (*remain_len)--;
    }

    return pos;
}

/**
 * @brief Get json key from string
 *
 * @param[in] node_begin string begin
 * @param[in,out] remain_len remaining length
 * @param[out] key key node
 * @return 0 for success
 */
static int _get_json_key(char *node_begin, int *remain_len, json_key *key)
{
    char *key_begin = _find_json_delimiter(JSON_DELIMITER_KEY, JSON_DELIMITER_SPACE, node_begin, remain_len);
    if (!key_begin) {
        return -1;
    }
    _increase_pos(&key_begin, remain_len);

    char *key_end = _find_json_delimiter(JSON_DELIMITER_KEY, JSON_DELIMITER_NONE, key_begin, remain_len);
    if (!key_end) {
        return -1;
    }

    key->type        = JSON_ELEMENT_KEY;
    key->pos_begin   = key_begin;
    key->pos_end     = key_end;
    key->element_len = key_end - key_begin;
    return 0;
}

/**
 * @brief Get json value from key end
 *
 * @param[in] key_end end of key
 * @param[in,out] remain_len remaining length
 * @param[out] value value node
 * @return 0 for success
 */
static int _get_json_value(char *key_end, int *remain_len, json_value *value)
{
    char *value_begin = _find_json_delimiter(JSON_DELIMITER_VALUE, JSON_DELIMITER_SPACE, key_end, remain_len);
    if (!value_begin) {
        return -1;
    }
    _increase_pos(&value_begin, remain_len);

    // filter all the space
    while (*value_begin == ' ') {
        value_begin++;
        (*remain_len)--;
        if (*remain_len <= 0) {
            return -1;
        }
    };

    JsonElementType type;

    char *value_end = NULL;

    switch (*value_begin) {
        case JSON_DELIMITER_ARRAY_BEGIN:
            // should + 1 for escape first [
            _increase_pos(&value_begin, remain_len);
            type = JSON_ELEMENT_ARRAY;
            break;
        case JSON_DELIMITER_OBJECT_BEGIN:
            // should + 1 for escape first {
            _increase_pos(&value_begin, remain_len);
            type = JSON_ELEMENT_OBJECT;
            break;
        case JSON_DELIMITER_TYPE_STRING:
            // should + 1 for escape first "
            _increase_pos(&value_begin, remain_len);
            type = JSON_ELEMENT_VALUE_STRING;
            break;
        case JSON_DELIMITER_TYPE_BOOLEAN_TRUE_UPPER:
        case JSON_DELIMITER_TYPE_BOOLEAN_TRUE_LOWER:
        case JSON_DELIMITER_TYPE_BOOLEAN_FALSE_UPPER:
        case JSON_DELIMITER_TYPE_BOOLEAN_FALSE_LOWER:
            type = JSON_ELEMENT_VALUE_BOOLEAN;
            break;
        case JSON_DELIMITER_TYPE_NULL_UPPER:
        case JSON_DELIMITER_TYPE_NULL_LOWER:
            type = JSON_ELEMENT_VALUE_NULL;
            break;
        default:
            type = JSON_ELEMENT_VALUE_NUMBER;
            break;
    }

    value_end = _find_json_value_element_end(sg_element_matched_func[type].is_begin_matched,
                                             sg_element_matched_func[type].is_end_matched, value_begin, remain_len);
    if (!value_end) {
        return -1;
    }

    int is_object_or_array           = (type == JSON_ELEMENT_ARRAY || type == JSON_ELEMENT_OBJECT);
    int is_object_or_array_or_string = (is_object_or_array || type == JSON_ELEMENT_VALUE_STRING);

    value->type        = type;
    value->pos_begin   = value_begin - is_object_or_array;                   // include '[' or '{'
    value->element_len = value_end - value->pos_begin + is_object_or_array;  // include ']' or '}'
    value->pos_end     = value_end + is_object_or_array_or_string;           // filter '"' , ']' or '}'
    return 0;
}

/**
 * @brief Get value by key from json string
 *
 * @param[in] str json string
 * @param[in] str_len strign length
 * @param[in] key key
 * @param[in] key_len key length
 * @param[out] value value
 * @return 0 for success
 */
int json_get_value_by_key(const char *str, int str_len, const char *key, int key_len, json_value *value)
{
    int      rc           = 0;
    char    *object_begin = NULL;
    json_key key_found    = {JSON_ELEMENT_KEY, 0, 0, 0};

    object_begin = _find_json_delimiter(JSON_DELIMITER_OBJECT_BEGIN, JSON_DELIMITER_SPACE, (char *)str, &str_len);
    if (!object_begin) {
        return -1;
    }
    _increase_pos(&object_begin, &str_len);

    do {
        rc = _get_json_key(object_begin, &str_len, &key_found);
        if (rc) {
            break;
        }
        _increase_pos(&key_found.pos_end, &str_len);

        rc = _get_json_value(key_found.pos_end, &str_len, value);
        if (rc) {
            break;
        }

        // NULL of object begin can be ok for reach the end of str
        object_begin = _find_json_delimiter(JSON_DELIMITER_ELEMENT_END, JSON_DELIMITER_SPACE, value->pos_end, &str_len);
        if (object_begin) {
            _increase_pos(&object_begin, &str_len);
        }
    } while (key_len != key_found.element_len || strncmp(key, key_found.pos_begin, key_found.element_len));

    return rc;
}

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
int utils_json_value_get(const char *key, int key_len, const char *src, int src_len, UtilsJsonValue *value)
{
    int rc = 0;

    char *delim        = NULL;
    char *src_iter     = (char *)src;
    int   src_iter_len = src_len;
    char *key_iter     = (char *)key;
    char *key_next     = NULL;
    int   key_next_len = 0;

    json_value value_tmp;

    // key can be separated by '.', such as: outer_key.(.......).inner_key
    while ((delim = strchr(key_iter, '.'))) {
        key_next     = key_iter;
        key_next_len = delim - key_iter;

        rc = json_get_value_by_key(src_iter, src_iter_len, key_next, key_next_len, &value_tmp);
        if (rc) {
            return rc;
        }

        src_iter     = value_tmp.pos_begin;
        src_iter_len = value_tmp.element_len;
        key_iter     = delim + 1;
    }

    // found inner key and get value
    rc = json_get_value_by_key(src_iter, src_iter_len, key_iter, key_len + key - key_iter, &value_tmp);
    if (rc) {
        return rc;
    }

    value->value     = value_tmp.pos_begin;
    value->value_len = value_tmp.element_len;
    return 0;
}

/**
 * @brief Get data of value with type.
 *
 * @param[in] value @see UtilsJsonValue
 * @param[in] type value type, string can use value directly. @see UtilsJsonValueType
 * @param[out] data data pointer, user should
 * @return 0 for success
 */
int utils_json_value_data_get(UtilsJsonValue value, UtilsJsonValueType type, void *data)
{
    char value_tmp[32] = {0};

    if (value.value_len > sizeof(value_tmp)) {
        return -1;
    }

    strncpy(value_tmp, value.value, value.value_len);

    switch (type) {
        case UTILS_JSON_VALUE_TYPE_INT32:
            return !(sscanf(value_tmp, "%" SCNi32, (int32_t *)data) == 1);
        case UTILS_JSON_VALUE_TYPE_INT64:
            return !(sscanf(value_tmp, "%" SCNi64, (int64_t *)data) == 1);
        case UTILS_JSON_VALUE_TYPE_UINT32:
            return !(sscanf(value_tmp, "%" SCNu32, (uint32_t *)data) == 1);
        case UTILS_JSON_VALUE_TYPE_UINT64:
            return !(sscanf(value_tmp, "%" SCNu64, (uint64_t *)data) == 1);
        case UTILS_JSON_VALUE_TYPE_FLOAT:
            return !(sscanf(value_tmp, "%f", (float *)data) == 1);
        case UTILS_JSON_VALUE_TYPE_DOUBLE:
            return !(sscanf(value_tmp, "%lf", (double *)data) == 1);
        case UTILS_JSON_VALUE_TYPE_BOOLEAN:
            *(int *)data = strcmp(value_tmp, "false");
            return 0;
        default:
            break;
    }
    return -1;
}

/**
 * @brief Return unsigned int value of key in json.
 *
 * @return 0 for success
 */
int utils_json_get_uint32(const char *key, int key_len, const char *src, int src_len, uint32_t *data)
{
    int            rc;
    UtilsJsonValue tmp;

    rc = utils_json_value_get(key, key_len, src, src_len, &tmp);
    if (rc) {
        return -1;
    }

    rc = utils_json_value_data_get(tmp, UTILS_JSON_VALUE_TYPE_UINT32, data);
    if (rc) {
        return -1;
    }
    return 0;
}

/**
 * @brief Remove '\\' in json string.
 *
 * @param[in,out] src string to transfer
 * @param[in] src_len string len
 * @return length after transfer
 */
int utils_json_strip_transfer(char *src, int src_len)
{
    char *src_tmp = src;
    char *end     = src + src_len;

    while (src_tmp < end) {
        if (*src_tmp == '\\') {
            memmove(src_tmp, src_tmp + 1, end - src_tmp);
            end--;
        }
        src_tmp++;
    }
    return end - src;
}
