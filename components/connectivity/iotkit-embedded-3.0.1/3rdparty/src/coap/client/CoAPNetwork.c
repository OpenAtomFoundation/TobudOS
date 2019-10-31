/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "iotx_coap_internal.h"
#include "Cloud_CoAPExport.h"
#include "Cloud_CoAPNetwork.h"

#ifdef COAP_DTLS_SUPPORT
static void *Cloud_CoAPDTLS_Malloc(uint32_t size)
{
#ifdef INFRA_MEM_STATS
    return LITE_malloc(size, MEM_MAGIC, "dtls");
#else
    return HAL_Malloc(size);
#endif
}
static void Cloud_CoAPDTLS_Free(void *ptr)
{
#ifdef INFRA_MEM_STATS
    LITE_free(ptr);
#else
    HAL_Free((void *)ptr);
#endif
}

static void Cloud_CoAPNetworkDTLS_freeSession(void *p_session);

unsigned int Cloud_CoAPNetworkDTLS_read(void *p_session,
                                        unsigned char              *p_data,
                                        unsigned int               *p_datalen,
                                        unsigned int                timeout)
{
    unsigned int           err_code  = DTLS_SUCCESS;
    const unsigned int     read_len  = *p_datalen;
    DTLSContext           *context   = NULL;

    COAP_TRC("<< secure_datagram_read, read buffer len %d, timeout %d", read_len, timeout);
    (void)read_len;
    if (NULL != p_session) {
        /* read dtls application data*/
        context = (DTLSContext *)p_session;
        err_code = HAL_DTLSSession_read(context, p_data, p_datalen, timeout);
        if (DTLS_PEER_CLOSE_NOTIFY == err_code
            || DTLS_FATAL_ALERT_MESSAGE  == err_code) {
            COAP_INFO("dtls session read failed, return (0x%04x)", err_code);
            Cloud_CoAPNetworkDTLS_freeSession(context);
        }
        if (DTLS_SUCCESS == err_code) {
            return COAP_SUCCESS;
        } else {
            return COAP_ERROR_READ_FAILED;
        }
    }

    return COAP_ERROR_INVALID_PARAM;
}

unsigned int Cloud_CoAPNetworkDTLS_write(void *p_session,
        const unsigned char        *p_data,
        unsigned int               *p_datalen)
{
    unsigned int err_code = DTLS_SUCCESS;
    if (NULL != p_session) {
        err_code =  HAL_DTLSSession_write((DTLSContext *)p_session, p_data, p_datalen);
        if (DTLS_SUCCESS == err_code) {
            return COAP_SUCCESS;
        } else {
            return COAP_ERROR_WRITE_FAILED;
        }
    }
    return COAP_ERROR_INVALID_PARAM;
}

static  void Cloud_CoAPNetworkDTLS_freeSession(void *p_session)
{
    /* Free the session.*/
    HAL_DTLSSession_free((DTLSContext *)p_session);
}

void *Cloud_CoAPNetworkDTLS_createSession(char *p_host,
        unsigned short         port,
        unsigned char         *p_ca_cert_pem)
{
    DTLSContext *context = NULL;
    dtls_hooks_t dtls_hooks;
    coap_dtls_options_t dtls_options;

    memset(&dtls_hooks, 0, sizeof(dtls_hooks_t));
    dtls_hooks.malloc = Cloud_CoAPDTLS_Malloc;
    dtls_hooks.free = Cloud_CoAPDTLS_Free;

    HAL_DTLSHooks_set(&dtls_hooks);

    memset(&dtls_options, 0x00, sizeof(coap_dtls_options_t));
    dtls_options.p_ca_cert_pem     = p_ca_cert_pem;
    dtls_options.p_host            = p_host;
    dtls_options.port              = port;

    context = HAL_DTLSSession_create(&dtls_options);
    return (void *)context;
}

#endif

unsigned int Cloud_CoAPNetwork_write(coap_network_t *p_network,
                                     const unsigned char   *p_data,
                                     unsigned int           datalen)
{
    int rc = COAP_ERROR_WRITE_FAILED;

#ifdef COAP_DTLS_SUPPORT
    if (COAP_ENDPOINT_DTLS == p_network->ep_type) {
        rc = Cloud_CoAPNetworkDTLS_write(p_network->context, p_data, &datalen);
    } else {
#endif
        rc = HAL_UDP_write((intptr_t)p_network->context, p_data, datalen);
        COAP_DEBUG("[CoAP-NWK]: Network write return %d", rc);

        if (-1 == rc) {
            rc = COAP_ERROR_WRITE_FAILED;
        } else {
            rc = COAP_SUCCESS;
        }
#ifdef COAP_DTLS_SUPPORT
    }
#endif
    return (unsigned int)rc;
}

int Cloud_CoAPNetwork_read(coap_network_t *network, unsigned char  *data,
                           unsigned int datalen, unsigned int timeout)
{
    int len = 0;

#ifdef COAP_DTLS_SUPPORT
    if (COAP_ENDPOINT_DTLS == network->ep_type)  {
        len = datalen;
        memset(data, 0x00, datalen);
        Cloud_CoAPNetworkDTLS_read(network->context, data, (unsigned int *)&len, timeout);
    } else {
#endif
        memset(data, 0x00, datalen);
        len = HAL_UDP_readTimeout((intptr_t)network->context,
                                  data, COAP_MSG_MAX_PDU_LEN, timeout);
#ifdef COAP_DTLS_SUPPORT
    }
#endif
    if (len > 0) {
        COAP_TRC("<< CoAP recv %d bytes data", len);
    }
    return len;
}

unsigned int Cloud_CoAPNetwork_init(const coap_network_init_t *p_param, coap_network_t *p_network)
{
    unsigned int    err_code = COAP_SUCCESS;

    if (NULL == p_param || NULL == p_network) {
        return COAP_ERROR_INVALID_PARAM;
    }

    /* TODO : Parse the url here */
    p_network->ep_type = p_param->ep_type;

#ifdef COAP_DTLS_SUPPORT
    if (COAP_ENDPOINT_DTLS == p_param->ep_type) {
        p_network->context = Cloud_CoAPNetworkDTLS_createSession(p_param->p_host,
                             p_param->port, p_param->p_ca_cert_pem);
        if (NULL == p_network->context) {
            return COAP_ERROR_NET_INIT_FAILED;
        }
    }
#endif
    if (COAP_ENDPOINT_NOSEC == p_param->ep_type
        || COAP_ENDPOINT_PSK == p_param->ep_type) {
        /*Create udp socket*/
        p_network->context = (void *)HAL_UDP_create(p_param->p_host, p_param->port);
        if ((void *) - 1 == p_network->context) {
            return COAP_ERROR_NET_INIT_FAILED;
        }
    }
    return err_code;
}


unsigned int Cloud_CoAPNetwork_deinit(coap_network_t *p_network)
{
    unsigned int    err_code = COAP_SUCCESS;

#ifdef COAP_DTLS_SUPPORT
    if (COAP_ENDPOINT_DTLS == p_network->ep_type) {
        Cloud_CoAPNetworkDTLS_freeSession(p_network->context);
        p_network->context = NULL;
    }
#endif
    if (COAP_ENDPOINT_NOSEC == p_network->ep_type
        || COAP_ENDPOINT_PSK == p_network->ep_type) {
        HAL_UDP_close_without_connect((intptr_t)p_network->context);
    }

    return err_code;
}

