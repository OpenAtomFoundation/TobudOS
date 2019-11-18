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

#ifndef _PORT_H_
#define  _PORT_H_

__PORT__ void       port_int_disable(void);

__PORT__ void       port_int_enable(void);

__PORT__ cpu_cpsr_t port_cpsr_save(void);

__PORT__ void       port_cpsr_restore(cpu_cpsr_t cpsr);

__PORT__ void       port_cpu_reset(void);

__PORT__ void       port_sched_start(void) __NO_RETURN__;

__PORT__ void       port_context_switch(void);

__PORT__ void       port_irq_context_switch(void);

__PORT__ void       port_systick_config(uint32_t cycle_per_tick);

__PORT__ void       port_systick_priority_set(uint32_t prio);

#if TOS_CFG_TICKLESS_EN > 0u

__PORT__ void       port_systick_resume(void);

__PORT__ void       port_systick_suspend(void);

__PORT__ void       port_systick_reload(uint32_t cycle_per_tick);

__PORT__ void       port_systick_pending_reset(void);

__PORT__ k_time_t   port_systick_max_delay_millisecond(void);

#endif

#if TOS_CFG_PWR_MGR_EN > 0u

__PORT__ void       port_sleep_mode_enter(void);

__PORT__ void       port_stop_mode_enter(void);

__PORT__ void       port_standby_mode_enter(void);

#endif


#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
__PORT__ void       HardFault_Handler(void);

__PORT__ void       port_fault_diagnosis(void);
#endif

#endif /* _PORT_H_ */

