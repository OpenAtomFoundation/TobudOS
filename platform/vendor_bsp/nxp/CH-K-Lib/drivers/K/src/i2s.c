/**
  ******************************************************************************
  * @file    i2s.c
  * @author  YANDLD
  * @version V2.5
  * @date    2015.7.23
  * \date    2015.10.08 FreeXc 完善了对 i2s 模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "i2s.h"
#include "gpio.h"
#include "common.h"


/**
 * /brief internal function
 */
void SAI_HAL_SetMclkDiv(uint32_t instance, uint32_t mclk, uint32_t src_clk)
{
    uint32_t freq = src_clk;
    uint16_t fract, divide;
    uint32_t remaind = 0;
    uint32_t current_remainder = 0xffffffff;
    uint16_t current_fract = 0;
    uint16_t current_divide = 0;
    uint32_t mul_freq = 0;
    uint32_t max_fract = 256;
    /*In order to prevent overflow */
    freq /= 100;
    mclk/= 100;
    max_fract = mclk * 4096/freq + 1;
    if(max_fract > 256)
    {
        max_fract = 256;
    }
    /* Looking for the closet frequency */
    for (fract = 1; fract < max_fract; fract ++)
    {
        mul_freq = freq * fract;
        remaind = mul_freq % mclk;
        divide = mul_freq/mclk;
        /* Find the exactly frequency */
        if (remaind == 0)
        {
            current_fract = fract;
            current_divide = mul_freq/mclk;
            break;
        }
        /* closer to next one */
        if (remaind > mclk/2)
        {
            remaind = mclk - remaind;
            divide += 1;
        }
        /* Update the closest div and fract */
        if (remaind < current_remainder)
        {
            current_fract = fract;
            current_divide = divide;
            current_remainder = remaind;
        }
    }
    /* Clear the FRACT and DIV bit  */
    I2S0->MDR = 0;
    /* Waiting for change updated */
    while(I2S0->MCR & I2S_MCR_DUF_MASK) {};
        
    I2S0->MDR |= I2S_MDR_DIVIDE(current_divide-1) | I2S_MDR_FRACT(current_fract-1);

    /* Waiting for change updated */
    while(I2S0->MCR & I2S_MCR_DUF_MASK) {};
}

/**
 * \brief I2S Transmitter Enable and Bit Clock Enable
 * \param[in] instance I2S instance
 * \param[in] val enable or disable
 *   						\arg 0 disable
 * 							\arg 1  enable
 * \return None
 */
void I2S_SetTxCmd(uint32_t instance, bool val)
{
    if(val)
    {
        I2S0->TCSR |= I2S_TCSR_BCE_MASK;
        I2S0->TCSR |= I2S_TCSR_TE_MASK;
    }
    else
    {
        I2S0->TCSR &= ~I2S_TCSR_BCE_MASK;
        I2S0->TCSR &= ~I2S_TCSR_TE_MASK;
    }
}

/**
 * \brief I2S Transmitter Protocol Config
 * \param[in] instance I2S instance
 * \param[in] protocol 协议，详见i2s.h文件
 * \return None
 */  
