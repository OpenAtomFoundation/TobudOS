/**
  ******************************************************************************
  * @file    can.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.4.10
  * @date    2015.10.08 FreeXc 完善了 can 模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "can.h"
#include "gpio.h"

#if defined(CAN0)

#define CAN_MB_MAX      (ARRAY_SIZE(CAN0->MB))

#if (!defined(CAN_BASES))
#ifdef CAN0
#define CAN_BASES   {CAN0}
#elif  CAN1
#define CAN_BASES   {CAN0, CAN1}
#endif

#endif

/* global vars */
CAN_Type * const CANBase[] = CAN_BASES;

static const Reg_t ClkTbl[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FLEXCAN0_MASK},
#ifdef CAN1
    {(void*)&(SIM->SCGC3), SIM_SCGC3_FLEXCAN1_MASK},
#endif
};

static const IRQn_Type CAN_IRQnTable[] = 
{
    CAN0_ORed_Message_buffer_IRQn,
#ifdef CAN1
    CAN1_ORed_Message_buffer_IRQn,
#endif
};

/* callback function  */
static CAN_CallBackType CAN_CallBackTable[ARRAY_SIZE(CANBase)] = {NULL};

#define CAN_GET_MB_CODE(cs)         (((cs) & CAN_CS_CODE_MASK)>>CAN_CS_CODE_SHIFT)
#define CAN_GET_FRAME_LEN(cs)       (((cs) & CAN_CS_DLC_MASK)>>CAN_CS_DLC_SHIFT)




/**
 * @brief  Set CAN baudrate，internal function
 * @note   
 * @param[in]  CANx          指向CAN_Type类型的指针
 * @param[in]  baudrate      CAN speed
 * @retval 0             ok
 * \retval other error code
 */
static uint32_t CAN_SetBaudrate(CAN_Type *CANx, uint32_t baudrate)
{
    uint32_t ps ,p1, p2, pd;
    uint32_t sclock = GetClock(kBusClock)/baudrate;
    uint32_t time_seg1, time_seg2, total_tq;

    for(pd=0xFF; pd>0; pd--)
    {
        for(ps=1; ps<8; ps++)
        {
            for(p1=1; p1<8; p1++)
            {
                for(p2=1; p2<8; p2++)
                {
                    time_seg1 = ps+p1+2;
                    time_seg2 = p2+1;
                    total_tq = time_seg1+time_seg2+1;

                    if(ABS((int32_t)(total_tq*(pd+1) - sclock)) < 2)
                    {
                        if((time_seg1 < (time_seg2+8)) && ((time_seg2+2) > time_seg1))
                        {
                            CANx->CTRL1 &= ~(CAN_CTRL1_PROPSEG_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK | CAN_CTRL1_PRESDIV_MASK);
                            CANx->CTRL1 |=  CAN_CTRL1_PROPSEG(ps) 
                                            | CAN_CTRL1_RJW(2)
                                            | CAN_CTRL1_PSEG1(p1) 
                                            | CAN_CTRL1_PSEG2(p2)
                                            | CAN_CTRL1_PRESDIV(pd);
                            //printf("Get baudrate param! pd %d ps %d p1 %d p2 %d\r\n", pd, ps,p1,p2);
                            return 1;
                        }
                    }
                }
            }
        }
    }
	return 0;
}

/**
 * @brief  Set CAN ID，internal function
 * @param[in]  instance CAN通信模块号
 *         			@arg HW_CAN0  0号CAN通信模块
 *         			@arg HW_CAN1  1号CAN通信模块
 * @param[in]  mb      CAN通信接收邮箱0~15
 * @param[in]  id      CAN通信接收ID，11位标准地址或者28位扩展地址
 * @retval 0             
 */
static uint32_t set_id(uint32_t instance, uint32_t mb, uint32_t id)
{
    if(id > 0x7FF)
    {
        CANBase[instance]->MB[mb].ID = (id & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));  /* ID [28-0]*/
        CANBase[instance]->MB[mb].CS |= (CAN_CS_SRR_MASK | CAN_CS_IDE_MASK);  
    }
    else
    {
        CANBase[instance]->MB[mb].ID = CAN_ID_STD(id);  /* ID[28-18] */
        CANBase[instance]->MB[mb].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK); 
    }
    return 0;
}

