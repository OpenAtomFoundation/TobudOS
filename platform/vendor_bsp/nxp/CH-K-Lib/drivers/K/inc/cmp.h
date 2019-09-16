/**
  ******************************************************************************
  * @file    cmp.h
  * @author  YANDLD
  * @version V2.5
  * @date    2015.1.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#ifndef __CH_LIB_CMP_H__
#define __CH_LIB_CMP_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h> 

/*!< CMP 回调函数声明 */
typedef void (*CMP_CallBackType)(void);
     
typedef struct
{
    uint32_t                instance;
}CMP_InitTypeDef;


//!< API functions
void CMP_Init(CMP_InitTypeDef* CMP_InitStruct);
void CMP_CallbackInstall(uint32_t instance, CMP_CallBackType AppCBFun);

#ifdef __cplusplus
}
#endif

#endif
