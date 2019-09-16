/**
  ******************************************************************************
  * @file    pit.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片PIT模块的底层功能函数
  ******************************************************************************
  */
#ifndef __CH_LIB_PIT_H__
#define __CH_LIB_PIT_H__

#include "common.h"

#ifdef __cplusplus
 extern "C" {
#endif


/*PIT模块的通道*/
#define HW_PIT_CH0   (0x00U) /*定义PIT模块的0通道,以下类推*/
#define HW_PIT_CH1   (0x01U) 
#define HW_PIT_CH2   (0x02U) 
#define HW_PIT_CH3   (0x03U) 

//!< PIT CallBack Type
typedef void (*PIT_CallBackType)(void);

/**
 * \struct PIT_InitTypeDef
 * \brief PIT初始化结构
 */
typedef struct
{
    uint8_t   chl;            ///<PIT模块通道选择
    uint32_t  timeInUs;       ///<定时时间单位为us
}PIT_InitTypeDef;

/**
 * \enum PIT_ITDMAConfig_Type
 * \brief PIT 中断DMA配置
 */
typedef enum
{
    kPIT_IT_TOF,            /**<定时器定时中断*/
}PIT_ITDMAConfig_Type;

//!< API functions
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct);
void PIT_QuickInit(uint8_t chl, uint32_t timeInUs);
void PIT_ResetCounter(uint8_t chl);
uint32_t PIT_GetCounterValue(uint8_t chl);
void PIT_CallbackInstall(uint8_t chl, PIT_CallBackType AppCBFun);
void PIT_ITDMAConfig(uint8_t chl, PIT_ITDMAConfig_Type config, bool flag);

#ifdef __cplusplus
}
#endif

#endif
