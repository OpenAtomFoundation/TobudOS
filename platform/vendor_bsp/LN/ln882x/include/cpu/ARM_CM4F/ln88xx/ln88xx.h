#ifndef __LN88XX_H__
#define __LN88XX_H__

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */
typedef enum IRQn
{
    /* -------------------  Processor Exceptions Numbers  ----------------------------- */
    Reset_IRQn              = -15,              /* -15  Reset Vector, invoked on Power up and warm reset                       */
    NMI_IRQn                = -14,              /* -14  Non maskable Interrupt, cannot be stopped or preempted                           */
    HardFault_IRQn          = -13,              /* -13  Hard Fault, all classes of Fault                                       */
    MemoryManagement_IRQn   = -12,              /* -12  Memory Management, MPU mismatch, including Access Violation and No Match */
    BusFault_IRQn           = -11,              /* -11  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                                        related Fault                                                          */
    UsageFault_IRQn         = -10,              /* -10  Usage Fault, i.e. Undef Instruction, Illegal State Transition          */
    SVCall_IRQn             =  -5,              /* -5   System Service Call via SVC instruction                                */
    DebugMonitor_IRQn       =  -4,              /* -4   Debug Monitor                                                          */
    PendSV_IRQn             =  -2,              /* -2   Pendable request for system service                                    */
    SysTick_IRQn            =  -1,              /* -1   System Tick Timer                                                      */

    /* -------------------  Processor Interrupt Numbers  ------------------------------ */
    WDT_IRQn                = 0,
    EXTERNAL_IRQn           = 1,
    RTC_IRQn                = 2,
    SLEEP_IRQn              = 3,
    MAC_IRQn                = 4,
    DMA_IRQn                = 5,
    QSPI_IRQn               = 6,
    SDIO_FUN1_IRQn          = 7,
    SDIO_FUN2_IRQn          = 8,
    SDIO_FUN3_IRQn          = 9,
    SDIO_FUN4_IRQn          = 10,
    SDIO_FUN5_IRQn          = 11,
    SDIO_FUN6_IRQn          = 12,
    SDIO_FUN7_IRQn          = 13,
    SDIO_ASYNC_HOST_IRQn    = 14,
    SDIO_M2S_IRQn           = 15,
    CM4_INTR0_IRQn          = 16,
    CM4_INTR1_IRQn          = 17,
    CM4_INTR2_IRQn          = 18,
    CM4_INTR3_IRQn          = 19,
    CM4_INTR4_IRQn          = 20,
    CM4_INTR5_IRQn          = 21,
    ADC_IRQn                = 22,
    TIMER_IRQn              = 23,
    I2C0_IRQn               = 24,
    I2C1_IRQn               = 25,
    SPI0_IRQn               = 26,
    SPI2_IRQn               = 27,
    UART0_IRQn              = 28,
    UART1_IRQn              = 29,
    SPI1_IRQn               = 30,
    GPIO_IRQn               = 31,
    I2S_IRQn                = 32,
    PAOTD_IRQn              = 33,  
#if   defined (LN881x)
#elif defined (LN882x)
    PWM_IRQn                = 34,
    TRNG_IRQn               = 35,
    AES_IRQn                = 36,
#else 
  #error Not supported device type
#endif
} IRQn_Type;

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

#define __CM4_REV                 0x0201U    /*!< Core Revision r2p1 */
/* ToDo: define the correct core features for the <Device> */
#define __MPU_PRESENT             1U         /*!< Set to 1 if MPU is present */
#define __VTOR_PRESENT            1U         /*!< Set to 1 if VTOR is present */
#define __NVIC_PRIO_BITS          3U         /*!< Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U         /*!< Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1U         /*!< Set to 1 if FPU is present */
#define __FPU_DP                  0U         /*!< Set to 1 if FPU is double precision FPU (default is single precision FPU) */
#define __ICACHE_PRESENT          0U         /*!< Set to 1 if I-Cache is present */
#define __DCACHE_PRESENT          0U         /*!< Set to 1 if D-Cache is present */
#define __DTCM_PRESENT            0U         /*!< Set to 1 if DTCM is present */

#include "core_cm4.h"                        /* Processor and core peripherals */
#include "system_ln88xx.h"                   /* System Header */

#include <stdbool.h>
#include <stddef.h>

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* =========================================================================================================================== */
/* ================                            Device Specific Peripheral Section                             ================ */
/* =========================================================================================================================== */



/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* =========================================================================================================================== */
/* ================                          Device Specific Peripheral Address Map                           ================ */
/* =========================================================================================================================== */
#if   defined (LN881x)
  #include "base_addr_ln881x.h"
  #include "mem_map_ln881x.h"
#elif defined (LN882x)
  #include "base_addr_ln882x.h"
  #include "mem_map_ln882x.h"
#else 
  #error Not supported device type
#endif


#ifdef __cplusplus
}
#endif

#endif  /* __LN88XX_H__ */
