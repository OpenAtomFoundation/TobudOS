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

#include "tos.h"
#include "core_cm4.h"

__PORT__ void port_cpu_reset(void)
{
    NVIC_SystemReset();
}

__PORT__ void port_systick_config(uint32_t cycle_per_tick)
{
    (void)SysTick_Config(cycle_per_tick);
}

__PORT__ void port_systick_priority_set(uint32_t prio)
{
    NVIC_SetPriority(SysTick_IRQn, prio);
}

#if TOS_CFG_TICKLESS_EN > 0u

__PORT__ k_time_t port_systick_max_delay_millisecond(void)
{
    k_time_t max_millisecond;
    uint32_t max_cycle;

    max_cycle = SysTick_LOAD_RELOAD_Msk; // 24 bit
    max_millisecond = (k_time_t)((uint64_t)max_cycle * K_TIME_MILLISEC_PER_SEC / TOS_CFG_CPU_CLOCK); // CLOCK: cycle per second
    return max_millisecond;
}

__PORT__ void port_systick_resume(void)
{
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

__PORT__ void port_systick_suspend(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

__PORT__ void port_systick_reload(uint32_t cycle_per_tick)
{
    uint32_t max_cycle;

    max_cycle = SysTick_LOAD_RELOAD_Msk; // 24 bit

    if (max_cycle - SysTick->VAL > cycle_per_tick - 1u) {
        SysTick->LOAD = max_cycle;
    } else {
        SysTick->LOAD = (cycle_per_tick - 1u) + SysTick->VAL;
    }

    SysTick->VAL = 0;
}

__PORT__ void port_systick_pending_reset(void)
{
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
}

#endif

#if TOS_CFG_PWR_MGR_EN > 0u

__PORT__ void port_sleep_mode_enter(void)
{
#if 1
    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#else
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#endif
}

__PORT__ void port_stop_mode_enter(void)
{
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

__PORT__ void port_standby_mode_enter(void)
{
    HAL_PWR_EnterSTANDBYMode();
}

#endif

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
__STATIC__ void port_fault_do_diagnosis(port_fault_regs_t *regs)
{
    k_fault_log_writer("\n\n====================== Fault Diagnosis =====================\n");

    if (regs->hfsr.bits.VECTBL) {
        k_fault_log_writer(fault_msg[FAULT_INFO_HFSR_VECTBL]);
    }
    if (regs->hfsr.bits.FORCED) {
        /* Memory Management Fault */
        if (regs->cfsr.part.mfsr.value) {
            if (regs->cfsr.part.mfsr.bits.IACCVIOL) {
                k_fault_log_writer(fault_msg[FAULT_INFO_MFSR_IACCVIOL]);
            }
            if (regs->cfsr.part.mfsr.bits.DACCVIOL) {
                k_fault_log_writer(fault_msg[FAULT_INFO_MFSR_DACCVIOL]);
            }
            if (regs->cfsr.part.mfsr.bits.MUNSTKERR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_MFSR_MUNSTKERR]);
            }
            if (regs->cfsr.part.mfsr.bits.MSTKERR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_MFSR_MSTKERR]);
            }

            if (regs->cfsr.part.mfsr.bits.MLSPERR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_MFSR_MLSPERR]);
            }

            if (regs->cfsr.part.mfsr.bits.MMARVALID) {
                if (regs->cfsr.part.mfsr.bits.IACCVIOL || regs->cfsr.part.mfsr.bits.DACCVIOL) {
                    k_fault_log_writer(fault_msg[FAULT_INFO_MMAR], regs->mmar);
                }
            }
        }
        /* Bus Fault */
        if (regs->cfsr.part.bfsr.value) {
            if (regs->cfsr.part.bfsr.bits.IBUSERR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_BFSR_IBUSERR]);
            }
            if (regs->cfsr.part.bfsr.bits.PRECISERR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_BFSR_PRECISERR]);
            }
            if (regs->cfsr.part.bfsr.bits.IMPREISERR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_BFSR_IMPREISERR]);
            }
            if (regs->cfsr.part.bfsr.bits.UNSTKERR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_BFSR_UNSTKERR]);
            }
            if (regs->cfsr.part.bfsr.bits.STKERR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_BFSR_STKERR]);
            }

            if (regs->cfsr.part.bfsr.bits.LSPERR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_BFSR_LSPERR]);
            }

            if (regs->cfsr.part.bfsr.bits.BFARVALID) {
                if (regs->cfsr.part.bfsr.bits.PRECISERR) {
                    k_fault_log_writer(fault_msg[FAULT_INFO_BFAR], regs->bfar);
                }
            }

        }
        /* Usage Fault */
        if (regs->cfsr.part.ufsr.value) {
            if (regs->cfsr.part.ufsr.bits.UNDEFINSTR) {
                k_fault_log_writer(fault_msg[FAULT_INFO_UFSR_UNDEFINSTR]);
            }
            if (regs->cfsr.part.ufsr.bits.INVSTATE) {
                k_fault_log_writer(fault_msg[FAULT_INFO_UFSR_INVSTATE]);
            }
            if (regs->cfsr.part.ufsr.bits.INVPC) {
                k_fault_log_writer(fault_msg[FAULT_INFO_UFSR_INVPC]);
            }
            if (regs->cfsr.part.ufsr.bits.NOCP) {
                k_fault_log_writer(fault_msg[FAULT_INFO_UFSR_NOCP]);
            }
            if (regs->cfsr.part.ufsr.bits.UNALIGNED) {
                k_fault_log_writer(fault_msg[FAULT_INFO_UFSR_UNALIGNED]);
            }
            if (regs->cfsr.part.ufsr.bits.DIVBYZERO0) {
                k_fault_log_writer(fault_msg[FAULT_INFO_UFSR_DIVBYZERO0]);
            }
        }
    }
    /* Debug Fault */
    if (regs->hfsr.bits.DEBUGEVT) {
        if (regs->dfsr.value) {
            if (regs->dfsr.bits.HALTED) {
                k_fault_log_writer(fault_msg[FAULT_INFO_DFSR_HALTED]);
            }
            if (regs->dfsr.bits.BKPT) {
                k_fault_log_writer(fault_msg[FAULT_INFO_DFSR_BKPT]);
            }
            if (regs->dfsr.bits.DWTTRAP) {
                k_fault_log_writer(fault_msg[FAULT_INFO_DFSR_DWTTRAP]);
            }
            if (regs->dfsr.bits.VCATCH) {
                k_fault_log_writer(fault_msg[FAULT_INFO_DFSR_VCATCH]);
            }
            if (regs->dfsr.bits.EXTERNAL) {
                k_fault_log_writer(fault_msg[FAULT_INFO_DFSR_EXTERNAL]);
            }
        }
    }
}

__PORT__ void port_fault_diagnosis(void)
{
    port_fault_regs_t regs;

    regs.syshndctrl.value   = SCB->SHCSR;
    regs.cfsr.value         = SCB->CFSR;
    regs.mmar               = SCB->MMFAR;
    regs.bfar               = SCB->BFAR;
    regs.hfsr.value         = SCB->HFSR;
    regs.dfsr.value         = SCB->DFSR;
    regs.afsr               = SCB->AFSR;

    port_fault_do_diagnosis(&regs);
}

__PORT__ void __NAKED__ HardFault_Handler(void)
{
    __ASM__ __VOLATILE__ (
        "MOV     r0, lr\n\t"
        "TST     lr, #0x04\n\t"
        "ITE     EQ\n\t"
        "MRSEQ   r1, MSP\n\t"
        "MRSNE   r1, PSP\n\t"
        "BL      fault_backtrace\n\t"
    );
}

#endif /* TOS_CFG_FAULT_BACKTRACE_EN */

