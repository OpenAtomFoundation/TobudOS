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
 * @file n32g020xx_dma.c
 * @author Nations Solution Team
 * @date     2018-12-29 14:43:20
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @brief    DMA控制模块接口实现文件
 */

#include <stdint.h>
#include <stdbool.h>
#include "n32g020xx.h"
#include "n32g020xx_common.h"
#include "n32g020xx_dma.h"
#include "n32g020xx_cgu.h"

/** @brief  DMA 通道状态使能标志位 */
#define DMA_CHSTS_EN_M		 BIT0
/** @brief  DMA 通道状态忙标志位 */
#define DMA_CHSTS_BUSY_M 	 BIT1
/** @brief  DMA 通道状态仲裁标志位 */
#define DMA_CHSTS_GRANT_M    BIT2

/** @brief  DMA 总控制使能位 */
#define DMA_CTRL_EN_M		 BIT0
/** @brief  DMA RAM清除中断使能位 */
#define DMA_INTRAMCLRMASK_RAMCLR_M	BIT0


static bool sDMAInitStatus = false;
static DMAIntCallback sDMAChannelCmpltCallbck[DMAC_MAX_CHANNEL] = {0};
static DMAIntCallback sDMAChannelHFCmpltCallbck[DMAC_MAX_CHANNEL] = {0};



int32_t DMA_Init(DMAConfig * pDmaCfg)
{
	if (!pDmaCfg)
	{
		return DMA_PARAM_INCORRECT;
	}

	if(!sDMAInitStatus)
	{
		CGU_EnableModuleFunctions(CGUM_DMA_HCLK);
		SET_REG_VAL(DMA->SOFTRESET,1);
		
		NVIC_ClearPendingIRQ(Dmac_Exception_IrqNum);
		NVIC_EnableIRQ((Dmac_Exception_IrqNum));

		SET_REG_VAL(DMA->INTSTATUS, 0xFFFFFFFF);
		SET_REG_VAL(DMA->INTMASK, pDmaCfg->irqmask);
		CLR_REG_BIT(DMA->INTRAMCLRMASK, DMA_INTRAMCLRMASK_RAMCLR_M);
		sDMAInitStatus = true;
	}

	return DMA_SUCCESS;
}


int32_t DMA_DeInit(void)
{
	CGU_DisableModuleFunctions(CGUM_DMA_HCLK);
	sDMAInitStatus = false;

	return DMA_SUCCESS;
}

/**
 * @brief DMA中断服务函数
 * 
 */
void DMAC_IRQHandler(void)
{
	uint32_t dmaintstatus ;
    uint32_t ramclrstatus ;
	uint32_t channel;	
	uint32_t intstatus ;

	ramclrstatus = GET_REG_VAL(DMA->INTRAMCLRSTATUS);
	dmaintstatus = GET_REG_VAL(DMA->INTSTATUS);

	SET_REG_BIT(DMA->INTRAMCLRSTATUS,ramclrstatus);
	SET_REG_BIT(DMA->INTSTATUS,dmaintstatus);

    if(ramclrstatus & DMA_RAMCLR_INIT_DONE)
    {  
		
	}
	
    if(ramclrstatus & DMA_RAMCLR_STARTADDR_ERROR)
    {  

	}

    if(ramclrstatus & DMA_RAMCLR_ENDADDR_ERROR)
    {  

	}

    if(ramclrstatus & DMA_RAMCLR_ADDR_REVERSED)
    {  

	}

	channel = 0;
	intstatus = ((dmaintstatus & DMA_INT_HF_CHALL)>>DMA_INT_HF_S);
	while(intstatus)
	{
		if(intstatus & (1<<channel))
		{
			if(sDMAChannelHFCmpltCallbck[channel])
			{
				sDMAChannelHFCmpltCallbck[channel]();
			}				
			intstatus &= ~(1<<channel);
		}		
		channel++;
	}

	channel = 0;
	intstatus = ((dmaintstatus & DMA_INT_BE_CHALL)>>DMA_INT_BE_S);
	while(intstatus)
	{
		if(intstatus & (1<<channel))
		{
			intstatus &= ~(1<<channel);
		}		
		channel++;
	}
	
	channel = 0;
	intstatus = ((dmaintstatus & DMA_INT_TC_CHALL)>>DMA_INT_TC_S);
	while(intstatus)
	{
		if(intstatus & (1<<channel))
		{
			if(sDMAChannelCmpltCallbck[channel])
			{
				sDMAChannelCmpltCallbck[channel]();
			}			
			intstatus &= ~(1<<channel);
		}		
		channel++;
	}

	if(dmaintstatus & DMA_INT_BE_CHALL) 
	{		        
		SET_REG_VAL(DMA->SOFTRESET,1);	
	}
}






