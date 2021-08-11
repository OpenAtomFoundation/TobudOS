#include "tos_at.h"
#include "ml302.h"
#include "sal_module_wrapper.h"
#include "main.h"
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

static int ml302_wait_ok(void)
{

    int try = 0;

    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);

    while (try++ < 10)
        {
            tos_at_cmd_exec(&echo, 1000, "AT\r\n");

            if (echo.status == AT_ECHO_STATUS_OK)
            {
                return 0;
            }
        }

    return -1;
}

static int ml302_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");

    if (echo.status == AT_ECHO_STATUS_OK)
    {
        return 0;
    }

    return -1;
}

static int ml302_pin_ready(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+CPIN: READY");
    tos_at_cmd_exec(&echo, 1000, "AT+CPIN?\r\n");

    if (echo.status == AT_ECHO_STATUS_OK)
    {
        return 0;
    }

    return -1;
}

static int ml302_gsm_network_check(void)
{
    int stat;
    at_echo_t echo;
    char echo_buffer[32], *str;

    int try = 0;

    while (try++ < 10)
        {
            tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
            tos_at_cmd_exec(&echo, 1000, "AT+CFUN?\r\n");

            if (echo.status != AT_ECHO_STATUS_OK)
            {
                return -1;
            }

            str = strstr(echo.buffer, "+CFUN:");

            if (!str)
            {
                return -1;
            }

            sscanf(str, "+CFUN:%d", &stat);

            if (stat == 1)
            {
                return 0;
            }
        }

    return -1;
}

static int ml302_signal_quality_check(void)
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

            if (rssi != 99)
            {
                return 0;
            }
        }

    return -1;
}

static int ml302_set_gprs_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+CGDCONT=1,\"IP\",\"CMIOT\"\r\n");

    if (echo.status == AT_ECHO_STATUS_OK)
    {
        return 0;
    }

    return -1 ;
}

static int ml302_activate(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;

    int try = 0;

    while (try++ < 10)
        {
            tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
            tos_at_cmd_exec(&echo, 6000, "AT+CGACT=1,1\r\n");

            if (echo.status != AT_ECHO_STATUS_OK)
            {
                return -1;
            }

            str = strstr(echo.buffer, "+CGACT:");

            if (!str)
            {
                return -1;
            }

            sscanf(str, "+CGACT:%d,%d", &n, &stat);

            if (stat == 1)
            {
                return 0;
            }
        }

    return -1;
}

static int ml302_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;
    char except_str[16];

    id = tos_at_channel_alloc(ip, port);

    if (id == -1)
    {
        return -1;
    }

    sprintf(except_str, "%d,CONNECT OK", id);
    tos_at_echo_create(&echo, NULL, 0, except_str);
    tos_at_cmd_exec_until(&echo, 6000, "AT+MIPOPEN=%d,\"%s\",\"%s\",%d\r\n",
                          id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, atoi(port));

    if (echo.status != AT_ECHO_STATUS_EXPECT )
    {
        tos_at_channel_free(id);
        return -1;
    }

    return id;
}

static int ml302_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int ml302_recv(int id, void *buf, size_t len)
{
    return ml302_recv_timeout(id, buf, len, (uint32_t)4000);
}

int ml302_recvfrom_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

int ml302_recvfrom(int id, void *buf, size_t len)
{
    return ml302_recvfrom_timeout(id, buf, len, (uint32_t)4000);
}

