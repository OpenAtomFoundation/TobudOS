
/***************************************************************************** 
* 
* File Name : wm_adc.c 
* 
* Description: adc Driver Module 
* 
* Copyright (c) 2014 Winner Microelectronics Co., Ltd. 
* All rights reserved. 
* 
* Author : dave
* 
* Date : 2014-8-15
*****************************************************************************/ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_regs.h"
#include "wm_adc.h"
#include "wm_dma.h"
#include "misc.h"
#include "wm_io.h"
#include "wm_irq.h"


static u16 adc_offset = 0;

volatile ST_ADC gst_adc;
volatile u8 adc_irq_flag = 0;
void ADC_IRQHandler(void)
{
	u16 adcvalue;
	int reg;

	reg = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	if(reg & CONFIG_ADC_INT)      //ADC中断
	{
	    adc_irq_flag = 1;
	    tls_adc_clear_irq(ADC_INT_TYPE_ADC);
	    adcvalue = tls_read_adc_result();
	    if(gst_adc.adc_cb)
			gst_adc.adc_cb(&adcvalue,1);
	}
	if(reg & CONFIG_ADC_INT_CMP)
	{
	    tls_adc_clear_irq(ADC_INT_TYPE_ADC_COMP);
	    if(gst_adc.adc_bigger_cb)
			gst_adc.adc_bigger_cb(NULL, 0);
	}
	
}

static void adc_dma_isr_callbk(void)
{
	if(gst_adc.adc_dma_cb)
		gst_adc.adc_dma_cb((u16 *)(ADC_DEST_BUFFER_DMA), gst_adc.valuelen);	
}


void tls_adc_init(u8 ifusedma,u8 dmachannel)
{
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, 0x0);
	tls_irq_enable(ADC_IRQn);

//注册中断和channel有关，所以需要先请求
	if(ifusedma)
	{
		gst_adc.dmachannel = tls_dma_request(dmachannel, NULL);	//请求dma，不要直接指定，因为请求的dma可能会被别的任务使用
		tls_dma_irq_register(gst_adc.dmachannel, (void(*)(void*))adc_dma_isr_callbk, NULL, TLS_DMA_IRQ_TRANSFER_DONE);
	}

	//printf("\ndma channel = %d\n",gst_adc.dmachannel);
}

void tls_adc_clear_irq(int inttype)
{
    int reg;
    reg = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	if(ADC_INT_TYPE_ADC == inttype)
	{
	    reg |= CONFIG_ADC_INT;
	    tls_reg_write32(HR_SD_ADC_CONFIG_REG, reg);
	}
	else if(ADC_INT_TYPE_ADC_COMP== inttype)
	{
	    reg |= CONFIG_ADC_INT_CMP;
	    tls_reg_write32(HR_SD_ADC_CONFIG_REG, reg);
	}
	else if(ADC_INT_TYPE_DMA == inttype)
	{
	    tls_dma_irq_clr(gst_adc.dmachannel, TLS_DMA_IRQ_TRANSFER_DONE);
	}
}

void tls_adc_irq_register(int inttype, void (*callback)(u16 *buf, u16 len))
{
	if(ADC_INT_TYPE_ADC == inttype)
	{
		gst_adc.adc_cb = callback;
	}
	else if(ADC_INT_TYPE_DMA == inttype)
	{
		gst_adc.adc_dma_cb = callback;
	}
	else if(ADC_INT_TYPE_ADC_COMP == inttype)
	{
	    gst_adc.adc_bigger_cb = callback;
	}
}

u16 tls_read_adc_result(void)
{
	u32 value;
	u16 ret;
	
	value = tls_reg_read32(HR_SD_ADC_RESULT_REG);
	ret = value&0x3fff;
	
	return ret;
}

void tls_adc_start_with_cpu(int Channel)
{
	u32 value;
	
	Channel &= CONFIG_ADC_CHL_MASK;
        
	/* Stop adc first */
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_DMA_MASK;
	value &= ~CONFIG_ADC_START;
	value |= Channel;

	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_START;
	value |= CONFIG_ADC_INT_MASK;
	if(Channel < 8)
	{
		value |= CONFIG_ADC_G_CTRL12;
		value &= ~ CONFIG_ADC_VCM(0x3F);
		value |= CONFIG_ADC_VCM(0x1F);
	}
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
}


void tls_adc_enable_calibration_buffer_offset(void)
{
	u32 value;
        
	/* Stop adc first */
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_DMA_MASK;
	value &= ~CONFIG_ADC_START;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_CHL_OFFSET;
	value |= CONFIG_ADC_START;
	value |= CONFIG_ADC_INT_MASK;
	value |= CONFIG_ADC_EN_CAL;
	value |= CONFIG_ADC_G_CTRL12;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
}

