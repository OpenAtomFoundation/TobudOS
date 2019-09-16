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

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
typedef struct port_fault_regs {
    union {
        /* System Handler Control and State Register (0xE000ED24) */
        uint32_t value;
        struct {
            /* Read as 1 if memory management fault is active */
            uint32_t MEMFAULTACT    : 1;
            /* Read as 1 if bus fault exception is active */
            uint32_t BUSFAULTACT    : 1;
            uint32_t UnusedBits1    : 1;
            /* Read as 1 if usage fault exception is active */
            uint32_t USGFAULTACT    : 1;
            uint32_t UnusedBits2    : 3;
            /* Read as 1 if SVC exception is active */
            uint32_t SVCALLACT      : 1;
            /* Read as 1 if debug monitor exception is active */
            uint32_t MONITORACT     : 1;
            uint32_t UnusedBits3    : 1;
            /* Read as 1 if PendSV exception is active */
            uint32_t PENDSVACT      : 1;
            /* Read as 1 if SYSTICK exception is active */
            uint32_t SYSTICKACT     : 1;
            /* Usage fault pended; usage fault started but was replaced by a higher-priority exception */
            uint32_t USGFAULTPENDED : 1;
            /* Memory management fault pended; memory management fault started but was replaced by a
               higher-priority exception */
            uint32_t MEMFAULTPENDED : 1;
            /* Bus fault pended; bus fault handler was started but was replaced by a higher-priority
               exception */
            uint32_t BUSFAULTPENDED : 1;
            /* SVC pended; SVC was started but was replaced by a higher-priority exception */
            uint32_t SVCALLPENDED   : 1;
            /* Memory management fault handler enable */
            uint32_t MEMFAULTENA    : 1;
            /* Bus fault handler enable */
            uint32_t BUSFAULTENA    : 1;
            /* Usage fault handler enable */
            uint32_t USGFAULTENA    : 1;
        } bits;
    } syshndctrl;                          

    union {
        uint32_t value;

        struct {
            union {
                /* Memory Management Fault Status Register (0xE000ED28) */
                uint8_t value;
                struct {
                    /* Instruction access violation */
                    uint8_t IACCVIOL    : 1;
                    /* Data access violation */
                    uint8_t DACCVIOL    : 1;
                    uint8_t UnusedBits  : 1;
                    /* Unstacking error */
                    uint8_t MUNSTKERR   : 1;
                    /* Stacking error */
                    uint8_t MSTKERR     : 1;
                    /* Floating-point lazy state preservation (M4/M7) */
                    uint8_t MLSPERR     : 1;
                    uint8_t UnusedBits2 : 1;
                    /* Indicates the MMAR is valid */
                    uint8_t MMARVALID   : 1;
                } bits;
            } mfsr;

            union {
                /* Bus Fault Status Register (0xE000ED29) */
                uint8_t value;
                struct {
                    /* Instruction access violation */
                    uint8_t IBUSERR    : 1;
                    /* Precise data access violation */
                    uint8_t PRECISERR  : 1;
                    /* Imprecise data access violation */
                    uint8_t IMPREISERR : 1;
                    /* Unstacking error */
                    uint8_t UNSTKERR   : 1;
                    /* Stacking error */
                    uint8_t STKERR     : 1;
                    /* Floating-point lazy state preservation (M4/M7) */
                    uint8_t LSPERR     : 1;
                    uint8_t UnusedBits : 1;
                    /* Indicates BFAR is valid */
                    uint8_t BFARVALID  : 1;
                } bits;
            } bfsr;

            union {
                /* Usage Fault Status Register (0xE000ED2A) */
                uint16_t value;
                struct {
                    /* Attempts to execute an undefined instruction */
                    uint16_t UNDEFINSTR : 1;
                    /* Attempts to switch to an invalid state (e.g., ARM) */
                    uint16_t INVSTATE   : 1;
                    /* Attempts to do an exception with a bad value in the EXC_RETURN number */
                    uint16_t INVPC      : 1;
                    /* Attempts to execute a coprocessor instruction */
                    uint16_t NOCP       : 1;
                    uint16_t UnusedBits : 4;
                    /* Indicates that an unaligned access fault has taken place */
                    uint16_t UNALIGNED  : 1;
                    /* Indicates a divide by zero has taken place (can be set only if DIV_0_TRP is set) */
                    uint16_t DIVBYZERO0 : 1;
                } bits;
            } ufsr;
        } part;
    } cfsr;

    /* Memory Management Fault Address Register (0xE000ED34) */
    uint32_t mmar;

    /* Bus Fault Manage Address Register (0xE000ED38) */
    uint32_t bfar;

    union {
        /* Hard Fault Status Register (0xE000ED2C) */
        uint32_t value;
        struct {
            uint32_t UnusedBits  : 1;
            /* Indicates hard fault is caused by failed vector fetch */
            uint32_t VECTBL      : 1;
            uint32_t UnusedBits2 : 28;
            /* Indicates hard fault is taken because of bus fault/memory management fault/usage fault */
            uint32_t FORCED      : 1;
            /* Indicates hard fault is triggered by debug event */
            uint32_t DEBUGEVT    : 1;
        } bits;
    } hfsr;

    union {
        /* Debug Fault Status Register (0xE000ED30) */
        uint32_t value;
        struct {
            /* Halt requested in NVIC */
            uint32_t HALTED   : 1;
            /* BKPT instruction executed */
            uint32_t BKPT     : 1;
            /* DWT match occurred */
            uint32_t DWTTRAP  : 1;
            /* Vector fetch occurred */
            uint32_t VCATCH   : 1;
            /* EDBGRQ signal asserted */
            uint32_t EXTERNAL : 1;
        } bits;
    } dfsr;

    /* Auxiliary Fault Status Register (0xE000ED3C), Vendor controlled (optional) */
    uint32_t afsr;
} port_fault_regs_t;