/**
 * @brief  设置CAN通讯屏蔽掩码
 * @note   内部函数 用于设置邮箱过滤的
 * @param[in]  instance CAN通信模块号
 *         			@arg HW_CAN0  0号CAN通信模块
 *         			@arg HW_CAN1  1号CAN通信模块
 * @param[in]  mb      CAN通信接收邮箱0~15
 * @param[in]  mask    CAN通信接收过滤掩码
 * @retval none
 */
void CAN_SetRxFilterMask(uint32_t instance, uint32_t mb, uint32_t mask)
{
    CANBase[instance]->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (CANBase[instance]->MCR))) {}; 
    if(mask > 0x7FF)
    {	 
        CANBase[instance]->RXIMR[mb] = CAN_ID_EXT(mask); 
    }
    else
    {
        CANBase[instance]->RXIMR[mb] = CAN_ID_STD(mask); 
    }
    
    CANBase[instance]->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (CANBase[instance]->MCR)));
}

/**
 * @brief  设置CAN通讯接收邮箱
 * @note   用户调用函数
 * @param[in]  instance CAN通信模块号
 *         			@arg HW_CAN0  0号CAN通信模块
 *         			@arg HW_CAN1  1号CAN通信模块
 * @param[in]  mb      CAN通信接收邮箱0~15
 * @param[in]  id      CAN通信接收ID，11位标准地址或者28位扩展地址
 * @retval none
 */
void CAN_SetRxMB(uint32_t instance, uint32_t mb, uint32_t id)
{
    set_id(instance, mb, id);
    CANBase[instance]->MB[mb].CS &= ~CAN_CS_CODE_MASK; 
	CANBase[instance]->MB[mb].CS |= CAN_CS_CODE(kFlexCanRX_Empty);
}

/**
 * @brief  CAN通讯初始化配置  （需要配合使用）
 * @note   通信速度是基于bus时钟为50MHz时候的计算
 * @param[in]  Init   CAN通信模块初始化配置结构体(指针)
 * \see CAN_QuickInit() and can.h中相关定义
 * @retval none
 */
void CAN_Init(CAN_InitTypeDef* Init)
{
    uint32_t i;
    CAN_Type *CANx;
    
    /* enable clock gate */
    IP_CLK_ENABLE(Init->instance);
  
    CANx = CANBase[Init->instance];
    
    /* set clock source is bus clock */
    CANx->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

    /* enable module */
    CANx->MCR &= ~CAN_MCR_MDIS_MASK;
    
    /* software reset */
	CANx->MCR |= CAN_MCR_SOFTRST_MASK;	
	while(CAN_MCR_SOFTRST_MASK & (CANx->MCR)) {}; 
        
    /* halt mode */
    CANx->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (CANx->MCR))) {}; 
        
    /* init all mb */
    for(i = 0; i < CAN_MB_MAX; i++)
	{
		CANx->MB[i].CS = 0x00000000;
		CANx->MB[i].ID = 0x00000000;
		CANx->MB[i].WORD0 = 0x00000000;
		CANx->MB[i].WORD1 = 0x00000000;
        CANx->RXIMR[i] = 0x00000000; /* received all frame */
        CANx->IMASK1 = 0x00000000;
        CANx->IFLAG1 = 0xFFFFFFFF;
	}
	/* set all masks */
	//CANx->RXMGMASK = CAN_ID_EXT(CAN_RXMGMASK_MG_MASK); 
   // CANx->RX14MASK = CAN_ID_EXT(CAN_RX14MASK_RX14M_MASK); 
   // CANx->RX15MASK = CAN_ID_EXT(CAN_RX15MASK_RX15M_MASK);
    /* use indviual mask, do not use RXMGMASK, RX14MASK and RX15MASK */
    CANx->MCR |= CAN_MCR_IRMQ_MASK;
    CANx->MCR &= ~CAN_MCR_IDAM_MASK;
    
    /* setting baudrate */
	CAN_SetBaudrate(CANx, Init->baudrate);
    
    /* bypass the frame sended by itself */
    CANx->MCR |= CAN_MCR_SRXDIS_MASK; 
    
    /* enable module */
    CANx->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (CANx->MCR)));
	while(((CANx->MCR)&CAN_MCR_NOTRDY_MASK));
}

