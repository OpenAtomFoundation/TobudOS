/**
  ******************************************************************************
  * @file    spi.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片SPI模块的底层功能函数
  ******************************************************************************
  */

#ifndef __CH_LIB_SPI_H__
#define __CH_LIB_SPI_H__

#include <stdint.h>
#include <stdbool.h>
  
/* SPI模块设备 */
#define HW_SPI0     (0x00)  /* SPI模块设备0,以下依次类推 */
#define HW_SPI1     (0x01)
#define HW_SPI2     (0x02)

/* Clock and Transfer Attributes Register */
#define HW_CTAR0    (0x00)  /* CTAR 0号寄存器 */
#define HW_CTAR1    (0x01)  

/* SPI Chip select */
#define HW_SPI_CS0  (0x00)
#define HW_SPI_CS1  (0x01)
#define HW_SPI_CS2  (0x02)
#define HW_SPI_CS3  (0x03)

/**
 * \enum SPI_FrameFormat_Type
 * \brief SPI帧格式选择
 */
typedef enum
{
    kSPI_CPOL0_CPHA0,
    kSPI_CPOL0_CPHA1,
    kSPI_CPOL1_CPHA0,
    kSPI_CPOL1_CPHA1
}SPI_FrameFormat_Type;

/**
 * \enum SPI_Mode_Type
 * \brief SPI 主从设置
 */
typedef enum
{
    kSPI_Master,        /**< master device */
    kSPI_Slave,         /**< slave device */
} SPI_Mode_Type;

/**
 * \enum SPI_ITDMAConfig_Type
 * \brief SPI interrupt and DMA select
 */
typedef enum
{
    kSPI_IT_TCF,            /**< SPI传输一次完成中断使能 */
    kSPI_DMA_TFFF,          /**< transmit FIFO full */
    kSPI_DMA_RFDF,          /**< receive FIFO drain */
}SPI_ITDMAConfig_Type;

/**
 * \enum SPI_PCS_Type
 * \brief SPI 传输完成后CS信号是否保持选中状态
 */
typedef enum 
{
    kSPI_PCS_ReturnInactive  = 0,   /**< after sending complete, return CS line to 1 */
    kSPI_PCS_KeepAsserted  = 1,     /**< after sending complete, keep CS line to 0 */
}SPI_PCS_Type;

/**
 * \enum SPI_BitOlder_Type
 * \brief SPI LSB or MSB
 */
typedef enum
{
    kSPI_MSB,      /**< 先发送最高位 */
    kSPI_LSB,      /**< 先发送最低位 */
} SPI_BitOlder_Type;

/**
 * \struct SPI_InitTypeDef
 * \brief SPI初始化结构
 */
typedef struct
{
    uint32_t                instance;               ///< 模块号
	SPI_Mode_Type           mode;                   ///< 主从模式
    uint8_t                 dataSize;               ///< 每帧数据有多少位 通常为8或16
    SPI_BitOlder_Type       bitOrder;               ///< 先发高位还是先发地位
    SPI_FrameFormat_Type    frameFormat;            ///< 四种帧格式选择
    uint32_t                baudrate;               ///< 速率
    uint32_t                ctar;                   ///< 帧格式寄存器选择 
}SPI_InitTypeDef;

/* 快速初始化结构 */
#define SPI0_SCK_PC05_SOUT_PC06_SIN_PC07        (0xca90U)
#define SPI0_SCK_PD01_SOUT_PD02_SIN_PD03        (0xc298U)
#define SPI1_SCK_PE02_SOUT_PE01_SIN_PE03        (0xc2a1U)
#define SPI0_SCK_PA15_SOUT_PA16_SIN_PA17        (0xde80U)
#define SPI2_SCK_PB21_SOUT_PB22_SIN_PB23        (0xea8aU)
#define SPI2_SCK_PD12_SOUT_PD13_SIN_PD14        (0xd89aU)
#define SPI0_SCK_PE17_SOUT_PE18_SIN_PE19        (0X0000E2A0U)



/* Callback Type */
typedef void (*SPI_CallBackType)(void);

/* API functions */
void SPI_Init(SPI_InitTypeDef * SPI_InitStruct);
uint16_t SPI_ReadWriteByte(uint32_t instance,uint32_t ctar, uint16_t data, uint16_t CSn, SPI_PCS_Type csState);
void SPI_ITDMAConfig(uint32_t instance, SPI_ITDMAConfig_Type config, bool status);
void SPI_CallbackInstall(uint32_t instance, SPI_CallBackType AppCBFun);
uint32_t SPI_QuickInit(uint32_t MAP, SPI_FrameFormat_Type frameFormat, uint32_t baudrate);
void SPI_CTARConfig(uint32_t instance, uint32_t ctar, SPI_FrameFormat_Type frameFormat, uint8_t dataSize, uint8_t bitOrder, uint32_t baudrate);

/* FIFO functions */
void SPI_EnableTxFIFO(uint32_t instance, bool status);
void SPI_EnableRxFIFO(uint32_t instance, bool status);

#endif