enum fault_info {
    FAULT_INFO_ASSERT_ON_THREAD,
    FAULT_INFO_HFSR_VECTBL,
    FAULT_INFO_MFSR_IACCVIOL,
    FAULT_INFO_MFSR_DACCVIOL,
    FAULT_INFO_MFSR_MUNSTKERR,
    FAULT_INFO_MFSR_MSTKERR,
    FAULT_INFO_MFSR_MLSPERR,
    FAULT_INFO_BFSR_IBUSERR,
    FAULT_INFO_BFSR_PRECISERR,
    FAULT_INFO_BFSR_IMPREISERR,
    FAULT_INFO_BFSR_UNSTKERR,
    FAULT_INFO_BFSR_STKERR,
    FAULT_INFO_BFSR_LSPERR,
    FAULT_INFO_UFSR_UNDEFINSTR,
    FAULT_INFO_UFSR_INVSTATE,
    FAULT_INFO_UFSR_INVPC,
    FAULT_INFO_UFSR_NOCP,
    FAULT_INFO_UFSR_UNALIGNED,
    FAULT_INFO_UFSR_DIVBYZERO0,
    FAULT_INFO_DFSR_HALTED,
    FAULT_INFO_DFSR_BKPT,
    FAULT_INFO_DFSR_DWTTRAP,
    FAULT_INFO_DFSR_VCATCH,
    FAULT_INFO_DFSR_EXTERNAL,
    FAULT_INFO_MMAR,
    FAULT_INFO_BFAR,
};

static const char *const fault_msg[] = {
        [FAULT_INFO_ASSERT_ON_THREAD]      = "Assert on thread %s\n",
        [FAULT_INFO_HFSR_VECTBL]           = "Hard fault is caused by failed vector fetch\n",
        [FAULT_INFO_MFSR_IACCVIOL]         = "Memory management fault: instruction access violation\n",
        [FAULT_INFO_MFSR_DACCVIOL]         = "Memory management fault: data access violation\n",
        [FAULT_INFO_MFSR_MUNSTKERR]        = "Memory management fault: unstacking error\n",
        [FAULT_INFO_MFSR_MSTKERR]          = "Memory management fault: stacking error\n",
        [FAULT_INFO_MFSR_MLSPERR]          = "Memory management fault: floating-point lazy state preservation\n",
        [FAULT_INFO_BFSR_IBUSERR]          = "Bus fault: instruction access violation\n",
        [FAULT_INFO_BFSR_PRECISERR]        = "Bus fault: precise data access violation\n",
        [FAULT_INFO_BFSR_IMPREISERR]       = "Bus fault: imprecise data access violation\n",
        [FAULT_INFO_BFSR_UNSTKERR]         = "Bus fault: unstacking error\n",
        [FAULT_INFO_BFSR_STKERR]           = "Bus fault: stacking error\n",
        [FAULT_INFO_BFSR_LSPERR]           = "Bus fault: floating-point lazy state preservation\n",
        [FAULT_INFO_UFSR_UNDEFINSTR]       = "Usage fault: undefined instruction\n",
        [FAULT_INFO_UFSR_INVSTATE]         = "Usage fault: invalid state (e.g., ARM)\n",
        [FAULT_INFO_UFSR_INVPC]            = "Usage fault: invalid EXC_RETURN\n",
        [FAULT_INFO_UFSR_NOCP]             = "Usage fault: coprocessor instruction\n",
        [FAULT_INFO_UFSR_UNALIGNED]        = "Usage fault: unaligned access\n",
        [FAULT_INFO_UFSR_DIVBYZERO0]       = "Usage fault: divide by zero(can be set only if DIV_0_TRP is set)\n",
        [FAULT_INFO_DFSR_HALTED]           = "Debug fault: halt requested in NVIC\n",
        [FAULT_INFO_DFSR_BKPT]             = "Debug fault: BKPT instruction executed\n",
        [FAULT_INFO_DFSR_DWTTRAP]          = "Debug fault: DWT match occurred\n",
        [FAULT_INFO_DFSR_VCATCH]           = "Debug fault: Vector fetch occurred\n",
        [FAULT_INFO_DFSR_EXTERNAL]         = "Debug fault: EDBGRQ signal asserted\n",
        [FAULT_INFO_MMAR]                  = "The memory management fault occurred address is %08x\n",
        [FAULT_INFO_BFAR]                  = "The bus fault occurred address is %08x\n",
};

__PORT__ void       HardFault_Handler(void);

__PORT__ void       port_fault_diagnosis(void);
#endif

#if defined(TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT) && (TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT == 1u)
__PORT__ uint32_t   port_clz(uint32_t val);
#endif

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

#endif /* _PORT_H_ */

