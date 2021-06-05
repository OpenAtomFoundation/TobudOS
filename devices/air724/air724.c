#include "air724.h"
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

static int air724_power_on(void)
{
#if AIR724_USE_PWRKEY_GPIO
    HAL_GPIO_WritePin(AIR724_PWR_GPIO_PORT, AIR724_PWR_GPIO_PIN, GPIO_PIN_RESET);
    tos_sleep_ms(1000);
    HAL_GPIO_WritePin(AIR724_PWR_GPIO_PORT, AIR724_PWR_GPIO_PIN, GPIO_PIN_SET);
#else
    tos_sleep_ms(1000);
#endif
    
    return 0;
}

static int air724_check_ready(void)
{
    at_echo_t echo;
    int try = 0;
    
    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}


static int air724_echo_close(void)
{
    at_echo_t echo;
    int try = 0;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    
    while(try++ < 10) {
        tos_at_cmd_exec(&echo, 300, "ATE0\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int air724_cscon_close(void)
{
    at_echo_t echo;
    int try = 0;

    tos_at_echo_create(&echo, NULL, 0, NULL);
     while  (try++ < 10) {
        tos_at_cmd_exec(&echo, 300, "AT+CSCON=0\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
        tos_sleep_ms(1000);
    }
    return -1;
}
static int air724_sim_card_check(void)
{
    at_echo_t echo;
    int try = 0;
    char echo_buffer[32];

    while (try++ < 10) {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
        tos_at_cmd_exec(&echo, 2000, "AT+CPIN?\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
                return -1;
        }

        if(strstr(echo.buffer, "READY")) {
            return 0;
        }
        
        tos_sleep_ms(1000);
    }
		 
    return -1;
}

static int air724_signal_quality_check(void)
{
    int rssi, ber;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;
	
    while (try++ < 10) {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
        tos_at_cmd_exec(&echo, 1000, "AT+CSQ\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
            return -1;
        }

        str = strstr(echo.buffer, "+CSQ:");
        if (!str)  {
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
static int air724_gsm_network_check(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;
	
    while (try++ < 10) {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
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

static int air724_gprs_network_check(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;

    while (try++ < 10) {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
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
        
        tos_sleep_ms(1000);
    }
		
    return -1;	
}

static int air724_close_apn(void)
{
    at_echo_t echo;
    char echo_buffer[32];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 3000, "AT+CIPSHUT\r\n");
   	
    if(strstr(echo.buffer, "SHUT OK") == NULL) {
        return -1;
    }
    
    tos_sleep_ms(2000);

    return 0;
}

static int air724_send_mode_set(air724_send_mode_t mode)
{
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u	
    int try = 0;
    at_echo_t echo;

    while (try++ < 10) {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 300, "AT+CIPMODE=%d\r\n", mode == SAL_SEND_MODE_NORMAL ? 0 : 1);
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
#else
    return 0;
#endif
}

static int air724_multilink_set(air724_multilink_state_t state)
{
    int try = 0;
    at_echo_t echo;

    while (try++ < 10) {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 300, "AT+CIPMUX=%d\r\n", state == AIR724_MULTILINK_STATE_ENABLE ? 1 : 0);
        if (echo.status == AT_ECHO_STATUS_OK) {
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
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

	tos_at_echo_create(&echo, NULL, 0, NULL);	
    tos_at_cmd_exec(&echo, 50000, "AT+CIICR\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }	
	
    tos_at_echo_create(&echo, NULL, 0, NULL);	
    tos_at_cmd_exec(&echo, 300, "AT+CIFSR\r\n");

    return 0;
}



static int air724_init(void)
{
    printf("Init air724 ...\n" );
    
    air724_power_on();
    tos_sleep_ms(2000);
    printf("module power on\n");
    
    if (air724_check_ready() != 0) {
        printf("wait module ready timeout, please check your module\n");
        return -1;
    }

    if (air724_echo_close() != 0) {
        printf("echo close failed\n");
        return -1;
    }
    if (air724_cscon_close() != 0) {
        printf("cscon close failed\n");
        return -1;
    }	
    if(air724_sim_card_check() != 0) {
        printf("sim card check failed,please insert your card\n");
        return -1;
    }

    if (air724_signal_quality_check() != 0) {
        printf("signal quality check status failed\n");
        return -1;
    }
		
    if(air724_gsm_network_check() != 0) {
        printf("GSM network register status check fail\n");
        return -1;
    }
    
    if(air724_gprs_network_check() != 0) {
        printf("GPRS network register status check fail\n");
        return -1;
    }
    
    if(air724_close_apn() != 0) {
        printf("close apn failed\n");
        return -1;
    }
    
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u		
    if (air724_multilink_set(SAL_MULTILINK_STATE_DISABLE) != 0) {
        printf("multilink set FAILED\n");
        return -1;
    }
    
#else
    if (air724_multilink_set(AIR724_MULTILINK_STATE_ENABLE) != 0) {
        printf("multilink set FAILED\n");
        return -1;
    }
#endif

    if (air724_send_mode_set(AIR724_SEND_MODE_NORMAL) != 0) {
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
    tos_at_cmd_exec(&echo, 2000, "AT+CIPHEAD=1\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_channel_free(id);
        return -1;
    }

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_echo_create(&echo, NULL, 0, "CONNECT OK");
    tos_at_cmd_exec_until(&echo, 4000, "AT+CIPSTART=%s,%s,%s\r\n",
                        proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, atoi(port));
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_channel_free(id);
        return -1;
    }
#else
    char result[20];
    sprintf(result, "%d, CONNECT OK", id);
    tos_at_echo_create(&echo, NULL, 0, result);
    tos_at_cmd_exec_until(&echo, 8000, "AT+CIPSTART=%d,%s,%s,%d\r\n",
                        id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, atoi(port));
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_channel_free(id);
        return -1;
    }
#endif
    
    return id;
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
    char expect_str[10];

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");
		
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec_until(&echo, 1000,
                            "AT+CIPSEND=%d\r\n",
                            len);
#else
    tos_at_cmd_exec_until(&echo, 1000,
                        "AT+CIPSEND=%d,%d\r\n",
                        id, len);
#endif

    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    sprintf(expect_str, "%d, SEND OK", id);
    tos_at_echo_create(&echo, NULL, 0, expect_str);
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
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

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec_until(&echo, 1000,
                            "AT+CIPSEND=%d\r\n",
                            len);
#else
    tos_at_cmd_exec_until(&echo, 1000,
                        "AT+CIPSEND=%d,%d\r\n",
                        id, len);
#endif

    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "SEND OK");
    tos_at_raw_data_send_until(&echo, 1000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
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
    at_echo_t echo;
    
    air724_transparent_mode_exit();
    
    tos_at_echo_create(&echo, NULL, 0, NULL);

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(&echo, 1000, "AT+CIPCLOSE\r\n");
#else
    tos_at_cmd_exec(&echo, 1000, "AT+CIPCLOSE=%d\r\n", id);
#endif

    tos_at_channel_free(id);

    return 0;
}

static int air724_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    at_echo_t echo;
    
    tos_sem_create_max(&domain_parser_sem, 0, 1);

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+CDNSGIP=\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }
    
    tos_sem_pend(&domain_parser_sem, TOS_TIME_FOREVER);

    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", domain_parser_addr.seg1, domain_parser_addr.seg2, domain_parser_addr.seg3, domain_parser_addr.seg4);
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
    if (tos_at_uart_read(&data, 1) != 1) {
        return;
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
		 
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ':') {
            break;
        }
        data_len = data_len * 10 + (data - '0');
    }
		
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == '\n') {
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

__STATIC__ void air724_domain_data_process(void)
{
    uint8_t data;
    int retcode = 0;
    
    /*
        +CDNSGIP: 1,"domain","xxx.xxx.xxx.xxx"
    */

    if (tos_at_uart_read(&data, 1) != 1) {
        return;
    }
    
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ',') {
            break;
        }
        
        retcode = retcode * 10 + (data - '0');
    }
    
    if (retcode != 1) {
        return;
    }
    
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ',') {
            break;
        }
    }		

    if (tos_at_uart_read(&data, 1) != 1) {
        return;
    }
    
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
    return;

}

at_event_t air724_at_event[] = {
	{ "+RECEIVE,", air724_incoming_data_process},
    { "+CDNSGIP:", air724_domain_data_process},
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
