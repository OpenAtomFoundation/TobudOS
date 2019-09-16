/**
  ******************************************************************************
  * @file    dma.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * \date    2015.10.08 FreeXc 完善了对 dma 模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片DMA模块的底层功能函数
  ******************************************************************************
  */

#include "dma.h"
#include "common.h"

#if !defined(DMAMUX_BASES)
#define DMAMUX_BASES {DMAMUX};

#endif

/* gloabl const table defination */
static DMAMUX_Type * const DMAMUX_InstanceTable[] = DMAMUX_BASES;
static DMA_CallBackType DMA_CallBackTable[16] = {NULL};
static uint32_t DMAChlMAP;

uint32_t _DMA_ChlAlloc(void);
void DMA_ChlFree(uint32_t chl);

/* DMA中断向量入口 */
/* 默认第一个DMA中断向量入口为0 */
static const IRQn_Type DMA_IRQnTable[] = 
{
    (IRQn_Type)(0 + 0),
    (IRQn_Type)(0 + 1),
    (IRQn_Type)(0 + 2),
    (IRQn_Type)(0 + 3),
    (IRQn_Type)(0 + 4),
    (IRQn_Type)(0 + 5),
    (IRQn_Type)(0 + 6),
    (IRQn_Type)(0 + 7),
    (IRQn_Type)(0 + 8),
    (IRQn_Type)(0 + 9),
    (IRQn_Type)(0 + 10),
    (IRQn_Type)(0 + 11),
    (IRQn_Type)(0 + 12),
    (IRQn_Type)(0 + 13),
    (IRQn_Type)(0 + 14),
    (IRQn_Type)(0 + 15),
};


/**
 * @brief  初始化DMA模块
 * @param[in]  DMA_InitStruct 指向DMA初始化配置结构体的指针，详见dma.h
 * @return 分配到的DMA通道
 */
uint32_t DMA_Init(DMA_InitTypeDef *DMA_InitStruct)
{
    uint8_t chl;
    
	/* enable DMA and DMAMUX clock */
#if defined(DMAMUX0)  
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX0_MASK;
#endif
#if  defined(DMAMUX1)
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX1_MASK;
#endif
#if  defined(DMAMUX)
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
#endif
	
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    
    chl = DMA_InitStruct->chl;

    /* disable chl first */
    DMA0->ERQ &= ~(1<<(chl));
    /* dma chl source config */
    DMAMUX_InstanceTable[0]->CHCFG[chl] = DMAMUX_CHCFG_SOURCE(DMA_InitStruct->chlTriggerSource);
    /* trigger mode */
    switch(DMA_InitStruct->triggerSourceMode)
    {
        case kDMA_TriggerSource_Normal:
            DMAMUX_InstanceTable[0]->CHCFG[chl] &= ~DMAMUX_CHCFG_TRIG_MASK;
            break;
        case kDMA_TriggerSource_Periodic:
            DMAMUX_InstanceTable[0]->CHCFG[chl] |= DMAMUX_CHCFG_TRIG_MASK;
            break;
        default:
            break;
    }
    /* clear some register */
    DMA0->TCD[chl].ATTR  = 0;
    DMA0->TCD[chl].CSR   = 0;
    /* minor loop cnt */
    DMA0->TCD[chl].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(DMA_InitStruct->minorLoopByteCnt);
    /* major loop cnt */
	DMA0->TCD[chl].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(DMA_InitStruct->majorLoopCnt);
	DMA0->TCD[chl].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(DMA_InitStruct->majorLoopCnt);
    /* source config */
    DMA0->TCD[chl].SADDR = DMA_InitStruct->sAddr;
    DMA0->TCD[chl].SOFF = DMA_InitStruct->sAddrOffset;
    DMA0->TCD[chl].ATTR |= DMA_ATTR_SSIZE(DMA_InitStruct->sDataWidth);
    DMA0->TCD[chl].ATTR |= DMA_ATTR_SMOD(DMA_InitStruct->sMod);
    DMA0->TCD[chl].SLAST = DMA_SLAST_SLAST(DMA_InitStruct->sLastAddrAdj);
    /* destation config */
    DMA0->TCD[chl].DADDR = DMA_InitStruct->dAddr;
    DMA0->TCD[chl].DOFF = DMA_InitStruct->dAddrOffset;
    DMA0->TCD[chl].ATTR |= DMA_ATTR_DSIZE(DMA_InitStruct->dDataWidth);
    DMA0->TCD[chl].ATTR |= DMA_ATTR_DMOD(DMA_InitStruct->dMod);
    DMA0->TCD[chl].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(DMA_InitStruct->dLastAddrAdj);
    /* auto close enable(disable req on major loop complete)*/
    DMA0->TCD[chl].CSR |= DMA_CSR_DREQ_MASK;
	/* enable DMAMUX */
	DMAMUX_InstanceTable[0]->CHCFG[chl] |= DMAMUX_CHCFG_ENBL_MASK;
    
    return chl;
}

