/**
  ******************************************************************************
  * @file    i2s.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#ifndef __I2S_H__
#define __I2S_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * \enum I2S_Protocol_t
 * \brief I2S 协议
 */
typedef enum 
{
    kSaiBusI2SLeft = 0x0u,  /**< Uses I2S left aligned format. @internal gui name="Left aligned" */
    kSaiBusI2SRight = 0x1u, /**< Uses I2S right aligned format. @internal gui name="Right aligned" */
    kSaiBusI2SType = 0x2u,  /**< Uses I2S format. @internal gui name="I2S format" */
    kSaiBusPCMA = 0x3u,     /**< Uses I2S PCM A format. @internal gui name="PCM A format" */
    kSaiBusPCMB = 0x4u,     /**< Uses I2S PCM B format. @internal gui name="PCM B format" */
    kSaiBusAC97 = 0x5u      /**< Uses I2S AC97 format. @internal gui name="AC97 format" */
}I2S_Protocol_t;

/**
 * \enum SAI_SyncMode_t
 * \brief SAI 同步模式设置
 */
typedef enum 
{
    kSaiModeAsync = 0x0u,           /**< Asynchronous mode @internal gui name="Asynchronous" */
    kSaiModeSync = 0x1u,            /**< Synchronous mode (with receiver or transmit) @internal gui name="Synchronous" */
    kSaiModeSyncWithOtherTx = 0x2u, /**< Synchronous with another SAI transmit @internal gui name="Synchronous with another Tx" */
    kSaiModeSyncWithOtherRx = 0x3u  /**< Synchronous with another SAI receiver @internal gui name="Synchronous with another Rx" */
} SAI_SyncMode_t;

/**
 * \struct I2S_InitTypeDef
 * \brief I2S初始化结构
 */
typedef struct
{
    uint32_t        instance;
    bool            isMaster;
    bool            isStereo;
    uint32_t        sampleRate;
    uint32_t        sampleBit;
    uint32_t        chl;
    I2S_Protocol_t  protocol;
}I2S_InitTypeDef;

/**
 * \enum I2S_Int_t
 * \brief I2S中断标志
 */
typedef enum
{
    kI2S_IntTxFIFOEmpty,
    kI2S_IntRxFIFOFull,
}I2S_Int_t;



/* API functions */
void I2S_Init(I2S_InitTypeDef *Init);
void I2S_TxSetProtocol(uint32_t instance, I2S_Protocol_t protocol);
void I2S_SetSampleBit(uint32_t instance, I2S_Protocol_t protocol, uint32_t bits);
void I2S_SendData(uint32_t instance, uint32_t sampleBit, uint32_t chl, uint8_t *buf, uint32_t len);
void I2S_TxSetSyncMode(uint32_t instance, SAI_SyncMode_t mode);
void I2S_SetIntMode(uint32_t instance, I2S_Int_t mode, bool val);

#endif

