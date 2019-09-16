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
 * @brief CGU功能配置接口实现文件。
 * @file n32g020xx_cgu.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 */

#include "n32g020xx_cgu.h"
#include "n32g020xx_common.h"
#include "n32g020xx.h"

#define IS_POW_OF_2(x) (((x) & ((x)-1)) == 0)

/**
 * @brief 计算传入的整数是2的多少次方。
 * @param x 整数值，必须是2的N次方且不为0。
 * @return 指数值。
 */
static __inline uint32_t CGU_CalcExp(uint32_t x)
{
    uint32_t exp = 0;
    while ((x & 0x01) == 0)
    {
        exp++;
        x >>= 1;
    }
    return exp;
}

int32_t CGU_EnableModuleFunctions(uint64_t modules)
{
    // 低48位的模块功能掩码正好和寄存器的位置是对应的
    SET_REG_BIT(CGU->MCGR1, modules & 0xff);
    SET_REG_BIT(CGU->MCGR2, (modules >> 8) & 0xff);
    SET_REG_BIT(CGU->MCGR3, (modules >> 16) & 0xff);
    SET_REG_BIT(CGU->MCGR4, (modules >> 24) & 0xff);
    SET_REG_BIT(CGU->MCGR5, (modules >> 32) & 0xff);
    SET_REG_BIT(CGU->MCGR6, (modules >> 40) & 0xff);

    // 剩余的模块功能需要单独配置寄存器
    if (modules & CGUM_USBPHY_CLK)
    {
        SET_REG_BIT(CGU->USBCLKCR, BIT5);
    }
    if (modules & CGUM_USB_HCLK)
    {
    }
    if (modules & CGUM_UCDR)
    {
    }
    if (modules & CGUM_DAC_CLK)
    {
        SET_REG_BIT(CGU->DACCLKCR, BIT0); // bit DACCLKEN
    }
    if (modules & CGUM_ADC_CLK)
    {
        SET_REG_BIT(CGU->ADCCLKCR, BIT0); // bit ADCCLKEN
    }
    if (modules & CGUM_SCC_CLK)
    {
        SET_REG_BIT(CGU->SCCCLKCR, BIT6); // bit SCCCLKEN
    }
    if (modules & CGUM_OSC80MOUT)
    {
        SET_REG_BIT(CGU->OSC80MOUTCR, BIT6); // bit OSC80MOUTEN
    }
    if (modules & CGUM_CRST12M)
    {
        SET_REG_BIT(CGU->CLKSRCFREQCR, BIT4); // bit CRST12MEN
    }
    if (modules & CGUM_RCVCNT_CLK)
    {
        SET_REG_BIT(CGU->PWMRCVCLKCR, BIT3); // bit RCVCNTCLKEN
    }
    if (modules & CGUM_PWM_CLK)
    {
        SET_REG_BIT(CGU->PWMRCVCLKCR, BIT1); // bit PWMCLKEN
    }

    // 检查分频状态是否完成

    if (modules & CGUM_UART2_PCLK)
    {
        while (!GET_REG_BIT(CGU->DIVSR0, BIT7))
            ;
    }
    if (modules & CGUM_UART1_PCLK)
    {
        while (!GET_REG_BIT(CGU->DIVSR0, BIT6))
            ;
    }
    if (modules & CGUM_UART0_PCLK)
    {
        while (!GET_REG_BIT(CGU->DIVSR0, BIT5))
            ;
    }
    if (modules & CGUM_OSC80MOUT)
    {
        while (!GET_REG_BIT(CGU->DIVSR1, BIT1))
            ;
    }
    if (modules & CGUM_WDT_PCLK)
    {
        while (!GET_REG_BIT(CGU->DIVSR1, BIT0))
            ;
    }
    if (modules & CGUM_DAC_CLK)
    {
        while (!GET_REG_BIT(CGU->DACCLKCR, BIT1))
            ;
    }
    if (modules & CGUM_ADC_CLK)
    {
        while (GET_REG_BIT(CGU->ADCCLKCR, BIT1))
            ;
    }

    return CGU_SUCCESS;
}

