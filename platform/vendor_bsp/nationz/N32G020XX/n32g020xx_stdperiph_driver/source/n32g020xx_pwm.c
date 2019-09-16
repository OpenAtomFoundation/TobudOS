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
 * @brief PWM 模块接口实现文件。
 * @file n32g020xx_pwm.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_pwm.h"
#include "n32g020xx.h"
#include <stdbool.h>

#define PWM_CH_MAX                  8
PWMGLBTypeDef *pPwmGlb = PWMGLB;
PWMTypeDef *pPwmCh[PWM_CH_MAX] = {PWM0,PWM1,PWM2,PWM3,PWM4,PWM5,PWM6,PWM7};


//PWM Nvic中断配置函数
int32_t PWM_NvicIrqConfig(bool Enable)
{
    if(Enable)
		NVIC_EnableIRQ(Pwm_Exception_IrqNum);
    else
		NVIC_DisableIRQ(Pwm_Exception_IrqNum);
    return PWM_SUCCESS;
}

//CGU模式中的，PWM模块时钟配置
int32_t PWM_ModuleClkConfig(uint32_t AutoInterOrExcFixValue)
{
    static uint8_t IntialFlag = 0;
    if(IntialFlag == 0)
    {
        IntialFlag = 1;
        //PWM模块总线时钟控制寄存器
        SET_REG_BIT(CGU->MCGR2,BIT1);   
        //配置PWM相应的时钟分频
        if(AutoInterOrExcFixValue == 0)
        {
            CLR_REG_BIT(CGU->PWMRCVCLKCR,BIT0); 
        }
        else 
        {
            SET_REG_BIT(CGU->PWMRCVCLKCR,BIT0); 
        }
        //配置PWM输入时钟使能
        SET_REG_BIT(CGU->PWMRCVCLKCR,BIT1); 
    }
    return PWM_SUCCESS;
}

//PWM模块IO Pin配置
int32_t	PWM_PinConfig(uint8_t ch)
{
    //IO PCLK
    SET_REG_BIT(CGU->MCGR3,BIT7);
    switch(ch)
    {
        case 0://GPIO0
            SET_REG_BIT_MSK(IOM->CR1,BIT0,BIT0|BIT1|BIT2);
            break;
        case 1://GPIO1
            SET_REG_BIT_MSK(IOM->CR1,BIT3,BIT3|BIT4|BIT5);
            break;
        case 2://GPIO2
            SET_REG_BIT_MSK(IOM->CR2,BIT0,BIT0|BIT1|BIT2);
            break;
        case 3://GPIO3
            SET_REG_BIT_MSK(IOM->CR2,BIT3,BIT3|BIT4|BIT5);
            break;
        case 4://GPIO4
            SET_REG_BIT_MSK(IOM->CR3,BIT0,BIT0|BIT1|BIT2);
            break;
        case 5://GPIO5
            SET_REG_BIT_MSK(IOM->CR3,BIT3,BIT3|BIT4|BIT5);
            break;
        case 6://GPIO6
            SET_REG_BIT_MSK(IOM->CR4,BIT0,BIT0|BIT1|BIT2);
            break;
        case 7://GPIO7
            SET_REG_BIT_MSK(IOM->CR4,BIT3,BIT3|BIT4|BIT5);
            break;
    }
    return PWM_SUCCESS;
}

//更变相应通道占空比
int32_t PWM_ChangePwmDutyCycle(uint8_t ch,uint16_t DutyCycle)
{
    if(GET_REG_BIT(pPwmCh[ch]->CR,BIT3))//连续，固定模式
        SET_REG_VAL(pPwmCh[ch]->MRINI0,((uint32_t)DutyCycle<<16)+1000);
    else//USER模式，中断回调
        SET_REG_VAL(pPwmCh[ch]->DR,((uint32_t)DutyCycle<<16)+1000);
    return PWM_SUCCESS;
}

