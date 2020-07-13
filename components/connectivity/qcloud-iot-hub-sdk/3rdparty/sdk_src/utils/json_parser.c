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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lite-utils.h"
#include "qcloud_iot_export_log.h"

#define json_debug Log_d

typedef struct JSON_NV {
    int   nLen;
    int   vLen;
    int   vType;
    char *pN;
    char *pV;
} JSON_NV;

char *json_get_object(int type, char *str)
{
    char *pos = 0;
    char  ch  = (type == JSOBJECT) ? '{' : '[';
    while (str != 0 && *str != 0) {
        if (*str == ' ') {
            str++;
            continue;
        }
        pos = (*str == ch) ? str : 0;
        break;
    }
    return pos;
}

char *json_get_next_object(int type, char *str, char **key, int *key_len, char **val, int *val_len, int *val_type)
{
    char  JsonMark[JSTYPEMAX][2] = {{'\"', '\"'}, {'{', '}'}, {'[', ']'}, {'0', ' '}};
    int   iMarkDepth = 0, iValueType = JSNONE, iNameLen = 0, iValueLen = 0;
    char *p_cName = 0, *p_cValue = 0, *p_cPos = str;
    char  lastchr = ' ';

    if (type == JSOBJECT) {
        /* Get Key */
        p_cPos = strchr(p_cPos, '"');
        if (!p_cPos) {
            return 0;
        }
        p_cName = ++p_cPos;
        p_cPos  = strchr(p_cPos, '"');
        if (!p_cPos) {
            return 0;
        }
        iNameLen = p_cPos - p_cName;

        /* Get Value */
        p_cPos = strchr(p_cPos, ':');
    }
    while (p_cPos && *p_cPos) {
        if (*p_cPos == '"') {
            iValueType = JSSTRING;
            lastchr    = *p_cPos;
            p_cValue   = ++p_cPos;
            break;
        } else if (*p_cPos == '{') {
            iValueType = JSOBJECT;
            p_cValue   = p_cPos++;
            break;
        } else if (*p_cPos == '[') {
            iValueType = JSARRAY;
            p_cValue   = p_cPos++;
            break;
        } else if ((*p_cPos == '-') || (*p_cPos >= '0' && *p_cPos <= '9')) {
            iValueType = JSNUMBER;
            p_cValue   = p_cPos++;
            break;
        } else if (*p_cPos == 't' || *p_cPos == 'T' || *p_cPos == 'f' || *p_cPos == 'F') {
            iValueType = JSBOOLEAN;
            p_cValue   = p_cPos;
            break;
        } else if (*p_cPos == 'n' || *p_cPos == 'N') {
            iValueType = JSNULL;
            p_cValue   = p_cPos;
            break;
        }
        p_cPos++;
    }
    while (p_cPos && *p_cPos && iValueType > JSNONE) {
        if (iValueType == JSBOOLEAN) {
            int len = strlen(p_cValue);

            if ((*p_cValue == 't' || *p_cValue == 'T') && len >= 4 &&
                (!strncmp(p_cValue, "true", 4) || !strncmp(p_cValue, "TRUE", 4))) {
                iValueLen = 4;
                // p_cPos = p_cValue + iValueLen;
                break;
            } else if ((*p_cValue == 'f' || *p_cValue == 'F') && len >= 5 &&
                       (!strncmp(p_cValue, "false", 5) || !strncmp(p_cValue, "FALSE", 5))) {
                iValueLen = 5;
                // p_cPos = p_cValue + iValueLen;
                break;
            }
        } else if (iValueType == JSNULL) {  // support null/NULL
            int nlen = strlen(p_cValue);

            if ((*p_cValue == 'n' || *p_cValue == 'N') && nlen >= 4 &&
                (!strncmp(p_cValue, "null", 4) || !strncmp(p_cValue, "NULL", 4))) {
                iValueLen = 4;
                // p_cPos = p_cValue + iValueLen;
                break;
            }
        } else if (iValueType == JSNUMBER) {
            // if (*p_cPos < '0' || *p_cPos > '9') {
            if ((*p_cPos < '0' || *p_cPos > '9') && (*p_cPos != '.')) {  // support float
                iValueLen = p_cPos - p_cValue;
                break;
            }
        } else if (*p_cPos == JsonMark[iValueType][1]) {
            if (iMarkDepth == 0) {
                iValueLen = p_cPos - p_cValue + (iValueType == JSSTRING ? 0 : 1);
                p_cPos++;
                if ((iValueType == JSSTRING) && (lastchr == '\\')) {
                    lastchr = *p_cPos;
                    continue;
                } else {
                    break;
                }
            } else {
                iMarkDepth--;
            }
        } else if (*p_cPos == JsonMark[iValueType][0]) {
            iMarkDepth++;
        }
        lastchr = *p_cPos;
        p_cPos++;
    }

    if (type == JSOBJECT) {
        *key     = p_cName;
        *key_len = iNameLen;
    }

    *val      = p_cValue;
    *val_len  = iValueLen;
    *val_type = iValueType;
    if (iValueType == JSSTRING) {
        return p_cValue + iValueLen + 1;
    } else {
        return p_cValue + iValueLen;
    }
}

