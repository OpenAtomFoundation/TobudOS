/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "iotx_coap_internal.h"
#include "CoAPExport.h"
#include "CoAPPlatform.h"
#include "CoAPNetwork.h"

int CoAPNetwork_read(NetworkContext         *p_context,
                     NetworkAddr     *p_remote,
                     unsigned char   *p_data,
                     unsigned int     datalen,
                     unsigned int     timeout_ms)

{
    int          len      = 0;
    NetworkConf  *network = NULL;

    if (NULL == p_context || NULL == p_remote || NULL == p_data) {
        return -1; /* TODO */
    }

    network = (NetworkConf *)p_context;
#ifdef COAP_DTLS_SUPPORT
    if (COAP_NETWORK_DTLS == network->type) {
    } else {
#endif
        len =  HAL_UDP_recvfrom(network->fd, p_remote, p_data,
                                datalen, timeout_ms);
        /* COAP_DEBUG("[CoAP-NWK]: Network read return %d", len); */
#ifdef COAP_DTLS_SUPPORT
    }
#endif
    return len;
}

int CoAPNetwork_write(NetworkContext          *p_context,
                      NetworkAddr   *p_remote,
                      const unsigned char  *p_data,
                      unsigned int          datalen,
                      unsigned int          timeout_ms)

{

    int          len      = 0;
    NetworkConf  *network = NULL;

    if (NULL == p_context || NULL == p_remote || NULL == p_data) {
        return -1; /* TODO */
    }

    network = (NetworkConf *)p_context;
#ifdef COAP_DTLS_SUPPORT
    /* TODO: */
    if (COAP_NETWORK_DTLS == network->type) {

    } else {
#endif
        len = HAL_UDP_sendto(network->fd, p_remote,
                             p_data, datalen, timeout_ms);
#ifdef COAP_DTLS_SUPPORT
    }
#endif
    return len;
}


NetworkContext *CoAPNetwork_init(const NetworkInit   *p_param)
{
    NetworkConf     *network = NULL;

    if (NULL == p_param) {
        return NULL;
    }

    network = coap_malloc(sizeof(NetworkConf));
    if (NULL == network) {
        return NULL;
    }

    memset(network, 0x00, sizeof(NetworkConf));
    network->type = p_param->type;

#ifdef COAP_DTLS_SUPPORT
    if (COAP_NETWORK_DTLS == network->type) {
        /* TODO: */
        coap_free(network);
        return NULL;
    } else {
#endif
        /*Create udp socket*/
        network->port = p_param->port;
        network->fd = (intptr_t)HAL_UDP_create_without_connect(NULL, network->port);
        if ((intptr_t) - 1 == network->fd) {
            coap_free(network);
            return NULL;
        }

        HAL_UDP_joinmulticast(network->fd, p_param->group);
#ifdef COAP_DTLS_SUPPORT
    }
#endif
    return (NetworkContext *)network;
}


void CoAPNetwork_deinit(NetworkContext *p_context)
{
    NetworkConf     *network = NULL;
    if (NULL == p_context) {
        return;
    }

    network = (NetworkConf *)p_context;
#ifdef COAP_DTLS_SUPPORT
    if (COAP_NETWORK_DTLS == network->type) {
        /* TODO: */
    } else {
#endif
        HAL_UDP_close_without_connect(network->fd);
        coap_free(p_context);
        p_context = NULL;
#ifdef COAP_DTLS_SUPPORT
    }
#endif
    return;
}

