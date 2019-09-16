/**
  ******************************************************************************
  * @file    crc.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
	
#ifndef __CH_LIB_CRC_H__
#define __CH_LIB_CRC_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * \enum CRC_Transpose_Type
 * \brief select the CRC type
 */
typedef enum
{
    kCRCNoTranspose    = 0U, /**< No transposition. @internal gui name="No Transpose" */
    kCRCTransposeBits  = 1U, /**< Bits in bytes are transposed; bytes are not transposed. @internal gui name="Transpose Bits" */
    kCRCTransposeBoth  = 2U, /**< Both bits in bytes and bytes are transposed. @internal gui name="Transpose Bits in Bytes and Bytes" */
    kCRCTransposeBytes = 3U  /**< Only bytes are transposed; no bits in a byte are transposed. @internal gui name="Transpose Bytes" */
}CRC_Transpose_Type;

/**
 * \enum CRC_ProtocolType
 * \brief CRC协议 用于快速初始化
 */
typedef enum
{
    kCRC16_IBM,             /**< */
    kCRC16_MAXIM,           /**< */
    kCRC16_USB,             /**< */
    kCRC16_MODBUS,          /**< */
    kCRC16_CCITT,           /**< */
    kCRC16_CCITTFalse,      /**< */
    kCRC16_X25,             /**< */
    kCRC16_XMODEM,          /**< */
    kCRC16_DNP,             /**< */
    kCRC32,                 /**< */
    kCRC32_MPEG2,           /**< */
    kCRC_ProtocolNameMax,   /**< */
}CRC_ProtocolType;

/**
 * \struct CRC_InitTypeDef
 * \brief CRC初始化结构
 */
typedef struct
{
    uint8_t crcWidth;                   ///< Selects 16 or 32-bit CRC protocol 
    uint32_t seed;                      ///< Value of the seed (initial) CRC value 
    uint32_t polynomial;                ///< Value of the polynomial for the CRC calculation 
    CRC_Transpose_Type writeTranspose;  ///< Defines transpose configuration of the data written to the CRC data register 
    CRC_Transpose_Type readTranspose;   ///< Defines transpose configuration of the value read from the CRC data register 
    bool complementRead;                ///< Enables complement read of CRC data register 
} CRC_InitTypeDef;



/* API functions */
void CRC_QuickInit(CRC_ProtocolType type);
void CRC_Init(CRC_InitTypeDef * CRC_InitStruct);
uint16_t CRC16_GenerateSoftware(const uint8_t *src, uint32_t len);
uint32_t CRC_Generate(uint8_t* data, uint32_t len);

#endif