/**
 * @brief  CAN通信快速初始化配置
 * @note   通信速度是基于bus时钟为50MHz时候的计算
 * @param[in]  CANxMAP   CAN通信快速配置地图，详见can.h文件
 * @param[in]  baudrate  CAN通信波特率
 * @retval CAN模块号0或1
 */
uint32_t CAN_QuickInit(uint32_t CANxMAP, uint32_t baudrate)
{
	uint32_t i;
    map_t * pq = (map_t*)&(CANxMAP); 
    CAN_InitTypeDef CAN_InitSturct1;
    CAN_InitSturct1.instance = pq->ip;
    CAN_InitSturct1.baudrate = baudrate;
    CAN_Init(&CAN_InitSturct1);
    /* init pinmux */
    for(i = 0; i < pq->pin_cnt; i++)
    {
        PORT_PinMuxConfig(pq->io, pq->pin_start + i, (PORT_PinMux_Type) pq->mux); 
    }
    return pq->ip;
}

/**
 * @brief  CAN通信通道状态检测
 * @note   内部函数
 * @param[in]  instance CAN通信模块号
 *         @arg HW_CAN0  0号CAN通信模块
 *         @arg HW_CAN1  1号CAN通信模块
 * @param[in]  mb      CAN通信接收邮箱0~15
 * \return 
 */
static bool is_mb_idle(uint32_t instance, uint32_t mb)
{
    uint32_t code;
    code = CAN_GET_MB_CODE(CANBase[instance]->MB[mb].CS);
    if((code == kFlexCanTX_Inactive) || (code == kFlexCanRX_Inactive) || (code == kFlexCanRX_Empty))
    {
        return true;
    }
    return false;
}



/**
 * @brief  CAN send data frame
 * @param[in]  instance  can instance
 *         			@arg HW_CAN0
 *         			@arg HW_CAN1
 * @param[in]  mb       Rx MessageBox
 * @param[in]  id       the Rx ID(if use filter)
 * @param[in]  buf      Rx buffer pointer
 * @param[in]  len      Rx frame len
 * @retval 0       ok
 * \retval other  err
 */
uint32_t CAN_WriteData(uint32_t instance, uint32_t mb, uint32_t id, uint8_t* buf, uint8_t len)
{
    uint32_t i;
	uint32_t word[2] = {0};
    CAN_Type *CANx;
    
    CANx = CANBase[instance];
    
    while(is_mb_idle(instance, mb) == false);

    /* setting data */
	for(i = 0; i < len; i++)
	{
        if(i<4)
            word[0] |= (*(buf+i)<<((3-i)*8));
        else
            word[1] |= (*(buf+i)<<((7-i)*8));  
	}
    CANx->MB[mb].WORD0 = word[0];
    CANx->MB[mb].WORD1 = word[1];
    
    /* DLC field */
    CANx->MB[mb].CS &= ~CAN_CS_DLC_MASK;
    CANx->MB[mb].CS |= CAN_CS_DLC(len);
    
    /* clear RTR */
    CANx->MB[mb].CS &= ~CAN_CS_RTR_MASK;
    
    /* ID and IDE */
    set_id(instance, mb, id);

    CANx->MB[mb].CS &= ~CAN_CS_CODE_MASK;
    CANx->MB[mb].CS |= CAN_CS_CODE(kFlexCanTX_Data);
    return 0;
}

/**
 * @brief  CAN send remote frame
 * @note   CAN remote frame has no data section, but has len section
 * @param[in]  instance  can instance
 *         			@arg HW_CAN0
 *         			@arg HW_CAN1
 * @param[in]  mb       Rx MessageBox
 * @param[in]  id       the Rx ID(if use filter)
 * @param[in]  len      Rx frame len
 * @retval 0       ok
 * \retval other  err
 */
