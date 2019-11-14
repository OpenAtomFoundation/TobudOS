/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "utils_param_check.h"
#include "utils_timer.h"
#include "at_client.h"
#include "at_utils.h"

#define AT_RESP_END_OK                 "OK"
#define AT_RESP_END_eLOG_ERROR              "eLOG_ERROR"
#define AT_RESP_END_FAIL               "FAIL"
#define AT_END_CR_LF                   "\r\n"

sRingbuff g_ring_buff;	
static at_client sg_at_client;
static uint32_t sg_flags = 0;

/*this function can be called only by at_uart_isr, just push the data to the at_client ringbuffer.*/
void at_client_uart_rx_isr_cb(uint8_t *pdata, uint8_t len)
{
	(void)ring_buff_push_data(&g_ring_buff, pdata, len);
}

/**
 * Create response object.
 *
 * @param buf_size the maximum response buffer size
 * @param line_num the number of setting response lines
 *		   = 0: the response data will auto return when received 'OK' or 'eLOG_ERROR'
 *		  != 0: the response data will return when received setting lines number data
 * @param timeout the maximum response time
 *
 * @return != NULL: response object
 *			= NULL: no memory
 */
at_response_t at_create_resp(uint32_t buf_size, uint32_t line_num, uint32_t timeout)
{
	at_response_t resp = NULL;

	resp = (at_response_t) HAL_Malloc(sizeof(at_response));
	if (resp == NULL)
	{
		Log_e("AT create response object failed! No memory for response object!");
		return NULL;
	}

	resp->buf = (char *) HAL_Malloc(buf_size);
	if (resp->buf == NULL)
	{
		Log_e("AT create response object failed! No memory for response buffer!");
		HAL_Free(resp);
		return NULL;
	}

	resp->buf_size = buf_size;
	resp->line_num = line_num;
	resp->line_counts = 0;
	resp->timeout = timeout;

	return resp;
}

/**
 * Delete and free response object.
 *
 * @param resp response object
 */
void at_delete_resp(at_response_t resp)
{
    if (resp && resp->buf)
    {
        HAL_Free(resp->buf);
    }

    if (resp)
    {
        HAL_Free(resp);
        resp = NULL;
    }
}

void at_delayms(uint32_t delayms)
{
#ifdef AT_OS_USED
	HAL_SleepMs(delayms);
#else
	HAL_DelayMs(delayms);
#endif

}

void at_setFlag(uint32_t flag)
{
	sg_flags |= flag&0xffffffff;
}

void at_clearFlag(uint32_t flag)
{
	sg_flags &= (~flag)&0xffffffff;
}

uint32_t at_getFlag(void)
{
	return sg_flags;
}

bool at_waitFlag(uint32_t flag, uint32_t timeout)
{
	Timer timer;
	bool Ret = false;
	
	countdown_ms(&timer, timeout);	
	do{
		if(flag == (at_getFlag()&flag)){
			Ret = true;
			break;
		}
		at_delayms(1);
	}while(!expired(&timer));

	return Ret;
}

/**
 * Get one line AT response buffer by line number.
 *
 * @param resp response object
 * @param resp_line line number, start from '1'
 *
 * @return != NULL: response line buffer
 *			= NULL: input response line error
 */
const char *at_resp_get_line(at_response_t resp, uint32_t resp_line)
{
	char *resp_buf = resp->buf;
	char *resp_line_buf = NULL;
	int line_num = 1;

	POINTER_SANITY_CHECK(resp, NULL);

	if (resp_line > resp->line_counts || resp_line <= 0)
	{
		Log_e("AT response get line failed! Input response line(%d) error!", resp_line);
		return NULL;
	}

	for (line_num = 1; line_num <= resp->line_counts; line_num++)
	{
		if (resp_line == line_num)
		{
			resp_line_buf = resp_buf;

			return resp_line_buf;
		}

		resp_buf += strlen(resp_buf) + 1;
	}

	return NULL;
}

/**
 * Get one line AT response buffer by keyword
 *
 * @param resp response object
 * @param keyword query keyword
 *
 * @return != NULL: response line buffer
 *			= NULL: no matching data
 */
