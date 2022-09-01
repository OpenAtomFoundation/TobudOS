/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "hal_device_registers.h"

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  extern uint32_t __VECTOR_TABLE;
#endif

void SystemInit(void)
{

#if defined (__FPU_PRESENT ) && (__FPU_PRESENT == 1U)
  #if defined(__FPU_USED) && (__FPU_USED == 1u)
    SCB->CPACR |= (SCB_CPACR_CP10_MASK | SCB_CPACR_CP11_MASK); /* set CP10, CP11 Full Access */
  #endif
#endif /* __FPU_PRESENT */

#if defined (__ICACHE_PRESENT )&& (__ICACHE_PRESENT == 1U)
#ifndef ICACHE_DISABLED
  if (SCB->CLIDR & SCB_CLIDR_IC_Msk)
  {
    SCB_EnableICache();
  }
#endif /* DCACHE_DISABLED */

#endif /* __ICACHE_PRESENT */
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
#ifndef DCACHE_DISABLED
  if (SCB->CLIDR & SCB_CLIDR_IC_Msk)
  {
    SCB_EnableDCache();
  }
#endif /* DCACHE_DISABLED */
#endif /* __DCACHE_PRESENT */
}

/* EOF. */
