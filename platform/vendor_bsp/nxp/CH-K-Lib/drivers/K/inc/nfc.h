/**
  ******************************************************************************
  * @file    nfc.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.5.5
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#ifndef __CH_LIB_NFC_H__
#define __CH_LIB_NFC_H__

#include <stdint.h>

/**
 * \enum NFC_Width_Type
 * \brief NFC 数据位宽设置
 */
typedef enum
{
    kNFC_Width8Bit,         /**< 8 bits */
    kNFC_Width16Bit,        /**<16 bits */
}NFC_Width_Type;

/**
 * \struct NFC_InitTypeDef
 * \brief NFC 初始化结构
 */
typedef struct
{
    uint32_t            cs;             ///< chip select
    uint32_t            sectorSize;     ///< how many byte in a page, ex:2048+64 
    NFC_Width_Type      width;          ///< NFC data width

}NFC_InitTypeDef;

/* Control API */
void NFC_Init(NFC_InitTypeDef *NFC_InitStruct);
void NFC_SendResetCmd(void);
void NFC_ReadFlashID(uint8_t nfcBufNum, uint32_t* id0, uint32_t* id1);
uint32_t NFC_GetBufAddr(uint8_t nfcbufNum);

/* RAW API */
void NFC_BlockErase(uint32_t cs, uint32_t row_addr);
void NFC_PageProgram(uint8_t cs, uint8_t nfcbufNum, uint32_t row_addr, uint16_t col_addr);
void NFC_PageRead(uint8_t cs, uint8_t nfcBufNum, uint32_t row_addr, uint16_t col_addr);


#endif
