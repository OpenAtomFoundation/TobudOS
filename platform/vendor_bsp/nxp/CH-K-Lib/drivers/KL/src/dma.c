/**
  ******************************************************************************
  * @file    dma.c
  * @author  YANDLD
  * @version V2.6
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片DMA模块的底层功能函数
  ******************************************************************************
  */

#include "dma.h"
#include "common.h"

    
static const IRQn_Type DMA_IRQnTable[] = 
{
    DMA0_IRQn,
    DMA1_IRQn,
    DMA2_IRQn,
    DMA3_IRQn,
};


/**
 * @brief  初始化DMA模块
 * @param  DMA_InitStruct :DMA初始化配置结构体，详见dma.h
 * @retval None
 */
void DMA_Init(DMA_Init_t *Init)
{
	/* enable DMA and DMAMUX clock */
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;    
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    
    /* disable chl first */
    DMA0->DMA[Init->chl].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[Init->chl].DCR &= ~DMA_DCR_ERQ_MASK;
    
    /* dma chl source config */
    DMAMUX0->CHCFG[Init->chl] = DMAMUX_CHCFG_SOURCE(Init->chlTrigSrc);
    
    /* trigger mode */
    switch(Init->trigSrcMod)
    {
        case kDMA_TrigSrc_Normal:
            DMAMUX0->CHCFG[Init->chl] &= ~DMAMUX_CHCFG_TRIG_MASK;
            break;
        case kDMA_TrigSrc_Periodic:
            DMAMUX0->CHCFG[Init->chl] |= DMAMUX_CHCFG_TRIG_MASK;
            break;
        default:
            break;
    }
    DMA0->DMA[Init->chl].DCR = 0;
    /* transfer bytes cnt */
    DMA0->DMA[Init->chl].DSR_BCR = DMA_DSR_BCR_BCR(Init->transCnt);
    /* source config */
    DMA0->DMA[Init->chl].SAR  = Init->sAddr;
    DMA0->DMA[Init->chl].DCR |= DMA_DCR_SSIZE(Init->sDataWidth);
    (Init->sAddrIsInc)?(DMA0->DMA[Init->chl].DCR |= DMA_DCR_SINC_MASK):(DMA0->DMA[Init->chl].DCR &= ~DMA_DCR_SINC_MASK);
    DMA0->DMA[Init->chl].DCR |= DMA_DCR_SMOD(Init->sMod);
    /* dest config */
    DMA0->DMA[Init->chl].DAR  = Init->dAddr;
    DMA0->DMA[Init->chl].DCR |= DMA_DCR_DSIZE(Init->sDataWidth);
    (Init->dAddrIsInc)?(DMA0->DMA[Init->chl].DCR |= DMA_DCR_DINC_MASK):(DMA0->DMA[Init->chl].DCR &= ~DMA_DCR_DINC_MASK);
    DMA0->DMA[Init->chl].DCR |= DMA_DCR_DMOD(Init->dMod);
    /* defaut: cycle steal */
    DMA0->DMA[Init->chl].DCR |= DMA_DCR_CS_MASK;
    /* defaut: enable auto disable req */
    DMA0->DMA[Init->chl].DCR |= DMA_DCR_D_REQ_MASK;
    /* enable chl */
    DMAMUX0->CHCFG[Init->chl] |= DMAMUX_CHCFG_ENBL_MASK;
}

/**
 * @brief  获得 DMA Counter 计数值
 * @param  chl: DMA通道号
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @retval 计数值
 */
uint32_t DMA_GetTransCnt(uint8_t chl)
{
    return (DMA0->DMA[chl].DSR_BCR & 0x0000FFFF )>>DMA_DSR_BCR_BCR_SHIFT;
}

void DMA_SetTransCnt(uint8_t chl, uint32_t val)
{
    DMA0->DMA[chl].DSR_BCR = DMA_DSR_BCR_BCR(val);
}

/**
 * @brief  使能通道响应传输
 * @code
 *     //开启DMA 的0通道进行数据传输
 *     DMA_EnableReq(HW_DMA_CH0);
 * @endcode
 * @param  chl: DMA通道号
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @retval None
 */
void DMA_EnableReq(uint8_t chl)
{
    DMA0->DMA[chl].DCR |= DMA_DCR_ERQ_MASK;
}


