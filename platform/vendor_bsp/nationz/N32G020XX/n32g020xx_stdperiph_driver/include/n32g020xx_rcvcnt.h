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
 * @file n32g020xx_rcvcnt.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief RCVCNT模块接口声明文件。
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup RCVCNT RCVCNT脉宽捕获器模块
 * @{
 */
#ifndef __N32G020XX_RCVCNT_H__
#define __N32G020XX_RCVCNT_H__
//------------------------------------------------------------------
//                        Headers
//------------------------------------------------------------------
#include "n32g020xx_common.h"

//------------------------------------------------------------------
//                        Definitions
//------------------------------------------------------------------


//------------------------------------------------------------------
//                        TypeDefs
//------------------------------------------------------------------
typedef enum
{
    OSC_CLK = 0,
    EXT_CRYSTAL = 1,    
} RcvcntClkSrc;

//------------------------------------------------------------------
//                        Exported variables
//------------------------------------------------------------------


//------------------------------------------------------------------
//                        Exported functions
//------------------------------------------------------------------
/**
 * @brief RCVCNT初始化函数
 * 
 * @param clk RCVCNT时钟源二选一，OSC_CLK：为OSC80M分频得到的20MHz，EXT_CRYSTAL：为12MHz/11.2896MHz
 * @param threshold RCVCNT滤波阈值，宽度低于该阈值的电平视为毛刺，不予处理，有效位为10位（0x0~0x3F）
 * 
 * 由于SWD接口与Xin/Xout接口复用GPIO20和GPIO21，故开启SWD，无法使用外部晶振！！
 */
void RCVCNT_Init(RcvcntClkSrc clk, uint16_t threshold);

/**
 * @brief RCVCNT_INT中断使能，含下降沿中断和上升沿中断
 * 
 */
void RCVCNT_EnableInt(void);

/**
 * @brief RCVCNT_INT中断禁止，含下降沿中断和上升沿中断
 * 
 */
void RCVCNT_DisableInt(void);

#endif //__N32G020XX_RCVCNT_H__

/** @} */
