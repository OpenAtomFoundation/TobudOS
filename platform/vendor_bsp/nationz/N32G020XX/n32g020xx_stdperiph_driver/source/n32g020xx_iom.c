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
 * @file n32g020xx_iom.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @brief IO管理模块接口实现文件。
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_iom.h"

#define IOM_OFFSET4 (0) // 寄存器便宜地址间隔为4字节
#define IOM_OFFSET8 (1) // 寄存器便宜地址间隔为8字节

#define IOM_GPIO_BIT_OPR(OPR_NAME, REG_NAME, OFFSET, ATTR)                     \
    OPR_NAME##_REG_BIT(                                                        \
        *((volatile uint32_t*)(&IOM->REG_NAME) + (((ATTR) >> 3) << (OFFSET))),      \
        (0x01UL << ((ATTR) & 0x07)))

/**
 * @brief  GPIO正沿中断使能函数
 *
 * @param GPIO_X 支持GPIO0~29
 */
static inline void IOM_EnableGpioIntPosEdge(IOM_GPIO GPIO_X)
{
    IOM_GPIO_BIT_OPR(SET, INTPEGA, IOM_OFFSET8, GPIO_X);
}

/**
 * @brief GPIO正沿中断禁止函数，原型：void IOM_DisableGpioIntPosEdge(IOM_GPIO
 * GPIO_X)
 *
 * @param GPIOx 支持GPIO0~29
 */

/**
 * @brief GPIO正沿中断禁止函数
 *
 * @param GPIOx
 */
static inline void IOM_DisableGpioIntPosEdge(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, INTPEGA, IOM_OFFSET8, GPIOx);
}

/**
 * @brief GPIO正沿中断触发开关函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：正沿中断禁止，其他：正沿中断使能
 */
inline void IOM_ConfigGpioIntPosEdge(IOM_GPIO GPIOx, uint32_t enable)
{
    if (enable)
    {
        IOM_EnableGpioIntPosEdge(GPIOx);
    }
    else
    {
        IOM_DisableGpioIntPosEdge(GPIOx);
    }
}

/**
 * @brief GPIO负沿中断使能函数
 *
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_EnableGpioIntNegEdge(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(SET, INTNEGA, IOM_OFFSET8, GPIOx);
}

/**
 * @brief GPIO负沿中断禁止函数
 *
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_DisableGpioIntNegEdge(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, INTNEGA, IOM_OFFSET8, GPIOx);
}

/**
 * @brief GPIO负沿中断触发开关函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：负沿中断禁止，其他：负沿中断使能
 */
inline void IOM_ConfigGpioIntNegEdge(IOM_GPIO GPIOx, uint32_t enable)
{
    if (enable)
    {
        IOM_EnableGpioIntNegEdge(GPIOx);
    }
    else
    {
        IOM_DisableGpioIntNegEdge(GPIOx);
    }
}

/**
 * @brief GPIO中断状态清除函数
 *
 * @param GPIOx 支持GPIO0~29
 */
inline void IOM_ClearGpioIntState(IOM_GPIO GPIOx)
{    
    SET_REG_VAL(*((volatile uint32_t*)(&IOM->INTSTATA) + (GPIOx / 8)),
                         (0x01UL << (GPIOx % 8)));
}

/**
 * @brief GPIO中断状态获取函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param state 输出GPIOx的中断状态
 */
inline void IOM_GetGpioIntStat(IOM_GPIO GPIOx, uint32_t* state)
{
    *state = GET_REG_BIT(*((volatile uint32_t*)(&IOM->INTSTATA) + (GPIOx / 8)),
                         (0x01UL << (GPIOx % 8)));
}

/**
 * @brief GPIO唤醒使能函数
 *
 * @param GPIOx 支持GPIO0~15
 */
