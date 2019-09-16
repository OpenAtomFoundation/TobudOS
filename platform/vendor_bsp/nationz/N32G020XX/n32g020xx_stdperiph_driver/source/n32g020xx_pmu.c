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
 * @file n32g020xx_pmu.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief PMU模块接口实现文件
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_pmu.h"

/**
 * @brief PMU唤醒源配置函数
 * 
 * @param src 唤醒源
 * @param sel 二选一，0：唤醒禁止或RCV/比较器低电平唤醒或RXEV事件唤醒，其他：唤醒使能或RCV/比较器高电平唤醒或INT10唤醒
 * 
 * 部分唤醒源需要叠加配置才能有效，具体详见产品用户手册
 */
void PMU_ConfigWakeUpSrc(PMU_WAKEUP_SRC src, uint32_t sel)
{
    if(sel)
    {
        SET_REG_BIT(*((volatile uint32_t*)(&PMU->WKSREN0) + (src / 8)), (0x01UL << (src % 8)));
    }
    else
    {        
        CLR_REG_BIT(*((volatile uint32_t*)(&PMU->WKSREN0) + (src / 8)), (0x01UL << (src % 8)));   
    }
}

/**
 * @brief 清除PMU唤醒状态函数
 * 
 * @param io 详见枚举类型PMU_WAKEUP_STATE_IO或产品用户手册
 */
void PMU_ClearWakeUpState(PMU_WAKEUP_STATE_IO io)
{
    SET_REG_VAL(*((volatile uint32_t*)(&PMU->WKSTR0) + (io / 8)), (0x01UL << (io % 8)));
}

/**
 * @brief 获取PMU唤醒状态函数
 * 
 * @param io 详见枚举类型PMU_WAKEUP_STATE_IO或产品用户手册
 * @param state io对应的唤醒状态
 */
void PMU_GetWakeUpState(PMU_WAKEUP_STATE_IO io, uint32_t *state)
{
    *state = GET_REG_BIT(*((volatile uint32_t*)(&PMU->WKSTR0) + (io / 8)), (0x01UL << (io % 8)));
}

/**
 * @brief 进入IDLE函数
 * 
 * @param method 二选一，WAKEUP_WFI：WFI方式，WAKEUP_WFE：WFE方式
 */
void PMU_EnterIdle(PMU_WU_METHOD method)
{
    CLR_REG_BIT(ARM->SCR, BIT2); // SRC[SLEEPDEEP] = 0, 0 = idle
    if(method == WAKEUP_WFI)
    {
        // 使用WFI进入Idle
        __asm("WFI");
    }
    else if(method == WAKEUP_WFE)
    {
        // 使用WFE进入Idle
        SET_REG_VAL(NVIC->ISER[0U], 0U); // 屏蔽所有中断，唤醒后再设置ISER
        __asm("SEV"); // 置一个事件
        __asm("WFE"); // 清除事件
        __asm("WFE"); // 进Idle
    }
}

/**
 * @brief 进入STANDBY函数
 * 
 * @param method 二选一，WAKEUP_WFI：WFI方式，WAKEUP_WFE：WFE方式
 */
void PMU_EnterStandBy(PMU_WU_METHOD method)
{
    SET_REG_BIT(ARM->SCR, BIT2); // SRC[SLEEPDEEP] = 1, 1 = StandBy
    if(method == WAKEUP_WFI)
    {
        // 使用WFI进入Deep Idle
        __asm("WFI");
    }
    else if(method == WAKEUP_WFE)
    {
        // 使用WFE进入Idle
        SET_REG_VAL(NVIC->ISER[0U], 0U); // 屏蔽所有中断，唤醒后再设置ISER
        __asm("SEV"); // 置一个事件
        __asm("WFE"); // 清除事件
        __asm("WFE"); // 进Deep Idle
    }
}

#define RETPMU_PMU_MODE_MASK (BIT2 | BIT1 | BIT0)
/**
 * @brief 利用RETPMU进入PowerDown
 * 
 * @param mode 三选一，NORMAL_MODE，PD_MODE_RTC_SRAM，PD_MODE，详见产品用户手册
 */
inline void RETPMU_SetPmuMode(RETPMU_PMU_MODE mode)
{
    // 使能PMU_MODE_en，配置PMU_MODE
    SET_REG_VAL(RETPMU->CTRL, BIT3 | (mode & 0x07));
} 

