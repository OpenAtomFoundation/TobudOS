/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "infra_config.h"

#ifdef INFRA_JSON_PARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "infra_types.h"
#include "infra_json_parser.h"

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

#ifdef INFRA_LOG
#include "infra_log.h"
#define jparser_debug(...)              log_debug("jparser", __VA_ARGS__)
#else
#define jparser_debug(...)              do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#endif

#ifdef INFRA_MEM_STATS
#include "infra_mem_stats.h"
#define jparser_malloc(size)            LITE_malloc(size, MEM_MAGIC, "jparser")
#define jparser_free(ptr)               LITE_free(ptr)
#else
#define jparser_malloc(size)            HAL_Malloc(size)
#define jparser_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

char *json_get_object(int type, char *str, char *str_end)
{
    char *pos = NULL;
    char ch = (type == JOBJECT) ? '{' : '[';

    if (!str || !str_end) {
        return NULL;
    }

    while (str != NULL && *str != 0 && str < str_end) {
        if (*str == ' ') {
            str++;
            continue;
        }
        pos = (*str == ch) ? str : NULL;
        break;
    }
    return pos;
}

char *json_get_next_object(int type, char *str, char *str_end, char **key, int *key_len,
                           char **val, int *val_len, int *val_type)
{
    char    JsonMark[JTYPEMAX][2] = { { '\"', '\"' }, { '{', '}' }, { '[', ']' }, { '0', ' ' } };
    int     iMarkDepth = 0, iValueType = JNONE, iNameLen = 0, iValueLen = 0, iStringDepth = 0;
    char   *p_cName = 0, *p_cValue = 0, *p_cPos = str;

    if (type == JOBJECT) {
        /* Get Key */
        p_cPos = strchr(p_cPos, '\"');
        if (!p_cPos) {
            goto do_exit;
        }
        p_cName = ++p_cPos;
        p_cPos = strchr(p_cPos, '\"');
        if (!p_cPos) {
            goto do_exit;
        }
        iNameLen = p_cPos - p_cName;

        /* Get Value */
        p_cPos = strchr(p_cPos, ':');
    }
    while (p_cPos && *p_cPos && p_cPos < str_end) {
        if (*p_cPos == '\"') {
            iValueType = JSTRING;
            p_cValue = ++p_cPos;
            break;
        } else if (*p_cPos == '{') {
            iValueType = JOBJECT;
            p_cValue = p_cPos++;
            break;
        } else if (*p_cPos == '[') {
            iValueType = JARRAY;
            p_cValue = p_cPos++;
            break;
        } else if ((*p_cPos == '-') || (*p_cPos >= '0' && *p_cPos <= '9')) {
            iValueType = JNUMBER;
            p_cValue = p_cPos++;
            break;
        } else if (*p_cPos == 't' || *p_cPos == 'T' || *p_cPos == 'f' || *p_cPos == 'F') {
            iValueType = JBOOLEAN;
            p_cValue = p_cPos;
            break;
        }
        p_cPos++;
    }

    while (p_cPos && *p_cPos && p_cPos < str_end && iValueType > JNONE) {
        if (iValueType == JBOOLEAN) {
            int     len = strlen(p_cValue);

            if ((*p_cValue == 't' || *p_cValue == 'T') && len >= 4
                && (!strncmp(p_cValue, "true", 4)
                    || !strncmp(p_cValue, "TRUE", 4))) {
                iValueLen = 4;
                p_cPos = p_cValue + iValueLen;
                break;
            } else if ((*p_cValue == 'f' || *p_cValue == 'F') && len >= 5
                       && (!strncmp(p_cValue, "false", 5)
                           || !strncmp(p_cValue, "FALSE", 5))) {
                iValueLen = 5;
                p_cPos = p_cValue + iValueLen;
                break;
            }
        } else if (iValueType == JNUMBER) {
            if ((*p_cPos < '0' || *p_cPos > '9') && (*p_cPos != '.') && (*p_cPos != '+') \
                && (*p_cPos != '-') && ((*p_cPos != 'e')) && (*p_cPos != 'E')) {
                iValueLen = p_cPos - p_cValue;
                break;
            }
        } else if (iValueType == JSTRING) {
            if (*p_cPos == '\"') {
                iValueLen = p_cPos - p_cValue;
                break;
            }
        } else if (*p_cPos == JsonMark[iValueType][1]) {
            if (iStringDepth  == 0) {
                if (iMarkDepth == 0) {
                    iValueLen = p_cPos - p_cValue + 1;
                    p_cPos++;
                    break;
                } else {
                    iMarkDepth--;
                }
            }
        } else if (*p_cPos == JsonMark[iValueType][0]) {
            if (iStringDepth == 0) {
                iMarkDepth++;
            }
        } else if (*p_cPos == '\"') {
            if (iStringDepth) {
                iStringDepth = 0;
            } else {
                iStringDepth = 1;
            }
        }
        p_cPos++;
    }

    if (type == JOBJECT) {
        if ((p_cName + iNameLen) > str_end) {
            goto do_exit;
        }
        *key = p_cName;
        *key_len = iNameLen;
    }
    if ((p_cValue + iValueLen) > str_end) {
        goto do_exit;
    }

    *val = p_cValue;
    *val_len = iValueLen;
    *val_type = iValueType;
    if (iValueType == JSTRING) {
        return p_cValue + iValueLen + 1;
    } else {
        return p_cValue + iValueLen;
    }

do_exit:
    *val = NULL;
    *val_len = 0;
    *key = NULL;
    *key_len = 0;
    return NULL;
}