void tls_adc_temp_offset_with_cpu(u8 calTemp12)
{
	u32 value;
        
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_DMA_MASK;
	value &= ~CONFIG_ADC_START;
	
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_CHL_TEMP;
	value |= CONFIG_ADC_START;
	value |= CONFIG_ADC_INT_MASK;
	if(calTemp12)
	{
		value |= CONFIG_ADC_CAL_OFFSET_TEMP12;
	}
	else
	{
		value &= ~CONFIG_ADC_CAL_OFFSET_TEMP12;
	}
	value &= ~CONFIG_ADC_G_CTRL12;
	value |= CONFIG_ADC_TEMP_ON; 
	value |= CONFIG_ADC_G_TEMP12(3);
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);		/*start adc*/
}

void tls_adc_voltage_start_with_cpu(void)
{
	u32 value;
        
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_DMA_MASK;
	value &= ~CONFIG_ADC_START;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_CHL_VOLT;
	value |= CONFIG_ADC_START;
	value |= CONFIG_ADC_INT_MASK;
	value |= CONFIG_ADC_G_CTRL12;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);		/*start adc*/
}

void tls_adc_start_with_dma(int Channel, int Length)
{
	u32 value;
	int len;

	if(Channel < 0 || Channel > 11)
		return;
        
	if(Length > ADC_DEST_BUFFER_SIZE)
		len = ADC_DEST_BUFFER_SIZE;
	else
		len = Length;

	gst_adc.valuelen = len;

	Channel &= CONFIG_ADC_CHL_MASK;

	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_START;      //stop
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	
	/* Stop dma if necessary */
	while(DMA_CHNLCTRL_REG(gst_adc.dmachannel) & 1)
	{
		DMA_CHNLCTRL_REG(gst_adc.dmachannel) = 2;
	}

	DMA_SRCADDR_REG(gst_adc.dmachannel) = HR_SD_ADC_RESULT_REG;
	DMA_DESTADDR_REG(gst_adc.dmachannel) = ADC_DEST_BUFFER_DMA;
	/* Hard, Normal, adc_req */
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);		
	if (Channel == 8){
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (0+6)<<2);
		value |= (0x1 << 11); 		
	}
	else if (Channel == 9){
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (2+6)<<2);
		value |= (0x1 << 13); 		
	}
	else if (Channel == 10){
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (4+6)<<2);
		value |= (0x1 << 15); 		
	}
	else if (Channel == 11){
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (6+6)<<2);
		value |= (0x1 << 17); 		
	}
	else{
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (Channel+6)<<2);
		value |= (0x1 << (11 + Channel)); 		
	}	
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	/* Dest_add_inc, halfword,  */
	DMA_CTRL_REG(gst_adc.dmachannel) = (1<<3)|(1<<5)|((len*2)<<8);
	DMA_INTMASK_REG &= ~(0x01 << (gst_adc.dmachannel *2 + 1));
	DMA_CHNLCTRL_REG(gst_adc.dmachannel) = 1;		/* Enable dma */

	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_START;
	value |= Channel;
	if(Channel < 8)
	{
		value |= CONFIG_ADC_G_CTRL12;
		value &= ~ CONFIG_ADC_VCM(0x3F);
		value |= CONFIG_ADC_VCM(0x1F);	
	}
//	printf("config value==%x\n", value);
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);		/*start adc*/
}

void tls_adc_stop(int ifusedma)
{
	u32 value;

	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_START;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);

	if(ifusedma)
		tls_dma_free(gst_adc.dmachannel);
}

void tls_adc_config_cmp_reg(int cmp_data, int cmp_pol)
{
    u32 value;

	cmp_data &= 0x3FFF;
    value = tls_reg_read32(HR_SD_ADC_RESULT_REG);
	value &= ~(0x3FFF<<14);
    value |= (cmp_data<<14);
    tls_reg_write32(HR_SD_ADC_RESULT_REG, value);

    value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
    value |= CONFIG_ADC_CMP_INT_MASK;
	if(cmp_pol)
	{
		value |= CONFIG_ADC_CMP_POL;
	}
	else
	{
		value &= ~CONFIG_ADC_CMP_POL;
	}
    tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
}

