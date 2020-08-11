#include "air724.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

static int air724_echo_close(void)
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
static int air724_cscon_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 300, "AT+CSCON=0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK)
    {
        return 0;
    }
    return -1;
}
static int air724_sim_card_check(void)
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

static int air724_signal_quality_check(void)
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
static int air724_gsm_network_check(void)
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

static int air724_gprs_network_check(void)
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

static int air724_close_apn(void)
{
    at_echo_t echo;
    char echo_buffer[32];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 3000, "AT+CIPSHUT\r\n");
   	
    if(strstr(echo.buffer, "SHUT OK") == NULL)
    {
        return -1;
    }

    return 0;
}

static int air724_send_mode_set(sal_send_mode_t mode)
{
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u	
    int try = 0;
    at_echo_t echo;

    while (try++ < 10)
    {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 300, "AT+CIPMODE=%d\r\n", mode == SAL_SEND_MODE_NORMAL ? 0 : 1);
        if (echo.status == AT_ECHO_STATUS_OK)
        {
            return 0;
        }
    }
    return -1;
#else
    return 0;
#endif
}

static int air724_multilink_set(sal_multilink_state_t state)
{
    int try = 0;
    at_echo_t echo;

    while (try++ < 10)
    {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 300, "AT+CIPMUX=%d\r\n", state == SAL_MULTILINK_STATE_ENABLE ? 1 : 0);
        if (echo.status == AT_ECHO_STATUS_OK)
        {
            return 0;
        }
    }
    return -1;
}

static int air724_set_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 300, "AT+CSTT=\"CMNET\"\r\n");
    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }

		
    tos_at_cmd_exec(&echo, 3000, "AT+CIICR\r\n");
    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }	
		
    tos_at_cmd_exec(&echo, 300, "AT+CIFSR\r\n");

    return 0;
}



static int air724_init(void)
{
    printf("Init air724 ...\n" );

    if (air724_echo_close() != 0)
    {
        printf("echo close failed\n");
        return -1;
    }
    if (air724_cscon_close() != 0)
    {
        printf("cscon close failed\n");
        return -1;
    }	
    if(air724_sim_card_check() != 0)
    {
        printf("sim card check failed,please insert your card\n");
        return -1;
    }

    if (air724_signal_quality_check() != 0)
    {
        printf("signal quality check status failed\n");
        return -1;
    }
		
    if(air724_gsm_network_check() != 0)
    {
        printf("GSM network register status check fail\n");
        return -1;
    }
    
    if(air724_gprs_network_check() != 0)
    {
        printf("GPRS network register status check fail\n");
        return -1;
    }
    
    if(air724_close_apn() != 0)
    {
        printf("close apn failed\n");
        return -1;
    }
    
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u		
    if (air724_multilink_set(SAL_MULTILINK_STATE_DISABLE) != 0)
    {
        printf("multilink set FAILED\n");
        return -1;
    }
    
#else
    if (air724_multilink_set(SAL_MULTILINK_STATE_ENABLE) != 0)
    {
        printf("multilink set FAILED\n");
        return -1;
    }
#endif

    if (air724_send_mode_set(SAL_SEND_MODE_NORMAL) != 0)
    {
        printf("send mode set FAILED\n");
        return -1;
    }	

    if (air724_set_apn() != 0) {
        printf("apn set FAILED\n");
        return -1;
    }

    printf("Init air724 ok\n" );
    return 0;
}

static int air724_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;

    id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }
		
    tos_at_echo_create(&echo, NULL, 0, NULL);	
    tos_at_cmd_exec(&echo, 2000, "%s=1\r\n", "AT+CIPHEAD");
    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_channel_free(id);
        return -1;
    }

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_echo_create(&echo, NULL, 0, "CONNECT OK");
    tos_at_cmd_exec(&echo, 4000, "AT+CIPSTART=%s,%s,%s\r\n",
                        proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, atoi(port));
    if (echo.status == AT_ECHO_STATUS_OK) {
        return id;
    }
