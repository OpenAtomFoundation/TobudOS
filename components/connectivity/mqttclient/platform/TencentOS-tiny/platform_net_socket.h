/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 13:39:00
 * @LastEditTime : 2020-01-12 00:22:22
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _PLATFORM_NET_SOCKET_H_
#define _PLATFORM_NET_SOCKET_H_

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include <lwip/sockets.h>
#include "lwip/netdb.h"

#include "network.h"
#include "error.h"

#define PLATFORM_NET_PROTO_TCP  0 /**< The TCP transport protocol */
#define PLATFORM_NET_PROTO_UDP  1 /**< The UDP transport protocol */

int platform_net_socket_connect(const char *host, const char *port, int proto);
int platform_net_socket_recv(int fd, void *buf, size_t len, int flags);
int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout);
int platform_net_socket_write(int fd, void *buf, size_t len);
int platform_net_socket_write_timeout(int fd, unsigned char *buf, int len, int timeout);
int platform_net_socket_close(int fd);
int platform_net_socket_set_block(int fd);
int platform_net_socket_set_nonblock(int fd);
int platform_net_socket_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

#endif /* _PLATFORM_NET_SOCKET_H_ */
