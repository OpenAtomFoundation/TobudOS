/**
  ******************************************************************************
  * @file    spi.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * \date    2015.10.07 FreeXc 完善了对 spi 模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片SPI模块的底层功能函数
  ******************************************************************************
  */
#include "spi.h"
#include "gpio.h"
#include "common.h"


#if (!defined(SPI_BASES))

#ifdef      SPI1
#define     SPI_BASES {SPI0, SPI1}
#elif       SPI2
#define     SPI_BASES {SPI0, SPI1, SPI2}
#elif       SPI3
#define     SPI_BASES {SPI0, SPI1, SPI2, SPI3}
#else
#define     SPI_BASES {SPI0}
#endif

#endif

SPI_Type * const SPI_InstanceTable[] = SPI_BASES;
static SPI_CallBackType SPI_CallBackTable[ARRAY_SIZE(SPI_InstanceTable)] = {NULL};

static const Reg_t SIM_SPIClockGateTable[] =
{
#ifdef SIM_SCGC6_DSPI0_MASK
    {(void*)&(SIM->SCGC6), SIM_SCGC6_DSPI0_MASK},
#else
    {(void*)&(SIM->SCGC6), SIM_SCGC6_SPI0_MASK},
#endif
    
#ifdef SPI1
#ifdef SIM_SCGC6_DSPI1_MASK
    {(void*)&(SIM->SCGC6), SIM_SCGC6_DSPI1_MASK},
#else
    {(void*)&(SIM->SCGC6), SIM_SCGC6_SPI1_MASK},
#endif
#endif
    
#ifdef SPI2
#ifdef SIM_SCGC3_DSPI2_MASK
    {(void*)&(SIM->SCGC3), SIM_SCGC3_DSPI2_MASK},
#else
    {(void*)&(SIM->SCGC3), SIM_SCGC3_SPI2_MASK},
#endif
#endif
};
static const IRQn_Type SPI_IRQnTable[] = 
{
    SPI0_IRQn,
#ifdef SPI1
    SPI1_IRQn,
#endif
#ifdef SPI2
    SPI2_IRQn,
#endif
};



/* Defines constant value arrays for the baud rate pre-scalar and scalar divider values.*/
static const uint32_t s_baudratePrescaler[] = { 2, 3, 5, 7 };
static const uint32_t s_baudrateScaler[] = { 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048,
                                           4096, 8192, 16384, 32768 };
/**
 * \brief 	dspi_hal_set_baud,Internal function
 * \details Set the DSPI baud rate in bits per second.
 * 					This function will take in the desired bitsPerSec (baud rate) and will calculate the nearest
 * 					possible baud rate without exceeding the desired baud rate, and will return the calculated
 * 					baud rate in bits-per-second. It requires that the caller also provide the frequency of the
 * 					module source clock (in Hz).
 */
static uint32_t dspi_hal_set_baud(uint32_t instance, uint8_t whichCtar, uint32_t bitsPerSec, uint32_t sourceClockInHz)                    
{
    uint32_t prescaler, bestPrescaler;
    uint32_t scaler, bestScaler;
    uint32_t dbr, bestDbr;
    uint32_t realBaudrate, bestBaudrate;
    uint32_t diff, min_diff;
    uint32_t baudrate = bitsPerSec;
    /* for master mode configuration, if slave mode detected, return 0*/
    if(!(SPI_InstanceTable[instance]->MCR & SPI_MCR_MSTR_MASK))
    {
        return 0;
    }
    /* find combination of prescaler and scaler resulting in baudrate closest to the */
    /* requested value */
    min_diff = 0xFFFFFFFFU;
    bestPrescaler = 0;
    bestScaler = 0;
    bestDbr = 1;
    bestBaudrate = 0; /* required to avoid compilation warning */

    /* In all for loops, if min_diff = 0, the exit for loop*/
    for (prescaler = 0; (prescaler < 4) && min_diff; prescaler++)
    {
        for (scaler = 0; (scaler < 16) && min_diff; scaler++)
        {
            for (dbr = 1; (dbr < 3) && min_diff; dbr++)
            {
                realBaudrate = ((sourceClockInHz * dbr) /
                                (s_baudratePrescaler[prescaler] * (s_baudrateScaler[scaler])));

                /* calculate the baud rate difference based on the conditional statement*/
                /* that states that the calculated baud rate must not exceed the desired baud rate*/
                if (baudrate >= realBaudrate)
                {
                    diff = baudrate-realBaudrate;
                    if (min_diff > diff)
                    {
                        /* a better match found */
                        min_diff = diff;
                        bestPrescaler = prescaler;
                        bestScaler = scaler;
                        bestBaudrate = realBaudrate;
                        bestDbr = dbr;
                    }
                }
            }
        }
    }

    uint32_t temp;
    /* write the best dbr, prescalar, and baud rate scalar to the CTAR*/
    temp = SPI_InstanceTable[instance]->CTAR[whichCtar];
    temp &= ~(SPI_CTAR_DBR_MASK| SPI_CTAR_PBR_MASK | SPI_CTAR_BR_MASK);
    if((bestDbr-1))
    {
        temp |= SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(bestPrescaler)|SPI_CTAR_BR(bestScaler);
    }
    else
    {
        temp |= SPI_CTAR_PBR(bestPrescaler)|SPI_CTAR_BR(bestScaler);
    }
    SPI_InstanceTable[instance]->CTAR[whichCtar] = temp;
    /* return the actual calculated baud rate*/
    LIB_TRACE("bestBaudrate:%d\r\n", bestBaudrate);
    return bestBaudrate;
}

