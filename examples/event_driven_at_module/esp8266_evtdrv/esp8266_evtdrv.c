#include "tos_at_evtdrv.h"
#include "esp8266_evtdrv.h"
#include "sal_module_wrapper.h"

#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"

typedef struct esp8266_send_info_st {
    int                 sockid;
    void               *buf;
    size_t              len;
} esp8266_send_info_t;

typedef struct esp8266_domain_parse_info_st {
    char               *host_ip;
    size_t              host_ip_len;
} esp8266_domain_parse_info_t;

typedef struct esp8266_context_st {
    at_echo_t           echo;
    char                echo_buffer[64];
    int                 try;
    evtdrv_task_id_t      self_task_id;
    int                 sockid;
    evtdrv_task_id_t      caller_task_id;
    char               *ssid;
    char               *pwd;
    esp8266_send_info_t send_info;
    esp8266_domain_parse_info_t domain_parse_info;
} esp8266_context_t;

esp8266_context_t esp8266_context;

#define CONTEXT(field)  (esp8266_context.##field)

static void esp8266_restore(void)
{
    tos_at_echo_create(&CONTEXT(echo), NULL, 0, NULL, CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_RESTORE);
    tos_at_cmd_exec(&CONTEXT(echo), 3000, "AT+RESTORE\r\n");
}

static void esp8266_echo_close(void)
{
    tos_at_echo_create(&CONTEXT(echo), NULL, 0, NULL, CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_ECHO_CLOSE);
    tos_at_cmd_exec(&CONTEXT(echo), 1000, "ATE0\r\n");
}

static void esp8266_net_mode_set(sal_net_mode_t mode)
{
    char *cmd = NULL;

    switch (mode) {
        case SAL_NET_MODE_STA:
            cmd = "AT+CWMODE=1\r\n";
            break;
        case SAL_NET_MODE_AP:
            cmd = "AT+CWMODE=2\r\n";
            break;
        case SAL_NET_MODE_STA_AP:
            cmd = "AT+CWMODE=3\r\n";
            break;
        default:
            return;
    }

    tos_at_echo_create(&CONTEXT(echo), NULL, 0, "no change", CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_NET_MODE_SET);
    tos_at_cmd_exec(&CONTEXT(echo), 1000, cmd);
}

static void esp8266_send_mode_set(sal_send_mode_t mode)
{
    tos_at_echo_create(&CONTEXT(echo), NULL, 0, NULL, CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_SEND_MODE_SET);
    tos_at_cmd_exec(&CONTEXT(echo), 1000, "AT+CIPMODE=%d\r\n", mode == SAL_SEND_MODE_NORMAL ? 0 : 1);
}

static void esp8266_multilink_set(sal_multilink_state_t state)
{
    tos_at_echo_create(&CONTEXT(echo), NULL, 0, NULL, CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_MULTILINK_SET);
    tos_at_cmd_exec(&CONTEXT(echo), 500, "AT+CIPMUX=%d\r\n", state == SAL_MULTILINK_STATE_ENABLE ? 1 : 0);
}