int32_t CGU_DisableModuleFunctions(uint64_t modules)
{
    // 低48位的模块功能掩码正好和寄存器的位置是对应的
    CLR_REG_BIT(CGU->MCGR1, modules & 0xff);
    CLR_REG_BIT(CGU->MCGR2, (modules >> 8) & 0xff);
    CLR_REG_BIT(CGU->MCGR3, (modules >> 16) & 0xff);
    CLR_REG_BIT(CGU->MCGR4, (modules >> 24) & 0xff);
    CLR_REG_BIT(CGU->MCGR5, (modules >> 32) & 0xff);
    CLR_REG_BIT(CGU->MCGR6, (modules >> 40) & 0xff);

    // 剩余的模块功能需要单独配置寄存器
    if (modules & CGUM_USBPHY_CLK)
    {
        CLR_REG_BIT(CGU->USBCLKCR, BIT5);
    }
    if (modules & CGUM_USB_HCLK)
    {
    }
    if (modules & CGUM_UCDR)
    {
    }
    if (modules & CGUM_DAC_CLK)
    {
        CLR_REG_BIT(CGU->DACCLKCR, BIT0); // bit DACCLKEN
    }
    if (modules & CGUM_ADC_CLK)
    {
        CLR_REG_BIT(CGU->ADCCLKCR, BIT0); // bit ADCCLKEN
    }
    if (modules & CGUM_SCC_CLK)
    {
        CLR_REG_BIT(CGU->SCCCLKCR, BIT6); // bit SCCCLKEN
    }
    if (modules & CGUM_OSC80MOUT)
    {
        CLR_REG_BIT(CGU->OSC80MOUTCR, BIT6); // bit OSC80MOUTEN
    }
    if (modules & CGUM_CRST12M)
    {
        CLR_REG_BIT(CGU->CLKSRCFREQCR, BIT4); // bit CRST12MEN
    }
    if (modules & CGUM_RCVCNT_CLK)
    {
        CLR_REG_BIT(CGU->PWMRCVCLKCR, BIT3); // bit RCVCNTCLKEN
    }
    if (modules & CGUM_PWM_CLK)
    {
        CLR_REG_BIT(CGU->PWMRCVCLKCR, BIT1); // bit PWMCLKEN
    }

    return CGU_SUCCESS;
}

int32_t CGU_ConfigClkMainDiv(uint32_t div)
{
    // HCLK的分频值为1，2，4，8 ... 512
    if (!IS_POW_OF_2(div) || div < 1 || div > 512)
    {
        return CGU_PARAM_INCORRECT;
    }

    // 计算div是2的多少次方
    uint32_t exp = CGU_CalcExp(div);

    // 配置寄存器CLK80MDIV1的低4位
    SET_REG_BIT_MSK(CGU->CLK80MDIV1, exp, BIT0 | BIT1 | BIT2 | BIT3);

    return CGU_SUCCESS;
}

int32_t CGU_ConfigPclk1Div(uint32_t div)
{
    // PCLK1的分频值为1,2,4,8,16
    if (!IS_POW_OF_2(div) || div < 1 || div > 16)
    {
        return CGU_PARAM_INCORRECT;
    }

    uint32_t exp = CGU_CalcExp(div);

    // 配置寄存器CLK80MDIV1的BIT4~6位
    SET_REG_BIT_MSK(CGU->CLK80MDIV1, exp << 4, BIT4 | BIT5 | BIT6);

    return CGU_SUCCESS;
}

int32_t CGU_ConfigPclk2Div(uint32_t div)
{
    // PCLK2的分频值为1,2,4,8, ... 128
    if (!IS_POW_OF_2(div) || div < 1 || div > 128)
    {
        return CGU_PARAM_INCORRECT;
    }

    uint32_t exp = CGU_CalcExp(div);

    // 配置寄存器CLK80MDIV2的BIT3~5
    SET_REG_BIT_MSK(CGU->CLK80MDIV2, exp << 3, BIT3 | BIT4 | BIT5);

    return CGU_SUCCESS;
}

