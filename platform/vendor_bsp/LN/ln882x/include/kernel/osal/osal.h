/**
 * @file   osal.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef __OSAL_H__
#define __OSAL_H__
#include "proj_config.h"

#if (__CONFIG_OS_KERNEL == RTOS_FREERTOS)
  #include "./FreeRTOS/adapter_common.h"
  #include "./FreeRTOS/adapter_cpuusage.h"
  #include "./FreeRTOS/adapter_errno.h"
  #include "./FreeRTOS/adapter_mutex.h"
  #include "./FreeRTOS/adapter_queue.h"
  #include "./FreeRTOS/adapter_semaphore.h"
  #include "./FreeRTOS/adapter_thread.h"
  #include "./FreeRTOS/adapter_time.h"
  #include "./FreeRTOS/adapter_timer.h"
#elif (__CONFIG_OS_KERNEL == RTOS_TENCENT_OS)
  #include "./TencentOS_Adapter/TencentOS_common.h"
//  #include "./TencentOS_Adapter/TencentOS_cpuusage.h"
//  #include "./TencentOS_Adapter/TencentOS_errno.h"
  #include "./TencentOS_Adapter/TencentOS_mutex.h"
  #include "./TencentOS_Adapter/TencentOS_queue.h"
  #include "./TencentOS_Adapter/TencentOS_semaphore.h"
  #include "./TencentOS_Adapter/TencentOS_thread.h"
  #include "./TencentOS_Adapter/TencentOS_time.h"
  #include "./TencentOS_Adapter/TencentOS_timer.h"
#elif (__CONFIG_OS_KERNEL == RTOS_RT_THREAD)
  #error "No OS Kernel defined!"
#else 
  #error "No OS Kernel defined!"
#endif

#endif /* __OSAL_H__ */
