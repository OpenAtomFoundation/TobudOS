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

#ifndef  _TOS_AT_SOCKET_H_
#define  _TOS_AT_SOCKET_H_

#include "tos_at_socket_lib.h"
#include "tos_at_socket_types.h"

#define AF_INET             0

#define AF_INET6            1

#define AF_UNIX             2

/* Provides sequenced, reliable, bidirectional, connection-mode byte streams, and may provide a transmission mechanism for out-of-band data. */
#define SOCK_STREAM         0

/* Provides datagrams, which are connectionless-mode, unreliable messages of fixed maximum length. */
#define SOCK_DGRAM          1

/* Peeks at an incoming message. The data is treated as unread and the next recv() or similar function shall still return this data. */
#define MSG_PEEK            0x01

/* Requests out-of-band data. The significance and semantics of out-of-band data are protocol-specific. */
#define MSG_OOB             0x02

/* On SOCK_STREAM sockets this requests that the function block until the full amount of data can be returned. The function may return the smaller amount of data if the socket is a message-based socket, if a signal is caught, if the connection is terminated, if MSG_PEEK was specified, or if an error is pending for the socket. */
#define MSG_WAITALL         0x04

int socket(int domain, int type, int protocol);

int connect(int socket, const struct sockaddr *address, socklen_t address_len);

int recv(int socket, void *buffer, size_t length, int flags);

int recvfrom(int socket, void * buffer, size_t length, int flags, struct sockaddr *address, socklen_t *address_len);

int send(int socket, const void *buffer, size_t length, int flags);

int sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);

int shutdown(int socket, int how);

int read(int socket, void *buffer, size_t length);

int close(int socket);

int write(int socket, const void *buffer, size_t length);

#endif /* _TOS_AT_SOCKET_H_ */

