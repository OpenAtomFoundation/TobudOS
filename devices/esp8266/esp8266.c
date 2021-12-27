#include "tos_at.h"
#include "esp8266.h"
#include "sal_module_wrapper.h"

#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"

#define TOS_CFG_MODULE_SINGLE_LINK_EN       0u

static int esp8266_restore(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "ready");
    while (try++ < 10) {
        tos_at_cmd_exec_until(&echo, 5000, "AT+RESTORE\r\n");
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int esp8266_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}

static int esp8266_net_mode_set(esp8266_net_mode_t mode)
{
    int try = 0;
    char *cmd = NULL;
    at_echo_t echo;

    switch (mode) {
        case ESP8266_NET_MODE_STA:
            cmd = "AT+CWMODE=1\r\n";
            break;
        case ESP8266_NET_MODE_AP:
            cmd = "AT+CWMODE=2\r\n";
            break;
        case ESP8266_NET_MODE_STA_AP:
            cmd = "AT+CWMODE=3\r\n";
            break;
        default:
            return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, cmd);
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int esp8266_send_mode_set(esp8266_send_mode_t mode)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CIPMODE=%d\r\n", mode == ESP8266_SEND_MODE_NORMAL ? 0 : 1);
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int esp8266_multilink_set(esp8266_multilink_state_t state)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 500, "AT+CIPMUX=%d\r\n", state == ESP8266_MULTILINK_STATE_ENABLE ? 1 : 0);
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        } else if (echo.status == AT_ECHO_STATUS_ERROR) {
            return -1;
        }
    }
    return -1;
}

int esp8266_join_ap(const char *ssid, const char *pwd)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 15000, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

/*
    if we have got wifi unconnected once before, and now we get connected
    to ap again, we should set echo close and single/multilink again
 */
static int esp8266_reconnect_init(void)
{
    if (esp8266_echo_close() != 0) {
        printf("esp8266 echo close FAILED\n");
        return -1;
    }

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    if (esp8266_multilink_set(SAL_MULTILINK_STATE_DISABLE) != 0) {
        printf("esp8266 multilink set FAILED\n");
        return -1;
    }
#else
    if (esp8266_multilink_set(ESP8266_MULTILINK_STATE_ENABLE) != 0) {
        printf("esp8266 multilink set FAILED\n");
        return -1;
    }
#endif

    return 0;
}

static int esp8266_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;

    id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, NULL);
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(&echo, 10000,
                        "AT+CIPSTART=\"%s\",\"%s\",%s\r\n",
                        proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, port);
#else
    tos_at_cmd_exec(&echo, 10000,
                        "AT+CIPSTART=%d,\"%s\",\"%s\",%s\r\n",
                        id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, port);
#endif
    if (echo.status == AT_ECHO_STATUS_OK) {
        return id;
    }

    tos_at_channel_free(id);

    return -1;
}

static int esp8266_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int esp8266_recv(int id, void *buf, size_t len)
{
    return tos_at_channel_read(id, buf, len);
}

static int esp8266_is_link_broken(const char *echo_buffer)
{
    if (strstr(echo_buffer, "CLOSED") ||
        strstr(echo_buffer, "link is not valid")) {
        return 1;
    }

    return 0;
}

static int esp8266_send(int id, const void *buf, size_t len)
{
    at_echo_t echo;
    char echo_buffer[64];

    if (!tos_at_channel_is_working(id)) {
        return -1;
    }

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), ">");
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec_until(&echo, 5000,
                        "AT+CIPSEND=%d\r\n", len);
#else
    tos_at_cmd_exec_until(&echo, 5000,
                        "AT+CIPSEND=%d,%d\r\n",
                        id, len);
#endif
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        if (esp8266_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "SEND OK");

    /*
        ATTENTION: we should wait util "SEND OK" is echoed, otherwise the next
        time we execute at command, the esp8266 maybe in a "busy s ..." state.
     */
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        if (esp8266_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();
    return len;
}

static int esp8266_sendto(int id, char *ip, char *port, const void *buf, size_t len)
{
    at_echo_t echo;
    char echo_buffer[64];

    if (!tos_at_channel_is_working(id)) {
        return -1;
    }

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), ">");

    if (ip && port) {
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
        tos_at_cmd_exec_until(&echo, 1000,
                            "AT+CIPSEND=%d,\"%s\",%s\r\n", len, ip, port);
#else
        tos_at_cmd_exec_until(&echo, 1000,
                            "AT+CIPSEND=%d,%d,\"%s\",%s\r\n", id, len, ip, port);
#endif
    } else {
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
        tos_at_cmd_exec_until(&echo, 1000,
                            "AT+CIPSEND=%d\r\n", len);
#else
        tos_at_cmd_exec_until(&echo, 1000,
                            "AT+CIPSEND=%d,%d\r\n", id, len);
#endif
    }

    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        if (esp8266_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "SEND OK");

    /*
        ATTENTION: we should wait util "SEND OK" is echoed, otherwise the next
        time we execute at command, the esp8266 maybe in a "busy s ..." state.
     */
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        if (esp8266_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();
    return len;
}

