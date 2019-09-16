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
 * @file n32g020xx_common.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief 通用功能接口声明文件。
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup COMMON 通用功能模块
 * @{
 */

#ifndef __N32G020XX_COMMON_H__
#define __N32G020XX_COMMON_H__

#include <stdint.h>

#include "n32g020xx_cgu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief 定义寄存器的BIT位掩码 */
#define BIT0     ((uint32_t)1 << 0)
#define BIT1     ((uint32_t)1 << 1)
#define BIT2     ((uint32_t)1 << 2)
#define BIT3     ((uint32_t)1 << 3)
#define BIT4     ((uint32_t)1 << 4)
#define BIT5     ((uint32_t)1 << 5)
#define BIT6     ((uint32_t)1 << 6)
#define BIT7     ((uint32_t)1 << 7)
#define BIT8     ((uint32_t)1 << 8)
#define BIT9     ((uint32_t)1 << 9)
#define BIT10    ((uint32_t)1 << 10)
#define BIT11    ((uint32_t)1 << 11)
#define BIT12    ((uint32_t)1 << 12)
#define BIT13    ((uint32_t)1 << 13)
#define BIT14    ((uint32_t)1 << 14)
#define BIT15    ((uint32_t)1 << 15)
#define BIT16    ((uint32_t)1 << 16)
#define BIT17    ((uint32_t)1 << 17)
#define BIT18    ((uint32_t)1 << 18)
#define BIT19    ((uint32_t)1 << 19)
#define BIT20    ((uint32_t)1 << 20)
#define BIT21    ((uint32_t)1 << 21)
#define BIT22    ((uint32_t)1 << 22)
#define BIT23    ((uint32_t)1 << 23)
#define BIT24    ((uint32_t)1 << 24)
#define BIT25    ((uint32_t)1 << 25)
#define BIT26    ((uint32_t)1 << 26)
#define BIT27    ((uint32_t)1 << 27)
#define BIT28    ((uint32_t)1 << 28)
#define BIT29    ((uint32_t)1 << 29)
#define BIT30    ((uint32_t)1 << 30)
#define BIT31    ((uint32_t)1 << 31)


#define SET_REG_VAL(reg, val)           ((reg) = (val))     ///< 设置寄存器的值

#define GET_REG_VAL(reg)                (reg)               ///< 取得寄存器的值

/**
 * @brief 设置寄存器的某些位的值为1
 * 
 * @param reg 寄存器
 * @param bit 只有`bit`中值为1的位，才会被设置到寄存器中，`bit`为0的位在寄存器中值不变
 */
#define SET_REG_BIT(reg, bit)           ((reg) |= (bit))

/**
 * @brief 设置寄存器的某些位的值
 * 
 * @param reg 寄存器
 * @param bit 要设置的值
 * @param msk `msk`中为1的位对应的`bit`参数中的值会被设置到寄存器中，0或1都会被设置
 */
#define SET_REG_BIT_MSK(reg, bit, msk)  ((reg) = ((reg) & ~(msk) | (bit)))

/**
 * @brief 清除寄存器的某些位的值为0
 * 
 * @param reg 寄存器
 * @param bit 只有`bit`中值为1的位，才会在寄存器中清0，`bit`为0的位在寄存器中值不变
 * @return  
 */
#define CLR_REG_BIT(reg, bit)           ((reg) &= ~(bit))

/**
 * @brief 取得寄存器中某些位的值
 * 
 * @param reg 寄存器
 * @param bit 只有`bit`中值为1的位，才会返回寄存器中的值，`bit`为0的位总是返回0
 * @return  
 */
#define GET_REG_BIT(reg, bit)           ((reg) & (bit))

/**
 * @brief 延时指定的毫秒数。
 * 
 * @param ms  毫秒数
 */
void Delay_MS(uint32_t ms);

/**
 * @brief 延时指定的微秒数。
 * 
 * @param us  微秒数
 */
void Delay_US(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif // !defined __N32G020XX_COMMON_H__

/** @} */
