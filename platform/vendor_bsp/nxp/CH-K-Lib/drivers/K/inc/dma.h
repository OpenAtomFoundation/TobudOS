/**
  ******************************************************************************
  * @file    dma.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片DMA模块的底层功能函数
  ******************************************************************************
  */
#ifndef __CH_LIB_DMA_H__
#define __CH_LIB_DMA_H__
  
#include <stdint.h>
#include <stdbool.h>

/* DMA 触发源选择 */
#define DISABLE_DMAREQ          0
#define UART0_REV_DMAREQ        2
#define UART0_TRAN_DMAREQ       3
#define UART1_REV_DMAREQ        4
#define UART1_TRAN_DMAREQ       5
#define UART2_REV_DMAREQ        6
#define UART2_TRAN_DMAREQ       7
#define UART3_REV_DMAREQ        8
#define UART3_TRAN_DMAREQ       9
#define UART4_REV_DMAREQ        10
#define UART4_TRAN_DMAREQ       11
#define UART5_REV_DMAREQ        12
#define UART5_TRAN_DMAREQ       13
#define I2S0_REV_DMAREQ         14
#define I2S0_TRAN_DMAREQ        15
#define SPI0_REV_DMAREQ         16
#define SPI0_TRAN_DMAREQ        17
#define SPI1_REV_DMAREQ         18
#define SPI1_TRAN_DMAREQ        19
#define SPI2_REV_DMAREQ         20
#define SPI2_TRAN_DMAREQ        21
#define I2C0_DMAREQ             22
#define I2C1_DMAREQ             23
#define FTM0_CH0_DMAREQ         24
#define FTM0_CH1_DMAREQ         25
#define FTM0_CH2_DMAREQ         26
#define FTM0_CH3_DMAREQ         27
#define FTM0_CH4_DMAREQ         28
#define FTM0_CH5_DMAREQ         29
#define FTM0_CH6_DMAREQ         30
#define FTM0_CH7_DMAREQ         31
#define FTM1_CH0_DMAREQ         32
#define FTM1_CH1_DMAREQ         33
#define FTM2_CH0_DMAREQ         34
#define FTM2_CH1_DMAREQ         35
#define FTM3_CH0_DMAREQ         36
#define FTM3_CH1_DMAREQ         37
#define FTM3_CH2_DMAREQ         38
#define FTM3_CH3_DMAREQ         39  
#define ADC0_DMAREQ             40
#define ADC1_DMAREQ             41
#define CMP0_DMAREQ             42
#define CMP1_DMAREQ             43
#define CMP2_DMAREQ             44
#define DAC0_DMAREQ             45
#define DAC1_DMAREQ             46
#define CMT_DMAREQ              47
#define PDB_DMAREQ              48
#define PORTA_DMAREQ            49
#define PORTB_DMAREQ            50
#define PORTC_DMAREQ            51
#define PORTD_DMAREQ            52
#define PORTE_DMAREQ            53
#define FTM3_CH4_DMAREQ         54
#define FTM3_CH5_DMAREQ         55
#define FTM3_CH6_DMAREQ         56
#define FTM3_CH7_DMAREQ         57
#define MUX0_DMAREQ             58
#define MUX1_DMAREQ             59
#define MUX2_DMAREQ             60
#define MUX3_DMAREQ             61

/* DMA通道 */
#define HW_DMA_CH0    (0x00)  /* DMA0通道，以下一次类推 */
#define HW_DMA_CH1    (0x01)  
#define HW_DMA_CH2    (0x02)  
#define HW_DMA_CH3    (0x03)  

/**
 * \enum DMA_TriggerSource_Type
 * \brief DMA 触发模式选择
 */
typedef enum
{
    kDMA_TriggerSource_Normal,    /**< 正常模式 */
    kDMA_TriggerSource_Periodic,  /**< 周期触发模式 */
}DMA_TriggerSource_Type;

/**
 * \enum DMA_DataWidthBit_Type
 * \brief DMA搬运数据端口位宽设置
 */
typedef enum
{
    kDMA_DataWidthBit_8,   /**< 8 bit data width */
    kDMA_DataWidthBit_16,  /**< 16 bit data width */
    kDMA_DataWidthBit_32,  /**< 32 bit data width */
}DMA_DataWidthBit_Type; 

/**
 * \enum DMA_Modulo_Type
 * \brief DMA Moduluo 设置
 */
