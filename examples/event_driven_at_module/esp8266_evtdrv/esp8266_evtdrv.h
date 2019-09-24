/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "tos_evtdrv.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#define EVENT_INTERNAL_ESP8266_INIT             (1u << 0)
#define EVENT_INTERNAL_ESP8266_RESTORE          (1u << 1)
#define EVENT_INTERNAL_ESP8266_ECHO_CLOSE       (1u << 2)
#define EVENT_INTERNAL_ESP8266_NET_MODE_SET     (1u << 3)
#define EVENT_INTERNAL_ESP8266_SEND_MODE_SET    (1u << 4)
#define EVENT_INTERNAL_ESP8266_MULTILINK_SET    (1u << 5)
#define EVENT_INTERNAL_ESP8266_JOIN_AP          (1u << 6)
#define EVENT_INTERNAL_ESP8266_CONNECT          (1u << 7)
#define EVENT_INTERNAL_ESP8266_SEND_READY       (1u << 8)
#define EVENT_INTERNAL_ESP8266_SEND             (1u << 9)
#define EVENT_INTERNAL_ESP8266_DOMAIN_PARSE     (1u << 10)

#define EVENT_IMSG_ESP8266_INIT                 (1u << 0)
#define EVENT_IMSG_ESP8266_JOIN_AP              (1u << 1)
#define EVENT_IMSG_ESP8266_CONNECT              (1u << 2)
#define EVENT_IMSG_ESP8266_SEND                 (1u << 3)
#define EVENT_IMSG_ESP8266_CLOSE                (1u << 4)
#define EVENT_IMSG_ESP8266_DOMAIN_PARSE         (1u << 5)


#define EVENT_OMSG_ESP8266_INIT                 (1u << 0)
#define EVENT_OMSG_ESP8266_JOIN_AP              (1u << 1)
#define EVENT_OMSG_ESP8266_CONNECT              (1u << 2)
#define EVENT_OMSG_ESP8266_SEND                 (1u << 3)
#define EVENT_OMSG_ESP8266_DOMAIN_PARSE         (1u << 4)

typedef struct esp8266_event_header_st {
    evtdrv_event_flag_t   event;
    evtdrv_task_id_t          sender_id;
} esp8266_event_hdr_t;

typedef struct esp8266_in_msg_init_st {
    esp8266_event_hdr_t header;
    hal_uart_port_t     uart_port;
    evtdrv_task_id_t      at_task_id;
    evtdrv_task_id_t      esp8266_task_id;
} esp8266_imsg_init_t;

typedef struct esp8266_in_msg_join_ap_st {
    esp8266_event_hdr_t header;
    char               *ssid;
    char               *pwd;
} esp8266_imsg_join_ap_t;

typedef struct esp8266_in_msg_connect_st {
    esp8266_event_hdr_t header;
    char               *ip;
    char               *port;
    sal_proto_t         proto;
} esp8266_imsg_connect_t;

typedef struct esp8266_in_msg_send_st {
    esp8266_event_hdr_t header;
    int                 sockid;
    void               *buf;
    size_t              len;
} esp8266_imsg_send_t;

typedef struct esp8266_in_msg_close_st {
    esp8266_event_hdr_t header;
    int                 sockid;
} esp8266_imsg_close_t;

typedef struct esp8266_in_msg_domain_parse_st {
    esp8266_event_hdr_t header;
    char               *host_name;
    char               *host_ip;
    size_t              host_ip_len;
} esp8266_imsg_domain_parse_t;

typedef struct esp8266_out_msg_common_st {
    esp8266_event_hdr_t     header;
    int                     return_code;
} esp8266_omsg_common_t;

typedef struct esp8266_out_msg_connect_st {
    esp8266_event_hdr_t header;
    int                 sockid;
} esp8266_omsg_connect_t;

typedef struct esp8266_out_msg_send_st {
    esp8266_event_hdr_t header;
    int                 write_len;
} esp8266_omsg_send_t;


typedef enum sal_net_mode {
    SAL_NET_MODE_STA,
    SAL_NET_MODE_AP,
    SAL_NET_MODE_STA_AP,
} sal_net_mode_t ;

typedef enum sal_send_mode {
    SAL_SEND_MODE_NORMAL,
    SAL_SEND_MODE_TRANSPARENT,
} sal_send_mode_t;

typedef enum sal_multilink_state {
    SAL_MULTILINK_STATE_ENABLE,
    SAL_MULTILINK_STATE_DISABLE,
} sal_multilink_state_t;

int esp8266_sal_init(hal_uart_port_t uart_port, evtdrv_task_id_t at_task_id, evtdrv_task_id_t esp8266_task_id);

int esp8266_join_ap(const char *ssid, const char *pwd);

evtdrv_event_flag_t esp8266_task(evtdrv_event_flag_t event);

#endif /* __ESP8266_H__ */

