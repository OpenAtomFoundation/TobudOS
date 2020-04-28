/* 
 * MODBUS Slave Library: A portable MODBUS slave for MODBUS ASCII/RTU/TCP.
 * Copyright (c) 2007 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * $Id$
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* ----------------------- Defines ------------------------------------------*/

#define vPortYieldProcessor               swi_handler

#define configUSE_PREEMPTION              ( 1 )
#define configUSE_IDLE_HOOK               ( 0 )
#define configUSE_TICK_HOOK               ( 1 )
#define configCPU_CLOCK_HZ                ( ( unsigned portLONG ) 48054857UL )
#define configTICK_RATE_HZ                ( ( portTickType ) 1000 )
#define configMAX_PRIORITIES              ( ( unsigned portBASE_TYPE ) 5 )
#define configMINIMAL_STACK_SIZE          ( ( unsigned portSHORT ) 128 )
#define configTOTAL_HEAP_SIZE             ( ( size_t ) 2048 )
#define configMAX_TASK_NAME_LEN           ( 16 )
#define configUSE_TRACE_FACILITY          ( 1 )
#define configUSE_16_BIT_TICKS            ( 0 )
#define configIDLE_SHOULD_YIELD           ( 1 )
#define configUSE_CO_ROUTINES             ( 0 )
#define configMAX_CO_ROUTINE_PRIORITIES   ( 2 )

#define INCLUDE_vTaskPrioritySet          ( 1 )
#define INCLUDE_uxTaskPriorityGet         ( 1 )
#define INCLUDE_vTaskDelete               ( 1 )
#define INCLUDE_vTaskCleanUpResources     ( 0 )
#define INCLUDE_vTaskSuspend              ( 1 )
#define INCLUDE_vTaskDelayUntil           ( 1 )
#define INCLUDE_vTaskDelay                ( 1 )
#define INCLUDE_xTaskGetCurrentTaskHandle ( 1 )

#endif
