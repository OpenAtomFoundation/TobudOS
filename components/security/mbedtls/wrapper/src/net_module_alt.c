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
#include "sal_module_wrapper.h"

void mbedtls_net_init(mbedtls_net_context *ctx)
{
    if (!ctx) {
        return;
    }

    ctx->fd = -1;
}

int mbedtls_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto)
{
    int fd;
    char ip[16] = { 0 };
    sal_proto_t sal_proto;

    if (!ctx) {
        return -1;
    }

    if (tos_sal_module_parse_domain(host, ip, sizeof(ip)) != 0) {
        return -1;
    }

    sal_proto = (proto == MBEDTLS_NET_PROTO_UDP ? TOS_SAL_PROTO_UDP : TOS_SAL_PROTO_TCP);
	fd = tos_sal_module_connect(ip, port, sal_proto);
    if (fd < 0) {
        return -1;
    }

    ctx->fd = fd;
    return 0;
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
    int fd, ret;

    if (!ctx) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    fd = ((mbedtls_net_context *)ctx)->fd;
    ret = tos_sal_module_recv(fd, buf, len);

    if (ret == 0) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (ret < 0) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

int mbedtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    int fd, ret;

    if (!ctx) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    fd = ((mbedtls_net_context *)ctx)->fd;
    ret = tos_sal_module_recv_timeout(fd, buf, len, timeout);

    if (ret == 0) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (ret < 0) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    int fd, ret;

    if (!ctx) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    fd = ((mbedtls_net_context *)ctx)->fd;
    ret = tos_sal_module_send(fd, buf, len);

    if (ret == 0) {
        return MBEDTLS_ERR_SSL_WANT_WRITE;
    } else if (ret < 0) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    return ret;
}

void mbedtls_net_free(mbedtls_net_context *ctx)
{
    if (!ctx) {
        return;
    }

    tos_sal_module_close(ctx->fd);
}

//////////////////////////////////////////
// dtls
void mbedtls_dtls_net_init(mbedtls_net_context *ctx)
{
    if (!ctx) {
        return;
    }

    ctx->fd = -1;
}

int mbedtls_dtls_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto)
{
    int fd;
    char ip[16] = { 0 };
    sal_proto_t sal_proto;

    if (!ctx) {
        return -1;
    }

    if (tos_sal_module_parse_domain(host, ip, sizeof(ip)) != 0) {
        return -1;
    }

    sal_proto = (proto == MBEDTLS_NET_PROTO_UDP ? TOS_SAL_PROTO_UDP : TOS_SAL_PROTO_TCP);
	fd = tos_sal_module_connect(ip, port, sal_proto);
    if (fd < 0) {
        return -1;
    }


    ctx->fd = fd;
    return 0;
}

void mbedtls_dtls_net_usleep(unsigned long usec)
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

int mbedtls_dtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
    int fd, ret;

    if (!ctx) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    fd = ((mbedtls_net_context *)ctx)->fd;
    ret = tos_sal_module_recvfrom(fd, buf, len);

    if (ret == 0) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (ret < 0) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

int mbedtls_dtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    int fd, ret;

    if (!ctx) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    fd = ((mbedtls_net_context *)ctx)->fd;
    ret = tos_sal_module_recvfrom_timeout(fd, buf, len, timeout);

    if (ret == 0) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (ret < 0) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

int mbedtls_dtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    int fd, ret;

    if (!ctx) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    fd = ((mbedtls_net_context *)ctx)->fd;
    ret = tos_sal_module_sendto(fd, NULL, NULL, buf, len);

    if (ret == 0) {
        return MBEDTLS_ERR_SSL_WANT_WRITE;
    } else if (ret < 0) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    return ret;
}

void mbedtls_dtls_net_free(mbedtls_net_context *ctx)
{
    if (!ctx) {
        return;
    }

    tos_sal_module_close(ctx->fd);
}

#endif /* MBEDTLS_NET_C */

