#ifndef _TOS_CONFIG_H_
#define  _TOS_CONFIG_H_

#include "n32g020xx.h"

#define TOS_CFG_TICKLESS_EN             0u

#define TOS_CFG_TASK_PRIO_MAX           10u

#define TOS_CFG_ROUND_ROBIN_EN          1u

#define TOS_CFG_OBJECT_VERIFY_EN           0u

#define TOS_CFG_TASK_DYNAMIC_CREATE_EN  0u

#define TOS_CFG_EVENT_EN                1u

#define TOS_CFG_MMBLK_EN                1u

#define TOS_CFG_MMHEAP_EN               1u

#define TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE        0x2000

#define TOS_CFG_MUTEX_EN                1u

#define TOS_CFG_TIMER_EN                1u

#define TOS_CFG_PWR_MGR_EN              0u

#define TOS_CFG_TICKLESS_EN             0u

#define TOS_CFG_SEM_EN                  1u

#define TOS_CFG_IDLE_TASK_STK_SIZE      80u

#define TOS_CFG_CPU_TICK_PER_SECOND     1000u

#define TOS_CFG_CPU_CLOCK               (CGU_GetOscFrequence())

#define TOS_CFG_TIMER_AS_PROC           1u

#endif