/**
 * @brief  初始化SPI模块
 * @note 需要其它函数配合使用，具体可参考SPI_QuickInit内部定义
 * @param[in]  SPI_InitStruct 指向SPI初始化配置结构体的指针
 * @return None
 */
void SPI_Init(SPI_InitTypeDef * SPI_InitStruct)
{
    SPI_Type *SPIx;
    
    SPIx = SPI_InstanceTable[SPI_InitStruct->instance];
    
    /* enable clock gate */
    *(uint32_t*)SIM_SPIClockGateTable[SPI_InitStruct->instance].addr |= SIM_SPIClockGateTable[SPI_InitStruct->instance].mask;
    
    /* let all PCS low when in inactive mode */
    /* stop SPI */
    SPIx->MCR |= SPI_MCR_HALT_MASK;
    
    /* master or slave */
    switch(SPI_InitStruct->mode)
    {
        case kSPI_Master:
            SPIx->MCR |= SPI_MCR_MSTR_MASK;
            break;
        case kSPI_Slave:
            SPIx->MCR &= ~SPI_MCR_MSTR_MASK;
            break;
        default:
            break;
    }
    
    /* enable SPI clock */
    SPIx->MCR &= ~SPI_MCR_MDIS_MASK;
    
    /* disable FIFO and clear FIFO flag */
    SPIx->MCR |= 
        SPI_MCR_PCSIS_MASK |
        SPI_MCR_HALT_MASK  |
        SPI_MCR_CLR_TXF_MASK|
       // SPI_MCR_MTFE_MASK   |
        SPI_MCR_CLR_RXF_MASK|
        SPI_MCR_DIS_TXF_MASK|
        SPI_MCR_DIS_RXF_MASK;
    
    /* config frame format */
    SPI_CTARConfig(SPI_InitStruct->instance, SPI_InitStruct->ctar, SPI_InitStruct->frameFormat, SPI_InitStruct->dataSize, SPI_InitStruct->bitOrder, SPI_InitStruct->baudrate);
    
    /* clear all flags */
    SPIx->SR = 0xFFFFFFFF;
    
    /* launch */
    SPIx->MCR &= ~SPI_MCR_HALT_MASK;
}

 
/**
 * @brief  SPI 波特率及传输控制寄存器配置
 * @param[in]  instance      芯片SPI端口
 *              @arg HW_SPI0 芯片的SPI0端口
 *              @arg HW_SPI1 芯片的SPI1端口
 *              @arg HW_SPI2 芯片的SPI2端口
 * @param[in]  ctar SPI通信通道选择
 *          		@arg HW_CTAR0  0配置寄存器
 *          		@arg HW_CTAR1  1配置寄存器
 * @param[in]  frameFormat SPI通信时的相位和极性的关系
 *         			@arg kSPI_CPOL0_CPHA0
 *         			@arg kSPI_CPOL1_CPHA0
 *         			@arg kSPI_CPOL0_CPHA1
 *         			@arg kSPI_CPOL1_CPHA1
 * \param[in]  dataSize 数据大小
 * \param[in]  bitOrder LSB First
 *  						\arg 0 Data is transferred MSB first
 * 							\arg 1 Data is transferred LSB first
 * @param[in]  baudrate SPI通信速度设置
 * @return None
 */
