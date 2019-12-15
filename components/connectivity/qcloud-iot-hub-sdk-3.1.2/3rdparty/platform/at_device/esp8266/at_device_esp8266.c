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
#include <stdio.h>
#include <string.h>

#include "qcloud_iot_import.h"
#include "qcloud_iot_export.h"

#include "utils_param_check.h"
#include "at_client.h"
#include "at_socket_inf.h"
#include "at_device_esp8266.h"

#define  WIFI_CONN_FLAG 			(1<<0)
#define  SEND_OK_FLAG 				(1<<1)
#define  SEND_FAIL_FLAG				(1<<2)

static uint8_t sg_SocketBitMap = 0;

static at_evt_cb_t at_evt_cb_table[] = {
        [AT_SOCKET_EVT_RECV] = NULL,
        [AT_SOCKET_EVT_CLOSED] = NULL,
};
		
static int alloc_fd(void)
{

	uint8_t i;
	int fd;

	for(i = 0; i < ESP8266_MAX_SOCKET_NUM; i++){
		if(0 == ((sg_SocketBitMap>>i) & 0x01)){
			sg_SocketBitMap |= (1<<i)&0xff;
			break;
		}
	}

	return (i < ESP8266_MAX_SOCKET_NUM)?(fd = i):(fd = UNUSED_SOCKET);
}

static void free_fd(int fd)
{
	uint8_t i = fd;

	if((fd != UNUSED_SOCKET) && fd <  ESP8266_MAX_SOCKET_NUM){
		sg_SocketBitMap &= ~((1<<i)&0xff);
	}	
}

static void urc_send_func(const char *data, size_t size)
{
	
	POINTER_SANITY_CHECK_RTN(data);
	
    if(strstr(data, "SEND OK")){
        at_setFlag(SEND_OK_FLAG);
    }else if(strstr(data, "SEND FAIL")){
        at_setFlag(SEND_FAIL_FLAG);
    }
}

static void urc_send_bfsz_func(const char *data, size_t size)
{
	POINTER_SANITY_CHECK_RTN(data);
    int send_bfsz = 0;

    sscanf(data, "Recv %d bytes", &send_bfsz);
}

static void urc_close_func(const char *data, size_t size)
{
    int fd = 0;
	POINTER_SANITY_CHECK_RTN(data);

    sscanf(data, "%d,CLOSED", &fd);
	
    /* notice the socket is disconnect by remote */
    if (at_evt_cb_table[AT_SOCKET_EVT_CLOSED]){
        at_evt_cb_table[AT_SOCKET_EVT_CLOSED](fd, AT_SOCKET_EVT_CLOSED, NULL, 0);
    }

	free_fd(fd);
}

static void urc_recv_func(const char *data, size_t size)
{
    int fd;
    size_t bfsz = 0, temp_size = 0;
    uint32_t timeout;
    char *recv_buf, temp[8];

    POINTER_SANITY_CHECK_RTN(data);

    /* get the current socket and receive buffer size by receive data */
    sscanf(data, "+IPD,%d,%d:", &fd, (int *) &bfsz);
	
    /* get receive timeout by receive buffer length */
    timeout = bfsz;

    if (fd < 0 || bfsz == 0)
        return;

    recv_buf = HAL_Malloc(bfsz);
    if (NULL == recv_buf)
    {
        Log_e("no memory for URC receive buffer (%d)!", bfsz);
        /* read and clean the coming data */
        while (temp_size < bfsz)
        {
            if (bfsz - temp_size > sizeof(temp)){
                at_client_recv(temp, sizeof(temp), timeout);
            }else{
                at_client_recv(temp, bfsz - temp_size, timeout);
            }
            temp_size += sizeof(temp);
        }
        return;
    }

    /* sync receive data */
    if (at_client_recv(recv_buf, bfsz, timeout) != bfsz)
    {
        Log_e("receive size(%d) data failed!", bfsz);
        HAL_Free(recv_buf);
        return;
    }

    /* notice the receive buffer and buffer size */
    if (at_evt_cb_table[AT_SOCKET_EVT_RECV]){
        at_evt_cb_table[AT_SOCKET_EVT_RECV](fd, AT_SOCKET_EVT_RECV, recv_buf, bfsz);
    }
}

static void urc_busy_p_func(const char *data, size_t size)
{
    POINTER_SANITY_CHECK_RTN(data);
	
    Log_d("system is processing a commands and it cannot respond to the current commands.");
}

