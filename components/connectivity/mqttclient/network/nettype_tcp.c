/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 13:38:52
 * @LastEditTime: 2020-05-25 10:13:41
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "nettype_tcp.h"
#include "mqtt_log.h"

int nettype_tcp_read(network_t *n, unsigned char *read_buf, int len, int timeout)
{
    return platform_net_socket_recv_timeout(n->socket, read_buf, len, timeout);
}

int nettype_tcp_write(network_t *n, unsigned char *write_buf, int len, int timeout)
{
    return platform_net_socket_write_timeout(n->socket, write_buf, len, timeout);
}

int nettype_tcp_connect(network_t* n)
{
    n->socket = platform_net_socket_connect(n->host, n->port, PLATFORM_NET_PROTO_TCP);
    if (n->socket < 0)
        RETURN_ERROR(n->socket);
    
    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

void nettype_tcp_disconnect(network_t* n)
{
    if (NULL != n)
        platform_net_socket_close(n->socket);
    n->socket = -1;
}