void tls_adc_cmp_start(int Channel, int cmp_data, int cmp_pol)
{
	u32 value;
	
	Channel &= CONFIG_ADC_CHL_MASK;
		
	/* Stop adc first */
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_DMA_MASK;
	value &= ~CONFIG_ADC_START;
	value |= Channel;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);

	tls_adc_config_cmp_reg(cmp_data, cmp_pol);
	
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_START;
	if(Channel < 8)
	{
		value |= CONFIG_ADC_G_CTRL12;
		value &= ~ CONFIG_ADC_VCM(0x3F);
		value |= CONFIG_ADC_VCM(0x1F);
	}
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);		/*start adc*/
}


void tls_adc_reference_sel(int ref)
{
    u32 value;
    
    value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
    if(ADC_REFERENCE_EXTERNAL == ref)
    {
        value &= ~CONFIG_ADC_REF_SEL;
		value |= CONFIG_ADC_BUF_BYPASS;
    }
    else if(ADC_REFERENCE_INTERNAL == ref)
    {
        value |= CONFIG_ADC_REF_SEL;
		value &= ~CONFIG_ADC_BUF_BYPASS;
    }
    tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);    
}

void tls_adc_set_clk(int div)
{
    u32 value;

    value = tls_reg_read32(HR_CLK_DIV_CTL);
    value &= ~(0xffff<<12);
    value |= (div << 12);
    value |= ((u32)0x1<<31);
    tls_reg_write32(HR_CLK_DIV_CTL, value);
}

void signedToUnsignedData(u16 *adcValue, u16 *offset)
{
//  s16 temp = 0;
  if(*adcValue >= 8192)
  {
    *adcValue -= 8192;
  }
  else
  {
    *adcValue += 8192;
  }

/*  if (*offset >=8192)
  {
    temp =((*offset-1)&0x1FFF) - 8192;
  }else{
    temp = *offset;
  }

  *adcValue -= temp;
*/
}

static void waitForAdcDone(void)
{
  while(1)
  {
    if(adc_irq_flag)      //ADC中断
    {
      adc_irq_flag = 0;
      break;
    }
  }
}

u16 adc_get_offset(void)
{ 
  tls_adc_init(0, 0); 
  tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);
  adc_irq_flag = 0;
  tls_adc_enable_calibration_buffer_offset(); //使能校准功能
  waitForAdcDone();
  adc_offset = tls_read_adc_result(); //获取adc转换结果
  tls_adc_stop(0);

  //printf("\r\noffset:%d", adc_offset);
  return adc_offset;
}

u32 adc_get_interTemp(void)
{
	u16 code2, code1, realCode;
  u32 tem;

  tls_adc_init(0, 0); 
  tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);
  adc_irq_flag = 0;
  tls_adc_temp_offset_with_cpu(1); //code2
  waitForAdcDone();
  code2 = tls_read_adc_result(); 
  tls_adc_stop(0);
  signedToUnsignedData(&code2, &adc_offset);

  adc_irq_flag = 0;
  tls_adc_temp_offset_with_cpu(0); //code1
  waitForAdcDone();
  code1 = tls_read_adc_result();
  tls_adc_stop(0);
  signedToUnsignedData(&code1, &adc_offset);

  realCode = ( (code1-code2)/2+8192 );
  //printf("\r\nTEMP:%.1f", realCode*0.12376-1294.58);
  //return (realCode*0.12376-1294.58);
  tem = realCode*124-1294580;
  return tem;
}

u16 adc_get_inputVolt(u8 channel)
{
  u16 average = 0;

  /*calibration*/
  adc_get_offset();
  /*get value*/
  tls_adc_init(0, 0);
  tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);
  adc_irq_flag = 0;
  tls_adc_start_with_cpu(channel);
  waitForAdcDone();
  average = tls_read_adc_result();
  tls_adc_stop(0);

  signedToUnsignedData(&average, &adc_offset);

  //printf("ch[%d]inputVolt:%.3f\r\n", channel+1, ((average-8192.0)/8192*2.25/1.2 + 1.584));
  return average;
}

u16 adc_get_interVolt(void)
{
  u16 voltValue;

  /*calibration*/
  adc_get_offset();
  /*get value*/
  tls_adc_init(0, 0);
  tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);

  adc_irq_flag = 0;
  tls_adc_voltage_start_with_cpu();
  waitForAdcDone();
  voltValue = tls_read_adc_result();
  tls_adc_stop(0);

  signedToUnsignedData(&voltValue, &adc_offset);
  float voltage = ( 1.214 - ((float)voltValue-8192)/8192*2.25/1.2 )*2;
  //printf("\r\ninterVolt:%.2f", voltage);
  return voltValue;
}

u32 adc_temp(void)
{
  u32 tem;
  adc_get_offset();
  tem = adc_get_interTemp();
  return tem;
}