void I2S_TxSetProtocol(uint32_t instance, I2S_Protocol_t protocol)
{
    switch (protocol)
    {
        case kSaiBusI2SLeft:
        case kSaiBusI2SRight:
            I2S0->TCR2 |= I2S_TCR2_BCP_MASK;    /* Bit clock polarity */
            I2S0->TCR4 |= I2S_TCR4_MF_MASK;     /* MSB transmitted fisrt */
            I2S0->TCR4 &= ~I2S_TCR4_FSE_MASK;   /* Frame sync not early */
            I2S0->TCR4 &= ~I2S_TCR4_FSP_MASK;   /* Frame sync polarity, left channel is high */
            I2S0->TCR4 |= I2S_TCR4_FRSZ(1);     /* I2S uses 2 word in a frame */
            I2S0->TCR3 &= ~I2S_TCR3_WDFL_MASK;  /* The first word set the start flag */
            break;

        case kSaiBusI2SType:
            I2S0->TCR2 |= I2S_TCR2_BCP_MASK;    /* Bit clock polarity */
            I2S0->TCR4 |= I2S_TCR4_MF_MASK;     /* MSB transmitted fisrt */
            I2S0->TCR4 |= I2S_TCR4_FSE_MASK;    /* Frame sync one bit early */
            I2S0->TCR4 |= I2S_TCR4_FSP_MASK;    /* Frame sync polarity, left channel is low */
            I2S0->TCR4 |= I2S_TCR4_FRSZ(1);     /* I2S uses 2 word in a frame */
            I2S0->TCR3 &= ~I2S_TCR3_WDFL_MASK;  /* The first word set the start flag */
            break;

        case kSaiBusPCMA:
            I2S0->TCR2 &= ~I2S_TCR2_BCP_MASK;    /* Bit clock polarity */
            I2S0->TCR4 |= I2S_TCR4_MF_MASK;     /* MSB transmitted fisrt */
            I2S0->TCR4 |= I2S_TCR4_SYWD(0);     /* Only one bit clock in a frame sync */
            I2S0->TCR4 |= I2S_TCR4_FSE_MASK;    /* Frame sync one bit early */
            I2S0->TCR4 &= ~I2S_TCR4_FSP_MASK;   /* Frame sync polarity, left chennel is high */  
            I2S0->TCR4 |= I2S_TCR4_FRSZ(1);     /* I2S uses 2 word in a frame */
            I2S0->TCR3 &= ~I2S_TCR3_WDFL_MASK;  /* The first word set the start flag */ 
            break;
            
        case kSaiBusPCMB:
            I2S0->TCR2 &= ~I2S_TCR2_BCP_MASK;    /* Bit clock polarity */
            I2S0->TCR4 |= I2S_TCR4_MF_MASK;     /* MSB transmitted fisrt */
            I2S0->TCR4 |= I2S_TCR4_FSE_MASK;    /* Frame sync one bit early */
            I2S0->TCR4 |= I2S_TCR4_SYWD(0);     /* Only one bit clock in a frame sync */
            I2S0->TCR4 &= ~I2S_TCR4_FSP_MASK;   /* Frame sync polarity, left chennel is high */  
            I2S0->TCR4 |= I2S_TCR4_FRSZ(1);     /* I2S uses 2 word in a frame */
            I2S0->TCR3 &= ~I2S_TCR3_WDFL_MASK;  /* The first word set the start flag */ 

            break;
            
        case kSaiBusAC97:

//            I2S_BWR_TCR2_BCP(base,1u); /* Bit clock active high */
//            I2S_BWR_TCR4_MF(base,1u); /* MSB transmitted first */
//            I2S_BWR_TCR4_FSE(base,1u);/* Frame sync one bit early */
//            I2S_BWR_TCR4_FRSZ(base,12u); /* There are 13 words in a frame in AC'97 */
//            I2S_BWR_TCR4_SYWD(base,15u); /* Length of frame sync, 16 bit transmitted in first word */
//            I2S_BWR_TCR5_W0W(base,15u); /* The first word have 16 bits */
//            I2S_BWR_TCR5_WNW(base,19u); /* Other word is 20 bits */
//            I2S_BWR_TCR3_WDFL(base, 0u); /* The first word set the start flag */
            break;
            
        default:
            break;
        }
}  

/**
 * \brief I2S Sample Bit Config
 * \param[in] instance I2S instance
 * \param[in] protocol 协议，详见i2s.h文件
 * \param[in] bits width
 * \return None
 */  
void I2S_SetSampleBit(uint32_t instance, I2S_Protocol_t protocol, uint32_t bits)
{
    if ((protocol == kSaiBusI2SLeft) ||(protocol == kSaiBusI2SRight) ||(protocol == kSaiBusI2SType))
    {
        I2S0->TCR4 &= ~I2S_TCR4_SYWD_MASK;
        I2S0->TCR4 |= I2S_TCR4_SYWD(bits-1);
    }
    I2S0->TCR5 = I2S_TCR5_WNW(bits - 1) | I2S_TCR5_W0W(bits - 1) | I2S_TCR5_FBT(bits - 1);
}

/**
 * \brief I2S Interrupt Mode Config
 * \param[in] instance I2S instance
 * \param[in] mode 中断模式，详见i2s.h文件
 * \param[in] val enable or disable
 *   						\arg 0 disable
 * 							\arg 1  enable
 * \return None
 */  
void I2S_SetIntMode(uint32_t instance, I2S_Int_t mode, bool val)
{   
    if(val)
    {
        NVIC_EnableIRQ(I2S0_Tx_IRQn);
        NVIC_EnableIRQ(I2S0_Rx_IRQn);
    }
    
    switch(mode)
    {
        case kI2S_IntTxFIFOEmpty:
            (val)?(I2S0->TCSR |= I2S_TCSR_FWIE_MASK):(I2S0->TCSR &= ~I2S_TCSR_FWIE_MASK);
            break;
        case kI2S_IntRxFIFOFull:
            (val)?(I2S0->RCSR |= I2S_RCSR_FWIE_MASK):(I2S0->RCSR &= ~I2S_RCSR_FWIE_MASK); 
            break;
    }
}

/**
 * \brief I2S Sync Mode config
 * \param[in] instance I2S instance
 * \param[in] mode 同步模式，详见i2s.h文件
 * \return None
 */  