int32_t CGU_ConfigWdtClock(ClockSrcEnum clk_src, uint32_t div)
{
    uint32_t reg_val = 0;

    // watchdog支持1~128分频
    if (div < 1 || div > 128 || clk_src == CLK_SRC_OSC_EXTERNAL_32K)
    {
        return CGU_PARAM_INCORRECT;
    }

    reg_val = div - 1;

    // 是否使用外部时钟
    if (clk_src == CLK_SRC_EXTERNAL_12M)
    {
        reg_val |= BIT7;
    }

    // 配置寄存器
    SET_REG_VAL(CGU->WDTCLKCR, reg_val);
    return CGU_SUCCESS;
}

int32_t CGU_ConfigUartClock(uint8_t channel, ClockSrcEnum clk_src, uint32_t div)
{
    uint32_t reg_val = 0;
    // UART支持1~128分频
    if (div < 1 || div > 128 || clk_src == CLK_SRC_OSC_EXTERNAL_32K)
    {
        return CGU_PARAM_INCORRECT;
    }

    reg_val = div - 1;

    // 是否使用外部时钟
    if (clk_src == CLK_SRC_EXTERNAL_12M)
    {
        reg_val |= BIT7;
    }

    // 配置channel对应的寄存器
    switch (channel)
    {
    case 0:
        SET_REG_VAL(CGU->UART0CLKCR, reg_val);
        break;
    case 1:
        SET_REG_VAL(CGU->UART1CLKCR, reg_val);
        break;
    case 2:
        SET_REG_VAL(CGU->UART2CLKCR, reg_val);
        break;
    default:
        return CGU_PARAM_INCORRECT;
    }

    return CGU_SUCCESS;
}

int32_t CGU_ConfigTimerClock(uint8_t timer, ClockSrcEnum clk_src, uint32_t div)
{
    // TIMER支持1~128分频，只有TIMER4支持内部或外部32K时钟
    if (div < 1 || div > 128 || timer > 4
        || (timer != 4 && clk_src == CLK_SRC_OSC_EXTERNAL_32K))
    {
        return CGU_PARAM_INCORRECT;
    }

    // 配置时钟源
    if (timer < 4)
    {
        SET_REG_BIT_MSK(
            CGU->TMRCLKSEL, (uint32_t)clk_src << timer, BIT0 << timer);
    }
    else
    {
        SET_REG_BIT_MSK(CGU->TMRCLKSEL, (uint32_t)clk_src << 4, BIT4 | BIT5);
    }

    // 配置分频
    // TIMER的寄存器地址是连续排布的
    volatile uint32_t* reg_addr = &(CGU->TMR0CLKCR) + timer;
    SET_REG_BIT_MSK(
        *reg_addr, div - 1, BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6);

    return CGU_SUCCESS;
}

int32_t CGU_EnableTimerClock(uint8_t timer)
{
    if (timer > 4)
    {
        return CGU_PARAM_INCORRECT;
    }
    // TIMER的寄存器地址是连续排布的
    volatile uint32_t* reg_addr = &(CGU->TMR0CLKCR) + timer;
    SET_REG_BIT(*reg_addr, BIT7);

    // 检查分频状态有效
    uint32_t bit = BIT4 >> timer;
    while (!GET_REG_BIT(CGU->DIVSR0, bit))
        ;

    return CGU_SUCCESS;
}
int32_t CGU_DisableTimerClock(uint8_t timer)
{
    if (timer > 4)
    {
        return CGU_PARAM_INCORRECT;
    }
    // TIMER的寄存器地址是连续排布的
    volatile uint32_t* reg_addr = &(CGU->TMR0CLKCR) + timer;
    CLR_REG_BIT(*reg_addr, BIT7);

    return CGU_SUCCESS;
}

