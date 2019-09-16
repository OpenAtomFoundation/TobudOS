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
 * @brief RTC 模块接口实现文件。
 * @file n32g020xx_rtc.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_rtc.h"
#include "n32g020xx_common.h"
#include "n32g020xx.h"
#include <stdbool.h>

typedef enum{
    Freq1Hz=0,
    Freq2Hz,
}RTC_IrqFreq;

//RTC NVIC 配置
//Enable: true or false
int32_t RTC_NvicIrqConfig(bool Enable)
{
    uint32_t *pNVIC_ISER = (uint32_t *)(0xE000E000+0x100);
    if(Enable)
        SET_REG_BIT(*pNVIC_ISER,BIT26);
    else
        CLR_REG_BIT(*pNVIC_ISER,BIT26);
    return RTC_SUCCESS;
}

//RTC CGU模式中的，模块时钟配置
int32_t RTC_ModuleClkConfig(void)
{
    //RETPMU pClk使能
    SET_REG_BIT(CGU->MCGR4,BIT4);
    return RTC_SUCCESS;
}

//RTC 使能wakeup
//Enable:true or false
int32_t RTC_WakeUpConfig(bool Enable)
{
    if(Enable)
        SET_REG_BIT(PMU->WKSREN1,BIT6);//RTC可以唤醒深度睡眠
    else
        CLR_REG_BIT(PMU->WKSREN1,BIT6);//RTC不可以唤醒深度睡眠
    return RTC_SUCCESS;
}


//同步完成处理函数,内部调用
static int32_t RTC_WaitAndClearSync(void)
{
    #if 1
    while(1)
    {
        if(GET_REG_BIT(RTC->INTSTS,BIT1))
        {
            SET_REG_VAL(RTC->INTSTS,BIT1);
            break;
        }
    }
    #endif
    return RTC_SUCCESS;
}

//读频率调整寄存器
//读取Adj中的Latch的值
int32_t RTC_GetAdjFreq(uint16_t *Val)
{
    uint16_t tmpAdj = 0;
    tmpAdj = GET_REG_VAL(RTC->ADJ);
    tmpAdj = (GET_REG_VAL(RTC->ADJ)<<8)|tmpAdj;
    *Val = tmpAdj;
	return RTC_SUCCESS;
}

//频率调整寄存器，该的配置 频率调整寄存器
//输入PpmVal直接填PPM值，范围为±1024ppm，内部会转换
//内部配置范围是 16’h77FF-16’h87FF；寄存器时实际读取的值是 寄存器时实际读取的值是 Latch中的值
int32_t RTC_SetJudjeFreq(int16_t PpmVal)
{   
    uint16_t WriteAdj = 0x7FFF + PpmVal*2;
    uint16_t tmpReadAdj = 0;
    SET_REG_VAL(RTC->ADJ,WriteAdj & 0xff);
    SET_REG_VAL(RTC->ADJ,WriteAdj >> 8);
    RTC_WaitAndClearSync();
    RTC_GetAdjFreq(&tmpReadAdj);
    if(WriteAdj != tmpReadAdj)
        return RTC_FAIL;
    return RTC_SUCCESS;
}

//读RTC计数器的值
int32_t RTC_GetDrData(int32_t *Val)
{
    //写任意值
    SET_REG_BIT(RTC->DRUPDATE,BIT0);
    RTC_WaitAndClearSync();
    //读DR
	uint32_t tmpDr = 0;    
    tmpDr = (GET_REG_VAL(RTC->DR)<<0)|tmpDr;
    tmpDr = (GET_REG_VAL(RTC->DR)<<8)|tmpDr;
    tmpDr = (GET_REG_VAL(RTC->DR)<<16)|tmpDr;
    tmpDr = (GET_REG_VAL(RTC->DR)<<24)|tmpDr;
    *Val = tmpDr;
	return RTC_SUCCESS;
}

//启动RTC,可以选择具有WakeUp功能
//如果不需要回调函数可传入0
RTC_RtcCallBack mRTC_RtcCallBack = 0;
int32_t RTC_StartRtc(bool WakeUp,RTC_RtcCallBack pRtcCallBack)
{
    SET_REG_VAL(RTC->INTSTSEN,0);
    RTC_ModuleClkConfig();
    RTC_WakeUpConfig(WakeUp);
    
    SET_REG_BIT(RTC->INTSTSEN,BIT0);//使能中断1Hz
    mRTC_RtcCallBack = pRtcCallBack;
    SET_REG_VAL(RTC->START,BIT0);//开始计数   
    RTC_NvicIrqConfig(true); 
    RTC_WaitAndClearSync();
    return RTC_SUCCESS;
}

//停止RTC
int32_t RTC_StopTimer(void)
{
    SET_REG_VAL(RTC->INTSTSEN,0);
    mRTC_RtcCallBack = 0;
    return RTC_SUCCESS;
}


//RTC 中断处理函数
void RTC_IRQHandler(void)
{
    //1Hz中断完成
    if(GET_REG_BIT(RTC->INTSTS,BIT0))
    {
        SET_REG_VAL(RTC->INTSTS,BIT0);//注，写1清0
        if(mRTC_RtcCallBack)
        {
            mRTC_RtcCallBack();
        }
    }
    //2Hz中断完成
    if(GET_REG_BIT(RTC->INTSTS,BIT2))
    {
        SET_REG_VAL(RTC->INTSTS,BIT2);
    }
}