const char *at_resp_get_line_by_kw(at_response_t resp, const char *keyword)
{
	char *resp_buf = resp->buf;
	char *resp_line_buf = NULL;
	int line_num = 1;

	POINTER_SANITY_CHECK(resp, NULL);
	POINTER_SANITY_CHECK(keyword, NULL);

	for (line_num = 1; line_num <= resp->line_counts; line_num++)
	{
		if (strstr(resp_buf, keyword))
		{
			resp_line_buf = resp_buf;

			return resp_line_buf;
		}

		resp_buf += strlen(resp_buf) + 1;
	}

	return NULL;
}

/**
 * Get and parse AT response buffer arguments by line number.
 *
 * @param resp response object
 * @param resp_line line number, start from '1'
 * @param resp_expr response buffer expression
 *
 * @return -1 : input response line number error or get line buffer error
 *			0 : parsed without match
 *		   >0 : the number of arguments successfully parsed
 */
int at_resp_parse_line_args(at_response_t resp, uint32_t resp_line, const char *resp_expr, ...)
{
	va_list args;
	int resp_args_num = 0;
	const char *resp_line_buf = NULL;


	POINTER_SANITY_CHECK(resp, -1);
	POINTER_SANITY_CHECK(resp_expr, -1);
	if ((resp_line_buf = at_resp_get_line(resp, resp_line)) == NULL)
	{
		return -1;
	}

	va_start(args, resp_expr);

	resp_args_num = vsscanf(resp_line_buf, resp_expr, args);

	va_end(args);

	return resp_args_num;
}

/**
 * Get and parse AT response buffer arguments by keyword.
 *
 * @param resp response object
 * @param keyword query keyword
 * @param resp_expr response buffer expression
 *
 * @return -1 : input keyword error or get line buffer error
 *			0 : parsed without match
 *		   >0 : the number of arguments successfully parsed
 */
int at_resp_parse_line_args_by_kw(at_response_t resp, const char *keyword, const char *resp_expr, ...)
{
	va_list args;
	int resp_args_num = 0;
	const char *resp_line_buf = NULL;


	POINTER_SANITY_CHECK(resp, -1); 
	POINTER_SANITY_CHECK(resp_expr, -1); 
	if ((resp_line_buf = at_resp_get_line_by_kw(resp, keyword)) == NULL)
	{
		return -1;
	}

	va_start(args, resp_expr);

	resp_args_num = vsscanf(resp_line_buf, resp_expr, args);

	va_end(args);

	return resp_args_num;
}

/**
 * Send commands to AT server and wait response.
 *
 * @param client current AT client object
 * @param resp AT response object, using NULL when you don't care response
 * @param cmd_expr AT commands expression
 *
 * @return 0 : success
 *		  -1 : response status error
 *		  -2 : wait timeout
 */
int at_obj_exec_cmd(at_response_t resp, const char *cmd_expr, ...)
{
	POINTER_SANITY_CHECK(cmd_expr, QCLOUD_ERR_INVAL);

	va_list args;
	int cmd_size = 0;
	int result = QCLOUD_RET_SUCCESS;
	const char *cmd = NULL;
	at_client_t client = at_client_get();
	

	if (client == NULL)
	{
		Log_e("input AT Client object is NULL, please create or get AT Client object!");
		return QCLOUD_ERR_FAILURE;
	}

	HAL_MutexLock(client->lock);
	resp->line_counts = 0;
	client->resp_status = AT_RESP_OK;
	client->resp = resp;

	va_start(args, cmd_expr);
	at_vprintfln(cmd_expr, args);
	va_end(args);

	if (resp != NULL)
	{
#ifndef AT_OS_USED
		client->resp_status = AT_RESP_TIMEOUT;
		at_client_yeild(NULL, resp->timeout);	
		if (client->resp_status != AT_RESP_OK)
		{
			cmd = at_get_last_cmd(&cmd_size);
			Log_e("execute command (%.*s) failed!", cmd_size, cmd);
			result = QCLOUD_ERR_FAILURE;
			goto __exit;
		}
#else
		if(HAL_SemaphoreWait(client->resp_sem, resp->timeout) < 0){
			cmd = at_get_last_cmd(&cmd_size);
			Log_e("execute command (%.*s) failed!", cmd_size, cmd);
			result = QCLOUD_ERR_FAILURE;
			goto __exit;
		}

		if (client->resp_status != AT_RESP_OK)
		{
			cmd = at_get_last_cmd(&cmd_size);
			Log_e("execute command (%.*s) failed!", cmd_size, cmd);
			result = QCLOUD_ERR_FAILURE;
			goto __exit;
		}
#endif	
	}

__exit:
	client->resp = NULL;
	HAL_MutexUnlock(client->lock);
	
	return result;
}