//PWM连续输出配置
//ch取值范围为0--7，DutyCycle 放大了1000倍
PWM_CallBack mPWM_CallBack[PWM_CH_MAX] = {0};
int32_t PWM_ConfigContinue(uint8_t ch,bool PolHigh,uint16_t DutyCycle)
{
    //时钟配置
    PWM_ModuleClkConfig(0);
    //引脚配置
    PWM_PinConfig(ch);
    //清零
    SET_REG_BIT(pPwmCh[ch]->CR,BIT1);
    CLR_REG_BIT(pPwmCh[ch]->CR,BIT1);
    SET_REG_BIT(pPwmCh[ch]->CR,BIT7);
    CLR_REG_BIT(pPwmCh[ch]->CR,BIT7);
    //配置
    uint32_t tmpCR = 0;
    CLR_REG_BIT(tmpCR,BIT14);
    CLR_REG_BIT(tmpCR,BIT13);
    if(PolHigh)
        SET_REG_BIT(tmpCR,BIT12);
    else
        CLR_REG_BIT(tmpCR,BIT12);
    CLR_REG_BIT(tmpCR,BIT11);
    CLR_REG_BIT(tmpCR,BIT10);
    CLR_REG_BIT(tmpCR,BIT9);
    CLR_REG_BIT(tmpCR,BIT8);
    SET_REG_BIT_MSK(tmpCR,0,BIT6|BIT5);//固定为0
    SET_REG_BIT(tmpCR,BIT4);//正常模式
    SET_REG_BIT(tmpCR,BIT3);//固定模式
    CLR_REG_BIT(tmpCR,BIT2);
    
    SET_REG_VAL(pPwmCh[ch]->CR,tmpCR);
    SET_REG_VAL(pPwmCh[ch]->MRINI0,((uint32_t)DutyCycle<<16)+1000);
    
    return PWM_SUCCESS;
}

//PWM中断输出占空比配置
//ch取值范围为0--7，DutyCycle 放大了1000倍，带中断回调函数
int32_t PWM_ConfigIrq(uint8_t ch,bool PolHigh,uint16_t DutyCycle,PWM_CallBack pPWM_CallBack)
{
    mPWM_CallBack[ch] = pPWM_CallBack;
    
    //时钟配置
    PWM_ModuleClkConfig(0);
    //引脚配置
    PWM_PinConfig(ch);
    //清零
    SET_REG_BIT(pPwmCh[ch]->CR,BIT1);
    CLR_REG_BIT(pPwmCh[ch]->CR,BIT1);
    SET_REG_BIT(pPwmCh[ch]->CR,BIT7);
    CLR_REG_BIT(pPwmCh[ch]->CR,BIT7);
    //配置
    uint32_t tmpCR = 0;
    CLR_REG_BIT(tmpCR,BIT14);
    CLR_REG_BIT(tmpCR,BIT13);
    if(PolHigh)
        SET_REG_BIT(tmpCR,BIT12);
    else
        CLR_REG_BIT(tmpCR,BIT12);
    CLR_REG_BIT(tmpCR,BIT11);
    CLR_REG_BIT(tmpCR,BIT10);
    CLR_REG_BIT(tmpCR,BIT9);
    CLR_REG_BIT(tmpCR,BIT8);
    SET_REG_BIT_MSK(tmpCR,0,BIT6|BIT5);//固定为0
    SET_REG_BIT(tmpCR,BIT4);//正常模式
    CLR_REG_BIT(tmpCR,BIT3);//非固定模式
    CLR_REG_BIT(tmpCR,BIT2);
    SET_REG_VAL(pPwmCh[ch]->CR,tmpCR);
    //中断配置
    SET_REG_BIT(pPwmCh[ch]->IER,BIT8);//模块总中断开
    SET_REG_BIT(pPwmCh[ch]->IER,BIT4);//空中断，确保当前发送的值是准备好的下一个值
    SET_REG_BIT(pPwmGlb->INTENSET,BIT0<<ch);//总中断使能置位
    //数据配置
    SET_REG_VAL(pPwmCh[ch]->DR,((uint32_t)DutyCycle<<16)+1000);
    return PWM_SUCCESS;
}

//开启PWM
//ChBitS为通道的组合
int32_t PWM_Start(uint8_t ChBitS)
{
    uint32_t tmpGen = GET_REG_VAL(pPwmGlb->GEN_CR);
    tmpGen |= ChBitS;
    SET_REG_VAL(pPwmGlb->GEN_CR,tmpGen);
    //NVIC中断开
    if(GET_REG_VAL(pPwmGlb->INTEN))
        PWM_NvicIrqConfig(true);
    return PWM_SUCCESS;
}

//关闭PWM
//ChBitS为通道的组合
int32_t PWM_Stop(uint8_t ChBitS)
{
    uint32_t tmpGen = GET_REG_VAL(pPwmGlb->GEN_CR);
    tmpGen &= (~ChBitS);
    SET_REG_VAL(pPwmGlb->GEN_CR,tmpGen);
    //中断复位
    for(uint8_t ch=0;ch < PWM_CH_MAX;ch++)
    {
        if((0x01<<ch) & ChBitS)
        {
            SET_REG_VAL(pPwmCh[ch]->IER,0);
            SET_REG_BIT(pPwmGlb->INTENCLR,BIT0<<ch);
        }
    }
    //NVIC中断关
    if(!GET_REG_VAL(pPwmGlb->INTEN))
        PWM_NvicIrqConfig(false);
    return PWM_SUCCESS;
}

