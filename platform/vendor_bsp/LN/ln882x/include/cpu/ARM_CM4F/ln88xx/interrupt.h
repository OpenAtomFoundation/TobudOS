#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_


//------------------------------------------------------------------------------
//
//   Weak attribute allows to replace default handler with the user's one
//
#define WEAK __attribute__ ((weak))
//------------------------------------------------------------------------------

extern unsigned long __top_of_stack[];
//------------------------------------------------------------------------------
//
//   Vector table item. Can be pointer to function or plain address value
//
typedef void (*intfun_t)(void);
typedef struct
{
    unsigned long *tos;
    intfun_t      vectors[101];
}__vector_table_t;

//------------------------------------------------------------------------------
//
//   Startup handler
//
void Reset_Handler(void);

//------------------------------------------------------------------------------
//
//   Cortex-M internal exceptions
//
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);

//------------------------------------------------------------------------------
//
//   Controller specific peripheral interrupts
//
WEAK void WDT_IRQHandler();
WEAK void EXTERNAL_IRQHandler();
WEAK void RTC_IRQHandler();
WEAK void SLEEP_IRQHandler();
WEAK void MAC_IRQHandler();
WEAK void DMAC_IRQHandler();
WEAK void QSPI_IRQHandler();
WEAK void SDIOCFUN1_IRQHandler();
WEAK void SDIOCFUN2_IRQHandler();
WEAK void SDIOCFUN3_IRQHandler();
WEAK void SDIOCFUN4_IRQHandler();
WEAK void SDIOCFUN5_IRQHandler();
WEAK void SDIOCFUN6_IRQHandler();
WEAK void SDIOCFUN7_IRQHandler();
WEAK void SDIOC_ASYNC_HOST_IRQHandler();
WEAK void SDIOC_M2S_IRQHandler();
WEAK void CM4_INTR0_IRQHandler();
WEAK void CM4_INTR1_IRQHandler();
WEAK void CM4_INTR2_IRQHandler();
WEAK void CM4_INTR3_IRQHandler();
WEAK void CM4_INTR4_IRQHandler();
WEAK void CM4_INTR5_IRQHandler();
WEAK void ADCC_IRQHandler();
WEAK void TIMER_IRQHandler();
WEAK void I2C0_IRQHandler();
WEAK void I2C1_IRQHandler();
WEAK void SPIM_IRQHandler();
WEAK void SPIS_IRQHandler();
WEAK void UART0_IRQHandler();
WEAK void UART1_IRQHandler();
WEAK void SPIM2_IRQHandler();
WEAK void GPIO_IRQHandler();
WEAK void I2S_IRQHandler();
WEAK void PAOTD_IRQHandler();

#endif /* _INTERRUPT_H_ */