int32_t DMA_GetChannelStatus(uint32_t ch)
{	
	if(ch>=DMAC_MAX_CHANNEL)
	{
		return DMA_PARAM_INCORRECT;
	}
	
	if(GET_REG_BIT(DMA->DMACHL[ch].CHSTS,DMA_CHSTS_EN_M|DMA_CHSTS_BUSY_M|DMA_CHSTS_GRANT_M))
	{
		return DMA_BUSY;
	}

    return DMA_IDLE;
}


int32_t DMA_GetFreeChannel(void)
{
    for(uint8_t i=0;i<DMAC_MAX_CHANNEL;i++)
    {
        if(!(GET_REG_BIT(DMA->DMACHL[i].CHSTS,DMA_CHSTS_EN_M|DMA_CHSTS_BUSY_M|DMA_CHSTS_GRANT_M)))
            return i;
    }

    return DMA_INVALID_CHANNEL;
}

#if DMA_CONFIG_SANITY

#define I2C_DR_OFFSET   	0x10
#define SCD_TDR_OFFSET  	0x10
#define SCD_RDR_OFFSET  	0x20
#define SCC_DR_OFFSET   	0x10
#define DAC_FIFO_OFFSET 	0x04
#define DAC_REG_OFFSET    	0x18
#define ADC_FIFO_OFFSET		0x1C
#define PWM7_DR_OFFSET		0x20
#define UART_DR_OFFSET		0x18
#define SPIS_DATA_OFFSET	0x80
#define SPIM_DR_OFFSET		0x1C

/**
 * @brief DMA外设目的地址/源地址表
 * 
 */
static const uint32_t sDMAPeripheralAddrList[DMA_CHCTRL_PERIPHERAL_NUM]=
{
	I2C0_BASE+I2C_DR_OFFSET,		//0x00
	SCD_BASE+SCD_RDR_OFFSET,		//0x01
	//SCD_BASE+SCD_TDR_OFFSET
	SCC_BASE+SCC_DR_OFFSET,			//0x02
	0,								//0x03
	I2C1_BASE+I2C_DR_OFFSET,		//0x04
	0, 								//0x05
    //DAC_BASE+DAC_FIFO_OFFSET
	//DAC_BASE+DAC_REG_OFFSET
	ADC_BASE+ADC_FIFO_OFFSET,		//0x06
	PWM7_BASE+PWM7_DR_OFFSET,		//0x07
	PWM6_BASE+PWM7_DR_OFFSET,		//0x08
	PWM5_BASE+PWM7_DR_OFFSET,		//0x09
	PWM4_BASE+PWM7_DR_OFFSET,		//0x0a
	PWM3_BASE+PWM7_DR_OFFSET,		//0x0b
	PWM2_BASE+PWM7_DR_OFFSET,		//0x0c
	PWM1_BASE+PWM7_DR_OFFSET,		//0x0d
	PWM0_BASE+PWM7_DR_OFFSET,		//0x0e
	UART2_BASE+UART_DR_OFFSET,		//0x0f
	UART2_BASE+UART_DR_OFFSET,		//0x10
	UART1_BASE+UART_DR_OFFSET,		//0x11
	UART1_BASE+UART_DR_OFFSET,		//0x12
	UART0_BASE+UART_DR_OFFSET,		//0x13
	UART0_BASE+UART_DR_OFFSET,		//0x14
	SPIS1_BASE+SPIS_DATA_OFFSET,	//0x15
	SPIS0_BASE+SPIS_DATA_OFFSET,	//0x16
	SPIM1_BASE+SPIM_DR_OFFSET,		//0x17
	SPIM1_BASE+SPIM_DR_OFFSET, 		//0x18
	SPIM0_BASE+SPIM_DR_OFFSET,		//0x19
	SPIM0_BASE+SPIM_DR_OFFSET,		//0x1a
};


/**
 * @brief DMA配置参数验证
 * 
 * @param[in] config DMA配置参数结构体
 * @return int32_t
 * - @ref DMA_SUCCESS： 表示成功
 * - @ref DMA_PARAM_INCORRECT： 表示参数错误 
 */