/**
 * \brief DMA通道分配
 * \code 
 *   // init DMA 
 *   uint8_t dma_chl;
 *　 dma_chl = DMA_ChlAlloc();
 * \endcode
 * \return dma channel
 */
uint32_t DMA_ChlAlloc(void)
{
    uint32_t i;
    uint32_t MaxDMAChl;
    
    /* get max DMA chl on this device */
    MaxDMAChl = (ARRAY_SIZE(DMAMUX_InstanceTable[0]->CHCFG)>32)?(ARRAY_SIZE(DMAMUX_InstanceTable[0]->CHCFG)):(32);
    
    /* alloc a chl */
    for(i=0;i<MaxDMAChl;i++)
    {
        if(!(DMAChlMAP & (1<< i)))
        {
            DMAChlMAP |= (1<<i);
            return i;
        }
    }
    return 0;
} 

/**
 * \brief DMA通道释放
 * \param[in] chl DMA通道号
 * \return None
 */
void DMA_ChlFree(uint32_t chl)
{
    DMAChlMAP &= ~(1<<chl);
}

/**
 * @brief  获得 DMA MajorLoopCount 计数值
 * @param[in]  chl DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @retval 计数值
 */
uint32_t DMA_GetMajorLoopCount(uint8_t chl)
{
    return (DMA0->TCD[chl].CITER_ELINKNO & DMA_CITER_ELINKNO_CITER_MASK) >> DMA_CITER_ELINKNO_CITER_SHIFT;
}

/**
 * @brief  设置 DMA MajorLoopCount 计数值
 * @param[in]  chl  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * \param[in]  val 计数值
 * @retval None
 * @note   数值不能超过 DMA_CITER_ELINKNO_CITER_MASK
 */
void DMA_SetMajorLoopCounter(uint8_t chl, uint32_t val)
{
    DMA0->TCD[chl].CITER_ELINKNO &= ~DMA_CITER_ELINKNO_CITER_MASK;
    DMA0->TCD[chl].CITER_ELINKNO |= DMA_CITER_ELINKNO_CITER(val);
}


/**
 * @brief  使能通道响应传输
 * @code
 *     //开启DMA 的0通道进行数据传输
 *     DMA_EnableRequest(HW_DMA_CH0);
 * @endcode
 * @param[in]  chl  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @retval None
 */
void DMA_EnableRequest(uint8_t chl)
{
    DMA0->SERQ = DMA_SERQ_SERQ(chl);
}

/**
 * @brief  禁止通道响应传输
 * @code
 *     //禁止DMA 的0通道响应传输
 *     DMA_DisableRequest(HW_DMA_CH0);
 * @endcode
 * @param[in]  chl  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @retval None
 */
void DMA_DisableRequest(uint8_t chl)
{
    DMA0->CERQ = DMA_CERQ_CERQ(chl);
}

/**
 * @brief  在Majloop 结束后  是否自动关闭Request
 * @param[in]  chl  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * \param[in] flag enable or disable
 * 							\arg 0 not affect
 *     					\arg 1 automatically clear
 */
