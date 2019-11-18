#ifndef INC_TOS_CONFIG_H_
#define INC_TOS_CONFIG_H_

#include "gd32vf103.h"
#include "stddef.h"

// 配置TencentOS tiny默认支持的最大优先级数量
#define TOS_CFG_TASK_PRIO_MAX 10u

// 配置TencentOS tiny的内核是否开启时间片轮转
#define TOS_CFG_ROUND_ROBIN_EN 0u

// 配置TencentOS tiny是否校验指针合法
#define TOS_CFG_OBJECT_VERIFY_EN 0u

#define TOS_CFG_TASK_DYNAMIC_CREATE_EN  0u

// TencentOS tiny 事件模块功能宏
#define TOS_CFG_EVENT_EN 1u

// 配置TencentOS tiny是否开启动态内存模块
#define TOS_CFG_MMHEAP_EN 1u

// 配置TencentOS tiny动态内存池大小
#define TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE 8192

// 配置TencentOS tiny是否开启互斥锁模块
#define TOS_CFG_MUTEX_EN 1u

// 配置TencentOS tiny是否开启软件定时器模块
#define TOS_CFG_TIMER_EN 0u

// 配置TencentOS tiny是否开启信号量模块
#define TOS_CFG_SEM_EN 1u

#define TOS_CFG_CPU_SYSTICK_PRIO 0xF

// 配置TencentOS tiny空闲任务栈大小
#define TOS_CFG_IDLE_TASK_STK_SIZE 512u

// 配置TencentOS tiny中断栈大小
#define TOS_CFG_IRQ_STK_SIZE 128u

// 配置TencentOS tiny的tick频率
#define TOS_CFG_CPU_TICK_PER_SECOND 1000u

// 配置TencentOS tiny CPU频率
// 除4的原因是，Bumblebee内核通过core_clk_aon四分频来更新mtime寄存器
// 具体信息参见《Bumblebee内核简明数据手册》
#define TOS_CFG_CPU_CLOCK (SystemCoreClock/4)

// 配置是否将TIMER配置成函数模式
#define TOS_CFG_TIMER_AS_PROC 1u

#define TOS_CFG_VFS_EN 1u

#define TOS_CFG_MMBLK_EN 1u


#endif /* INC_TOS_CONFIG_H_ */
