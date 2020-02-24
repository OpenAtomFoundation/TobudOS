/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 * \file
 * Interface for default exception handlers.
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/*----------------------------------------------------------------------------
 *        Types
 *----------------------------------------------------------------------------*/

/* Function prototype for exception table items (interrupt handler). */
typedef void( *IntFunc )( void );

/* Define WEAK attribute */
#if defined   ( __CC_ARM   )
    #define WEAK __attribute__ ((weak))
#elif defined ( __ICCARM__ )
    #define WEAK __weak
#elif defined (  __GNUC__  )
    #define WEAK __attribute__ ((weak))
#endif

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern void IrqHandlerNotUsed(void);

extern WEAK void NMI_Handler( void );
extern WEAK void HardFault_Handler( void );
extern WEAK void MemManage_Handler( void );
extern WEAK void BusFault_Handler( void );
extern WEAK void UsageFault_Handler( void );
extern WEAK void SVC_Handler( void );
extern WEAK void DebugMon_Handler( void );
extern WEAK void PendSV_Handler( void );
extern WEAK void SysTick_Handler( void );

extern WEAK void SUPC_IrqHandler(void);
extern WEAK void RSTC_IrqHandler(void);
extern WEAK void RTC_IrqHandler(void);
extern WEAK void RTT_IrqHandler(void);
extern WEAK void WDT_IrqHandler(void);
extern WEAK void PMC_IrqHandler(void);
extern WEAK void EEFC_IrqHandler(void);
extern WEAK void UART0_IrqHandler(void);
extern WEAK void UART1_IrqHandler(void);
extern WEAK void SMC_IrqHandler(void);
extern WEAK void PIOA_IrqHandler(void);
extern WEAK void PIOB_IrqHandler(void);
extern WEAK void PIOC_IrqHandler(void);
extern WEAK void USART0_IrqHandler(void);
extern WEAK void USART1_IrqHandler(void);
extern WEAK void MCI_IrqHandler(void);
extern WEAK void TWI0_IrqHandler(void);
extern WEAK void TWI1_IrqHandler(void);
extern WEAK void SPI_IrqHandler(void);
extern WEAK void SSC_IrqHandler(void);
extern WEAK void TC0_IrqHandler(void);
extern WEAK void TC1_IrqHandler(void);
extern WEAK void TC2_IrqHandler(void);
extern WEAK void TC3_IrqHandler(void);
extern WEAK void TC4_IrqHandler(void);
extern WEAK void TC5_IrqHandler(void);
extern WEAK void ADC_IrqHandler(void);
extern WEAK void DAC_IrqHandler(void);
extern WEAK void PWM_IrqHandler(void);
extern WEAK void CRCCU_IrqHandler(void);
extern WEAK void ACC_IrqHandler(void);
extern WEAK void USBD_IrqHandler(void);

#endif /* #ifndef EXCEPTIONS_H */
