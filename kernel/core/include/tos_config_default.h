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

#ifndef _TOS_CONFIG_DEFAULT_H_
#define  _TOS_CONFIG_DEFAULT_H_

#ifndef TOS_CFG_EVENT_DRIVEN_EN
#define TOS_CFG_EVENT_DRIVEN_EN     0u
#endif

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

/////////////////////////////////////////
// disable round robin
#ifdef TOS_CFG_ROUND_ROBIN_EN
#undef  TOS_CFG_ROUND_ROBIN_EN
#endif
#define  TOS_CFG_ROUND_ROBIN_EN             0u
/////////////////////////////////////////


/////////////////////////////////////////
// disable dynamic task create
#ifdef TOS_CFG_TASK_DYNAMIC_CREATE_EN
#undef  TOS_CFG_TASK_DYNAMIC_CREATE_EN
#endif
#define TOS_CFG_TASK_DYNAMIC_CREATE_EN      0u
/////////////////////////////////////////


/////////////////////////////////////////
// disable event
#ifdef TOS_CFG_EVENT_EN
#undef  TOS_CFG_EVENT_EN
#endif
#define TOS_CFG_EVENT_EN                    0u
/////////////////////////////////////////


/////////////////////////////////////////
// disable mutex
#ifdef TOS_CFG_MUTEX_EN
#undef  TOS_CFG_MUTEX_EN
#endif
#define  TOS_CFG_MUTEX_EN                   0u
/////////////////////////////////////////


/////////////////////////////////////////
// disable semaphore
#ifdef TOS_CFG_SEM_EN
#undef  TOS_CFG_SEM_EN
#endif
#define TOS_CFG_SEM_EN                      0u
/////////////////////////////////////////


/////////////////////////////////////////
// disable countdownlatch
#ifdef TOS_CFG_COUNTDOWNLATCH_EN
#undef  TOS_CFG_COUNTDOWNLATCH_EN
#endif
#define TOS_CFG_COUNTDOWNLATCH_EN           0u
/////////////////////////////////////////


/////////////////////////////////////////
// disable completion
#ifdef TOS_CFG_COMPLETION_EN
#undef  TOS_CFG_COMPLETION_EN
#endif
#define TOS_CFG_COMPLETION_EN           0u
/////////////////////////////////////////


/////////////////////////////////////////
// disable the "traditional" timer
#ifdef TOS_CFG_TIMER_EN
#undef  TOS_CFG_TIMER_EN
#endif
#define TOS_CFG_TIMER_EN                    0u
/////////////////////////////////////////


/////////////////////////////////////////
// disable stack draught depth detact
#ifdef TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN
#undef  TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN
#endif
#define TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN  0u
/////////////////////////////////////////


/////////////////////////////////////////
// enable mmheap
#ifndef TOS_CFG_MMHEAP_EN
#define TOS_CFG_MMHEAP_EN                       1u
#endif

#ifndef TOS_CFG_MMHEAP_DEFAULT_POOL_EN
#define TOS_CFG_MMHEAP_DEFAULT_POOL_EN          1u
#endif
/////////////////////////////////////////


/////////////////////////////////////////
// disable default
#ifndef TOS_CFG_MMBLK_EN
#define TOS_CFG_MMBLK_EN                       0u
#endif
/////////////////////////////////////////


/////////////////////////////////////////
// disable default
#ifndef TOS_CFG_FAULT_BACKTRACE_EN
#define  TOS_CFG_FAULT_BACKTRACE_EN         0u
#endif
/////////////////////////////////////////

/////////////////////////////////////////
#ifndef TOS_CFG_CPU_SYSTICK_PRIO
#define  TOS_CFG_CPU_SYSTICK_PRIO               0u
#endif
/////////////////////////////////////////


/////////////////////////////////////////
// disable default
#ifndef TOS_CFG_PWR_MGR_EN
#define TOS_CFG_PWR_MGR_EN                  0u
#endif

#ifndef TOS_CFG_TICKLESS_EN
#define TOS_CFG_TICKLESS_EN                 0u
#endif


/////////////////////////////////////////
// we donot really need these, it's a compromise to the compiler.
#ifndef TOS_CFG_TASK_PRIO_MAX
#define  TOS_CFG_TASK_PRIO_MAX                  8u
#endif

#ifndef TOS_CFG_IDLE_TASK_STK_SIZE
#define  TOS_CFG_IDLE_TASK_STK_SIZE         128u
#endif
/////////////////////////////////////////


/////////////////////////////////////////

#else /* TOS_CFG_EVENT_DRIVEN_EN */

#ifndef TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN
#define  TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN  0u
#endif

#ifndef TOS_CFG_TASK_DYNAMIC_CREATE_EN
#define  TOS_CFG_TASK_DYNAMIC_CREATE_EN     0u
#endif

#ifndef TOS_CFG_ROUND_ROBIN_EN
#define  TOS_CFG_ROUND_ROBIN_EN             0u
#endif

