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

#include "l610.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include <stdio.h>

static int l610_check_ready(void)
{
    at_echo_t echo;
    int try = 0;
    
    while (++try) {
        tos_sleep_ms(2000);
        
        tos_at_echo_create(&echo, NULL, 0, NULL);
        
        tos_at_cmd_exec(&echo, 1000, "AT\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }

    return -1;
}

static int l610_echo_close(void)
{
    at_echo_t echo;
    int try = 0;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
        tos_sleep_ms(1000);
       
    }

    return -1;
}
static int l610_sim_card_check(void)
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
        tos_sleep_ms(2000);
    }
    
    return -1;
}

static int l610_signal_quality_check(void)
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
        tos_sleep_ms(2000);
    }

    return -1;
}
static int l610_gsm_network_check(void)
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
        tos_sleep_ms(2000);
    }
    return -1;	
}

static int l610_gprs_network_check(void)
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
        
        tos_sleep_ms(2000);
    }
		
    return -1;	
}

static int l610_set_data_format(void)
{
    at_echo_t echo;
    
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+GTSET=\"IPRFMT\",0\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }
    
    return 0;
}

static int l610_check_apn(void)
{
    at_echo_t echo;
    char buffer[64];
    char *str = NULL;
    int apn_state;
    
    tos_at_echo_create(&echo, buffer, 64, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+MIPCALL?\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }
    
    str = strstr(buffer, "+MIPCALL:");
    if (str == NULL) {
        return -1;
    }
    
    sscanf(str+strlen("+MIPCALL: "), "%d", &apn_state);
    if (apn_state == 0) {
        return 0;
    } else if (apn_state == 1) {
        return 1;
    } else {
        return -1;
    }
}

static int l610_check_close_apn(void)
{
    at_echo_t echo;
    char buffer[64];
    char *str = NULL;
    int apn_state;
    
    apn_state = l610_check_apn();
    
    if (apn_state == -1) {
        return -1;
    } else if (apn_state == 0) {
        return 0;
    }

    tos_at_echo_create(&echo, buffer, 64, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+MIPCALL=0\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }
    
    str = strstr(buffer, "+MIPCALL:");
    if (str == NULL) {
        return -1;
    }
    
    sscanf(str+strlen("+MIPCALL: "), "%d", &apn_state);
    if (apn_state == 0) {
        return 0;
    }
   
    return -1;
}

static int l610_set_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+MIPCALL");
    tos_at_cmd_exec_until(&echo, 10000, "AT+MIPCALL=1,\"CMNET\"\r\n");
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }
    
    if (l610_check_apn() == 1) {
        return 0;
    } else {
        return -1;
    }
}

static int l610_init(void)
{
    printf("Init l610 ...\n" );
    
    if (l610_check_ready() != 0) {
        printf("wait module ready timeout, please check your module\n");
        return -1;
    }
    
    if (l610_echo_close() != 0) {
        printf("echo close failed,please check your module\n");
        return -1;
    }
		
    if (l610_sim_card_check() != 0) {
        printf("sim card check failed,please insert your card\n");
        return -1;
    }

    if (l610_signal_quality_check() != 0) {
        printf("signal quality check status failed\n");
        return -1;
    }
		
    if (l610_gsm_network_check() != 0) {
        printf("GSM network register status check fail\n");
        return -1;
    }
    
    if (l610_gprs_network_check() != 0) {
        printf("GPRS network register status check fail\n");
        return -1;
    }
    
    if (l610_set_data_format() != 0) {
        printf("set data format fail\n");
        return -1;
    }
    
    if (l610_check_close_apn() != 0) {
        printf("close apn failed\n");
        return -1;
    }
		
    if (l610_set_apn() != 0) {
        printf("apn set FAILED\n");
        return -1;
    }

    printf("Init l610 ok\n" );
    return 0;
}

static int l610_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;

    id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "+MIPOPEN");
    tos_at_cmd_exec_until(&echo, 4000, "AT+MIPOPEN=%d,,\"%s\",%d,%d\r\n",
                        id + 1, ip, atoi(port), proto == TOS_SAL_PROTO_UDP ? 1 : 0);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_channel_free(id);
        return -1;
    }
    
    return id;
}

static int l610_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int l610_recv(int id, void *buf, size_t len)
{
    return l610_recv_timeout(id, buf, len, (uint32_t)4000);
}

int l610_send(int id, const void *buf, size_t len)
{
    at_echo_t echo;

    if (!tos_at_channel_is_working(id)) {
        return -1;
    }
	
    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }
    
    tos_at_echo_create(&echo, NULL, 0, ">");

    tos_at_cmd_exec_until(&echo, 1000, "AT+MIPSEND=%d,%d\r\n", id + 1, len);

    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "+MIPSEND");
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}

int l610_recvfrom_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

int l610_recvfrom(int id, void *buf, size_t len)
{
    return l610_recvfrom_timeout(id, buf, len, (uint32_t)4000);
}