static void urc_busy_s_func(const char *data, size_t size)
{
    POINTER_SANITY_CHECK_RTN(data);

    Log_d("system is sending data and it cannot respond to the current commands.");
}

static void urc_func(const char *data, size_t size)
{
	POINTER_SANITY_CHECK_RTN(data);
	int i;
	
    if (strstr(data, "WIFI CONNECTED")){
        at_setFlag(WIFI_CONN_FLAG);
	}else if (strstr(data, "WIFI DISCONNECT")){	
		/* notice the socket is disconnect by remote */
	    if (at_evt_cb_table[AT_SOCKET_EVT_CLOSED]){
			for(i = 0; i < ESP8266_MAX_SOCKET_NUM; i++){
				 at_evt_cb_table[AT_SOCKET_EVT_CLOSED](i, AT_SOCKET_EVT_CLOSED, NULL, 0);	
				 free_fd(i);
			}
	    }
    }
}

static at_urc urc_table[] = {
        {"SEND OK",          "\r\n",           urc_send_func},
        {"SEND FAIL",        "\r\n",           urc_send_func},
        {"Recv",             "bytes\r\n",      urc_send_bfsz_func},
        {"",                 ",CLOSED\r\n",    urc_close_func},
        {"+IPD",             ":",              urc_recv_func},
        {"busy p",           "\r\n",           urc_busy_p_func},
        {"busy s",           "\r\n",           urc_busy_s_func},
        {"WIFI CONNECTED",   "\r\n",           urc_func},
        {"WIFI DISCONNECT",  "\r\n",           urc_func},
};

static void esp8266_set_event_cb(at_socket_evt_t event, at_evt_cb_t cb)
{
    if (event < sizeof(at_evt_cb_table) / sizeof(at_evt_cb_table[0])){
        at_evt_cb_table[event] = cb;
    }
}

