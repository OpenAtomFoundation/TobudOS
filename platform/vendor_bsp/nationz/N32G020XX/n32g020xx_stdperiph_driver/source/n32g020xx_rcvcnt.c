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
 * @file n32g020xx_rcvcnt.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief RCVCNT模块接口实现文件。
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_rcvcnt.h"
#include "n32g020xx.h"
#include "n32g020xx_cgu.h"
#include "n32g020xx_iom.h"

/**
 * @brief 12M晶振配置函数，含使能12MHz晶振、配置IOM中Xin和Xout
 * 
 */
static void RCVCNT_ConfigCrystal(void)
{
    // 使能CGU 12M晶振
    CGU_EnableModuleFunctions(CGUM_CRST12M);

    // 配置IOM Xin/Xout
    IOM_ConfigGpio20Xin12M(); // 配置GPIO20为Xin_12M
    IOM_ConfigGpio21Xout12M(); // 配置GPIO20为Xout_12M

    IOM_ConfigGpioOut(GPIO20, 0); // 关闭Xin输出
    IOM_ConfigGpioIn(GPIO20, 0); // 关闭Xin输入
    IOM_ConfigGpioPullUp(GPIO20, 0); // 关闭Xin上拉
    IOM_ConfigGpioPullDown(GPIO20, 0); // 关闭Xin下拉
    IOM_ConfigGpioOut(GPIO21, 0); // 关闭Xout输出
    IOM_ConfigGpioIn(GPIO21, 0); // 关闭Xout输入
    IOM_ConfigGpioPullUp(GPIO21, 0); // 关闭Xout上拉
    IOM_ConfigGpioPullDown(GPIO21, 0); // 关闭Xout下拉   
}

/**
 * @brief RCVCNT时钟配置函数
 * 
 * @param clk RCVCNT时钟源二选一，OSC_CLK：为OSC80M分频得到的20MHz，EXT_CRYSTAL：为12MHz/11.2896MHz
 */
static void RCVCNT_ConfigClk(RcvcntClkSrc clk)
{    
    if (EXT_CRYSTAL == clk)
    {        
        // 配置晶振
        RCVCNT_ConfigCrystal();

        // RCVCNT时钟源为外部晶振
        SET_REG_BIT(CGU->PWMRCVCLKCR, BIT2);
    }
	else
	{
        // RCVCNT时钟源为OSC80M分频得到的20MHz
        CLR_REG_BIT(CGU->PWMRCVCLKCR, BIT2);		
	}

    // 使能RCVCNT时钟和RCVCNT pclk
    CGU_EnableModuleFunctions(CGUM_RCVCNT_CLK | CGUM_RCVCNT_PCLK);
}

/**
 * @brief 比较器配置函数，含配置IOM Comp_Dp和Comp_Dm、配置Comp来源(Comp)和比较器相关使能
 * 
 * 驱动中暂仅支持RCV来源为比较器一种情况！！
 * 驱动中比较器配置已使能比较器偏置电压！！
 */
static void RCVCNT_ConfigComp(void)
{
    // 配置CompDp和Comp_Dm输入
    IOM_ConfigGpio26CompP(); 
    IOM_ConfigGpio27CompM(); 

    IOM_ConfigGpioOut(GPIO26, 0); // 关闭GPIO26输出
    IOM_ConfigGpioIn(GPIO26, 1); // 使能GPIO26输入
    IOM_ConfigGpioPullUp(GPIO26, 0); // 关闭GPIO26上拉
    IOM_ConfigGpioPullDown(GPIO26, 0); // 关闭GPIO26下拉
    IOM_ConfigGpioOut(GPIO27, 0); // 关闭GPIO27输出
    IOM_ConfigGpioIn(GPIO27, 1); // 使能GPIO27输入
    IOM_ConfigGpioPullUp(GPIO27, 0); // 关闭GPIO27上拉
    IOM_ConfigGpioPullDown(GPIO27, 0); // 关闭GPIO27下拉

    // RCV源配置为Comp
    CLR_REG_BIT(AFEC->CR, BIT4);
    // 使能比较器，比较器正端/负端输入
    SET_REG_BIT(AFEC->COMPCR1, (BIT2 | BIT1 | BIT0));
    // 使能比较器偏置电压(差模电路/单端输入需使能)
    SET_REG_BIT(AFEC->COMPCR1, (BIT4 | BIT3)); 
}

/**
 * @brief RCVCNT初始化函数
 * 
 * @param clk RCVCNT时钟源二选一，0：为OSC80M分频得到的20MHz，1：为12MHz/11.2896MHz
 * @param threshold RCVCNT滤波阈值，宽度低于该阈值的电平视为毛刺，不予处理，有效位为10位（0x0~0x3F）
 * 
 * 由于SWD接口与Xin/Xout接口复用GPIO20和GPIO21，故开启SWD，无法使用外部晶振！！
 */
void RCVCNT_Init(RcvcntClkSrc clk, uint16_t threshold)
{   
    // 使能IOM模块，配置12MHz晶振Xin/Xout和比较器输入/输出需要操作IOM
    CGU_EnableModuleFunctions(CGUM_IOM_PCLK);

    // 配置RCVCNT时钟源
    RCVCNT_ConfigClk(clk);

    // 使能AFEC时钟，配置比较器需要操作AFEC
    CGU_EnableModuleFunctions(CGUM_AFEC_PCLK); 
    
    // 配置比较器
    RCVCNT_ConfigComp();

    // 配置滤波阈值寄存器
    SET_REG_VAL(RCVCNT->CTHRL, threshold & 0xFF);
    SET_REG_VAL(RCVCNT->CTHRH, (threshold >> 8) & 0x03);    

    // 使能RCV计数
    SET_REG_BIT(RCVCNT->CR, BIT0);

    // 清零RCV计数
    SET_REG_BIT(RCVCNT->CR, BIT3);    
}

/**
 * @brief RCVCNT_INT中断使能，含下降沿中断和上升沿中断
 * 
 */
void RCVCNT_EnableInt(void)
{ 
    // 使能RCV上升沿，下降沿中断
    SET_REG_BIT(RCVCNT->CR, (BIT2 | BIT1));
    SET_REG_BIT(RCVCNT_INT->INT_SET_ENABLE, (BIT2 | BIT1));
    // 使能RCVCNT_INT中断
    NVIC_EnableIRQ(RcvCnt_Exception_IrqNum);
}

/**
 * @brief RCVCNT_INT中断禁止，含下降沿中断和上升沿中断
 * 
 */
void RCVCNT_DisableInt(void)
{
    // 关闭RCV上升沿，下降沿中断
    CLR_REG_BIT(RCVCNT->CR, (BIT2 | BIT1));
    SET_REG_BIT(RCVCNT_INT->INT_CLR_ENABLE, (BIT2 | BIT1));
    // 关闭RCVCNT_INT中断
    NVIC_DisableIRQ(RcvCnt_Exception_IrqNum);
}