void SPI_CTARConfig(uint32_t instance, uint32_t ctar, SPI_FrameFormat_Type frameFormat, uint8_t dataSize, uint8_t bitOrder, uint32_t baudrate)
{
    SPI_Type *SPIx;
    uint32_t clock;
    
    SPIx = SPI_InstanceTable[instance];
    
    
    /* data size */
    SPIx->CTAR[ctar] &= ~SPI_CTAR_FMSZ_MASK;
    SPIx->CTAR[ctar] |= SPI_CTAR_FMSZ(dataSize-1);
    
    /* bit order */
    switch(bitOrder)
    {
        case kSPI_MSB:
            SPIx->CTAR[ctar] &= ~SPI_CTAR_LSBFE_MASK;
            break;
        case kSPI_LSB:
            SPIx->CTAR[ctar] |= SPI_CTAR_LSBFE_MASK;
            break;
        default:
            break;
    }
    
    /* frame format */
    switch(frameFormat)
    {
        case kSPI_CPOL0_CPHA0:
            SPIx->CTAR[ctar] &= ~SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[ctar] &= ~SPI_CTAR_CPHA_MASK;
            break;
        case kSPI_CPOL0_CPHA1:
            SPIx->CTAR[ctar] &= ~SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[ctar] |= SPI_CTAR_CPHA_MASK;
            break;   
        case kSPI_CPOL1_CPHA0:
            SPIx->CTAR[ctar] |= SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[ctar] &= ~SPI_CTAR_CPHA_MASK;
            break;  
        case kSPI_CPOL1_CPHA1:
            SPIx->CTAR[ctar] |= SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[ctar] |= SPI_CTAR_CPHA_MASK;  
            break;  
        default:
            break;
    }
    
    /* set SPI clock, SPI use Busclock */
    clock = GetClock(kBusClock);
    dspi_hal_set_baud(instance, ctar, baudrate, clock);
    
    /* add more CS time */
    SPIx->CTAR[ctar] |= SPI_CTAR_ASC(1)|SPI_CTAR_CSSCK(1)|SPI_CTAR_PASC(1)|SPI_CTAR_PCSSCK(1);  
}

/**
 * @brief  快速初始化SPI模块
 * @code
 *     //使用SPI的1模块SCK-PE02 SOUT-PE01 SIN-PE03 通信速度为48000hz 极性和相位都是0 
 *     SPI_QuickInit(SPI1_SCK_PE02_SOUT_PE01_SIN_PE03, kSPI_CPOL0_CPHA0, 48000);
 * @endcode
 * @param[in]  MAP SPI通信快速配置引脚预定义，详见spi.h文件
 * @param[in]  frameFormat SPI通信时的相位和极性的关系
 *         			@arg kSPI_CPOL0_CPHA0
 *         			@arg kSPI_CPOL1_CPHA0
 *         			@arg kSPI_CPOL0_CPHA1
 *         			@arg kSPI_CPOL1_CPHA1
 * @param[in]  baudrate SPI通信速度设置
 * @return SPI模块号
 */
uint32_t SPI_QuickInit(uint32_t MAP, SPI_FrameFormat_Type frameFormat, uint32_t baudrate)
{
    uint32_t i;
    map_t * pq = (map_t*)&(MAP);
    SPI_InitTypeDef SPI_InitStruct1;
    SPI_InitStruct1.baudrate = baudrate;
    SPI_InitStruct1.frameFormat = (SPI_FrameFormat_Type)frameFormat;
    SPI_InitStruct1.dataSize = 8;
    SPI_InitStruct1.instance = pq->ip;
    SPI_InitStruct1.mode = kSPI_Master;
    SPI_InitStruct1.bitOrder = kSPI_MSB;
    SPI_InitStruct1.ctar = HW_CTAR0;
    /* init pinmux */
    for(i = 0; i < pq->pin_cnt; i++)
    {
        PORT_PinMuxConfig(pq->io, pq->pin_start + i, (PORT_PinMux_Type) pq->mux); 
    }
    /* init moudle */
    SPI_Init(&SPI_InitStruct1);
    return pq->ip;
}

/**
 * @brief  使能SPI发送的FIFO功能
 * @param[in]  instance      芯片SPI端口
 *              @arg HW_SPI0 芯片的SPI0端口
 *              @arg HW_SPI1 芯片的SPI1端口
 *              @arg HW_SPI2 芯片的SPI2端口
 * \param[in] status enable or disable Tx FIFO
 *              \arg 0 disable
 *              \arg 1  enable
 * @retval None
 */
