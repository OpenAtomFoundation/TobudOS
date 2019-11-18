/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>

#ifndef COAP_TRANSPORT_H__
#define COAP_TRANSPORT_H__

typedef enum {
    COAP_ENDPOINT_NOSEC = 0,
    COAP_ENDPOINT_DTLS,
    COAP_ENDPOINT_PSK,
} coap_endpoint_type;


typedef struct {
    DTLSContext         *context;
} coap_remote_session_t;


typedef struct {
    int                      socket_id;
    coap_endpoint_type       ep_type;
    void                    *context;
} coap_network_t;


typedef struct {
    coap_endpoint_type       ep_type;
    unsigned char           *p_ca_cert_pem;
    char                    *p_host;
    unsigned short           port;
} coap_network_init_t;


unsigned int Cloud_CoAPNetwork_init(const coap_network_init_t    *p_param, coap_network_t *p_network);


unsigned int Cloud_CoAPNetwork_write(coap_network_t *p_network,
                                     const unsigned char   *p_data,
                                     unsigned int           datalen);

int Cloud_CoAPNetwork_read(coap_network_t *network, unsigned char  *data,
                           unsigned int datalen, unsigned int timeout);

unsigned int Cloud_CoAPNetwork_deinit(coap_network_t *p_network);


#endif

