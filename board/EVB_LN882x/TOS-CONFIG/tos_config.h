#ifndef _TOS_CONFIG_H_
#define  _TOS_CONFIG_H_

#include "ln88xx.h"

#define TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN 1u
#define TOS_CFG_TICKLESS_EN                        0u
#define TOS_CFG_TASK_PRIO_MAX                      10u
#define TOS_CFG_TASK_DYNAMIC_CREATE_EN             1u
#define TOS_CFG_ROUND_ROBIN_EN                     1u
#define TOS_CFG_OBJECT_VERIFY_EN                   1u


#define TOS_CFG_EVENT_EN                 1u
#define TOS_CFG_MMBLK_EN                 1u
#define TOS_CFG_MMHEAP_EN                1u
#define TOS_CFG_MMHEAP_DEFAULT_POOL_EN   0u


#define TOS_CFG_MUTEX_EN                   1u
#define TOS_CFG_SEM_EN                     1u
#define TOS_CFG_MESSAGE_QUEUE_EN           1u
#define TOS_CFG_MAIL_QUEUE_EN              1u
#define TOS_CFG_PRIORITY_MESSAGE_QUEUE_EN  1u
#define TOS_CFG_PRIORITY_MAIL_QUEUE_EN     1u


#define TOS_CFG_IDLE_TASK_STK_SIZE      80u
#define TOS_CFG_CPU_TICK_PER_SECOND     1000u
#define TOS_CFG_CPU_CLOCK               160000000//(SystemCoreClock)


#define TOS_CFG_TIMER_EN                1u
#define TOS_CFG_TIMER_AS_PROC           0u
#define TOS_CFG_TIMER_TASK_STK_SIZE     1024u
#define TOS_CFG_TIMER_TASK_PRIO         0u






#endif