void DMA_EnableAutoDisableRequest(uint8_t chl , bool flag)
{
    if(flag)
    {
        DMA0->TCD[chl].CSR |= DMA_CSR_DREQ_MASK;
    }
    else
    {
        DMA0->TCD[chl].CSR &= ~DMA_CSR_DREQ_MASK;  
    }
}

/**
 * @brief  使能Major LoopLink 功能
 * @note   当一个通道结束MajorLoopLink后 自动开始另一个通道的传输
 * @param[in]  chl  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @param[in]  linkChl 需要连接到通道号
 * \param[in] flag enable or disable
 * 							\arg 0 disable
 *     					\arg 1 enable
 * @retval None
 */
void DMA_EnableMajorLink(uint8_t chl , uint8_t linkChl, bool flag)
{
    if(flag)
    {
        /* enable major loop link */
        DMA0->TCD[chl].CSR |= DMA_CSR_MAJORELINK_MASK;
        /* set chl */
        DMA0->TCD[chl].CSR &= ~DMA_CSR_MAJORLINKCH_MASK;
        DMA0->TCD[chl].CSR |= DMA_CSR_MAJORLINKCH(linkChl);
    }
    else
    {
        DMA0->TCD[chl].CSR &= ~DMA_CSR_MAJORELINK_MASK;
    }
}

/**
 * @brief  设置DMA传输完成中断
 * @code
 *     //开启DMA 的0通道的传输完成中断功能
 *     DMA_StartTransfer(HW_DMA_CH0);
 * \endcode
 * @param[in]  chl  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @param[in] config 配置模式
 *         			@arg kDMA_IT_Half_Disable 禁止DMA传输一半中断触发
 *         			@arg kDMA_IT_Major_Disable 禁止DMA传输完成中断触发
 *         			@arg kDMA_IT_Half 开启DMA传输一半中断触发
 *         			@arg kDMA_IT_Major 开启DMA传世完成中断触发
 * \param[in] status enable or disable
 * 							\arg 0 disable
 *     					\arg 1 enable
 * @retval None
 */
void DMA_ITConfig(uint8_t chl, DMA_ITConfig_Type config, bool status)
{
    if(status)
    {
        NVIC_EnableIRQ(DMA_IRQnTable[chl]);
    }
    switch(config)
    {
        case kDMA_IT_Half:
            (status)?
            (DMA0->TCD[chl].CSR |= DMA_CSR_INTHALF_MASK):
            (DMA0->TCD[chl].CSR &= ~DMA_CSR_INTHALF_MASK);
            break;
        case kDMA_IT_Major:
            (status)?
            (DMA0->TCD[chl].CSR |= DMA_CSR_INTMAJOR_MASK):
            (DMA0->TCD[chl].CSR &= ~DMA_CSR_INTMAJOR_MASK);
            break; 
        default:
            break;
    }
}

/**
 * @brief  注册中断回调函数
 * @param[in]  chl  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @param[in] AppCBFun 回调函数指针
 * @retval None
 */
void DMA_CallbackInstall(uint8_t chl, DMA_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        DMA_CallBackTable[chl] = AppCBFun;
    }
}

/**
 * @brief  检测DMA传输是否完成
 * @code
 *     //检测DMA的0通道是否完成数据传输
 *     status = IsMajorLoopComplete(HW_DMA_CH0);
 * \endcode
 * @param[in]  chl  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @retval 0 数据传输完成 
 * \retval 1 数据传输未完成
 */

uint8_t DMA_IsMajorLoopComplete(uint8_t chl)
{
    if(DMA0->ERQ & (1 << chl))
    {
        if(DMA0->TCD[chl].CSR & DMA_CSR_DONE_MASK)
        {
            /* clear this bit */
            DMA0->CDNE = DMA_CDNE_CDNE(chl);
            return 0;
        }
        else
        {
            return 1;
        }
    }
    /* this chl is idle, so return 0 and clear DONE bit anyway; */
    DMA0->CDNE = DMA_CDNE_CDNE(chl);
    return 0;
}