typedef enum 
{
    kDMA_ModuloDisable = 0x0U,
    kDMA_Modulo2bytes = 0x1U,
    kDMA_Modulo4bytes = 0x2U,
    kDMA_Modulo8bytes = 0x3U,
    kDMA_Modulo16bytes = 0x4U,
    kDMA_Modulo32bytes = 0x5U,
    kDMA_Modulo64bytes = 0x6U,
    kDMA_Modulo128bytes = 0x7U,
    kDMA_Modulo256bytes = 0x8U,
    kDMA_Modulo512bytes = 0x9U,
    kDMA_Modulo1Kbytes = 0xaU,
    kDMA_Modulo2Kbytes = 0xbU,
    kDMA_Modulo4Kbytes = 0xcU,
    kDMA_Modulo8Kbytes = 0xdU,
    kDMA_Modulo16Kbytes = 0xeU,
    kDMA_Modulo32Kbytes = 0xfU,
    kDMA_Modulo64Kbytes = 0x10U,
    kDMA_Modulo128Kbytes = 0x11U,
    kDMA_Modulo256Kbytes = 0x12U,
    kDMA_Modulo512Kbytes = 0x13U,
    kDMA_Modulo1Mbytes = 0x14U,
    kDMA_Modulo2Mbytes = 0x15U,
    kDMA_Modulo4Mbytes = 0x16U,
    kDMA_Modulo8Mbytes = 0x17U,
    kDMA_Modulo16Mbytes = 0x18U,
    kDMA_Modulo32Mbytes = 0x19U,
    kDMA_Modulo64Mbytes = 0x1aU,
    kDMA_Modulo128Mbytes = 0x1bU,
    kDMA_Modulo256Mbytes = 0x1cU,
    kDMA_Modulo512Mbytes = 0x1dU,
    kDMA_Modulo1Gbytes = 0x1eU,
    kDMA_Modulo2Gbytes = 0x1fU
} DMA_Modulo_Type;

/**
 * \struct DMA_InitTypeDef
 * \brief DMA 初始化结构
 */
typedef struct 
{
    uint8_t                     chl;                                ///< DMA通道号0~15      
    uint8_t                     chlTriggerSource;                   ///< DMA触发源选择 
    uint16_t                    minorLoopByteCnt;                   ///< MINOR LOOP 中一次传输的字节数 
    uint16_t                    majorLoopCnt;                       ///< MAJOR LOOP 循环次数 
    DMA_TriggerSource_Type      triggerSourceMode;                  ///< 触发模式选择 
    /* 源地址配置 */
    int32_t                     sAddrOffset;                ///< DMA每次读取sAddr之后 sAddr的偏移量 可正可负 
    uint32_t                    sAddr;                      ///< 数据源地址 
    DMA_DataWidthBit_Type       sDataWidth;                 ///< 数据源地址数据宽度 8 16 32 
    int32_t                     sLastAddrAdj;               ///< 所有MAJOR LOOP循环完成后 源地址偏移量 
    DMA_Modulo_Type             sMod;                       ///< Modulo 设置 参见 AN2898 
    /* 目标地址属性配置 */
    int32_t                     dAddrOffset;                
    uint32_t                    dAddr;                      
    DMA_DataWidthBit_Type       dDataWidth;                 
    int32_t                     dLastAddrAdj;               
    DMA_Modulo_Type             dMod;                       
}DMA_InitTypeDef;

/**
 * \enum DMA_ITConfig_Type
 * \brief DMA interrupt select
 */
typedef enum
{
    kDMA_IT_Half,               /**< 传输一半中断开启 */
    kDMA_IT_Major,              /**< 传输完成中断开启 */
}DMA_ITConfig_Type;  

/* Callback Type */
typedef void (*DMA_CallBackType)(void);

/* API functions */
uint32_t DMA_ChlAlloc(void);
void DMA_ChlFree(uint32_t chl);
uint32_t DMA_Init(DMA_InitTypeDef *DMA_InitStruct);
/* get vars and status */
uint32_t DMA_GetDestAddress(uint8_t ch);
uint32_t DMA_GetSourceAddress(uint8_t ch);
uint8_t DMA_IsMajorLoopComplete(uint8_t chl);
uint32_t DMA_GetMajorLoopCount(uint8_t chl);
/* control */
void DMA_SetMajorLoopCounter(uint8_t chl, uint32_t val);
void DMA_CancelTransfer(void);
void DMA_SetDestAddress(uint8_t chl, uint32_t address);
void DMA_SetSourceAddress(uint8_t chl, uint32_t address);
void DMA_EnableRequest(uint8_t chl);
void DMA_DisableRequest(uint8_t chl);
void DMA_EnableAutoDisableRequest(uint8_t chl , bool flag);
/* IT funtions */
void DMA_ITConfig(uint8_t chl, DMA_ITConfig_Type config, bool status);
void DMA_CallbackInstall(uint8_t chl, DMA_CallBackType AppCBFun);
/* chl link */
void DMA_EnableMajorLink(uint8_t chl , uint8_t linkChl, bool flag);


#endif
  