uint32_t CAN_WriteRemote(uint32_t instance, uint32_t mb, uint32_t id, uint8_t len)
{
    CAN_Type *CANx;
    
    CANx = CANBase[instance];
    
    uint32_t ret;
    ret = is_mb_idle(instance, mb);
    if(ret)
    {
        return 2;
    }
    /* DLC field, remote frame still has DLC filed */
    CANx->MB[mb].CS &= ~CAN_CS_DLC_MASK;
    CANx->MB[mb].CS |= CAN_CS_DLC(len);
    
    CANx->MB[mb].CS |= CAN_CS_RTR_MASK;
    
    /* ID and IDE */
    set_id(instance, mb, id);
    
    CANx->MB[mb].CS &= ~CAN_CS_CODE_MASK;
    CANx->MB[mb].CS |= CAN_CS_CODE(kFlexCanTX_Remote);
    return 0;
}


/**
 * @brief  注册中断回调函数
 * @param[in]  instance CAN模块中断入口号
 *         @arg HW_CAN0 芯片的CAN模块0中断入口
 *         @arg HW_CAN1 芯片的CAN模块1中断入口
 * @param[in] AppCBFun 回调函数指针入口
 * @retval None
 * @note 对于此函数的具体应用请查阅应用实例
 */
void CAN_CallbackInstall(uint32_t instance, CAN_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        CAN_CallBackTable[instance] = AppCBFun;
    }
}

/**
 * @brief  设置CAN模块的中断类型或者DMA功能
 * @param[in]  instance CAN模块号
 *         			@arg HW_CAN0 芯片的CAN模块0号
 *         			@arg HW_CAN1 芯片的CAN模块1号
 * @param[in]  mb      邮箱编号 0~15
 * @param[in] config   配置模式
 *         			@arg kCAN_IT_Tx_Disable 禁止发送中断
 *         			@arg kCAN_IT_Rx_Disable 禁止接收中断
 *         			@arg kCAN_IT_Tx         发生中断
 *         			@arg kCAN_IT_RX         接收中断
 * @retval None
 */
void CAN_ITDMAConfig(uint32_t instance, uint32_t mb, CAN_ITDMAConfig_Type config)
{
    switch(config)
    {
        case kCAN_IT_Tx_Disable:
            CANBase[instance]->IMASK1 &= ~(1 << mb);
            break;
        case kCAN_IT_Tx:
            CANBase[instance]->IMASK1 |= (1 << mb);
            NVIC_EnableIRQ(CAN_IRQnTable[instance]);
            break;
        case kCAN_IT_Rx_Disable:
            CANBase[instance]->IMASK1 &= ~(1 << mb);
            break;
        case kCAN_IT_RX:
            CANBase[instance]->IMASK1 |= (1 << mb);
            NVIC_EnableIRQ(CAN_IRQnTable[instance]);
            break;
        default:
            break;
    }
}

/**
 * @brief  读取CAN邮箱接收到的数据
 * @param[in]  instance CAN模块号
 *         			@arg HW_CAN0 芯片的CAN模块0号
 *         			@arg HW_CAN1 芯片的CAN模块1号
 * @param[in]  mb      CAN通信接收邮箱0~15
 * \param[in]  id      CAN通信接收ID指针地址
 * @param[in]  buf     CAN通信接收数据指针地址
 * @param[in]  len     CAN通信接收数据长度指针地址
 * @retval 0 正常
 * \retval 1 无数据 
 * \retval 2 正在接收
 */
uint32_t CAN_ReadData(uint32_t instance, uint32_t mb, uint32_t *id, uint8_t *buf, uint8_t *len)
{
	uint32_t code, i;
	uint32_t word[2] = {0};
    code = CAN_GET_MB_CODE(CANBase[instance]->MB[mb].CS);
    if((code & 0x01))
    { 
        return 1; /* MB is busy and controlled by hardware */
    }
    
    if(CANBase[instance]->IFLAG1 & (1<<mb))
    {
        /* clear IT pending bit */
        CANBase[instance]->IFLAG1 = (1 << mb);
        /* read content */
        *len = CAN_GET_FRAME_LEN(CANBase[instance]->MB[mb].CS);
        word[0] = CANBase[instance]->MB[mb].WORD0;
        word[1] = CANBase[instance]->MB[mb].WORD1;
        for(i = 0; i < *len; i++)
        {  
            if(i < 4)
            (*(buf + i))=(word[0]>>((3-i)*8));
            else							
            (*(buf + i))=(word[1]>>((7-i)*8));
        }
        *id = (CANBase[instance]->MB[mb].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK));
        i = CANBase[instance]->TIMER; /* unlock MB */
        return 0;
    }
    i = CANBase[instance]->TIMER; /* unlock MB */
    return 1;
}