/**
 * Send data to AT server, send data don't have end sign(eg: \r\n).
 *
 * @param client current AT client object
 * @param buf		send data buffer
 * @param size		send fixed data size
 * @param timeout	timeout for send
 *
 * @return >0: send data size
 *		   =0: send failed
 */
int at_client_send(at_client_t client, const char *buf, int size, uint32_t timeout)
{
	POINTER_SANITY_CHECK(buf, 0);

	if (client == NULL)
	{
		Log_e("input AT Client object is NULL, please create or get AT Client object!");
		return 0;
	}

#ifdef AT_DEBUG
	at_print_raw_cmd("send", buf, size);
#endif

	return HAL_AT_Uart_Send((void *)buf, size);
}


static int at_client_getchar(at_client_t client, char *pch, uint32_t timeout)
{
	int ret = QCLOUD_RET_SUCCESS;
	Timer timer;

	countdown_ms(&timer, timeout);
	do   
    {

#ifndef AT_UART_RECV_IRQ  
    	if(0 == HAL_AT_Uart_Recv((void *)pch, 1, NULL, timeout)) 
    	{
			continue;
		}
#else
		if(0 == ring_buff_pop_data(client->pRingBuff, (uint8_t *)pch, 1)) //push data to ringbuff @ AT_UART_IRQHandler
    	{
			continue;
		}
#endif	
		else
		{
			break;
		}    
    } while (!expired(&timer));

    if(expired(&timer))
    {
		ret = QCLOUD_ERR_FAILURE;
	}
	
	return ret;
}

/**
 * AT client receive fixed-length data.
 *
 * @param client current AT client object
 * @param buf	receive data buffer
 * @param size	receive fixed data size
 * @param timeout  receive data timeout (ms)
 *
 * @note this function can only be used in execution function of URC data
 *
 * @return >0: receive data size
 *		   =0: receive failed
 */
int at_client_obj_recv(char *buf, int size, int timeout)
{
	int read_idx = 0;
	int result = QCLOUD_RET_SUCCESS;
	char ch;

	POINTER_SANITY_CHECK(buf, 0);
	at_client_t client = at_client_get();

	if (client == NULL)
	{
		Log_e("input AT Client object is NULL, please create or get AT Client object!");
		return 0;
	}

	while (1)
	{
		if (read_idx < size)
		{
			result = at_client_getchar(client, &ch, timeout);
			if (result != QCLOUD_RET_SUCCESS)
			{
				Log_e("AT Client receive failed, uart device get data error(%d)", result);
				return 0;
			}

			buf[read_idx++] = ch;
		}
		else
		{
			break;
		}
	}

#ifdef AT_DEBUG
	at_print_raw_cmd("urc_recv", buf, size);
#endif

	return read_idx;
}

/**
 *	AT client set end sign.
 *
 * @param client current AT client object
 * @param ch the end sign, can not be used when it is '\0'
 */
void at_set_end_sign(char ch)
{
	at_client_t client = at_client_get();
	
	if (client == NULL)
	{
		Log_e("input AT Client object is NULL, please create or get AT Client object!");
		return;
	}

	client->end_sign = ch;
}

/**
 * set URC(Unsolicited Result Code) table
 *
 * @param client current AT client object
 * @param table URC table
 * @param size table size
 */
void at_set_urc_table(at_client_t client, const at_urc_t urc_table, uint32_t table_sz)
{
	int idx;

	if (client == NULL)
	{
		Log_e("input AT Client object is NULL, please create or get AT Client object!");
		return;
	}

	for (idx = 0; idx < table_sz; idx++)
	{
		POINTER_SANITY_CHECK_RTN(urc_table[idx].cmd_prefix);
		POINTER_SANITY_CHECK_RTN(urc_table[idx].cmd_suffix);		
	}

	client->urc_table = urc_table;
	client->urc_table_size = table_sz;
}

at_client_t at_client_get(void)
{
    return &sg_at_client;
}