static int esp8266_init(void)
{
	at_response_t resp = NULL;
	int ret;
	int i;

	resp = at_create_resp(512, 0, AT_RESP_TIMEOUT_MS);
	if (NULL == resp){
		Log_e("No memory for response structure!");
		ret = QCLOUD_ERR_FAILURE;
		goto __exit;
	}

	/* reset module */
	at_exec_cmd(resp, "AT+RST");
	
	/* reset waiting delay */
	at_delayms(2000);
	
	/* disable echo */
	
	ret = at_exec_cmd(resp, "ATE0");
	if(QCLOUD_RET_SUCCESS != ret){
    	Log_e("cmd ATE0 exec err");
		//goto exit;
	}
	
	at_delayms(100);
	/* set current mode to Wi-Fi station */
	ret = at_exec_cmd(resp, "AT+CWMODE=1");
	if(QCLOUD_RET_SUCCESS != ret){
    	Log_e("cmd AT+CWMODE=1 exec err");
		//goto exit;
	}
	
	at_delayms(100);
	/* get module version */
	ret = at_exec_cmd(resp, "AT+GMR");
	if(QCLOUD_RET_SUCCESS != ret){
    	Log_e("cmd AT+CWMODE=1 exec err");
		//goto exit;
	}
	
	/* show module version */
	for (i = 0; i < resp->line_counts - 1; i++){
		Log_d("%s", at_resp_get_line(resp, i + 1));
	}

	at_delayms(100);
	at_clearFlag(WIFI_CONN_FLAG);
	/* connect to WiFi AP */
	ret = at_exec_cmd(resp, "AT+CWJAP=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASSWORD);

	if(!at_waitFlag(WIFI_CONN_FLAG, AT_RESP_TIMEOUT_MS)){
		Log_e("wifi connect timeout");
		ret = QCLOUD_ERR_FAILURE;
		goto __exit;
	}
	
	ret = at_exec_cmd(resp, "AT+CIPMUX=1");
	if(QCLOUD_RET_SUCCESS != ret){
    	Log_e("cmd AT+CIPMUX=1 exec err");
	}

__exit:

	if (resp){
		at_delete_resp(resp);
	}
	
	return ret;
}

static int esp8266_close(int fd)
{
	at_response_t resp;
	int ret;

	resp = at_create_resp(128, 0, AT_RESP_TIMEOUT_MS);
	if (NULL == resp){
		Log_e("No memory for response structure!");
		return QCLOUD_ERR_FAILURE;
	}

	ret = at_exec_cmd(resp, "AT+CIPCLOSE=%d", fd);

	if (QCLOUD_RET_SUCCESS != ret)	{ //fancyxu
		Log_e("close socket(%d) fail",fd);
	}

	if (resp){
		at_delete_resp(resp);
	}

	return ret;
}

static int esp8266_connect(const char *ip, uint16_t port, eNetProto proto)
{
	at_response_t resp;
	bool retryed = false;
	int fd, ret;
	
	POINTER_SANITY_CHECK(ip, QCLOUD_ERR_INVAL);
	resp = at_create_resp(128, 0, AT_RESP_TIMEOUT_MS);
	if (NULL == resp){
		Log_e("No memory for response structure!");
		return QCLOUD_ERR_FAILURE;
	}
	
	fd = alloc_fd();
	if(fd < 0){
		Log_e("esp8226 support max %d chain", ESP8266_MAX_SOCKET_NUM);
		return QCLOUD_ERR_FAILURE;
	}
	
__retry:	

	switch (proto)
	{
		case eNET_TCP:
			/* send AT commands to connect TCP server */
			ret = at_exec_cmd(resp, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d,60", fd, ip, port); 
			if(QCLOUD_RET_SUCCESS != ret){
				Log_e("start tcp connect failed, fd=%d,ip(%s),port(%d)",fd, ip, port);
			}
			break;

		case eNET_UDP:
			ret = at_exec_cmd(resp, "AT+CIPSTART=%d,\"UDP\",\"%s\",%d", fd, ip, port);	
			if(QCLOUD_RET_SUCCESS != ret){
				Log_e("start udp connect failed, fd=%d,ip(%s),port(%d)",fd, ip, port);
			}
			break;

		default:
			Log_e("Not supported connect type : %d", proto);
			ret = QCLOUD_ERR_FAILURE;
			goto __exit;
	}

	if ((QCLOUD_RET_SUCCESS != ret) && !retryed){
		Log_e("socket(%d) connect failed, maybe the socket was not be closed at the last time and now will retry.", fd);
		if (QCLOUD_RET_SUCCESS != esp8266_close(fd)){
			goto __exit;
		}
		retryed = true;
		at_delayms(100);
		goto __retry;
	}

__exit:

	if (resp){
		at_delete_resp(resp);
	}

	if(QCLOUD_RET_SUCCESS != ret){
		free_fd(fd);
		fd = UNUSED_SOCKET;
	}

	at_delayms(200);
	return fd;
}

static int esp8266_send(int fd, const void *buff, size_t len)
{
	int ret;
	at_response_t resp ;
	size_t cur_pkt_size = 0;
	size_t sent_size = 0;
	size_t temp_size = 0;

	POINTER_SANITY_CHECK(buff, QCLOUD_ERR_INVAL);
	resp = at_create_resp(512, 2, AT_RESP_TIMEOUT_MS);
	if (NULL == resp){
		Log_e("No memory for response structure!");
		return QCLOUD_ERR_FAILURE;
	}
	
	/* set AT client end sign to deal with '>' sign.*/
	at_set_end_sign('>');

	while (sent_size < len)
	{
		if (len - sent_size < ESP8266_SEND_MAX_LEN_ONCE){
			cur_pkt_size = len - sent_size;
		}else{
			cur_pkt_size = ESP8266_SEND_MAX_LEN_ONCE;
		}

		at_clearFlag(SEND_OK_FLAG);
		at_clearFlag(SEND_FAIL_FLAG);
		/* send the "AT+CIPSEND" commands to AT server than receive the '>' response on the first line. */
		ret = at_exec_cmd(resp, "AT+CIPSEND=%d,%d", fd, cur_pkt_size);
		if(QCLOUD_RET_SUCCESS != ret){
			Log_e("cmd AT+CIPSEND exec err");
			goto __exit;
		}
		at_set_end_sign(0);
		/* send the real data to server or client */
		temp_size = at_client_send(at_client_get(), (char *)buff + sent_size, cur_pkt_size, AT_RESP_TIMEOUT_MS);

		if(cur_pkt_size != temp_size){
			Log_e("at send real data failed");
			goto __exit;
		}

#ifndef AT_OS_USED
		at_urc urc_send = {.cmd_prefix = "SEND OK",.cmd_suffix = "\r\n", NULL};
		at_client_yeild(&urc_send, resp->timeout);	
		if (at_client_get()->resp_status != AT_RESP_OK)
		{
			Log_e("send fail");
			ret = QCLOUD_ERR_FAILURE;
			goto __exit;
		}
#else
		if(at_waitFlag(SEND_OK_FLAG|SEND_FAIL_FLAG, AT_RESP_TIMEOUT_MS)){
			if(at_waitFlag(SEND_FAIL_FLAG, AT_RESP_TIMEOUT_MS)){
				Log_e("send fail");
				ret = QCLOUD_ERR_FAILURE;
				goto __exit;
			}
		}
#endif
		sent_size += cur_pkt_size;
	}

	ret = QCLOUD_RET_SUCCESS;

__exit:
	/* reset the end sign for data */
	if (resp){
		at_delete_resp(resp);
	}

	return sent_size; //fancyxu
}

static int esp8266_recv_timeout(int fd, void *buf, size_t len, uint32_t timeout)
{
#ifndef AT_OS_USED
	at_client_yeild(NULL, timeout);
#endif
	return QCLOUD_RET_SUCCESS;
}

static int esp8266_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
#define RESOLVE_RETRY        5

	char recv_ip[16] = { 0 };
	at_response_t resp;		
	int ret,i;

	POINTER_SANITY_CHECK(host_name, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(host_ip, QCLOUD_ERR_INVAL);


	if(host_ip_len < 16){
		Log_e("host ip buff too short");
		return QCLOUD_ERR_FAILURE;
	}

	resp = at_create_resp(128, 0, 4*AT_RESP_TIMEOUT_MS);
	if (NULL == resp){
		Log_e("No memory for response structure!");
		return QCLOUD_ERR_FAILURE;
	}

	for (i = 0; i < RESOLVE_RETRY; i++){
		ret = at_exec_cmd(resp, "AT+CIPDOMAIN=\"%s\"", host_name);
		if (QCLOUD_RET_SUCCESS != ret){
			Log_e("exec AT+CIPDOMAIN=\"%s\" fail", host_name);
			goto __exit;
		}

		/* parse the third line of response data, get the IP address */
		if (at_resp_parse_line_args_by_kw(resp, "+CIPDOMAIN:", "+CIPDOMAIN:%s", recv_ip) < 0){
			at_delayms(100);
			/* resolve failed, maybe receive an URC CRLF */
			continue;
		}

		if (strlen(recv_ip) < 8){
			at_delayms(100);
			/* resolve failed, maybe receive an URC CRLF */
			continue;
		}
		else{
			strncpy(host_ip, recv_ip, 15);
			host_ip[15] = '\0';
			break;
		}
	}

__exit:

	if (resp){
		at_delete_resp(resp);
	}
	
#undef RESOLVE_RETRY 

	return ret;	
}

at_device_op_t at_ops_esp8266 = {
    .init           = esp8266_init,
    .connect        = esp8266_connect,
    .send           = esp8266_send,
    .recv_timeout   = esp8266_recv_timeout,
    .close          = esp8266_close,
    .parse_domain   = esp8266_parse_domain,
    .set_event_cb   = esp8266_set_event_cb,
    .deviceName		= "esp8266",
};

int at_device_esp8266_init(void)
{	
	int i;
	int ret;
	at_client_t p_client;
	
	ret = HAL_AT_Uart_Init();
    if (QCLOUD_RET_SUCCESS != ret) {
        Log_e("at uart init fail!");
    } else {
		Log_d("at uart init success!");
	}

    /* initialize AT client */
    ret = at_client_init(&p_client);

	if(QCLOUD_RET_SUCCESS != ret)
 	{
		Log_e("at client init fail,ret:%d", ret);
		goto exit;
	}
	else
	{
		Log_d("at client init success");
	}
	
    /* register URC data execution function  */
    at_set_urc_table(p_client, urc_table, sizeof(urc_table) / sizeof(urc_table[0]));

	Log_d("urc table addr:%p, size:%d", p_client->urc_table, p_client->urc_table_size);
	for(i=0; i < p_client->urc_table_size; i++)
	{
		Log_d("%s",p_client->urc_table[i].cmd_prefix);
	}

	ret = at_device_op_register(&at_ops_esp8266);
	if(QCLOUD_RET_SUCCESS != ret){
		Log_e("at device driver register fail");
		goto exit;
	}

exit:
	if(QCLOUD_RET_SUCCESS != ret){
		if(NULL != p_client){
			at_client_deinit(p_client);
		}			
	}
	
    return ret;
}

/*at device driver must realize this api which called by HAL_AT_TCP_Init*/
int at_device_init(void)
{
	return at_device_esp8266_init();
}
