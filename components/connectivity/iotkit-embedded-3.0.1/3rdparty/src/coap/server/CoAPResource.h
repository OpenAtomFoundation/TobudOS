/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __COAP_RESOURCE_H__
#define __COAP_RESOURCE_H__

#include <stdint.h>
#include "iotx_coap_internal.h"
#include "CoAPExport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define COAP_MAX_PATH_CHECKSUM_LEN (5)

typedef struct {
    unsigned short           permission;
    CoAPRecvMsgHandler       callback;
    unsigned int             ctype;
    unsigned int             maxage;
    struct list_head         reslist;
    char                     path[COAP_MAX_PATH_CHECKSUM_LEN];
    char                     *filter_path;
    path_type_t              path_type;
} CoAPResource;

int CoAPResource_init(CoAPContext *context, int res_maxcount);

int CoAPPathMD5_sum(const char *path, int len, char outbuf[], int outlen);

int CoAPResource_register(CoAPContext *context, const char *path,
                          unsigned short permission, unsigned int ctype,
                          unsigned int maxage, CoAPRecvMsgHandler callback);

CoAPResource *CoAPResourceByPath_get(CoAPContext *context, const char *path);
int CoAPResource_topicFilterMatch(const char *filter, const char *topic);
int CoAPResource_deinit(CoAPContext *context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
