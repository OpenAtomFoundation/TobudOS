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
 * @file n32g020xx_icache.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief 指令缓存模块接口实现文件。
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_icache.h"

#define ICACHE_SET_REG_MSK_VAL(reg, msk, val) SET_REG_BIT_MSK(reg, val, msk)

/**
 * @brief ICACHE锁定Way选择LochStart指令，启动锁定程序
 * 
 * @param way 锁定的Way，WAY0：锁定Way0，WAY1：锁定Way1，WAY2：锁定Way2，WAY3：锁定Way3 
 */
inline void ICACHE_LockStart(ICACHE_LOCK_WAY way)
{    
    uint32_t lock_way_rev = GET_REG_BIT(ICACHE->CSR, ICACHE_LOCK_WAY_MSK) << 4;    
    ICACHE_SET_REG_MSK_VAL(ICACHE->CSR, ICACHE_LOCK_WAY_REV_MSK, lock_way_rev);
    
    ICACHE_SET_REG_MSK_VAL(ICACHE->CSR, ICACHE_LOCK_WAY_MSK, ((~ way) & 0x0FUL) << 4);
}

/**
 * @brief ICACHE锁定Way选择LochStop指令，停止锁定程序
 * 
 * @param way 锁定的Way，WAY0：锁定Way0，WAY1：锁定Way1，WAY2：锁定Way2，WAY3：锁定Way3
 */
inline void ICACHE_LockStop(ICACHE_LOCK_WAY way)
{    
    uint32_t lock_way = GET_REG_BIT(ICACHE->CSR, ICACHE_LOCK_WAY_REV_MSK) >> 4;
    ICACHE_SET_REG_MSK_VAL(ICACHE->CSR, ICACHE_LOCK_WAY_MSK, lock_way);
    
    SET_REG_BIT(ICACHE->CSR, way << 4);
}

/**
 * @brief ICACHE锁定Way选择UnLock指令，解锁该Way锁定
 * 
 * @param way 锁定的Way，WAY0：锁定Way0，WAY1：锁定Way1，WAY2：锁定Way2，WAY3：锁定Way3
 */
inline void ICACHE_UnLock(ICACHE_LOCK_WAY way)
{
    CLR_REG_BIT(ICACHE->CSR, way << 4);
}
