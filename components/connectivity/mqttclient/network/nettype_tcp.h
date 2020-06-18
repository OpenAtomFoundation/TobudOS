/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 13:39:00
 * @LastEditTime: 2020-05-24 15:49:46
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _NETTYPE_TCP_H_
#define _NETTYPE_TCP_H_

#include "platform_net_socket.h"
#include "network.h"
#include "mqtt_error.h"

int nettype_tcp_read(network_t *n, unsigned char *buf, int len, int timeout);
int nettype_tcp_write(network_t *n, unsigned char *buf, int len, int timeout);
int nettype_tcp_connect(network_t* n);
void nettype_tcp_disconnect(network_t* n);

#endif