void I2S_TxSetSyncMode(uint32_t instance, SAI_SyncMode_t mode)
{
    I2S0->TCR2 &= ~I2S_TCR2_SYNC_MASK;
    switch (mode)
    {
        case kSaiModeAsync:
            break;
        case kSaiModeSync:
            I2S0->TCR2 |= I2S_TCR2_SYNC(1);
            I2S0->RCR2 &= ~I2S_TCR2_SYNC_MASK; /* Receiver must be async mode */
            break;
        case kSaiModeSyncWithOtherTx:
            I2S0->TCR2 |= I2S_TCR2_SYNC(2);
            break;
        case kSaiModeSyncWithOtherRx:
            I2S0->TCR2 |= I2S_TCR2_SYNC(3);
            break;
        default:
            break;
    }
}

/**
 * \brief I2S Send Data
 * \param[in] instance I2S instance
 * \param[in] sampleBit 
 * \param[in] chl 通道选择
 * \param[in] buf data pointer
 * \param[in] len data length
 * \return None
 */  
void I2S_SendData(uint32_t instance, uint32_t sampleBit, uint32_t chl, uint8_t *buf, uint32_t len)
{
    uint32_t i, j, data;

    I2S_SetTxCmd(instance, true);
    I2S0->TCR3 |= I2S_TCR3_TCE(1<<(chl));
    
    for(i=0; i<(len/(sampleBit/8)); i++)
    {
        data = 0;
        for(j = 0; j < (sampleBit/8); j ++)
        {
            data |= ((uint32_t)(*buf) << (j * 8u));
            buf++;
        }
        while(!(I2S0->TCSR & I2S_TCSR_FWF_MASK)) {};
        I2S0->TDR[chl] = data;
    }
    I2S0->TCR3 &= ~I2S_TCR3_TCE(1<<(chl));
    I2S_SetTxCmd(instance, false);
}

/**
 * \brief I2S 初始化
 * \param[in] Init 指向I2S初始化结构体的指针 
 * \return None
 */
void I2S_Init(I2S_InitTypeDef *Init)
{
    /* clock gate */
    SIM->SCGC6 |= SIM_SCGC6_I2S_MASK;
    
    /* software reset and FIFO reset */
    I2S0->TCSR |= I2S_TCSR_SR_MASK | I2S_TCSR_FR_MASK;
    
    /* clear all registers */
    I2S0->TCSR = 0;
    I2S0->TCR1 = 0;
    I2S0->TCR2 = 0;
    I2S0->TCR3 = 0;
    I2S0->TCR4 = 0;
    I2S0->TCR5 = 0;
    I2S0->TMR = 0;
    
    /* Tx or Rx */
    if(Init->isMaster)
    {
        I2S0->TCR2 |= I2S_TCR2_BCD_MASK;
        I2S0->TCR4 |= I2S_TCR4_FSD_MASK;
        #ifdef I2S_MCR_MICS_MASK
        I2S0->MCR |= I2S_MCR_MOE_MASK;
        #endif
    }
    else
    {
        I2S0->TCR2 &=  ~I2S_TCR2_BCD_MASK;
        I2S0->TCR4 &=  ~I2S_TCR4_FSD_MASK;
        #ifdef I2S_MCR_MICS_MASK
        I2S0->MCR &= ~I2S_MCR_MOE_MASK;
        #endif
    }
    
    /* stereo or mono */
    if (Init->isStereo)
    {
        I2S0->TMR |= I2S_TMR_TWM(0);
    }
    else
    {
        I2S0->TMR |= I2S_TMR_TWM(2); /* mask the second bit */
    }
    
    /* bit & mclk config: bit clock is fix to mclk, mclk is fixed to 384*sample rate */
    uint32_t bclk, bclk_div, mclk, core_clk;
    bclk = (Init->sampleBit)*(Init->sampleRate)*2;
    mclk = Init->sampleRate*384;
    bclk_div = mclk/bclk;
    I2S0->TCR2 |= I2S_TCR2_MSEL(1) | I2S_TCR2_DIV(bclk_div/2 -1);

#if I2S_MCR_MICS_MASK
    I2S0->MCR |= I2S_MCR_MICS(0); /* mclk src = core clock */
#if I2S_MDR_DIVIDE_MASK
    /* Configure MCLK divider */
    core_clk = GetClock(kCoreClock);
    SAI_HAL_SetMclkDiv(0, mclk, core_clk);
#endif
#endif
    
    /* Tx water mark */
    I2S0->TCR1 |= I2S_TCR1_TFW(4);
    
    /* sample bit and prol */
    I2S_TxSetProtocol(Init->instance, Init->protocol);
    I2S_SetSampleBit(Init->instance, Init->protocol, Init->sampleBit);
    I2S_TxSetSyncMode(Init->instance, kSaiModeAsync);

}