/**
 * @brief  读取CAN的FIFO中数据
 * @param[in]  instance CAN模块号
 *         			@arg HW_CAN0 芯片的CAN模块0号
 *         			@arg HW_CAN1 芯片的CAN模块1号
 * \param[in]  id      CAN通信接收ID指针地址
 * @param[in]  buf     CAN通信接收数据指针地址
 * @param[in]  len     CAN通信接收数据长度指针地址
 * @retval 0 正常
 */
uint32_t CAN_ReadFIFO(uint32_t instance, uint32_t *id, uint8_t *buf, uint8_t *len)
{
	uint32_t i;
	uint32_t word[2] = {0};

    /* read content */
    *len = CAN_GET_FRAME_LEN(CANBase[instance]->MB[0].CS);
    word[0] = CANBase[instance]->MB[0].WORD0;
    word[1] = CANBase[instance]->MB[0].WORD1;
    for(i = 0; i < *len; i++)
    {
        if(i < 4)
        (*(buf + i))=(word[0]>>((3-i)*8));
        else							
        (*(buf + i))=(word[1]>>((7-i)*8));
    }
    *id = (CANBase[instance]->MB[0].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK));
    i = CANBase[instance]->TIMER; /* unlock MB */
    CANBase[instance]->IFLAG1 = (1 << CAN_RX_FIFO_MB);
    return 0;
}

/**
 * @brief  CAN的FIFO接收是否使能
 * @param[in]  instance CAN模块号
 *         			@arg HW_CAN0 芯片的CAN模块0号
 *         			@arg HW_CAN1 芯片的CAN模块1号
 * @retval 0 Rx FIFO not enabled
 * @retval 1 Rx FIFO enabled
 */
bool CAN_IsRxFIFOEnable(uint32_t instance)
{
    return (CANBase[instance]->MCR & CAN_MCR_RFEN_MASK);
}

/**
 * @brief  设置CAN的FIFO接收功能
 * @param[in]  instance CAN模块号
 *         			@arg HW_CAN0 芯片的CAN模块0号
 *         			@arg HW_CAN1 芯片的CAN模块1号
 * @retval None
 */
void CAN_SetRxFIFO(uint32_t instance)
{
    CAN_Type *CANx;
    
    CANx = CANBase[instance];
    /* halt mode */
    CANx->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (CANx->MCR))) {}; 
        
    CANx->MCR |= CAN_MCR_RFEN_MASK;
    CANx->CTRL2 &= ~CAN_CTRL2_RFFN_MASK;
    CANx->CTRL2 |= CAN_CTRL2_RFFN(0);
    
    /* enable module */
    CANx->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (CANx->MCR)));
	while(((CANx->MCR)&CAN_MCR_NOTRDY_MASK));
}


/**
 * @brief  CAN中断处理函数
 * @param[in]  instance CAN模块号
 *         			@arg HW_CAN0 芯片的CAN模块0号
 *         			@arg HW_CAN1 芯片的CAN模块1号
 * @note 触发中断之后调用用户注册的中断处理函数
 */
void CAN_IRQHandler(uint32_t instance)
{
    if(CAN_CallBackTable[instance])
    {
        CAN_CallBackTable[instance]();
    }
}

/**
 * @brief  CAN0中断处理函数入口
 * @note 函数内部用于中断事件处理
 */
void CAN0_ORed_Message_buffer_IRQHandler(void) {CAN_IRQHandler(HW_CAN0);}

/**
 * @brief  CAN1中断处理函数入口
 * @note 函数内部用于中断事件处理
 */
void CAN1_ORed_Message_buffer_IRQHandler(void) {CAN_IRQHandler(HW_CAN1);}


#endif