void SPI_EnableTxFIFO(uint32_t instance, bool status)
{
    /* enable SPI clock */
    SPI_InstanceTable[instance]->MCR &= ~SPI_MCR_MDIS_MASK;
    
    (status)?
    (SPI_InstanceTable[instance]->MCR &= ~SPI_MCR_DIS_TXF_MASK):
    (SPI_InstanceTable[instance]->MCR |= SPI_MCR_DIS_TXF_MASK);
}

/**
 * @brief  使能SPI接收的FIFO功能
 * @param[in]  instance      芯片SPI端口
 *              @arg HW_SPI0 芯片的SPI0端口
 *              @arg HW_SPI1 芯片的SPI1端口
 *              @arg HW_SPI2 芯片的SPI2端口
 * \param[in] status enable or disable Tx FIFO
 *              \arg 0 disable
 *              \arg 1  enable
 * @retval None
 */
void SPI_EnableRxFIFO(uint32_t instance, bool status)
{
    /* enable SPI clock */
    SPI_InstanceTable[instance]->MCR &= ~SPI_MCR_MDIS_MASK;
    
    (status)?
    (SPI_InstanceTable[instance]->MCR &= ~SPI_MCR_DIS_RXF_MASK):
    (SPI_InstanceTable[instance]->MCR |= SPI_MCR_DIS_RXF_MASK);
}



/**
 * @brief  SPI模块 中断和DMA功能配置
 * @code
 *     //使用SPI的1模块发送完成中断
 *     SPI_ITDMAConfig(HW_SPI1, kSPI_IT_TCF, true);
 * @endcode
 * @param[in]  instance      芯片SPI端口
 *              @arg HW_SPI0 芯片的SPI0端口
 *              @arg HW_SPI1 芯片的SPI1端口
 *              @arg HW_SPI2 芯片的SPI2端口
 * @param[in]  config SPI中断类型
 *         			@arg kSPI_IT_TCF          开启发送完成中断
 *         			@arg kSPI_DMA_TFFF        TxFIFO 空 DMA请求
 *         			@arg kSPI_DMA_RFDF        RxFIFO 空 DMA请求
 * \param[in] status enable or disable IT/DMA
 *              \arg 0 disable
 *              \arg 1  enable
 * @retval None
 */
void SPI_ITDMAConfig(uint32_t instance, SPI_ITDMAConfig_Type config, bool status)
{
    SPI_Type* SPIx;
    
    SPIx = SPI_InstanceTable[instance];
    
    switch(config)
    {
        case kSPI_IT_TCF: 
            (status)?
            (SPIx->RSER |= SPI_RSER_TCF_RE_MASK):
            (SPIx->RSER &= ~SPI_RSER_TCF_RE_MASK);
            NVIC_EnableIRQ(SPI_IRQnTable[instance]);
            break;
        case kSPI_DMA_TFFF:
            (status)?
            (SPIx->RSER |= SPI_RSER_TFFF_RE_MASK):
            (SPIx->RSER &= ~SPI_RSER_TFFF_RE_MASK); 
            (status)?
            (SPIx->RSER |= SPI_RSER_TFFF_DIRS_MASK):
            (SPIx->RSER &= ~SPI_RSER_TFFF_DIRS_MASK); 
            break;
        case kSPI_DMA_RFDF:
            (status)?
            (SPIx->RSER |= SPI_RSER_RFDF_RE_MASK):
            (SPIx->RSER &= ~SPI_RSER_RFDF_RE_MASK); 
            (status)?
            (SPIx->RSER |= SPI_RSER_RFDF_DIRS_MASK):
            (SPIx->RSER &= ~SPI_RSER_RFDF_DIRS_MASK);   
            break;
        default:
            break;
    }
}

/**
 * @brief  注册中断回调函数
 * @param[in]  instance      芯片SPI端口
 *              @arg HW_SPI0 芯片的SPI0端口
 *              @arg HW_SPI1 芯片的SPI1端口
 *              @arg HW_SPI2 芯片的SPI2端口
 * @param[in]  AppCBFun 回调函数指针入口
 * @retval None
 * @note 对于此函数的具体应用请查阅应用实例
 */
void SPI_CallbackInstall(uint32_t instance, SPI_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        SPI_CallBackTable[instance] = AppCBFun;
    }
}

