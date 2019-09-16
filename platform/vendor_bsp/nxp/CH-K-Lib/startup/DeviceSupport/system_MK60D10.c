/*
** ###################################################################
**     Processors:          MK60DN512VLL10
**                          MK60DX256VLL10
**                          MK60DN256VLL10
**                          MK60DN512VLQ10
**                          MK60DN256VLQ10
**                          MK60DX256VLQ10
**                          MK60DN512VMC10
**                          MK60DN256VMC10
**                          MK60DX256VMC10
**                          MK60DN512VMD10
**                          MK60DX256VMD10
**                          MK60DN256VMD10
**
**     Compilers:           ARM Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    K60P144M100SF2V2RM Rev. 2, Jun 2012
**     Version:             rev. 1.3, 2012-10-29
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright: 2012 Freescale, Inc. All Rights Reserved.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2012-01-03)
**         Initial version
**     - rev. 1.1 (2012-04-13)
**         Added new #define symbol MCU_MEM_MAP_VERSION_MINOR.
**         Added new #define symbols <peripheralType>_BASE_PTRS.
**     - rev. 1.2 (2012-07-09)
**         UART0 - Fixed register definition - CEA709.1-B (LON) registers added.
**     - rev. 1.3 (2012-10-29)
**         Registers updated according to the new reference manual revision - Rev. 2, Jun 2012
**
** ###################################################################
*/

/**
 * @file MK60D10
 * @version 1.3
 * @date 2012-10-29
 * @brief Device specific configuration file for MK60D10 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "MK60D10.h"

#define DISABLE_WDOG    1

#ifndef CLOCK_SETUP
#define CLOCK_SETUP     4
#endif
/* Predefined clock setups
   0 ... Internal clock 32.768KHz  FLL output   Core clock = 41.94MHz   BusClock = 41.94MHz
   1 ... External clock 25Mhz      PLL output   Core clock = 200MHz     BusClock = 100MHz
   2 ... External clock 8Mhz    no PLL output   Core clock = 8MHz       BusClock = 8MHz
   3 ... External clock 50Mhz      PLL output   Core clock = 100MHz     BusClock = 50MHz  
   4 ... Internal clock 32.768KHz  FLL output   Core clock = 96MHz      BusClock = 48MHz  
   5 ... External clock 50Mhz      PLL output   Core clock = 200MHz     BusClock = 100MHz  
   6 ... External clock 12Mhz      PLL output   Core clock = 96MHz      BusClock = 48MHz  
*/

/*----------------------------------------------------------------------------
  Define clock source values
 *----------------------------------------------------------------------------*/
#if (CLOCK_SETUP == 0)
    #define CPU_XTAL_CLK_HZ                 50000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            41943040u /* Default System clock value */
#elif (CLOCK_SETUP == 1)
    #define CPU_XTAL_CLK_HZ                 25000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            200000000u /* Default System clock value */
#elif (CLOCK_SETUP == 2)
    #define CPU_XTAL_CLK_HZ                 8000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            8000000u /* Default System clock value */
#elif (CLOCK_SETUP == 3)
    #define CPU_XTAL_CLK_HZ                 50000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            100000000u /* Default System clock value */
#elif (CLOCK_SETUP == 4)
    #define CPU_XTAL_CLK_HZ                 50000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            96000000u /* Default System clock value */
#elif (CLOCK_SETUP == 5)
    #define CPU_XTAL_CLK_HZ                 50000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            200000000u /* Default System clock value */
#elif (CLOCK_SETUP == 6)
    #define CPU_XTAL_CLK_HZ                 12000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            96000000u /* Default System clock value */
#endif /* (CLOCK_SETUP == 6) */


