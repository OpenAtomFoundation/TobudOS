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
 * @brief TIMER 模块接口实现文件。
 * @file n32g020xx_timer.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_timer.h"
#include "n32g020xx_common.h"
#include "n32g020xx.h"
#include <stdbool.h>

//n32g020xx.h中的寄存器名容易混淆,此处采用宏转义
#define ControlStatus  CSR
#define InitialValue   CR
#define CountValue     CVR

#define TIMER_W0_BIT_MSK (~(BIT18|BIT17|BIT15|BIT14|BIT13|BIT3))
#define TIMER_SET_REG_BIT(x,y) SET_REG_VAL((x),GET_REG_VAL((x)) & TIMER_W0_BIT_MSK | (y))
#define TIMER_CLR_REG_BIT(x,y) SET_REG_VAL((x),GET_REG_VAL((x)) & TIMER_W0_BIT_MSK | ~(y))

#define TIMER_MAX 5
TIMERTypeDef * pTimerTypedef[TIMER_MAX]={TIMER0,TIMER1,TIMER2,TIMER3,TIMER4};


//Timer NVIC 中的中断配置
int32_t TIMER_NvicIrqConfig(TimerId id,bool Enable)
{
	if (Enable) {
		NVIC_EnableIRQ((IRQn_Type)((uint32_t)Timer0_Exception_IrqNum + (uint32_t)id));
	}
	else
	{
		NVIC_DisableIRQ((IRQn_Type)((uint32_t)Timer0_Exception_IrqNum + (uint32_t)id));
	}
    return TIMER_SUCCESS;
}

//TIM4 使能wakeup
int32_t TIMER_WakeUpConfig(bool Enable)
{
    if(Enable)
        SET_REG_BIT(PMU->WKSREN1,BIT0);//TIM4可以唤醒深度睡眠
    else
        CLR_REG_BIT(PMU->WKSREN1,BIT0);//TIM4不可以唤醒深度睡眠
    return TIMER_SUCCESS;
}

//TIMER capture IO Pin 配置
int32_t	TIMER_CapturePinConfig(TimerId id)
{
    SET_REG_BIT(CGU->MCGR3,BIT7);
    switch(id)
    {
        case Timer0://GPIO0,TIM0_CH输入
            SET_REG_BIT_MSK(IOM->CR1,BIT1,BIT0|BIT1|BIT2);
            SET_REG_BIT(IOM->GPIOPUA, BIT0);
            CLR_REG_BIT(IOM->GPIOPDA, BIT0);
            break;
        case Timer1://GPIO1,TIM1_CH输入
            SET_REG_BIT_MSK(IOM->CR1,BIT4,BIT3|BIT4|BIT5);
            SET_REG_BIT(IOM->GPIOPUA, BIT1);
            CLR_REG_BIT(IOM->GPIOPDA, BIT1);
            break;
        case Timer2://GPIO2,TIM2_CH输入
            SET_REG_BIT_MSK(IOM->CR2,BIT1,BIT0|BIT1|BIT2);
            SET_REG_BIT(IOM->GPIOPUA, BIT2);
            CLR_REG_BIT(IOM->GPIOPDA, BIT2);
            break;
        case Timer3://GPIO3,TIM3_CH输入
            SET_REG_BIT_MSK(IOM->CR2,BIT4,BIT3|BIT4|BIT5);
            SET_REG_BIT(IOM->GPIOPUA, BIT3);
            CLR_REG_BIT(IOM->GPIOPDA, BIT3);
            break;
        case Timer4://GPIO4,TIM4_CH输入
            SET_REG_BIT_MSK(IOM->CR3,BIT1,BIT0|BIT1|BIT2);
            SET_REG_BIT(IOM->GPIOPUA, BIT4);
            CLR_REG_BIT(IOM->GPIOPDA, BIT4);
            break;
    }
    return TIMER_SUCCESS;
}