static const at_urc *get_urc_obj(at_client_t client)
{
	int i, prefix_len, suffix_len;
	int buf_sz;
	char *buffer = NULL;

	if (client->urc_table == NULL)
	{
		return NULL;
	}

	buffer = client->recv_buffer;
	buf_sz = client->cur_recv_len;

	for (i = 0; i < client->urc_table_size; i++)
	{
		prefix_len = strlen(client->urc_table[i].cmd_prefix);
		suffix_len = strlen(client->urc_table[i].cmd_suffix);
		if (buf_sz < prefix_len + suffix_len)
		{
			continue;
		}
		if ((prefix_len ? !strncmp(buffer, client->urc_table[i].cmd_prefix, prefix_len) : 1)
			&& (suffix_len ? !strncmp(buffer + buf_sz - suffix_len, client->urc_table[i].cmd_suffix, suffix_len) : 1))
		{
			//Log_d("matched:%s", client->urc_table[i].cmd_prefix);
			return &client->urc_table[i];
		}
	}

	return NULL;
}

static int at_recv_readline(at_client_t client)
{
	int read_len = 0;
	char ch = 0, last_ch = 0;
	bool is_full = false;
	int ret;

	memset(client->recv_buffer, 0x00, client->recv_bufsz);
	client->cur_recv_len = 0;

	while (1)
	{
		ret = at_client_getchar(client, &ch, GET_CHAR_TIMEOUT_MS);
		
		if(QCLOUD_RET_SUCCESS != ret)
		{
			return -1;
		}

		if (read_len < client->recv_bufsz)
		{
			client->recv_buffer[read_len++] = ch;
			client->cur_recv_len = read_len;
		}
		else
		{
			is_full = true;
		}
		
		/* is newline or URC data */		
		if ((ch == '\n' && last_ch == '\r') || (client->end_sign != 0 && ch == client->end_sign) || get_urc_obj(client))
		{
			
			if (is_full)
			{
				Log_e("read line failed. The line data length is out of buffer size(%d)!", client->recv_bufsz);
				memset(client->recv_buffer, 0x00, client->recv_bufsz);
				client->cur_recv_len = 0;
				ring_buff_flush(client->pRingBuff);
				return -1;
			}
			break;
		}
		last_ch = ch;
	}

#ifdef AT_DEBUG
	at_print_raw_cmd("recvline", client->recv_buffer, read_len);
#endif

	return read_len;
}


