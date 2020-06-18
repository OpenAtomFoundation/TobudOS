/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-10 23:45:59
 * @LastEditTime : 2020-01-13 02:48:53
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_net_socket.h"

int platform_net_socket_connect(const char *host, const char *port, int proto)
{
    int fd;

    fd = tos_sal_module_connect(host, port, TOS_SAL_PROTO_TCP);
    
    if (fd < 0) {
        return MQTT_CONNECT_FAILED_ERROR;
    }
    
    return fd;
}

int platform_net_socket_recv(int fd, void *buf, size_t len, int flags)
{
    return tos_sal_module_recv(fd, buf, len);
}

int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	return tos_sal_module_recv_timeout(fd, buf, len, timeout);
}

int platform_net_socket_write(int fd, void *buf, size_t len)
{
    return tos_sal_module_send(fd, buf, len);
}

int platform_net_socket_write_timeout(int fd, unsigned char *buf, int len, int timeout)
{
    return tos_sal_module_send(fd, buf, len);
}

int platform_net_socket_close(int fd)
{
    return tos_sal_module_close(fd);
}

