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

  
#ifndef __AT_CLIENT_H__
#define __AT_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "utils_ringbuff.h"


#define AT_FRAME_VERSION               "1.0.0"

#define AT_CMD_NAME_LEN                16
#define AT_END_MARK_LEN                4

#define CLINET_BUFF_LEN				   (1024)
#define RING_BUFF_LEN         		   CLINET_BUFF_LEN	 //uart ring buffer len
#define GET_CHAR_TIMEOUT_MS			   (5000)
#define CMD_RESPONSE_INTERVAL_MS 	   (100)

typedef void *(*ParserFunc)(void *userContex);

typedef enum 
{
    AT_STATUS_UNINITIALIZED = 0,
    AT_STATUS_INITIALIZED = 0x55,
    AT_STATUS_BUSY = 0xaa,
}at_status;

enum at_resp_status
{
     AT_RESP_OK = 0,                   /* AT response end is OK */
     AT_RESP_ERROR = -1,               /* AT response end is ERROR */
     AT_RESP_TIMEOUT = -2,             /* AT response is timeout */
     AT_RESP_BUFF_FULL= -3,            /* AT response buffer is full */
};

typedef enum at_resp_status at_resp_status_t;

typedef struct _at_response_
{
    /* response buffer */
    char *buf;
    /* the maximum response buffer size */
    int buf_size;
    /* the number of setting response lines
     * == 0: the response data will auto return when received 'OK' or 'ERROR'
     * != 0: the response data will return when received setting lines number data */
    int line_num;
    /* the count of received response lines */
    int line_counts;
    /* the maximum response time */
    uint32_t timeout;
}at_response;

typedef  at_response * at_response_t;

/* URC(Unsolicited Result Code) object, such as: 'RING', 'READY' request by AT server */
typedef struct _at_urc_
{
    const char *cmd_prefix;
    const char *cmd_suffix;
    void (*func)(const char *data, size_t size);
}at_urc;

typedef at_urc *at_urc_t;

typedef struct _at_client_
{
    at_status status;
    char end_sign;
	
	ring_buff_t pRingBuff;

    char *recv_buffer;
    uint32_t recv_bufsz;
    uint32_t cur_recv_len;
	void *lock;      		//pre cmd take the lock wait for resp , another cmd need wait for unlock
	
    at_response_t resp;
    at_resp_status_t resp_status;

    const at_urc *urc_table;
    uint16_t urc_table_size;
        
#ifdef AT_OS_USED
	volatile void *thread_t;	
	void *resp_sem;	 		// resp received, send sem to notic ack wait
	ParserFunc parser; 		// RX parser
#else
	//bool resp_notice;
#endif
}at_client;

typedef at_client *at_client_t;

/* AT client initialize and start*/
int at_client_init(at_client_t *pClient);

/* AT client deinitial*/
int at_client_deinit(at_client_t pClient);

/* get AT client handle*/
at_client_t at_client_get(void);

/*AT connect detect*/
int at_client_wait_connect(uint32_t timeout);

/*wrapper for os and nonos delay*/
void at_delayms(uint32_t delayms);
void at_setFlag(uint32_t flag);
void at_clearFlag(uint32_t flag);
uint32_t at_getFlag(void);
bool at_waitFlag(uint32_t flag, uint32_t timeout);

/* ========================== multiple AT client function ============================ */
/* set AT client a line end sign */
void at_set_end_sign(char ch);

/* Set URC(Unsolicited Result Code) table */
void at_set_urc_table(at_client_t client, const at_urc_t table, uint32_t size);

/* AT client send or receive data */
int at_client_send(at_client_t client, const char *buf, int size, uint32_t timeout);
int at_client_obj_recv(char *buf, int size, int timeout);

/* AT client send commands to AT server and waiter response */
int at_obj_exec_cmd(at_response_t resp, const char *cmd_expr, ...);

#define at_exec_cmd(resp, ...)                   at_obj_exec_cmd(resp, __VA_ARGS__)
#define at_client_recv(buf, size, timeout)       at_client_obj_recv(buf, size, timeout)

/* AT response object create and delete */
at_response_t at_create_resp(uint32_t buf_size, uint32_t line_num, uint32_t timeout);
void at_delete_resp(at_response_t resp);

/* AT response line buffer get and parse response buffer arguments */
const char *at_resp_get_line(at_response_t resp, uint32_t resp_line);
const char *at_resp_get_line_by_kw(at_response_t resp, const char *keyword);
int at_resp_parse_line_args(at_response_t resp, uint32_t resp_line, const char *resp_expr, ...);
int at_resp_parse_line_args_by_kw(at_response_t resp, const char *keyword, const char *resp_expr, ...);

/* ========================== single AT client function ============================ */
void at_client_yeild(at_urc *expect_urc, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* __AT_H__ */
