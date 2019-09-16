/**
  ******************************************************************************
  * @file    pdb.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_PDB_H__
#define __CH_LIB_PDB_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "common.h"
#include <stdint.h>
     
/* PDB外设模块号 */
#define HW_PDB_CH0 /* PDB外设模块channel 0，依次类推 */
#define HW_PDB_CH1
     
#define HW_PDB_TRIG_CH0
#define HW_PDB_TRIG_CH1
     
//!< PDB CallBack Type
typedef void (*PDB_CallBackType)(void);

/**
 * \enum  PDB_TriggerSrc_Type
 * \brief PDB 触发源 
 * \see Chip configuration章节 K60P144M100SF2RM Table 3-53
 */
typedef enum
{
    kPDB_TriggerSrc0,       /**< External Trigger */
    kPDB_TriggerSrc1,       /**< CMP 0 */
    kPDB_TriggerSrc2,       /**< CMP 1 */
    kPDB_TriggerSrc3,       /**< CMP 2 */
    kPDB_TriggerSrc4,       /**< PIT Ch 0 Output */
    kPDB_TriggerSrc5,       /**< PIT Ch 1 Output */
    kPDB_TriggerSrc6,       /**< PIT Ch 2 Output */
    kPDB_TriggerSrc7,       /**< PIT Ch 3 Output */
    kPDB_TriggerSrc8,       /**< FTM0 Init and Ext Trigger Outputs */
    kPDB_TriggerSrc9,       /**< FTM1 Init and Ext Trigger Outputs */
    kPDB_TriggerSrc10,      /**< FTM2 Init and Ext Trigger Outputs */
    kPDB_TriggerSrc11,      /**< Reserved */
    kPDB_TriggerSrc12,      /**< RTC Alarm */
    kPDB_TriggerSrc13,      /**< RTC Seconds */
    kPDB_TriggerSrc14,      /**< LPTMR Output */
    kPDB_SoftwareTrigger,   /**< Software Trigger */
}PDB_TriggerSrc_Type;

/**
 * \enum  FTM_PWM_Mode_Type
 * \brief PWM Mode Select
 */
typedef enum
{
	kPWM_EdgeAligned,           /**< 边沿对齐 最常用 */
	kPWM_Combine,               /**< 组合模式 */
    kPWM_Complementary,         /**< 互补模式 类似组合模式 但是Chl(n) 和 Chl(n+1) 是互补输出 */
}FTM_PWM_Mode_Type;
     
/**
 * \struct PDB_InitTypeDef
 * \brief PDB初始化结构
 */
typedef struct
{
    uint32_t                srcClock;           ///< 时钟源频率
    PDB_TriggerSrc_Type     inputTrigSource;    ///< 输入输入触发源
    bool                    isContinuesMode;    ///< 是否是连续触发模式
    uint32_t                timeInUs;           ///< us delay
}PDB_InitTypeDef;

/**
 * \enum PDB_ITDMAConfig_Type
 * \brief PDB中断及DMA配置选择
 */
typedef enum
{
    kPDB_IT_CF,         /**< Interrupt Mode */
    kPDB_DMA_CF,        /**< DMA Mode */
}PDB_ITDMAConfig_Type;


/* API functions */
void PDB_QuickInit(PDB_TriggerSrc_Type triggerSrc, uint32_t timeInUs);
void PDB_Init(PDB_InitTypeDef * PDB_InitStruct);
void PDB_SoftwareTrigger(void);
void PDB_ITDMAConfig(PDB_ITDMAConfig_Type config, bool status);
void PDB_CallbackInstall(PDB_CallBackType AppCBFun);
uint32_t PDB_GetMODValue(void);
/* ADC trigger function */
void PDB_SetADCPreTrigger(uint32_t adcInstance, uint32_t adcMux, uint32_t dlyValue, bool status);
void PDB_SetBackToBackMode(uint32_t adcInstance, uint32_t adcMux, bool status);


#ifdef __cplusplus
}
#endif

#endif