/**
 * @brief  设置DMA模块指定通道的目标地址
 * @param[in]  ch  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @param[in] address 32位的目标数据地址
 * @retval None
 */
void DMA_SetDestAddress(uint8_t ch, uint32_t address)
{
    DMA0->TCD[ch].DADDR = address;
}

/**
 * @brief  获取DMA模块指定通道的目标地址
 * @param[in]  ch  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @retval 32位的目标数据地址
 */
uint32_t DMA_GetDestAddress(uint8_t ch)
{
    return DMA0->TCD[ch].DADDR;
}

/**
 * @brief  设置DMA模块指定通道的源地址
 * @param[in]  ch  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @param[in] address 32位的源数据地址
 * @retval None
 */
void DMA_SetSourceAddress(uint8_t ch, uint32_t address)
{
    DMA0->TCD[ch].SADDR = address;
}

/**
 * @brief  获取DMA模块指定通道的源地址
 * @param[in]  ch  DMA通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * @retval 32位的源数据地址
 */
uint32_t DMA_GetSourceAddress(uint8_t ch)
{
    return DMA0->TCD[ch].SADDR;
}

/**
 * @brief  取消DMA模块指定通道的数据传输
 * @retval None
 */
void DMA_CancelTransfer(void)
{
    DMA0->CR |= DMA_CR_CX_MASK;
}

/**
 * @brief  DMA中断，Internal function
 * @retval None
 */
static void DMA_IRQHandler(uint32_t instance)
{
    DMA0->CINT = DMA_CINT_CINT(instance);
    if(DMA_CallBackTable[instance]) DMA_CallBackTable[instance]();
}

/**
 * \brief DMA0中断函数入口，用户无需使用
 */
void DMA0_IRQHandler(void) {DMA_IRQHandler(0);}
/**
 * \brief DMA1中断函数入口，用户无需使用
 */
void DMA1_IRQHandler(void) {DMA_IRQHandler(1);}
/**
 * \brief DMA2中断函数入口，用户无需使用
 */
void DMA2_IRQHandler(void) {DMA_IRQHandler(2);}
/**
 * \brief DMA3中断函数入口，用户无需使用
 */
void DMA3_IRQHandler(void) {DMA_IRQHandler(3);}
/**
 * \brief DMA4中断函数入口，用户无需使用
 */
void DMA4_IRQHandler(void) {DMA_IRQHandler(4);}
/**
 * \brief DMA5中断函数入口，用户无需使用
 */
void DMA5_IRQHandler(void) {DMA_IRQHandler(5);}
/**
 * \brief DMA6中断函数入口，用户无需使用
 */
void DMA6_IRQHandler(void) {DMA_IRQHandler(6);}
/**
 * \brief DMA7中断函数入口，用户无需使用
 */
void DMA7_IRQHandler(void) {DMA_IRQHandler(7);}
/**
 * \brief DMA8中断函数入口，用户无需使用
 */
void DMA8_IRQHandler(void) {DMA_IRQHandler(8);}
/**
 * \brief DMA9中断函数入口，用户无需使用
 */
void DMA9_IRQHandler(void) {DMA_IRQHandler(9);}
/**
 * \brief DMA10中断函数入口，用户无需使用
 */
void DMA10_IRQHandler(void) {DMA_IRQHandler(10);}
/**
 * \brief DMA11中断函数入口，用户无需使用
 */
void DMA11_IRQHandler(void) {DMA_IRQHandler(11);}
/**
 * \brief DMA12中断函数入口，用户无需使用
 */
void DMA12_IRQHandler(void) {DMA_IRQHandler(12);}
/**
 * \brief DMA13中断函数入口，用户无需使用
 */
void DMA13_IRQHandler(void) {DMA_IRQHandler(13);}
/**
 * \brief DMA14中断函数入口，用户无需使用
 */
void DMA14_IRQHandler(void) {DMA_IRQHandler(14);}
/**
 * \brief DMA15中断函数入口，用户无需使用
 */
void DMA15_IRQHandler(void) {DMA_IRQHandler(15);}


