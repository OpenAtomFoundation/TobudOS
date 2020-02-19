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
    int fd, ret = MQTT_SOCKET_UNKNOWN_HOST;
    struct addrinfo hints, *addr_list, *cur;
    
    /* Do name resolution with both IPv6 and IPv4 */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = (proto == PLATFORM_NET_PROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = (proto == PLATFORM_NET_PROTO_UDP) ? IPPROTO_UDP : IPPROTO_TCP;
    
    if (getaddrinfo(host, port, &hints, &addr_list) != 0) {
        return ret;
    }
    
    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            ret = MQTT_SOCKET_FAILED;
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            ret = fd;
            break;
        }

        close(fd);
        ret = MQTT_CONNECT_FAILED_ERROR;
    }

    freeaddrinfo(addr_list);
    return ret;
}

int platform_net_socket_recv(int fd, void *buf, size_t len, int flags)
{
    return recv(fd, buf, len, flags);
}

int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout)
{
    int rc;
    int bytes = 0;
	struct timeval tv = {
        timeout / 1000, 
        (timeout % 1000) * 1000
    };
    
	if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec <= 0)) {
		tv.tv_sec = 0;
		tv.tv_usec = 100;
	}

	platform_net_socket_setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	while (bytes < len) {
		rc = platform_net_socket_recv(fd, &buf[bytes], (size_t)(len - bytes), 0);
		if (rc <= 0) {
			bytes = rc;
			break;
		} else {
			bytes += rc;
		}
	}
	return bytes;
}

int platform_net_socket_write(int fd, void *buf, size_t len)
{
    return write(fd, buf, len);
}

int platform_net_socket_write_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	struct timeval tv = {
        timeout / 1000, 
        (timeout % 1000) * 1000
    };
    
	if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec <= 0)) {
		tv.tv_sec = 0;
		tv.tv_usec = 100;
	}

	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
	
    return write(fd, buf, len);
}

int platform_net_socket_close(int fd)
{
    return close(fd);
}

int platform_net_socket_set_block(int fd)
{
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, F_GETFL) & ~O_NONBLOCK);
}

int platform_net_socket_set_nonblock(int fd)
{
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, F_GETFL) | O_NONBLOCK);
}

int platform_net_socket_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
    return setsockopt(fd, level, optname, optval, optlen);
}

