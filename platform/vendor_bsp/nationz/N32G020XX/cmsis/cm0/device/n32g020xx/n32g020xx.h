/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32g020xx.h
 * @author Nations Solution Team
 * @version v1.0.1
 * @brief N32G020基本配置定义文件。
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup N32G020 N32G020基本配置
 * @{
 */
#ifndef __N32G020XX_H__
#define __N32G020XX_H__

#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C"
{
#endif


/**
  * @brief Configuration of the Cortex-M0 Processor and Core Peripherals 
  */
#define __NVIC_PRIO_BITS          2 /*!< n32g020 uses 2 Bits for the Priority Levels      */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used  */

   
   
/**
 * @brief n32n032 Interrupt Number Definition
 * 
 */

/*!< Interrupt Number Definition */
typedef enum
{
/******  Cortex-M0 Processor Exceptions Numbers ******************************************************/
    NMI_IrqNum                = -14,    /*!< 2 Non Maskable Interrupt                                */
    HardFault_IrqNum          = -13,    /*!< 3 Cortex-M0+ Hard Fault Interrupt                       */

    SVCall_IrqNum             = -5,     /*!< 11 Cortex-M0+ SV Call Interrupt                         */

    PendSV_IrqNum             = -2,     /*!< 14 Cortex-M0+ Pend SV Interrupt                         */
    SysTick_IrqNum            = -1,     /*!< 15 Cortex-M0+ System Tick Interrupt                     */
    SysTick_IRQn              = -1,     /*!< 15 Cortex-M0+ System Tick Interrupt                     */

/******  n32g020 specific Interrupt Numbers **********************************************************/
    Wdt_Exception_IrqNum      = 0,      
    Flash_Exception_IrqNum    = 3,
    Timer0_Exception_IrqNum   = 5,
    Timer1_Exception_IrqNum   = 6,
    Timer2_Exception_IrqNum   = 7,
    Timer3_Exception_IrqNum   = 8,
    Timer4_Exception_IrqNum   = 9,
    Pmu_Exception_IrqNum      = 10,
    Iom_Exception_IrqNum      = 11,
    Dmac_Exception_IrqNum     = 14,
    Usb_Exception_IrqNum      = 15,
    Ucdr_Exception_IrqNum     = 16,
    Scd_Exception_IrqNum      = 17,
    Scc_Exception_IrqNum      = 18,
    Spim0_Exception_IrqNum    = 19,
    Spim1_Exception_IrqNum    = 20,
    Spis0_Exception_IrqNum    = 21,
    Spis1_Exception_IrqNum    = 22,
    Uart0_Exception_IrqNum    = 23,
    Uart1_Exception_IrqNum    = 24,
    Uart2_Exception_IrqNum    = 25,
    Rtc_Exception_IrqNum      = 26,
    Adc_Dac_Exception_IrqNum  = 27,     /*!< ADC/DAC中断信号由AdcDacCtrl控制共享1个中断                */
    RcvCnt_Exception_IrqNum   = 28,     /*!< RcvCnt中断请求                               */
    I2c0_Exception_IrqNum     = 29,
    I2c1_Exception_IrqNum     = 30,
    Pwm_Exception_IrqNum      = 31,
} IRQn_Type;


#include "core_cm0.h"
// #include <stdint.h>

//------------------------------------------------------------------------------
//                        CHIP INFORMATION
//------------------------------------------------------------------------------
/* IP FEATURES */
#define DMAC_MAX_CHANNEL (8)

//------------------------------------------------------------------------------
//                       REGISTER BASE INFORMATION
//------------------------------------------------------------------------------
#define ACTLR                                                                  \
    (*((volatile unsigned long*)(0xE000E008))) // The Auxiliary Control
// Register,
// #define SCB_BASE (0xE000ED00) // System Control Block.0xE000ED00-0xE000ED8F
#define DMA_BASE (0x40000000)
#define ARAM_BASE (0x40200000)
#define FLASH_BASE (0x40400000)
#define CRC_BASE (0x40500000)
#define USB_BASE (0x40600000)
#define USBDC_BASE (USB_BASE + 0x27C)
#define SPIM0_BASE (0x40700000)
#define SPIM1_BASE (0x40700400)
#define ICACHE_BASE (0x40800000)
#define SPIS0_BASE (0x50000000)
#define SPIS1_BASE (0x50000400)
#define I2C0_BASE (0x50200000)
#define I2C1_BASE (0x50200400)
#define TIMER0_BASE (0x50300000)
#define TIMER1_BASE (0x50300020)
#define TIMER2_BASE (0x50300040)
#define TIMER3_BASE (0x50300060)
#define TIMER4_BASE (0x50300080)
#define SCC_BASE (0x50400000)
#define SCD_BASE (0x50500000)
#define ADC_BASE (0x50700000)
#define PWM_BASE (0x50800000)
#define PWM0_BASE (PWM_BASE)
#define PWM1_BASE (PWM_BASE + 0x50)
#define PWM2_BASE (PWM_BASE + 0xA0)
#define PWM3_BASE (PWM_BASE + 0xF0)
#define PWM4_BASE (PWM_BASE + 0x140)
#define PWM5_BASE (PWM_BASE + 0x190)
#define PWM6_BASE (PWM_BASE + 0x1E0)
#define PWM7_BASE (PWM_BASE + 0x230)
#define PWMGLB_BASE (PWM_BASE + 0x280)
#define DAC_BASE (0x50900000)
#define UART0_BASE (0x50C00000)
#define UART1_BASE (0x50C00400)
#define UART2_BASE (0x50C00800)
#define UART_BASE (0x50C00000)
#define CGU_BASE (0x60000000)
#define RCU_BASE (0x60100000)
#define SMC_BASE (0x60200000)
#define PMU_BASE (0x60300000)
#define IOM_BASE (0x60500000)
#define WDT_BASE (0x60600000)
#define RNG_BASE (0x60800000)
#define RETPMU_BASE (0x60800000)
#define UCDR_BASE (0x60A00000)
#define RCVCNT_BASE (0x60B00000)
#define RCVCNT_INT_BASE (0x60C00080)
#define AFEC_BASE (0x60D00000)
#define RTC_BASE (0x60E00000)
#define ADCDAC_BASE (0x50700000+0x20)
#define DACOUT_BASE (0x60C00000)


//------------------------------------------------------------------------------
//                       REGISTER INFORMATION
//------------------------------------------------------------------------------
// ARM
typedef struct
{
    volatile uint32_t CPUID; //(*((volatile unsigned long *)(SCB_BASE + //0x000)))//CPUID Base Register on page B3-264
    volatile uint32_t ICSR; //(*((volatile unsigned long *)(SCB_BASE + 0x004)))//Interrupt Control State Register, ICSR on page B3-265
    volatile uint32_t VTOR; //(*((volatile unsigned long *)(SCB_BASE + 0x008)))//Vector Table Offset Register, VTOR on page B3-267
    volatile uint32_t AIRCR; //(*((volatile unsigned long *)(SCB_BASE + 0x00C)))//Application Interrupt and Reset Control Register,AIRCR on page B3 - 268
    volatile uint32_t SCR; //(*((volatile unsigned long *)(SCB_BASE + 0x010)))//Optional System Control Register, SCR on page B3-269
    volatile uint32_t CCR; //(*((volatile unsigned long *)(SCB_BASE + 0x014)))//Configuration and Control Register, CCR on page B3 - 271
    volatile uint32_t SHPR2; //(*((volatile unsigned long *)(SCB_BASE + 0x01C)))//System Handler Priority Register 2, SHPR2 on page B3 - 272
    volatile uint32_t SHPR3; //(*((volatile unsigned long *)(SCB_BASE + 0x020)))//System Handler Priority Register 3, SHPR3 on page B3 - 273
    volatile uint32_t SHCSR; //(*((volatile unsigned long *)(SCB_BASE + 0x024)))//System Handler Control and State Register, SHCSR on page C1 - 329d
    volatile uint32_t DFSR; //(*((volatile unsigned long *)(SCB_BASE + 0x030)))//Debug Fault Status Register, DFSR on page C1-330d
} ARMTypeDef;

// PWM 8ch
typedef struct
{
    volatile uint32_t GEN_CR; //(PWM_BASE + 0x280) //8通道使能控制
    volatile uint32_t INTSTATUS; //(PWM_BASE + 0x284) //8通道中断状态
    volatile uint32_t INTSTATUSSET; //(PWM_BASE + 0x288) //
    volatile uint32_t INTSTATUSCLR; //(PWM_BASE + 0x28C)
    volatile uint32_t INTEN; //(PWM_BASE + 0x290) //8通道中断使能
    volatile uint32_t INTENSET; //(PWM_BASE + 0x294)
    volatile uint32_t INTENCLR; //(PWM_BASE + 0x298)
} PWMGLBTypeDef;

typedef struct
{
    volatile uint32_t CR;    //(PWM_BASE + 0x000)//PWMDMA模式控制寄存器
    volatile uint32_t DMACR; //(PWM_BASE + 0x004)//PWMDMA模式控制寄存器
    volatile uint32_t MRUSERSET0; //(PWM_BASE + 0x008)//USER模式bit0预匹配
    volatile uint32_t MRUSERSET1; //(PWM_BASE + 0x00C)//USER模式bit1预匹配
    volatile uint32_t MRINI0; //(PWM_BASE + 0x010)//固定输出配置0预匹配值
    volatile uint32_t MRINI1; //(PWM_BASE + 0x014)//固定输出配置1预匹配值
    volatile uint32_t MRINI2; //(PWM_BASE + 0x018)//固定输出配置2预匹配值
    volatile uint32_t MRIDLE; //(PWM_BASE + 0x01C)//IDLE输出预匹配值
    volatile uint32_t DR;     //(PWM_BASE + 0x020)//数据寄存器
    volatile uint32_t IER;    //(PWM_BASE + 0x024)//PWM中断使能寄存器
    volatile uint32_t IFR;    //(PWM_BASE + 0x028)//PWM中断标志寄存器
    volatile uint32_t ISR;    //(PWM_BASE + 0x02C)//PWM中断状态寄存器
    volatile uint32_t DSR;    //(PWM_BASE + 0x030)//PWM数据状态寄存器
    volatile uint32_t ENSTOPCR; //(PWM_BASE + 0x034)//PWM紧急停机控制寄存器
    volatile uint32_t PHASEDELAYDR; //(PWM_BASE + 0x038)//PWM相位延迟数据寄存器
    volatile uint32_t DEADZONETIMEDR; //(PWM_BASE + 0x03C)//PWM死区时间数据寄存器
} PWMTypeDef;

// UART
typedef struct
{
    volatile uint32_t RES;  //(UART_BASE + 0x000)// NONE
    volatile uint32_t ISR;  //(UART_BASE + 0x004)//UART中断状态寄存器
    volatile uint32_t IER;  //(UART_BASE + 0x008)//UART中断使能寄存器
    volatile uint32_t CR;   //(UART_BASE + 0x00C)//UART控制寄存器
    volatile uint32_t BRPL; //(UART_BASE + 0x010)//UART波特率低位寄存器
    volatile uint32_t BRPH; //(UART_BASE + 0x014)//UART波特率高位存器
    volatile uint32_t DR;   //(UART_BASE + 0x018)//UART数据寄存器
    volatile uint32_t CR1; //(UART_BASE + 0x01c)//UART流控模式控制寄存器
} UARTTypeDef;

// TIMER
typedef struct
{
    volatile uint32_t CSR; // T0状态控制寄存器
    volatile uint32_t CR;  //定时初值寄存器
    volatile uint32_t CVR; //当前计数值寄存器
} TIMERTypeDef;

// CRC
typedef struct
{
    volatile uint32_t CR;  //(CRC_BASE + 0x000)//CRC控制寄存器
    volatile uint32_t DR;  //(CRC_BASE + 0x004)//CRC待校验数据寄存器
    volatile uint32_t H8R; //(CRC_BASE + 0x008)//CRC高8位校验结果
    volatile uint32_t L8R; //(CRC_BASE + 0x00C)//CRC低8位校验结果
    volatile uint32_t LRC; //(CRC_BASE + 0x010)//LRC校验值寄存器
} CRCTypeDef;

// USB
typedef struct
{
    volatile uint32_t RESERVE1;//(*((volatile unsigned long *)(USB_BASE + 0x27C)))
    volatile uint32_t RESERVE2;//(*((volatile unsigned long *)(USB_BASE + 0x280)))
    volatile uint32_t RESERVE3;//(*((volatile unsigned long *)(USB_BASE + 0x284)))	
    volatile uint32_t RESERVE4;//(*((volatile unsigned long *)(USB_BASE + 0x288)))
    volatile uint32_t EPCSR; //(*((volatile unsigned long *)(USB_BASE + 0x28C)))
    volatile uint32_t EP0CSR; //(*((volatile unsigned long *)(USB_BASE + 0x290)))
    volatile uint32_t EP0BCR; //(*((volatile unsigned long *)(USB_BASE + 0x294)))
    volatile uint32_t EP1CSR; //(*((volatile unsigned long *)(USB_BASE + 0x298)))
    volatile uint32_t EP2CSR; //(*((volatile unsigned long *)(USB_BASE + 0x29C)))
    volatile uint32_t RESERVE5;//(*((volatile unsigned long *)(USB_BASE + 0x2A0)))
    volatile uint32_t RESERVE6;//(*((volatile unsigned long *)(USB_BASE + 0x2A4)))
    volatile uint32_t RESERVE7;//(*((volatile unsigned long *)(USB_BASE + 0x2A8)))
    volatile uint32_t EP5CSR; //(*((volatile unsigned long *)(USB_BASE + 0x2AC)))
    volatile uint32_t EP2BCR; //(*((volatile unsigned long *)(USB_BASE + 0x2B0)))
    volatile uint32_t EP3CSR; //(*((volatile unsigned long *)(USB_BASE + 0x2B4)))
    volatile uint32_t EP4CSR; //(*((volatile unsigned long *)(USB_BASE + 0x2B8)))
    volatile uint32_t EP4BCR; //(*((volatile unsigned long *)(USB_BASE + 0x2BC)))
    volatile uint32_t RESERVE8;//(*((volatile unsigned long *)(USB_BASE + 0x2C0)))
    volatile uint32_t SUDFIFO; //(*((volatile unsigned long *)(USB_BASE + 0x2C4)))
    volatile uint32_t EP0IFIFO; //(*((volatile unsigned long *)(USB_BASE + 0x2C8)))
    volatile uint32_t EP0OFIFO; //(*((volatile unsigned long *)(USB_BASE + 0x2CC)))
    volatile uint32_t EP1FIFO; //(*((volatile unsigned long *)(USB_BASE + 0x2D0)))
    volatile uint32_t EP2FIFO; //(*((volatile unsigned long *)(USB_BASE + 0x2D4)))
    volatile uint32_t EP3FIFO; //(*((volatile unsigned long *)(USB_BASE + 0x2D8)))
    volatile uint32_t EP4FIFO; //(*((volatile unsigned long *)(USB_BASE + 0x2DC)))
    volatile uint32_t RESERVE9;//(*((volatile unsigned long *)(USB_BASE + 0x2E0)))
    volatile uint32_t USBIE; // (*((volatile unsigned long *)(USB_BASE + 0x2E4)))
    volatile uint32_t USBIR; // (*((volatile unsigned long *)(USB_BASE + 0x2E8)))
    volatile uint32_t EPIE; //(*((volatile unsigned long *)(USB_BASE + 0x2EC)))
    volatile uint32_t EPIR; // (*((volatile unsigned long *)(USB_BASE + 0x2F0)))
    volatile uint32_t TKIE; //(*((volatile unsigned long *)(USB_BASE + 0x2F4)))
    volatile uint32_t TKIR; //(*((volatile unsigned long *)(USB_BASE + 0x2F8)))
    volatile uint32_t DEVCFG; //(*((volatile unsigned long *)(USB_BASE + 0x2FC)))
    volatile uint32_t RESERVE10; //(*((volatile unsigned long *)(USB_BASE + 0x300)))
    volatile uint32_t RESERVE11; //(*((volatile unsigned long *)(USB_BASE + 0x304)))
    volatile uint32_t RESERVE12; //(*((volatile unsigned long *)(USB_BASE + 0x308)))
    volatile uint32_t RESERVE13; //(*((volatile unsigned long *)(USB_BASE + 0x30C)))
    volatile uint32_t TIR; //(*((volatile unsigned long *)(USB_BASE + 0x310)))
    volatile uint32_t RESERVE14;//(*((volatile unsigned long *)(USB_BASE + 0x314)))
    volatile uint32_t RESERVE15;//(*((volatile unsigned long *)(USB_BASE + 0x318)))
    volatile uint32_t UFMSR; //(*((volatile unsigned long *)(USB_BASE + 0x31C)))
    volatile uint32_t RESERVE16[7];//(*((volatile unsigned long *)(USB_BASE + 0x320~0x338)))
    volatile uint32_t EP5FIFO; //(*((volatile unsigned long *)(USB_BASE + 0x33C)))
    volatile uint32_t DEVCFG2; //(*((volatile unsigned long *)(USB_BASE + 0x340)))
    volatile uint32_t EPCSR2; //(*((volatile unsigned long *)(USB_BASE + 0x344)))
    volatile uint32_t EP6CSR; //(*((volatile unsigned long *)(USB_BASE + 0x348)))
    volatile uint32_t EP7CSR; //(*((volatile unsigned long *)(USB_BASE + 0x34C)))
    volatile uint32_t EP7BCR; //(*((volatile unsigned long *)(USB_BASE + 0x350)))
    volatile uint32_t EPIE2; //(*((volatile unsigned long *)(USB_BASE + 0x354)))
    volatile uint32_t EPIR2; //(*((volatile unsigned long *)(USB_BASE + 0x358)))
    volatile uint32_t EP6FIFO; //(*((volatile unsigned long *)(USB_BASE + 0x35C)))
    volatile uint32_t EP7FIFO; //(*((volatile unsigned long *)(USB_BASE + 0x360)))
} USBTypeDef;

// I2C
typedef struct
{
    volatile uint32_t ADR; //(I2C0_BASE+0X00)//I2C0地址寄存器
    volatile uint32_t FDR; //(I2C0_BASE+0X04)//I2C0预分频寄存器
    volatile uint32_t CR;  //(I2C0_BASE+0X08)//I2C0控制寄存器
    volatile uint32_t SR;  //(I2C0_BASE+0X0C)//I2C0状态寄存器
    volatile uint32_t DR;  //(I2C0_BASE+0X10)//I2C0数据寄存器
} I2CTypeDef;

// ADC
typedef struct
{
    volatile uint32_t ISR;
    volatile uint32_t IFR;
    volatile uint32_t IER;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t DATCHSTS;
    volatile uint32_t DMACR;
    volatile uint32_t FIFO;
} ADCTypeDef;

// DAC
typedef struct
{
    volatile uint32_t CR;      // (DAC_BASE+0X00)//DAC控制寄存器
    volatile uint32_t FIFO;    //(DAC_BASE+0X04)//DACFIFO寄存器
    volatile uint32_t ONE;     //(DAC_BASE+0X08)//DAC输出1数据寄存器
    volatile uint32_t ZERO;    //(DAC_BASE+0X0C)//DAC输出0数据寄存器
    volatile uint32_t FIFOSTS; //(DAC_BASE+0X10)//DACFIFO状态寄存器
    volatile uint32_t FIFOINTEN; //(DAC_BASE+0X14)//DACFIFO中断使能寄存器
    volatile uint32_t REG;       //(DAC_BASE+0X18)//DAC输出数据寄存器
} DACTypeDef;

// MICDET
typedef struct
{
    volatile uint32_t CR1; //(DACOUT_BASE + 0x00)
    volatile uint32_t CR2; //(DACOUT_BASE + 0x04)
} DACOUTTypeDef;

// WDT
typedef struct
{
    volatile uint32_t TAP; //(WDT_BASE + 0x000)//WDT写保护寄存器
    volatile uint32_t CSR; //(WDT_BASE + 0x004)//WDT状态控制寄存器
} WDTTypeDef;

// SPIS
typedef struct
{
    volatile uint32_t CR;   //(SPIS0_BASE + 0x000) ///< base address
    volatile uint32_t SR;   //(SPIS0_BASE + 0x004) ///< base address
    volatile uint32_t DCR;  //(SPIS0_BASE + 0x008) ///< base address
    volatile uint32_t DSR;  //(SPIS0_BASE + 0x00C) ///< base address
    volatile uint32_t IER;  //(SPIS0_BASE + 0x010) ///< base address
    volatile uint32_t IFR;  //(SPIS0_BASE + 0x014) ///< base address
    volatile uint32_t UDR;  //(SPIS0_BASE + 0x018) ///< base address
    volatile uint32_t RES[25]; // reserved
    volatile uint32_t DATA; //(SPIS0_BASE + 0x080) ///< base address
} SPISTypeDef;

// SPIM
typedef struct
{
    volatile uint32_t CR;       //(SPIM0_BASE + 0x000) ///< base address
    volatile uint32_t BAUD1;    //(SPIM0_BASE + 0x004) ///< base address
    volatile uint32_t EN;       //(SPIM0_BASE + 0x008) ///< base address
    volatile uint32_t IER;      //(SPIM0_BASE + 0x00C) ///< base address
    volatile uint32_t BAUD2;    //(SPIM0_BASE + 0x010) ///< base address
    volatile uint32_t ISR;      //(SPIM0_BASE + 0x014) ///< base address
    volatile uint32_t DSR;      //(SPIM0_BASE + 0x018) ///< base address
    volatile uint32_t DR;       //(SPIM0_BASE + 0x01C) ///< base address
    volatile uint32_t RXCNT;    //(SPIM0_BASE + 0x020) ///< base address
    volatile uint32_t TXCNT;    //(SPIM0_BASE + 0x024) ///< base address
    volatile uint32_t DUMMYNUM; //(SPIM0_BASE + 0x028) ///< base address
} SPIMTypeDef;

// RTC---TBC
typedef struct
{
    volatile uint32_t ADJ;      //(RTC_BASE + 0x000) ///< base address
    volatile uint32_t START;    //(RTC_BASE + 0x004) ///< base address
    volatile uint32_t DRUPDATE; //(RTC_BASE + 0x008) ///< base address
    volatile uint32_t DR;       //(RTC_BASE + 0x00C) ///< base address
    volatile uint32_t INTSTS;   //(RTC_BASE + 0x010) ///< base address
    volatile uint32_t INTSTSEN; //(RTC_BASE + 0x014) ///< base address
} RTCTypeDef;

// RCVCNT
typedef struct
{
    volatile uint32_t CR;     //(RCVCNT_BASE + 0x000) ///< base address
    volatile uint32_t IS;    //(RCVCNT_BASE + 0x004) ///< base address
    volatile uint32_t CNT0HR; //(RCVCNT_BASE + 0x008) ///< base address
    volatile uint32_t CNT0LR; //(RCVCNT_BASE + 0x00C) ///< base address
    volatile uint32_t CNT1HR; //(RCVCNT_BASE + 0x010) ///< base address
    volatile uint32_t CNT1LR; //(RCVCNT_BASE + 0x014) ///< base address
    volatile uint32_t RCVR;   //(RCVCNT_BASE + 0x018) ///< base address
    volatile uint32_t CTHRL;   //(RCVCNT_BASE + 0x01C) ///< base address
    volatile uint32_t CTHRH;   //(RCVCNT_BASE + 0x020) ///< base address
} RCVCNTTypeDef;

// RCVCNT_INT
typedef struct
{
    volatile uint32_t INT_STATUS; //(RCVCNT_INT_BASE + 0x00) //RCVCNT_INT中断状态总寄存器
    volatile uint32_t INT_SET_STATUS; //(RCVCNT_INT_BASE + 0x04) //RCVCNT_INT中断状态置位总寄存器
    volatile uint32_t INT_CLR_STATUS; //(RCVCNT_INT_BASE + 0x08)  //RCVCNT_INT中断状态清零总寄存器
    volatile uint32_t INT_ENABLE; //(RCVCNT_INT + 0x0C)  //RCVCNT_INT中断输出总使能寄存器
    volatile uint32_t INT_SET_ENABLE; //(RCVCNT_INT + 0x10)  //RCVCNT_INT中断输出总使能置位寄存器
    volatile uint32_t INT_CLR_ENABLE; //(RCVCNT_INT + 0x14) //RCVCNT_INT中断输出总使能清零寄存器
} RCVCNTIntTypeDef;

// PMU Power Management Unit
typedef struct
{
    volatile uint32_t RES[3];          //(PMU_BASE + 0x00)
    volatile uint32_t WKSREN0;        //(PMU_BASE + 0x0C)
    volatile uint32_t WKSREN1;        //(PMU_BASE + 0x10)
    volatile uint32_t WKEXTINEN;      //(PMU_BASE + 0x14)
    volatile uint32_t WKEXTINEN_SET;  //(PMU_BASE + 0x18)
    volatile uint32_t WKEXTINEN_CLR;  //(PMU_BASE + 0x1C)
    volatile uint32_t WKEXTINSTR;     //(PMU_BASE + 0x20)
    volatile uint32_t WKEXTINSTR_SET; //(PMU_BASE + 0x24)
    volatile uint32_t WKEXTINSTR_CLR; //(PMU_BASE + 0x28)
    volatile uint32_t WKSTR0;         //(PMU_BASE + 0x2C)
    volatile uint32_t WKSTR1;         //(PMU_BASE + 0x30)
    volatile uint32_t WKSTR2;         //(PMU_BASE + 0x34)
    volatile uint32_t WKSTR3;         //(PMU_BASE + 0x38)
    volatile uint32_t WKSTR4;         //(PMU_BASE + 0x3C)
} PMUTypeDef;

// Retention pum
typedef struct
{
    volatile uint32_t CTRL;        //(RETPMU_BASE + 0x00)
    volatile uint32_t XO32K_CTRL1; //(RETPMU_BASE + 0x04)
    volatile uint32_t XO32K_CTRL2; //(RETPMU_BASE + 0x08)
    volatile uint32_t RC32K_CTRL1; //(RETPMU_BASE + 0x0C)
    volatile uint32_t RC32K_CTRL2; //(RETPMU_BASE + 0x10)
    volatile uint32_t RES2;        //(RETPMU_BASE + 0x14)
    volatile uint32_t RTCWUP_CTRL; //(RETPMU_BASE + 0x18)
    volatile uint32_t ON_STAT;     //(RETPMU_BASE + 0x1C)
} RETPMUTypeDef;

// SMC System Management Control
typedef struct
{
    volatile uint32_t RC32KCFG0; //(SMC_BASE + 0x00)
    volatile uint32_t RC32KCFG1; //(SMC_BASE + 0x04)
    volatile uint32_t XO32KCFG0; //(SMC_BASE + 0x08)
    volatile uint32_t XO32KCFG1; //(SMC_BASE + 0x0C)
    volatile uint32_t RETLDOCFG; //(SMC_BASE + 0x10)
    volatile uint32_t SRAMCFG0;  //(SMC_BASE + 0x14)
    volatile uint32_t SRAMCFG1;  //(SMC_BASE + 0x18)
    volatile uint32_t SRAMCFG2;  //(SMC_BASE + 0x1C)
    volatile uint32_t SRAMCFG3;  //(SMC_BASE + 0x20)
    volatile uint32_t SRAMCFG4;  //(SMC_BASE + 0x24)
    volatile uint32_t SRAMCFG5;  //(SMC_BASE + 0x28)
    volatile uint32_t SRAMCFG6;  //(SMC_BASE + 0x2C)
    volatile uint32_t SRAMCFG7;  //(SMC_BASE + 0x30)
    volatile uint32_t SRAMCFG8;  //(SMC_BASE + 0x34)
    volatile uint32_t SRAMCFG9;  //(SMC_BASE + 0x38)
    volatile uint32_t SRAMCFG10; //(SMC_BASE + 0x3C)
    volatile uint32_t INITSTS;   //(SMC_BASE + 0x40)
} SMCTypeDef;

// CGU  Clock Generation Unit
typedef struct
{
    volatile uint32_t RES[3];    //(CGU_BASE + 0x00)
    volatile uint32_t CLK80MDIV1;   //(CGU_BASE + 0x0C)
    volatile uint32_t CLK80MDIV2;   //(CGU_BASE + 0x10)
    volatile uint32_t USBCLKCR;     //(CGU_BASE + 0x14)
    volatile uint32_t MCGR1;        //(CGU_BASE + 0x18)
    volatile uint32_t MCGR2;        //(CGU_BASE + 0x1C)
    volatile uint32_t MCGR3;        //(CGU_BASE + 0x20)
    volatile uint32_t MCGR4;        //(CGU_BASE + 0x24)
    volatile uint32_t MCGR5;        //(CGU_BASE + 0x28)
    volatile uint32_t MCGR6;        //(CGU_BASE + 0x2C)
    volatile uint32_t ALGCGR;       //(CGU_BASE + 0x30)
    volatile uint32_t RES0;
    volatile uint32_t WDTCLKCR;   //(CGU_BASE + 0x38)
    volatile uint32_t UART0CLKCR; //(CGU_BASE + 0x3C)
    volatile uint32_t UART1CLKCR; //(CGU_BASE + 0x40)
    volatile uint32_t UART2CLKCR; //(CGU_BASE + 0x44)
    volatile uint32_t TMRCLKSEL;  //(CGU_BASE + 0x48)
    volatile uint32_t TMR0CLKCR;  //(CGU_BASE + 0x4C)
    volatile uint32_t TMR1CLKCR;  //(CGU_BASE + 0x50)
    volatile uint32_t TMR2CLKCR;  //(CGU_BASE + 0x54)
    volatile uint32_t TMR3CLKCR;  //(CGU_BASE + 0x58)
    volatile uint32_t TMR4CLKCR;  //(CGU_BASE + 0x5C)
    volatile uint32_t RES1;
    volatile uint32_t DACCLKFDR0;  //(CGU_BASE + 0x64)
    volatile uint32_t DACCLKFDR1;  //(CGU_BASE + 0x68)
    volatile uint32_t DACCLKCR;    //(CGU_BASE + 0x6C)
    volatile uint32_t ADCCLKFDR0;  //(CGU_BASE + 0x70)
    volatile uint32_t ADCCLKFDR1;  //(CGU_BASE + 0x74)
    volatile uint32_t ADCCLKCR;    //(CGU_BASE + 0x78)
    volatile uint32_t SCCCLKCR;    //(CGU_BASE + 0x7c)
    volatile uint32_t RES2[2];
    volatile uint32_t OSCRNGCR;    //(CGU_BASE + 0x88)
    volatile uint32_t RES3[18];
    volatile uint32_t DIVSR0;   //(CGU_BASE + 0xD4)
    volatile uint32_t DIVSR1;   //(CGU_BASE + 0xD8)
    volatile uint32_t RES4[4];
    volatile uint32_t OSC80MOUTCR; //(CGU_BASE + 0xEC)
    volatile uint32_t CLKSRCFREQCR;  //(CGU_BASE + 0xF0)
    volatile uint32_t RES5[2];
    volatile uint32_t PWMRCVCLKCR; //(CGU_BASE + 0xFC)
} CGUTypeDef;

// RCU Reset Control Unit
typedef struct
{
    volatile uint32_t CR0;    //(RCU_BASE + 0x00)
    volatile uint32_t CR1;    //(RCU_BASE + 0x04)
    volatile uint32_t CR1_SET; //(RCU_BASE + 0x08)
    volatile uint32_t CR1_CLR; //(RCU_BASE + 0x0C)
    volatile uint32_t CR2;    //(RCU_BASE + 0x10)
    volatile uint32_t CR2_SET; //(RCU_BASE + 0x14)
    volatile uint32_t CR2_CLR; //(RCU_BASE + 0x18)
    volatile uint32_t CR3;    //(RCU_BASE + 0x1C)
    volatile uint32_t CR3_SET; //(RCU_BASE + 0x20)
    volatile uint32_t CR3_CLR; //(RCU_BASE + 0x24)
    volatile uint32_t SR0;    //(RCU_BASE + 0x28)
    volatile uint32_t SR1;    //(RCU_BASE + 0x2C)
} RCUTypeDef;

// IOM IO Multiplex
typedef struct
{
    volatile uint32_t INTPEGA;   //(IOM_BASE + 0x00)
    volatile uint32_t INTNEGA;   //(IOM_BASE + 0x04)
    volatile uint32_t INTPEGB;   //(IOM_BASE + 0x08)
    volatile uint32_t INTNEGB;   //(IOM_BASE + 0x0c)
    volatile uint32_t INTPEGC;   //(IOM_BASE + 0x10)
    volatile uint32_t INTNEGC;   //(IOM_BASE + 0x14)
    volatile uint32_t INTPEGD;   //(IOM_BASE + 0x18)
    volatile uint32_t INTNEGD;   //(IOM_BASE + 0x1c)
    volatile uint32_t INTSTATA;  //(IOM_BASE + 0x20)
    volatile uint32_t INTSTATB;  //(IOM_BASE + 0x24)
    volatile uint32_t INTSTATC;  //(IOM_BASE + 0x28)
    volatile uint32_t INTSTATD;  //(IOM_BASE + 0x2c)
    volatile uint32_t INTSTATE;  //(IOM_BASE + 0x30)
    volatile uint32_t WUENA;     //(IOM_BASE + 0x34)
    volatile uint32_t WUENB;     //(IOM_BASE + 0x38)
    volatile uint32_t WUENC;     //(IOM_BASE + 0x3C)
    volatile uint32_t WUEND;     //(IOM_BASE + 0x40)
    volatile uint32_t WULEVELA;  //(IOM_BASE + 0x44)
    volatile uint32_t WULEVELB;  //(IOM_BASE + 0x48)
    volatile uint32_t WULEVELC;  //(IOM_BASE + 0x4C)
    volatile uint32_t CR1;       //(IOM_BASE + 0x50)
    volatile uint32_t CR2;       //(IOM_BASE + 0x54)
    volatile uint32_t CR3;       //(IOM_BASE + 0x58)
    volatile uint32_t CR4;       //(IOM_BASE + 0x5c)
    volatile uint32_t CR5;       //(IOM_BASE + 0x60)
    volatile uint32_t CR6;       //(IOM_BASE + 0x64)
    volatile uint32_t CR7;       //(IOM_BASE + 0x68)
    volatile uint32_t CR8;       //(IOM_BASE + 0x6c)
    volatile uint32_t CR9;       //(IOM_BASE + 0x70)
    volatile uint32_t CR10;      //(IOM_BASE + 0x74)
    volatile uint32_t CR11;      //(IOM_BASE + 0x78)
    volatile uint32_t CR12;      //(IOM_BASE + 0x7c)
    volatile uint32_t CR13;      //(IOM_BASE + 0x80)
    volatile uint32_t CR14;      //(IOM_BASE + 0x84)
    volatile uint32_t CR15;      //(IOM_BASE + 0x88)
    volatile uint32_t RES1;      //(IOM_BASE + 0x8C)
    volatile uint32_t GPIODRA;   //(IOM_BASE + 0x90)
    volatile uint32_t GPIODRB;   //(IOM_BASE + 0x94)
    volatile uint32_t GPIODRC;   //(IOM_BASE + 0x98)
    volatile uint32_t GPIODRD;   //(IOM_BASE + 0x9C)
    volatile uint32_t GPIOOEA;   //(IOM_BASE + 0xA0)
    volatile uint32_t GPIOOEB;   //(IOM_BASE + 0xA4)
    volatile uint32_t GPIOOEC;   //(IOM_BASE + 0xa8)
    volatile uint32_t GPIOOED;   //(IOM_BASE + 0xac)
    volatile uint32_t GPIOIEA;   //(IOM_BASE + 0xB0)
    volatile uint32_t GPIOIEB;   //(IOM_BASE + 0xB4)
    volatile uint32_t GPIOIEC;   //(IOM_BASE + 0xB8)
    volatile uint32_t GPIOIED;   //(IOM_BASE + 0xBC)
    volatile uint32_t GPIOPUA;   //(IOM_BASE + 0xC0)
    volatile uint32_t GPIOPUB;   //(IOM_BASE + 0xC4)
    volatile uint32_t GPIOPUC;   //(IOM_BASE + 0xC8)
    volatile uint32_t GPIOPUD;   //(IOM_BASE + 0xCC)
    volatile uint32_t GPIOPDA;   //(IOM_BASE + 0xD0)
    volatile uint32_t GPIOPDB;   //(IOM_BASE + 0xD4)
    volatile uint32_t GPIOPDC;   //(IOM_BASE + 0xD8)
    volatile uint32_t GPIOPDD;   //(IOM_BASE + 0xDC)
    volatile uint32_t GPIOSMTA;  //(IOM_BASE + 0xE0)
    volatile uint32_t GPIOSMTB;  //(IOM_BASE + 0xE4)
    volatile uint32_t GPIOSMTC;  //(IOM_BASE + 0xE8)
    volatile uint32_t GPIOSMTD;  //(IOM_BASE + 0xEC)
    volatile uint32_t GPIOPINA1; //(IOM_BASE + 0xF0)
    volatile uint32_t GPIOPINA2; //(IOM_BASE + 0xF4)
    volatile uint32_t GPIOPINB1; //(IOM_BASE + 0xF8)
    volatile uint32_t GPIOPINB2; //(IOM_BASE + 0xFC)
    volatile uint32_t GPIOPINC1; //(IOM_BASE + 0x100)
    volatile uint32_t GPIOPINC2; //(IOM_BASE + 0x104)
    volatile uint32_t GPIOPIND1; //(IOM_BASE + 0x108)
    volatile uint32_t GPIOPIND2; //(IOM_BASE + 0x10C)
    volatile uint32_t RES2;      //(IOM_BASE + 0x110)
    volatile uint32_t PWMEMSTP;  //(IOM_BASE + 0x114)
} IOMTypeDef;

// AFEC Analog Front End Control
typedef struct
{
    volatile uint32_t RES1;      //(AFEC_BASE + 0x00)
    volatile uint32_t LDOCR2;    //(AFEC_BASE + 0x04)
    volatile uint32_t OSC80MCR1; //(AFEC_BASE + 0x08)
    volatile uint32_t COMPCR1;   //(AFEC_BASE + 0x0C)
    volatile uint32_t REV1CR;    //(AFEC_BASE + 0x10)
    volatile uint32_t REV2CR;    //(AFEC_BASE + 0x14)
    volatile uint32_t XO12MCR1;  //(AFEC_BASE + 0x18)
    volatile uint32_t OSC300MCR; //(AFEC_BASE + 0x1C)
    volatile uint32_t USBCR1;    //(AFEC_BASE + 0x20)
    volatile uint32_t USBCR2;    //(AFEC_BASE + 0x24)
    volatile uint32_t USBCR3;    //(AFEC_BASE + 0x28)
    volatile uint32_t USBREVCR;  //(AFEC_BASE + 0x2C)
    volatile uint32_t RES0[0x0B];
    volatile uint32_t CR; //(AFEC_BASE + 0x5C)
} AFECTypeDef;

// SCD
typedef struct
{
    volatile uint32_t ISR1; //(SCD_BASE + 0x000) ///< interrupt status 1
    volatile uint32_t IER1; //(SCD_BASE + 0x004) ///< interrupt enable 1
    volatile uint32_t ISR2; //(SCD_BASE + 0x008) ///< base address
    volatile uint32_t IER2; //(SCD_BASE + 0x00C) ///< base address
    volatile uint32_t CR1;  //(SCD_BASE + 0x010) ///< control1
    volatile uint32_t CR2;  //(SCD_BASE + 0x014) ///< base address
    volatile uint32_t CRCINITSET; //(SCD_BASE + 0x018) ///< crc ininial set
    volatile uint32_t TDR;        //(SCD_BASE + 0x01C) ///< transfer data
    volatile uint32_t RDR;        //(SCD_BASE + 0x020) ///< receive data
    volatile uint32_t BPRL;       //(SCD_BASE + 0x024) ///< baudrate low
    volatile uint32_t BPRH;       //(SCD_BASE + 0x028) ///< base address
    volatile uint32_t CRCDATA;    //    (SCD_BASE + 0x02C)//
    volatile uint32_t CRCL;       //(SCD_BASE + 0x030)// crc value low
    volatile uint32_t CRCH;       //(SCD_BASE + 0x034)//
    volatile uint32_t EECRL;      //(SCD_BASE + 0x038)// extetu low
    volatile uint32_t EECRH;      //(SCD_BASE + 0x03C)//
    volatile uint32_t GTRH;       //(SCD_BASE + 0x040)// guard time
    volatile uint32_t ETUEN;      //(SCD_BASE + 0x044)// etu en
} SCDTypeDef;

// SCC
typedef struct
{
    volatile uint32_t SR;         //(SCC_BASE + 0x000) ///< base address
    volatile uint32_t IER;        //(SCC_BASE + 0x004) ///< base address
    volatile uint32_t TCR;        //(SCC_BASE + 0x008) ///< base address
    volatile uint32_t UCR;        //(SCC_BASE + 0x00C) ///< base address
    volatile uint32_t DR;         //(SCC_BASE + 0x010) ///< base address
    volatile uint32_t BPRL;       //(SCC_BASE + 0x014) ///< base address
    volatile uint32_t BPRH;       //(SCC_BASE + 0x018) ///< base address
    volatile uint32_t UCR1;       //(SCC_BASE + 0x01C) ///< base address
    volatile uint32_t SR1;        //(SCC_BASE + 0x020) ///< base address
    volatile uint32_t IER1;       //(SCC_BASE + 0x024) ///< base address
    volatile uint32_t ECNTL;      //(SCC_BASE + 0x028) ///< base address
    volatile uint32_t ECNTH;      //(SCC_BASE + 0x02C)//
    volatile uint32_t GTR;        //(SCC_BASE + 0x030)//
    volatile uint32_t RETXCNT;    //(SCC_BASE + 0x034)//
    volatile uint32_t RETXCNTRMI; //(SCC_BASE + 0x038)//
} SCCTypeDef;

// DAMC
typedef struct
{
    struct
    {
        volatile uint32_t SRCADDR; //(DMA_BASE + (ch)*0x10 + 0x00)< source address
        volatile uint32_t DSTADDR; //(DMA_BASE + (ch)*0x10 + 0x04)< destination address
        volatile uint32_t CHCTRL; //(DMA_BASE + (ch)*0x10 + 0x08)< channel control
        volatile uint32_t CHSTS; //(DMA_BASE + (ch)*0x10 + 0x0C)< channel status
    } DMACHL[DMAC_MAX_CHANNEL];

    volatile uint32_t EN;              //(DMA_BASE + 0x80)
    volatile uint32_t SOFTRESET;       //(DMA_BASE + 0x84)
    volatile uint32_t INTSTATUS;       //(DMA_BASE + 0x88)
    volatile uint32_t INTMASK;         //(DMA_BASE + 0x8C)
    volatile uint32_t BASICINFM;       //(DMA_BASE + 0x90)
    volatile uint32_t PERREQ;          //(DMA_BASE + 0x94)
    volatile uint32_t RES1;            //(DMA_BASE + 0x98)
    volatile uint32_t RES2;            //(DMA_BASE + 0x9C)
    volatile uint32_t INTRAMCLRSTATUS; //(DMA_BASE + 0xa0)
    volatile uint32_t INTRAMCLRMASK;   //(DMA_BASE + 0xa4)
    volatile uint32_t RAMCLROPT;       //(DMA_BASE + 0xa8)
} DMACTypeDef;

// FLASH
typedef struct
{
    volatile uint32_t CR;  //(FLASH_BASE + 0x000)  // 控制寄存器
    volatile uint32_t OPR; //(FLASH_BASE + 0x004)  // 操作码寄存器
    volatile uint32_t IER; //(FLASH_BASE + 0x008)  // 中断使能寄存器
    volatile uint32_t SR;  //(FLASH_BASE + 0x00C)  // 中断状态寄存器
    volatile uint32_t ADR; //(FLASH_BASE + 0x010)  // 校验地址寄存器
    volatile uint32_t TR;  //(FLASH_BASE + 0x014)  // 时间控制寄存器
    volatile uint32_t SECR; //(FLASH_BASE + 0x018)  // 报警状态寄存器，Chip复位域
} FLASHTypeDef;

// ICACHE
typedef struct
{
    volatile uint32_t CSR; //(ICACHE_BASE + 0x000)
} ICACHETypeDef;

// UCDR
typedef struct
{
    volatile uint32_t CTRL;    //(UCDR_BASE + 0X00)
    volatile uint32_t CLKUCDRDIV;  //(UCDR_BASE + 0X04)
    volatile uint32_t DTCMODESEL;  //(UCDR_BASE + 0X08)
    volatile uint32_t DlYCNTDEFT;  //(UCDR_BASE + 0X0C)
    volatile uint32_t DlYCNTDTC;   //(UCDR_BASE + 0X10)
    volatile uint32_t DlYCNTADJ;   //(UCDR_BASE + 0X14)
    volatile uint32_t CKCNTDEFT;   //(UCDR_BASE + 0X18)
    volatile uint32_t CKCNTDTC;    //   (UCDR_BASE + 0X1C)
    volatile uint32_t CKCNTADJ;    //   (UCDR_BASE + 0X20)
    volatile uint32_t BITWIDTHCNT; //(UCDR_BASE + 0X24)
    volatile uint32_t CKUNITDEFT;  //(UCDR_BASE + 0X28)
    volatile uint32_t CKUNITDTC;   //(UCDR_BASE + 0X2C)
    volatile uint32_t CKUNITADJ;   //(UCDR_BASE + 0X30)
    volatile uint32_t OSC300MCFG0; //(UCDR_BASE + 0X34)
    volatile uint32_t OSC300MCFG1; //(UCDR_BASE + 0X38)
    volatile uint32_t DlYCNTBIAS;  //(UCDR_BASE + 0X3C)
    volatile uint32_t RESERVE;  //(UCDR_BASE + 0X40)
    volatile uint32_t SYNCDTCFREQ; //(UCDR_BASE + 0X44)
} UCDRTypeDef;

// ADCDAC 0x5070 0000
typedef struct
{
    volatile uint32_t INTSTATUS;//RD ONLY
    volatile uint32_t INTSETSTATUS;//WR ONLY
    volatile uint32_t INTCLRSTATUS;//WR ONLY
    volatile uint32_t INTENABLE;//RD ONLY
    volatile uint32_t INTSETENABLE;//WR ONLY
    volatile uint32_t INTCLRENABLE;//WR ONLY
}ADCDACTypeDef;
//------------------------------------------------------------------------------
//                       REGISTER DEFINATION
//------------------------------------------------------------------------------
#define ARM ((ARMTypeDef*)SCB_BASE)
#define FLASH ((FLASHTypeDef*)FLASH_BASE)
#define CRC ((CRCTypeDef*)CRC_BASE)
#define USB ((USBTypeDef*)USBDC_BASE)
#define SPIM0 ((SPIMTypeDef*)SPIM0_BASE)
#define SPIM1 ((SPIMTypeDef*)SPIM1_BASE)
#define ICACHE ((ICACHETypeDef*)ICACHE_BASE)
#define SPIS0 ((SPISTypeDef*)SPIS0_BASE)
#define SPIS1 ((SPISTypeDef*)SPIS1_BASE)
#define I2C0 ((I2CTypeDef*)I2C0_BASE)
#define I2C1 ((I2CTypeDef*)I2C1_BASE)
#define TIMER0 ((TIMERTypeDef*)TIMER0_BASE)
#define TIMER1 ((TIMERTypeDef*)TIMER1_BASE)
#define TIMER2 ((TIMERTypeDef*)TIMER2_BASE)
#define TIMER3 ((TIMERTypeDef*)TIMER3_BASE)
#define TIMER4 ((TIMERTypeDef*)TIMER4_BASE)
#define SCC ((SCCTypeDef*)SCC_BASE)
#define SCD ((SCDTypeDef*)SCD_BASE)
#define ADC ((ADCTypeDef*)ADC_BASE)
#define PWM ((PWMTypeDef*)PWM_BASE)
#define PWM0 ((PWMTypeDef*)PWM0_BASE)
#define PWM1 ((PWMTypeDef*)PWM1_BASE)
#define PWM2 ((PWMTypeDef*)PWM2_BASE)
#define PWM3 ((PWMTypeDef*)PWM3_BASE)
#define PWM4 ((PWMTypeDef*)PWM4_BASE)
#define PWM5 ((PWMTypeDef*)PWM5_BASE)
#define PWM6 ((PWMTypeDef*)PWM6_BASE)
#define PWM7 ((PWMTypeDef*)PWM7_BASE)
#define PWMGLB ((PWMGLBTypeDef*)PWMGLB_BASE)
#define DAC ((DACTypeDef*)DAC_BASE)
#define DACOUT ((DACOUTTypeDef*)DACOUT_BASE)
#define UART0 ((UARTTypeDef*)UART0_BASE)
#define UART1 ((UARTTypeDef*)UART1_BASE)
#define UART2 ((UARTTypeDef*)UART2_BASE)
#define CGU ((CGUTypeDef*)CGU_BASE)
#define RCU ((RCUTypeDef*)RCU_BASE)
#define SMC ((SMCTypeDef*)SMC_BASE)
#define PMU ((PMUTypeDef*)PMU_BASE)
#define RETPMU ((RETPMUTypeDef*)RETPMU_BASE)
#define IOM ((IOMTypeDef*)IOM_BASE)
#define WDT ((WDTTypeDef*)WDT_BASE)
#define DMA ((DMACTypeDef*)DMA_BASE)
#define UCDR ((UCDRTypeDef*)UCDR_BASE)
#define RCVCNT ((RCVCNTTypeDef*)RCVCNT_BASE)
#define RCVCNT_INT ((RCVCNTIntTypeDef*)RCVCNT_INT_BASE) 
#define AFEC ((AFECTypeDef*)AFEC_BASE)
#define RTC ((RTCTypeDef*)RTC_BASE)
#define ADCDAC ((ADCDACTypeDef*)ADCDAC_BASE)

#ifdef __cplusplus
}
#endif

#endif //__N32G020XX_H__

/** @} */
