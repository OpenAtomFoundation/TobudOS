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
 * @brief CGU功能配置接口声明文件。
 * @file n32g020xx_cgu.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup CGU CGU时钟控制单元模块
 * @{
 */

#ifndef __N32G020XX_CGU_H__
#define __N32G020XX_CGU_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    CGU_SUCCESS = 0,     ///< 操作成功
    CGU_PARAM_INCORRECT, ///< 参数错误
};

#define CGUM_CRC_HCLK 0x0000000000000001ULL
#define CGUM_DMA_HCLK 0x0000000000000004ULL
#define CGUM_PWM_PCLK 0x0000000000000200ULL
#define CGUM_RCVCNT_PCLK 0x0000000000000400ULL
#define CGUM_SPIM0_PCLK 0x0000000000000800ULL
#define CGUM_SPIM1_PCLK 0x0000000000001000ULL
#define CGUM_SPIS0_PCLK 0x0000000000002000ULL
#define CGUM_SPIS1_PCLK 0x0000000000004000ULL
#define CGUM_SCD_PCLK 0x0000000000008000ULL
#define CGUM_I2C0_PCLK 0x0000000000010000ULL
#define CGUM_I2C1_PCLK 0x0000000000020000ULL
#define CGUM_SCC_PCLK 0x0000000000040000ULL
#define CGUM_UART0_PCLK 0x0000000000080000ULL
#define CGUM_UART1_PCLK 0x0000000000100000ULL
#define CGUM_UART2_PCLK 0x0000000000200000ULL
#define CGUM_IOM_PCLK 0x0000000000800000ULL
#define CGUM_WDT_PCLK 0x0000000001000000ULL
#define CGUM_RETPMU_PCLK 0x0000000010000000ULL
#define CGUM_PMU_PCLK 0x0000000020000000ULL
#define CGUM_AFEC_PCLK 0x0000000040000000ULL
#define CGUM_DACOUT_PCLK 0x0000000080000000ULL
#define CGUM_UCDR_PCLK 0x0000000100000000ULL
#define CGUM_DAC_PCLK 0x0000000400000000ULL
#define CGUM_ADC_PCLK 0x0000000800000000ULL
#define CGUM_TMR_PCLK 0x0000001000000000ULL
#define CGUM_SCD_CLK 0x0000010000000000ULL
#define CGUM_SPIS0_CLK 0x0000400000000000ULL
#define CGUM_SPIS1_CLK 0x0000800000000000ULL

#define CGUM_USBPHY_CLK 0x0004000000000000ULL
#define CGUM_USB_HCLK 0x0010000000000000ULL
#define CGUM_UCDR 0x0020000000000000ULL
#define CGUM_DAC_CLK 0x0040000000000000ULL
#define CGUM_ADC_CLK 0x0080000000000000ULL
#define CGUM_SCC_CLK 0x0100000000000000ULL
#define CGUM_OSC80MOUT 0x0200000000000000ULL
#define CGUM_CRST12M 0x0400000000000000ULL
#define CGUM_RCVCNT_CLK 0x0800000000000000ULL
#define CGUM_PWM_CLK 0x1000000000000000ULL

#define CGUM_ALL 0x1FFFE71DFDBFFF0FULL

/**
 * @brief 使能各模块时钟。
 *
 * @param modules 模块对应值的按位或操作的结果。
 *
 * @return `CGU_SUCCESS`
 */
int32_t CGU_EnableModuleFunctions(uint64_t modules);

/**
 * @brief 关闭各模块时钟。
 *
 * @param modules 模块对应值的按位或操作的结果。
 *
 * @return `CGU_SUCCESS`
 */
int32_t CGU_DisableModuleFunctions(uint64_t modules);

/**
 * @brief 设置HCLK的分频值。
 *
 * @param div 分频值，可取值为1，2，4，8 ... 512
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigClkMainDiv(uint32_t div);

/**
 * @brief 设置PCLK1分频值。
 *
 * @param div 分频值，可取值为1，2，4，8，16
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigPclk1Div(uint32_t div);

/**
 * @brief 设置PCLK2的分频值。
 *
 * @param div 分频值，可取值为1，2，4，8 ... 128
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigPclk2Div(uint32_t div);

/**
 * @brief 定义时钟源
 */
typedef enum ClockSrcEnum
{
    CLK_SRC_OSC_80M = 0,      ///< 内部80M OSC
    CLK_SRC_EXTERNAL_12M,     ///< 外部12M/11.8296M晶体
    CLK_SRC_OSC_EXTERNAL_32K, ///< 内部或外部32K时钟源
} ClockSrcEnum;

