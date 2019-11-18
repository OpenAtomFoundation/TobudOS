/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _TOS_CONFIG_CHECK_H_
#define  _TOS_CONFIG_CHECK_H_

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

#if     TOS_CFG_MMHEAP_EN == 0u
#error  "INVALID config, must enable tos_mmheap to use event-driven"
#endif

#if     TOS_CFG_TICKLESS_EN == 1u
#error  "INVALID config, tickless not supported in event-driven yet"
#endif

#if     (TOS_CFG_MMHEAP_EN > 0u) && (TOS_CFG_MMHEAP_DEFAULT_POOL_EN > 0u)
#if     !defined(TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE) || (TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE == 0u)
#error  "INVALID config, must define a valid TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE"
#endif
#endif

#else /* TOS_CFG_EVENT_DRIVEN_EN */

#if     TOS_CFG_TASK_PRIO_MAX < 8u
#error  "INVALID config, TOS_CFG_TASK_PRIO_MAX must be >= 8"
#endif

#if     (TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u) && (TOS_CFG_MMHEAP_EN == 0u)
#error  "INVALID config, must enable TOS_CFG_MMHEAP_EN to support dynamic task create"
#endif

#if     (TOS_CFG_PRIORITY_MESSAGE_QUEUE_EN > 0u) && (TOS_CFG_MMHEAP_EN == 0u)
#error  "INVALID config, must enable TOS_CFG_MMHEAP_EN to support tos_prio_msg_q"
#endif

#if     (TOS_CFG_PRIORITY_MAIL_QUEUE_EN > 0u) && (TOS_CFG_MMHEAP_EN == 0u)
#error  "INVALID config, must enable TOS_CFG_MMHEAP_EN to support tos_prio_mail_q"
#endif

#if     ((TOS_CFG_TIMER_EN > 0u) && !defined(TOS_CFG_TIMER_AS_PROC))
#error  "UNDECLARED config, TOS_CFG_TIMER_AS_PROC"
#endif

#if     (TOS_CFG_MMHEAP_EN > 0u) && (TOS_CFG_MMHEAP_DEFAULT_POOL_EN > 0u)
#if     !defined(TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE) || (TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE == 0u)
#error  "INVALID config, must define a valid TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE"
#endif
#endif

#if     (TOS_CFG_VFS_EN > 0u) && (TOS_CFG_MMHEAP_EN == 0u)
#error  "INVALID config, must enable TOS_CFG_MMHEAP_EN to use tos_vfs"
#endif

#ifndef  TOS_CFG_CPU_HRTIMER_EN
#error  "UNDECLARED config, TOS_CFG_CPU_HRTIMER_EN should be declared in 'port.h'"
#elif   (TOS_CFG_CPU_HRTIMER_EN > 0u) && !defined(TOS_CFG_CPU_HRTIMER_SIZE)
#error  "UNDECLARED config, TOS_CFG_CPU_HRTIMER_SIZE should be declared in 'port.h'"
#elif  ((TOS_CFG_CPU_HRTIMER_SIZE != CPU_WORD_SIZE_08) && \
        (TOS_CFG_CPU_HRTIMER_SIZE != CPU_WORD_SIZE_16) && \
        (TOS_CFG_CPU_HRTIMER_SIZE != CPU_WORD_SIZE_32) && \
        (TOS_CFG_CPU_HRTIMER_SIZE != CPU_WORD_SIZE_64))
#error  "INVALID config, TOS_CFG_CPU_HRTIMER_SIZE"
#endif

#ifndef  TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT
#error  "UNDECLARED config, TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT, should be declared in 'port.h'"
#endif

#ifndef  TOS_CFG_CPU_STK_GROWTH
#error  "UNDECLARED config, TOS_CFG_CPU_STK_GROWTH, should be declared in 'port.h'"
#elif  ((TOS_CFG_CPU_STK_GROWTH != CPU_STK_GROWTH_ASCENDING) && \
        (TOS_CFG_CPU_STK_GROWTH != CPU_STK_GROWTH_DESCENDING))
#error  "INVALID config, TOS_CFG_CPU_STK_GROWTH"
#endif

#ifndef  TOS_CFG_CPU_ADDR_SIZE
#error  "UNDECLARED config, TOS_CFG_CPU_ADDR_SIZE, should be declared in 'port.h'"
#elif  ((TOS_CFG_CPU_ADDR_SIZE != CPU_WORD_SIZE_08) && \
        (TOS_CFG_CPU_ADDR_SIZE != CPU_WORD_SIZE_16) && \
        (TOS_CFG_CPU_ADDR_SIZE != CPU_WORD_SIZE_32) && \
        (TOS_CFG_CPU_ADDR_SIZE != CPU_WORD_SIZE_64))
#error  "INVALID config, TOS_CFG_CPU_ADDR_SIZE"
#endif

#ifndef  TOS_CFG_CPU_DATA_SIZE
#error  "UNDECLARED config, TOS_CFG_CPU_DATA_SIZE, should be declared in 'port.h'"
#elif  ((TOS_CFG_CPU_DATA_SIZE != CPU_WORD_SIZE_08) && \
        (TOS_CFG_CPU_DATA_SIZE != CPU_WORD_SIZE_16) && \
        (TOS_CFG_CPU_DATA_SIZE != CPU_WORD_SIZE_32) && \
        (TOS_CFG_CPU_DATA_SIZE != CPU_WORD_SIZE_64))
#error  "INVALID config, TOS_CFG_CPU_DATA_SIZE"
#endif

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

#endif /* _TOS_CONFIG_CHECK_H_ */

