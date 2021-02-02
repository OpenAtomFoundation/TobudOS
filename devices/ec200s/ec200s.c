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

#include "ec200s.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct ip_addr_st {
    uint8_t seg1;
    uint8_t seg2;
    uint8_t seg3;
    uint8_t seg4;
}ip_addr_t;

static ip_addr_t domain_parser_addr = {0};
static k_sem_t domain_parser_sem;
static k_sem_t module_ready_sem;

static int ec200s_check_ready(void)
{
    at_echo_t echo;
    
    tos_sem_create_max(&module_ready_sem, 0, 1);
    
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    } else {
        tos_sem_pend(&module_ready_sem, TOS_TIME_FOREVER);
        return 0;
    }   
}

static int ec200s_echo_close(void)
{
    at_echo_t echo;
    int try = 0;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }

    return -1;
}
static int ec200s_sim_card_check(void)
{
    at_echo_t echo;
    int try = 0;
    char echo_buffer[32];
	
    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CPIN?\r\n");
        if (strstr(echo_buffer, "READY")) {
            return 0;
        }
    }
    
    return -1;
}

static int ec200s_signal_quality_check(void)
{
    int rssi, ber;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;
	
    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CSQ\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
            return -1;
        }

        str = strstr(echo.buffer, "+CSQ:");
        if (!str) {
            return -1;
        }
        
        sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
        if (rssi != 99) {
            return 0;
        }
    }

    return -1;
}
static int ec200s_gsm_network_check(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;
	
    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CREG?\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
            return -1;
        }

        str = strstr(echo.buffer, "+CREG:");
        if (!str) {
            return -1;
        }
        sscanf(str, "+CREG:%d,%d", &n, &stat);
        if (stat == 1) {
            return 0;
        }
    }
    
    return -1;	
}

static int ec200s_gprs_network_check(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    while (try++ < 10) {
       
        tos_at_cmd_exec(&echo, 1000, "AT+CGREG?\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
            return -1;
        }

        str = strstr(echo.buffer, "+CGREG:");
        if (!str) {
            return -1;
        }
        sscanf(str, "+CGREG:%d,%d", &n, &stat);
        if (stat == 1) {
            return 0;
        }
    }
		
    return -1;
}

static int ec200s_close_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 3000, "AT+QIDEACT=1\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
   
    return -1;
}

static int ec200s_set_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+QICSGP=1,1,\"CMNET\"\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    tos_at_cmd_exec(&echo, 3000, "AT+QIACT=1\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    return 0;
}

static int ec200s_init(void)
{
    printf("Init ec200s ...\n" );
    
    if (ec200s_check_ready() != 0) {
        printf("wait module ready timeout, please check your module\n");
        return -1;
    }
    
    if (ec200s_echo_close() != 0) {
        printf("echo close failed,please check your module\n");
        return -1;
    }
		
    if(ec200s_sim_card_check() != 0) {
        printf("sim card check failed,please insert your card\n");
        return -1;
    }

    if (ec200s_signal_quality_check() != 0) {
        printf("signal quality check status failed\n");
        return -1;
    }
		
    if(ec200s_gsm_network_check() != 0) {
        printf("GSM network register status check fail\n");
        return -1;
    }
    
    if(ec200s_gprs_network_check() != 0) {
        printf("GPRS network register status check fail\n");
        return -1;
    }
    
    if(ec200s_close_apn() != 0) {
        printf("close apn failed\n");
        return -1;
    }
		
    if (ec200s_set_apn() != 0) {
        printf("apn set FAILED\n");
        return -1;
    }

    printf("Init ec200s ok\n" );
    return 0;
}

static int ec200s_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;
    char except_str[16];

    id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }
    
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+QICLOSE=%d\r\n", id);
    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_channel_free(id);
        return -1;
    }

    sprintf(except_str, "+QIOPEN: %d,0", id);
    tos_at_echo_create(&echo, NULL, 0, except_str);
    tos_at_cmd_exec_until(&echo, 4000, "AT+QIOPEN=1,%d,\"%s\",\"%s\",%d,0,1\r\n",
                        id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, atoi(port));
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_channel_free(id);
        return -1;
    }
    
    return id;
}

static int ec200s_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int ec200s_recv(int id, void *buf, size_t len)
{
    return ec200s_recv_timeout(id, buf, len, (uint32_t)4000);
}

int ec200s_send(int id, const void *buf, size_t len)
{
    at_echo_t echo;

    if (!tos_at_channel_is_working(id)) {
        return -1;
    }
	
    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");

    tos_at_cmd_exec_until(&echo, 1000, "AT+QISEND=%d,%d\r\n", id, len);

    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "SEND OK");
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}

int ec200s_recvfrom_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