#else
    tos_at_echo_create(&echo, NULL, 0, "CONNECT OK");
    tos_at_cmd_exec(&echo, 4000, "AT+CIPSTART=%d,%s,%s,%d\r\n",
                        id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, atoi(port));
    if (echo.status == AT_ECHO_STATUS_OK) {
        return id;
    }
#endif
		
    tos_at_channel_free(id);
    return -1;
}

static int air724_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int air724_recv(int id, void *buf, size_t len)
{
    return air724_recv_timeout(id, buf, len, (uint32_t)4000);
}

int air724_send(int id, const void *buf, size_t len)
{
    at_echo_t echo;
    char expect[10] = "";

    if (tos_at_global_lock_pend() != 0)
    {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");
		
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(&echo, 1000,
                            "AT+CIPSEND=%d\r\n",
                            len);
#else
    tos_at_cmd_exec(&echo, 1000,
                        "AT+CIPSEND=%d,%d\r\n",
                        id, len);
#endif

    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT)
    {
        tos_at_global_lock_post();
        return -1;
    }

    sprintf(expect, "%d, SEND OK", id);
    tos_at_echo_create(&echo, NULL, 0, expect);
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT)
    {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}

int air724_recvfrom_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

int air724_recvfrom(int id, void *buf, size_t len)
{
    return air724_recvfrom_timeout(id, buf, len, (uint32_t)4000);
}

int air724_sendto(int id, char *ip, char *port, const void *buf, size_t len)
{
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0)
    {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(&echo, 1000,
                            "AT+CIPSEND=%d\r\n",
                            len);
#else
    tos_at_cmd_exec(&echo, 1000,
                        "AT+CIPSEND=%d,%d\r\n",
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

static void air724_transparent_mode_exit(void)
{
    tos_at_cmd_exec(NULL, 500, "+++");
}

static int air724_close(int id)
{
    air724_transparent_mode_exit();

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(NULL, 1000, "AT+CIPCLOSE\r\n");
#else
    tos_at_cmd_exec(NULL, 1000, "AT+CIPCLOSE=%d\r\n", id);
#endif

    tos_at_channel_free(id);

    return 0;
}

static int air724_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    char *str;
    at_echo_t echo;
    char echo_buffer[128];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+CDNSGIP=\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK)
    {
        return -1;
    }

    /*
        +CDNSGIP: 1,"domain","xxx.xxx.xxx.xxx"
    */
    
    str = strstr(echo.buffer, ":");
    if (!str) 
    {
        return -1;
    }
    str += 1;
    if(*str == '0')
    {
        /* parse fail */
        return -1;
    }
    
    str += strlen(host_name) + 7;

    int seg1, seg2, seg3, seg4;
    sscanf(str, "%d.%d.%d.%d", &seg1, &seg2, &seg3, &seg4);
    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", seg1, seg2, seg3, seg4);
    host_ip[host_ip_len - 1] = '\0';

    printf("GOT IP: %s\n", host_ip);

    return 0;
}

__STATIC__ void air724_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, data_len = 0, read_len;
    uint8_t buffer[128];

    /*
        +RECEIVE,0,<data_len>:
        <data context>

        +RECEIVE: prefix
        0: scoket id
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
        data_len = data_len * 10 + (data - '0');
    }
		
    while (1)
    {
        if (tos_at_uart_read(&data, 1) != 1)
        {
            return;
        }

        if (data == '\n')
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

at_event_t air724_at_event[] = {
	{ "+RECEIVE,", air724_incoming_data_process},
};

sal_module_t sal_module_air724 = {
    .init           = air724_init,
    .connect        = air724_connect,
    .send           = air724_send,
    .recv_timeout   = air724_recv_timeout,
    .recv           = air724_recv,
    .sendto         = air724_sendto,
    .recvfrom       = air724_recvfrom,
    .recvfrom_timeout = air724_recvfrom_timeout,
    .close          = air724_close,
    .parse_domain   = air724_parse_domain,
};

int air724_sal_init(hal_uart_port_t uart_port)
{

    if (tos_at_init(uart_port, air724_at_event,
                        sizeof(air724_at_event) / sizeof(air724_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_air724) != 0) {
        return -1;
    }
    if (tos_sal_module_init() != 0) {
        return -1;
    }
		
    return 0;
}
