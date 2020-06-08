/**
 ******************************************************************************
 * @file    utilities_conf.h
 * @author  MCD Application Team
 * @brief   Configuration file to utilities
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UTILITIES_CONF_H
#define UTILITIES_CONF_H

#ifdef __cplusplus
extern "C" {
#endif



/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx.h"

/* definitions to be provided to "sequencer" utility */
#include "stm32_mem.h"
/* definition and callback for tiny_vsnprintf */
#include "stm32_tiny_vsnprintf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/******************************************************************************
 * Common
 ******************************************************************************/
#if defined(__CC_ARM)
#define UTIL_PLACE_IN_SECTION( __x__ )  __attribute__((section (__x__), zero_init))
#elif defined(__ICCARM__)
#define UTIL_PLACE_IN_SECTION( __x__ )  __attribute__((section (__x__)))
#else  /* __GNUC__ */
#define UTIL_PLACE_IN_SECTION( __x__ )  __attribute__((section (__x__)))
#endif

#undef ALIGN
#ifdef WIN32
#define ALIGN(n)
#else
#define ALIGN(n)             __attribute__((aligned(n)))
#endif

/*---------------------------------------------------------------------------*/
/*                             sequencer configuration                       */
/*---------------------------------------------------------------------------*/
#define UTIL_SEQ_RFU 0

#define UTIL_SEQ_INIT_CRITICAL_SECTION( )    UTILS_INIT_CRITICAL_SECTION()
#define UTIL_SEQ_ENTER_CRITICAL_SECTION( )   UTILS_ENTER_CRITICAL_SECTION()
#define UTIL_SEQ_EXIT_CRITICAL_SECTION( )    UTILS_EXIT_CRITICAL_SECTION()
#define UTIL_SEQ_MEMSET8( dest, value, size )   UTIL_MEM_set_8( dest, value, size )


/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define UTILS_INIT_CRITICAL_SECTION()
#define UTILS_ENTER_CRITICAL_SECTION() uint32_t primask_bit= __get_PRIMASK();\
                                       __disable_irq()
#define UTILS_EXIT_CRITICAL_SECTION()  __set_PRIMASK(primask_bit)


/* Exported functions ------------------------------------------------------- */
/******************************************************************************
 * trace\advanced
 * the define option 
 *    UTIL_ADV_TRACE_CONDITIONNAL shall be defined if you want use conditionnal function
 *    UTIL_ADV_TRACE_UNCHUNK_MODE shall be defined if you want use the unchunk mode
 *     
 ******************************************************************************/


#define VLEVEL_OFF 0
#define VLEVEL_L 1  /* just essential traces */
#define VLEVEL_M 2
#define VLEVEL_H 3  /* all traces */

#define TS_OFF 0  /*Log without TimeStamp*/
#define TS_ON 1  /*Log with TimeStamp*/

#define T_REG_OFF  0  /*Log without bitmask*/
                                         
#define UTIL_ADV_TRACE_CONDITIONNAL
#define UTIL_ADV_TRACE_UNCHUNK_MODE
#define UTIL_ADV_TRACE_DEBUG(...)
#define UTIL_ADV_TRACE_INIT_CRITICAL_SECTION( )    UTILS_INIT_CRITICAL_SECTION()
#define UTIL_ADV_TRACE_ENTER_CRITICAL_SECTION( )   UTILS_ENTER_CRITICAL_SECTION()
#define UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION( )    UTILS_EXIT_CRITICAL_SECTION()
#define UTIL_ADV_TRACE_TMP_BUF_SIZE                (256U)
#define UTIL_ADV_TRACE_TMP_MAX_TIMESTMAP_SIZE      (15U)
#define UTIL_ADV_TRACE_FIFO_SIZE                   (512U)
#define UTIL_ADV_TRACE_MEMSET8( dest, value, size) UTIL_MEM_set_8((dest),(value),(size))
#define UTIL_ADV_TRACE_VSNPRINTF(...)                   tiny_vsnprintf_like(__VA_ARGS__)

                                         
#ifdef __cplusplus
}
#endif

#endif /*UTILITIES_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