int ec200s_recvfrom(int id, void *buf, size_t len)
{
    return ec200s_recvfrom_timeout(id, buf, len, (uint32_t)4000);
}

int ec200s_sendto(int id, char *ip, char *port, const void *buf, size_t len)
{
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");
    tos_at_cmd_exec_until(&echo, 1000, "AT+QISEND=%d,%d\r\n", id, len);

    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "SEND OK");
    tos_at_raw_data_send(&echo, 1000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}

static void ec200s_transparent_mode_exit(void)
{
    at_echo_t echo;
    
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 500, "+++");
}

static int ec200s_close(int id)
{
    at_echo_t echo;
    
    ec200s_transparent_mode_exit();

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+QICLOSE=%d\r\n", id);

    tos_at_channel_free(id);

    return 0;
}

static int ec200s_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    at_echo_t echo;
    char echo_buffer[128];

    tos_sem_create_max(&domain_parser_sem, 0, 1);

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+QIDNSGIP=1,\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    tos_sem_pend(&domain_parser_sem, TOS_TIME_FOREVER);
    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", domain_parser_addr.seg1, domain_parser_addr.seg2, domain_parser_addr.seg3, domain_parser_addr.seg4);
    host_ip[host_ip_len - 1] = '\0';

    printf("GOT IP: %s\n", host_ip);

    return 0;
}

__STATIC__ void ec200s_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, data_len = 0, read_len;
    uint8_t buffer[128];

    /*
		+QIURC: "recv",<sockid>,<datalen>
		<data content>
    */
	
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }
        if (data == ',') {
            break;
        }
        channel_id = channel_id * 10 + (data - '0');
    }

    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == '\r') {
            break;
        }
        data_len = data_len * 10 + (data - '0');
    }

    if (tos_at_uart_read(&data, 1) != 1) {
        return;
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

__STATIC__ void ec200s_domain_data_process(void)
{
    uint8_t data;

    /*
        +QIURC: "dnsgip",0,1,600

		+QIURC: "dnsgip","xxx.xxx.xxx.xxx"
    */

    if (tos_at_uart_read(&data, 1) != 1) {
        return;
    }

    if (data == '0') {
        return;
    }
    
    if (data == '\"') {
        /* start parser domain */
        while (1) {
            if (tos_at_uart_read(&data, 1) != 1) {
                return;
            }
            if (data == '.') {
                break;
            }
            domain_parser_addr.seg1 = domain_parser_addr.seg1 *10 + (data-'0');
        }
        while (1) {
            if (tos_at_uart_read(&data, 1) != 1) {
                return;
            }
            if (data == '.') {
                break;
            }
            domain_parser_addr.seg2 = domain_parser_addr.seg2 *10 + (data-'0');
        }
        while (1) {
            if (tos_at_uart_read(&data, 1) != 1) {
                return;
            }
            if (data == '.') {
                break;
            }
            domain_parser_addr.seg3 = domain_parser_addr.seg3 *10 + (data-'0');
        }
        while (1) {
            if (tos_at_uart_read(&data, 1) != 1) {
                return;
            }
            if (data == '\"') {
                break;
            }
            domain_parser_addr.seg4 = domain_parser_addr.seg4 *10 + (data-'0');
        }
        tos_sem_post(&domain_parser_sem);
    }
    return;

}

__STATIC__ void ec200s_ready_data_process(void)
{
    printf("mopdule ready\r\n");
    tos_sem_post(&module_ready_sem);
}

at_event_t ec200s_at_event[] = {
	{ "+QIURC: \"recv\",",   ec200s_incoming_data_process},
    { "+QIURC: \"dnsgip\",", ec200s_domain_data_process},
    { "RDY", ec200s_ready_data_process},
};

sal_module_t sal_module_ec200s = {
    .init           = ec200s_init,
    .connect        = ec200s_connect,
    .send           = ec200s_send,
    .recv_timeout   = ec200s_recv_timeout,
    .recv           = ec200s_recv,
    .sendto         = ec200s_sendto,
    .recvfrom       = ec200s_recvfrom,
    .recvfrom_timeout = ec200s_recvfrom_timeout,
    .close          = ec200s_close,
    .parse_domain   = ec200s_parse_domain,
};

int ec200s_sal_init(hal_uart_port_t uart_port)
{
    
    if (tos_at_init(uart_port, ec200s_at_event,
                        sizeof(ec200s_at_event) / sizeof(ec200s_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_ec200s) != 0) {
        return -1;
    }
    if (tos_sal_module_init() != 0) {
        return -1;
    }
		
    return 0;
}

int ec200s_sal_deinit()
{
    int id = 0;

    for (id = 0; id < AT_DATA_CHANNEL_NUM; ++id) {
        tos_sal_module_close(id);
    }

    tos_sal_module_register_default();

    tos_at_deinit();
    
    return 0;
}

