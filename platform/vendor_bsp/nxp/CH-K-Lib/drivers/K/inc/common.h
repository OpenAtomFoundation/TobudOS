/**
  ******************************************************************************
  * @file    common.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_COMMON_H__
#define __CH_LIB_COMMON_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef MK10D5
#include "MK10D5.h"
#elif MK10D7
#include "MK10D7.h"
#elif MK20D5
#include "MK20D5.h"
#elif MK20D7
#include "MK20D7.h"
#elif MK10D10
#include "MK10D10.h"
#elif MK20D10
#include "MK20D10.h"
#elif MK40D10
#include "MK40D10.h"
#elif MK60D10
#include "MK60D10.h"
#elif MK60F15
#include "MK60F15.h"
#elif MK60DZ10
#include "MK60DZ10.h"
#elif MK70F12
#include "MK70F12.h"
#elif MK70F15
#include "MK70F15.h"
#elif MK21D5
#include "MK21D5.h"
#elif MK64F12
#include "MK64F12.h"
#elif MK22F12
#include "MK22F12.h"
#elif MK22F25612
#include "MK22F25612.h"
#elif MK22F51212
#include "MK22F51212.h"
#elif MK22F12810
#include "MK22F12810.h"
#else
#error "No CPU defined! please define CPU Type in Preprocessor Symbols, eg: MK60D10"
#endif

/* configuration */
#if !defined(UART_DMA_SUPPORT)
    #define UART_DMA_SUPPORT 0
#endif


/* version information */
#define CHK_VERSION                   2L              /*  major version number */
#define CHK_SUBVERSION                5L              /*  minor version number */
#define CHK_REVISION                  0L              /* revise version number */

/* CH_Kinetis version */
#define FW_VERSION                ((CHK_VERSION * 10000) + \
                                  (CHK_SUBVERSION * 100) + CHK_REVISION)

//参数检测器
#if defined(LIB_DEBUG)

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((char *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(char * file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif
  
#if defined(LIB_DEBUG)
    #include <stdio.h>
    #define LIB_TRACE	printf
#else
    #define LIB_TRACE(...)
#endif

/**
 * \enum  FunctionalState
 * \brief disable or enable
 */
typedef enum 
{
    DISABLE = 0,        /**< 不使能 */
    ENABLE = !DISABLE,  /**< 使能 */
}FunctionalState;

#define NVIC_PriorityGroup_0         ((uint32_t)0x7) /*!< 0 bits for pre-emption priority   4 bits for subpriority */                                               
#define NVIC_PriorityGroup_1         ((uint32_t)0x6) /*!< 1 bits for pre-emption priority   3 bits for subpriority */                                                  
#define NVIC_PriorityGroup_2         ((uint32_t)0x5) /*!< 2 bits for pre-emption priority   2 bits for subpriority */                                                   
#define NVIC_PriorityGroup_3         ((uint32_t)0x4) /*!< 3 bits for pre-emption priority   1 bits for subpriority */                                                   
#define NVIC_PriorityGroup_4         ((uint32_t)0x3) /*!< 4 bits for pre-emption priority   0 bits for subpriority */

#if !defined(MIN)
    #define MIN(a, b)       ((a) < (b) ? (a) : (b))
#endif

#if !defined(MAX)
    #define MAX(a, b)       ((a) > (b) ? (a) : (b))
#endif

#if !defined(ABS)
    #define ABS(a)         (((a) < 0) ? (-(a)) : (a))
#endif

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#ifndef BSWAP_32
    #define BSWAP_32(val)	(uint32_t)((BSWAP_16((uint32_t)(val) & (uint32_t)0xFFFF) << 0x10) |  \
                                   (BSWAP_16((uint32_t)((val) >> 0x10))))
#endif

#ifndef BSWAP_16
    #define BSWAP_16(x)     (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#endif

#ifndef ALIGN
/* Compiler Related Definitions */
#ifdef __CC_ARM                         /* ARM Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #define PRAGMA(x)                   _Pragma(#x)
    #define ALIGN(n)                    PRAGMA(data_alignment=n)
#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#endif /* Compiler Related Definitions */
#endif
    
#define IP_CLK_ENABLE(x)        (*((uint32_t*) ClkTbl[x].addr) |= ClkTbl[x].mask)
#define IP_CLK_DISABLE(x)       (*((uint32_t*) ClkTbl[x].addr) &= ~ClkTbl[x].mask)

#define CLK_EN(t, x)               (*((uint32_t*) t[x].addr) |= t[x].mask)
#define CLK_DIS(t, x)              (*((uint32_t*) t[x].addr) &= ~t[x].mask)
#define REG_GET(t, x)              ((*(uint32_t*) t[x].addr & t[x].mask)>>t[x].shift)

enum
{
    CH_OK,
    CH_ERR,
    CH_TIMEOUT,
    CH_OVERFLOW,
    CH_BUSY,
};

/**
 * \struct map_t
 * \brief QuickInitType
 */
typedef struct
{
    uint32_t ip:3;
    uint32_t io:3;
    uint32_t mux:3;
    uint32_t pin_start:5;
    uint32_t pin_cnt:5;
    uint32_t chl:5;
    uint32_t reserved:8;
}map_t;

/**
 * \enum  Clock_t
 * \brief 时钟源定义
 */
typedef enum
{
    kCoreClock,     /**< core时钟频率 */
    kBusClock,      /**< 总线时钟频率 */
    kFlexBusClock,  /**< Flex总线时钟频率 */
    kFlashClock,    /**< flash时钟频率 */
    kMCGOutClock,   /**< MCG模块输出时钟频率 */
}Clock_t; 

/**
 * \struct Reg_t
 * \brief 寄存器变量
 */
typedef struct 
{
    void *      addr;
    uint32_t    mask;
    uint32_t    shift;
}Reg_t;

/**
 * \brief 系统软复位
 */
static inline void SystemSoftReset(void)
{
    NVIC_SystemReset();
}

#define EnableInterrupts    __enable_irq
#define DisableInterrupts   __disable_irq
void DelayMs(uint32_t ms);
void DelayUs(uint32_t us);
void DelayInit(void);
uint32_t GetClock(Clock_t clockName);
uint32_t QuickInitEncode(map_t * type);
void QuickInitDecode(uint32_t map, map_t* type);
void EnterSTOPMode(bool enSleepOnExit);
uint32_t GetUID(void);

void DWT_DelayInit(void);
void DWT_DelayUs(uint32_t us);
void DWT_DelayMs(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif
