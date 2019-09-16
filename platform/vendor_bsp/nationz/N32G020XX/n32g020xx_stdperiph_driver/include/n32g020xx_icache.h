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
 * @file n32g020xx_icache.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief 指令缓存模块接口声明文件。
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup ICACHE ICACHE指令缓存模块
 * @{
 */
#ifndef __N32G020XX_ICACHE_H__
#define __N32G020XX_ICACHE_H__
//------------------------------------------------------------------
//                        Headers
//------------------------------------------------------------------
#include "n32g020xx_common.h"
#include "n32g020xx.h"

//------------------------------------------------------------------
//                        Definitions
//------------------------------------------------------------------
/**
 * @brief 启动ICACHE，上电复位后ICACHE处于关闭状态
 * 
 */
#define ICACHE_Enable() SET_REG_BIT(ICACHE->CSR, BIT0)

/**
 * @brief 关闭ICACHE
 * 
 */
#define ICACHE_Disable() CLR_REG_BIT(ICACHE->CSR, BIT0)

/**
 * @brief 清零ICACHE
 * 
 */
#define ICACHE_Clear()                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        ICACHE_Disable();                                                                                              \
        SET_REG_BIT(ICACHE->CSR, BIT1);                                                                                \
        while (GET_REG_BIT(ICACHE->CSR, BIT1))                                                                         \
            ;                                                                                                          \
    } while (0)

#define ICACHE_REP_MSK (BIT3 | BIT2)
#define ICACHE_LOCK_WAY_MSK (BIT7 | BIT6 | BIT5 | BIT4)
#define ICACHE_LOCK_WAY_REV_MSK (BIT11 | BIT10 | BIT9 | BIT8)
#define ICACHE_SIZE_MSK (BIT16 | BIT15 | BIT14 | BIT13 | BIT12)

/**
 * @brief 获取ICACHE容量，n32g020的ICACHE容量为2KBytes，读出的值为0xB
 * 
 */
#define ICACHE_GetSize() (GET_REG_BIT(ICACHE->CSR, ICACHE_SIZE_MSK) >> 12)

//------------------------------------------------------------------
//                        TypeDefs
//------------------------------------------------------------------
typedef enum
{
    WAY0 = (0x01 << 0), //锁定Way0
    WAY1 = (0x01 << 1), //锁定Way1
    WAY2 = (0x01 << 2), //锁定Way2
    WAY3 = (0x01 << 3), //锁定Way3
} ICACHE_LOCK_WAY;

//------------------------------------------------------------------
//                        Exported variables
//------------------------------------------------------------------


//------------------------------------------------------------------
//                        Exported functions
//------------------------------------------------------------------
/**
 * @brief ICACHE锁定Way选择LochStart指令，启动锁定程序
 * 
 * @param way 锁定的Way，WAY0：锁定Way0，WAY1：锁定Way1，WAY2：锁定Way2，WAY3：锁定Way3 
 */
void ICACHE_LockStart(ICACHE_LOCK_WAY way);

/**
 * @brief ICACHE锁定Way选择LochStop指令，停止锁定程序
 * 
 * @param way 锁定的Way，WAY0：锁定Way0，WAY1：锁定Way1，WAY2：锁定Way2，WAY3：锁定Way3
 */
void ICACHE_LockStop(ICACHE_LOCK_WAY way);

/**
 * @brief ICACHE锁定Way选择UnLock指令，解锁该Way锁定
 * 
 * @param way 锁定的Way，WAY0：锁定Way0，WAY1：锁定Way1，WAY2：锁定Way2，WAY3：锁定Way3
 */
void ICACHE_UnLock(ICACHE_LOCK_WAY way);

#endif // __N32G020XX_ICACHE_H__

/** @} */
