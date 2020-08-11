/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#include "m26.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

static int m26_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 300, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK)
    {
        return 0;
    }
    return -1;
}
static int m26_sim_card_check(void)
{
    at_echo_t echo;
    int try = 0;
    char echo_buffer[32];

    while (try++ < 10)
    {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
        tos_at_cmd_exec(&echo, 2000, "AT+CPIN?\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
                return -1;
        }

        if(strstr(echo.buffer, "READY"))
        {
            return 0;
        }
    }
		 
    return -1;
}

static int m26_signal_quality_check(void)
{
    int rssi, ber;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;
	
    while (try++ < 10) 
    {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
        tos_at_cmd_exec(&echo, 1000, "AT+CSQ\r\n");
        if (echo.status != AT_ECHO_STATUS_OK)
        {
            return -1;
        }

        str = strstr(echo.buffer, "+CSQ:");
        if (!str) 
        {
            return -1;
        }
        sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
        if (rssi != 99) {
            return 0;
        }
    }

    return -1;
}
static int m26_gsm_network_check(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;
	
    while (try++ < 10)
    {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
        tos_at_cmd_exec(&echo, 1000, "AT+CREG?\r\n");
        if (echo.status != AT_ECHO_STATUS_OK)
        {
            return -1;
        }

        str = strstr(echo.buffer, "+CREG:");
        if (!str) 
        {
            return -1;
        }
        sscanf(str, "+CREG:%d,%d", &n, &stat);
        if (stat == 1)
        {
            return 0;
        }
	}
    return -1;	
}

static int m26_gprs_network_check(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;

    while (try++ < 10)
    {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
        tos_at_cmd_exec(&echo, 1000, "AT+CGREG?\r\n");
        if (echo.status != AT_ECHO_STATUS_OK)
        {
            return -1;
        }

        str = strstr(echo.buffer, "+CGREG:");
        if (!str) 
        {
            return -1;
        }
        sscanf(str, "+CGREG:%d,%d", &n, &stat);
        if (stat == 1)
        {
            return 0;
        }
	}
		
    return -1;	
}

static int m26_close_apn(void)
{
    at_echo_t echo;
    char echo_buffer[32];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 3000, "AT+QIDEACT\r\n");
   	
    if(strstr(echo.buffer, "DEACT OK") == NULL)
    {
        return -1;
    }

    return 0;
}

static int m26_send_mode_set(m26_send_mode_t mode)
{
    int try = 0;
    at_echo_t echo;

    while (try++ < 10)
    {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 300, "AT+QIMODE=%d\r\n", mode == M26_SEND_MODE_NORMAL ? 0 : 1);
        if (echo.status == AT_ECHO_STATUS_OK)
        {
            return 0;
        }
    }
    return -1;
}

static int m26_multilink_set(m26_multilink_state_t state)
{
    int try = 0;
    at_echo_t echo;

    while (try++ < 10)
    {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 300, "AT+QIMUX=%d\r\n", state == M26_MULTILINK_STATE_ENABLE ? 1 : 0);
        if (echo.status == AT_ECHO_STATUS_OK)
        {
            return 0;
        }
    }
    return -1;
}

static int m26_recv_mode_set()
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 300, "AT+QINDI=0\r\n");
    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }
    return 0;
}

static int m26_set_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 300, "AT+QICSGP=1,\"CMNET\"\r\n");
    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }

    tos_at_cmd_exec(&echo, 300, "AT+QIFGCNT=0\r\n");
    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }
		
    tos_at_cmd_exec(&echo, 300, "AT+QIREGAPP\r\n");
    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }
		
    tos_at_cmd_exec(&echo, 3000, "AT+QIACT\r\n");
    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }	
		
    tos_at_cmd_exec(&echo, 300, "AT+QILOCIP\r\n");

    return 0;
}



static int m26_init(void)
{
    printf("Init m26 ...\n" );

    if (m26_echo_close() != 0)
    {
        printf("echo close failed\n");
        return -1;
    }
		
    if(m26_sim_card_check() != 0)
    {
        printf("sim card check failed,please insert your card\n");
        return -1;
    }

    if (m26_signal_quality_check() != 0)
    {
        printf("signal quality check status failed\n");
        return -1;
    }
		
    if(m26_gsm_network_check() != 0)
    {
        printf("GSM network register status check fail\n");
        return -1;
    }
    
    if(m26_gprs_network_check() != 0)
    {
        printf("GPRS network register status check fail\n");
        return -1;
    }
    
    if(m26_close_apn() != 0)
    {
        printf("close apn failed\n");
        return -1;
    }
		
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u		
    if (m26_multilink_set(SAL_MULTILINK_STATE_DISABLE) != 0)
    {
        printf("multilink set FAILED\n");
        return -1;
    }
#else
    if (m26_multilink_set(M26_MULTILINK_STATE_ENABLE) != 0)
    {
        printf("multilink set FAILED\n");
        return -1;
    }
#endif		

    if (m26_send_mode_set(M26_SEND_MODE_NORMAL) != 0)
    {
        printf("send mode set FAILED\n");
        return -1;
    }
		
    if(m26_recv_mode_set() != 0)
    {
        printf("recv mode set FAILED\n");
        return -1;
    }
		
    if (m26_set_apn() != 0) {
        printf("apn set FAILED\n");
        return -1;
    }

    printf("Init m26 ok\n" );
    return 0;
}