//CGU模式中的，TIMER 模块时钟配置
int32_t TIMER_ModuleClkConfig(TimerId id,uint32_t AutoInterOrExcFixValue)
{
    bool InterOsc = 0;
    bool Exc12M = 0;
    if(AutoInterOrExcFixValue == 0)
    {
        InterOsc = 1;
        Exc12M = 0;
    }
    else if(AutoInterOrExcFixValue > 32*1000)
    {
        InterOsc = 0;
        Exc12M = 1;
    }
    const uint32_t TimeIdToBit[TIMER_MAX-1] = {BIT0,BIT1,BIT2,BIT3};
    //TIME时钟选择
    if(InterOsc)
    {//内部时钟
        if(id == Timer4)
        {
            CLR_REG_BIT(CGU->TMRCLKSEL,BIT4);
            CLR_REG_BIT(CGU->TMRCLKSEL,BIT5);
        }
        else
            CLR_REG_BIT(CGU->TMRCLKSEL,TimeIdToBit[id]);
    }
    else if(Exc12M)
    {//12M or 11.8296M
        if(id == Timer4)
        {
            SET_REG_BIT(CGU->TMRCLKSEL,BIT4);
            CLR_REG_BIT(CGU->TMRCLKSEL,BIT5);
        }
        else
            SET_REG_BIT(CGU->TMRCLKSEL,TimeIdToBit[id]);   
    }
    else
    {//32K
        if(id == Timer4)
        {
            SET_REG_BIT(CGU->TMRCLKSEL,BIT5);
        }
        else
            return -1;//只有定时器4可以选择32K时钟
    }
    //定时器模块总线时钟控制寄存器
    SET_REG_BIT(CGU->MCGR5,BIT4);    
    //各定时器时钟使能和分频系数
    switch(id)
    {
        case Timer0:
            SET_REG_VAL(CGU->TMR0CLKCR,0x00);//分步1，[6:0]+1为分频值,此处1分分频
            SET_REG_BIT(CGU->TMR0CLKCR,BIT7);
            break;
        case Timer1:
            SET_REG_VAL(CGU->TMR1CLKCR,0x00);//分步1，[6:0]+1为分频值,此处1分分频
            SET_REG_BIT(CGU->TMR1CLKCR,BIT7);
            break;
        case Timer2:
            SET_REG_VAL(CGU->TMR2CLKCR,0x00);//分步1，[6:0]+1为分频值,此处1分分频
            SET_REG_BIT(CGU->TMR2CLKCR,BIT7);
            break;
        case Timer3:
            SET_REG_VAL(CGU->TMR3CLKCR,0x00);//分步1，[6:0]+1为分频值,此处1分分频
            SET_REG_BIT(CGU->TMR3CLKCR,BIT7);
            break;
        case Timer4:
            SET_REG_VAL(CGU->TMR4CLKCR,0x00);//分步1，[6:0]+1为分频值,此处1分分频
            SET_REG_BIT(CGU->TMR4CLKCR,BIT7);
            break;
    }
    return TIMER_SUCCESS;
}

//定时功能开启，内部低级函数
//指定定时器id,定时us，采用的时钟源，回调函数
//AutoInterOrExcFixValue 为0的时候，自动选择内部高速时钟，如果外部时钟，填入非0值 ，是否唤醒
//唤醒只有Timer4支持，配置错误返回-1
TIMER_TimerCallBack pTimerCall[TIMER_MAX]={0};
static int32_t TIMER_LowStartTimerUs(TimerId id,int32_t us,uint32_t AutoInterOrExcFixValue,TIMER_TimerCallBack pTimerCallBack,bool WakeUp)
{
    if(TIMER_ModuleClkConfig(id,AutoInterOrExcFixValue) == -1)
        return -1;
    if(AutoInterOrExcFixValue == 0)
    {
        AutoInterOrExcFixValue = 20000000;//此处调用CGU模式给出的PCLK时钟,依赖于CGU对Pclk的配置
    }
    TIMER_StopTimer(id);
    pTimerCall[id] = pTimerCallBack;
    int32_t tmpInitialValue = us/1000000.0*AutoInterOrExcFixValue;
    SET_REG_VAL(pTimerTypedef[id]->InitialValue,tmpInitialValue);
    int32_t tmpControlStatus=0;
    CLR_REG_BIT(tmpControlStatus,BIT6);//定时模式
    if(pTimerCallBack)
    {
        SET_REG_BIT(tmpControlStatus,BIT4);//充许定时完成中断请求
    }
    SET_REG_BIT(tmpControlStatus,BIT5);//继续定时模式
    SET_REG_BIT(tmpControlStatus,BIT0);//启动定时
    SET_REG_VAL(pTimerTypedef[id]->ControlStatus,tmpControlStatus);
    if(pTimerCallBack)
    {
        TIMER_NvicIrqConfig(id,true);
    }
    else
    {
        while(!GET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT3))
        {
            ;
        }
        SET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT3);
    }
    return TIMER_SUCCESS;
}

