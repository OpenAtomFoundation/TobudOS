/**
 * @file freertos_common.c
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */
#include "proj_config.h"
#include "./FreeRTOS_Adapter/freertos_common.h"
#include "ln88xx.h"

#ifdef __CC_ARM
    extern unsigned int Image$$HEAP_SPACE0$$ZI$$Base;
    extern unsigned int Image$$HEAP_SPACE0$$ZI$$Limit;
    extern unsigned int Image$$HEAP_SPACE1$$ZI$$Base;
    extern unsigned int Image$$HEAP_SPACE1$$ZI$$Limit;
    #define HEAP0_START                      (&Image$$HEAP_SPACE0$$ZI$$Base)
    #define HEAP0_END                        (&Image$$HEAP_SPACE0$$ZI$$Limit)
    #define HEAP1_START                      (&Image$$HEAP_SPACE1$$ZI$$Base)
    #define HEAP1_END                        (&Image$$HEAP_SPACE1$$ZI$$Limit)
#elif __ICCARM__
    #error "TODO: support iar compiler!!!"
#elif __GNUC__
    #error "TODO: support gcc compiler!!!"
#else
    #error "Unknown compiler!!!"
#endif


static HeapRegion_t xHeapRegions[] = {
    {NULL, 0},
    {NULL, 0},
    {NULL, 0}
};

void OS_HeapSizeConfig(void)
{
    xHeapRegions[0].pucStartAddress = (uint8_t *)(HEAP0_START);
    xHeapRegions[0].xSizeInBytes    = (size_t)   ((uint8_t *)HEAP0_END - (uint8_t *)HEAP0_START);

    xHeapRegions[1].pucStartAddress = (uint8_t *)(HEAP1_START);
    xHeapRegions[1].xSizeInBytes    = (size_t)   ((uint8_t *)HEAP1_END - (uint8_t *)HEAP1_START);

    xHeapRegions[2].pucStartAddress = NULL;
    xHeapRegions[2].xSizeInBytes    = 0;
}

void OS_DefineHeapRegions(void)
{
    OS_HeapSizeConfig();
    vPortDefineHeapRegions(xHeapRegions);
}


