/**
  ******************************************************************************
  * @file    pit.c
  * @author  YANDLD
  * @version V2.6
  * @date    2015.06.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "pit.h"
#include "common.h"

//!< Internal vars
static uint32_t fac_us; //!< usDelay Mut

static const Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_PIT_MASK, SIM_SCGC6_PIT_SHIFT},
};

static const IRQn_Type PIT_IrqTbl[] = 
{
    PIT_IRQn,
};

//! @defgroup CHKinetis
//! @{


//! @defgroup PIT
//! @brief PIT API functions
//! @{

 /**
 * @brief  详细初始化PIT模块 推荐使用PIT_QuickInit函数
 * @param  
 * @retval None
 */
void PIT_Init(void)
{
    CLK_EN(CLKTbl, 0);
    /* get clock */
    fac_us = GetClock(kBusClock);
    fac_us /= 1000000;
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
}


 /**
 * @brief  设置PIT模块是否开启中断功能
 * 
 * @param  chl  :通道号
 *         @arg HW_PIT0_CH0   :0通道
 *         @arg HW_PIT0_CH1   :1通道
 *         @arg HW_PIT0_CH2   :2通道
 *         @arg HW_PIT0_CH3   :3通道
 * @param  NewState ENABLE 或DISABLE
 * @retval None
 */
uint32_t PIT_SetIntMode(uint32_t chl, bool val)
{
    CLK_EN(CLKTbl, 0);
    if(val)
    {
        NVIC_EnableIRQ(PIT_IrqTbl[chl]);
    }
    (val)?
    (PIT->CHANNEL[chl].TCTRL |= PIT_TCTRL_TIE_MASK):
    (PIT->CHANNEL[chl].TCTRL &= ~PIT_TCTRL_TIE_MASK);

    PIT->CHANNEL[chl].TFLG |= PIT_TFLG_TIF_MASK;
    return 0;
}

void PIT_SetTime(uint32_t chl, uint32_t us)
{
    PIT->CHANNEL[chl].TCTRL &= (~PIT_TCTRL_TEN_MASK);
    PIT->CHANNEL[chl].LDVAL = fac_us * us;
    PIT->CHANNEL[chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
}

uint32_t PIT_GetCnt(uint32_t chl)
{
    return PIT->CHANNEL[chl].CVAL;
}

void PIT_SetCnt(uint8_t chl, uint32_t val)
{
    PIT->CHANNEL[chl].TCTRL &= (~PIT_TCTRL_TEN_MASK);
    PIT->CHANNEL[chl].LDVAL = val;
    PIT->CHANNEL[chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
}


//! @}

//! @}
/*
void PIT_IRQHandler(void)
{
    if(PIT->CHANNEL[0].TFLG)
    {
        PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
        if(PIT_CallBackTable[0])
        {
            PIT_CallBackTable[0]();
        } 
    }
    if(PIT->CHANNEL[1].TFLG)
    {
        PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
        if(PIT_CallBackTable[1])
        {
            PIT_CallBackTable[1]();
        } 
    }
}
*/

