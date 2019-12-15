/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __COAP_SERIALIZE_H__
#define __COAP_SERIALIZE_H__
#include "iotx_coap_internal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

unsigned short CoAPSerialize_MessageLength(CoAPMessage *msg);

int CoAPSerialize_Message(CoAPMessage *msg, unsigned char *buf, unsigned short buflen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

