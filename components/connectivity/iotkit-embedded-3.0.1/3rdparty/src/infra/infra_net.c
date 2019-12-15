/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "infra_config.h"

#ifdef INFRA_NET
#include <stdio.h>
#include <string.h>
#include "infra_defs.h"
#include "infra_net.h"
#include "wrappers_defs.h"

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define net_err(...)      log_err("infra_net", __VA_ARGS__)
#else
    #define net_err(...)
#endif



/*** SSL connection ***/
#ifdef  SUPPORT_TLS
void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define NET_MALLOC(size) LITE_malloc(size, MEM_MAGIC, "infra_net")
    #define NET_FREE(ptr)    LITE_free(ptr)
#else
    #define NET_MALLOC(size) HAL_Malloc(size)
    #define NET_FREE(ptr)    HAL_Free(ptr)
#endif

static void *ssl_malloc(uint32_t size)
{
    return NET_MALLOC(size);
}
static void ssl_free(void *ptr)
{
    NET_FREE(ptr);
}
#endif

#if  defined(SUPPORT_TLS)
uintptr_t HAL_SSL_Establish(const char *host, uint16_t port, const char *ca_crt, uint32_t ca_crt_len);
int32_t HAL_SSL_Destroy(uintptr_t handle);
int HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms);
int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms);
int ssl_hooks_set(ssl_hooks_t *hooks);
int HAL_GetProductKey(char *product_key);
int HAL_GetProductSecret(char *product_secret);

static int read_ssl(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        net_err("network is null");
        return -1;
    }

    return HAL_SSL_Read((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int write_ssl(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    if (NULL == pNetwork) {
        net_err("network is null");
        return -1;
    }

    return HAL_SSL_Write((uintptr_t)pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_ssl(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        net_err("network is null");
        return -1;
    }

    HAL_SSL_Destroy((uintptr_t)pNetwork->handle);
    pNetwork->handle = 0;

    return 0;
}

static int connect_ssl(utils_network_pt pNetwork)
{
    ssl_hooks_t ssl_hooks;

    if (NULL == pNetwork) {
        net_err("network is null");
        return 1;
    }

#ifdef INFRA_MEM_STATS
    memset(&ssl_hooks, 0, sizeof(ssl_hooks_t));
    ssl_hooks.malloc = ssl_malloc;
    ssl_hooks.free = ssl_free;

    ssl_hooks_set(&ssl_hooks);
#else
    (void)ssl_hooks;
#endif

    if (0 != (pNetwork->handle = (intptr_t)HAL_SSL_Establish(
            pNetwork->pHostAddress,
            pNetwork->port,
            pNetwork->ca_crt,
            pNetwork->ca_crt_len + 1))) {
        return 0;
    }
    else {
        /* TODO SHOLUD not remove this handle space */
        /* The space will be freed by calling disconnect_ssl() */
        /* utils_memory_free((void *)pNetwork->handle); */
#ifdef INFRA_EVENT
        iotx_event_post(IOTX_CONN_CLOUD_FAIL);
#endif
        return -1;
    }
}

#elif defined(AT_TCP_ENABLED)
uintptr_t AT_TCP_Establish(const char *host, uint16_t port);
int AT_TCP_Destroy(uintptr_t fd);
int32_t AT_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms);
int32_t AT_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms);
void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);

/*** TCP connection ***/
static int read_tcp(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return AT_TCP_Read(pNetwork->handle, buffer, len, timeout_ms);
}

static int write_tcp(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return AT_TCP_Write(pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_tcp(utils_network_pt pNetwork)
{
    if (pNetwork->handle == (uintptr_t)(-1)) {
        net_err("Network->handle = -1");
        return -1;
    }

    AT_TCP_Destroy(pNetwork->handle);
    pNetwork->handle = (uintptr_t)(-1);
    return 0;
}

static int connect_tcp(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        net_err("network is null");
        return 1;
    }

    pNetwork->handle = AT_TCP_Establish(pNetwork->pHostAddress, pNetwork->port);
    if (pNetwork->handle == (uintptr_t)(-1)) {
        return -1;
    }

    return 0;
}

#else
uintptr_t HAL_TCP_Establish(const char *host, uint16_t port);
int HAL_TCP_Destroy(uintptr_t fd);
int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms);
int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms);
void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);

