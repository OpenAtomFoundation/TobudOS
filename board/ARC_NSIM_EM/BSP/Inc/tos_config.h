#ifndef _TOS_CONFIG_H_
#define _TOS_CONFIG_H_

#include "stddef.h"
#include "embARC.h"

#define TOS_CFG_TASK_PRIO_MAX 10u

#define TOS_CFG_ROUND_ROBIN_EN 1u

#define TOS_CFG_OBJECT_VERIFY 0u

#define TOS_CFG_EVENT_EN 1u

#define TOS_CFG_MMHEAP_EN 1u

#define TOS_CFG_MMHEAP_POOL_SIZE (1024*20)

#define TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE (1024*20)

#define TOS_CFG_MUTEX_EN    1u 

#define TOS_CFG_QUEUE_EN    1u

#define TOS_CFG_TIMER_EN    1u

#define TOS_CFG_SEM_EN      1u

#define TOS_CFG_MMBLK_EN    1u

#if (TOS_CFG_QUEUE_EN > 0u)
#define TOS_CFG_MSG_EN 1u
#else
#define TOS_CFG_MSG_EN 0u
#endif

#define TOS_CFG_MSG_POOL_SIZE 10u

#define TOS_CFG_IDLE_TASK_STK_SIZE 256u

#define TOS_CFG_CPU_TICK_PER_SECOND 100u

#define TOS_CFG_CPU_CLOCK BOARD_CPU_CLOCK

#define TOS_CFG_TIMER_AS_PROC 1u

#endif