int json_parse_name_value(char *p_cJsonStr, int iStrLen, json_parse_cb pfnCB, void *p_CBData)
{
    char    *pos = 0, *key = 0, *val = 0;
    int     klen = 0, vlen = 0, vtype = 0;
    int     ret = JSON_RESULT_ERR;

    if (p_cJsonStr == NULL || iStrLen == 0 || pfnCB == NULL) {
        return ret;
    }

    json_object_for_each_kv(p_cJsonStr, iStrLen, pos, key, klen, val, vlen, vtype) {
        if (key && klen && val && vlen) {
            ret = JSON_RESULT_OK;
            if (JSON_PARSE_FINISH == pfnCB(key, klen, val, vlen, vtype, p_CBData)) {
                break;
            }
        }
    }

    return ret;
}

int json_get_value_by_name_cb(char *p_cName, int iNameLen, char *p_cValue, int iValueLen, int iValueType,
                              void *p_CBData)
{
    JSON_NV     *p_stNameValue = (JSON_NV *)p_CBData;

#ifdef JSON_DEBUG
    int         i;

    if (p_cName) {
        jparser_debug("Name:");
        for (i = 0; i < iNameLen; i++) {
            jparser_debug("%c", *(p_cName + i));
        }
    }

    if (p_cValue) {
        jparser_debug("Value:");
        for (i = 0; i < iValueLen; i++) {
            jparser_debug("%c", *(p_cValue + i));
        }
    }
#endif

    if ((iNameLen == p_stNameValue->nLen) && !strncmp(p_cName, p_stNameValue->pN, p_stNameValue->nLen)) {
        p_stNameValue->pV = p_cValue;
        p_stNameValue->vLen = iValueLen;
        p_stNameValue->vType = iValueType;
        return JSON_PARSE_FINISH;
    } else {
        return JSON_PARSE_OK;
    }
}

char *json_get_value_by_name(char *p_cJsonStr, int iStrLen, char *p_cName, int *p_iValueLen, int *p_iValueType)
{
    JSON_NV     stNV;

    memset(&stNV, 0, sizeof(stNV));
    stNV.pN = p_cName;
    stNV.nLen = strlen(p_cName);
    if (JSON_RESULT_OK == json_parse_name_value(p_cJsonStr, iStrLen, json_get_value_by_name_cb, (void *)&stNV)) {
        if (p_iValueLen) {
            *p_iValueLen = stNV.vLen;
        }
        if (p_iValueType) {
            *p_iValueType = stNV.vType;
        }
    }
    return stNV.pV;
}

