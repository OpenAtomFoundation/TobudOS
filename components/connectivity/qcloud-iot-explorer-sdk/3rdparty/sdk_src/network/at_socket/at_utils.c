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

#include <ctype.h>
#include <string.h>

#include "qcloud_iot_import.h"
#include "utils_param_check.h"
#include "at_utils.h"
#include "at_client.h"

static char send_buf[CLINET_BUFF_LEN];
static int  last_cmd_len = 0;

/**
 * dump hex format data to console device
 *
 * @param name name for hex object, it will show on log header
 * @param buf hex buffer
 * @param size buffer size
 */
void at_print_raw_cmd(const char *name, const char *buf, int size)
{
    int i, j;

    for (i = 0; i < size; i += WIDTH_SIZE)
    {
        HAL_Printf("%s: %04X-%04X: ", name, i, i + WIDTH_SIZE);
        for (j = 0; j < WIDTH_SIZE; j++)
        {
            if (i + j < size)
            {
                HAL_Printf("%02X ", buf[i + j]);
            }
            else
            {
                HAL_Printf("   ");
            }
            if ((j + 1) % 8 == 0)
            {
                HAL_Printf(" ");
            }
        }
        HAL_Printf("  ");
        for (j = 0; j < WIDTH_SIZE; j++)
        {
            if (i + j < size)
            {
                HAL_Printf("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
            }
        }
        HAL_Printf("\n\r");
    }
}

const char *at_get_last_cmd(int *cmd_size)
{
    *cmd_size = last_cmd_len;
    return send_buf;
}

int at_vprintf(const char *format, va_list args)
{
    last_cmd_len = vsnprintf(send_buf, sizeof(send_buf), format, args);

#ifdef AT_DEBUG
    at_print_raw_cmd("send", send_buf, last_cmd_len);
#endif

    return HAL_AT_Uart_Send((uint8_t *)send_buf, last_cmd_len);
}

int at_vprintfln(const char *format, va_list args)
{
    int len;

    len = at_vprintf(format, args);

    HAL_AT_Uart_Send("\r\n", 2);

    return len + 2;
}

/**
 * at_sscanf - Unformat a buffer into a list of arguments, rewrite sscanf
 * @buf:	input buffer
 * @fmt:	format of buffer
 * @args:	arguments
 */
int at_sscanf(const char * buf, const char * fmt, va_list args)
{
	const char *str = buf;
	char *next;
	int num = 0;
	int qualifier;
	int base;
	int field_width = -1;
	int is_sign = 0;

	while(*fmt && *str) {
		/* skip any white space in format */
		/* white space in format matchs any amount of
		 * white space, including none, in the input.
		 */
		if (isspace(*fmt)) {
			while (isspace(*fmt))
				++fmt;
			while (isspace(*str))
				++str;
		}

		/* anything that is not a conversion must match exactly */
		if (*fmt != '%' && *fmt) {
			if (*fmt++ != *str++)
				break;
			continue;
		}

		if (!*fmt)
			break;
		++fmt;
		
		/* skip this conversion.
		 * advance both strings to next white space
		 */
		if (*fmt == '*') {
			while (!isspace(*fmt) && *fmt)
				fmt++;
			while (!isspace(*str) && *str)
				str++;
			continue;
		}

		/* get field width */
		if (isdigit(*fmt))
			field_width = atoi(fmt);

		/* get conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'Z') {
			qualifier = *fmt;
			fmt++;
		}
		base = 10;
		is_sign = 0;

		if (!*fmt || !*str)
			break;

		switch(*fmt++) {
		case 'c':
		{
			char *s = (char *) va_arg(args,char*);
			if (field_width == -1)
				field_width = 1;
			do {
				*s++ = *str++;
			} while(field_width-- > 0 && *str);
			num++;
		}
		continue;
		case 's':
		{
			char *s = (char *) va_arg(args, char *);
			if(field_width == -1)
				field_width = INT_MAX;
			/* first, skip leading white space in buffer */
			while (isspace(*str))
				str++;

			/* now copy until next white space */
			while (*str && ((*str) != ',')) {
				if(isspace(*str)){
					str++;
				}else{
					*s++ = *str++;
				}			
			}
			*s = '\0';
			num++;
		}
		continue;
		/* S for special handling for MQTTPUB JSON content */
		case 'S':
		{
			char *s = (char *) va_arg(args, char *);
			if(field_width == -1)
				field_width = INT_MAX;
			/* first, skip leading white space in buffer */
			while (isspace(*str))
				str++;

			/* now copy until next white space */
			while (*str) {
				if(isspace(*str)){
					str++;
				}else{
					*s++ = *str++;
				}			
			}
			*s = '\0';
			num++;
		}
		continue;
		case 'n':
			/* return number of characters read so far */
		{
			int *i = (int *)va_arg(args,int*);
			*i = str - buf;
		}
		continue;
		case 'o':
			base = 8;
			break;
		case 'x':
		case 'X':
			base = 16;
			break;
		case 'd':
		case 'i':
			is_sign = 1;
		case 'u':
			break;
		case '%':
			/* looking for '%' in str */
			if (*str++ != '%') 
				return num;
			continue;
		default:
			/* invalid format; stop here */
			return num;
		}

		/* have some sort of integer conversion.
		 * first, skip white space in buffer.
		 */
		while (isspace(*str))
			str++;

		if (!*str || !isdigit(*str))
			break;

		switch(qualifier) {
		case 'h':
			if (is_sign) {
				short *s = (short *) va_arg(args,short *);
				*s = (short) strtol(str,&next,base);
			} else {
				unsigned short *s = (unsigned short *) va_arg(args, unsigned short *);
				*s = (unsigned short) strtoul(str, &next, base);
			}
			break;
		case 'l':
			if (is_sign) {
				long *l = (long *) va_arg(args,long *);
				*l = strtol(str,&next,base);
			} else {
				unsigned long *l = (unsigned long*) va_arg(args,unsigned long*);
				*l = strtoul(str,&next,base);
			}
			break;
		case 'L':
			if (is_sign) {
				long long *l = (long long*) va_arg(args,long long *);
				*l = strtoll(str,&next,base);
			} else {
				unsigned long long *l = (unsigned long long*) va_arg(args,unsigned long long*);
				*l = strtoull(str,&next,base);
			}
			break;
		case 'Z':
		{
			unsigned long *s = (unsigned long*) va_arg(args,unsigned long*);
			*s = (unsigned long) strtoul(str,&next,base);
		}
		break;
		default:
			if (is_sign) {
				int *i = (int *) va_arg(args, int*);
				*i = (int) strtol(str,&next,base);
			} else {
				unsigned int *i = (unsigned int*) va_arg(args, unsigned int*);
				*i = (unsigned int) strtoul(str,&next,base);
			}
			break;
		}
		num++;

		if (!next)
			break;
		str = next;
	}
	return num;
}

/**
 * AT server request arguments parse arguments
 *
 * @param req_args request arguments
 * @param req_expr request expression
 *
 * @return  -1 : parse arguments failed
 *           0 : parse without match
 *          >0 : The number of arguments successfully parsed  
 */
int at_req_parse_args(const char *req_args, const char *req_expr, ...)
{
    va_list args;
    int req_args_num = 0;

    POINTER_SANITY_CHECK(req_args, 0);
    POINTER_SANITY_CHECK(req_expr, 0);

    va_start(args, req_expr);

    //req_args_num = vsscanf(req_args, req_expr, args);	
    req_args_num = at_sscanf(req_args, req_expr, args);
    

    va_end(args);

    return req_args_num;
}

void at_strip(char *str, const char patten)
{
	char *start, *end;
	
	start = str;			
	end = str + strlen(str) -1;	
	
	if(*str == patten)
	{
		start++;
	}

	if(*end == patten)
	{
		*end-- = '\0';	
	}

	strcpy(str, start);
}

void chr_strip(char *str, const char patten)
{
	char *end = str + strlen(str);
	
 	while(*str != '\0')
 	{
		if(*str == patten)
		{
			memmove(str, str+1, end - str);
		}
		str++;
	}
}
