#include "ln88xx.h"
#include "utils/debug/CmBacktrace/cm_backtrace.h"


void set_interrupt_priority(void)
{
    NVIC_SetPriorityGrouping(4);

    NVIC_SetPriority(SysTick_IRQn,   (1<<__NVIC_PRIO_BITS) - 1);
    NVIC_SetPriority(MAC_IRQn,       2);
    NVIC_SetPriority(PAOTD_IRQn,     2);
    NVIC_SetPriority(WDT_IRQn,       4);
    NVIC_SetPriority(EXTERNAL_IRQn,  4);
    NVIC_SetPriority(RTC_IRQn,       4);
    NVIC_SetPriority(SLEEP_IRQn,     4);
    NVIC_SetPriority(DMA_IRQn,       4);
    NVIC_SetPriority(QSPI_IRQn,      4);
    NVIC_SetPriority(SDIO_FUN1_IRQn, 4);
    NVIC_SetPriority(SDIO_FUN2_IRQn, 4);
    NVIC_SetPriority(SDIO_FUN3_IRQn, 4);
    NVIC_SetPriority(SDIO_FUN4_IRQn, 4);
    NVIC_SetPriority(SDIO_FUN5_IRQn, 4);
    NVIC_SetPriority(SDIO_FUN6_IRQn, 4);
    NVIC_SetPriority(SDIO_FUN7_IRQn, 4);
    NVIC_SetPriority(SDIO_ASYNC_HOST_IRQn, 4);
    NVIC_SetPriority(SDIO_M2S_IRQn,  4);
    NVIC_SetPriority(CM4_INTR0_IRQn, 4);
    NVIC_SetPriority(CM4_INTR1_IRQn, 4);
    NVIC_SetPriority(CM4_INTR2_IRQn, 4);
    NVIC_SetPriority(CM4_INTR3_IRQn, 4);
    NVIC_SetPriority(CM4_INTR4_IRQn, 4);
    NVIC_SetPriority(CM4_INTR5_IRQn, 4);
    NVIC_SetPriority(ADC_IRQn,       4);
    NVIC_SetPriority(TIMER_IRQn,     4);
    NVIC_SetPriority(I2C0_IRQn,      4);
    NVIC_SetPriority(I2C1_IRQn,      4);
    NVIC_SetPriority(SPI0_IRQn,      4);
    NVIC_SetPriority(SPI2_IRQn,      4);
    NVIC_SetPriority(UART0_IRQn,     4);
    NVIC_SetPriority(UART1_IRQn,     2);
    NVIC_SetPriority(SPI1_IRQn,      4);
#if BLE_SUPPORT==ENABLE    
    NVIC_SetPriority(GPIO_IRQn, 3);//BLE use the gpio for IRQ, need higher priority
#else 
    NVIC_SetPriority(GPIO_IRQn, 4);
#endif
    NVIC_SetPriority(I2S_IRQn, 4);
}


void switch_global_interrupt(bool enable)
{
    if(enable)
        __enable_irq();
    else
        __disable_irq();
}


#if defined (__ARMCC_VERSION)
    __asm void NMI_Handler (void)
    {   
        MOV    R0, LR
        MOV    R1, SP
        BL     __cpp(cm_backtrace_fault)
NMI_Fault_Loop
        B      NMI_Fault_Loop
    }
    
    __asm void HardFault_Handler (void)
    {   
        MOV    R0, LR
        MOV    R1, SP
        BL     __cpp(cm_backtrace_fault)
Hard_Fault_Loop
        B      Hard_Fault_Loop
    }
    
    __asm void MemManage_Handler (void)
    {   
        MOV    R0, LR
        MOV    R1, SP
        BL     __cpp(cm_backtrace_fault)
MM_Fault_Loop
        B      MM_Fault_Loop
    }
    
    __asm void BusFault_Handler (void)
    {   
        MOV    R0, LR
        MOV    R1, SP
        BL     __cpp(cm_backtrace_fault)
Bus_Fault_Loop
        B      Bus_Fault_Loop
    }
    
    __asm void UsageFault_Handler (void)
    {   
        MOV    R0, LR
        MOV    R1, SP
        BL     __cpp(cm_backtrace_fault)
Usage_Fault_Loop
        B      Usage_Fault_Loop
    }

    __asm void DebugMon_Handler (void)
    {   
        MOV    R0, LR
        MOV    R1, SP
        BL     __cpp(cm_backtrace_fault)
DBG_Fault_Loop
        B      DBG_Fault_Loop
    }
 
#elif defined ( __GNUC__ )
    #error "gcc compiler!!!"
#elif defined ( __ICCARM__ )
    #error "iar compiler!!!"
#else
    #error "Unsupported compiler!!!"
#endif