#ifdef AT_OS_USED
static void *client_parser(void *userContex)
{
	int resp_buf_len = 0;
	const at_urc *urc;
	int line_counts = 0;
	at_client_t client = at_client_get();

	Log_d("client_parser start...");
	
	while(1)
	{
		if (at_recv_readline(client) > 0)
		{
		
#ifdef 	AT_DEBUG	
			const char *cmd = NULL;
			int cmdsize = 0;
			cmd = at_get_last_cmd(&cmdsize);
			Log_d("last_cmd:(%.*s), readline:%s",  cmdsize, cmd, client->recv_buffer);
#endif			
			if ((urc = get_urc_obj(client)) != NULL)
			{
				/* current receive is request, try to execute related operations */
				if (urc->func != NULL)
				{
					urc->func(client->recv_buffer, client->cur_recv_len);
				}

			}
			else if (client->resp != NULL)
			{
				if(client->end_sign != 0) // handle endsign
				{	
					if(client->recv_buffer[client->cur_recv_len - 1] != client->end_sign)
					{	
						continue;
					} 
					else
					{
						goto exit;
					}
				} 

				/* current receive is response */
				client->recv_buffer[client->cur_recv_len - 1] = '\0';
				if (resp_buf_len + client->cur_recv_len < client->resp->buf_size)
				{
					/* copy response lines, separated by '\0' */
					memcpy(client->resp->buf + resp_buf_len, client->recv_buffer, client->cur_recv_len);
					resp_buf_len += client->cur_recv_len;

					line_counts++;				
				}
				else
				{
					client->resp_status = AT_RESP_BUFF_FULL;
					Log_e("Read response buffer failed. The Response buffer size is out of buffer size(%d)!", client->resp->buf_size);
				}
				/* check response result */
				if (memcmp(client->recv_buffer, AT_RESP_END_OK, strlen(AT_RESP_END_OK)) == 0
						&& client->resp->line_num == 0)
				{
					/* get the end data by response result, return response state END_OK. */
					client->resp_status = AT_RESP_OK;
				}
				else if (strstr(client->recv_buffer, AT_RESP_END_eLOG_ERROR)
						|| (memcmp(client->recv_buffer, AT_RESP_END_FAIL, strlen(AT_RESP_END_FAIL)) == 0))
				{
					client->resp_status = AT_RESP_eLOG_ERROR;
				}
				else if (line_counts == client->resp->line_num && client->resp->line_num)
				{
					/* get the end data by response line, return response state END_OK.*/
					client->resp_status = AT_RESP_OK;
				}
				else
				{
					continue;
				}
exit:
				client->resp->line_counts = line_counts;
				client->resp = NULL;				
				resp_buf_len = 0;
				line_counts = 0;
				HAL_SemaphorePost(client->resp_sem);
			}
			else
			{
//				  Log_d("unrecognized line: %.*s", client->cur_recv_len, client->recv_buffer);
			}
		}
		else
		{
			//Log_d("read no new line");
		}
		
	}
	return NULL;
}
#else
void at_client_yeild(at_urc *expect_urc, uint32_t timeout)
{
	int resp_buf_len = 0;
	const at_urc *urc;
	int line_counts = 0;
	int prefix_len;
	int suffix_len;
	Timer timer;
	at_client_t client = at_client_get();
	
	Log_d("Entry...");	
	countdown_ms(&timer, timeout);
	do
	{
		if (at_recv_readline(client) > 0)
		{
#ifdef 	AT_DEBUG	
			const char *cmd = NULL;
			int cmdsize = 0;
			cmd = at_get_last_cmd(&cmdsize);
			Log_d("last_cmd:(%.*s), readline:%s",  cmdsize, cmd, client->recv_buffer);
#endif
			if ((urc = get_urc_obj(client)) != NULL)
			{
				/* current receive is request, try to execute related operations */
				if (urc->func != NULL)
				{
					urc->func(client->recv_buffer, client->cur_recv_len);
				}
			
				/*expect urc matched then break*/
				if(expect_urc != NULL)
				{
					prefix_len = strlen(expect_urc->cmd_prefix);
					suffix_len = strlen(expect_urc->cmd_suffix);
					if ((prefix_len ? !strncmp(urc->cmd_prefix, expect_urc->cmd_prefix, prefix_len) : 1)
						&& (suffix_len ? !strncmp(urc->cmd_suffix, expect_urc->cmd_suffix, suffix_len) : 1)){
						client->resp_status = AT_RESP_OK;
						break;
					}
				}
			}
			else if (client->resp != NULL)
			{
				if(client->end_sign != 0) // handle endsign
				{
					if(client->recv_buffer[client->cur_recv_len - 1] != client->end_sign)
					{
						continue;
					} 
					else
					{
						client->resp_status = AT_RESP_OK;
						client->resp->line_counts = line_counts;
						client->resp = NULL;
						//client->resp_notice = true;
						resp_buf_len = 0;
						line_counts = 0;
						break;
					}
				} 
					
				/* current receive is response */
				client->recv_buffer[client->cur_recv_len - 1] = '\0';
				if (resp_buf_len + client->cur_recv_len < client->resp->buf_size)
				{
					/* copy response lines, separated by '\0' */
					memcpy(client->resp->buf + resp_buf_len, client->recv_buffer, client->cur_recv_len);
					resp_buf_len += client->cur_recv_len;

					line_counts++;					
				}
				else
				{
					client->resp_status = AT_RESP_BUFF_FULL;
					Log_e("Read response buffer failed. The Response buffer size is out of buffer size(%d)!", client->resp->buf_size);
				}
				/* check response result */
				if (memcmp(client->recv_buffer, AT_RESP_END_OK, strlen(AT_RESP_END_OK)) == 0
					&& client->resp->line_num == 0)
				{
					/* get the end data by response result, return response state END_OK. */
					client->resp_status = AT_RESP_OK;
				}
				else if (strstr(client->recv_buffer, AT_RESP_END_eLOG_ERROR)
						|| (memcmp(client->recv_buffer, AT_RESP_END_FAIL, strlen(AT_RESP_END_FAIL)) == 0))
				{
					client->resp_status = AT_RESP_eLOG_ERROR;
				}
				else if (line_counts == client->resp->line_num && client->resp->line_num)
				{
					/* get the end data by response line, return response state END_OK.*/
					client->resp_status = AT_RESP_OK;
				}
				else
				{ 	if(!expired(&timer))					
					{
						continue;
					}
					else
					{
						break;
					}
				}

				client->resp->line_counts = line_counts;
				client->resp = NULL;
				//client->resp_notice = true;
				resp_buf_len = 0;
				line_counts = 0;
				break;
			}
			else
			{
				 // Log_d("unrecognized line: %.*s", client->cur_recv_len, client->recv_buffer);
			}
		}
		else
		{
			//Log_d("read no new line");
		}
	}while(!expired(&timer));
}
#endif

