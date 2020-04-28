/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-08 21:48:09
 * @LastEditTime : 2020-01-12 00:23:42
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if !defined(MBEDTLS_NET_C)

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#endif

#include "mbedtls/net_sockets.h"
#include "platform_net_socket.h"
#include "platform_timer.h"

/*
 * Initialize a context
 */
void mbedtls_net_init(mbedtls_net_context *ctx)
{
    if (!ctx) {
        return;
    }

    ctx->fd = -1;
}

/*
 * Initiate a TCP connection with host:port and the given protocol
 */
int mbedtls_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto)
{
    int net_proto;

    net_proto = (proto == MBEDTLS_NET_PROTO_UDP) ? PLATFORM_NET_PROTO_UDP : PLATFORM_NET_PROTO_TCP;

    ctx->fd = platform_net_socket_connect(host, port, net_proto);

    if (ctx->fd < 0) {
        return ctx->fd;
    }

    return 0;
}

/*
 * Set the socket blocking or non-blocking
 */
int mbedtls_net_set_block( mbedtls_net_context *ctx )
{
    return platform_net_socket_set_block(ctx->fd);
}

int mbedtls_net_set_nonblock( mbedtls_net_context *ctx )
{
    return platform_net_socket_set_nonblock(ctx->fd);
}

/*
 * Read at most 'len' characters
 */
int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;
    
    if (fd < 0) {
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }
    
    ret = platform_net_socket_recv(fd, buf, len, 0);

    if (ret == 0) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (ret < 0) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
int mbedtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }

    ret = platform_net_socket_recv_timeout(fd, buf, len, timeout);

    if (ret == 0) {
        return MBEDTLS_ERR_SSL_TIMEOUT;
    } else if (ret < 0) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

/*
 * Write at most 'len' characters
 */

int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if (fd < 0) {
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }

    ret = platform_net_socket_write(fd, (unsigned char*)buf, len);

    if (ret == 0) {
        return MBEDTLS_ERR_SSL_WANT_WRITE;
    } else if (ret < 0) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    return ret;
}

/*
 * Gracefully close the connection
 */

void mbedtls_net_free(mbedtls_net_context *ctx)
{
    if (ctx->fd < 0) {
        return;
    }

    // shutdown(ctx->fd, 2);
    platform_net_socket_close(ctx->fd);

    ctx->fd = -1;
}

/*
 * Portable usleep helper
 */
void mbedtls_net_usleep( unsigned long usec )
{
    platform_timer_usleep(usec);
}

/* dtls */ 
void mbedtls_dtls_net_init(mbedtls_net_context *ctx)
{
    mbedtls_net_init(ctx);
}

int mbedtls_dtls_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto)
{
    return mbedtls_net_connect(ctx, host, port, proto);
}

void mbedtls_dtls_net_usleep(unsigned long usec)
{
    mbedtls_net_usleep(usec);
}

int mbedtls_dtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
    return mbedtls_net_recv(ctx, buf, len);
}

int mbedtls_dtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    return mbedtls_net_recv_timeout(ctx, buf, len, timeout);
}

int mbedtls_dtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    return mbedtls_net_send(ctx, buf, len);
}

void mbedtls_dtls_net_free(mbedtls_net_context *ctx)
{
    mbedtls_net_free(ctx);
}

#endif /* MBEDTLS_NET_C */
