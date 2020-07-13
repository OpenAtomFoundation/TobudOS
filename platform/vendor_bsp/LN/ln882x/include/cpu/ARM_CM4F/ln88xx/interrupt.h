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
WEAK void WDT_IRQHandler(void);
WEAK void EXTERNAL_IRQHandler(void);
WEAK void RTC_IRQHandler(void);
WEAK void SLEEP_IRQHandler(void);
WEAK void MAC_IRQHandler(void);
WEAK void DMAC_IRQHandler(void);
WEAK void QSPI_IRQHandler(void);
WEAK void SDIOCFUN1_IRQHandler(void);
WEAK void SDIOCFUN2_IRQHandler(void);
WEAK void SDIOCFUN3_IRQHandler(void);
WEAK void SDIOCFUN4_IRQHandler(void);
WEAK void SDIOCFUN5_IRQHandler(void);
WEAK void SDIOCFUN6_IRQHandler(void);
WEAK void SDIOCFUN7_IRQHandler(void);
WEAK void SDIOC_ASYNC_HOST_IRQHandler(void);
WEAK void SDIOC_M2S_IRQHandler(void);
WEAK void CM4_INTR0_IRQHandler(void);
WEAK void CM4_INTR1_IRQHandler(void);
WEAK void CM4_INTR2_IRQHandler(void);
WEAK void CM4_INTR3_IRQHandler(void);
WEAK void CM4_INTR4_IRQHandler(void);
WEAK void CM4_INTR5_IRQHandler(void);
WEAK void ADCC_IRQHandler(void);
WEAK void TIMER_IRQHandler(void);
WEAK void I2C0_IRQHandler(void);
WEAK void I2C1_IRQHandler(void);
WEAK void SPIM_IRQHandler(void);
WEAK void SPIS_IRQHandler(void);
WEAK void UART0_IRQHandler(void);
WEAK void UART1_IRQHandler(void);
WEAK void SPIM2_IRQHandler(void);
WEAK void GPIO_IRQHandler(void);
WEAK void I2S_IRQHandler(void);
WEAK void PAOTD_IRQHandler(void);

#endif /* _INTERRUPT_H_ */
