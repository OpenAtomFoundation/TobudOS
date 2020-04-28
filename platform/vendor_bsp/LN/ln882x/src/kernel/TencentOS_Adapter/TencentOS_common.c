/**
 * @file freertos_common.c
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */
#include "proj_config.h"
#include "./TencentOS_Adapter/TencentOS_common.h"
#include "ln88xx.h"

#ifdef __CC_ARM
    extern unsigned int Image$$HEAP_SPACE$$ZI$$Base;
    extern unsigned int Image$$HEAP_SPACE$$ZI$$Limit;
    #define HEAP_START                      (&Image$$HEAP_SPACE$$ZI$$Base)
    #define HEAP_END                        (&Image$$HEAP_SPACE$$ZI$$Limit)
#elif __ICCARM__
    #error "TODO: support iar compiler!!!"
#elif __GNUC__
    #error "TODO: support gcc compiler!!!"
#else
    #error "Unknown compiler!!!"
#endif

#include "utils/debug/art_assert.h"
#include "utils/debug/log.h"

void os_heap_mem_add_pool(void)
{
    uint8_t * heap_pool0_base = (uint8_t *)((((uint32_t)HEAP_START) + sizeof(size_t) - 1U) & ~(sizeof(size_t)-1U));

    size_t pool0_size = ((uint32_t)HEAP_END - (uint32_t)heap_pool0_base);
    
    if(K_ERR_NONE != tos_mmheap_pool_add(heap_pool0_base, pool0_size)){
        LOG(LOG_LVL_ERROR,"[%s, %d]add pool fail.\r\n", __func__, __LINE__);
    };
}


void * OS_Malloc(size_t size)
{
    void * mem = NULL;
    
//    tos_knl_sched_lock();
    mem = tos_mmheap_alloc(size);
//    tos_knl_sched_unlock();
    ART_ASSERT(mem);
    return mem;
}

void OS_Free(void *p)
{
//    tos_knl_sched_lock();
    tos_mmheap_free(p);
//    tos_knl_sched_unlock();
}