static inline void IOM_EnableGpioWakeUp(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(SET, WUENA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO唤醒禁止函数
 *
 * @param GPIOx 支持GPIO0~15
 */
static inline void IOM_DisableGpioWakeUp(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, WUENA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO唤醒开关函数
 *
 * @param GPIOx 支持GPIO0~15
 * @param enable 0：GPIO唤醒禁止，其他：GPIO唤醒使能
 * @return uint32_t IOM_SUCCESS：成功，IOM_GPIO_ERR：GPIO超出范围
 */
inline uint32_t IOM_ConfigGpioWakeUp(IOM_GPIO GPIOx, uint32_t enable)
{
    if (GPIOx > 15)
    {
        return IOM_GPIO_ERR;
    }
    else
    {
        if (enable)
        {
            IOM_EnableGpioWakeUp(GPIOx);
        }
        else
        {
            IOM_DisableGpioWakeUp(GPIOx);
        }
        return IOM_SUCCESS;
    }
}

static inline void IOM_SetGpioWakeUpHighLevel(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(SET, WULEVELA, IOM_OFFSET4, GPIOx);
}

static inline void IOM_SetGpioWakeUpLowLevel(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, WULEVELA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO唤醒电平配置函数
 *
 * @param GPIOx 支持GPIO0~15
 * @param level 0：低电平，其他：高电平
 * @return uint32_t IOM_SUCCESS：成功，IOM_GPIO_ERR：GPIO超出范围
 */
inline uint32_t IOM_SetGpioWakeUpLevel(IOM_GPIO GPIOx, uint32_t level)
{
    if (GPIOx > 15)
    {
        return IOM_GPIO_ERR;
    }
    else
    {
        if (level)
        {
            IOM_SetGpioWakeUpHighLevel(GPIOx);
        }
        else
        {
            IOM_SetGpioWakeUpLowLevel(GPIOx);
        }
        return IOM_SUCCESS;
    }
}

/**
 * @brief GPIO功能配置函数
 *
 * @param GPIOx 支持GPIO0~29
 */
inline void IOM_ConfigGpioMode(IOM_GPIO GPIOx)
{
    CLR_REG_BIT(*((volatile uint32_t*)(&IOM->CR1) + (GPIOx / 2)),
                (0x07UL << (3 * (GPIOx % 2))));
}

static inline void IOM_SetGpioHighLevel(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(SET, GPIODRA, IOM_OFFSET4, GPIOx);
}

static inline void IOM_SetGpioLowLevel(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, GPIODRA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO输出电平配置函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param level 0：低电平，其他：高电平
 */
inline void IOM_SetGpioLevel(IOM_GPIO GPIOx, uint32_t level)
{
    if (level)
    {
        IOM_SetGpioHighLevel(GPIOx);
    }
    else
    {
        IOM_SetGpioLowLevel(GPIOx);
    }
}

/**
 * @brief GPIO电平状态获取函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param level 输出GPIOx电平状态
 */
inline void IOM_GetGpioLevel(IOM_GPIO GPIOx, uint32_t* level)
{
    *level = GET_REG_BIT(*((volatile uint32_t*)(&IOM->GPIODRA) + (GPIOx / 8)),
                         (0x01UL << (GPIOx % 8)));
}

/**
 * @brief GPIO输出使能函数
 *
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_EnableGpioOut(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(SET, GPIOOEA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO输出禁止函数
 *
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_DisableGpioOut(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, GPIOOEA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO输出开关函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO输出禁止，其他：GPIO输出使能
 */
inline void IOM_ConfigGpioOut(IOM_GPIO GPIOx, uint32_t enable)
{
    if (enable)
    {
        IOM_EnableGpioOut(GPIOx);
    }
    else
    {
        IOM_DisableGpioOut(GPIOx);
    }
}

/**
 * @brief GPIO输入使能函数
 *
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_EnableGpioIn(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(SET, GPIOIEA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO输入禁止函数
 *
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_DisableGpioIn(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, GPIOIEA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO输入开关函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO输入禁止，其他：GPIO输入使能
 */
inline void IOM_ConfigGpioIn(IOM_GPIO GPIOx, uint32_t enable)
{
    if (enable)
    {
        IOM_EnableGpioIn(GPIOx);
    }
    else
    {
        IOM_DisableGpioIn(GPIOx);
    }
}

/**
 * @brief GPIO上拉使能函数
 *
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_EnableGpioPullUp(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(SET, GPIOPUA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO上拉禁止函数
 *
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_DisableGpioPullUp(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, GPIOPUA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO上拉开关函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO上拉禁止，其他：GPIO上拉使能
 */
inline void IOM_ConfigGpioPullUp(IOM_GPIO GPIOx, uint32_t enable)
{
    if (enable)
    {
        IOM_EnableGpioPullUp(GPIOx);
    }
    else
    {
        IOM_DisableGpioPullUp(GPIOx);
    }
}

/**
 * @brief GPIO下拉使能函数
 *
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_EnableGpioPullDown(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(SET, GPIOPDA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO下拉禁止函数
 * 
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_DisableGpioPullDown(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, GPIOPDA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO下拉开关函数
 * 
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO下拉禁止，其他：GPIO下拉使能
 */
inline void IOM_ConfigGpioPullDown(IOM_GPIO GPIOx, uint32_t enable)
{
    if(enable)
    {
        IOM_EnableGpioPullDown(GPIOx);
    }
    else
    {
        IOM_DisableGpioPullDown(GPIOx);
    }
}

/**
 * @brief GPIO SMT使能函数
 * 
 * @param GPIOx GPIO SMT使能函数
 */
static inline void IOM_EnableGpioSmt(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(SET, GPIOSMTA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO SMT禁止函数
 * 
 * @param GPIOx 支持GPIO0~29
 */
static inline void IOM_DisableGpioSmt(IOM_GPIO GPIOx)
{
    IOM_GPIO_BIT_OPR(CLR, GPIOSMTA, IOM_OFFSET4, GPIOx);
}

/**
 * @brief GPIO SMT开关函数
 * 
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO SMT禁止，其他：GPIO SMT使能
 */
inline void IOM_ConfigGpioSmt(IOM_GPIO GPIOx, uint32_t enable)
{
    if(enable)
    {
        IOM_EnableGpioSmt(GPIOx);
    }
    else
    {
        IOM_DisableGpioSmt(GPIOx);
    }
}

/**
 * @brief GPIO驱动能力配置函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param val
 * 四选一，0：驱动能力4.5mA，1：驱动能力9mA，2：驱动能力13.5mA，3：驱动能力18mA
 */
inline void IOM_ConfigGpioPinDrvCapa(IOM_GPIO GPIOx, IOM_PADS_DRV_CAPA val)
{
    IOM_SET_REG_MSK_VAL(*((volatile uint32_t*)(&IOM->GPIOPINA1) + (GPIOx / 4)),
                        (0x03UL << ((GPIOx % 4) << 1)),
                        (val << ((GPIOx % 4) << 1)));
}

/**
 * @brief PWM紧急停车输入源配置函数
 *
 * @param GPIOx 支持GPIO14~18和GPIO20~23
 * @return uint32_t IOM_SUCCESS：成功，IOM_GPIO_ERR：GPIO超出范围
 */
inline uint32_t IOM_SetPwmEmergencyStopSrc(IOM_GPIO GPIOx)
{
    if (GPIOx >= 14 && GPIOx <= 18)
    {
        IOM_SET_REG_MSK_VAL(
            IOM->PWMEMSTP, (BIT3 | BIT2 | BIT1 | BIT0), (GPIOx - 14));
    }
    else if (GPIOx >= 20 && GPIOx <= 23)
    {
        IOM_SET_REG_MSK_VAL(
            IOM->PWMEMSTP, (BIT3 | BIT2 | BIT1 | BIT0), (GPIOx - 15));
    }
    else
    {
        return IOM_GPIO_ERR;
    }

    return IOM_SUCCESS;
}
