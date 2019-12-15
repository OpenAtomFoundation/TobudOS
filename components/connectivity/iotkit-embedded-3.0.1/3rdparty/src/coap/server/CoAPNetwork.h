/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __CoAPNETWORK_H__
#define __CoAPNETWORK_H__
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum {
    COAP_NETWORK_NOSEC = 0,
    COAP_NETWORK_DTLS,
} CoAPNetworkType;

typedef struct {
    CoAPNetworkType       type;
    unsigned short        port;
    intptr_t             fd;
} NetworkConf;

typedef void NetworkContext;


typedef struct {
    CoAPNetworkType       type;
    char                  *group;
    unsigned short        port;
#ifdef COAP_DTLS_SUPPORT
    /* TODO: */
#endif
} NetworkInit;

NetworkContext *CoAPNetwork_init(const NetworkInit   *p_param);


int CoAPNetwork_write(NetworkContext          *p_context,
                      NetworkAddr   *p_remote,
                      const unsigned char  *p_data,
                      unsigned int          datalen,
                      unsigned int          timeout);

int CoAPNetwork_read(NetworkContext *p_context,
                     NetworkAddr    *p_remote,
                     unsigned char  *p_data,
                     unsigned int datalen,
                     unsigned int timeout);

void CoAPNetwork_deinit(NetworkContext *p_context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