//定时功能开启
//指定定时器id,定时us，采用的时钟源，回调函数
//AutoInterOrExcFixValue 为0的时候，自动选择内部高速时钟，如果外部时钟，填入非0值 
int32_t TIMER_StartTimerUs(TimerId id,int32_t us,uint32_t AutoInterOrExcFixValue,TIMER_TimerCallBack pTimerCallBack)
{
    return TIMER_LowStartTimerUs(id,us,AutoInterOrExcFixValue,pTimerCallBack,false);
}

//关闭定时器
//指定定时器id
int32_t TIMER_StopTimer(TimerId id)
{
    TIMER_NvicIrqConfig(id,false);
    TIMER_SET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT1);
    while(GET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT2) == 0L);
    pTimerCall[id] = 0;
    return TIMER_SUCCESS;
}

//捕获功能
//指定定时器id，捕获模式，捕获成功回调函数
TIMER_CaptureCallBack pCaptureCall[TIMER_MAX]={0};
int32_t TIMER_StartCapture(TimerId id,TIMER_EnumeCapModue mode,TIMER_CaptureCallBack pCaptureCallBack)
{
    if(TIMER_ModuleClkConfig(id,0) == -1)//使用系统PCLK时钟为捕获基准时钟
        return -1;
    TIMER_StopCapture(id);
    pCaptureCall[id] = pCaptureCallBack;
    int32_t tmpControlStatus=0;
    SET_REG_BIT_MSK(tmpControlStatus,mode,BIT9|BIT8|BIT7);//捕获模式选择
    SET_REG_BIT_MSK(tmpControlStatus,0L,BIT12|BIT11|BIT10);//不滤除毛刺 
    SET_REG_BIT(tmpControlStatus,BIT21);//捕获触发中断
    CLR_REG_BIT(tmpControlStatus,BIT20);//捕获源为External_Signal
    SET_REG_BIT(tmpControlStatus,BIT5);//连续工作模式
    SET_REG_BIT(tmpControlStatus,BIT6);//Timer为捕获模式
    SET_REG_BIT(tmpControlStatus,BIT19);//捕获使能,RO
    SET_REG_BIT(tmpControlStatus,BIT4);//充许定时完成中断
    SET_REG_VAL(pTimerTypedef[id]->ControlStatus,tmpControlStatus);
    //*(uint32_t *)(0x50300020)  = 0x00280060;
    TIMER_CapturePinConfig(id);
    TIMER_NvicIrqConfig(id,true);
    return TIMER_SUCCESS;
}

//关闭捕获功能
//指定定时器id
int32_t TIMER_StopCapture(TimerId id)
{
    TIMER_CLR_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT19);//捕获关闭
    pCaptureCall[id] = 0;//同时清空
    return TIMER_SUCCESS;
}

//读取计数器的值
int32_t TIMER_GetCountValue(TimerId id,uint32_t *Val)
{   
    uint32_t tmpCountValue;
    {
        while(GET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT18));
        SET_REG_VAL(pTimerTypedef[id]->CountValue,0xffffffff);
        while((!GET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT17)));
    }
    TIMER_SET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT18);
    TIMER_SET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT17);
    
    tmpCountValue = GET_REG_VAL(pTimerTypedef[id]->CountValue);
    *Val = tmpCountValue;
    return TIMER_SUCCESS;
}

//内部使用回调函数
static int32_t TIMER_WakeUpTimerCallBack(TimerId id)
{
    TIMER_StopTimer(Timer4);
    return TIMER_SUCCESS;
}

//Timer4 StandBy WakeUp 中断配置
int32_t TIMER_PMU_NvicIrqConfig(bool Enable)
{
    uint32_t *pNVIC_ISER = (uint32_t *)(0xE000E000+0x100);    
    if(Enable)
        SET_REG_BIT(*pNVIC_ISER,BIT10);
    else
        CLR_REG_BIT(*pNVIC_ISER,BIT10);
    return TIMER_SUCCESS;
}

