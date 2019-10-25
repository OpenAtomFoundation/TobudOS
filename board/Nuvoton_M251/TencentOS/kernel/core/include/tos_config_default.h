#ifndef _TOS_CONFIG_DEFAULT_H_
#define  _TOS_CONFIG_DEFAULT_H_

#ifndef TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN
#define TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN  0u
#endif

#ifndef TOS_CFG_ROUND_ROBIN_EN
#define  TOS_CFG_ROUND_ROBIN_EN             0u
#endif

#ifndef TOS_CFG_EVENT_EN
#define  TOS_CFG_EVENT_EN                   0u
#endif

#ifndef TOS_CFG_MMHEAP_EN
#define  TOS_CFG_MMHEAP_EN                  0u
#endif

#ifndef TOS_CFG_MUTEX_EN
#define  TOS_CFG_MUTEX_EN                   0u
#endif

#ifndef TOS_CFG_QUEUE_EN
#define  TOS_CFG_QUEUE_EN                   0u
#endif

#ifndef TOS_CFG_SEM_EN
#define  TOS_CFG_SEM_EN                     0u
#endif

#if     (TOS_CFG_QUEUE_EN > 0u) && !defined(TOS_CFG_MSG_EN)
#define  TOS_CFG_MSG_EN                   1u
#elif   (TOS_CFG_QUEUE_EN == 0u) && !defined(TOS_CFG_MSG_EN)
#define  TOS_CFG_MSG_EN                   0u
#endif

#ifndef TOS_CFG_TIMER_EN
#define  TOS_CFG_TIMER_EN                   0u
#endif

#if (TOS_CFG_TIMER_EN > 0u) && !defined(TOS_CFG_TIMER_AS_PROC)
#define TOS_CFG_TIMER_AS_PROC               0u
#endif

#ifndef TOS_CFG_MSG_POOL_SIZE
#define  TOS_CFG_MSG_POOL_SIZE              100u
#endif

#ifndef TOS_CFG_IDLE_TASK_STK_SIZE
#define  TOS_CFG_IDLE_TASK_STK_SIZE         128u
#endif

#ifndef TOS_CFG_OBJECT_VERIFY_EN
#define  TOS_CFG_OBJECT_VERIFY_EN               0u
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
#define  TOS_CFG_TASK_PRIO_MAX                  10u
#endif

#ifndef TOS_CFG_MMBLK_EN
#define TOS_CFG_MMBLK_EN                       0u
#endif

#ifndef TOS_CFG_MMHEAP_EN
#define TOS_CFG_MMHEAP_EN                       0u
#endif

#if     (TOS_CFG_MMHEAP_EN > 0u) && !defined(TOS_CFG_MMHEAP_POOL_SIZE)
#define  TOS_CFG_MMHEAP_POOL_SIZE               0x1000
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

#ifndef TOS_CFG_FAULT_BACKTRACE_EN
#define  TOS_CFG_FAULT_BACKTRACE_EN         0u
#endif

#endif /* _TOS_CONFIG_DEFAULT_H_ */

