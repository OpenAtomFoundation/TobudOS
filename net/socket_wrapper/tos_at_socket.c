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

#include "tos_at_socket.h"
#include "tos_at_socket_prv.h"
#include "sal_module_wrapper.h"

int socket(int domain, int type, int protocol)
{
    int pub_fd;
    sal_proto_t proto;

    protocol = protocol;

    if (domain != AF_INET) {
        return -1;
    }

    if (type == SOCK_STREAM) {
        proto = TOS_SAL_PROTO_TCP;
    } else if (type == SOCK_DGRAM) {
        proto = TOS_SAL_PROTO_UDP;
    } else {
        return -1;
    }

    pub_fd = socket_id_alloc();
    if (pub_fd < 0) {
        return -1;
    }

    socket_set_protocol(pub_fd, proto);

    return pub_fd;
}

static int sockaddr2ip(const struct sockaddr_in *addr_in, char ip[], int ip_len)
{
    snprintf(ip, 16, "%u.%u.%u.%u", ((uint8_t *)(&addr_in->sin_addr.s_addr))[0],
                                    ((uint8_t *)(&addr_in->sin_addr.s_addr))[1],
                                    ((uint8_t *)(&addr_in->sin_addr.s_addr))[2],
                                    ((uint8_t *)(&addr_in->sin_addr.s_addr))[3]);

    return 0;
}

static int sockaddr2port(const struct sockaddr_in *addr_in, char port[], int port_len)
{
    snprintf(port, port_len, "%d", htons(addr_in->sin_port));

    return 0;
}

int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
    int prv_fd;
    char ip[16], port[8];
    const struct sockaddr_in *addr_in = (const struct sockaddr_in *)address;

    SOCKET_ID_SANITY_CHECK(socket);

    if (sockaddr2ip(addr_in, ip, sizeof(ip)) != 0) {
        return -1;
    }

    if (sockaddr2port(addr_in, port, sizeof(port)) != 0) {
        return -1;
    }

    prv_fd = tos_sal_module_connect(ip, port, socket_get_protocol(socket));
    if (prv_fd < 0) {
        return -1;
    }

    socket_set_fd(socket, prv_fd);

    return 0;
}

int recv(int socket, void *buffer, size_t length, int flags)
{
    int total_len = 0, remain_len = length, recv_len;

    SOCKET_ID_SET_SANITY_CHECK(socket);

    if (!(flags & MSG_WAITALL)) {
        return tos_sal_module_recv(socket_get_fd(socket), buffer, length);
    }

    while (total_len < length) {
        recv_len = tos_sal_module_recv(socket_get_fd(socket),
                                        (uint8_t *)buffer + total_len,
                                        remain_len);

        if (recv_len <= 0) {
            continue;
        }

        remain_len -= recv_len;
        total_len += recv_len;
    }

    return length;
}

int recvfrom(int socket, void *buffer, size_t length, int flags, struct sockaddr *address, socklen_t *address_len)
{
    int total_len = 0, remain_len = length, recv_len;

    (void)address;
    (void)address_len;

    SOCKET_ID_SET_SANITY_CHECK(socket);

    if (!(flags & MSG_WAITALL)) {
        return tos_sal_module_recvfrom(socket_get_fd(socket), buffer, length);
    }

    while (total_len < length) {
        recv_len = tos_sal_module_recvfrom(socket_get_fd(socket),
                                        (uint8_t *)buffer + total_len,
                                        remain_len);

        if (recv_len <= 0) {
            continue;
        }

        remain_len -= recv_len;
        total_len += recv_len;
    }

    return length;
}

int send(int socket, const void *buffer, size_t length, int flags)
{
    SOCKET_ID_SET_SANITY_CHECK(socket);

    return tos_sal_module_send(socket_get_fd(socket), buffer, length);
}

int sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len)
{
    (void)dest_addr;
    (void)dest_len;

    SOCKET_ID_SET_SANITY_CHECK(socket);

    return tos_sal_module_send(socket_get_fd(socket), message, length);
}

int shutdown(int socket, int how)
{
    int rc;

    how = how;

    SOCKET_ID_SET_SANITY_CHECK(socket);

    if (socket_id_set_sanity_check(socket)) {
        rc = tos_sal_module_close(socket_get_fd(socket));
        if (rc != 0) {
            return rc;
        }
    }

    socket_id_free(socket);

    return 0;
}

int read(int socket, void *buffer, size_t length)
{
    SOCKET_ID_SET_SANITY_CHECK(socket);

    return tos_sal_module_recv(socket_get_fd(socket), buffer, length);
}

int close(int socket)
{
    int rc;

    SOCKET_ID_SANITY_CHECK(socket);

    if (socket_id_set_sanity_check(socket)) {
        rc = tos_sal_module_close(socket_get_fd(socket));
        if (rc != 0) {
            return rc;
        }
    }

    socket_id_free(socket);

    return 0;
}

int write(int socket, const void *buffer, size_t length)
{
    SOCKET_ID_SET_SANITY_CHECK(socket);

    return tos_sal_module_send(socket_get_fd(socket), buffer, length);
}

