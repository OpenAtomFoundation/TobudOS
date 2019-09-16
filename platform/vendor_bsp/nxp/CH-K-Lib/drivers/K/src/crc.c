/**
  ******************************************************************************
  * @file    crc.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * \date    2015.10.08 FreeXc 完善了对 crc 模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片IIC模块的底层功能函数
  ******************************************************************************
  */
#include "crc.h"
#include "common.h"

/* 
 * FIXME
 * CRC只能在MK64上验证通过, K60上结果不对
 */
 
/* common CRC protrool define */
static CRC_InitTypeDef CRCProtocolAttrTable[] = 
{
    {16, 0x0000U, 0x8005, kCRCTransposeBits, kCRCTransposeBoth, false},
    {16, 0x0000U, 0x8005, kCRCTransposeBits, kCRCTransposeBoth, true},
    {16, 0xFFFFU, 0x8005, kCRCTransposeBits, kCRCTransposeBoth, true},
    {16, 0xFFFFU, 0x8005, kCRCTransposeBits, kCRCTransposeBoth, false},
    {16, 0x0000U, 0x1021, kCRCTransposeBits, kCRCTransposeBoth, false},         //CCITT
    {16, 0xFFFFU, 0x1021, kCRCNoTranspose, kCRCNoTranspose, false},             //CCITT False
    {16, 0xFFFFU, 0x1021, kCRCTransposeBits, kCRCTransposeBoth, true},          //X25
    {16, 0x0000U, 0x1021, kCRCNoTranspose, kCRCNoTranspose, false},             //XMODEM
    {16, 0x0000U, 0x3D65, kCRCTransposeBits, kCRCTransposeBoth, true},          //DNP
    {32, 0xFFFFFFFFU, 0x04C11DB7, kCRCTransposeBits, kCRCTransposeBoth, true},  //CRC32
    {32, 0xFFFFFFFFU, 0x04C11DB7, kCRCNoTranspose, kCRCNoTranspose, false},     //CRC32-MPEG2
};



/**
 * \brief not use
 */
uint16_t CRC16_GenerateSoftware(const uint8_t *src, uint32_t len)
{
    uint32_t crc = 0;
    uint32_t j;  
    for (j = 0; j < len; ++j)
    {
        uint32_t i;
        uint32_t byte = src[j];
        crc ^= byte << 8;
        for (i = 0; i < 8; ++i)
        {
            uint32_t temp = crc << 1;
            if (crc & 0x8000)
            {
                temp ^= 0x1021;
            }
            crc = temp;
        }
    }
    return crc;
}


/**
 * @brief  初始化CRC硬件模块  
 * @param[in]  CRC_InitStruct  指向CRC初始化结构体的指针
 * @retval None
 */
void CRC_Init(CRC_InitTypeDef * CRC_InitStruct)
{
    /* enable clock */
    SIM->SCGC6 |= SIM_SCGC6_CRC_MASK;
    
    /* crc width */
    (CRC_InitStruct->crcWidth == 16)?
    (CRC0->CTRL &= ~CRC_CTRL_TCRC_MASK):
    (CRC0->CTRL |= CRC_CTRL_TCRC_MASK);
    
    /* 2. set transpose and complement options */
    CRC0->CTRL &= ~CRC_CTRL_TOTR_MASK;
    CRC0->CTRL |= CRC_CTRL_TOTR(CRC_InitStruct->readTranspose);
    
    CRC0->CTRL &= ~CRC_CTRL_TOT_MASK;
    CRC0->CTRL |= CRC_CTRL_TOT(CRC_InitStruct->writeTranspose);
    
    (CRC_InitStruct->complementRead)?
    (CRC0->CTRL |= CRC_CTRL_FXOR_MASK):
    (CRC0->CTRL &= ~CRC_CTRL_FXOR_MASK);
    
    /* 3. Write polynomial */
    CRC0->GPOLY = CRC_InitStruct->polynomial;
    
    /* 4. Set seed value */
    CRC0->CTRL |= CRC_CTRL_WAS_MASK;
#ifdef CRC_DATAL_DATAL_MASK
    CRC0->DATA = CRC_InitStruct->seed;
#else
    CRC0->CRC = CRC_InitStruct->seed;
#endif
    CRC0->CTRL &= ~CRC_CTRL_WAS_MASK;
}

/**
 * @brief  快速初始化CRC硬件模块  
 * @param[in]  type  CRC协议类型，详细请见crc.h文件中的定义
 * @retval None
 */
void CRC_QuickInit(CRC_ProtocolType type)
{
    CRC_Init(&CRCProtocolAttrTable[type]);
}