/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemInit (void) {
#if (DISABLE_WDOG)
  /* Disable the WDOG module */
  /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG->UNLOCK = (uint16_t)0xC520u;     /* Key 1 */
  /* WDOG_UNLOCK : WDOGUNLOCK=0xD928 */
  WDOG->UNLOCK  = (uint16_t)0xD928u;    /* Key 2 */
  WDOG->STCTRLH = (uint16_t)0x01D2u;
#endif /* (DISABLE_WDOG) */
#if (CLOCK_SETUP == 0)
  SIM->CLKDIV1 = (uint32_t)0x00110000u; /* Update system prescalers */
  /* Switch to FEI Mode */
  /* MCG->C1: CLKS=0,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = (uint8_t)0x06u;
  MCG->C2 = (uint8_t)0x00u;
  /* MCG_C4: DMX32=0,DRST_DRS=1 */
  MCG->C4 = (uint8_t)((MCG->C4 & (uint8_t)~(uint8_t)0xC0u) | (uint8_t)0x20u);
  MCG->C5 = (uint8_t)0x00u;
  /* MCG->C6: LOLIE=0,PLLS=0,CME=0,VDIV0=0 */
  MCG->C6 = (uint8_t)0x00u;
  while((MCG->S & MCG_S_IREFST_MASK) == 0u) { /* Check that the source of the FLL reference clock is the internal reference clock. */
  }
  while((MCG->S & 0x0Cu) != 0x00u) {    /* Wait until output of the FLL is selected */
  }
#elif (CLOCK_SETUP == 1)
    SIM->CLKDIV1 = (uint32_t)0xFFFFFFFFu;
    OSC->CR = (uint8_t)0x00u;
    SIM->SOPT2 &= ~0x01u;                               /* select OSCCLK as MCG input clock */
    MCG->C2 = (uint8_t)0x24u;  
    MCG->C1 = (uint8_t)0x9Au;
    MCG->C4 &= (uint8_t)~(uint8_t)0xE0u;
    MCG->C5 = (uint8_t)0x03u;
    MCG->C6 = (uint8_t)0x00u;
    while((MCG->S & MCG_S_OSCINIT0_MASK) == 0u);        /* 检查 FLL参考时钟是内部参考时钟 */
    while((MCG->S & MCG_S_IREFST_MASK) != 0u);          /* 检查 FLL参考时钟是内部参考时钟 */
    while((MCG->S & 0x0Cu) != 0x08u);                   /* 等待 FBE 被选择 */
    MCG->C5 = (uint8_t)MCG_C5_PRDIV0(5);                /* 25/6 */
    MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV0(24));        /* (25/6)*(6*8) = 200 */
    SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(7));	
    while((MCG->S & MCG_S_PLLST_MASK) == 0u);           /* 等待PLLS 时钟源转到 PLL */
    while((MCG->S & MCG_S_LOCK0_MASK) == 0u);           /* 等待锁定 */
    /* 启动PLL */
    MCG->C1 = (uint8_t)0x1Au;
    while((MCG->S & 0x0Cu) != 0x0Cu);                   /* 等待PLL输出 */
    while((MCG->S & MCG_S_LOCK0_MASK) == 0u);           /* 等待PLL锁定 */
#elif (CLOCK_SETUP == 2)
  SIM->CLKDIV1 = (uint32_t)0x00110000u; /* Update system prescalers */
  /* Switch to FBE Mode */
  OSC->CR = (uint8_t)0x00u;
  /* MCG->C7: OSCSEL=0 */
  MCG->C7 = (uint8_t)0x00u;
  MCG->C2 = (uint8_t)0x24u;
  /* MCG->C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = (uint8_t)0x9Au;
  /* MCG->C4: DMX32=0,DRST_DRS=0 */
  MCG->C4 &= (uint8_t)~(uint8_t)0xE0u;
  MCG->C5 = (uint8_t)0x00u;
  /* MCG->C6: LOLIE=0,PLLS=0,CME=0,VDIV0=0 */
  MCG->C6 = (uint8_t)0x00u;
  while((MCG->S & MCG_S_OSCINIT0_MASK) == 0u) { /* Check that the oscillator is running */
  }
  while((MCG->S & MCG_S_IREFST_MASK) != 0u) { /* Check that the source of the FLL reference clock is the external reference clock. */
  }
  while((MCG->S & 0x0CU) != 0x08u) {    /* Wait until external reference clock is selected as MCG output */
  }
  /* Switch to BLPE Mode */
  MCG->C2 = (uint8_t)0x24u;
