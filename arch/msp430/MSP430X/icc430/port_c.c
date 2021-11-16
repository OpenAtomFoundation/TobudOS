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
    WDTCTL = WDTPW | 0xff00; // write a wrong Watchdog timer password will cause a PUC(reset)
}

__PORT__ void port_systick_config(uint32_t cycle_per_tick)
{
}

__PORT__ void port_systick_priority_set(uint32_t prio)
{
}

/* MSP430 */
__PORT__ void port_setup_systick(void)
{
    const unsigned short aclk_frequency_hz = 32768;

    /* Ensure the timer is stopped. */
    TA0CTL = 0;

    /* Run the timer from the ACLK. */
    TA0CTL = TASSEL_1;

    /* Clear everything to start with. */
    TA0CTL |= TACLR;

    /* Set the compare match value according to the tick rate we want. */
    TA0CCR0 = aclk_frequency_hz / TOS_CFG_CPU_TICK_PER_SECOND;

    /* Enable the interrupts. */
    TA0CCTL0 = CCIE;

    /* Start up clean. */
    TA0CTL |= TACLR;

    /* Up mode. */
    TA0CTL |= MC_1;
}

/* The MSP430X port uses a callback function to configure its tick interrupt.
 */
#pragma vector=TIMER0_A0_VECTOR
__PORT__ __interrupt __raw void timer0_isr( void )
{
    extern void port_systick_isr( void );

    __bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF );
    port_systick_isr();
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

}

__PORT__ void port_stop_mode_enter(void)
{

}

__PORT__ void port_standby_mode_enter(void)
{

}

#endif

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
__PORT__ void port_fault_diagnosis(void)
{
    k_fault_log_writer("fault diagnosis is not supported in MSP430\n");
}


/*------------------ 430 IAR Compiler-------------------*/
#if defined (__ICC430__) // __IAR_SYSTEMS_ICC__

#endif

#endif /* TOS_CFG_FAULT_BACKTRACE_EN */