/**
 * \brief  CRC_HAL_GetCrcResult,Internal function
 * \return Crc result
 */
static uint32_t CRC_HAL_GetCrcResult(void)
{
    uint32_t result = 0;
    CRC_Transpose_Type transpose;
    if(CRC0->CTRL & CRC_CTRL_TCRC_MASK)
    {
        /* 32 */
#ifdef CRC_DATAL_DATAL_MASK
        result = CRC0->DATA;
#else
        result = CRC0->CRC;
#endif
    }
    else
    {
        /* 16 */
        transpose = (CRC_Transpose_Type)((CRC0->CTRL & CRC_CTRL_TOTR_MASK) >> CRC_CTRL_TOTR_SHIFT);    
        if((transpose == kCRCTransposeBoth) || (transpose == kCRCTransposeBytes))
        {
            /* Return upper 16bits of CRC because of transposition in 16bit mode */
#ifdef CRC_DATAL_DATAL_MASK
            result = CRC0->ACCESS16BIT.DATAH;
#else
            result = CRC0->ACCESS16BIT.CRCH;
#endif
        }
        else
        {
#ifdef CRC_DATAL_DATAL_MASK
            result = CRC0->ACCESS16BIT.DATAL;
#else
            result = CRC0->ACCESS16BIT.CRCL;
#endif
        }
    }
    return result;
}

/**
 * @brief  计算并产生CRC运算结果 
 * @param[in]  data  数据指针
 * @param[in]  len   数据长度
 * @return CRC计算结果
 */
uint32_t CRC_Generate(uint8_t* data, uint32_t len)
{
    CRC_Transpose_Type oldInputTranspose;
    uint32_t *data32;
    uint8_t *data8;
    uint32_t result;
    data32 = (uint32_t *)data;

    /* flip bytes because of little endian architecture */
    oldInputTranspose = (CRC_Transpose_Type)((CRC0->CTRL & CRC_CTRL_TOT_MASK) >> CRC_CTRL_TOT_SHIFT);
    
    CRC0->CTRL &= ~CRC_CTRL_TOT_MASK;
    switch (oldInputTranspose)
    {
        case kCRCNoTranspose:
            CRC0->CTRL |= CRC_CTRL_TOT(kCRCTransposeBytes);
             break;
        case kCRCTransposeBits:
            CRC0->CTRL |= CRC_CTRL_TOT(kCRCTransposeBoth);
             break;
        case kCRCTransposeBoth:
            CRC0->CTRL |= CRC_CTRL_TOT(kCRCTransposeBits);
             break;
        case kCRCTransposeBytes:
            CRC0->CTRL |= CRC_CTRL_TOT(kCRCNoTranspose);
             break;
        default:
             break;
    }
    
    /* Start the checksum calculation */
    while (len >= sizeof(uint32_t))
    {
#ifdef CRC_DATAL_DATAL_MASK
        CRC0->DATA = *(data32++);
#else
        CRC0->CRC = *(data32++);
#endif
        len -= sizeof(uint32_t);
    }

    data8 = (uint8_t *)data32;

    switch(len)
    {
        case 3U:
#ifdef CRC_DATAL_DATAL_MASK
            CRC0->ACCESS16BIT.DATAL =  *(uint16_t *)data8;  /* 16 bit */
            CRC0->ACCESS8BIT.DATALL = *(data8+2U);          /* 8 bit */  
#else
            CRC0->ACCESS16BIT.CRCL =  *(uint16_t *)data8;  /* 16 bit */
            CRC0->ACCESS8BIT.CRCLL = *(data8+2U);          /* 8 bit */  
#endif        
            break;
        case 2U:
#ifdef CRC_DATAL_DATAL_MASK   
            CRC0->ACCESS16BIT.DATAL = *(uint16_t *)data8;   /* 16 bit */
        
#else
            CRC0->ACCESS16BIT.CRCL = *(uint16_t *)data8;   /* 16 bit */
#endif
            break;
        case 1U:
#ifdef CRC_DATAL_DATAL_MASK   
            CRC0->ACCESS8BIT.DATALL = *data8;               /* 8 bit */    
#else
            CRC0->ACCESS8BIT.CRCLL = *data8;               /* 8 bit */    
#endif
            break;
        default:
             break;
    }

    result = CRC_HAL_GetCrcResult();
    CRC0->CTRL &= ~CRC_CTRL_TOT_MASK;
    CRC0->CTRL |= CRC_CTRL_TOT(oldInputTranspose);

    return result;
}
