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
 * @file n32g020xx_rcu.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief RCU模块接口声明文件。
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @defgroup RCU RCU复位控制单元模块
 * @{
 */
#ifndef __N32G020XX_RCU_H__
#define __N32G020XX_RCU_H__
//------------------------------------------------------------------
//                        Headers
//------------------------------------------------------------------
#include "n32g020xx_common.h"
#include "n32g020xx.h"

//------------------------------------------------------------------
//                        Definitions
//------------------------------------------------------------------
/*!< Global Soft Reset */
#define RCU_GlobalReset()                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        SET_REG_BIT(RCU->CR0, BIT0);                                                                                   \
        SET_REG_BIT(RCU->CR0, BIT1);                                                                                   \
        SET_REG_BIT(RCU->CR0, BIT2);                                                                                   \
        SET_REG_BIT(RCU->CR0, BIT3);                                                                                   \
    } while (0)

/*!< Gbdn Reset */
#define RCU_EnableGbdnRst() SET_REG_BIT(RCU->CR0, BIT4)
#define RCU_DisableGbdnRst() CLR_REG_BIT(RCU->CR0, BIT4)

/*!< Gbd Reset */
#define RCU_EnableGbdRst() SET_REG_BIT(RCU->CR0, BIT5)
#define RCU_DisableGbdRst() CLR_REG_BIT(RCU->CR0, BIT5)

/*!< Vd25 Reset */
#define RCU_EnableVd25Rst() SET_REG_BIT(RCU->CR0, BIT6)
#define RCU_DisableVd25Rst() CLR_REG_BIT(RCU->CR0, BIT6)

/*!< PWM Soft Reset */
#define RCU_GetPwmSoftRst() GET_REG_BIT(RCU->CR1, BIT0)
#define RCU_SetPwmSoftRst() SET_REG_BIT(RCU->CR1_SET, BIT0)
#define RCU_ClearPwmSoftRst() SET_REG_BIT(RCU->CR1_CLR, BIT0)

/*!< SCD Soft Reset */
#define RCU_GetScdSoftRst() GET_REG_BIT(RCU->CR1, BIT1)
#define RCU_SetScdSoftRst() SET_REG_BIT(RCU->CR1_SET, BIT1)
#define RCU_ClearScdSoftRst() SET_REG_BIT(RCU->CR1_CLR, BIT1)

/*!< SCC Soft Reset */
#define RCU_GetSccSoftRst() GET_REG_BIT(RCU->CR1, BIT2)
#define RCU_SetSccSoftRst() SET_REG_BIT(RCU->CR1_SET, BIT2)
#define RCU_ClearSccSoftRst() SET_REG_BIT(RCU->CR1_CLR, BIT2)

/*!< I2C0 Soft Reset */
#define RCU_GetI2c0SoftRst() GET_REG_BIT(RCU->CR1, BIT4)
#define RCU_SetI2c0SoftRst() SET_REG_BIT(RCU->CR1_SET, BIT4)
#define RCU_ClearI2c0SoftRst() SET_REG_BIT(RCU->CR1_CLR, BIT4)

/*!< I2C1 Soft Reset */
#define RCU_GetI2c1SoftRst() GET_REG_BIT(RCU->CR1, BIT5)
#define RCU_SetI2c1SoftRst() SET_REG_BIT(RCU->CR1_SET, BIT5)
#define RCU_ClearI2c1SoftRst() SET_REG_BIT(RCU->CR1_CLR, BIT5)

/*!< SPIM0 Soft Reset */
#define RCU_GetSpim0SoftRst() GET_REG_BIT(RCU->CR1, BIT6)
#define RCU_SetSpim0SoftRst() SET_REG_BIT(RCU->CR1_SET, BIT6)
#define RCU_ClearSpim0SoftRst() SET_REG_BIT(RCU->CR1_CLR, BIT6)

/*!< SPIM1 Soft Reset */
#define RCU_GetSpim1SoftRst() GET_REG_BIT(RCU->CR1, BIT7)
#define RCU_SetSpim1SoftRst() SET_REG_BIT(RCU->CR1_SET, BIT7)
#define RCU_ClearSpim1SoftRst() SET_REG_BIT(RCU->CR1_CLR, BIT7)

/*!< DAC Soft Reset */
#define RCU_GetDacSoftRst() GET_REG_BIT(RCU->CR2, BIT0)
#define RCU_SetDacSoftRst() SET_REG_BIT(RCU->CR2_SET, BIT0)
#define RCU_ClearDacSoftRst() SET_REG_BIT(RCU->CR2_CLR, BIT0)

/*!< ADC Soft Reset */
#define RCU_GetAdcSoftRst() GET_REG_BIT(RCU->CR2, BIT1)
#define RCU_SetAdcSoftRst() SET_REG_BIT(RCU->CR2_SET, BIT1)
#define RCU_ClearAdcSoftRst() SET_REG_BIT(RCU->CR2_CLR, BIT1)

/*!< Timer0 Soft Reset */
#define RCU_GetTimer0SoftRst() GET_REG_BIT(RCU->CR2, BIT2)
#define RCU_SetTimer0SoftRst() SET_REG_BIT(RCU->CR2_SET, BIT2)
#define RCU_ClearTimer0SoftRst() SET_REG_BIT(RCU->CR2_CLR, BIT2)

/*!< Timer1 Soft Reset */
#define RCU_GetTimer1SoftRst() GET_REG_BIT(RCU->CR2, BIT3)
#define RCU_SetTimer1SoftRst() SET_REG_BIT(RCU->CR2_SET, BIT3)
#define RCU_ClearTimer1SoftRst() SET_REG_BIT(RCU->CR2_CLR, BIT3)

