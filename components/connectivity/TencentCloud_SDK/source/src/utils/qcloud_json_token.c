/*
 * Copyright (c) 2017-2019 Tencent Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "qcloud.h"

#ifndef SCNi8
#define SCNi8 "hhi"
#endif

#ifndef SCNu8
#define SCNu8 "hhu"
#endif

#ifndef SCNi16
#define SCNi16 "hi"
#endif

#ifndef SCNu16
#define SCNu16 "hu"
#endif

#ifndef SCNi32
#define SCNi32 "i"
#endif

#ifndef SCNu32
#define SCNu32 "u"
#endif

char *LITE_json_value_of(char *key, char *src)
{
    char       *value = NULL;
    int         value_len = -1;
    char       *ret = NULL;

    char       *delim = NULL;
    char       *key_iter;
    char       *key_next;
    int         key_len;
    char       *src_iter;

    src_iter = src;
    key_iter = key;

    do {
        if ((delim = strchr(key_iter, '.')) != NULL) {
            key_len = delim - key_iter;
            key_next = osal_malloc(key_len + 1);
            strncpy(key_next, key_iter, key_len);
            key_next[key_len] = '\0';
            value = json_get_value_by_name(src_iter, strlen(src_iter), key_next, &value_len, 0);

            if (value == NULL) {
                osal_free(key_next);
                return NULL;
            }

            src_iter = value;
            key_iter = delim + 1;
            osal_free(key_next);
        }
    } while (delim);

    value = json_get_value_by_name(src_iter, strlen(src_iter), key_iter, &value_len, 0);
    if (NULL == value) {
        return NULL;
    }
    ret = osal_malloc((value_len + 1) * sizeof(char));
    if (NULL == ret) {
        return NULL;
    }
    osal_snprintf(ret, value_len + 1, "%s", value);
    return ret;
}

list_head_t *LITE_json_keys_of(char *src, char *prefix)
{
    static              LIST_HEAD(keylist);

    char    *pos = 0, *key = 0, *val = 0;
    int     klen = 0, vlen = 0, vtype = 0;

    if (src == NULL || prefix == NULL) {
        return NULL;
    }

    if (!strcmp("", prefix)) {
        INIT_LIST_HEAD(&keylist);
    }

    json_object_for_each_kv(src, pos, key, klen, val, vlen, vtype) {
        if (key && klen && val && vlen) {

            json_key_t     *entry = NULL;

            entry = osal_malloc(sizeof(json_key_t));
            memset(entry, 0, sizeof(json_key_t));
            entry->key = LITE_format_string("%s%.*s", prefix, klen, key);
            list_add_tail(&entry->list, &keylist);

            if (JSOBJECT == vtype) {
                char *iter_val = LITE_format_string("%.*s", vlen, val);
                char *iter_pre = LITE_format_string("%s%.*s.", prefix, klen, key);
                LITE_json_keys_of(iter_val, iter_pre);
                osal_free(iter_val);
                osal_free(iter_pre);
            }
        }
    }

    if (!strcmp("", prefix)) {
        json_key_t     *entry = NULL;

        entry = osal_malloc(sizeof(json_key_t));
        memset(entry, 0, sizeof(json_key_t));
        list_add_tail(&entry->list, &keylist);

        return &keylist;
    }

    return NULL;
}

void LITE_json_keys_release(list_head_t *keylist)
{
    json_key_t         *pos, *tmp;

    list_for_each_entry_safe(pos, tmp, keylist, list, json_key_t) {
        if (pos->key) {
            osal_free(pos->key);
        }
        list_del(&pos->list);
        osal_free(pos);
    }
}

qcloud_err_t LITE_get_int32(int32_t *value, char *src) {
	return (sscanf(src, "%" SCNi32, value) == 1) ? QCLOUD_ERR_SUCCESS : QCLOUD_ERR_FAILURE;
}

qcloud_err_t LITE_get_int16(int16_t *value, char *src) {
	return (sscanf(src, "%" SCNi16, value) == 1) ? QCLOUD_ERR_SUCCESS : QCLOUD_ERR_FAILURE;
}

qcloud_err_t LITE_get_int8(int8_t *value, char *src) {
	return (sscanf(src, "%" SCNi8, value) == 1) ? QCLOUD_ERR_SUCCESS : QCLOUD_ERR_FAILURE;
}

qcloud_err_t LITE_get_uint32(uint32_t *value, char *src) {
	return (sscanf(src, "%" SCNu32, value) == 1) ? QCLOUD_ERR_SUCCESS : QCLOUD_ERR_FAILURE;
}

qcloud_err_t LITE_get_uint16(uint16_t *value, char *src) {
	return (sscanf(src, "%" SCNu16, value) == 1) ? QCLOUD_ERR_SUCCESS : QCLOUD_ERR_FAILURE;
}

qcloud_err_t LITE_get_uint8(uint8_t *value, char *src) {
	return (sscanf(src, "%" SCNu8, value) == 1) ? QCLOUD_ERR_SUCCESS : QCLOUD_ERR_FAILURE;
}

qcloud_err_t LITE_get_float(float *value, char *src) {
	return (sscanf(src, "%f", value) == 1) ? QCLOUD_ERR_SUCCESS : QCLOUD_ERR_FAILURE;
}

qcloud_err_t LITE_get_double(double *value, char *src) {
	return (sscanf(src, "%lf", value) == 1) ? QCLOUD_ERR_SUCCESS : QCLOUD_ERR_FAILURE;
}

qcloud_err_t LITE_get_boolean(bool *value, char *src) {
	if (!strcmp(src, "false")) {
		*value = false;
	}
	else {
		*value = true;
	}

	return QCLOUD_ERR_SUCCESS;
}