char *json_get_value_by_name_len(char *p_cJsonStr, int iStrLen, char *p_cName, int p_cNameLen, int *p_iValueLen,
                                 int *p_iValueType)
{
    JSON_NV     stNV;

    memset(&stNV, 0, sizeof(stNV));
    stNV.pN = p_cName;
    stNV.nLen = p_cNameLen;
    if (JSON_RESULT_OK == json_parse_name_value(p_cJsonStr, iStrLen, json_get_value_by_name_cb, (void *)&stNV)) {
        if (p_iValueLen) {
            *p_iValueLen = stNV.vLen;
        }
        if (p_iValueType) {
            *p_iValueType = stNV.vType;
        }
    }
    return stNV.pV;
}

char *LITE_json_value_of(char *key, char *src, ...)
{
    char       *value = NULL;
    char       *ret = NULL;
    char       *delim = NULL;
    char       *key_iter;
    char       *key_next;
    char       *src_iter;

    int         key_len;
    int         value_len = -1;
    int         src_iter_len;

    if (NULL == key || NULL == src) {
        return NULL;
    }

#if WITH_MEM_STATS_PER_MODULE
{
    char       *module_name = NULL;
    int         magic = 0;
    va_list     ap;
    va_start(ap, src);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
}
#endif

    src_iter = src;
    src_iter_len = strlen(src_iter);
    key_iter = key;

    do {
        if ((delim = strchr(key_iter, '.')) != NULL) {
            key_len = delim - key_iter;
            key_next = key_iter;

            value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, key_len, &value_len, 0);
            if (value == NULL) {
                return NULL;
            }

            src_iter = value;
            src_iter_len = value_len;
            key_iter = delim + 1;
        }
    } while (delim);

    key_len = strlen(key_iter);
    key_next = key_iter;
    value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, key_len, &value_len, 0);
    if (NULL == value) {
        return NULL;
    }

    ret = jparser_malloc((value_len + 1) * sizeof(char));

    if (NULL == ret) {
        return NULL;
    }

    HAL_Snprintf(ret, value_len + 1, "%s", value);
    return ret;
}

#if WITH_JSON_KEYS_OF
static list_head_t *_LITE_json_keys_of(char *src, int src_len, char *prefix, ...)
{
    static          LIST_HEAD(keylist);
    char           *module_name = NULL;
    char           *iter_pre = NULL;
    char           *pos = 0, *key = 0, *val = 0;
    int             klen = 0, vlen = 0, vtype = 0;
    int             magic = 0;
    unsigned int    mlen = 0;

#if WITH_MEM_STATS_PER_MODULE
    va_list         ap;
    va_start(ap, prefix);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    if (!strcmp("", prefix)) {
        INIT_LIST_HEAD(&keylist);
    }

    json_object_for_each_kv(src, src_len, pos, key, klen, val, vlen, vtype) {
        if (key && klen && val && vlen) {

            json_key_t     *entry = NULL;

            entry = jparser_malloc(sizeof(json_key_t), magic, module_name);
            if (NULL == entry) {
                utils_err("jparser_malloc failed!");
                return NULL;
            }
            memset(entry, 0, sizeof(json_key_t));

            mlen = strlen(prefix) + klen + 1;
            if (module_name) {
                entry->key = LITE_format_nstring(mlen, "%s%.*s", magic, module_name, prefix, klen, key);
            } else {
                entry->key = LITE_format_nstring(mlen, "%s%.*s", prefix, klen, key);
            }
            if (NULL == entry->key) {
                jparser_free(entry);
                return NULL;
            }

            list_add_tail(&entry->list, &keylist);

            if (JOBJECT == vtype) {
                mlen = strlen(prefix) + klen + 2;
                if (module_name) {
                    iter_pre = LITE_format_nstring(mlen, "%s%.*s.", magic, module_name, prefix, klen, key);
                } else {
                    iter_pre = LITE_format_nstring(mlen, "%s%.*s.", prefix, klen, key);
                }
                if (NULL == iter_pre) {
                    return NULL;
                }

                _LITE_json_keys_of(val, vlen, iter_pre, magic, module_name);
                jparser_free(iter_pre);
            }
        }
    }

    if (!strcmp("", prefix)) {
        json_key_t     *entry = NULL;

        entry = jparser_malloc(sizeof(json_key_t), magic, module_name);
        if (NULL == entry) {
            utils_err("jparser_malloc failed!");
            return NULL;
        }
        memset(entry, 0, sizeof(json_key_t));
        list_add_tail(&entry->list, &keylist);

        return &keylist;
    }

    return NULL;
}