#elif (CLOCK_SETUP == 3)
    SIM->CLKDIV1 = (uint32_t)0xFFFFFFFFu;               /* 配置系统预分频器 先设置为都为最低分频 */
    OSC->CR = (uint8_t)0x00u;
    SIM->SOPT2 &= ~0x01u;                               /* select OSCCLK as MCG input clock */
    MCG->C2 = (uint8_t)0x24u;  
    MCG->C1 = (uint8_t)0x9Au;
    MCG->C4 &= (uint8_t)~(uint8_t)0xE0u;
    MCG->C5 = (uint8_t)0x03u;
    MCG->C6 = (uint8_t)0x00u;
    while((MCG->S & MCG_S_OSCINIT0_MASK) == 0u);        /* 检查 FLL参考时钟是内部参考时钟 */
    while((MCG->S & MCG_S_IREFST_MASK) != 0u);          /* 检查 FLL参考时钟是内部参考时钟 */
    while((MCG->S & 0x0Cu) != 0x08u);                   /* 等待 FBE 被选择 */
    MCG->C5 = (uint8_t)MCG_C5_PRDIV0(24);               /* 50/25 = 2M */
    MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV0(26));
    SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));	
    while((MCG->S & MCG_S_PLLST_MASK) == 0u);           /* 等待PLLS 时钟源转到 PLL */
    while((MCG->S & MCG_S_LOCK0_MASK) == 0u);           /* 等待锁定 */
    /* 启动PLL */
    MCG->C1 = (uint8_t)0x1Au;
    while((MCG->S & 0x0Cu) != 0x0Cu);                   /* 等待PLL输出 */
    while((MCG->S & MCG_S_LOCK0_MASK) == 0u);           /* 等待PLL锁定 */
#elif (CLOCK_SETUP == 4)
	SIM->CLKDIV1 = (uint32_t)0xFFFFFFFFu;
	// 转到 FEI 模式 
	MCG->C1 = (uint8_t)0x06u;
	MCG->C2 = (uint8_t)0x00u;
	MCG->C4|= (1<<6)|(1<<7)|(1<<5);   //内部参考慢速时钟32.768KHZ  倍频因子 2197 倍频后为96MHZ 参见MCG->C4寄存器
	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));
    MCG->C5 = (uint8_t)0x00u;
    MCG->C6 = (uint8_t)0x00u;
    while((MCG->S & MCG_S_IREFST_MASK) == 0u);  //检查 FLL参考时钟是内部参考时钟
    while((MCG->S & 0x0Cu) != 0x00u);           //等待FLL被选择
#elif (CLOCK_SETUP == 5)
    SIM->CLKDIV1 = (uint32_t)0xFFFFFFFFu;               /* 配置系统预分频器 先设置为都为最低分频 */
    OSC->CR = (uint8_t)0x00u;
    SIM->SOPT2 &= ~0x01u;                               /* select OSCCLK as MCG input clock */
    MCG->C2 = (uint8_t)0x24u;  
    MCG->C1 = (uint8_t)0x9Au;
    MCG->C4 &= (uint8_t)~(uint8_t)0xE0u;
    MCG->C5 = (uint8_t)0x03u;
    MCG->C6 = (uint8_t)0x00u;
    while((MCG->S & MCG_S_OSCINIT0_MASK) == 0u);        /* 检查 FLL参考时钟是内部参考时钟 */
    while((MCG->S & MCG_S_IREFST_MASK) != 0u);          /* 检查 FLL参考时钟是内部参考时钟 */
    while((MCG->S & 0x0Cu) != 0x08u);                   /* 等待 FBE 被选择 */
    MCG->C5 = (uint8_t)MCG_C5_PRDIV0(12);               /* 50/13 */
    MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV0(28));        /* 50/13*52 = 200 */
    SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(7));	
    while((MCG->S & MCG_S_PLLST_MASK) == 0u);           /* 等待PLLS 时钟源转到 PLL */
    while((MCG->S & MCG_S_LOCK0_MASK) == 0u);           /* 等待锁定 */
    /* 启动PLL */
    MCG->C1 = (uint8_t)0x1Au;
    while((MCG->S & 0x0Cu) != 0x0Cu);                   /* 等待PLL输出 */
    while((MCG->S & MCG_S_LOCK0_MASK) == 0u);           /* 等待PLL锁定 */
