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
 * @brief ADC 模块接口实现文件。
 * @file n32g020xx_adc.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_adc.h"
#include "n32g020xx_dac.h"
#include "n32g020xx_common.h"
#include "n32g020xx.h"
#include <stdbool.h>

ADCTypeDef *pADC = ADC;

/*
 * 通道5有问题，通道0-4返回补码，输入电压==0时，读到的数值会在0x0FFF和0x0023范围内跳动
 * 输入电压>=参考电压时，读到的值是0x08000
 * 通道6到10在量程范围内，读到的值是0x08000到0x0FFF
 */
 
//ADC Nvic中断配置
int32_t ADC_NvicIrqConfig(bool Enable)
{
    if(Enable) {
		NVIC_EnableIRQ(Adc_Dac_Exception_IrqNum);
	} else {
		NVIC_DisableIRQ(Adc_Dac_Exception_IrqNum);
	}
    return ADC_SUCCESS;
}

//CGU模式中的，ADC模块时钟配置
//AdcDiv最多低14位有效
int32_t ADC_ModuleClkConfig(uint32_t AutoInterOrExcFixValue,uint16_t AdcDiv)
{
    //ADC模块总线时钟控制寄存器
    SET_REG_BIT(CGU->MCGR5,BIT3);   
    //配置ADC输入时钟和相应的时钟分频
    AdcDiv &= 0x3FFF;
    SET_REG_VAL(CGU->ADCCLKFDR0,AdcDiv&0x00FF);
    uint8_t tmpADCCLKFDR1 = AdcDiv>>8;
    if(AutoInterOrExcFixValue == 0)
    {
        CLR_REG_BIT(tmpADCCLKFDR1,BIT6); 
    }
    else 
    {
        SET_REG_BIT(tmpADCCLKFDR1,BIT6); 
    }
    SET_REG_VAL(CGU->ADCCLKFDR1,tmpADCCLKFDR1); 
    //ADC时钟DIV检查和使能
    while(!GET_REG_BIT(CGU->ADCCLKCR,BIT1))
        ;
    SET_REG_BIT(CGU->ADCCLKCR,BIT0);
    return ADC_SUCCESS;
}

//开启ADC转换
ADC_CallBack mADC_CallBack;
int32_t ADC_Start(void)
{
    if(mADC_CallBack != 0)
    {
        SET_REG_VAL(ADC->IER,BIT8 | BIT0);
        ADC_NvicIrqConfig(true);
    }
    //ADCDAC共享中断ADC的标记位使能
    SET_REG_BIT(ADCDAC->INTSETENABLE,BIT0);
    //开启转换
    SET_REG_BIT(ADC->CR1,BIT19);
    return ADC_SUCCESS;
}

//停止转换函数
int32_t ADC_Stop(bool ClrIrqCallBack)
{
    //停止转换
    CLR_REG_BIT(ADC->CR1,BIT19);
    //关闭ADC模块中断
    ADC_NvicIrqConfig(false);
    SET_REG_VAL(ADC->IER,0);
    //ADCDAC共享中断ADC的标记位失能
    SET_REG_BIT(ADCDAC->INTCLRENABLE,BIT0);
    if(ClrIrqCallBack)
        mADC_CallBack = 0;    
    return ADC_SUCCESS;
}

//ADC IO配置
int32_t	ADC_PinConfig(uint16_t mADCCh)
{
    SET_REG_BIT(CGU->MCGR3,BIT7);
    for(uint8_t i = 0;i<16;i++)
    {
        switch(mADCCh & (0x0001<<i))
        {
            case ADC_EN_SINGLE_CH0://GPIO6
                SET_REG_BIT_MSK(IOM->CR4,BIT0|BIT1|BIT2,BIT0|BIT1|BIT2);
                break;
            case ADC_EN_SINGLE_CH1://GPIO7
                SET_REG_BIT_MSK(IOM->CR4,BIT3|BIT4|BIT5,BIT3|BIT4|BIT5);
                break;
            case ADC_EN_SINGLE_CH2://GPIO8
                SET_REG_BIT_MSK(IOM->CR5,BIT0|BIT1|BIT2,BIT0|BIT1|BIT2);
                break;
            case ADC_EN_SINGLE_CH3://GPIO9
                SET_REG_BIT_MSK(IOM->CR5,BIT3|BIT4|BIT5,BIT3|BIT4|BIT5);
                break;
            case ADC_EN_SINGLE_CH4://GPIO10
                SET_REG_BIT_MSK(IOM->CR6,BIT0|BIT1|BIT2,BIT0|BIT1|BIT2);
                break;
            case ADC_EN_SINGLE_CH5://GPIO11
                SET_REG_BIT_MSK(IOM->CR6,BIT3|BIT4|BIT5,BIT3|BIT4|BIT5);
                break;
            case ADC_EN_SINGLE_CH6://GPIO12
                SET_REG_BIT_MSK(IOM->CR7,BIT0|BIT1|BIT2,BIT0|BIT1|BIT2);
                break;
            case ADC_EN_SINGLE_CH7://GPIO13
                SET_REG_BIT_MSK(IOM->CR7,BIT3|BIT4|BIT5,BIT3|BIT4|BIT5);
                break;
            case ADC_EN_SINGLE_CH8://GPIO24
                SET_REG_BIT_MSK(IOM->CR13,BIT0|BIT1|BIT2,BIT0|BIT1|BIT2);
                break;
            case ADC_EN_SINGLE_CH9://GPIO25
                SET_REG_BIT_MSK(IOM->CR13,BIT3|BIT4|BIT5,BIT3|BIT4|BIT5);
                break;
        }
    }
    return ADC_SUCCESS;
}