static void esp8266_do_join_ap(const char *ssid, const char *pwd)
{
    tos_at_echo_create(&CONTEXT(echo), NULL, 0, "OK", CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_JOIN_AP);
    tos_at_cmd_exec(&CONTEXT(echo), 1000, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
}

static int esp8266_do_connect(const char *ip, const char *port, sal_proto_t proto)
{
    CONTEXT(sockid) = tos_at_channel_alloc(ip, port);
    if (CONTEXT(sockid) == -1) {
        return -1;
    }

    tos_at_echo_create(&CONTEXT(echo), NULL, 0, "OK", CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_CONNECT);
    tos_at_cmd_exec(&CONTEXT(echo), 10000,
                        "AT+CIPSTART=%d,\"%s\",\"%s\",%s\r\n",
                        CONTEXT(sockid), proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, port);

    return 0;
}

static int esp8266_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int esp8266_recv(int id, void *buf, size_t len)
{
    return esp8266_recv_timeout(id, buf, len, (uint32_t)4000);
}

static int esp8266_is_link_broken(const char *echo_buffer)
{
    if (strstr(echo_buffer, "CLOSED") ||
        strstr(echo_buffer, "link is not valid")) {
        return 1;
    }

    return 0;
}

static int esp8266_do_send(int id, const void *buf, size_t len)
{
    tos_at_echo_create(&CONTEXT(echo), CONTEXT(echo_buffer), sizeof(CONTEXT(echo_buffer)), "SEND OK", CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_SEND);

    /*
        ATTENTION: we should wait util "SEND OK" is echoed, otherwise the next
        time we execute at command, the esp8266 maybe in a "busy s ..." state.
     */
    tos_at_raw_data_send(&CONTEXT(echo), 10000, (uint8_t *)buf, len);

    return 0;
}

static int esp8266_do_send_ready(int id, const void *buf, size_t len)
{
    if (!tos_at_channel_is_working(id)) {
        return -1;
    }

    CONTEXT(sockid) = id;

    tos_at_echo_create(&CONTEXT(echo), CONTEXT(echo_buffer), sizeof(CONTEXT(echo_buffer)), ">", CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_SEND_READY);
    tos_at_cmd_exec(&CONTEXT(echo), 1000,
                        "AT+CIPSEND=%d,%d\r\n",
                        id, len);

    return 0;
}

static int esp8266_do_close(int id)
{
    tos_at_cmd_exec(NULL, 1000, "AT+CIPCLOSE=%d\r\n", id);
    tos_at_channel_free(id);
    return 0;
}

static int esp8266_close(int id)
{
    esp8266_imsg_close_t *msg_close;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(esp8266_imsg_close_t));
    msg_close = (esp8266_imsg_close_t *)msg_body;

    msg_close->header.event     = EVENT_IMSG_ESP8266_CLOSE;
    msg_close->header.sender_id = tos_evtdrv_task_self();

    msg_close->sockid           = id;

    if (tos_evtdrv_msg_send(CONTEXT(self_task_id), (evtdrv_msg_body_t)msg_close) != EVTDRV_ERR_NONE) {
        return -1;
    }

    return 0;
}

static int esp8266_send(int id, const void *buf, size_t len)
{
    esp8266_imsg_send_t *msg_send;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(esp8266_imsg_send_t));
    msg_send = (esp8266_imsg_send_t *)msg_body;

    msg_send->header.event      = EVENT_IMSG_ESP8266_SEND;
    msg_send->header.sender_id  = tos_evtdrv_task_self();

    msg_send->sockid            = id;
    msg_send->buf               = (void *)buf;
    msg_send->len               = len;

    if (tos_evtdrv_msg_send(CONTEXT(self_task_id), (evtdrv_msg_body_t)msg_send) != EVTDRV_ERR_NONE) {
        return -1;
    }

    return 0;
}

static int esp8266_connect(const char *ip, const char *port, sal_proto_t proto)
{
    esp8266_imsg_connect_t *msg_connect;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(esp8266_imsg_connect_t));
    msg_connect = (esp8266_imsg_connect_t *)msg_body;

    msg_connect->header.event       = EVENT_IMSG_ESP8266_CONNECT;
    msg_connect->header.sender_id   = tos_evtdrv_task_self();

    msg_connect->ip                 = (char *)ip;
    msg_connect->port               = (char *)port;
    msg_connect->proto              = proto;

    if (tos_evtdrv_msg_send(CONTEXT(self_task_id), (evtdrv_msg_body_t)msg_connect) != EVTDRV_ERR_NONE) {
        return -1;
    }

    return 0;
}

static int esp8266_do_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    tos_at_echo_create(&CONTEXT(echo), CONTEXT(echo_buffer), sizeof(CONTEXT(echo_buffer)), NULL, CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_DOMAIN_PARSE);
    tos_at_cmd_exec(&CONTEXT(echo), 2000, "AT+CIPDOMAIN=\"%s\"\r\n", host_name);

    return 0;
}

