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

#include "tos_k.h"

__PORT__ void port_cpu_reset(void)
{

}

__PORT__ void port_systick_config(uint32_t cycle_per_tick)
{

}

__PORT__ void port_systick_priority_set(uint32_t prio)
{

}

#if TOS_CFG_TICKLESS_EN > 0u

__PORT__ k_time_t port_systick_max_delay_millisecond(void)
{

}

__PORT__ void port_systick_resume(void)
{

}

__PORT__ void port_systick_suspend(void)
{

}

__PORT__ void port_systick_reload(uint32_t cycle_per_tick)
{

}

__PORT__ void port_systick_pending_reset(void)
{
}

#endif

#if TOS_CFG_PWR_MGR_EN > 0u

__PORT__ void port_sleep_mode_enter(void)
{
#if 0
#if 1
    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#else
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#endif
#endif
}

__PORT__ void port_stop_mode_enter(void)
{
#if 0
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
#endif
}

__PORT__ void port_standby_mode_enter(void)
{
#if 0
    HAL_PWR_EnterSTANDBYMode();
#endif
}

#endif

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
__PORT__ void port_fault_diagnosis(void)
{
    // k_fault_log_writer("fault diagnosis does not supported in CORTEX M0\n");
}

#if 0
/*------------------ RealView Compiler -----------------*/
/* V5 */
#if defined(__CC_ARM)

__PORT__ __ASM__ void HardFault_Handler(void)
{
    IMPORT  fault_backtrace

    MOV     r0, lr
    TST     lr, #0x04
    ITE     EQ
    MRSEQ   r1, MSP
    MRSNE   r1, PSP
    BL      fault_backtrace
}

/*------------------ ARM Compiler V6 -------------------*/
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)

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

#endif /* ARMCC VERSION */
#endif

#endif /* TOS_CFG_FAULT_BACKTRACE_EN */

