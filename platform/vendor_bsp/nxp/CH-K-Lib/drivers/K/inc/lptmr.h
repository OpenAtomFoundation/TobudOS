/**
  ******************************************************************************
  * @file    lptmr.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
	
#ifndef __CH_LIB_LPTMR_H__
#define __CH_LIB_LPTMR_H__

#include <stdint.h>
#include <stdbool.h>


//!LPTMR 回调函数声明
typedef void (*LPTMR_CallBackType)(void);

/**
 * \enum LPTMR_ITDMAConfig_Type
 * \brief LPTMR 中断DMA配置
 */
typedef enum
{
    kLPTMR_IT_TOF,            /**<开启计数器溢出中断 */
}LPTMR_ITDMAConfig_Type;

/**
 * \struct LPTMR_TC_InitTypeDef
 * \brief LPTMR 定时器功能 初始化
 */
typedef struct
{
    uint16_t        timeInMs;  ///<定时时间
}LPTMR_TC_InitTypeDef;

/**
 * \enum LPTMR_PC_IntputSource_Type
 * \brief LPTMR 脉冲计数源选择
 */
typedef enum
{
    kLPTMR_PC_InputSource_CMP0, /**< CMP0 作为脉冲计数时钟源 */
    kLPTMR_PC_InputSource_ALT1, /**< 外部引脚LPTMR_ALT1作为外部计数时钟源 */
    kLPTMR_PC_InputSource_ALT2, /**< 外部引脚LPTMR_ALT2作为外部计数时钟源 */
}LPTMR_PC_IntputSource_Type;

/**
 * \enum LPTMR_PC_PinPolarity_Type
 * \brief LPTMR 外部引脚作为计数时 触发极性选择
 */
typedef enum
{
    kLPTMR_PC_PinPolarity_RigsingEdge,  /**< 上升沿触发计数 */
    kLPTMR_PC_PinPolarity_FallingEdge,  /**< 下降沿触发计数 */
}LPTMR_PC_PinPolarity_Type;

/**
 * \struct LPTMR_PC_InitTypeDef
 * \brief LPTMR 用作外部单路脉冲技术 初始化
 */
typedef struct
{
    uint16_t                        counterOverflowValue;   ///< 脉冲累加器溢出阀值 最大0xFFFF 
    LPTMR_PC_IntputSource_Type      inputSource;            ///< 输入源选择 
    LPTMR_PC_PinPolarity_Type       pinPolarity;            ///< 输入捕捉极性选择 
}LPTMR_PC_InitTypeDef;

//!< 快速初始化结构 用于单路脉冲计数
#define LPTMR_ALT1_PA19   (0x86780U)   /* 使用PTA19引脚作为脉冲计数源 */
#define LPTMR_ALT2_PC05   (0x104b10U)  /* 使用PTC05引脚作为脉冲计数源 */

//!< API functions
void LPTMR_TC_Init(LPTMR_TC_InitTypeDef* LPTMR_TC_InitStruct);
void LPTMR_PC_Init(LPTMR_PC_InitTypeDef* LPTMR_PC_InitStruct);
void LPTMR_ITDMAConfig(LPTMR_ITDMAConfig_Type config, bool status);
void LPTMR_CallbackInstall(LPTMR_CallBackType AppCBFun);
uint32_t LPTMR_PC_ReadCounter(void);
uint32_t LPTMR_PC_QuickInit(uint32_t MAP);
void LPTMR_ClearCounter(void);

#endif

