/**
 * @brief    Quectel EC20 LTGE Cat4 Module
 * @author   Mculover666 <2412828003@qq.com>
 * @date     2020/05/07
*/

#include "ec20.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

static int ec20_echo_close(void)
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
static int ec20_sim_card_check(void)
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

static int ec20_signal_quality_check(void)
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
        sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
        if (rssi != 99) {
            return 0;
        }
    }

    return -1;
}
static int ec20_gsm_network_check(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;
	int try = 0;
	
	while (try++ < 10)
    {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
        tos_at_cmd_exec(&echo, 1000, "AT+CREG?\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
            return -1;
        }

        str = strstr(echo.buffer, "+CREG:");
        sscanf(str, "+CREG:%d,%d", &n, &stat);
        if (stat == 1) {
            return 0;
        }
	}
    return -1;	
}

static int ec20_gprs_network_check(void)
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
        sscanf(str, "+CGREG:%d,%d", &n, &stat);
        if (stat == 1)
        {
            return 0;
        }
	}
		
    return -1;	
}

static int ec20_close_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 3000, "AT+QIDEACT=1\r\n");
   	if (echo.status == AT_ECHO_STATUS_OK)
	{
		return 0;
	}
   
	return -1;
}

static int ec20_set_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 300, "AT+QICSGP=1,1,\"CMNET\"\r\n");
    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }


	tos_at_cmd_exec(&echo, 3000, "AT+QIACT=1\r\n");
    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }	

    return 0;
}

static int ec20_init(void)
{
    printf("Init ec20 ...\n" );

    if (ec20_echo_close() != 0)
    {
        printf("echo close failed\n");
        return -1;
    }
		
    if(ec20_sim_card_check() != 0)
    {
        printf("sim card check failed,please insert your card\n");
        return -1;
    }

    if (ec20_signal_quality_check() != 0)
    {
        printf("signal quality check status failed\n");
        return -1;
    }
		
    if(ec20_gsm_network_check() != 0)
    {
        printf("GSM network register status check fail\n");
        return -1;
    }
    
    if(ec20_gprs_network_check() != 0)
    {
        printf("GPRS network register status check fail\n");
        return -1;
    }
    
    if(ec20_close_apn() != 0)
    {
        printf("close apn failed\n");
        return -1;
    }
		
    if (ec20_set_apn() != 0) {
        printf("apn set FAILED\n");
        return -1;
    }

    printf("Init ec20 ok\n" );
    return 0;
}

static int ec20_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;

    id = tos_at_channel_alloc(ip, port);
    if (id == -1)
	{
        return -1;
    }

	tos_at_echo_create(&echo, NULL, 0, "CONNECT OK");
    tos_at_cmd_exec(&echo, 4000, "AT+QIOPEN=1,%d,\"%s\",\"%s\",%d,0,0\r\n",
                        id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, atoi(port));
    if (echo.status == AT_ECHO_STATUS_OK)
	{
        return id;
    }
		
    tos_at_channel_free(id);
    return -1;
}

static int ec20_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int ec20_recv(int id, void *buf, size_t len)
{
    return ec20_recv_timeout(id, buf, len, (uint32_t)4000);
}

int ec20_send(int id, const void *buf, size_t len)
{
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0)
	{
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");

	tos_at_cmd_exec(&echo, 1000, "AT+QISEND=%d,%d\r\n", id, len);

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

static void ec20_transparent_mode_exit(void)
{
    tos_at_cmd_exec(NULL, 500, "+++");
}

static int ec20_close(int id)
{
    ec20_transparent_mode_exit();

	tos_at_cmd_exec(NULL, 1000, "AT+QICLOSE=%d\r\n", id);

    tos_at_channel_free(id);

    return 0;
}

static int ec20_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    char *str;
    at_echo_t echo;
    char echo_buffer[128];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+QIDNSGIP=1,\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK)
	{
        return -1;
    }

    /*
        +QIURC: "dnsgip",0,1,600

		+QIURC: "dnsgip","xxx.xxx.xxx"
    */

    int seg1, seg2, seg3, seg4;
    str = strstr(echo.buffer, "dnsgip");
	*str = '0';
	str = strstr(echo.buffer, "dnsgip");
    str += strlen("dnsgip\",\"");
    sscanf(str, "%d.%d.%d.%d", &seg1, &seg2, &seg3, &seg4);
    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", seg1, seg2, seg3, seg4);
    host_ip[host_ip_len - 1] = '\0';

    printf("GOT IP: %s\n", host_ip);

    return 0;
}

__STATIC__ void ec20_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, data_len = 0, read_len;
    uint8_t buffer[128];

    /*
		+QIURC: "recv",0

        +QIURC:		prefix
        0:          scoket id
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

        if (data == '\r')
        {
            break;
        }
        channel_id = channel_id * 10 + (data - '0');
    }
	printf("channel id = %d\n", channel_id);
	
	tos_at_cmd_exec(NULL, 0, "AT+QIRD=%d,%d\r\n", channel_id, sizeof(buffer));
	
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
	printf("data_len = %d\n", data_len);

    if (tos_at_uart_read(&data, 1) != 1)
    {
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

at_event_t ec20_at_event[] = {
	{ "+QIURC: \"recv\"", ec20_incoming_data_process},
};

sal_module_t sal_module_ec20 = {
    .init           = ec20_init,
    .connect        = ec20_connect,
    .send           = ec20_send,
    .recv_timeout   = ec20_recv_timeout,
    .recv           = ec20_recv,
    .close          = ec20_close,
    .parse_domain   = ec20_parse_domain,
};

int ec20_sal_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, ec20_at_event,
                        sizeof(ec20_at_event) / sizeof(ec20_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_ec20) != 0) {
        return -1;
    }
    if (tos_sal_module_init() != 0) {
        return -1;
    }
		
    return 0;
}

