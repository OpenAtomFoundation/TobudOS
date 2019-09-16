/**
  ******************************************************************************
  * @file    pit.h
  * @author  YANDLD
  * @version V2.6
  * @date    2015.06.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_KL_PIT_H__
#define __CH_LIB_KL_PIT_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif


//!< hardware instances
#define HW_PIT_CH0   (0x00U)
#define HW_PIT_CH1   (0x01U)

//!< API functions
void PIT_Init(void);
uint32_t PIT_SetIntMode(uint32_t chl, bool val);
uint32_t PIT_GetCnt(uint32_t chl);
void PIT_SetCnt(uint8_t chl, uint32_t val);
void PIT_SetTime(uint32_t chl, uint32_t us);
     

#ifdef __cplusplus
}
#endif

#endif