/**
 * @brief  SPI读写一字节数据
 * @code
 *     //使用SPI的1模块的1片选信号写一字节的数据0x55，片选信号最后为选中状态
 *    SPI_ReadWriteByte(HW_SPI1, HW_CTAR0, 0x55, 1, kSPI_PCS_ReturnInactive);
 * @endcode
 * @param[in]  instance      芯片SPI端口
 *              @arg HW_SPI0 芯片的SPI0端口
 *              @arg HW_SPI1 芯片的SPI1端口
 *              @arg HW_SPI2 芯片的SPI2端口
 * @param[in]  ctar SPI通信通道选择
 *          		@arg HW_CTAR0  0配置寄存器
 *          		@arg HW_CTAR1  1配置寄存器
 * @param[in]  data     要发送的一字节数据
 * @param[in]  CSn      片选信号端口选择
 * @param[in]  csState  片选信号最后的状态
 *          		@arg kSPI_PCS_ReturnInactive  最后处于选中状态
 *          		@arg kSPI_PCS_KeepAsserted    最后保持未选中状态
 * @return 读取到的数据
 */
uint16_t SPI_ReadWriteByte(uint32_t instance, uint32_t ctar, uint16_t data, uint16_t CSn, SPI_PCS_Type csState)
{
	SPI_InstanceTable[instance]->PUSHR = (((uint32_t)(((csState))<<SPI_PUSHR_CONT_SHIFT))&SPI_PUSHR_CONT_MASK) 
            | SPI_PUSHR_CTAS(ctar)      
            | SPI_PUSHR_PCS(1<<CSn)
            | SPI_PUSHR_TXDATA(data);
    
    /* waitting for complete */
    if(!(SPI_InstanceTable[instance]->RSER & SPI_RSER_TCF_RE_MASK)) /* if it is polling mode */
    {
        while(!(SPI_InstanceTable[instance]->SR & SPI_SR_TCF_MASK));
        SPI_InstanceTable[instance]->SR |= SPI_SR_TCF_MASK;
    }
    return (uint16_t)SPI_InstanceTable[instance]->POPR;
}


/*
void SPI_WaitSync(uint32_t instance)
{
    uint16_t pointer; 
    while(pointer != 0)
    {
        pointer = (SPI_InstanceTable[instance]->SR & SPI_SR_TXCTR_MASK) >> SPI_SR_TXCTR_SHIFT;
    }
    SPI_InstanceTable[instance]->SR |= SPI_SR_TCF_MASK;
    while(!(SPI_InstanceTable[instance]->SR & SPI_SR_TCF_MASK));
    SPI_InstanceTable[instance]->SR |= SPI_SR_TCF_MASK;
}
*/

/**
 * @brief  系统中断函数，该函数调用用户注册的回调函数，用户无需使用
 */
static void SPI_IRQHandler(uint32_t instance)
{
    SPI_InstanceTable[instance]->SR |= SPI_SR_TCF_MASK ;
    if(SPI_CallBackTable[instance])
    {
        SPI_CallBackTable[instance]();
    }
}

/**
 * @brief  系统SPI0中断函数，用户无需使用
 */
void SPI0_IRQHandler(void)
{
    SPI_IRQHandler(HW_SPI0);
}

/**
 * @brief  系统SPI1中断函数，用户无需使用
 */
void SPI1_IRQHandler(void)
{
    SPI_IRQHandler(HW_SPI1);
}

#if defined(SPI2)
/**
 * @brief  系统SPI2中断函数，用户无需使用
 */
void SPI2_IRQHandler(void)
{
    SPI_IRQHandler(HW_SPI2);
}
#endif


/*
static const map_t SPI_QuickInitTable[] =
{
    { 0, 2, 2, 5, 3, 0}, //SPI0_SCK_PC05_SOUT_PC06_SIN_PC07 2
    { 0, 3, 2, 1, 3, 0}, //SPI0_SCK_PD01_SOUT_PD02_SIN_PD03 2
    { 1, 4, 2, 1, 3, 0}, //SPI1_SCK_PE02_SOUT_PE01_SIN_PE03 2
    { 0, 0, 2,15, 3, 0}, //SPI0_SCK_PA15_SOUT_PA16_SIN_PA17 2
    { 2, 1, 2,21, 3, 0}, //SPI2_SCK_PB21_SOUT_PB22_SIN_PB23 2
    { 2, 3, 2,12, 3, 0}, //SPI2_SCK_PD12_SOUT_PD13_SIN_PD14 2
};
*/
