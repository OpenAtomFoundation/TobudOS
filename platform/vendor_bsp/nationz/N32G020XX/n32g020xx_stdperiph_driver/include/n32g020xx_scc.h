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
 * @brief SCC智能卡主机控制模块接口声明文件。
 * @file n32g020xx_scc.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup SCC SCC智能卡主机控制模块
 * @{
 */
#ifndef __N32G020XX_SCC_H__
#define __N32G020XX_SCC_H__

#include <stdbool.h>

#include "n32g020xx.h"
#include "n32g020xx_common.h"
#include "n32g020xx_cgu.h"
#include "n32g020xx_uart.h"
#include "n32g020xx_iom.h"

//-----------------------------------------------------------------------------
//                        TypeDefs
//-----------------------------------------------------------------------------

enum
{
    SCC_SUCCESS = 0,     ///< 操作成功
    SCC_PARAM_INCORRECT, ///< 参数错误
};


//Interrupt proc handle
typedef void ( *DmaProcHandle)(void);
typedef struct
{
	uint32_t locked;								
	DmaProcHandle ch_tc_func;				
	DmaProcHandle ch_buserr_func;			
	DmaProcHandle ch_accerr_func;			
	DmaProcHandle ch_halftc_func;			
}DMA_INTR_HANDLE;
		
typedef enum
{
    SCC_PARITY_EVEN,
    SCC_PARITY_ODD,
    SCC_PARITY_NONE
}SccParityType;

typedef struct
{    
    uint8_t div;        ///<打开使能 选择时钟来源 设置分频系数
	uint8_t conv;       ///< 0.自动; 1.手动 - 3B; 2.手动 - 3F
	uint8_t protocol;   ///<PROT=1 表示 T=1；  PROT=0 表示 T=0。
    SccParityType parity;     ///<SCC_PARITY_EVEN-even, SCC_PARITY_ODD-odd, SCC_PARITY_NONE: 关闭奇偶校验    
    uint8_t tbeIrq;     ///<1-表示 SCC 发送 FIFO 空，即发送完成；0-表示 SCC 发送 FIFO 非空      
    uint8_t rbfIrq;     ///<1-表示 SCC 接收 FIFO 已满；0-表示 SCC 接收 FIFO 未满    
    
    uint8_t clklevel;   ///<1，在使能停时钟功能时，输出到卡的 Clk IO 的电平值由本寄存器决定。 2，软件可读可写，复位值为高  
    
    uint8_t gt;         ///<1，该寄存器复位值是 0xff，T0 协议包含 2 个ETU 的固定 Guard Time；T1 协议包含 1 个ETU 的固定 Guard Time; 2，当该寄存器配置在 0-254，假设为 N 时，T0 协议下，Guard Time 为 N+2；T1 协议为N+1 
                               
    
}SCC_Config;



//-----------------------------------------------------------------------------
//                        Exported functions
//-----------------------------------------------------------------------------

/**
 * @brief 初始化SCC
 * @param _config  初始化配置信息
 * @return `SCC_SUCCESS`表示成功，`SCC_PARAM_INCORRECT`表示参数错误
 */
int32_t SCC_Init(const SCC_Config* _config);

/**
 * @brief set rst_io hight level and start card reset
 * @param isActivate  TRUE为激活SCC；FALSE为反激活SCC
 * @return `SCC_SUCCESS`表示成功，`SCC_PARAM_INCORRECT`表示参数错误
 */
int32_t SCC_SetRstIOAndResetCard(bool isActivate);


/**
 * @brief 设置奇偶校验
 * @param parityType SCC_PARITY_EVEN-even, SCC_PARITY_ODD-odd, SCC_PARITY_NONE: 关闭奇偶校验 
 * @return `SCC_SUCCESS`表示成功，`SCC_PARAM_INCORRECT`表示参数错误
 */
int32_t SCC_SetParity(const SccParityType parityType);

/**
 *  @brief 设置scc中断注册和使能
 *  @param isEnable TRUE为开启中断；FALSE为关闭中断			
 *  
 *  @return `SCC_SUCCESS`表示成功，`SCC_PARAM_INCORRECT`表示参数错误
*/
int32_t SCC_SetIrq(bool isEnable);

/**
 * @brief 发送数据
 * @param data  要发送的数据
 * @param len   要发送的数据的长度
 * 
 * @return `SCC_SUCCESS`表示成功，`SCC_PARAM_INCORRECT`表示参数错误
*/
int32_t SCC_SendData(const uint8_t *data, uint32_t len);

/**
 * @brief 接受数据
 * @param data  保存接受的数据
 * @param len   接受数据的长度
 * 
 * @return `SCC_SUCCESS`表示成功，`SCC_PARAM_INCORRECT`表示参数错误
*/
int32_t SCC_ReceiveData(uint8_t *data, uint32_t len);

/**
 * @brief 设置波特率
 * @param fidi  波特率，默认值为‘FI_DI_DEFAULT’
 * 
 * @return `SCC_SUCCESS`表示成功，`SCC_PARAM_INCORRECT`表示参数错误
*/
int32_t SCC_SetBpr(uint8_t fidi);

/**
 * @brief 设置发送顺序
 * @param conv      // 0.自动; 1.手动 - 3B; 2.手动 - 3F
 * 
 * @return `SCC_SUCCESS`表示成功，`SCC_PARAM_INCORRECT`表示参数错误
*/
int32_t SCC_SetCONV(uint8_t conv);


#endif	/*__SCC_H__*/

/** @} */