/*** TCP connection ***/
static int read_tcp(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Read(pNetwork->handle, buffer, len, timeout_ms);
}


static int write_tcp(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Write(pNetwork->handle, buffer, len, timeout_ms);
}

static int disconnect_tcp(utils_network_pt pNetwork)
{
    if (pNetwork->handle == (uintptr_t)(-1)) {
        net_err("Network->handle = -1");
        return -1;
    }

    HAL_TCP_Destroy(pNetwork->handle);
    pNetwork->handle = (uintptr_t)(-1);
    return 0;
}

static int connect_tcp(utils_network_pt pNetwork)
{
    if (NULL == pNetwork) {
        net_err("network is null");
        return 1;
    }

    pNetwork->handle = HAL_TCP_Establish(pNetwork->pHostAddress, pNetwork->port);
    if (pNetwork->handle == (uintptr_t)(-1)) {
        return -1;
    }

    return 0;
}
#endif  /* #ifdef SUPPORT_TLS */

/****** network interface ******/
int utils_net_read(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms)
{
    int     ret = 0;
#ifdef SUPPORT_TLS
    if (NULL != pNetwork->ca_crt) {
        ret = read_ssl(pNetwork, buffer, len, timeout_ms);
    }
#else
    if (NULL == pNetwork->ca_crt) {
        ret = read_tcp(pNetwork, buffer, len, timeout_ms);
    }
#endif
    else {
        ret = -1;
        net_err("no method match!");
    }

    return ret;
}

int utils_net_write(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms)
{
    int     ret = 0;
#ifdef SUPPORT_TLS
    if (NULL != pNetwork->ca_crt) {
        ret = write_ssl(pNetwork, buffer, len, timeout_ms);
    }
#else
    if (NULL == pNetwork->ca_crt) {
        ret = write_tcp(pNetwork, buffer, len, timeout_ms);
    }
#endif

    else {
        ret = -1;
        net_err("no method match!");
    }

    return ret;
}

int iotx_net_disconnect(utils_network_pt pNetwork)
{
    int     ret = 0;
#ifdef SUPPORT_TLS
    if (NULL != pNetwork->ca_crt) {
        ret = disconnect_ssl(pNetwork);
    }
#else
    if (NULL == pNetwork->ca_crt) {
        ret = disconnect_tcp(pNetwork);
    }
#endif
    else {
        ret = -1;
        net_err("no method match!");
    }

    return  ret;
}

int iotx_net_connect(utils_network_pt pNetwork)
{
    int     ret = 0;
#ifdef SUPPORT_TLS
    if (NULL != pNetwork->ca_crt) {
        ret = connect_ssl(pNetwork);
    }
#else
    if (NULL == pNetwork->ca_crt) {
        ret = connect_tcp(pNetwork);
    }
#endif
    else {
        ret = -1;
        net_err("no method match!");
    }

    return ret;
}

int iotx_net_init(utils_network_pt pNetwork, const char *host, uint16_t port, const char *ca_crt)
{
    if (!pNetwork || !host) {
        net_err("parameter error! pNetwork=%p, host = %p", pNetwork, host);
        return -1;
    }
    pNetwork->pHostAddress = host;
    pNetwork->port = port;
    pNetwork->ca_crt = ca_crt;

    if (NULL == ca_crt) {
        pNetwork->ca_crt_len = 0;
    } else {
        pNetwork->ca_crt_len = strlen(ca_crt);
    }

    pNetwork->handle = 0;
    pNetwork->read = utils_net_read;
    pNetwork->write = utils_net_write;
    pNetwork->disconnect = iotx_net_disconnect;
    pNetwork->connect = iotx_net_connect;

    return 0;
}
#endif


