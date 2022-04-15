/******************************************************************************
* File:             usart.h
*
* Author:           iysheng@163.com  
* Created:          04/14/22 
* Description:      
*****************************************************************************/
#ifndef __USART_H__
#define __USART_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "gd32f3x0_libopt.h"

/**
  * @brief 板级 usart 硬件初始化
  * @param void: 
  * retval N/A.
  */
int board_usart_init(void);

#ifdef __cplusplus
}
#endif
#endif /*__USART_H__ */
