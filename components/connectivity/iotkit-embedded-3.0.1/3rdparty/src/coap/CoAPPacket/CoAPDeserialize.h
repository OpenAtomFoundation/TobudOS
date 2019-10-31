/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __COAP_DESERIALIZE_H__
#define __COAP_DESERIALIZE_H__
#include <stdio.h>
#include "iotx_coap_internal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int CoAPDeserialize_Message(CoAPMessage *msg, unsigned char *buf, int buflen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