//定时器4 StandBy Wakeup配置
static void TIMER_EnterStandByByTime4WakeUp(bool StandBy,bool IrqStyle)
{
    if(IrqStyle)
    { 
        //配置PMU INT
        TIMER_PMU_NvicIrqConfig(true);
        SET_REG_BIT(PMU->WKSREN0,BIT7); 
        //Deep Idle
        if(StandBy)
            SET_REG_BIT(ARM->SCR,BIT2);
        else
            CLR_REG_BIT(ARM->SCR,BIT2);  
        __WFI();
    }
    else
    {    
        //Deep Idle
        __SEV();
        if(StandBy)
            SET_REG_BIT(ARM->SCR,BIT2);
        else
            CLR_REG_BIT(ARM->SCR,BIT2);  
        __WFE();
        __WFE();
    }
}

int32_t TIMER_WakeUpUsIntial(void)
{
    //开PMU RETPMU pclk
    SET_REG_BIT(CGU->MCGR4, BIT5|BIT4);
    //初始化RC32K Trim值
    SET_REG_VAL(RETPMU->RC32K_CTRL1,GET_REG_VAL(SMC->RC32KCFG0));
    //唤醒源Timer4
    SET_REG_BIT(PMU->WKSREN1,BIT0); 
    return TIMER_SUCCESS;
}

//唤醒使能
int32_t TIMER_StartWakeUpUs(int32_t us,bool StandBy,bool IrqStyle)
{
    int32_t  AutoInterOrExcFixValue = 32000;
    TimerId id = Timer4;
    if(TIMER_ModuleClkConfig(id,AutoInterOrExcFixValue) == -1)
        return -1;
    TIMER_StopTimer(id);
    int32_t tmpInitialValue = us/1000000.0*AutoInterOrExcFixValue;
    SET_REG_VAL(pTimerTypedef[id]->InitialValue,tmpInitialValue);
    int32_t tmpControlStatus=0;
    CLR_REG_BIT(tmpControlStatus,BIT6);//定时模式
    SET_REG_BIT(tmpControlStatus,BIT16);//启动wakeup
    SET_REG_BIT(tmpControlStatus,BIT0);//启动定时
    if(!StandBy)
    {
        pTimerCall[id] = TIMER_WakeUpTimerCallBack;
        SET_REG_BIT(tmpControlStatus,BIT4);//充许定时完成中断请求
    }
    TIMER_NvicIrqConfig(id,true);
    SET_REG_VAL(pTimerTypedef[id]->ControlStatus,tmpControlStatus);
    TIMER_EnterStandByByTime4WakeUp(StandBy,IrqStyle);
    return TIMER_SUCCESS;
}


//中断处理函数，由各中断向量函数调用,统一入口
//指定定时器id
void TIMER_IrqDeal(TimerId id)
{
    //捕获成功
    if(GET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT13))
    {
        TIMER_SET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT13);
        //则标识获取预期宽度成功
        if(GET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT3))
        {
            TIMER_SET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT3);
            if(pCaptureCall[id])
            {
                pCaptureCall[id](id,0);
            }
        }
        else
        {
            if(pCaptureCall[id])
            {
                uint32_t Val;
                TIMER_GetCountValue(id,&Val);
                pCaptureCall[id](id,Val);
            }
        }
    }
    else
    {
        //则标识本次定时到
        if(GET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT3))
        {
            TIMER_SET_REG_BIT(pTimerTypedef[id]->ControlStatus,BIT3);
            if(pTimerCall[id])
            {
                pTimerCall[id](id);
            }
        }
    }
}

//Timer0中断入口用
void TIM0_IRQHandler(void)
{
    TIMER_IrqDeal(Timer0);
}

//Timer1中断入口用
void TIM1_IRQHandler(void)
{
    TIMER_IrqDeal(Timer1);
}

//Timer2中断入口用
void TIM2_IRQHandler(void)
{
    TIMER_IrqDeal(Timer2);
}

//Timer3中断入口用
void TIM3_IRQHandler(void)
{
    TIMER_IrqDeal(Timer3);
}

//Timer4中断入口用
void TIM4_IRQHandler(void)
{
    TIMER_IrqDeal(Timer4);
}