list_head_t *LITE_json_keys_of(char *src, char *prefix, ...)
{
    char *module_name = NULL;
    int magic = 0;

    if (!src || !prefix) {
        return NULL;
    }

#if WITH_MEM_STATS_PER_MODULE

    va_list ap;
    va_start(ap, prefix);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    return _LITE_json_keys_of(src, strlen(src), prefix, magic, module_name);

}

#if WITH_JSON_TOKEN_EXT
static list_head_t *_LITE_json_keys_of_ext(int type, char *src, int src_len, char *prefix, ...)
{
    static          LIST_HEAD(keylist);
    char           *module_name = NULL;
    char           *iter_pre = NULL;
    char           *pos = 0, *key = 0, *val = 0;
    int             klen = 0, vlen = 0, vtype = 0;
    int             magic = 0;
    unsigned int    count = 1;
    unsigned int    mlen = 0;

    if (src == NULL || prefix == NULL) {
        return NULL;
    }

#if WITH_MEM_STATS_PER_MODULE
    va_list         ap;
    va_start(ap, prefix);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    if (!strcmp("", prefix)) {
        INIT_LIST_HEAD(&keylist);
    }

    if (JOBJECT == type) {
        json_object_for_each_kv(src, src_len, pos, key, klen, val, vlen, vtype) {
            if (key && klen && val && vlen) {

                json_key_t     *entry = NULL;

                entry =  jparser_malloc(sizeof(json_key_t), magic, module_name);

                if (NULL == entry) {
                    utils_err("jparser_malloc failed!");
                    return NULL;
                }
                memset(entry, 0, sizeof(json_key_t));

                mlen = strlen(prefix) + klen + 1;
                if (module_name) {
                    entry->key = LITE_format_nstring(mlen, "%s%.*s", magic, module_name, prefix, klen, key);
                } else {
                    entry->key = LITE_format_nstring(mlen, "%s%.*s", prefix, klen, key);
                }

                if (NULL == entry->key) {
                    jparser_free(entry);
                    return NULL;
                }

                list_add_tail(&entry->list, &keylist);

                if (JOBJECT == vtype) {
                    mlen = strlen(prefix) + klen + 2;
                    if (module_name) {
                        iter_pre = LITE_format_nstring(mlen, "%s%.*s.", magic, module_name, prefix, klen, key);
                    } else {
                        iter_pre = LITE_format_nstring(mlen, "%s%.*s.", prefix, klen, key);
                    }
                    if (NULL == iter_pre) {
                        return NULL;
                    }

                    _LITE_json_keys_of_ext(vtype, val, vlen, iter_pre, magic, module_name);
                    jparser_free(iter_pre);
                } else if (JARRAY == vtype) {
                    mlen = strlen(prefix) + klen + 1;
                    if (module_name) {
                        iter_pre = LITE_format_nstring(mlen, "%s%.*s", magic, module_name, prefix, klen, key);
                    } else {
                        iter_pre = LITE_format_nstring(mlen, "%s%.*s", prefix, klen, key);
                    }
                    if (NULL == iter_pre) {
                        return NULL;
                    }

                    _LITE_json_keys_of_ext(vtype, val, vlen, iter_pre, magic, module_name);
                    jparser_free(iter_pre);
                }
            }
        }
    } else if (JARRAY == type) {
        json_array_for_each_entry(src, src_len, pos, val, vlen, vtype) {
            if (val && vlen) {

                json_key_t     *entry = NULL;
                unsigned int    tmp = 0;
                unsigned int    arridxlen = 0;
                entry = jparser_malloc(sizeof(json_key_t), magic, module_name);
                if (NULL == entry) {
                    utils_err("jparser_malloc failed!");
                    return NULL;
                }
                memset(entry, 0, sizeof(json_key_t));

                tmp = count;
                do {
                    tmp /= 10;
                    ++arridxlen;
                } while (tmp);
                mlen = strlen("%s[%d]") + strlen(prefix) + arridxlen;
                if (module_name) {
                    entry->key = LITE_format_nstring(mlen, "%s[%d]", magic, module_name, prefix, count);
                } else {
                    entry->key = LITE_format_nstring(mlen, "%s[%d]", prefix, count);
                }
                if (NULL == entry->key) {
                    jparser_free(entry);
                    return NULL;
                }

                list_add_tail(&entry->list, &keylist);

                if (JOBJECT == vtype) {
                    mlen = strlen("%s[%d].") + strlen(prefix) + arridxlen;
                    if (module_name) {
                        iter_pre = LITE_format_nstring(mlen, "%s[%d].", magic, module_name, prefix, count);
                    } else {
                        iter_pre = LITE_format_nstring(mlen, "%s[%d].", prefix, count);
                    }
                    if (NULL == iter_pre) {
                        return NULL;
                    }

                    _LITE_json_keys_of_ext(vtype, val, vlen, iter_pre, magic, module_name);
                    jparser_free(iter_pre);
                } else if (JARRAY == vtype) {
                    mlen = strlen("%s[%d]") + strlen(prefix) + arridxlen;
                    if (module_name) {
                        iter_pre = LITE_format_nstring(mlen, "%s[%d]", magic, module_name, prefix, count);
                    } else {
                        iter_pre = LITE_format_nstring(mlen, "%s[%d]", prefix, count);
                    }
                    if (NULL == iter_pre) {
                        return NULL;
                    }

                    _LITE_json_keys_of_ext(vtype, val, vlen, iter_pre, magic, module_name);
                    jparser_free(iter_pre);
                }
                ++count;
            }
        }
    }

    if (!strcmp("", prefix)) {
        json_key_t     *entry = NULL;

        entry = jparser_malloc(sizeof(json_key_t), magic, module_name);
        if (NULL == entry) {
            utils_err("jparser_malloc failed!");
            return NULL;
        }
        memset(entry, 0, sizeof(json_key_t));
        list_add_tail(&entry->list, &keylist);

        return &keylist;
    }

    return NULL;

}

