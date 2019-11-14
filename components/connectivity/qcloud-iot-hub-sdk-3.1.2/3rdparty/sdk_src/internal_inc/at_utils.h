/*
* Copyright (c) 2019-2021 Tencent Group. All rights reserved.
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

#ifndef _AT_UTILS_H_
#define _AT_UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define WIDTH_SIZE           32

#ifndef __INT_MAX__
#define __INT_MAX__     2147483647
#endif

#ifndef INT_MAX
#define INT_MAX         (__INT_MAX__)
#endif

#define AT_CMD_COMMA_MARK              ','
#define AT_CMD_DQUOTES_MARK            '"'

#define __is_print(ch)       ((unsigned int)((ch) - ' ') < 127u - ' ')

int at_vprintfln(const char *format, va_list args);
void at_print_raw_cmd(const char *name, const char *cmd, int size);
const char *at_get_last_cmd(int *cmd_size);
int at_req_parse_args(const char *req_args, const char *req_expr, ...);
int at_sscanf(const char * buf, const char * fmt, va_list args);
void at_strip(char *str, const char patten);
void chr_strip(char *str, const char patten);

#endif
