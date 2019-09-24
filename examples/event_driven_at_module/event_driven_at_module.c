#include "esp8266_evtdrv.h"
#include "tos_at_evtdrv.h"
#include "tos_evtdrv.h"
#include "sal_module_wrapper.h"

evtdrv_task_id_t at_task_id = 0;
evtdrv_task_id_t esp8266_task_id = 1;
evtdrv_task_id_t user_task_id = 2;

int socket_id_0 = 0;

#define RECV_LEN            1024
uint8_t recv_data_0[RECV_LEN];

evtdrv_timer_t task0_send_timer;

#define EVENT_USER_INIT         (1u << 0)
#define EVENT_USER_SEND         (1u << 1)
#define EVENT_USER_CLOSE        (1u << 2)

int send_count = 0;

char host_ip[16];

void timer_callback(void *arg)
{
    tos_evtdrv_event_set(user_task_id, EVENT_USER_SEND);
}

evtdrv_event_flag_t user_task(evtdrv_event_flag_t event)
{
    esp8266_event_hdr_t *event_hdr;
    esp8266_omsg_common_t *omsg_common;
    esp8266_omsg_connect_t *omsg_connect;
    esp8266_omsg_send_t *omsg_send;

    if (event & TOS_EVTDRV_SYS_EVENT_INIT) {
        tos_evtdrv_event_set(user_task_id, EVENT_USER_INIT);
        return TOS_EVTDRV_SYS_EVENT_INIT;
    }

    if (event & EVENT_USER_INIT) {
        esp8266_sal_init(HAL_UART_PORT_0, at_task_id, esp8266_task_id);
        return EVENT_USER_INIT;
    }

    if (event & TOS_EVTDRV_SYS_EVENT_MSG) {
        while (event_hdr = (esp8266_event_hdr_t *)tos_evtdrv_msg_recv()) {
            if (event_hdr->sender_id == esp8266_task_id) {
                switch (event_hdr->event) {
                    case EVENT_OMSG_ESP8266_INIT:
                        omsg_common = (esp8266_omsg_common_t *)event_hdr;

                        if (omsg_common->return_code == 0) {
                            esp8266_join_ap("SheldonDai", "srnr6x9xbhmb0");
                        } else {
                            printf("esp8266 init failed!\n");
                        }

                        break;

                    case EVENT_OMSG_ESP8266_JOIN_AP:
                        omsg_common = (esp8266_omsg_common_t *)event_hdr;

                        if (omsg_common->return_code != 0) {
                            printf("esp8266 join ap failed!\n");
                            break;
                        }

                        tos_sal_module_parse_domain("www.tencent.com", host_ip, sizeof(host_ip));
                        break;

                    case EVENT_OMSG_ESP8266_DOMAIN_PARSE:
                        omsg_common = (esp8266_omsg_common_t *)event_hdr;

                        if (omsg_common->return_code != 0) {
                            printf("esp8266 domain parse failed!\n");
                        } else {
                            printf("domain parse(www.tencent.com): %s\n", host_ip);
                        }

                        tos_sal_module_connect("39.108.190.129", "8080", TOS_SAL_PROTO_TCP);

                        break;

                    case EVENT_OMSG_ESP8266_CONNECT:
                        omsg_connect = (esp8266_omsg_connect_t *)event_hdr;
                        socket_id_0 = omsg_connect->sockid;

                        if (socket_id_0 < 0) {
                            printf("esp8266 connect failed!\n");
                            break;
                        }

                        /* ¶¨Ê±·¢ËÍ */
                        tos_evtdrv_timer_create(&task0_send_timer,
                                                    timer_callback, NULL,
                                                    EVTDRV_TIMER_OPT_PERIODIC);
                        tos_evtdrv_timer_start(&task0_send_timer, 6000);

                        break;

                    case EVENT_OMSG_ESP8266_SEND:
                        omsg_send = (esp8266_omsg_send_t *)event_hdr;
                        int write_len = omsg_send->write_len;
                        if (write_len < 0) {
                            printf("write failed! %d\n", write_len);
                        } else {
                            printf("task0 write success: %d\n", write_len);
                        }

                        break;
                }
            }

            tos_evtdrv_msg_free((evtdrv_msg_body_t)event_hdr);
        }

        return TOS_EVTDRV_SYS_EVENT_MSG;
    }

    if (event & EVENT_USER_SEND) {
        if (++send_count > 5) {
            // after send 5 times, we shutdown the connection
            tos_evtdrv_event_set(user_task_id, EVENT_USER_CLOSE);
            return EVENT_USER_SEND;
        }

        tos_sal_module_send(socket_id_0, "hello", 5);
        int len = tos_sal_module_recv(socket_id_0, recv_data_0, sizeof(recv_data_0));
        if (len > 0) {
            printf("task0 receive from server(%d): %s\n", len, recv_data_0);
        }

        return EVENT_USER_SEND;
    }

    if (event & EVENT_USER_CLOSE) {
        tos_evtdrv_timer_stop(&task0_send_timer);
        tos_sal_module_close(socket_id_0);
        return EVENT_USER_CLOSE;
    }

    return TOS_EVTDRV_EVENT_NONE;
}

evtdrv_task_entry_t tasks[] = {
    tos_at_evtdrv_task,
    esp8266_task,
    user_task,
};

int main(void)
{
    board_init();

    tos_evtdrv_sys_init(tasks, sizeof(tasks) / sizeof(tasks[0]), K_NULL);
    tos_evtdrv_sys_start();
}