int contain_arr(const char *src, int src_len, const char **arr_pre)
{
    int         i = 0;
    int         ret = -1;
    int         deep = 0;
    const char *pre = NULL;

    if (NULL == src || NULL == arr_pre || src_len <= 0) {
        return -1;
    }

    *arr_pre = NULL;
    for (i = 0; i < src_len; ++i) {
        switch (src[i]) {
            case '[': {
                if (deep != 0) {
                    return ret;
                }
                ++deep;
                if (!pre) {
                    pre = &src[i];
                }
            }
            break;
            case ']': {
                if (deep != 1) {
                    return ret;
                }
                --deep;
                if ('[' == src[i - 1]) {
                    return ret;
                }
            }
            break;
            default: {
                if ((pre != NULL) && (0 == deep)) {
                    return ret;
                }
            }
            break;
        }
    }
    if ((NULL != pre) && (pre < src + src_len) && (pre >= src)) {
        *arr_pre = pre;
        ret = 0;
    }
    return ret;
}

static char *_json_value_by_arrname(char *src, int src_len, const char *key, int key_len, int *val_len)
{
    char       *pos = src;
    char       *entry = NULL;
    const char *p = NULL;
    const char *arr_pre = key;
    const char *arr_suf = NULL;
    int         vtype;
    int         loop;
    int         loop_tmp = 0;
    do {
        loop = 0;

        arr_pre = strchr(arr_pre, '[');
        if (arr_pre && (arr_pre <  key + key_len)) {
            arr_suf = strchr(arr_pre, ']');
        }
        if (arr_pre && arr_suf && (arr_suf < key + key_len)) {
            loop_tmp = 0;
            for (p = arr_pre + 1; p < arr_suf; ++p) {
                if (*p > '9' || *p < '0') {
                    return NULL;
                }
                loop_tmp *= 10;
                loop_tmp += *p - '0';

            }

            pos = json_get_object(JARRAY, pos, src + src_len);
            if (pos != 0 && *pos != 0) {
                if (*pos == '[' && *(pos + 1) == ']') {
                    return NULL;
                }
            }

            json_array_for_each_entry(src, src_len, pos, entry, *val_len, vtype) {
                if (entry && *val_len) {
                    if (++loop >= loop_tmp) {
                        break;
                    }
                }
            }

            if (loop != loop_tmp) {
                return NULL;
            }
            src = entry;
            arr_pre = arr_suf + 1;
        }
    } while (arr_pre && arr_suf && (arr_pre < key + key_len));
    return entry;
}