int32_t CGU_ConfigDacClock(ClockSrcEnum clk_src, uint32_t div)
{
    if (div < 1 || div > 1024 || clk_src == CLK_SRC_OSC_EXTERNAL_32K)
    {
        return CGU_PARAM_INCORRECT;
    }

    uint32_t fdr0, fdr1;
    fdr0 = (div - 1) & 0xff;
    fdr1 = ((div - 1) >> 8) & 0x03;
    if (clk_src == CLK_SRC_EXTERNAL_12M)
    {
        fdr1 |= BIT2;
    }

    /*
     * DACCLKFDR0 的改写应该和DACCLKFDR1 的改写先后连续进行，中间不可插入别的
     * CPU 操作
     */
    SET_REG_VAL(CGU->DACCLKFDR0, fdr0);
    SET_REG_VAL(CGU->DACCLKFDR1, fdr1);

    return CGU_SUCCESS;
}

int32_t CGU_ConfigAdcClock(ClockSrcEnum clk_src, uint32_t div)
{
    switch (clk_src)
    {
    case CLK_SRC_OSC_80M:
        if (div < 2 || div > (1 << 14))
        {
            //在使用 OSC80 的情况下，(ADCDIV1<<8 + ADCDIV0+1)应该为>=2.
            return CGU_PARAM_INCORRECT;
        }
        break;
    case CLK_SRC_EXTERNAL_12M:
        if (div == 0 || div > 2)
        {
            //在外部晶体的情况下，ADCCLKDIV0 应该设定为 0 或1，选择 1 分频
            return CGU_PARAM_INCORRECT;
        }
        break;
    default:
        return CGU_PARAM_INCORRECT;
    }

    uint32_t fdr0, fdr1;
    fdr0 = (div - 1) & 0xff;
    fdr1 = ((div - 1) >> 8) & 0x03;
    if (clk_src == CLK_SRC_EXTERNAL_12M)
    {
        fdr1 |= BIT2;
    }
    /*
     * ADCCLKFDR0 的改写和ADCCLKFDR1 的改写应该为连续的先后关系。中间不可以有别
     * 的寄存器软件操作。
     */
    SET_REG_VAL(CGU->ADCCLKFDR0, fdr0);
    SET_REG_VAL(CGU->ADCCLKFDR1, fdr1);

    return CGU_SUCCESS;
}

int32_t CGU_ConfigSccClock(uint32_t div)
{
    // SCC支持2 ~ 128 分频，且必须为偶数
    if (div < 2 || div > 128 || (div & 0x01) == 1)
    {
        return CGU_PARAM_INCORRECT;
    }

    // 配置SCCCLKCR寄存器的Bit0~5
    SET_REG_BIT_MSK(
        CGU->SCCCLKCR, (div >> 1) - 1, BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);

    return CGU_SUCCESS;
}

int32_t CGU_ConfigOsc80MOutClock(uint32_t div)
{
    //  OSC80MOUT支持2 ~ 128 分频，且必须为偶数
    if (div < 2 || div > 128 || (div & 0x01) == 1)
    {
        return CGU_PARAM_INCORRECT;
    }

    // 配置OSC80MOUTCR寄存器的Bit0~5
    SET_REG_BIT_MSK(CGU->OSC80MOUTCR,
                    (div >> 1) - 1,
                    BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);

    return CGU_SUCCESS;
}

// 40M / 80M
int32_t CGU_ConfigOscClock(OscFrequenceEnum freq)
{
    switch (freq)
    {
    case OSC_FREQUENCE_80M:
    case OSC_FREQUENCE_40M:
        // 配置CLKSRCFREQCR的OSCOUTCTRL
        SET_REG_BIT_MSK(CGU->CLKSRCFREQCR, (uint32_t)freq, BIT0);
        return CGU_SUCCESS;
    default:
        return CGU_PARAM_INCORRECT;
    }
}

