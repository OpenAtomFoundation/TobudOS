/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __COAP_PLATFORM_OS_H__
#define __COAP_PLATFORM_OS_H__
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define coap_malloc(size)            LITE_malloc(size, MEM_MAGIC, "coap.cloud")
    #define coap_free(ptr)               LITE_free(ptr)
#else
    #define coap_malloc(size)            HAL_Malloc(size)
    #define coap_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
