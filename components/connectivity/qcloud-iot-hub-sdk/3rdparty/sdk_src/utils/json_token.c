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

#include "json_parser.h"
#include "lite-utils.h"
#include "qcloud_iot_export_error.h"

#ifndef SCNi8
#define SCNi8 "hhi"
#endif

#ifndef SCNu8
#define SCNu8 "hhu"
#endif

char *LITE_json_value_of(char *key, char *src)
{
    char *value     = NULL;
    int   value_len = -1;
    char *ret       = NULL;

    char *delim = NULL;
    char *key_iter;
    char *key_next;
    int   key_len;
    char *src_iter;

    src_iter = src;
    key_iter = key;

    do {
        if ((delim = strchr(key_iter, '.')) != NULL) {
            key_len  = delim - key_iter;
            key_next = HAL_Malloc(key_len + 1);
            strncpy(key_next, key_iter, key_len);
            key_next[key_len] = '\0';
            value             = json_get_value_by_name(src_iter, strlen(src_iter), key_next, &value_len, 0);

            if (value == NULL) {
                HAL_Free(key_next);
                return NULL;
            }

            src_iter = value;
            key_iter = delim + 1;
            HAL_Free(key_next);
        }
    } while (delim);

    value = json_get_value_by_name(src_iter, strlen(src_iter), key_iter, &value_len, 0);
    if (NULL == value) {
        return NULL;
    }
    ret = HAL_Malloc((value_len + 1) * sizeof(char));
    if (NULL == ret) {
        return NULL;
    }
    HAL_Snprintf(ret, value_len + 1, "%s", value);
    return ret;
}

list_head_t *LITE_json_keys_of(char *src, char *prefix)
{
    static LIST_HEAD(keylist);

    char *pos = 0, *key = 0, *val = 0;
    int   klen = 0, vlen = 0, vtype = 0;

    if (src == NULL || prefix == NULL) {
        return NULL;
    }

    if (!strcmp("", prefix)) {
        INIT_LIST_HEAD(&keylist);
    }

    json_object_for_each_kv(src, pos, key, klen, val, vlen, vtype)
    {
        if (key && klen && val && vlen) {
            json_key_t *entry = NULL;

            entry = HAL_Malloc(sizeof(json_key_t));
            memset(entry, 0, sizeof(json_key_t));
            entry->key = LITE_format_string("%s%.*s", prefix, klen, key);
            list_add_tail(&entry->list, &keylist);

            if (JSOBJECT == vtype) {
                char *iter_val = LITE_format_string("%.*s", vlen, val);
                char *iter_pre = LITE_format_string("%s%.*s.", prefix, klen, key);
                LITE_json_keys_of(iter_val, iter_pre);
                HAL_Free(iter_val);
                HAL_Free(iter_pre);
            }
        }
    }

    if (!strcmp("", prefix)) {
        json_key_t *entry = NULL;

        entry = HAL_Malloc(sizeof(json_key_t));
        memset(entry, 0, sizeof(json_key_t));
        list_add_tail(&entry->list, &keylist);

        return &keylist;
    }

    return NULL;
}

void LITE_json_keys_release(list_head_t *keylist)
{
    json_key_t *pos, *tmp;

    list_for_each_entry_safe(pos, tmp, keylist, list, json_key_t)
    {
        if (pos->key) {
            HAL_Free(pos->key);
        }
        list_del(&pos->list);
        HAL_Free(pos);
    }
}

static void _strip_transfer(char *src)
{
    char *end = src + strlen(src) + 1;

    while (*src != '\0') {
        if (*src == '\\') {
            memmove(src, src + 1, end - src);
            end--;
        }
        src++;
    }
}

char *LITE_json_string_value_strip_transfer(char *key, char *src)
{
    char *str = LITE_json_value_of(key, src);

    if (NULL != str) {
        _strip_transfer(str);
    }
    return str;
}

int LITE_get_int32(int32_t *value, char *src)
{
    return (sscanf(src, "%" SCNi32, value) == 1) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

int LITE_get_int16(int16_t *value, char *src)
{
    return (sscanf(src, "%" SCNi16, value) == 1) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

int LITE_get_int8(int8_t *value, char *src)
{
    return (sscanf(src, "%" SCNi8, value) == 1) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

int LITE_get_uint32(uint32_t *value, char *src)
{
    return (sscanf(src, "%" SCNu32, value) == 1) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

int LITE_get_uint16(uint16_t *value, char *src)
{
    return (sscanf(src, "%" SCNu16, value) == 1) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

int LITE_get_uint8(uint8_t *value, char *src)
{
    return (sscanf(src, "%" SCNu8, value) == 1) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

int LITE_get_float(float *value, char *src)
{
    return (sscanf(src, "%f", value) == 1) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

int LITE_get_double(double *value, char *src)
{
    return (sscanf(src, "%lf", value) == 1) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

int LITE_get_boolean(bool *value, char *src)
{
    if (!strcmp(src, "false")) {
        *value = false;
    } else {
        *value = true;
    }

    return QCLOUD_RET_SUCCESS;
}
