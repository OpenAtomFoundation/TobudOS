/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _AT_SOCKET_INF_H_
#define _AT_SOCKET_INF_H_

#include <stdint.h>
#include <stdio.h>

#include "utils_list.h"

#define UNUSED_SOCKET             (-1)
#define MAX_AT_SOCKET_NUM         (5)
#define AT_SOCKET_SEND_TIMEOUT_MS (1000)
#define AT_SOCKET_RECV_TIMEOUT_MS (1000)
#define IPV4_STR_MAX_LEN          (16)

typedef enum { eNET_TCP = 6, eNET_UDP = 17, eNET_DEFAULT = 0xff } eNetProto;

typedef enum { eSOCKET_ALLOCED = 0, eSOCKET_CONNECTED, eSOCKET_CLOSED } eSocketState;

/* AT receive package list structure */
typedef struct at_recv_pkt {
    List   list;
    size_t bfsz_totle;
    size_t bfsz_index;
    char * buff;
} at_recv_pkt;

typedef enum {
    AT_SOCKET_EVT_RECV = 0,
    AT_SOCKET_EVT_CLOSED,
} at_socket_evt_t;

typedef void (*at_evt_cb_t)(int fd, at_socket_evt_t event, char *buff, size_t bfsz);

/*at device driver ops, use at_device_op_register register to at socket*/
typedef struct {
    int (*init)(void);
    int (*get_local_mac)(char *macbuff, size_t bufflen);
    int (*get_local_ip)(char *ip, size_t iplen, char *gw, size_t gwlen, char *mask, size_t masklen);
    int (*parse_domain)(const char *host_name, char *host_ip, size_t host_ip_len);
    int (*connect)(const char *ip, uint16_t port, eNetProto proto);
    int (*send)(int fd, const void *buf, size_t len);
    int (*recv_timeout)(int fd, void *buf, size_t len, uint32_t timeout);
    int (*close)(int fd);
    void (*set_event_cb)(at_socket_evt_t event, at_evt_cb_t cb);
    char *deviceName;
} at_device_op_t;

/*at socket context*/
typedef struct {
    int             fd; /** socket fd */
    List *          recvpkt_list;
    char            remote_ip[IPV4_STR_MAX_LEN];
    uint16_t        remote_port;
    uint32_t        send_timeout_ms;
    uint32_t        recv_timeout_ms;
    void *          recv_lock;
    at_device_op_t *dev_op;
    eNetProto       net_type;
    eSocketState    state;
} at_socket_ctx_t;

// at socket api
int at_device_op_register(at_device_op_t *device_op);
int at_socket_init(void);
int at_socket_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len);
int at_socket_get_local_mac(char *macbuff, size_t bufflen);
int at_socket_get_local_ip(char *ip, size_t iplen, char *gw, size_t gwlen, char *mask, size_t masklen);
int at_socket_connect(const char *host, uint16_t port, eNetProto eProto);
int at_socket_close(int fd);
int at_socket_send(int fd, const void *buf, size_t len);
int at_socket_recv(int fd, void *buf, size_t len);
#endif