/*!< Timer2 Soft Reset */
#define RCU_GetTimer2SoftRst() GET_REG_BIT(RCU->CR2, BIT4)
#define RCU_SetTimer2SoftRst() SET_REG_BIT(RCU->CR2_SET, BIT4)
#define RCU_ClearTimer2SoftRst() SET_REG_BIT(RCU->CR2_CLR, BIT4)

/*!< Timer3 Soft Reset */
#define RCU_GetTimer3SoftRst() GET_REG_BIT(RCU->CR2, BIT5)
#define RCU_SetTimer3SoftRst() SET_REG_BIT(RCU->CR2_SET, BIT5)
#define RCU_ClearTimer3SoftRst() SET_REG_BIT(RCU->CR2_CLR, BIT5)

/*!< Timer4 Soft Reset */
#define RCU_GetTimer4SoftRst() GET_REG_BIT(RCU->CR2, BIT6)
#define RCU_SetTimer4SoftRst() SET_REG_BIT(RCU->CR2_SET, BIT6)
#define RCU_ClearTimer4SoftRst() SET_REG_BIT(RCU->CR2_CLR, BIT6)

/*!< UART0 Soft Reset */
#define RCU_GetUart0SoftRst() GET_REG_BIT(RCU->CR3, BIT0)
#define RCU_SetUart0SoftRst() SET_REG_BIT(RCU->CR3_SET, BIT0)
#define RCU_ClearUart0SoftRst() SET_REG_BIT(RCU->CR3_CLR, BIT0)

/*!< UART1 Soft Reset */
#define RCU_GetUart1SoftRst() GET_REG_BIT(RCU->CR3, BIT1)
#define RCU_SetUart1SoftRst() SET_REG_BIT(RCU->CR3_SET, BIT1)
#define RCU_ClearUart1SoftRst() SET_REG_BIT(RCU->CR3_CLR, BIT1)

/*!< UART2 Soft Reset */
#define RCU_GetUart2SoftRst() GET_REG_BIT(RCU->CR3, BIT2)
#define RCU_SetUart2SoftRst() SET_REG_BIT(RCU->CR3_SET, BIT2)
#define RCU_ClearUart2SoftRst() SET_REG_BIT(RCU->CR3_CLR, BIT2)

/*!< SPIS0 Soft Reset */
#define RCU_GetSpis0SoftRst() GET_REG_BIT(RCU->CR3, BIT3)
#define RCU_SetSpis0SoftRst() SET_REG_BIT(RCU->CR3_SET, BIT3)
#define RCU_ClearSpis0SoftRst() SET_REG_BIT(RCU->CR3_CLR, BIT3)

/*!< SPIS1 Soft Reset */
#define RCU_GetSpis1SoftRst() GET_REG_BIT(RCU->CR3, BIT4)
#define RCU_SetSpis1SoftRst() SET_REG_BIT(RCU->CR3_SET, BIT4)
#define RCU_ClearSpis1SoftRst() SET_REG_BIT(RCU->CR3_CLR, BIT4)

/*!< UCDR Soft Reset */
#define RCU_GetUcdrSoftRst() GET_REG_BIT(RCU->CR3, BIT5)
#define RCU_SetUcdrSoftRst() SET_REG_BIT(RCU->CR3_SET, BIT5)
#define RCU_ClearUcdrSoftRst() SET_REG_BIT(RCU->CR3_CLR, BIT5)

/*!< USB Soft Reset */
#define RCU_GetUsbSoftRst() GET_REG_BIT(RCU->CR3, BIT6)
#define RCU_SetUsbSoftRst() SET_REG_BIT(RCU->CR3_SET, BIT6)
#define RCU_ClearUsbSoftRst() SET_REG_BIT(RCU->CR3_CLR, BIT6)

/******  Reset State *********************************************************/
#define RCU_GetWdtRstState() GET_REG_BIT(RCU->SR0, BIT0)
#define RCU_ClearWdtRstState() SET_REG_VAL(RCU->SR0, BIT0)

#define RCU_GetScdRstState() GET_REG_BIT(RCU->SR0, BIT2)
#define RCU_ClearScdRstState() SET_REG_VAL(RCU->SR0, BIT2)

#define RCU_GetSecRstState() GET_REG_BIT(RCU->SR0, BIT3)
#define RCU_ClearSecRstState() SET_REG_VAL(RCU->SR0, BIT3)

#define RCU_GetGbdRstState() GET_REG_BIT(RCU->SR1, BIT1)
#define RCU_ClearGbdRstState() SET_REG_VAL(RCU->SR1, BIT1)

#define RCU_GetGbdnRstState() GET_REG_BIT(RCU->SR1, BIT2)
#define RCU_ClearGbdnRstState() SET_REG_VAL(RCU->SR1, BIT2)

/*!< 软件配置RCUCR0[3:0]或配置Core寄存器AIRCR[2]进行复位 */
#define RCU_GetSoftRst0State() GET_REG_BIT(RCU->SR0, BIT1)
#define RCU_ClearSoftRst0State() SET_REG_VAL(RCU->SR0, BIT1)

/*!< 软件配置Core寄存器AIRCR[2]进行复位 */
#define RCU_GetSoftRst1State() GET_REG_BIT(RCU->SR1, BIT3)
#define RCU_ClearSoftRst1State() SET_REG_VAL(RCU->SR1, BIT3)


#endif // __N32G020XX_RCU_H__

/** @} */
