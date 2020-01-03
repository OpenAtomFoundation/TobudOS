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

#include "tos_k.h"
#include "mbedtls/net_sockets.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

void mbedtls_net_init(mbedtls_net_context *ctx)
{
    if (!ctx) {
        return;
    }

    ctx->fd = -1;
}

int mbedtls_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto)
{
    int ret;
    struct addrinfo hints, *addr_list, *cur;

#if 0
    if ((ret = net_prepare() ) != 0) {
        return ret;
    }
#endif

    /* Do name resolution with both IPv6 and IPv4 */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if (getaddrinfo(host, port, &hints, &addr_list) != 0) {
        return MBEDTLS_ERR_NET_UNKNOWN_HOST;
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        ctx->fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (ctx->fd < 0) {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        if (connect(ctx->fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            ret = 0;
            break;
        }

        close(ctx->fd);
        ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

    freeaddrinfo(addr_list);

    return ret;
}

/*
 * Check if the requested operation would be blocking on a non-blocking socket
 * and thus 'failed' with a negative return value.
 *
 * Note: on a blocking socket this function always returns 0!
 */
static int net_would_block(const mbedtls_net_context *ctx)
{
    int err = errno;

    /*
    * Never return 'WOULD BLOCK' on a non-blocking socket
    */
    if ((fcntl(ctx->fd, F_GETFL, F_GETFL) & O_NONBLOCK) != O_NONBLOCK) {
        errno = err;
        return 0;
    }

    switch (errno = err) {
#if defined EAGAIN
        case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
#endif
        return 1;
    }

    return 0;
}

/*
 * Set the socket blocking or non-blocking
 */
int mbedtls_net_set_block( mbedtls_net_context *ctx )
{
    return fcntl(ctx->fd, F_SETFL, fcntl(ctx->fd, F_GETFL, F_GETFL) & ~O_NONBLOCK);
}

int mbedtls_net_set_nonblock( mbedtls_net_context *ctx )
{
    return fcntl(ctx->fd, F_SETFL, fcntl(ctx->fd, F_GETFL, F_GETFL) | O_NONBLOCK);
}

void mbedtls_net_usleep(unsigned long usec)
{
    uint32_t ms;
    k_tick_t tick;

    ms = usec / 1000;
    if (ms == 0) {
        ms = 1;
    }

    tick = tos_millisec2tick(ms);

    tos_sleep_ms(tick);
}

int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }

    ret = read(fd, buf, len);

    if (ret < 0) {
        if (net_would_block(ctx) != 0) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }

        if (errno == EPIPE || errno == ECONNRESET) {
            return MBEDTLS_ERR_NET_CONN_RESET;
        }

        if (errno == EINTR) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }

        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

int mbedtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    ret = select(fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);

    /* Zero fds ready means we timed out */
    if (ret == 0) {
        return MBEDTLS_ERR_SSL_TIMEOUT;
    }

    if (ret < 0) {
        if (errno == EINTR) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }

        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    /* This call will not block */
    return mbedtls_net_recv(ctx, buf, len);
}

int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if (fd < 0) {
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }

    ret = write(fd, buf, len);

    if (ret < 0) {
        if (net_would_block( ctx ) != 0) {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        }


        if (errno == EPIPE || errno == ECONNRESET) {
            return MBEDTLS_ERR_NET_CONN_RESET;
        }

        if (errno == EINTR) {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        }

        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    return ret;
}

void mbedtls_net_free(mbedtls_net_context *ctx)
{
    if (ctx->fd < 0) {
        return;
    }

    shutdown(ctx->fd, 2);
    close(ctx->fd);

    ctx->fd = -1;
}

//////////////////////////////////////////
// dtls
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