/**
 * @brief 配置WDT时钟相关参数。
 *
 * @param clk_src 时钟源，可以是`CLK_SRC_OSC_80M`或`CLK_SRC_EXTERNAL_12M`
 * @param div   分频值，可取值为 1 ~ 128
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigWdtClock(ClockSrcEnum clk_src, uint32_t div);

/**
 * @brief 配置UART时钟相关参数。
 *
 * @param channel UART通道号，值为0，1，2
 * @param clk_src 时钟源，可以是`CLK_SRC_OSC_80M`或`CLK_SRC_EXTERNAL_12M`
 * @param div   分频值，可取值为 1 ~ 128
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t
CGU_ConfigUartClock(uint8_t channel, ClockSrcEnum clk_src, uint32_t div);

/**
 * @brief 配置Timer时钟相关参数
 *
 * @param timer     Timer通道号，0 ~ 4
 * @param clk_src   时钟源，Timer0~3 可以是`CLK_SRC_OSC_80M`或
 * `CLK_SRC_EXTERNAL_12M`，Timer4可以是所有3种时钟源
 * @param div   分频值，可取值为 1 ~ 128
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigTimerClock(uint8_t timer, ClockSrcEnum clk_src, uint32_t div);

/**
 * @brief 使能Timer时钟。
 *
 * @param timer     Timer通道号，0 ~ 4
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_EnableTimerClock(uint8_t timer);

/**
 * @brief 关闭Timer时钟。
 *
 * @param timer     Timer通道号，0 ~ 4
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_DisableTimerClock(uint8_t timer);

/**
 * @brief 设置DAC时钟相关参数。
 *
 * @param clk_src 时钟源，可以是`CLK_SRC_OSC_80M`或`CLK_SRC_EXTERNAL_12M`
 * @param div   分频值，可取值为 1 ~ 1024
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigDacClock(ClockSrcEnum clk_src, uint32_t div);

/**
 * @brief 设置ADC时钟相关参数。
 *
 * @param clk_src 时钟源，可以是`CLK_SRC_OSC_80M`或`CLK_SRC_EXTERNAL_12M`
 * @param div   时钟源为`CLK_SRC_OSC_80M`时，可取值为 2 ~ 16384，时钟源为
 * `CLK_SRC_EXTERNAL_12M`时，取值为1或2
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigAdcClock(ClockSrcEnum clk_src, uint32_t div);

/**
 * @brief 设置SCC时钟分频值。
 *
 * @param div 分频值，可取值为 2 ~ 128，且必须为偶数
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigSccClock(uint32_t div);

/**
 * @brief 设置OSC80M片外输出时钟的分频值。
 *
 * @param div 分频值，可取值为 2 ~ 128, 且必须为偶数
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigOsc80MOutClock(uint32_t div);

/**
 * @brief 定义OSC80IP的内部输出频率值。
 */
typedef enum OscFrequenceEnum
{
    OSC_FREQUENCE_40M, ///< 输出40M
    OSC_FREQUENCE_80M, ///< 输出80M
} OscFrequenceEnum;

/**
 * @brief 配置OSC80IP的输出频率。
 *
 * @param freq 输出频率的枚举值。
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigOscClock(OscFrequenceEnum freq);

/**
 * @brief 设置ClkSrc分频值。
 *
 * @param div 分频值，可取值为1或2
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigClkSrcDiv(uint32_t div);

/**
 * @brief 设置ClkTop分频值。
 *
 * @param div 分频值，可取值为1，2，4，8
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigClkTopDiv(uint32_t div);

/**
 * @brief 配置PWM的时钟源。
 *
 * @param clk_src 时钟源，可以是`CLK_SRC_OSC_80M`或`CLK_SRC_EXTERNAL_12M`
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigPwmClock(ClockSrcEnum clk_src);

/**
 * @brief 配置RCVCNT的时钟源。
 *
 * @param clk_src 时钟源，可以是`CLK_SRC_OSC_80M`或`CLK_SRC_EXTERNAL_12M`
 *
 * @return `CGU_SUCCESS`表示成功，`CGU_PARAM_INCORRECT`表示参数错误
 */
int32_t CGU_ConfigRcvcntClock(ClockSrcEnum clk_src);

/**
 * @brief 取得内部OSC的频率。
 *
 * @return 频率值，为80000000或40000000。
 */
uint32_t CGU_GetOscFrequence(void);

/**
 * @brief 取得ClkTopPre阶段的频率值。
 *
 * @return 频率值
 */
uint32_t CGU_GetClkTopPreFrequence(void);

/**
 * @brief 取得ClkTop阶段的频率值。
 *
 * @return 频率值
 */
uint32_t CGU_GetClkTopFrequence(void);

/**
 * @brief 取得ClkMain阶段的频率值。
 *
 * @return 频率值
 */
uint32_t CGU_GetClkMainFrequence(void);

/**
 * @brief 取得HCLK的频率值。
 *
 * @return 频率值
 */
uint32_t CGU_GetHclkFrequence(void);

/**
 * @brief 取得PCLK1的频率值。
 *
 * @return 频率值
 */
uint32_t CGU_GetPclk1Frequence(void);

/**
 * @brief 取得PCLK2的频率值。
 *
 * @return 频率值
 */
uint32_t CGU_GetPclk2Frequence(void);

#ifdef __cplusplus
}
#endif

#endif // __N32G020XX_CGU_H__

/** @} */
