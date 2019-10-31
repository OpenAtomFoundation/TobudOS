/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __COAP_PLATFORM_OS_H__
#define __COAP_PLATFORM_OS_H__

#include <stdio.h>
#include "iotx_coap_internal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define coap_malloc(size)            LITE_malloc(size, MEM_MAGIC, "coap.local")
    #define coap_free(ptr)               LITE_free(ptr)
#else
    #define coap_malloc(size)            HAL_Malloc(size)
    #define coap_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define COAP_ERR(...)           log_err("coap_local", __VA_ARGS__)
    #define COAP_WRN(...)           log_warning("coap_local", __VA_ARGS__)
    #define COAP_INFO(...)          log_info("coap_local", __VA_ARGS__)
    #define COAP_TRC(...)           log_debug("coap_local", __VA_ARGS__)
    #define COAP_DUMP(...)          log_debug("coap_local", __VA_ARGS__)
    #define COAP_DEBUG(...)         log_debug("coap_local", __VA_ARGS__)
    #define COAP_FLOW(...)          log_flow("coap_local", __VA_ARGS__)
#else
    #define COAP_ERR(...)   
    #define COAP_WRN(...)   
    #define COAP_INFO(...)  
    #define COAP_TRC(...)   
    #define COAP_DUMP(...)  
    #define COAP_DEBUG(...) 
    #define COAP_FLOW(...)  
#endif

int platform_is_multicast(const char *ip_str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
