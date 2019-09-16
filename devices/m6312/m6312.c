#include "tos_at.h"
#include "m6312.h"
#include "sal_module_wrapper.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>


static int m6312_reset(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT\r\n");

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 20000, "AT+CMRESET\r\n");

    return 0;
}

static int m6312_wait_ok(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }

    return -1;
}

static int m6312_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }

    return -1;
}

static int m6312_pin_ready(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+CPIN: READY");
    tos_at_cmd_exec(&echo, 1000, "AT+CPIN?\r\n");
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }

    return -1;
}

static int m6312_activate(void) {
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 5) {
        tos_at_cmd_exec(&echo, 3000, "AT+CGACT=1,1\r\n");
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }

    return -1;
}

static int m6312_attach_gprs() {
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "1");
    tos_at_cmd_exec(&echo, 1000, "AT+CGATT=1\r\n");
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }

    return -1;
}

static int m6312_enable_head() {
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+CMHEAD=1\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }

    return -1;
}


static int m6312_reconnect_init(void)
{
    if (m6312_echo_close() != 0) {
        printf("m6312 echo close FAILED\n");
        return -1;
    }

    return 0;
}

static int m6312_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id;
    at_echo_t echo;

    m6312_reconnect_init();

    id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "CONNECT OK");
    tos_at_cmd_exec_until(&echo, 10000,
                        "AT+IPSTART=\"%s\",\"%s\",%s\r\n",
                        proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, port);
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        return id;
    }

    tos_at_channel_free(id);

    return -1;
}

static int m6312_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int m6312_recv(int id, void *buf, size_t len)
{
    return m6312_recv_timeout(id, buf, len, (uint32_t)4000);
}

static int m6312_is_link_broken(const char *echo_buffer)
{
    if (strstr(echo_buffer, "CLOSED") ||
        strstr(echo_buffer, "TIMEOUT")) {
        return 1;
    }

    return 0;
}


static int m6312_send(int id, const void *buf, size_t len)
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
    tos_at_cmd_exec(&echo, 3000, "AT+IPSEND=%d\r\n", len);
    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT) {
        if (m6312_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "SEND OK");

    /*
        ATTENTION: we should wait util "SEND OK" is echoed, otherwise the next
        time we execute at command, the m6312 maybe in a "busy s ..." state.
     */
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        if (m6312_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}

static int m6312_sendto(int id, char *ip, char *port, const void *buf, size_t len)
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
        tos_at_cmd_exec(&echo, 3000,
                        "AT+CIPSEND=%d,\"%s\",%s\r\n", len, ip, port);
    } else {
        tos_at_cmd_exec(&echo, 3000,
                        "AT+CIPSEND=%d\r\n", len);
    }

    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT) {
        if (m6312_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "SEND OK");

    /*
        ATTENTION: we should wait util "SEND OK" is echoed, otherwise the next
        time we execute at command, the m6312 maybe in a "busy s ..." state.
     */
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        if (m6312_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();
    return len;
}

static int m6312_recvfrom_timeout(int id, char *ip, char *port, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int m6312_recvfrom(int id, char *ip, char *port, void *buf, size_t len)
{
    return m6312_recvfrom_timeout(id, ip, port, buf, len, (uint32_t)4000);
}

static int m6312_close(int id)
{
    tos_at_cmd_exec(NULL, 1000, "AT+IPCLOSE\r\n");
    tos_at_channel_free(id);
    return 0;
}
static int m6312_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len) {
    char *str;
    at_echo_t echo;
    char echo_buffer[64];

    m6312_reconnect_init();

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 5000, "AT+CDNSGIP=\"%s\"\r\n", host_name);
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    /* +CDNSGIP: "1.2.3.4"\r\n */
    str = strstr((const char *)echo.buffer, "+CDNSGIP:");
    if (!str) {
        return -1;
    }

    sscanf(str, "+CDNSGIP: \"%s\"", host_ip);
    host_ip[host_ip_len - 1] = '\0';

    int iplen = strlen(host_ip);
    if (iplen > 1 && host_ip[iplen-1] == '"') {
        host_ip[iplen-1] = 0;
    }

    printf("GOT IP: %s\n", host_ip);
    return 0;
}

static int m6312_init(void)
{
    printf("Init M6312 ...\n" );

    if (m6312_reset() != 0) {
        printf("m6312 reset FAILED\n");
        return -1;
    }


    if (m6312_wait_ok() != 0) {
        printf("m6312 start FAILED\n");
        return -1;
    }

    if (m6312_echo_close() != 0) {
        printf("m6312 echo close FAILED\n");
        return -1;
    }

    if (m6312_pin_ready() != 0) {
        printf("m6312 pin not ready\n");
        return -1;
    }

    if (m6312_activate()) {
        printf("m6312 activate FAILED\n");
        return -1;
    }


    if (m6312_attach_gprs()) {
        printf("m6312 attach gprs FAILED\n");
        return -1;
    }

    if (m6312_enable_head()) {
        printf("m6312 enable recv head FAILED\n");
        return -1;
    }

    printf("Init M6312 Done\n" );
    return 0;
}

__STATIC__ uint8_t incoming_data_buffer[512];
__STATIC__ void m6312_incoming_data_process(void)
{
    // <IPDATA: 4>\r\nDATA\r\nOK\r\n
    //
    // "<IPDATA: "    prefix
    // "4"            data length
    // "\r\nOK\r\n"   suffix

    uint8_t data=0;
    int data_len = 0;
    int discard_suffix = 1;

    while (data != '\n') {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if(data >= '0' && data <= '9') {
            data_len = data_len * 10 + (data - '0');
        }
    }

    if (data_len > sizeof(incoming_data_buffer)) {
        discard_suffix = 0;
        data_len = sizeof(incoming_data_buffer);
    }

    if (tos_at_uart_read(incoming_data_buffer, data_len) != data_len) {
        return;
    }

    // discard suffix "\r\nOK\r\n"
    while(discard_suffix && (tos_at_uart_read_timed(&data, 1, 1000) == 1)) { }

    tos_at_channel_write(0, incoming_data_buffer, data_len);

}

at_event_t m6312_at_event[] = {
    { "<IPDATA: ", m6312_incoming_data_process },
};

sal_module_t sal_module_m6312 = {
    .init               = m6312_init,
    .connect            = m6312_connect,
    .send               = m6312_send,
    .recv_timeout       = m6312_recv_timeout,
    .recv               = m6312_recv,
    .sendto             = m6312_sendto,
    .recvfrom           = m6312_recvfrom,
    .recvfrom_timeout   = m6312_recvfrom_timeout,
    .close              = m6312_close,
    .parse_domain       = m6312_parse_domain,
};

int m6312_sal_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, m6312_at_event,
                        sizeof(m6312_at_event) / sizeof(m6312_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_m6312) != 0) {
        return -1;
    }

    if (tos_sal_module_init() != 0) {
        return -1;
    }

    return 0;
}

