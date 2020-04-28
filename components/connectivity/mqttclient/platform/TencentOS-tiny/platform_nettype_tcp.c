/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 13:38:52
 * @LastEditTime : 2020-02-16 02:22:48
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_nettype_tcp.h"
#include "log.h"

int platform_nettype_tcp_read(network_t *n, unsigned char *read_buf, int len, int timeout)
{
	return platform_net_socket_recv_timeout(n->socket, read_buf, len, timeout);
}

int platform_nettype_tcp_write(network_t *n, unsigned char *write_buf, int len, int timeout)
{
    return platform_net_socket_write_timeout(n->socket, write_buf, len, timeout);
}

int platform_nettype_tcp_connect(network_t* n)
{
    if (n->socket >= 0) {
        platform_net_socket_close(n->socket);       // prevent opening too many fd descriptors
    }

    n->socket = platform_net_socket_connect(n->network_params.addr, n->network_params.port, PLATFORM_NET_PROTO_TCP);
    if (n->socket < 0)
        RETURN_ERROR(n->socket);
    
    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

void platform_nettype_tcp_disconnect(network_t* n)
{
    if (NULL != n)
	    platform_net_socket_close(n->socket);
    n->socket = -1;
}