static int m26_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;

    id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }
		
    tos_at_echo_create(&echo, NULL, 0, NULL);	
    tos_at_cmd_exec(&echo, 2000, "%s=1\r\n", "AT+QIHEAD");
    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_channel_free(id);
        return -1;
    }

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_echo_create(&echo, NULL, 0, "CONNECT OK");
    tos_at_cmd_exec(&echo, 4000, "AT+QIOPEN=\"%s\",\"%s\",\"%s\"\r\n",
                        proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, atoi(port));
    if (echo.status == AT_ECHO_STATUS_OK) {
        return id;
    }
#else
    tos_at_echo_create(&echo, NULL, 0, "CONNECT OK");
    tos_at_cmd_exec(&echo, 4000, "AT+QIOPEN=%d,\"%s\",\"%s\",%d\r\n",
                        id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, atoi(port));
    if (echo.status == AT_ECHO_STATUS_OK) {
        return id;
    }
#endif
		
    tos_at_channel_free(id);
    return -1;
}

static int m26_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int m26_recv(int id, void *buf, size_t len)
{
    return m26_recv_timeout(id, buf, len, (uint32_t)4000);
}

int m26_send(int id, const void *buf, size_t len)
{
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0)
    {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");
		
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(&echo, 1000,
                            "AT+QISEND=%d\r\n",
                            len);
#else
    tos_at_cmd_exec(&echo, 1000,
                            "AT+QISEND=%d,%d\r\n",
                            id, len);
#endif

    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT)
    {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "SEND OK");
    tos_at_raw_data_send(&echo, 1000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT)
    {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}

static void m26_transparent_mode_exit(void)
{
    tos_at_cmd_exec(NULL, 500, "+++");
}

static int m26_close(int id)
{
    m26_transparent_mode_exit();

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(NULL, 1000, "AT+QICLOSE=%d\r\n", id);
#else
    tos_at_cmd_exec(NULL, 1000, "AT+QICLOSE=%d\r\n", id);
#endif

    tos_at_channel_free(id);

    return 0;
}

static int m26_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    char *str;
    at_echo_t echo;
    char echo_buffer[64];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+QIDNSGIP=\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }

    /*
        "xxx.xxx.xxx.xxx"
    */

    int seg1, seg2, seg3, seg4;
    str = strstr(echo.buffer, "OK");
    if (!str) 
    {
        return -1;
    }
    str += strlen("OK\r\n");
    sscanf(str, "%d.%d.%d.%d", &seg1, &seg2, &seg3, &seg4);
    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", seg1, seg2, seg3, seg4);
    host_ip[host_ip_len - 1] = '\0';

    printf("GOT IP: %s\n", host_ip);

    return 0;
}

__STATIC__ void m26_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, data_len = 0, read_len;
    uint8_t buffer[128];

    /*
        +RECEIVE: 0, <data_len>
	    IPD<data_len>:<data context>

        +RECEIVE:     prefix
        0:          	scoket id
    */
    if (tos_at_uart_read(&data, 1) != 1)
    {
            return;
    }
	
    while (1)
    {
        if (tos_at_uart_read(&data, 1) != 1)
        {
            return;
        }

        if (data == ',')
        {
            break;
        }
        channel_id = channel_id * 10 + (data - '0');
    }
		
    if (tos_at_uart_read(&data, 1) != 1)
    {
        return;
    }
		 
    while (1)
    {
        if (tos_at_uart_read(&data, 1) != 1)
        {
            return;
        }

        if (data == '\r')
        {
            break;
        }
        data_len = data_len * 10 + (data - '0');
    }
		
    while (1)
    {
        if (tos_at_uart_read(&data, 1) != 1)
        {
            return;
        }

        if (data == ':')
        {
            break;
        }
}		
		
    do {
#define MIN(a, b)   ((a) < (b) ? (a) : (b))
        read_len = MIN(data_len, sizeof(buffer));
        if (tos_at_uart_read(buffer, read_len) != read_len) {
            return;
        }

        if (tos_at_channel_write(channel_id, buffer, read_len) <= 0) {
            return;
        }

        data_len -= read_len;
    } while (data_len > 0);

	return;
}

at_event_t m26_at_event[] = {
	{ "+RECEIVE:", m26_incoming_data_process},
};

sal_module_t sal_module_m26 = {
    .init           = m26_init,
    .connect        = m26_connect,
    .send           = m26_send,
    .recv_timeout   = m26_recv_timeout,
    .recv           = m26_recv,
    .close          = m26_close,
    .parse_domain   = m26_parse_domain,
};

int m26_sal_init(hal_uart_port_t uart_port)
{

    if (tos_at_init(uart_port, m26_at_event,
                        sizeof(m26_at_event) / sizeof(m26_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_m26) != 0) {
        return -1;
    }
    if (tos_sal_module_init() != 0) {
        return -1;
    }
		
    return 0;
}

