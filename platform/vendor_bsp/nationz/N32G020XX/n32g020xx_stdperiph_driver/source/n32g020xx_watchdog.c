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
 * @brief WDT模块的接口实现。
 * @file n32g020xx_watchdog.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_watchdog.h"

#include <stdbool.h>
#include <stdint.h>

#include "n32g020xx_cgu.h"
#include "n32g020xx_common.h"
#include "n32g020xx.h"

#define BIT_RWT BIT0
#define BIT_EWT BIT1
#define BIT_WD0 BIT2
#define BIT_WD1 BIT3
#define BIT_CLRINT BIT6

#define NVIC_ISER                                                              \
    (*((volatile unsigned long*)(0xE000E100))) // Interrupt Set-Enable Register
#define NVIC_ICER                                                              \
    (*((volatile unsigned long*)(0xE000E180))) // Interrupt Clear Enable
// Register
#define NVIC_ISPR                                                              \
    (*((volatile unsigned long*)(0xE000E200))) // Interrupt Set-Pending Register
#define NVIC_ICPR                                                              \
    (*((volatile unsigned long*)(0xE000E280))) // Interrupt Clear-Pending
// Register

/*
 * 若软件需要对 WDTCSR 的低两位进行写操作，则必须先向写保护寄存器 WDTTAP 中写入
 * 数据 0xAA，在之后的 27 个系统周期里再向该寄存器中写入 0x55，再在紧接的 27 个
 * 系统周期里对 WDT 状态控制寄存器 WDTCSR 的 EWT 位或 RWT 位进行写操作，才能改变
 * 这两个信号的内容。
 */
#define WDT_TapSet()                                                           \
    do                                                                         \
    {                                                                          \
        SET_REG_VAL(WDT->TAP, 0xAA);                                           \
        SET_REG_VAL(WDT->TAP, 0x55);                                           \
    } while (0)

#define WDT_IRQ_BIT BIT0
#define WDT_EnableIRQ()                                                        \
    do                                                                         \
    {                                                                          \
        SET_REG_VAL(NVIC_ICPR, WDT_IRQ_BIT);                                   \
        SET_REG_VAL(NVIC_ISER, WDT_IRQ_BIT);                                   \
    } while (0)

#define WDT_DisableIRQ()                                                       \
    do                                                                         \
    {                                                                          \
        SET_REG_VAL(NVIC_ICER, WDT_IRQ_BIT);                                   \
    } while (0)

/* 清中断状态 */
#define WDT_ClearIntFlag()                                                     \
    do                                                                         \
    {                                                                          \
        SET_REG_BIT(WDT->CSR, BIT_CLRINT);                                     \
    } while (0)

static WDT_Callback s_call_back = 0;

/* WDT中断函数 */
void WDT_IRQHandler(void)
{
    WDT_ClearIntFlag();
    if (s_call_back)
    {
        // 调用用户指定的回调函数
        s_call_back();
    }
}

#define WDT_ACTUAL_FREQ_INTERNAL 20000000

int32_t WDT_Init(const WdtConfig* config)
{
    /*
     * 通过用户给出的时间，计算WDT的分频值和WDT计数溢出值参数（表9-4）
     *
     * -------------------------------------------------------------
     * | WDTCSR[3] | WDTCSR[2] | 中断产生时间 | 复位产生时间       |
     * |-----------|-----------|--------------|------------------- |
     * | 0         | 0         | 2^17 clocks  | 2^17 + 512 clocks  |
     * | 0         | 1         | 2^20 clocks  | 2^20 + 512 clocks  |
     * | 1         | 0         | 2^23 clocks  | 2^23 + 512 clocks  |
     * | 1         | 1         | 2^26 clocks  | 2^26 + 512 clocks  |
     * -------------------------------------------------------------
     *
     * 设溢出值为N个周期（根据表9-4，只有4种取值），用户设置超时时间为t，时钟源
     * 频率为F，分频值为d，则有 N / (F / d) == t，
     * 因此，d == (t * F) / N
     * 通过代入4种N的值，可求得最合适的d值
     */

    bool use_external_clk = (config->frequence != WDT_FREQ_INTERNAL);

    // WDT的时钟根据选择，为OSC80M分频得到的20M时钟，或外部的12M/11.8296M时钟
    // freq的单位为KHz
    uint32_t freq = use_external_clk ? config->frequence / 1000
                                     : WDT_ACTUAL_FREQ_INTERNAL / 1000;

    // time_ms单位为毫秒，和freq相乘正好不需要除以1000
    uint32_t d = (config->time_ms * freq) >> 17; // 取第一个溢出值 2^17

    uint8_t wd_val = 0;

    // 分频的取值范围是 1 ~ 128
    while (d > 128 && wd_val < 3)
    {
        d >>= 3; // 取下一个溢出值
        wd_val++;
    }

    if (d < 1 || d > 128)
    {
        // 指定的时长超出范围
        return WDT_PARAM_INCORRECT;
    }

    // 配置CGU
    if (CGU_SUCCESS
        != CGU_ConfigWdtClock(
               use_external_clk ? CLK_SRC_EXTERNAL_12M : CLK_SRC_OSC_80M, d))
    {
        return WDT_PARAM_INCORRECT;
    }

    // 配置回调函数
    s_call_back = config->callback;

    // 启动CGU
    CGU_EnableModuleFunctions(CGUM_WDT_PCLK);

    WDT_ClearIntFlag();

    uint32_t csr_val = 0;

    // 配置溢出值参数
    csr_val |= wd_val << 2;

    // 配置模式
    switch (config->mode)
    {
    case WDT_MODE_INT:
        WDT_EnableIRQ();
        break;
    case WDT_MODE_RST:
        WDT_DisableIRQ();
        csr_val |= BIT_EWT;
        break;
    case WDT_MODE_INT_RST:
        WDT_EnableIRQ();
        csr_val |= BIT_EWT;
        break;
    default:
        return WDT_PARAM_INCORRECT;
    }

    csr_val |= BIT_RWT; // 复位计数器的值

    WDT_TapSet();
    SET_REG_VAL(WDT->CSR, csr_val);

    return WDT_SUCCESS;
}

int32_t WDT_Close(void)
{
    WDT_DisableIRQ();

    // 关闭复位使能
    WDT_TapSet();
    SET_REG_VAL(WDT->CSR, 0);

    // 关闭CGU
    CGU_DisableModuleFunctions(CGUM_WDT_PCLK);

    // 清除回调函数
    s_call_back = 0;

    return WDT_SUCCESS;
}

int32_t WDT_FeedDog(void)
{
    uint32_t csr_val = GET_REG_VAL(WDT->CSR);
    csr_val |= BIT_RWT; // 复位计数器的值
    WDT_TapSet();
    SET_REG_VAL(WDT->CSR, csr_val);
    return WDT_SUCCESS;
}
