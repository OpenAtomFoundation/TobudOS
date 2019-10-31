#ifndef _DYNREG_INTERNAL_H_
#define _DYNREG_INTERNAL_H_

#include "dynreg_wrapper.h"

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define dynreg_info(...)                log_info("dynreg", __VA_ARGS__)
    #define dynreg_err(...)                 log_err("dynreg", __VA_ARGS__)
    #define dynreg_dbg(...)                 log_debug("dynreg", __VA_ARGS__)
#else
    #define dynreg_info(...)                do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
    #define dynreg_err(...)                 do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
    #define dynreg_dbg(...)                 do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#endif

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define dynreg_malloc(size)             LITE_malloc(size, MEM_MAGIC, "dynreg")
    #define dynreg_free(ptr)                LITE_free(ptr)
#else
    #define dynreg_malloc(size)             HAL_Malloc(size)
    #define dynreg_free(ptr)                {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#endif