int ml302_sendto(int id, char *ip, char *port, const void *buf, size_t len)
{
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0)
    {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");

    tos_at_cmd_exec_until(&echo, 1000, "AT+MIPSEND=%d,%d\r\n", id, len);

    if (echo.status != AT_ECHO_STATUS_EXPECT)
    {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "SEND OK");
    tos_at_raw_data_send(&echo, 1000, (uint8_t *)buf, len);

    if (echo.status != AT_ECHO_STATUS_EXPECT)
    {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}


int ml302_send(int id, const void *buf, size_t len)
{
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0)
    {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");

    tos_at_cmd_exec(&echo, 1000,
                    "AT+MIPSEND=%d,%d\r\n",
                    id, len);

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
    return 0 ;
}


static void ml302_transparent_mode_exit(void)
{
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(NULL, 500, "+++");
}


static int ml302_close(int id)
{
    at_echo_t echo;

    ml302_transparent_mode_exit();

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+MIPCLOSE=%d\r\n", id);

    tos_at_channel_free(id);

    return 0;
}


static int ml302_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    char *str;
    at_echo_t echo;
    char echo_buffer[64];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+MDNSGIP=\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }

    /*
    		+MDNSGIP: 0,"www.baidu.com","39.156.66.14","39.156.66.18"
     */

    int seg1, seg2, seg3, seg4;
    str = strstr(echo.buffer, "+MDNSGIP: 0,");

    if (!str)
    {
        return -1;
    }

    str += strlen("+MDNSGIP: 0,\"") + strlen(host_name) + 3;
    sscanf(str, "%d.%d.%d.%d", &seg1, &seg2, &seg3, &seg4);
    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", seg1, seg2, seg3, seg4);
    host_ip[host_ip_len - 1] = '\0';

    printf("GOT IP: %s\n", host_ip);

    return 0;
}

static int ml302_init(void)
{
    printf("Init ML302 ...\n" );

    if (ml302_wait_ok() != 0)
    {
        printf("ml302 start FAILED\n");
        return -1;
    }

    if (ml302_echo_close() != 0)
    {
        printf("ml302 echo close FAILED\n");
        return -1;
    }

    if (ml302_pin_ready() != 0)
    {
        printf("ml302 pin not ready\n");
        return -1;
    }

    if(ml302_gsm_network_check() != 0)
    {
        printf("ml302 GSM network register status check fail\n");
        return -1 ;
    }

    if (ml302_signal_quality_check() != 0)
    {
        printf("ml302 signal quality check status failed\n");
        return -1;
    }

    if (ml302_set_gprs_apn() != 0)
    {
        printf("ml302 set gprs_apn failed\n");
        return -1;
    }

    if (ml302_activate())
    {
        printf("ml302 activate FAILED\n");
        return -1;
    }

    printf("Init ML302 Done\n" );
    return 0;
}

__STATIC__ void ml302_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, data_len = 0, read_len;
    uint8_t buffer[128];

    /*
    	+MIPURC: "recv",<sockid>,<datalen>
    	<data content>
    */

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

    if (tos_at_uart_read(&data, 1) != 1)
    {
        return;
    }

    do
    {
#define MIN(a, b)   ((a) < (b) ? (a) : (b))
        read_len = MIN(data_len, sizeof(buffer));

        if (tos_at_uart_read(buffer, read_len) != read_len)
        {
            return;
        }

        if (tos_at_channel_write(channel_id, buffer, read_len) <= 0)
        {
            return;
        }

        data_len -= read_len;
    }
    while (data_len > 0);

    return;
}

at_event_t ml302_at_event[] =
{
    { "+MIPURC: \"recv\",", ml302_incoming_data_process },
};


sal_module_t sal_module_ml302 =
{
    .init               = ml302_init,
    .connect            = ml302_connect,
    .send               = ml302_send,
    .recv 							= ml302_recv,
    .recv_timeout       = ml302_recv_timeout,
    .sendto							=	ml302_sendto,
    .recvfrom						= ml302_recvfrom,
    .recvfrom_timeout   =	ml302_recvfrom_timeout,
    .close              = ml302_close,
    .parse_domain       = ml302_parse_domain,
};

int ml302_sal_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, ml302_at_event,
                    sizeof(ml302_at_event) / sizeof(ml302_at_event[0])) != 0)
    {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_ml302) != 0)
    {
        return -1;
    }

    if (tos_sal_module_init() != 0)
    {
        return -1;
    }

    return 0;
}