/*
 * 修改FifoWidthSet 设置，需要先关闭ADC 使能后修改
 * 单通道单次采样，即采样一次后，自动关闭AdcSampleEn
 * 单通道循环采样，即AdcSampleEn 使能后，循环采样，直到软件关闭AdcSampleEn使能
 * 多通道单次采样，即每个选中通道采样1 次后，自动关闭AdcSampleEn
 * 多通道循环采样，即每个选中通道采样1 次后，自动从选中的第一个通道开始采样至最后一个选中的通道，直到软件关闭AdcSampleEn 使能
 */
int32_t ADC_IntialConfig(AdcParamInitial *pAdcParamInitial)
{
    ADC_ModuleClkConfig(0,pAdcParamInitial->AdcDiv+2);

	/* 停止ADC采样、ADC中断禁能、清除中断状态标志 */
    ADC_Stop(true);
    SET_REG_BIT(ADC->CR1,BIT18);
    uint32_t tmpCR1 = GET_REG_VAL(ADC->CR1);
    
    SET_REG_BIT_MSK(tmpCR1,pAdcParamInitial->Chs,0xFFFF);
    CLR_REG_BIT(tmpCR1,BIT16);//tmpAdcCr1.Bits.FifoWidthSet = 0;//0:每fifo地址存1个数据，1：每fifo存2个数据
    CLR_REG_BIT(tmpCR1,BIT17);//0:Fifo中存储原码，1:Fifo中存储补码
    if(pAdcParamInitial->InnerRef)
    {
        SET_REG_BIT(tmpCR1,BIT20);
    }
    else
    {
        SET_REG_BIT_MSK(IOM->CR15,BIT0|BIT1|BIT2,BIT0|BIT1|BIT2);//VREF+
        SET_REG_BIT_MSK(IOM->CR15,BIT3|BIT4|BIT5,BIT3|BIT4|BIT5);//VREF-//设置了VREF-，VREF-会自动转为VREF-
        CLR_REG_BIT(tmpCR1,BIT20);
    }
    if(pAdcParamInitial->LoopSampleEn)
        SET_REG_BIT(tmpCR1,BIT21);
    else
        CLR_REG_BIT(tmpCR1,BIT21);
    SET_REG_BIT(tmpCR1,BIT30);//硬件自动控制Da_reg[3:2]
    
    SET_REG_VAL(ADC->CR1,tmpCR1);
    
    mADC_CallBack = pAdcParamInitial->pADC_CallBack;
    
    ADC_PinConfig(pAdcParamInitial->Chs);
    return 0;
}

//读ADC值
int32_t ADC_ReadValue(uint8_t *ChNum,uint16_t *AdVal)
{
    if(GET_REG_BIT(ADC->ISR,BIT0))
    {
        uint16_t tmpFifo = GET_REG_VAL(ADC->FIFO);
        *ChNum = (tmpFifo>>12 & 0x000F);
        *AdVal = tmpFifo&0x0FFF;
        if(*ChNum < 6)
        {
            if(*AdVal  < 0x0800)
                *AdVal = 0x0FFF;
            *AdVal = 0xFFF - *AdVal;
        }
        else// if(*ChNum < 10)
        {
            if(*AdVal < 0x0800)
                *AdVal = 0x0800;
            *AdVal -= 0x0800;
        }
        return ADC_ONE_VALUE;
    }
    else
        return ADC_NONE_VALUE;
}

//DMA设置设置
int32_t ADC_SetDmaCfg(uint32_t Flags)
{
    SET_REG_VAL(ADC->DMACR,Flags);
    return ADC_SUCCESS;
}

//ADC中断处理函数
void ADAC_IRQHandlerAdc(void)
{
    if(mADC_CallBack)
    {
        while(GET_REG_BIT(ADC->ISR , BIT0))
        {
            uint8_t ChNum;
            uint16_t AdVal;
            if(ADC_ReadValue(&ChNum,&AdVal) == 1)
            {
                mADC_CallBack(ChNum,AdVal);
            }
        }
    }
    SET_REG_BIT(ADC->IFR,0xFF);
    //ADCDAC共享中断标记对ADC的标记位清0
    SET_REG_BIT(ADCDAC->INTCLRSTATUS,BIT0);
}

__weak void ADAC_IRQHandlerDac(void)
{
}

/* ADC模块和DAC模块中断总入口函数,通过中断状态判断产生源模块 */
void ADAC_IRQHandler(void)
{
	/* ADC模块中断处理 */
	if (ADCDAC->INTSTATUS & BIT0)
	{
		ADAC_IRQHandlerAdc();
	}

	/* DAC模块中断处理 */
	if (ADCDAC->INTSTATUS & BIT1)
	{
		ADAC_IRQHandlerDac();
	}
}