static int esp8266_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    esp8266_imsg_domain_parse_t *msg_domain_parse;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(esp8266_imsg_domain_parse_t));
    msg_domain_parse = (esp8266_imsg_domain_parse_t *)msg_body;

    msg_domain_parse->header.event      = EVENT_IMSG_ESP8266_DOMAIN_PARSE;
    msg_domain_parse->header.sender_id  = tos_evtdrv_task_self();

    msg_domain_parse->host_name         = (char *)host_name;
    msg_domain_parse->host_ip           = (char *)host_ip;
    msg_domain_parse->host_ip_len       = host_ip_len;

    if (tos_evtdrv_msg_send(CONTEXT(self_task_id), (evtdrv_msg_body_t)msg_domain_parse) != EVTDRV_ERR_NONE) {
        return -1;
    }

    return 0;
}

static int esp8266_init(void)
{
    if (tos_evtdrv_event_set(CONTEXT(self_task_id), EVENT_INTERNAL_ESP8266_INIT) != EVTDRV_ERR_NONE) {
        return -1;
    }

    return 0;
}

__STATIC__ uint8_t incoming_data_buffer[512];

__STATIC__ void esp8266_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, data_len = 0;

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

    if (data_len > sizeof(incoming_data_buffer)) {
        data_len = sizeof(incoming_data_buffer);
    }

    if (tos_at_uart_read(incoming_data_buffer, data_len) != data_len) {
        return;
    }

    tos_at_channel_write(channel_id, incoming_data_buffer, data_len);
}

at_event_t esp8266_at_event[] = {
    { "+IPD,", esp8266_incoming_data_process },
};

sal_module_t sal_module_esp8266 = {
    .init               = esp8266_init,
    .connect            = esp8266_connect,
    .send               = esp8266_send,
    .recv_timeout       = esp8266_recv_timeout,
    .recv               = esp8266_recv,
    .close              = esp8266_close,
    .parse_domain       = esp8266_parse_domain,
};

int esp8266_join_ap(const char *ssid, const char *pwd)
{
    esp8266_imsg_join_ap_t *msg_join_ap;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(esp8266_imsg_join_ap_t));
    msg_join_ap = (esp8266_imsg_join_ap_t *)msg_body;

    msg_join_ap->header.event       = EVENT_IMSG_ESP8266_JOIN_AP;
    msg_join_ap->header.sender_id   = tos_evtdrv_task_self();

    msg_join_ap->ssid               = (char *)ssid;
    msg_join_ap->pwd                = (char *)pwd;

    if (tos_evtdrv_msg_send(CONTEXT(self_task_id), (evtdrv_msg_body_t)msg_join_ap) != EVTDRV_ERR_NONE) {
        return -1;
    }

    return 0;
}

int esp8266_sal_init(hal_uart_port_t uart_port, evtdrv_task_id_t at_task_id, evtdrv_task_id_t esp8266_task_id)
{
    esp8266_imsg_init_t *msg_init;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(esp8266_imsg_init_t));
    msg_init = (esp8266_imsg_init_t *)msg_body;

    msg_init->header.event      = EVENT_IMSG_ESP8266_INIT;
    msg_init->header.sender_id  = tos_evtdrv_task_self();

    msg_init->uart_port         = uart_port;
    msg_init->at_task_id        = at_task_id;
    msg_init->esp8266_task_id   = esp8266_task_id;

    if (tos_evtdrv_msg_send(esp8266_task_id, (evtdrv_msg_body_t)msg_init) != EVTDRV_ERR_NONE) {
        return -1;
    }

    return 0;
}

static void esp8266_send_msg_reply(int write_len)
{
    esp8266_omsg_send_t *omsg_send;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(esp8266_omsg_send_t));

    omsg_send = (esp8266_omsg_send_t *)msg_body;

    omsg_send->header.event         = EVENT_OMSG_ESP8266_SEND;
    omsg_send->header.sender_id     = tos_evtdrv_task_self();

    omsg_send->write_len            = write_len;

    tos_evtdrv_msg_send(CONTEXT(caller_task_id), (evtdrv_msg_body_t)omsg_send);
}

