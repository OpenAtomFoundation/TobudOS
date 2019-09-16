#ifndef _TOS_CONFIG_CHECK_H_
#define  _TOS_CONFIG_CHECK_H_

#if     TOS_CFG_TASK_PRIO_MAX < 8u
#error  "INVALID config, TOS_CFG_TASK_PRIO_MAX must be >= 8"
#endif

#if     (TOS_CFG_QUEUE_EN > 0u) && (TOS_CFG_MSG_EN == 0u)
#error  "INVALID config, must enable tos_msg to use tos_queue"
#endif

#if     ((TOS_CFG_TIMER_EN > 0u) && !defined(TOS_CFG_TIMER_AS_PROC))
#error  "UNDECLARED config, TOS_CFG_TIMER_AS_PROC"
#endif

#if     (TOS_CFG_VFS_EN > 0u) && (TOS_CFG_MMHEAP_EN == 0u)
#error  "INVALID config, must enable tos_mmheap to use tos_vfs"
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

#endif /* _TOS_CHECK_CONFIG_H_ */