static int32_t DMA_ConfigSanity(const DMAConfig *config)
{
	uint32_t flow;
	uint32_t size;
	uint32_t width;
	uint32_t peripheral;

	width = ((config->chctrl&DMA_CHCTRL_WIDTH_MASK)>>DMA_CHCTRL_WIDTH_S);
	flow = (config->chctrl&DMA_CHCTRL_FLOW_MASK);
	size = ((config->chctrl&DMA_CHCTRL_TRANSFER_SIZE_MASK)>>DMA_CHCTRL_TRANSFER_SIZE_S);

	size <<= width;

	switch(flow)
	{
		case DMA_CHCTRL_FLOW_M2M:
			if(config->addr_src<DMA_RAM_START_ADDR||config->addr_src+size>DMA_RAM_END_ADDR)
			{
				return DMA_PARAM_INCORRECT;
			}	
			if(config->addr_dst<DMA_RAM_START_ADDR||config->addr_dst+size>DMA_RAM_END_ADDR)
			{
				return DMA_PARAM_INCORRECT;
			}						
		break;
		case DMA_CHCTRL_FLOW_M2P:
			if(config->addr_src<DMA_RAM_START_ADDR||config->addr_src+size>DMA_RAM_END_ADDR)
			{
				return DMA_PARAM_INCORRECT;
			}
			peripheral = ((config->chctrl&DMA_CHCTRL_DSTPER_MASK)>>DMA_CHCTRL_DSTPER_S);
			if(peripheral>=DMA_CHCTRL_PERIPHERAL_NUM)
			{
				return DMA_PARAM_INCORRECT;
			}
			else if(peripheral == (DMA_CHCTRL_DSTPER_SCD>>DMA_CHCTRL_DSTPER_S))
			{
				if(config->addr_dst!=SCD_BASE+SCD_TDR_OFFSET)
				{
					return DMA_PARAM_INCORRECT;
				}
			}
			else if(peripheral == (DMA_CHCTRL_DSTPER_DAC>>DMA_CHCTRL_DSTPER_S))
			{
				if(config->addr_dst!=DAC_BASE+DAC_FIFO_OFFSET
					&&config->addr_dst!=DAC_BASE+DAC_REG_OFFSET)
				{
					return DMA_PARAM_INCORRECT;
				}
			}
			else
			{
				if(config->addr_dst!=sDMAPeripheralAddrList[peripheral])
				{
					return DMA_PARAM_INCORRECT;
				}
			}
		break;
		case DMA_CHCTRL_FLOW_P2M:
			if(config->addr_dst<DMA_RAM_START_ADDR||config->addr_dst+size>DMA_RAM_END_ADDR)
			{
				return DMA_PARAM_INCORRECT;
			}
			peripheral = ((config->chctrl&DMA_CHCTRL_SRCPER_MASK)>>DMA_CHCTRL_SRCPER_S);
			if(peripheral>=DMA_CHCTRL_PERIPHERAL_NUM)
			{
				return DMA_PARAM_INCORRECT;
			}
			else
			{
				if(config->addr_src!=sDMAPeripheralAddrList[peripheral])
				{
					return DMA_PARAM_INCORRECT;
				}				
			}			
		break;	
		default:			
		break;
	}

	return DMA_SUCCESS;
}
#endif


int32_t DMA_SetChannelConfig(uint32_t ch, const DMAConfig *config)
{	
	if(ch>=DMAC_MAX_CHANNEL)
	{
		return DMA_PARAM_INCORRECT;
	}

	#if DMA_CONFIG_SANITY
	if(DMA_ConfigSanity(config)!=DMA_SUCCESS)
	{
		return DMA_PARAM_INCORRECT;
	}
	#endif

	while(DMA_GetChannelStatus(ch) == DMA_BUSY)
	{
		CLR_REG_BIT(DMA->EN,DMA_CTRL_EN_M);
	}

    SET_REG_VAL(DMA->DMACHL[ch].SRCADDR,config->addr_src);
    SET_REG_VAL(DMA->DMACHL[ch].DSTADDR,config->addr_dst);
    SET_REG_VAL(DMA->DMACHL[ch].CHCTRL,config->chctrl);

	sDMAChannelCmpltCallbck[ch] = config->cmpltcb;
	sDMAChannelHFCmpltCallbck[ch] = config->hfcb;
	return DMA_SUCCESS;
}





int32_t DMA_TransferStart(uint32_t ch)
{
	if(ch>=DMAC_MAX_CHANNEL)
	{
		return DMA_PARAM_INCORRECT;
	}

    SET_REG_BIT(DMA->DMACHL[ch].CHSTS,DMA_CHSTS_EN_M);
    SET_REG_BIT(DMA->EN,DMA_CTRL_EN_M);

	return DMA_SUCCESS;
}