int l610_sendto(int id, char *ip, char *port, const void *buf, size_t len)
{
    at_echo_t echo;

    if (!tos_at_channel_is_working(id)) {
        return -1;
    }
	
    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }
    
    tos_at_echo_create(&echo, NULL, 0, ">");

    tos_at_cmd_exec_until(&echo, 1000, "AT+MIPSEND=%d,%d\r\n", id + 1, len);

    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "+MIPSEND");
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}

static void l610_transparent_mode_exit(void)
{
    tos_at_cmd_exec(NULL, 500, "+++");
}

static int l610_close(int id)
{
    at_echo_t echo;
    
    l610_transparent_mode_exit();

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(NULL, 1000, "AT+MIPCLOSE=%d\r\n", id+1);

    tos_at_channel_free(id);

    return 0;
}

static int l610_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    at_echo_t echo;
    char echo_buffer[128];
    char *str = NULL;
    int seg1, seg2, seg3, seg4;
    
    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+MIPDNS=\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    str = strstr(echo_buffer, "+MIPDNS:");
    if (str == NULL) {
        return -1;
    }
    
    sscanf(str + strlen("+MIPDNS: ") + strlen(host_name) + 3, "%d.%d.%d.%d", &seg1, &seg2, &seg3, &seg4);
    
    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", seg1, seg2, seg3, seg4);
    host_ip[host_ip_len - 1] = '\0';

    printf("GOT IP: %s\n", host_ip);

    return 0;
}

__STATIC__ uint8_t __ascii2hex(char in) {
    if (('0' <= in) && (in <= '9')) {
        return in - '0';
    }
    if (('A' <= in) && (in <= 'F')) {
        return in - 'A' + 10;
    }
    if (('a' <= in) && (in <= 'f')) {
        return in - 'a' + 10;
    }
    return (uint8_t)-1;
}

__STATIC__ void __asciistr2hex(char *in, uint8_t *out, int len) {
    int i = 0;

    for (i = 0; i < len; i += 2) {
        out[i / 2] = (__ascii2hex(in[i]) << 4) + __ascii2hex(in[i + 1]);
    }
}

__STATIC__ char ascii_stream[1024];
__STATIC__ uint8_t hex_stream[512];

__STATIC__ void l610_tcp_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, leaf_data_len = 0, read_len;

    /*
		+MIPRTCP: <socket id>,<leaf length>,<hex string>
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
    channel_id--;

    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ',') {
            break;
        }
        leaf_data_len = leaf_data_len * 10 + (data - '0');
    }

    read_len = tos_at_uart_readline((uint8_t*)ascii_stream, sizeof(ascii_stream));
    
    read_len -= 2;
    ascii_stream[read_len] = '\0';
    __asciistr2hex(ascii_stream, hex_stream, read_len);

    tos_at_channel_write(channel_id, hex_stream, read_len/2);

    return;
}

__STATIC__ void l610_udp_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, leaf_data_len = 0, read_len;

    /*
		+MIPRUDP: <src ip>,<src port>, <socket id>,<leaf length>,<hex string>
    */
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ',') {
            break;
        }
    }
    
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ',') {
            break;
        }
    }
    
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ',') {
            break;
        }
        channel_id = channel_id * 10 + (data - '0');
    }
    channel_id--;

    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ',') {
            break;
        }
        leaf_data_len = leaf_data_len * 10 + (data - '0');
    }

    read_len = tos_at_uart_readline((uint8_t*)ascii_stream, sizeof(ascii_stream));
    
    read_len -= 2;
    ascii_stream[read_len] = '\0';
    __asciistr2hex(ascii_stream, hex_stream, read_len);

    tos_at_channel_write(channel_id, hex_stream, read_len/2);

    return;
}

at_event_t l610_at_event[] = {
	{ "+MIPRTCP: ", l610_tcp_incoming_data_process},
    { "+MIPRUDP: ", l610_udp_incoming_data_process},
};

sal_module_t sal_module_l610 = {
    .init           = l610_init,
    .connect        = l610_connect,
    .send           = l610_send,
    .recv_timeout   = l610_recv_timeout,
    .recv           = l610_recv,
    .sendto         = l610_sendto,
    .recvfrom       = l610_recvfrom,
    .recvfrom_timeout = l610_recvfrom_timeout,
    .close          = l610_close,
    .parse_domain   = l610_parse_domain,
};

int l610_sal_init(hal_uart_port_t uart_port)
{
    
    if (tos_at_init(uart_port, l610_at_event,
                        sizeof(l610_at_event) / sizeof(l610_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_l610) != 0) {
        return -1;
    }
    if (tos_sal_module_init() != 0) {
        return -1;
    }
		
    return 0;
}

int l610_sal_deinit()
{
    int id = 0;

    for (id = 0; id < AT_DATA_CHANNEL_NUM; ++id) {
        tos_sal_module_close(id);
    }

    tos_sal_module_register_default();

    tos_at_deinit();
    
    return 0;
}