void LITE_json_keys_release(list_head_t *keylist)
{
    json_key_t         *pos, *tmp;

    if (NULL == keylist) {
        return;
    }

    list_for_each_entry_safe(pos, tmp, keylist, list, json_key_t) {
        if (pos->key) {
            jparser_free(pos->key);
        }
        list_del(&pos->list);
        jparser_free(pos);
    }
}

list_head_t *LITE_json_keys_of_ext(char *src, char *prefix, ...)
{
    char *module_name = NULL;
    int magic = 0;

#if WITH_MEM_STATS_PER_MODULE

    va_list ap;
    va_start(ap, prefix);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);
#endif

    if (!src || !prefix) {
        return NULL;
    }

    return _LITE_json_keys_of_ext(JOBJECT, src, strlen(src), prefix, magic, module_name);
}

static char *_LITE_json_value_of_ext(char *key, char *src, int src_len, int *val_len)
{
    char       *value = NULL;
    char       *delim = NULL;
    char       *key_iter;
    char       *key_next;
    char       *src_iter;
    const char *arr_pre = NULL;

    int         value_len;
    int         key_len;
    int         obj_key_len = 0;
    int         key_type;
    int         src_iter_len;

    src_iter = src;
    src_iter_len = src_len;
    key_iter = key;
    value_len = src_iter_len;
    do {
        if ((delim = strchr(key_iter, '.')) != NULL) {
            key_len = delim - key_iter;
            key_type = JOBJECT;
            key_next = key_iter;
            if (0 == contain_arr(key_next, key_len, &arr_pre)) {
                key_type = JARRAY;
                obj_key_len = arr_pre - key_iter;
                if (obj_key_len) {
                    value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, obj_key_len, &value_len, 0);
                } else {
                    value = src_iter;
                }
            } else {
                value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, key_len, &value_len, 0);
            }

            if (NULL == value) {
                return NULL;
            }

            if (key_type == JARRAY) {
                if (NULL == (value = _json_value_by_arrname(value, value_len, arr_pre, key_len - obj_key_len, &value_len))) {
                    return NULL;
                }
            }
            src_iter = value;
            src_iter_len = value_len;
            key_iter = delim + 1;

        }
    } while (delim);

    key_len = strlen(key_iter);
    key_next = key_iter;
    key_type = JOBJECT;
    if (0 == contain_arr(key_next, key_len, &arr_pre)) {
        key_type = JARRAY;
        obj_key_len = arr_pre - key_iter;
        if (obj_key_len) {
            value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, obj_key_len, &value_len, 0);
        } else {
            value = src_iter;
        }
    } else {
        value = json_get_value_by_name_len(src_iter, src_iter_len, key_next, key_len, &value_len, 0);
    }

    if (NULL == value) {
        return NULL;
    }
    if (key_type == JARRAY) {
        if (NULL == (value = _json_value_by_arrname(value, value_len, arr_pre, key_len - obj_key_len, &value_len))) {
            return NULL;
        }
    }
    *val_len = value_len;
    return value;
}

#endif  /* #if WITH_JSON_TOKEN_EXT */
#endif  /* #if WITH_JSON_KEYS_OF */
#endif