/**
 * @brief  禁止通道响应传输
 * @code
 *     //开启DMA 的0通道进行数据传输
 *     DMA_EnableReq(HW_DMA_CH0);
 * @endcode
 * @param  chl: DMA通道号
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @retval None
 */
void DMA_DisableReq(uint8_t chl)
{
    DMA0->DMA[chl].DCR &= ~DMA_DCR_ERQ_MASK;
}


/**
 * @brief  每次一个触发信号, 只触发一次传输
 */
void DMA_EnableCycleSteal(uint8_t chl, bool flag)
{
    (flag)?
    (DMA0->DMA[chl].DCR |= DMA_DCR_CS_MASK):
    (DMA0->DMA[chl].DCR &= ~DMA_DCR_CS_MASK);
}

/**
 * @brief  在Majloop 结束后  是否自动关闭Request
 */
void DMA_EnableAutoDisableRequest(uint8_t chl , bool flag)
{
    (flag)?
    (DMA0->DMA[chl].DCR |= DMA_DCR_D_REQ_MASK):
    (DMA0->DMA[chl].DCR &= ~DMA_DCR_D_REQ_MASK);
}

void DMA_SetDestAddr(uint8_t ch, uint32_t address)
{
    DMA0->DMA[ch].DAR = address;
}

uint32_t DMA_GetDestAddr(uint8_t ch)
{
    return DMA0->DMA[ch].DAR;
}

void DMA_SetSrcAddr(uint8_t ch, uint32_t address)
{
    DMA0->DMA[ch].SAR = address;
}

uint32_t DMA_GetSrcAddr(uint8_t ch)
{
    return DMA0->DMA[ch].SAR;
}

/**
 * @brief  设置DMA传输完成中断
 * @code
 *     //开启DMA 的0通道的传输完成中断功能
 *     DMA_StartTransfer(HW_DMA_CH0);
 * @param  chl: DMA通道号
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @param config: 配置模式
 *         @arg kDMA_IT_Half_Disable 禁止DMA传输一半中断触发
 *         @arg kDMA_IT_Major_Disable 禁止DMA传输完成中断触发
 *         @arg kDMA_IT_Half 开启DMA传输一半中断触发
 *         @arg kDMA_IT_Major 开启DMA传世完成中断触发
 * @retval None
 */

void DMA_SetIntMode(uint8_t chl, DMA_Int_t mode, bool val)
{
    if(val)
    {
        NVIC_EnableIRQ(DMA_IRQnTable[chl]);
    }
    
    switch(val)
    {
        case kDMA_Int_Finish:
            (val)?
            (DMA0->DMA[chl].DCR |= DMA_DCR_EINT_MASK):
            (DMA0->DMA[chl].DCR &= DMA_DCR_EINT_MASK);
            break; 
        default:
            break;
    }
}

///**
// * @brief  注册中断回调函数
// * @param  chl: DMA通道号
// *         @arg HW_DMA_CH0
// *         @arg HW_DMA_CH1
// *         @arg HW_DMA_CH2
// *         @arg HW_DMA_CH3
// * @param AppCBFun: 回调函数指针
// * @retval None
// */
//void DMA_CallbackInstall(uint8_t chl, DMA_CallBackType AppCBFun)
//{
//    if(AppCBFun != NULL)
//    {
//        DMA_CallBackTable[chl] = AppCBFun;
//    }
//}

void DMA_CancelTransfer(uint8_t chl)
{
    DMA0->DMA[chl].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
}

//void DMA0_IRQHandler(void)
//{
//    DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
//    if(DMA_CallBackTable[0]) DMA_CallBackTable[0]();
//}

//void DMA1_IRQHandler(void)
//{
//    DMA0->DMA[1].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
//    if(DMA_CallBackTable[1]) DMA_CallBackTable[1]();
//}

//void DMA2_IRQHandler(void)
//{
//    DMA0->DMA[2].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
//    if(DMA_CallBackTable[2]) DMA_CallBackTable[2]();
//}

//void DMA3_IRQHandler(void)
//{
//    DMA0->DMA[3].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
//    if(DMA_CallBackTable[3]) DMA_CallBackTable[3]();
//}

