#include "sim7600ce.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

static int sim7600ce_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 2000, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}

static int sim7600ce_set_apn(void)
{
    int try = 0;
    at_echo_t echo;

    while (try++ < 10) {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 2500, "AT+CGDCONT=1,\"%s\",\"%s\"\r\n", "IP","CMNET");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int sim7600ce_signal_quality_check(void)
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
    sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
    if (rssi == 99 || ber == 99) {
        return -1;
    }

    return 0;
}

static int sim7600ce_net_status_check(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+CREG?\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    return 0;
}

static int sim7600ce_cpsi_status_check(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+CPSI?\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    return 0;
}
static int sim7600ce_send_mode_set(sal_send_mode_t mode)
{
    int try = 0;
    at_echo_t echo;

    while (try++ < 3) {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 2500, "AT+CIPMODE=%d\r\n", mode == SAL_SEND_MODE_NORMAL ? 0 : 1);
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int sim7600ce_cipccfg_set(void)
{
    int try = 0;
    at_echo_t echo;

    while (try++ < 10) {
        tos_at_echo_create(&echo, NULL, 0, NULL);
        tos_at_cmd_exec(&echo, 2500, "AT+CIPCCFG=10,0,0,1,1,0,500\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int sim7600ce_csocksetpn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+CSOCKSETPN=1\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    return 0;
}


static int sim7600ce_net_open(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+NETOPEN\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    return 0;
}


static int sim7600ce_init(void)
{
    printf("Init sim7600ce ...\n" );
    
    if (sim7600ce_echo_close() != 0) {
        printf("echo close failed\n");
        return -1;
    }
    if (sim7600ce_set_apn() != 0) {
        printf("apn set FAILED\n");
        return -1;
    }
    
    if (sim7600ce_signal_quality_check() != 0) {
        printf("signal quality check status failed\n");
        return -1;
    }
    if (sim7600ce_net_status_check() != 0) {
        printf("net_status_check status failed\n");
        return -1;
    }
    if (sim7600ce_cpsi_status_check() != 0) {
        printf("cpsi_status_check status failed\n");
        return -1;
    }
    if (sim7600ce_cipccfg_set() != 0)  {
        printf("sim7600ce_cipccfg_set FAILED\n");
        return -1;
   }
    if (sim7600ce_send_mode_set(SAL_SEND_MODE_TRANSPARENT) != 0)  {
        printf("send mode set FAILED\n");
        //return -1;
    }
    if (sim7600ce_csocksetpn() != 0) {
        printf("sim7600ce_net_open FAILED\n");
        return -1;
    }    
    
    if (sim7600ce_send_mode_set(SAL_SEND_MODE_NORMAL) != 0)  {
        printf("send mode set FAILED\n");
        return -1;
    }    
    
    if (sim7600ce_net_open() != 0) {
        printf("sim7600ce_net_open FAILED\n");
        return -1;
    }

    printf("Init sim7600ce ok\n" );
    return 0;
}

static int sim7600ce_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;

    id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, NULL);

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
    tos_at_cmd_exec(&echo, 4000, "AT+CIPOPEN=%d,\"%s\",\"%s\",%s\r\n",
                        id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, port);
    if (echo.status == AT_ECHO_STATUS_OK) {
        return id;
    }

    tos_at_channel_free(id);
    return -1;
}

static int sim7600ce_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int sim7600ce_recv(int id, void *buf, size_t len)
{
    return sim7600ce_recv_timeout(id, buf, len, (uint32_t)4000);
}

int sim7600ce_send(int id, const void *buf, size_t len)
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

static void sim7600ce_transparent_mode_exit(void)
{
    tos_at_cmd_exec(NULL, 500, "+++");
}

static int sim7600ce_close(int id)
{
    sim7600ce_transparent_mode_exit();

    tos_at_cmd_exec(NULL, 1000, "AT+CIPCLOSE=%d\r\n", id);

    tos_at_channel_free(id);

    return 0;
}

static int sim7600ce_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
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
    str += strlen("+CDNSGIP: 1,\"") + strlen(host_name) + 3;
    sscanf(str, "%d.%d.%d.%d", &seg1, &seg2, &seg3, &seg4);
    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", seg1, seg2, seg3, seg4);
    host_ip[host_ip_len - 1] = '\0';

    printf("GOT IP: %s\n", host_ip);

    return 0;
}

__STATIC__ uint8_t incoming_data_buffer[512];

__STATIC__ void sim7600ce_incoming_data_process(void)
{
    uint8_t data;
    uint8_t str[2];
    int channel_id = 0, data_len = 0;

    /*
    +RECEIVE,0,44:1234...

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
        if (tos_at_uart_read(str, 2) != 1) {
            return;
        }
        if (str[0] == '\r') {
            break;
        }
        data_len = data_len * 10 + (data - '0');
    }

    if (data_len > sizeof(incoming_data_buffer)) {
        data_len = sizeof(incoming_data_buffer);
    }

    if (tos_at_uart_read(incoming_data_buffer, data_len) != data_len) {
        return;
    }

    tos_at_channel_write(channel_id, incoming_data_buffer, data_len);
}

at_event_t sim7600ce_at_event[] = {
    { "+RECEIVE,", sim7600ce_incoming_data_process },
};

sal_module_t sal_module_sim7600ce = {
    .init           = sim7600ce_init,
    .connect        = sim7600ce_connect,
    .send           = sim7600ce_send,
    .recv_timeout   = sim7600ce_recv_timeout,
    .recv           = sim7600ce_recv,
    .close          = sim7600ce_close,
    .parse_domain   = sim7600ce_parse_domain,
};

int sim7600ce_sal_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, sim7600ce_at_event,
                        sizeof(sim7600ce_at_event) / sizeof(sim7600ce_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_sim7600ce) != 0) {
        return -1;
    }
    if (tos_sal_module_init() != 0) {
        return -1;
    }

    return 0;
}