#elif (CLOCK_SETUP == 6)
    SIM->CLKDIV1 = (uint32_t)0xFFFFFFFFu;               /* 配置系统预分频器 先设置为都为最低分频 */
    OSC->CR = (uint8_t)0x00u;
    SIM->SOPT2 &= ~0x01u;                               /* select OSCCLK as MCG input clock */
    MCG->C2 = (uint8_t)0x24u;  
    MCG->C1 = (uint8_t)0x9Au;
    MCG->C4 &= (uint8_t)~(uint8_t)0xE0u;
    MCG->C5 = (uint8_t)0x03u;
    MCG->C6 = (uint8_t)0x00u;
    while((MCG->S & MCG_S_OSCINIT0_MASK) == 0u);        /* 检查 FLL参考时钟是内部参考时钟 */
    while((MCG->S & MCG_S_IREFST_MASK) != 0u);          /* 检查 FLL参考时钟是内部参考时钟 */
    while((MCG->S & 0x0Cu) != 0x08u);                   /* 等待 FBE 被选择 */
    MCG->C5 = (uint8_t)MCG_C5_PRDIV0(3);                /* 12/4  */
    MCG->C6 = (uint8_t)(0x40u|MCG_C6_VDIV0(8));         /* 12/4*32 = 96 */
    SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));	
    while((MCG->S & MCG_S_PLLST_MASK) == 0u);           /* 等待PLLS 时钟源转到 PLL */
    while((MCG->S & MCG_S_LOCK0_MASK) == 0u);           /* 等待锁定 */
    /* 启动PLL */
    MCG->C1 = (uint8_t)0x1Au;
    while((MCG->S & 0x0Cu) != 0x0Cu);                   /* 等待PLL输出 */
    while((MCG->S & MCG_S_LOCK0_MASK) == 0u);           /* 等待PLL锁定 */
#endif /* (CLOCK_SETUP == 6) */
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {
  uint32_t MCGOUTClock;                                                        /* Variable to store output clock frequency of the MCG module */
  uint8_t Divider;

  if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x0u) {
    /* Output of FLL or PLL is selected */
    if ((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u) {
      /* FLL is selected */
      if ((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u) {
        /* External reference clock is selected */
        if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
          MCGOUTClock = CPU_XTAL_CLK_HZ;                                       /* System oscillator drives MCG clock */
        } else { /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
          MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                    /* RTC 32 kHz oscillator drives MCG clock */
        } /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
        Divider = (uint8_t)(1u << ((MCG->C1 & MCG_C1_FRDIV_MASK) >> MCG_C1_FRDIV_SHIFT));
        MCGOUTClock = (MCGOUTClock / Divider);  /* Calculate the divided FLL reference clock */
        if ((MCG->C2 & MCG_C2_RANGE0_MASK) != 0x0u) {
          MCGOUTClock /= 32u;                                                  /* If high range is enabled, additional 32 divider is active */
        } /* ((MCG->C2 & MCG_C2_RANGE0_MASK) != 0x0u) */
      } else { /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */
        MCGOUTClock = CPU_INT_SLOW_CLK_HZ;                                     /* The slow internal reference clock is selected */
      } /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */
      /* Select correct multiplier to calculate the MCG output clock  */
      switch (MCG->C4 & (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) {
        case 0x0u:
          MCGOUTClock *= 640u;
          break;
        case 0x20u:
          MCGOUTClock *= 1280u;
          break;
        case 0x40u:
          MCGOUTClock *= 1920u;
          break;
        case 0x60u:
          MCGOUTClock *= 2560u;
          break;
        case 0x80u:
          MCGOUTClock *= 732u;
          break;
        case 0xA0u:
          MCGOUTClock *= 1464u;
          break;
        case 0xC0u:
          MCGOUTClock *= 2197u;
          break;
        case 0xE0u:
          MCGOUTClock *= 2929u;
          break;
        default:
          break;
      }
    } else { /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
      /* PLL is selected */
      Divider = (1u + (MCG->C5 & MCG_C5_PRDIV0_MASK));
      MCGOUTClock = (uint32_t)(CPU_XTAL_CLK_HZ / Divider);                     /* Calculate the PLL reference clock */
      Divider = ((MCG->C6 & MCG_C6_VDIV0_MASK) + 24u);
      MCGOUTClock *= Divider;                       /* Calculate the MCG output clock */
    } /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
  } else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x40u) {
    /* Internal reference clock is selected */
    if ((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u) {
      MCGOUTClock = CPU_INT_SLOW_CLK_HZ;                                       /* Slow internal reference clock selected */
    } else { /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
      MCGOUTClock = CPU_INT_FAST_CLK_HZ / (1 << ((MCG->SC & MCG_SC_FCRDIV_MASK) >> MCG_SC_FCRDIV_SHIFT));  /* Fast internal reference clock selected */
    } /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
  } else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u) {
    /* External reference clock is selected */
    if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
      MCGOUTClock = CPU_XTAL_CLK_HZ;                                           /* System oscillator drives MCG clock */
    } else { /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
      MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                        /* RTC 32 kHz oscillator drives MCG clock */
    } /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
  } else { /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
    /* Reserved value */
    return;
  } /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
  SystemCoreClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT)));
}
