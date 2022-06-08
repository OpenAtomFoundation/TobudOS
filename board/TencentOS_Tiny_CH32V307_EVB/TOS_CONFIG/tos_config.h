#ifndef INC_TOS_CONFIG_H_
#define INC_TOS_CONFIG_H_

#include "ch32v30x.h"
#include "stddef.h"


#define TOS_CFG_TASK_PRIO_MAX 10u


#define TOS_CFG_ROUND_ROBIN_EN 0u


#define TOS_CFG_OBJECT_VERIFY 1u

#define TOS_CFG_OBJ_DYNAMIC_CREATE_EN  1u

#define TOS_CFG_EVENT_EN 1u

#define TOS_CFG_MMBLK_EN                1u

#define TOS_CFG_MMHEAP_EN 1u

#define TOS_CFG_MMHEAP_DEFAULT_POOL_EN  1u

#define TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE 0x8000

#define TOS_CFG_MAIL_QUEUE_EN           1u

#define TOS_CFG_PRIORITY_MESSAGE_QUEUE_EN   1u

#define TOS_CFG_PRIORITY_MAIL_QUEUE_EN  1u

#define TOS_CFG_MUTEX_EN                1u

#define TOS_CFG_MESSAGE_QUEUE_EN        1u

#define TOS_CFG_TIMER_EN                1u

#define TOS_CFG_SEM_EN                  1u

#define TOS_CFG_BARRIER_EN              1u

#define TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN      0u

#define TOS_CFG_CPU_SYSTICK_PRIO       0xF0     //V307���ȼ�����λ��Ч


#define TOS_CFG_IDLE_TASK_STK_SIZE 512u


#define TOS_CFG_IRQ_STK_SIZE    512u


#define TOS_CFG_CPU_TICK_PER_SECOND 1000u


#define TOS_CFG_CPU_CLOCK (SystemCoreClock)


#define TOS_CFG_TIMER_AS_PROC 1u



#endif /* INC_TOS_CONFIG_H_ */
