
#include "ln88xx.h"

#if   defined ( __GNUC__ )
  #include "interrupt.h"
#endif

#include "proj_config.h"
#include "hal/hal_syscon.h"

/*----------------------------------------------------------------------------
  Externals
 *----------------------------------------------------------------------------*/
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
extern uint32_t __Vectors[];
#define vector_table	__Vectors
#endif

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency */


/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)
{
    SystemCoreClock = SYSTEM_CLOCK;
}
uint32_t get_SystemCoreClock(void)
{
    return SystemCoreClock;
}

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
void SystemInit (void)
{

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    __DMB();
    SCB->VTOR = (uint32_t) &vector_table;
    __DSB();
#endif

#if defined (__FPU_USED) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3U << 10U*2U) | /* enable CP10 Full Access */
                  (3U << 11U*2U) ); /* enable CP11 Full Access */
#endif

#if defined(SCB_SHCSR_MEMFAULTENA_Msk)
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
#endif
#if defined(SCB_SHCSR_BUSFAULTENA_Msk)
    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
#endif
#if defined(SCB_SHCSR_USGFAULTENA_Msk)
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
#endif
    
#if defined(SCB_CCR_DIV_0_TRP_Msk)
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif


}

/* 
* Configure the System clock source, 
* PLL Multiplier and Divider factors, 
* AHB/APBx prescalers and 32K settings.
* This function must be called in __main, because RW section must be initialized.
*/
void SetSysClock(void)
{
    SystemCoreClock = SYSTEM_CLOCK;

#if (USE_PLL == DISABLE)
    HAL_SYSCON_SelectSysClkSrc (SYSCLK_SRC_XTAL);
#else
    HAL_SYSCON_SelectSysClkSrc (SYSCLK_SRC_PLL);    
#endif

    HAL_SYSCON_SetHclkDivision(SOURCE_CLOCK / AHBUS_CLOCK);
    HAL_SYSCON_SetPclk0Division(AHBUS_CLOCK / APBUS0_CLOCK);
    HAL_SYSCON_SetPclk1Division(AHBUS_CLOCK / APBUS1_CLOCK);
    HAL_SYSCON_32KCaliEnable();

    //cpu reset request unmask
    //HAL_SYSCON_CPUResetReqMask(0);

    HAL_SYSCON_PmuCfg();    
}