#ifndef TOS_CFG_EVENT_EN
#define  TOS_CFG_EVENT_EN                   0u
#endif

#ifndef TOS_CFG_MUTEX_EN
#define  TOS_CFG_MUTEX_EN                   0u
#endif

#ifndef TOS_CFG_MESSAGE_QUEUE_EN
#define  TOS_CFG_MESSAGE_QUEUE_EN           0u
#endif

#ifndef TOS_CFG_MAIL_QUEUE_EN
#define  TOS_CFG_MAIL_QUEUE_EN              0u
#endif

#ifndef TOS_CFG_PRIORITY_MESSAGE_QUEUE_EN
#define  TOS_CFG_PRIORITY_MESSAGE_QUEUE_EN  0u
#endif

#ifndef TOS_CFG_PRIORITY_MAIL_QUEUE_EN
#define  TOS_CFG_PRIORITY_MAIL_QUEUE_EN     0u
#endif

#ifndef TOS_CFG_SEM_EN
#define  TOS_CFG_SEM_EN                     0u
#endif

#ifndef TOS_CFG_COUNTDOWNLATCH_EN
#define TOS_CFG_COUNTDOWNLATCH_EN           0u
#endif

#ifndef TOS_CFG_COMPLETION_EN
#define TOS_CFG_COMPLETION_EN               0u
#endif

#ifndef TOS_CFG_TIMER_EN
#define  TOS_CFG_TIMER_EN                   0u
#endif

#if (TOS_CFG_TIMER_EN > 0u) && !defined(TOS_CFG_TIMER_AS_PROC)
#define TOS_CFG_TIMER_AS_PROC               0u
#endif

#ifndef TOS_CFG_IDLE_TASK_STK_SIZE
#define  TOS_CFG_IDLE_TASK_STK_SIZE         128u
#endif

#ifndef TOS_CFG_OBJECT_VERIFY_EN
#define  TOS_CFG_OBJECT_VERIFY_EN               0u
#endif

#ifndef TOS_CFG_LIBC_PRINTF_EN
#define  TOS_CFG_LIBC_PRINTF_EN                     1u // we enable this by default
#endif

#if     (TOS_CFG_TIMER_AS_PROC == 0u) && !defined(TOS_CFG_TIMER_TASK_PRIO)
#define  TOS_CFG_TIMER_TASK_PRIO            (k_prio_t)(K_TASK_PRIO_IDLE - (k_prio_t)1u)
#endif

#if     (TOS_CFG_TIMER_AS_PROC == 0u) && !defined(TOS_CFG_TIMER_TASK_STK_SIZE)
#define  TOS_CFG_TIMER_TASK_STK_SIZE        128u
#endif

#ifndef TOS_CFG_CPU_SYSTICK_PRIO
#define  TOS_CFG_CPU_SYSTICK_PRIO               0u
#endif

#ifndef TOS_CFG_CPU_TICK_PER_SECOND
#define TOS_CFG_CPU_TICK_PER_SECOND             1000u
#endif

#ifndef TOS_CFG_CPU_CLOCK
#define TOS_CFG_CPU_CLOCK                        16000000u
#endif

#ifndef TOS_CFG_TASK_PRIO_MAX
#define  TOS_CFG_TASK_PRIO_MAX                  8u
#endif

#ifndef TOS_CFG_MMBLK_EN
#define TOS_CFG_MMBLK_EN                       0u
#endif

#ifndef TOS_CFG_MMHEAP_EN
#define TOS_CFG_MMHEAP_EN                       0u
#endif

#ifndef TOS_CFG_MMHEAP_DEFAULT_POOL_EN
#define TOS_CFG_MMHEAP_DEFAULT_POOL_EN          1u
#endif

#ifndef TOS_CFG_PWR_MGR_EN
#define TOS_CFG_PWR_MGR_EN                  0u
#endif

#ifndef TOS_CFG_TICKLESS_EN
#define TOS_CFG_TICKLESS_EN                 0u
#endif

#if (TOS_CFG_PWR_MGR_EN > 0u) || (TOS_CFG_TICKLESS_EN > 0u)
#if TOS_CFG_IDLE_TASK_STK_SIZE < 256
#undef TOS_CFG_IDLE_TASK_STK_SIZE
#define TOS_CFG_IDLE_TASK_STK_SIZE          256u
#endif
#endif

#if (TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u)
#if TOS_CFG_IDLE_TASK_STK_SIZE < 512
#undef TOS_CFG_IDLE_TASK_STK_SIZE
#define TOS_CFG_IDLE_TASK_STK_SIZE          512u
#endif
#endif

#ifndef TOS_CFG_FAULT_BACKTRACE_EN
#define  TOS_CFG_FAULT_BACKTRACE_EN         0u
#endif

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

#endif /* _TOS_CONFIG_DEFAULT_H_ */