//PWW刹车配置
//ChBitS为通道的组合，BreakEn为使能，BreakInLogic为输入逻辑电平，BreakOutLogic为输出逻辑电平
int32_t PWM_BreakConfig(uint8_t ChBitS,bool BreakEn,bool BreakInLogic,bool BreakOutLogic)
{
    uint8_t ch = 0;
    for(;ch < PWM_CH_MAX;ch++)
    {
        if((0x01<<ch) & ChBitS)
        {
            SET_REG_VAL(pPwmCh[ch]->ENSTOPCR,BreakEn|(BreakInLogic<<1)|(BreakOutLogic<<2));
        }
    }
    return PWM_SUCCESS;
}

//PWW刹车IO配置
//Pin为 PWM_EnumBrakIO 某一个枚举值
int32_t	PWM_BreakPIn(PWM_EnumBrakIO Pin)
{
    SET_REG_BIT(CGU->MCGR3,BIT7);
    SET_REG_VAL(IOM->PWMEMSTP,Pin);
	switch(Pin)
	{
		case PWM_GPIO14:
			SET_REG_BIT_MSK(IOM->CR8,0,BIT0|BIT1|BIT2);
            CLR_REG_BIT(IOM->GPIOOEB, BIT6);
            SET_REG_BIT(IOM->GPIOIEB, BIT6);
			break;
		case PWM_GPIO15:
			SET_REG_BIT_MSK(IOM->CR8,0,BIT3|BIT4|BIT5);
            CLR_REG_BIT(IOM->GPIOOEB, BIT7);
			SET_REG_BIT(IOM->GPIOIEB, BIT7);
			break;
		case PWM_GPIO16:
			SET_REG_BIT_MSK(IOM->CR9,0,BIT0|BIT1|BIT2);
            CLR_REG_BIT(IOM->GPIOOEC, BIT0);
			SET_REG_BIT(IOM->GPIOIEC, BIT0);
			break;
		case PWM_GPIO17:
			SET_REG_BIT_MSK(IOM->CR9,0,BIT3|BIT4|BIT5);
            CLR_REG_BIT(IOM->GPIOOEC, BIT1);
			SET_REG_BIT(IOM->GPIOIEC, BIT1);
			break;
		case PWM_GPIO18:
			SET_REG_BIT_MSK(IOM->CR10,0,BIT0|BIT1|BIT2);
            CLR_REG_BIT(IOM->GPIOOEC, BIT2);
			SET_REG_BIT(IOM->GPIOIEC, BIT2);
			break;
		case PWM_GPIO20:
			SET_REG_BIT_MSK(IOM->CR11,0,BIT0|BIT1|BIT2);
            CLR_REG_BIT(IOM->GPIOOEC, BIT4);
			SET_REG_BIT(IOM->GPIOIEC, BIT4);
			break;		
		case PWM_GPIO21:
			SET_REG_BIT_MSK(IOM->CR11,0,BIT3|BIT4|BIT5);
            CLR_REG_BIT(IOM->GPIOOEC, BIT5);
			SET_REG_BIT(IOM->GPIOIEC, BIT5);
			break;
		case PWM_GPIO22:
			SET_REG_BIT_MSK(IOM->CR12,0,BIT0|BIT1|BIT2);
            CLR_REG_BIT(IOM->GPIOOEC, BIT6);
			SET_REG_BIT(IOM->GPIOIEC, BIT6);
			break;
		case PWM_GPIO23:
			SET_REG_BIT_MSK(IOM->CR12,0,BIT3|BIT4|BIT5);
            CLR_REG_BIT(IOM->GPIOOEC, BIT7);
			SET_REG_BIT(IOM->GPIOIEC, BIT7);
			break;
	}
    return PWM_SUCCESS;
}

//中断处理函数
void PWM_IRQHandler(void)
{
	uint8_t ChStatus;
    //PWM全局中断处理
	ChStatus = GET_REG_VAL(pPwmGlb->INTSTATUS);
	SET_REG_VAL(pPwmGlb->INTSTATUSCLR,ChStatus);
    for(uint8_t ch = 0;ch < 8;ch++)
    {
        if((0x01<<ch) & ChStatus)
        {
            //PWM各通道中断处理
            uint8_t sr = GET_REG_VAL(pPwmCh[ch]->IFR);
            SET_REG_VAL(pPwmCh[ch]->IFR,sr);
            uint16_t DutyCycle;
            if(mPWM_CallBack[ch])
            {
                if(mPWM_CallBack[ch](ch,&DutyCycle) == 1)
                    SET_REG_VAL(pPwmCh[ch]->DR,((uint32_t)DutyCycle<<16)+1000);
            }
        }
    }
}