int json_parse_name_value(char *p_cJsonStr, int iStrLen, json_parse_cb pfnCB, void *p_CBData)
{
    char *pos = 0, *key = 0, *val = 0;
    int   klen = 0, vlen = 0, vtype = 0;
    char  last_char = 0;
    int   ret       = JSON_RESULT_ERR;

    if (p_cJsonStr == NULL || iStrLen == 0 || pfnCB == NULL) {
        return ret;
    }

    if (iStrLen != strlen(p_cJsonStr)) {
        Log_w("Backup last_char since %d != %d", iStrLen, (int)strlen(p_cJsonStr));
        backup_json_str_last_char(p_cJsonStr, iStrLen, last_char);
    }

    json_object_for_each_kv(p_cJsonStr, pos, key, klen, val, vlen, vtype)
    {
        if (key && klen && val && vlen) {
            ret = JSON_RESULT_OK;
            if (JSON_PARSE_FINISH == pfnCB(key, klen, val, vlen, vtype, p_CBData)) {
                break;
            }
        }
    }

    if (iStrLen != strlen(p_cJsonStr)) {
        restore_json_str_last_char(p_cJsonStr, iStrLen, last_char);
    }

    return ret;
}

int json_get_value_by_name_cb(char *p_cName, int iNameLen, char *p_cValue, int iValueLen, int iValueType,
                              void *p_CBData)
{
    JSON_NV *p_stNameValue = (JSON_NV *)p_CBData;

#if (JSON_DEBUG == 1)
    int i;

    if (p_cName) {
        json_debug("Name:");
        for (i = 0; i < iNameLen; i++) {
            json_debug("%c", *(p_cName + i));
        }
    }

    if (p_cValue) {
        json_debug("Value:");
        for (i = 0; i < iValueLen; i++) {
            json_debug("%c", *(p_cValue + i));
        }
    }
#endif

    if ((iNameLen == p_stNameValue->nLen) && !strncmp(p_cName, p_stNameValue->pN, p_stNameValue->nLen)) {
        p_stNameValue->pV    = p_cValue;
        p_stNameValue->vLen  = iValueLen;
        p_stNameValue->vType = iValueType;
        return JSON_PARSE_FINISH;
    } else {
        return JSON_PARSE_OK;
    }
}

char *json_get_value_by_name(char *p_cJsonStr, int iStrLen, char *p_cName, int *p_iValueLen, int *p_iValueType)
{
    JSON_NV stNV;

    memset(&stNV, 0, sizeof(stNV));
    stNV.pN   = p_cName;
    stNV.nLen = strlen(p_cName);
    if (JSON_RESULT_OK == json_parse_name_value(p_cJsonStr, iStrLen, json_get_value_by_name_cb, (void *)&stNV)) {
        if (p_iValueLen) {
            *p_iValueLen = stNV.vLen;
        }
        if (p_iValueType) {
            *p_iValueType = stNV.vType;
            if (JSNULL == stNV.vType) {
                stNV.pV = NULL;
            }
        }
    }
    return stNV.pV;
}
