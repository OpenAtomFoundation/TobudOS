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
 * @file n32g020xx_dac.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief DAC模块的接口实现文件。
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_dac.h"
#include "n32g020xx_dma.h"
#include "n32g020xx_cgu.h"

#include <stdio.h>

/**
 * @brief DAC_SetVrefOpt 设置DAC量化电平档位
 *
 * @param vrefOpt 量化电平档位
 */
static void DAC_SetVrefOpt(VrefOptEnum vrefOpt)
{
    switch (vrefOpt)
    {
    case VREF_OPT_1P2V:
        // 1.2V
        SET_REG_BIT_MSK(DAC->CR, 0, BIT1 | BIT2);
        break;
    case VREF_OPT_1P8V:
        // 1.8V
        SET_REG_BIT_MSK(DAC->CR, BIT1 | 0, BIT1 | BIT2);
        break;
    case VREF_OPT_2P4V:
        // 2.4V
        SET_REG_BIT_MSK(DAC->CR, 0 | BIT2, BIT1 | BIT2);
        break;
    case VREF_OPT_3P0V:
    default:
        // 3.0V
        SET_REG_BIT_MSK(DAC->CR, BIT1 | BIT2, BIT1 | BIT2);
        break;
    }
}

int32_t DAC_Init(DACConfig* pDACConfig)
{
    if ((uint32_t)pDACConfig->pin < 1 || (uint32_t)pDACConfig->pin > 3)
    {
        return DAC_PARAM_INCORRECT;
    }
    // 配置DAC分频
    if (CGU_SUCCESS != CGU_ConfigDacClock(CLK_SRC_OSC_80M, pDACConfig->dacClkDiv))
    {
        return DAC_PARAM_INCORRECT;
    }
    // 使能DAC相关时钟
    CGU_EnableModuleFunctions(CGUM_DAC_CLK | CGUM_DAC_PCLK | CGUM_DACOUT_PCLK);
    // 使能DACOUT
    SET_REG_BIT(DACOUT->CR1, BIT1);
    // 配置DAC输出到哪个管脚
    SET_REG_BIT_MSK(DACOUT->CR2, (uint32_t)pDACConfig->pin, BIT1 | BIT0);
    //设置DAC量化电平值
    DAC_SetVrefOpt(pDACConfig->vrefOpt);
    // DAC数据来源控制：SoftMode，数据来自DACREG
    SET_REG_BIT_MSK(DAC->CR, 0 | 0 | 0 | BIT6, BIT3 | BIT4 | BIT5 | BIT6);
    // DAC模拟IP使能控制,使能DAC
    SET_REG_BIT_MSK(DAC->CR, BIT0, BIT0);
    // DACFIFO清空控制，此位写1时产生DACFIFO指标清零操作
    SET_REG_BIT_MSK(DAC->CR, BIT8, BIT8);
    return DAC_SUCCESS;
}

int32_t DAC_Output(uint16_t data)
{
    SET_REG_BIT_MSK(DAC->REG, data & 0x3FF, BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7 | BIT8 | BIT9); //DacSetRegVal(___parabuf[2])
    return DAC_SUCCESS;
}

int32_t DAC_Close(void)
{
    CGU_DisableModuleFunctions(CGUM_DAC_CLK | CGUM_DAC_PCLK | CGUM_DACOUT_PCLK);
    return DAC_SUCCESS;
}