/* initialize the client parameters */
int at_client_para_init(at_client_t client)
{
	client->lock = HAL_MutexCreate();
	if(NULL == client->lock)
	{
		Log_e("create lock err");
		return QCLOUD_ERR_FAILURE;
	}

#ifdef AT_OS_USED	
	client->resp_sem = HAL_SemaphoreCreate();
	if (NULL == client->resp_sem)
	{
		Log_e("create sem err");
		return QCLOUD_ERR_FAILURE;
	}
	
	client->parser = client_parser;
#endif	

	char * ringBuff = HAL_Malloc(RING_BUFF_LEN);
	if(NULL == ringBuff)
	{
		Log_e("malloc ringbuff err");
		return QCLOUD_ERR_FAILURE;
	}
	ring_buff_init(&g_ring_buff, ringBuff,  RING_BUFF_LEN);

	char * recvBuff = HAL_Malloc(CLINET_BUFF_LEN);
	if(NULL == recvBuff)
	{
		Log_e("malloc recvbuff err");
		return QCLOUD_ERR_FAILURE;
	}
	client->recv_buffer = recvBuff;


	client->pRingBuff = &g_ring_buff;
	client->recv_bufsz = CLINET_BUFF_LEN;	
	client->cur_recv_len = 0;
	
	client->resp = NULL;

	client->urc_table = NULL;
	client->urc_table_size = 0;	
	client->end_sign = 0;

	return QCLOUD_RET_SUCCESS;
}

/**
 * AT client initialize.
 *
 * @param pClient pinter of at client which to be inited
 * @return @see eTidResault
 */
int at_client_init(at_client_t *pClient)
{
	POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL); 
	at_client_t client;
	int result;

	client = at_client_get();

	if(NULL == client)
	{
		Log_e("no at client get");
		result = QCLOUD_ERR_FAILURE;		
		goto exit; 
	}

	if(AT_STATUS_INITIALIZED == client->status)
	{
		Log_e("at client has been initialized");
		result = QCLOUD_ERR_FAILURE;
		goto exit;
	}
	
	result = at_client_para_init(client);
	if (result == QCLOUD_RET_SUCCESS)
	{
		Log_d("AT client(V%s) initialize success.", AT_FRAME_VERSION);
		client->status = AT_STATUS_INITIALIZED;
		*pClient = client;

#ifdef AT_OS_USED	
		//	create thread for at parser
		if(NULL != client->parser)
		{
#define AT_PARSER_THREAD_STACK		6144
#define	AT_PARSER_THREAD_PRIORITY	0

			client->thread_t = HAL_ThreadCreate(
										AT_PARSER_THREAD_STACK, \
										AT_PARSER_THREAD_PRIORITY,
										"at_client_parser",
										client->parser, \
 										client);
										 
			result = client->thread_t ?  QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
			if(QCLOUD_RET_SUCCESS == result){
				Log_d("create at_parser thread success!");
			}else{
				Log_d("create at_parser thread fail!");
			}
		
#undef 	AT_PARSER_THREAD_STACK
#undef	AT_PARSER_THREAD_PRIORITY
		}	
#endif	

	}
	else
	{
		*pClient = NULL;
		client->status = AT_STATUS_UNINITIALIZED;
		Log_e("AT client(V%s) initialize failed(%d).", AT_FRAME_VERSION, result);
	}

exit:

	return result;
}

int at_client_deinit(at_client_t pClient){

	//TO DO:
	return QCLOUD_RET_SUCCESS;
}
