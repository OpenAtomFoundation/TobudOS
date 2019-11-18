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

/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016-2018 Armink (armink.ztl@gmail.com)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _PORT_H_
#define  _PORT_H_

#include <pthread.h>

#define SIG_SUSPEND					SIGUSR1
#define SIG_RESUME					SIGUSR2
#define SIG_CONTEXT_SWITCH          SIGRTMIN
#define SIG_TICK					SIGALRM
#define TIMER_TYPE					ITIMER_REAL

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
typedef struct port_fault_regs {
    uint32_t error_flags;
} port_fault_regs_t;

enum fault_info {
    //FAULT_INFO_XXX,
};

static const char *const fault_msg[] = {
        //[FAULT_INFO_XXX]      = "FAULT_INFO_XXX string msg",
};

__PORT__ void       port_fault_diagnosis(void);
#endif

#if defined(TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT) && (TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT == 1u)
__PORT__ uint32_t   port_clz(uint32_t val);
#endif

__PORT__ void       port_int_disable(void);

__PORT__ void       port_int_enable(void);

__PORT__ cpu_cpsr_t port_cpsr_save(void);

__PORT__ void       port_cpsr_restore(cpu_cpsr_t cpsr);

__PORT__ void       port_cpu_reset(void); //ok

__PORT__ void       port_sched_start(void) __NO_RETURN__;

__PORT__ void       port_context_switch(void);

__PORT__ void       port_irq_context_switch(void);

__PORT__ void       port_systick_config(uint32_t cycle_per_tick);

__PORT__ void       port_init(void);

__PORT__ pthread_t  port_create_thread(void *arg);

__PORT__ void       port_delay_ms(uint32_t ms);

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

#endif /* _PORT_H_ */