static int esp8266_recvfrom_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int esp8266_recvfrom(int id, void *buf, size_t len)
{
    return esp8266_recvfrom_timeout(id, buf, len, (uint32_t)4000);
}

static int esp8266_close(int id)
{
    at_echo_t echo;
    
    tos_at_echo_create(&echo, NULL, 0, NULL);
    
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(&echo, 1000, "AT+CIPCLOSE\r\n");
#else
    tos_at_cmd_exec(&echo, 1000, "AT+CIPCLOSE=%d\r\n", id);
#endif
    tos_at_channel_free(id);
    return 0;
}

static int esp8266_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    char *str;
    at_echo_t echo;
    char echo_buffer[64];

    esp8266_reconnect_init();

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+CIPDOMAIN=\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    /*
    +CIPDOMAIN:183.232.231.172
     */
    str = strstr((const char *)echo.buffer, "+CIPDOMAIN:");
    if (!str) {
        return -1;
    }
    sscanf(str, "+CIPDOMAIN:%s", host_ip);
    host_ip[host_ip_len - 1] = '\0';
    printf("GOT IP: %s\n", host_ip);
    return 0;
}

static int esp8266_init(void)
{
    printf("Init ESP8266 ...\n" );

    if (esp8266_restore() != 0) {
        printf("esp8266 restore FAILED\n");
        return -1;
    }

    if (esp8266_echo_close() != 0) {
        printf("esp8266 echo close FAILED\n");
        return -1;
    }

    if (esp8266_net_mode_set(ESP8266_NET_MODE_STA) != 0) {
        printf("esp8266 net mode set FAILED\n");
        return -1;
    }

    if (esp8266_send_mode_set(ESP8266_SEND_MODE_NORMAL) != 0) {
        printf("esp8266 send mode set FAILED\n");
        return -1;
    }

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    if (esp8266_multilink_set(SAL_MULTILINK_STATE_DISABLE) != 0) {
        printf("esp8266 multilink set FAILED\n");
        return -1;
    }
#else
    if (esp8266_multilink_set(ESP8266_MULTILINK_STATE_ENABLE) != 0) {
        printf("esp8266 multilink set FAILED\n");
        return -1;
    }
#endif

    printf("Init ESP8266 Done\n" );
    return 0;
}

__STATIC__ void esp8266_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, data_len = 0, read_len;
    static uint8_t buffer[128];

    /*
    +IPD,0,44:1234...

    +IPD:       prefix
    0:          link id
    44:         data length
    1234...:    data content
    */
#if TOS_CFG_MODULE_SINGLE_LINK_EN == 0u
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }
        if (data == ',') {
            break;
        }
        channel_id = channel_id * 10 + (data - '0');
    }
#endif

    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }
        if (data == ':') {
            break;
        }
        data_len = data_len * 10 + (data - '0');
    }

    do {
#if !defined(MIN)
#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#endif
        read_len = MIN(data_len, sizeof(buffer));
        if (tos_at_uart_read(buffer, read_len) != read_len) {
            return;
        }
        
        //delay has two reason, wait for the data to be cached and untrigger scheduling
        tos_stopwatch_delay(200);

        if (tos_at_channel_write(channel_id, buffer, read_len) <= 0) {
            return;
        }

        data_len -= read_len;
    } while (data_len > 0);
}

at_event_t esp8266_at_event[] = {
    { "+IPD,", esp8266_incoming_data_process },
    { "PD,", esp8266_incoming_data_process },
};

sal_module_t sal_module_esp8266 = {
    .init               = esp8266_init,
    .connect            = esp8266_connect,
    .send               = esp8266_send,
    .recv_timeout       = esp8266_recv_timeout,
    .recv               = esp8266_recv,
    .sendto             = esp8266_sendto,
    .recvfrom           = esp8266_recvfrom,
    .recvfrom_timeout   = esp8266_recvfrom_timeout,
    .close              = esp8266_close,
    .parse_domain       = esp8266_parse_domain,
};

int esp8266_sal_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, esp8266_at_event,
                        sizeof(esp8266_at_event) / sizeof(esp8266_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_esp8266) != 0) {
        return -1;
    }

    if (tos_sal_module_init() != 0) {
        return -1;
    }

    return 0;
}

int esp8266_sal_deinit()
{
    int id = 0;

    for (id = 0; id < AT_DATA_CHANNEL_NUM; ++id) {
        tos_sal_module_close(id);
    }

    tos_sal_module_register_default();

    tos_at_deinit();
    
    return 0;
}

