 #ifndef TOS_CONFIG_H
#define  TOS_CONFIG_H

#include "stm32f10x.h"	
#include <string.h>
// 目标芯片头文件，用户需要根据情况更改

#define TOS_CFG_TASK_PRIO_MAX           10u			// 配置TencentOS tiny默认支持的最大优先级数量

#define TOS_CFG_ROUND_ROBIN_EN          1u				// 配置TencentOS tiny的内核是否开启时间片轮转

#define TOS_CFG_OBJECT_VERIFY           0u			// 配置TencentOS tiny是否校验指针合法

#define TOS_CFG_EVENT_EN                1u				// TencentOS tiny 事件模块功能宏

#define TOS_CFG_MMBLK_EN                1u   		//配置TencentOS tiny是否开启内存块管理模块

#define TOS_CFG_MMHEAP_EN               1u		// 配置TencentOS tiny是否开启动态内存模块

#define TOS_CFG_MMHEAP_POOL_SIZE        0x100	// 配置TencentOS tiny动态内存池大小

#define TOS_CFG_MUTEX_EN                1u		// 配置TencentOS tiny是否开启互斥锁模块

#define TOS_CFG_QUEUE_EN                1u		// 配置TencentOS tiny是否开启队列模块

#define TOS_CFG_TIMER_EN                1u		// 配置TencentOS tiny是否开启软件定时器模块

#define TOS_CFG_SEM_EN                  1u		// 配置TencentOS tiny是否开启信号量模块

#define TOS_CFG_TICKLESS_EN             0u   // 配置Tickless 低功耗模块开关

#if (TOS_CFG_QUEUE_EN > 0u)
#define TOS_CFG_MSG_EN     1u
#else
#define TOS_CFG_MSG_EN     0u
#endif

#define TOS_CFG_MSG_POOL_SIZE           10u		// 配置TencentOS tiny消息队列大小

#define TOS_CFG_IDLE_TASK_STK_SIZE      128u		// 配置TencentOS tiny空闲任务栈大小

#define TOS_CFG_CPU_TICK_PER_SECOND     1000u	// 配置TencentOS tiny的tick频率

#define TOS_CFG_CPU_CLOCK     (SystemCoreClock)	// 配置TencentOS tiny CPU频率

#define TOS_CFG_TIMER_AS_PROC           1u		// 配置是否将TIMER配置成函数模式

#endif



