/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _INFRA_NET_H_
#define _INFRA_NET_H_

#include "infra_types.h"

/**
 * @brief The structure of network connection(TCP or SSL).
 *   The user has to allocate memory for this structure.
 */

struct utils_network;
typedef struct utils_network utils_network_t, *utils_network_pt;

struct utils_network {
    const char *pHostAddress;
    uint16_t port;
    uint16_t ca_crt_len;

    /**< NULL, TCP connection; NOT NULL, SSL connection */
    const char *ca_crt;
    /**< NOT NULL,iTLS connection*/
    char *product_key;
    /**< connection handle: 0, NOT connection; NOT 0, handle of the connection */
    uintptr_t handle;

    /**< Read data from server function pointer. */
    int (*read)(utils_network_pt, char *, uint32_t, uint32_t);

    /**< Send data to server function pointer. */
    int (*write)(utils_network_pt, const char *, uint32_t, uint32_t);

    /**< Disconnect the network */
    int (*disconnect)(utils_network_pt);

    /**< Establish the network */
    int (*connect)(utils_network_pt);
};

int utils_net_read(utils_network_pt pNetwork, char *buffer, uint32_t len, uint32_t timeout_ms);
int utils_net_write(utils_network_pt pNetwork, const char *buffer, uint32_t len, uint32_t timeout_ms);
int iotx_net_disconnect(utils_network_pt pNetwork);
int iotx_net_connect(utils_network_pt pNetwork);
int iotx_net_init(utils_network_pt pNetwork, const char *host, uint16_t port, const char *ca_crt);

#endif /* IOTX_COMMON_NET_H */


