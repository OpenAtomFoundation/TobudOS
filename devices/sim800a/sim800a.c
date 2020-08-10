#include "sim800a.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

static int sim800a_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 2000, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}

static int sim800a_signal_quality_check(void)
{
    int rssi, ber;
    at_echo_t echo;
    char echo_buffer[32], *str;

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+CSQ\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    str = strstr(echo.buffer, "+CSQ:");
    if (!str) 
    {
        return -1;
    }
    sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
    if (rssi == 99 || ber == 99) {
        return -1;
    }

    return 0;
}

static int sim800a_send_mode_set(sal_send_mode_t mode)
{
    int try = 0;
    at_echo_t echo;

    while (try++ < 10) {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 2500, "AT+CIPMODE=%d\r\n", mode == SAL_SEND_MODE_NORMAL ? 0 : 1);
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int sim800a_multilink_set(sal_multilink_state_t state)
{
    int try = 0;
    at_echo_t echo;

    while (try++ < 10) {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 1000, "AT+CIPMUX=%d\r\n", state == SAL_MULTILINK_STATE_ENABLE ? 1 : 0);
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int sim800a_set_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 3000, "AT+CSTT=CMNET\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    tos_at_cmd_exec(&echo, 3000, "AT+CIICR\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    tos_at_cmd_exec(&echo, 3000, "AT+CIFSR\r\n");

    return 0;
}

static int sim800a_init(void)
{
    printf("Init sim800a ...\n" );

    if (sim800a_echo_close() != 0) {
        printf("echo close failed\n");
        return -1;
    }

    if (sim800a_signal_quality_check() != 0) {
        printf("signal quality check status failed\n");
        return -1;
    }

    if (sim800a_send_mode_set(SAL_SEND_MODE_NORMAL) != 0)  {
        printf("send mode set FAILED\n");
        return -1;
    }

    if (sim800a_multilink_set(SAL_MULTILINK_STATE_ENABLE) != 0) {
        printf("multilink set FAILED\n");
        return -1;
    }

    if (sim800a_set_apn() != 0) {
        printf("apn set FAILED\n");
        return -1;
    }

    printf("Init sim800a ok\n" );
    return 0;
}

static int sim800a_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;

    id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, NULL);

    tos_at_cmd_exec(&echo, 2000, "%s=\"B\"\r\n", "AT+CGCLASS");
    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_channel_free(id);
        return -1;
    }

    tos_at_cmd_exec(&echo, 2000, "%s=1,\"IP\",\"CMNET\"\r\n", "AT+CGDCONT");
    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_channel_free(id);
        return -1;
    }

    tos_at_cmd_exec(&echo, 2000, "%s=1\r\n", "AT+CGATT");
    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_channel_free(id);
        return -1;
    }

    tos_at_cmd_exec(&echo, 2000, "%s=1\r\n", "AT+CIPHEAD");
    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_channel_free(id);
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "CONNECT OK");
    tos_at_cmd_exec(&echo, 4000, "AT+CIPSTART=%d,\"%s\",\"%s\",%s\r\n",
                        id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, port);
    if (echo.status == AT_ECHO_STATUS_OK) {
        return id;
    }

    tos_at_channel_free(id);
    return -1;
}

static int sim800a_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int sim800a_recv(int id, void *buf, size_t len)
{
    return sim800a_recv_timeout(id, buf, len, (uint32_t)4000);
}

int sim800a_send(int id, const void *buf, size_t len)
{
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");
    tos_at_cmd_exec(&echo, 1000,
                            "AT+CIPSEND=%d,%d\r\n",
                            id, len);

    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "SEND OK");
    tos_at_raw_data_send(&echo, 1000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}

static void sim800a_transparent_mode_exit(void)
{
    tos_at_cmd_exec(NULL, 500, "+++");
}

static int sim800a_close(int id)
{
    sim800a_transparent_mode_exit();

    tos_at_cmd_exec(NULL, 1000, "AT+CIPCLOSE=%d\r\n", id);

    tos_at_channel_free(id);

    return 0;
}

static int sim800a_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    char *str;
    at_echo_t echo;
    char echo_buffer[64];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+CDNSGIP=\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    /*
    +CDNSGIP: 1,"www.xyz.com","xxx.xxx.xxx.xxx","xxx.xxx.xxx.xxx"
     */

    int seg1, seg2, seg3, seg4;
    str = strstr(echo.buffer, "+CDNSGIP: 1,");
    if (!str) 
    {
        return -1;
    }
    str += strlen("+CDNSGIP: 1,\"") + strlen(host_name) + 3;
    sscanf(str, "%d.%d.%d.%d", &seg1, &seg2, &seg3, &seg4);
    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", seg1, seg2, seg3, seg4);
    host_ip[host_ip_len - 1] = '\0';

    printf("GOT IP: %s\n", host_ip);

    return 0;
}

__STATIC__ void sim800a_incoming_data_process(void)
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
}

at_event_t sim800a_at_event[] = {
    { "+IPD,", sim800a_incoming_data_process },
};

sal_module_t sal_module_sim800a = {
    .init           = sim800a_init,
    .connect        = sim800a_connect,
    .send           = sim800a_send,
    .recv_timeout   = sim800a_recv_timeout,
    .recv           = sim800a_recv,
    .close          = sim800a_close,
    .parse_domain   = sim800a_parse_domain,
};

int sim800a_sal_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, sim800a_at_event,
                        sizeof(sim800a_at_event) / sizeof(sim800a_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_sim800a) != 0) {
        return -1;
    }
    if (tos_sal_module_init() != 0) {
        return -1;
    }

    return 0;
}

