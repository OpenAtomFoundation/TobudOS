#ifndef _TOS_CONFIG_H_
#define  _TOS_CONFIG_H_

#include "stm32l4xx.h"

#define TOS_CFG_EVENT_DRIVEN_EN         1u

#define TOS_CFG_MMHEAP_EN               1u

#define TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE        0x8000

#define TOS_CFG_PWR_MGR_EN              0u

#define TOS_CFG_CPU_TICK_PER_SECOND     1000u

#define TOS_CFG_CPU_CLOCK               (SystemCoreClock)

#endif