static void esp8266_connect_msg_reply(int sockid)
{
    esp8266_omsg_connect_t *omsg_connect;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(esp8266_omsg_connect_t));

    omsg_connect = (esp8266_omsg_connect_t *)msg_body;

    omsg_connect->header.event      = EVENT_OMSG_ESP8266_CONNECT;
    omsg_connect->header.sender_id  = tos_evtdrv_task_self();

    omsg_connect->sockid            = sockid;

    tos_evtdrv_msg_send(CONTEXT(caller_task_id), (evtdrv_msg_body_t)omsg_connect);
}

static void esp8266_common_msg_reply(evtdrv_event_flag_t event_flag, int return_code)
{
    esp8266_omsg_common_t *omsg_common;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(esp8266_omsg_common_t));

    omsg_common = (esp8266_omsg_common_t *)msg_body;

    omsg_common->header.event       = event_flag;
    omsg_common->header.sender_id   = tos_evtdrv_task_self();

    omsg_common->return_code        = return_code;

    tos_evtdrv_msg_send(CONTEXT(caller_task_id), (evtdrv_msg_body_t)omsg_common);
}

evtdrv_event_flag_t esp8266_task(evtdrv_event_flag_t event)
{
    esp8266_event_hdr_t *event_hdr;
    esp8266_imsg_init_t *imsg_init;
    esp8266_imsg_join_ap_t *imsg_join_ap;
    esp8266_imsg_connect_t *imsg_connect;
    esp8266_imsg_send_t *imsg_send;
    esp8266_imsg_close_t *imsg_close;
    esp8266_imsg_domain_parse_t *imsg_domain_parse;

    if (event & TOS_EVTDRV_SYS_EVENT_INIT) {
        return TOS_EVTDRV_SYS_EVENT_INIT;
    }

    if (event & TOS_EVTDRV_SYS_EVENT_MSG) {
        while (event_hdr = (esp8266_event_hdr_t *)tos_evtdrv_msg_recv()) {
            switch (event_hdr->event) {
                case EVENT_IMSG_ESP8266_INIT:
                    imsg_init = (esp8266_imsg_init_t *)event_hdr;

                    CONTEXT(caller_task_id) = event_hdr->sender_id;
                    CONTEXT(self_task_id)   = imsg_init->esp8266_task_id;
                    CONTEXT(try)            = 0;

                    if (tos_at_init(imsg_init->uart_port, imsg_init->at_task_id, esp8266_at_event,
                                        sizeof(esp8266_at_event) / sizeof(esp8266_at_event[0])) != 0) {
                        esp8266_common_msg_reply(EVENT_OMSG_ESP8266_INIT, -1);
                        break;
                    }

                    if (tos_sal_module_register(&sal_module_esp8266) != 0) {
                        esp8266_common_msg_reply(EVENT_OMSG_ESP8266_INIT, -1);
                        break;
                    }

                    tos_sal_module_init();

                    break;

                case EVENT_IMSG_ESP8266_JOIN_AP:
                    imsg_join_ap = (esp8266_imsg_join_ap_t *)event_hdr;

                    CONTEXT(ssid)   = imsg_join_ap->ssid;
                    CONTEXT(pwd)    = imsg_join_ap->pwd;

                    esp8266_do_join_ap(imsg_join_ap->ssid, imsg_join_ap->pwd);
                    break;

                case EVENT_IMSG_ESP8266_CONNECT:
                    imsg_connect = (esp8266_imsg_connect_t *)event_hdr;

                    if (esp8266_do_connect(imsg_connect->ip, imsg_connect->port, imsg_connect->proto) < 0) {
                        esp8266_connect_msg_reply(-1);
                    }
                    break;


                case EVENT_IMSG_ESP8266_SEND:
                    imsg_send = (esp8266_imsg_send_t *)event_hdr;

                    CONTEXT(send_info).sockid   = imsg_send->sockid;
                    CONTEXT(send_info).buf      = imsg_send->buf;
                    CONTEXT(send_info).len      = imsg_send->len;

                    if (esp8266_do_send_ready(imsg_send->sockid, imsg_send->buf, imsg_send->len) != 0) {
                        esp8266_send_msg_reply(-1);
                    }

                    break;

                case EVENT_IMSG_ESP8266_CLOSE:
                    imsg_close = (esp8266_imsg_close_t *)event_hdr;

                    esp8266_do_close(imsg_close->sockid);

                    break;

                case EVENT_IMSG_ESP8266_DOMAIN_PARSE:
                    imsg_domain_parse = (esp8266_imsg_domain_parse_t *)event_hdr;

                    CONTEXT(domain_parse_info).host_ip      = imsg_domain_parse->host_ip;
                    CONTEXT(domain_parse_info).host_ip_len  = imsg_domain_parse->host_ip_len;

                    esp8266_do_parse_domain(imsg_domain_parse->host_name, imsg_domain_parse->host_ip, imsg_domain_parse->host_ip_len);

                    break;
            }

            tos_evtdrv_msg_free((evtdrv_msg_body_t)event_hdr);
        }

        return TOS_EVTDRV_SYS_EVENT_MSG;
    }

    if (event & EVENT_INTERNAL_ESP8266_INIT) {
        esp8266_restore();

        return EVENT_INTERNAL_ESP8266_INIT;
    }

    if (event & EVENT_INTERNAL_ESP8266_RESTORE) {
        at_delay_ms(3000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_OK) {
            if (CONTEXT(try)++ < 10) {
                esp8266_restore();
            } else {
                printf("restore failed\n");
                esp8266_common_msg_reply(EVENT_OMSG_ESP8266_INIT, -1);
            }
        } else {
            esp8266_echo_close();
        }

        CONTEXT(try) = 0;
        return EVENT_INTERNAL_ESP8266_RESTORE;
    }

    if (event & EVENT_INTERNAL_ESP8266_ECHO_CLOSE) {
        at_delay_ms(2000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_OK) {
            printf("echo close failed\n");
            esp8266_common_msg_reply(EVENT_OMSG_ESP8266_INIT, -1);
        } else {
            esp8266_net_mode_set(SAL_NET_MODE_STA);
        }

        return EVENT_INTERNAL_ESP8266_ECHO_CLOSE;
    }

    if (event & EVENT_INTERNAL_ESP8266_NET_MODE_SET) {
        at_delay_ms(1000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_OK &&
            CONTEXT(echo).status != AT_ECHO_STATUS_EXPECT) {
            if (CONTEXT(try)++ < 10) {
                esp8266_net_mode_set(SAL_NET_MODE_STA);
            } else {
                printf("net mode set failed\n");
                esp8266_common_msg_reply(EVENT_OMSG_ESP8266_INIT, -1);
            }
        } else {
            esp8266_send_mode_set(SAL_SEND_MODE_NORMAL);
        }

        CONTEXT(try) = 0;
        return EVENT_INTERNAL_ESP8266_NET_MODE_SET;
    }

    if (event & EVENT_INTERNAL_ESP8266_SEND_MODE_SET) {
        at_delay_ms(1000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_OK) {
            if (CONTEXT(try)++ < 10) {
                esp8266_send_mode_set(SAL_SEND_MODE_NORMAL);
            } else {
                printf("send mode set failed\n");
                esp8266_common_msg_reply(EVENT_OMSG_ESP8266_INIT, -1);
            }
        } else {
            esp8266_multilink_set(SAL_MULTILINK_STATE_ENABLE);
        }

        CONTEXT(try) = 0;
        return EVENT_INTERNAL_ESP8266_SEND_MODE_SET;
    }

    if (event & EVENT_INTERNAL_ESP8266_MULTILINK_SET) {
        at_delay_ms(1000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_OK) {
            if (CONTEXT(try)++ < 10) {
                esp8266_multilink_set(SAL_MULTILINK_STATE_ENABLE);
            } else {
                printf("multilink set failed\n");
                esp8266_common_msg_reply(EVENT_OMSG_ESP8266_INIT, -1);
            }
        } else {
            esp8266_common_msg_reply(EVENT_OMSG_ESP8266_INIT, 0);
        }

        CONTEXT(try) = 0;
        return EVENT_INTERNAL_ESP8266_MULTILINK_SET;
    }

    if (event & EVENT_INTERNAL_ESP8266_JOIN_AP) {
        at_delay_ms(1000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_EXPECT) {
            if (CONTEXT(try)++ < 10) {
                esp8266_do_join_ap(CONTEXT(ssid), CONTEXT(pwd));
            } else {
                printf("esp8266 join ap failed");
                esp8266_common_msg_reply(EVENT_OMSG_ESP8266_JOIN_AP, -1);
            }
        } else {
            esp8266_common_msg_reply(EVENT_OMSG_ESP8266_JOIN_AP, 0);
        }

        CONTEXT(try) = 0;
        return EVENT_INTERNAL_ESP8266_JOIN_AP;
    }

    if (event & EVENT_INTERNAL_ESP8266_CONNECT) {
        at_delay_ms(1000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_EXPECT &&
            CONTEXT(echo).status != AT_ECHO_STATUS_OK) {
            printf("esp8266 connect failed\n");
            esp8266_connect_msg_reply(-1);
        } else {
            esp8266_connect_msg_reply(CONTEXT(sockid));
        }

        return EVENT_INTERNAL_ESP8266_CONNECT;
    }

    if (event & EVENT_INTERNAL_ESP8266_SEND_READY) {
        at_delay_ms(1000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_OK &&
            CONTEXT(echo).status != AT_ECHO_STATUS_EXPECT) {
            if (esp8266_is_link_broken((const char *)CONTEXT(echo).buffer)) {
                tos_at_channel_set_broken(CONTEXT(sockid));
            }

            esp8266_send_msg_reply(-1);
        } else {
            esp8266_do_send(CONTEXT(send_info).sockid, CONTEXT(send_info).buf, CONTEXT(send_info).len);
        }

        return EVENT_INTERNAL_ESP8266_SEND_READY;
    }

    if (event & EVENT_INTERNAL_ESP8266_SEND) {
        at_delay_ms(1000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_EXPECT) {
            if (esp8266_is_link_broken((const char *)CONTEXT(echo).buffer)) {
                tos_at_channel_set_broken(CONTEXT(sockid));
            }

            esp8266_send_msg_reply(-1);
        } else {
            esp8266_send_msg_reply(CONTEXT(send_info).len);
        }

        return EVENT_INTERNAL_ESP8266_SEND;
    }

    if (event & EVENT_INTERNAL_ESP8266_DOMAIN_PARSE) {
        at_delay_ms(1000);
        if (CONTEXT(echo).status != AT_ECHO_STATUS_OK) {
            esp8266_common_msg_reply(EVENT_OMSG_ESP8266_DOMAIN_PARSE, -1);
        } else {
            /*
            +CIPDOMAIN:183.232.231.172
             */
            char *str = strstr((const char *)CONTEXT(echo_buffer), "+CIPDOMAIN:");
            if (!str) {
                esp8266_common_msg_reply(EVENT_OMSG_ESP8266_DOMAIN_PARSE, -1);
            } else {
                char *host_ip_buffer = CONTEXT(domain_parse_info).host_ip;
                size_t host_ip_buffer_len = CONTEXT(domain_parse_info).host_ip_len;

                sscanf(str, "+CIPDOMAIN:%s", host_ip_buffer);
                host_ip_buffer[host_ip_buffer_len - 1] = '\0';
                printf("GOT IP: %s\n", host_ip_buffer);

                esp8266_common_msg_reply(EVENT_OMSG_ESP8266_DOMAIN_PARSE, 0);
            }
        }

        return EVENT_INTERNAL_ESP8266_DOMAIN_PARSE;
    }

    return TOS_EVTDRV_EVENT_NONE;
}

