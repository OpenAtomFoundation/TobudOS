/**
  ******************************************************************************
  * @file    dac.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_DAC_H__
#define __CH_LIB_DAC_H__

#include <stdint.h>

/* DAC外设模块号 */
#define HW_DAC0     (0x00)  /* DAC外设模块号0，依次类推 */
#define HW_DAC1     (0x01)  


//!< DAC 回调函数定义
typedef void (*DAC_CallBackType)(void);

/**
 * \enum DAC_TriggerSelect_Type
 * \brief DAC Trigger Style
 */
typedef enum
{
    kDAC_TriggerSoftware,  /**<The DAC software trigger is selected.*/
    kDAC_TriggerHardware,  /**<The DAC hardware trigger is selected.*/
}DAC_TriggerSelect_Type;

/**
 * \enum DAC_BufferMode_Type
 * \brief DAC Buffer Work Mode Select
 */
typedef enum
{
    kDAC_Buffer_Disable,    /**<关闭DAC缓存*/
    kDAC_Buffer_Normal,     /**<开启DAC缓存*/
    kDAC_Buffer_Swing,      /**<dac摇摆模式*/
    kDAC_Buffer_OneTimeScan,/**<DAC一次浏览*/
}DAC_BufferMode_Type;

/**
 * \enum DAC_ReferenceSelect_Type
 * \brief DAC Reference Select
 */
typedef enum
{
    kDAC_Reference_1,       /**< 参考电压源 VREF_OUT */
    kDAC_Reference_2,       /**< 参考电压源 VDDA */
}DAC_ReferenceSelect_Type;

/**
 * \struct DAC_InitTypeDef
 * \brief DAC初始化结构
 */
typedef struct
{
    uint32_t                    instance;      ///<DAC模块号0~1
    DAC_TriggerSelect_Type      triggerMode;   ///<DAC触发模式
    DAC_BufferMode_Type         bufferMode;    ///<缓存类型
    DAC_ReferenceSelect_Type    referenceMode; ///<参考源选择
}DAC_InitTypeDef;

/**
 * \enum DAC_ITDMAConfig_Type
 * \brief DAC中断及DMA配置选择
 */
typedef enum
{
    kDAC_DMA_Disable,                       /**<disable DMA*/
    kDAC_IT_Disable,                        /**<disable interrupt*/
    kDAC_IT_BufferPointer_WaterMark,        /**<when read pointer reach watermask*/
    kDAC_IT_BufferPointer_TopFlag,          /**<when read pointer reach 0 */
    kDAC_IT_BufferPointer_BottomFlag,       /**<when read pointer reach upper limit */
    kDAC_DMA_BufferPointer_WaterMark,       /**<when read pointer reach watermask(DMA)*/
    kDAC_DMA_BufferPointer_TopFlag,         /**<when read pointer reach 0 (DMA)*/
    kDAC_DMA_BufferPointer_BottomFlag,      /**<when read pointer reach upper limit (DMA)*/
}DAC_ITDMAConfig_Type;
 
/**
 * \enum DAC_WaterMarkMode_Type
 * \brief DAC Buffer Watermark Select(1–4 words away from the upper limit)
 */
typedef enum
{
    kDAC_WaterMark_1Word,       /**<1个字*/
    kDAC_WaterMark_2Word,       /**<2个字*/
    kDAC_WaterMark_3Word,       /**<3个字*/
    kDAC_WaterMark_4Word,       /**<4个字*/
}DAC_WaterMarkMode_Type;

 //!< API functions
void DAC_Init(DAC_InitTypeDef* DAC_InitStruct);
void DAC_CallbackInstall(uint8_t instance, DAC_CallBackType AppCBFun);
void DAC_ITDMAConfig(uint32_t instance, DAC_ITDMAConfig_Type config);
uint32_t DAC_GetBufferReadPointer(uint32_t instance);
void DAC_SetBufferReadPointer(uint32_t instance, uint32_t value);
void DAC_SetBufferUpperLimit(uint32_t instance, uint32_t value);
void DAC_ITDMAConfig(uint32_t instance, DAC_ITDMAConfig_Type config);
void DAC_SetBufferValue(uint32_t instance, uint16_t* buf, uint8_t len);
void DAC_SetWaterMark(uint32_t instance, DAC_WaterMarkMode_Type value);
void DAC_SoftwareStartConversion(uint32_t instance);

#endif