int32_t CGU_ConfigClkSrcDiv(uint32_t div)
{
    if (div < 1 || div > 2)
    {
        return CGU_PARAM_INCORRECT;
    }

    // 配置CLKSRCFREQCR的CLKSRCDIV
    SET_REG_BIT_MSK(CGU->CLKSRCFREQCR, (div - 1), BIT1);

    return CGU_SUCCESS;
}

int32_t CGU_ConfigClkTopDiv(uint32_t div)
{
    if (!IS_POW_OF_2(div) || div < 1 || div > 8)
    {
        return CGU_PARAM_INCORRECT;
    }

    // 配置CLKSRCFREQCR的CLKTOPDIV
    SET_REG_BIT_MSK(CGU->CLKSRCFREQCR, CGU_CalcExp(div) << 2, BIT2 | BIT3);

    return CGU_SUCCESS;
}

int32_t CGU_ConfigPwmClock(ClockSrcEnum clk_src)
{
    switch (clk_src)
    {
    case CLK_SRC_OSC_80M:
        CLR_REG_BIT(CGU->PWMRCVCLKCR, BIT0);
        break;
    case CLK_SRC_EXTERNAL_12M:
        SET_REG_BIT(CGU->PWMRCVCLKCR, BIT0);
        break;
    default:
        return CGU_PARAM_INCORRECT;
    }
    return CGU_SUCCESS;
}

int32_t CGU_ConfigRcvcntClock(ClockSrcEnum clk_src)
{
    switch (clk_src)
    {
    case CLK_SRC_OSC_80M:
        CLR_REG_BIT(CGU->PWMRCVCLKCR, BIT2);
        break;
    case CLK_SRC_EXTERNAL_12M:
        SET_REG_BIT(CGU->PWMRCVCLKCR, BIT2);
        break;
    default:
        return CGU_PARAM_INCORRECT;
    }
    return CGU_SUCCESS;
}

uint32_t CGU_GetOscFrequence(void)
{
    return GET_REG_BIT(CGU->CLKSRCFREQCR, BIT0) ? 80000000 : 40000000;
}

uint32_t CGU_GetClkTopPreFrequence(void)
{
    uint32_t osc_freq = CGU_GetOscFrequence();
    if (GET_REG_BIT(CGU->CLKSRCFREQCR, BIT1))
    {
        // 2分频
        return osc_freq >> 1;
    }
    else
    {
        return osc_freq;
    }
}

uint32_t CGU_GetClkTopFrequence(void)
{
    uint32_t div_exp = GET_REG_BIT(CGU->CLKSRCFREQCR, BIT2 | BIT3);
    div_exp >>= 2;

    return CGU_GetClkTopPreFrequence() >> div_exp;
}

uint32_t CGU_GetClkMainFrequence(void)
{
    return CGU_GetClkTopFrequence();
}

uint32_t CGU_GetHclkFrequence(void)
{
    uint32_t div_exp = GET_REG_BIT(CGU->CLK80MDIV1, BIT0 | BIT1 | BIT2 | BIT3);
    if (div_exp > 0x09)
    {
        div_exp = 0x09;
    }
    return CGU_GetClkMainFrequence() >> div_exp;
}

uint32_t CGU_GetPclk1Frequence(void)
{
    uint32_t div_exp = GET_REG_BIT(CGU->CLK80MDIV1, BIT4 | BIT5 | BIT6);
    div_exp >>= 4;
    if (div_exp > 0x04)
    {
        div_exp = 0x04;
    }
    return CGU_GetClkMainFrequence() >> div_exp;
}

uint32_t CGU_GetPclk2Frequence(void)
{
    uint32_t div_exp = GET_REG_BIT(CGU->CLK80MDIV2, BIT3 | BIT4 | BIT5);
    div_exp >>= 3;
    return CGU_GetClkMainFrequence() >> div_exp;
}

