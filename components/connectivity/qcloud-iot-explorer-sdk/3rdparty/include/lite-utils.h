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
/**
 * Edit by shockcao@tencent.com 2018/3/15
 */

#ifndef __LITE_UTILS_H__
#define __LITE_UTILS_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#if defined(_PLATFORM_IS_LINUX_)
#include <assert.h>
#endif

#include "lite-list.h"
#include "qcloud_iot_import.h"

#define LITE_TRUE                   (1)
#define LITE_FALSE                  (0)

#ifndef container_of
#define container_of(ptr, type, member)  \
    ((type *) ((char *) (ptr) - offsetof(type, member)))
#endif

#define LITE_MINIMUM(a, b)          (((a) <= (b)) ? (a) : (b))
#define LITE_MAXIMUM(a, b)          (((a) >= (b)) ? (a) : (b))
#define LITE_isdigit(c)             (((c) <= '9' && (c) >= '0') ? (LITE_TRUE) : (LITE_FALSE))

#if defined(_PLATFORM_IS_LINUX_)
#define LITE_ASSERT(expr)           assert(expr)
#else
#define LITE_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            HAL_Printf("### %s | %s(%d): ASSERT FAILED ###: %s is FALSE\r\n", \
                        __FILE__, __func__, __LINE__, #expr); \
        } \
    } while(0)
#endif

char       	   *LITE_strdup(const char *src);
char       	   *LITE_format_string(const char *fmt, ...);
char       	   *LITE_format_nstring(const int len, const char *fmt, ...);
void        	LITE_hexbuf_convert(unsigned char *digest, char *out, int buflen, int uppercase);
void        	LITE_hexstr_convert(char *hexstr, uint8_t *out_buf, int len);
void        	LITE_replace_substr(char orig[], char key[], char swap[]);

char           *LITE_json_value_of(char *key, char *src);
list_head_t    *LITE_json_keys_of(char *src, char *prefix);
void            LITE_json_keys_release(list_head_t *keylist);
char 		   *LITE_json_string_value_strip_transfer(char *key, char *src);


int 			LITE_get_int32(int32_t *value, char *src);
int 			LITE_get_int16(int16_t *value, char *src);
int 			LITE_get_int8(int8_t *value, char *src);
int 			LITE_get_uint32(uint32_t *value, char *src);
int 			LITE_get_uint16(uint16_t *value, char *src);
int 			LITE_get_uint8(uint8_t *value, char *src);
int 			LITE_get_float(float *value, char *src);
int 			LITE_get_double(double *value, char *src);
int 			LITE_get_boolean(bool *value, char *src);
int 			LITE_get_string(int8_t        *value, char *src, uint16_t max_len);


typedef struct _json_key_t {
    char           *key;
    list_head_t     list;
} json_key_t;

#define foreach_json_keys_in(src, iter_key, keylist, pos)   \
    for(keylist = (void *)LITE_json_keys_of((char *)src, ""), \
        pos = (void *)list_first_entry((list_head_t *)keylist, json_key_t, list), \
        iter_key = ((json_key_t *)pos)->key; \
            (iter_key = ((json_key_t *)pos)->key); \
                pos = list_next_entry((json_key_t *)pos, list, json_key_t))

#endif  /* __LITE_UTILS_H__ */
